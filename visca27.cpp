// visca27.cpp : Defines the functions for the static library.
//
#include "visca27.h"
#include "ViscaAPI.h"
#include <iostream>
#include <stdio.h>
#include <errno.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 5678

std::string removeSpaces(std::string input)
{
	input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
	return input;
}
std::vector<unsigned char> hexStringToVector(const std::string &hexIn)
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
std::vector<unsigned char> encapsulateCommand(int32_t sequence,
					      std::string &hexIn)
{
	std::vector<unsigned char> encapsulatedBytes;
	std::vector<unsigned char> payload = hexStringToVector(hexIn);
	encapsulatedBytes.reserve(4 + payload.size() +
				  4); // Reserve space for encapsulated bytes

	encapsulatedBytes.push_back(0x01); // Start byte
	encapsulatedBytes.push_back(0x00); // Command type
	encapsulatedBytes.push_back(0x00); // empty size byte
	encapsulatedBytes.push_back((unsigned char)payload.size()); // size byte

	// Insert the 32-bit sequence number (big-endian order)
	encapsulatedBytes.push_back((sequence >> 24) &
				    0xFF); // Most significant byte
	encapsulatedBytes.push_back((sequence >> 16) & 0xFF);
	encapsulatedBytes.push_back((sequence >> 8) & 0xFF);
	encapsulatedBytes.push_back(sequence & 0xFF); // Least significant byte

	encapsulatedBytes.insert(encapsulatedBytes.end(), payload.begin(),
				 payload.end());
	return encapsulatedBytes;
}
std::vector<unsigned char> encapsulateInquiry(int32_t sequence,
					      const std::string &hexIn)
{
	std::vector<unsigned char> encapsulatedBytes;
	std::vector<unsigned char> payload = hexStringToVector(hexIn);
	encapsulatedBytes.reserve(4 + payload.size() +
				  4); // Reserve space for encapsulated bytes
	encapsulatedBytes.push_back(0x01); // Start byte
	encapsulatedBytes.push_back(0x10); // Inquiry type
	encapsulatedBytes.push_back(0x00); // empty size byte
	encapsulatedBytes.push_back((unsigned char)payload.size()); // size byte

	// Insert the 32-bit sequence number (big-endian order)
	encapsulatedBytes.push_back((sequence >> 24) &
				    0xFF); // Most significant byte
	encapsulatedBytes.push_back((sequence >> 16) & 0xFF);
	encapsulatedBytes.push_back((sequence >> 8) & 0xFF);
	encapsulatedBytes.push_back(sequence & 0xFF); // Least significant byte

	encapsulatedBytes.insert(encapsulatedBytes.end(), payload.begin(),
				 payload.end());
	return encapsulatedBytes;
}

