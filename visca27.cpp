// visca27.cpp : Defines the functions for the static library.
//
#include "visca27.h"
// visca27.cpp :
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstdio>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
//#include <errno.h>
#include <chrono>
#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 5678

std::string removeSpaces(std::string input)
{
	input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
	return input;
}
std::vector<unsigned char> hexStringToVector(const std::string& hexIn)
{
	std::vector<unsigned char> bytes;
	std::string hex = removeSpaces(hexIn);

	for (unsigned int i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		unsigned char byte =
			(unsigned char)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(byte);
	}

	return bytes;
}
std::string vectorToHexString(const std::vector<unsigned char>& data)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (const auto& byte : data) {
		ss << std::setw(2) << static_cast<int>(byte);
	}
	return ss.str();
}
bool ACK(std::string hex)
{
	return hex[2] == '4';
}
bool Completed(std::string hex)
{
	return hex[2] == '5';
}
int Error(std::string hex)
{
	if (hex[2] == '6') {
		if (hex[5] == '2')
			return VSYNTAX_ERR;
		if (hex[5] == '3')
			return VBUFFERFULL_ERR;
		if (hex[5] == '4')
			return VCANCEL_ERR;
		if (hex[5] == '5')
			return VNOSOCKET_ERR;
		if (hex[5] == '1')
			return VCANTEXEC_ERR;
		return VERR;
	}
	return VOK;
}
std::string toUpper(const std::string& str)
{
	std::string result = str;
	for (auto& c : result) {
		c = std::toupper(static_cast<unsigned char>(c));
	}
	return result;
}
int SetCamera(UINT_PTR ConnectSocket, std::string hexcmd)
{
	int iResult;
	WSADATA wsaData;
	int result = VERR;

	int recvbuflen = DEFAULT_BUFLEN;
	std::vector<unsigned char> sendbuf = hexStringToVector(hexcmd);

	//const char* sendbuf = (const char[26])"Client: sending data test";
	char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Send an initial buffer
	iResult = send(ConnectSocket, reinterpret_cast<char*>(sendbuf.data()), (int)sendbuf.size(), 0);
	if (iResult == SOCKET_ERROR) {
		return VCONNECT_ERR;
	}

	// Set non-blocking mode
	u_long iMode = 1;
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR) {
		return VCONNECT_ERR;
	}
	bool ACKReceived = false;
	bool CompletedReceived = false;
	bool ErrorReceived = false;

	auto start = std::chrono::steady_clock::now();

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::string hexReceived = vectorToHexString(
				std::vector<unsigned char>(recvbuf,
					recvbuf + iResult));
			if (ACK(hexReceived)) {
				ACKReceived = true;
				start = std::chrono::steady_clock::now();
			}
			else if (ACKReceived && Completed(hexReceived)) {
				CompletedReceived = true;
				result = VOK;
				break;
			}
			else if (Error(hexReceived) != VOK) {
				ErrorReceived = true;
				result = Error(hexReceived);
				break;
			}
		}
	} while (std::chrono::steady_clock::now() - start <
		std::chrono::milliseconds(1000));

	// Did we timeout?
	if (!CompletedReceived) {
		result = VTIMEOUT_ERR;
	}
;
	return result;
}
int OpenSocket(UINT_PTR *ConnectSocket, std::string IP, int port) {
	int iResult;
	WSADATA wsaData;
	int result = VERR;
	*ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService = {};

	char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		return VCONNECT_ERR;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	*ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		return VCONNECT_ERR;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(IP.c_str());
	clientService.sin_port = htons(port);

	//----------------------
	// Connect to server.
	iResult = connect(*ConnectSocket, (SOCKADDR*)&clientService,
		sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		*ConnectSocket = INVALID_SOCKET;
		closesocket(*ConnectSocket);
		WSACleanup();
		return VCONNECT_ERR;
	}

	// Set non-blocking mode
	u_long iMode = 1;
	iResult = ioctlsocket(*ConnectSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR) {
		closesocket(*ConnectSocket);
		*ConnectSocket = INVALID_SOCKET;
		WSACleanup();
		return VCONNECT_ERR;
	}
	return VOK;
}

int CloseSocket(UINT_PTR ConnectSocket) {
	int iResult;
	// close the socket
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		WSACleanup();
		return VCLOSE_ERR;
	}
	WSACleanup();
	return VOK;
}