std::string vectorToHexString(const std::vector<unsigned char> &data)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (const auto &byte : data) {
		ss << std::setw(2) << static_cast<int>(byte);
	}
	return ss.str();
}
std::string encapsulatedReplyToHexString(const std::vector<unsigned char> &data)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (size_t i = 8; i < data.size();
	     ++i) { // Start from the 8th character (index 7)
		ss << std::setw(2) << static_cast<int>(data[i]);
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
std::string toUpper(const std::string &str)
{
	std::string result = str;
	for (auto &c : result) {
		c = (char)std::toupper(static_cast<unsigned char>(c));
	}
	return result;
}
int SetCamera(SOCKET ConnectSocket, std::string hexcmd,
	int32_t &sequence, bool encapsulated)
{
	int iResult;

	int result = VERR;

	int recvbuflen = DEFAULT_BUFLEN;
	std::vector<unsigned char> sendbuf =
		encapsulated ? encapsulateCommand(sequence++, hexcmd)
			     : hexStringToVector(hexcmd);
	std::cout << "SetCamera sendbuf: " << vectorToHexString(sendbuf)
		  << std::endl;

	//const char* sendbuf = (const char[26])"Client: sending data test";
	char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Send an initial buffer
#if defined(_WIN32) || defined(_WIN64)
	iResult = (int)send(ConnectSocket,
			    reinterpret_cast<char *>(sendbuf.data()),
			    (int)sendbuf.size(), 0);
#else
	iResult = (int)send(ConnectSocket,
			    reinterpret_cast<char *>(sendbuf.data()),
			    sendbuf.size(), 0);
#endif
	if (iResult == SOCKET_ERROR) {
		return VCONNECT_ERR;
	}

#if defined(_WIN32) || defined(_WIN64)
	// Set non-blocking mode
	u_long iMode = 1;
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR) {
		return VCONNECT_ERR;
	}
#endif
	bool ACKReceived = false;
	bool CompletedReceived = false;
	bool ErrorReceived = false;

	auto start = std::chrono::steady_clock::now();

	// Receive until the peer closes the connection
	do {
#if defined(_WIN32) || defined(_WIN64)
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
#else
		iResult = (int)recv(ConnectSocket, recvbuf, (size_t)recvbuflen,
				    MSG_DONTWAIT);
#endif
		if (iResult > 0) {
			std::string hexReceived =
				encapsulated
					? encapsulatedReplyToHexString(
						  std::vector<unsigned char>(
							  recvbuf,
							  recvbuf + iResult))
					: vectorToHexString(
						  std::vector<unsigned char>(
							  recvbuf,
							  recvbuf + iResult));
			std::cout << "SetCamera hexReceived: " << hexReceived
				  << std::endl;
			if (ACK(hexReceived)) {
				ACKReceived = true;
				start = std::chrono::steady_clock::now();
			} else if (ACKReceived && Completed(hexReceived)) {
				CompletedReceived = true;
				result = VOK;
				break;
			} else if (Error(hexReceived) != VOK) {
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
	return result;
}
int OpenSocket(SOCKET *ConnectSocket, std::string IP, int port,
	       int32_t &sequence, bool udp)
{
	int iResult;
	*ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
	sequence = 0;

	// Check if the IP address is active using ping
	/*
    std::string pingCommand;
#if (defined(_WIN32) || defined(_WIN64))
    pingCommand = "ping -n 1 " + IP + " > NUL 2>&1";
#else
    pingCommand = "ping -c 1 " + IP + " > /dev/null 2>&1";
#endif

    int pingResult = system(pingCommand.c_str());
    if (pingResult != 0) {
        //std::cerr << "Ping to " << IP << " failed." << std::endl;
        return VIP_ERR;
    }
	*/
#if (defined(_WIN32) || defined(_WIN64))
	// Initialize Winsock
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return VCONNECT_ERR;
	}
#endif

	//----------------------
	// Create a SOCKET for connecting to server
	*ConnectSocket = socket(AF_INET, udp ? SOCK_DGRAM : SOCK_STREAM,
				udp ? IPPROTO_UDP : IPPROTO_TCP);
	if (*ConnectSocket == INVALID_SOCKET) {
		// std::cerr << "socket create error " << std::endl;
		return VSOCKET_ERR;
	}

#if (defined(_WIN32) || defined(_WIN64))
	// Set non-blocking mode
	u_long iMode = 1;
	iResult = ioctlsocket(*ConnectSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR) {
		closesocket(*ConnectSocket);
		*ConnectSocket = INVALID_SOCKET;
		//WSACleanup();
		return VCONNECT_ERR;
	}
#else
	// Set non-blocking mode
	int flags = fcntl(*ConnectSocket, F_GETFL, 0);
	fcntl(*ConnectSocket, F_SETFL, flags | O_NONBLOCK);
#endif

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(IP.c_str());
	clientService.sin_port = htons((u_short)port);

	//----------------------
	// Connect to server.
	iResult = connect(*ConnectSocket, (SOCKADDR *)&clientService,
			  sizeof(clientService));
	if (iResult == 0) {
		return VOK;
#if defined(_WIN32) || defined(_WIN64)
	} else if ((iResult < 0) && (WSAGetLastError() == WSAEWOULDBLOCK)) {
#else
	} else if ((iResult < 0) && (errno == EINPROGRESS)) {
#endif
		// std::cerr << "waiting for connection " << std::endl;
		// Wait for the connection to complete
		auto start = std::chrono::steady_clock::now();
		fd_set writefds;
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000; // 100 milliseconds

		do {
			FD_ZERO(&writefds);
			FD_SET(*ConnectSocket, &writefds);

			iResult = select((int)*ConnectSocket + 1, NULL,
					 &writefds, NULL, &tv);
			if (iResult > 0) {
				if (FD_ISSET(*ConnectSocket, &writefds)) {
					int so_error;
					socklen_t len = sizeof(so_error);
#if defined(_WIN32) || defined(_WIN64)
					getsockopt(*ConnectSocket, SOL_SOCKET,
						   SO_ERROR, (char *)&so_error,
						   &len);
#else
					getsockopt(*ConnectSocket, SOL_SOCKET,
						   SO_ERROR, &so_error, &len);
#endif
					if (so_error == 0) {
						return VOK;
					} else {
						errno = so_error;
						//std::cerr << "socketopt error " << std::endl;
						return VCONNECT_ERR;
					}
				}
			} else if (iResult < 0) {
				// select error
				//std::cerr << "socket select error " << std::endl;
				return VSELECT_ERR;
			}
		} while (std::chrono::steady_clock::now() - start <
			 std::chrono::milliseconds(500));

		// Timeout occurred
		// std::cerr << "socket timeout error " << std::endl;
		return VPORT_ERR; // If we timed out, we have a port error as the IP was checked
	}

#if defined(_WIN32) || defined(_WIN64)
	closesocket(*ConnectSocket);
#else
	close(*ConnectSocket);
#endif
	*ConnectSocket = INVALID_SOCKET;
	return VCONNECT_ERR;
}

int CloseSocket(SOCKET ConnectSocket)
{
	int iResult;
// close the socket
#if defined(_WIN32) || defined(_WIN64)
	iResult = closesocket(ConnectSocket);
#else
	iResult = close(ConnectSocket);
#endif
	if (iResult == SOCKET_ERROR) {
		//WSACleanup();
		return VCLOSE_ERR;
	}
	//WSACleanup();
	return VOK;
}

int GetCamera(SOCKET ConnectSocket, std::string hexcmd, std::string *returnhex, int32_t &sequence,
	bool encapsulated)
{
	int iResult;

	int result = VOK;
	*returnhex = "";

	int recvbuflen = DEFAULT_BUFLEN;
	std::vector<unsigned char> sendbuf =
		encapsulated ? encapsulateInquiry(sequence++, hexcmd)
			     : hexStringToVector(hexcmd);
	char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Send an initial buffer
#if defined(_WIN32) || defined(_WIN64)
	iResult = (int)send(ConnectSocket,
			    reinterpret_cast<char *>(sendbuf.data()),
			    (int)sendbuf.size(), 0);
#else
	iResult = (int)send(ConnectSocket,
			    reinterpret_cast<char *>(sendbuf.data()),
			    sendbuf.size(), 0);
#endif
	if (iResult == SOCKET_ERROR) {
		return VCONNECT_ERR;
	}

	bool ErrorReceived = false;

	auto start = std::chrono::steady_clock::now();

	// Receive until the peer closes the connection
	do {
#if defined(_WIN32) || defined(_WIN64)
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
#else
		iResult = (int)recv(ConnectSocket, recvbuf, (size_t)recvbuflen,
				    MSG_DONTWAIT);
#endif
		if (iResult > 0) {
			std::string hexReceived =
				encapsulated
					? encapsulatedReplyToHexString(
						  std::vector<unsigned char>(
							  recvbuf,
							  recvbuf + iResult))
					: vectorToHexString(
						  std::vector<unsigned char>(
							  recvbuf,
							  recvbuf + iResult));
			std::cout << "GetCamera hexReceived: " << hexReceived
				  << std::endl;

			if (Error(hexReceived) != VOK) {
				ErrorReceived = true;
				result = Error(hexReceived);
				break;
			}
			*returnhex = toUpper(hexReceived); // Happy path
			result = VOK;
			break;
		} else {
			// int wle = WSAGetLastError();
		}
	} while (std::chrono::steady_clock::now() - start <
		 std::chrono::milliseconds(1000));

	// Did we timeout?
	if ((*returnhex == "") && (!ErrorReceived)) {
		result = VTIMEOUT_ERR;
	}

	return result;
}

bool isHex(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
	       (c >= 'A' && c <= 'F');
}
ValueField::ValueField(char field, std::string fmt)
{
	startIndex = 0;
	nDigits = 0;
	skip = 0;
	size_t lastIndex = 0;
	for (size_t i = 0; i < fmt.length(); i++) {
		if (!isHex(fmt[i])) {
			if ((fmt[i] == field) || (field == ' ')) {
				if (startIndex == 0) {
					startIndex = i;
				} else if (lastIndex > 0) {
					skip = i - lastIndex;
				}
				nDigits++;

				lastIndex = i;
			}
		}
	}
};

ValueConverter::ValueConverter(std::string format, char f0, char f1, char f2)
{
	fmt = removeSpaces(format);
	command = fmt;
	nFields = 0;
	addField(f0);
	addField(f1);
	addField(f2);
}
ValueConverter::~ValueConverter() {};

void ValueConverter::addField(char f)
{
	if (f == ' ')
		return;
	ValueField field(f, fmt);
	fields[f] = field;
	nFields++;
}
int ValueConverter::getValue(char f, std::string replyString)
{
	auto it = fields.find(f);
	if (it == fields.end())
		return 0;
	ValueField field = it->second;
	if ((field.nDigits == 0) || (field.nDigits >= 30) ||
	    (replyString.size() <
	     field.startIndex + ((field.nDigits - 1) * field.skip)))
		return -1;
	char valueHex[30] = "";
	for (size_t d = 0; d < field.nDigits; d++) {
		valueHex[d] = replyString[field.startIndex + (d * field.skip)];
	}
	return static_cast<int>(
		std::stoi(std::string(valueHex, field.nDigits), nullptr, 16));
}
void ValueConverter::init()
{
	command = fmt;
}
void ValueConverter::setValue(char f, int val)
{
	auto it = fields.find(f);
	if (it == fields.end())
		return;
	ValueField field = it->second;
	std::stringstream ss1;
	ss1 << std::hex << (short)val;
	std::string valueHex = ss1.str();

	for (size_t d = 0; d < field.nDigits; d++) {
		char fillChar = '0';
		if (d >= field.nDigits - valueHex.length())
			fillChar =
				valueHex[valueHex.length() - field.nDigits + d];
		command[field.startIndex + (d * field.skip)] = fillChar;
	}
}