int GetCamera(UINT_PTR ConnectSocket, std::string hexcmd, std::string *returnhex)
{
	int iResult;
	WSADATA wsaData;
	int result = VOK;
	*returnhex = "";

	int recvbuflen = DEFAULT_BUFLEN;
	std::vector<unsigned char> sendbuf = hexStringToVector(hexcmd);
	char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Send an initial buffer
	iResult = send(ConnectSocket, reinterpret_cast<char*>(sendbuf.data()),
		(int)sendbuf.size(), 0);
	if (iResult == SOCKET_ERROR) {
		return VCONNECT_ERR;
	}

	bool ErrorReceived = false;

	auto start = std::chrono::steady_clock::now();

	// Receive until the peer closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::string hexReceived = vectorToHexString(
				std::vector<unsigned char>(recvbuf,
					recvbuf + iResult));
			if (Error(hexReceived) != VOK) {
				ErrorReceived = true;
				result = Error(hexReceived);
				break;
			}
			*returnhex = toUpper(hexReceived); // Happy path
		}
		else {
			int wle = WSAGetLastError();
		}
	} while (std::chrono::steady_clock::now() - start <
		std::chrono::milliseconds(1000));

	// Did we timeout?
	if ((*returnhex == "") && (!ErrorReceived)){
		result = VTIMEOUT_ERR;
	}

	return result;
}

bool isHex(char c )
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F');
}

ValueConverter::ValueConverter(std::string format, char field1, char field2)
{
	fmt = removeSpaces(format);
	startIndex1 = -1;
	startIndex2 = -1;
	nDigits1 = 0;
	nDigits2 = 0;
	skip1 = 0;
	skip2 = 0;
	int lastIndex1 = -1;
	int lastIndex2 = -1;
	for (int i = 0; i < fmt.length(); i++) {
		if (!isHex(fmt[i])) {
			if ((fmt[i] == field1) || (field1 == ' ')) {
				if (startIndex1 == -1) {
					startIndex1 = i;
				}
				else if (lastIndex1 > -1) {
					skip1 = i - lastIndex1;
				}
				nDigits1++;

				lastIndex1 = i;
			}
			if (fmt[i] == field2) {
				if (startIndex2 == -1) {
					startIndex2 = i;
				}
				else if (lastIndex2 > -1) {
					skip2 = i - lastIndex2;
				}
				nDigits2++;

				lastIndex2 = i;
			}
		} 
	}
}
ValueConverter::~ValueConverter() {};

int ValueConverter::getValue1(std::string returnString)
{
	if ((nDigits1 == 0) ||
		(nDigits1 >= 30) ||
		(returnString.size() < startIndex1 + ((nDigits1 - 1) * skip1)))
		return -1;

	char valueHex[30] = "";
	for (int d = 0; d < nDigits1; d++) {
		valueHex[d] = returnString[startIndex1 + (d * skip1)];
	}
	return std::stoi(std::string(valueHex, nDigits1), nullptr, 16);
}
int ValueConverter::getValue2(std::string returnString)
{
	if ((nDigits2 == 0) ||
		(nDigits2 >= 30) ||
		(returnString.size() < startIndex2 + ((nDigits2 - 1) * skip2)))
		return -1;

	char valueHex[30] = "";
	for (int d = 0; d < nDigits2; d++) {
		valueHex[d] = returnString[startIndex2 + (d * skip2)];
	}
	return std::stoi(std::string(valueHex, nDigits2), nullptr, 16);
}
std::string ValueConverter::setValue(int val1, int val2)
{

	if (nDigits1 == 0)
		return fmt;

	char setCommand[30];
	std::stringstream ss1;
	ss1 << std::hex << val1;
	std::string valueHex = ss1.str();

	for (int i = 0; i < fmt.length(); i++) {
		setCommand[i] = fmt[i];
	}

	for (int d = 0; d < nDigits1; d++) {
		char fillChar = '0';
		if (d >= nDigits1 - valueHex.length())
			fillChar = valueHex[valueHex.length() - nDigits1 + d];
		setCommand[startIndex1 + (d * skip1)] = fillChar;
	}

	if ((val2 != -1) && (nDigits2 > 0)) {
		std::stringstream ss2;
		ss2 << std::hex << val2;
		valueHex = ss2.str();
		for (int d = 0; d < nDigits2; d++) {
			char fillChar = '0';
			if (d >= nDigits2 - valueHex.length())
				fillChar = valueHex[valueHex.length() - nDigits2 + d];
			setCommand[startIndex2 + (d * skip2)] = fillChar;
		}
	}
	return toUpper(std::string(setCommand, fmt.length()));
}
