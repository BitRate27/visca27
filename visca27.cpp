// visca27.cpp : Defines the functions for the static library.
//
#include "visca27.h"

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
int SetCamera(uint64_t ConnectSocket, std::string hexcmd)
{
	int iResult;

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
int OpenSocket(uint64_t *ConnectSocket, std::string IP, int port) {
	int iResult;
	//WSADATA wsaData;
	int result = VERR;
	*ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService = {};

	//char recvbuf[DEFAULT_BUFLEN] = "";

	//----------------------
	// Initialize Winsock
	/*
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		return VCONNECT_ERR;
	}
	*/
	//----------------------
	// Create a SOCKET for connecting to server
	*ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*ConnectSocket == INVALID_SOCKET) {
		//WSACleanup();
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
	if (iResult == SOCKET_ERROR) { // TODO: force error and test
		*ConnectSocket = INVALID_SOCKET;
		closesocket(*ConnectSocket);
		//WSACleanup();
		return VCONNECT_ERR;
	}

	// Set non-blocking mode
	u_long iMode = 1;
	iResult = ioctlsocket(*ConnectSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR) {
		closesocket(*ConnectSocket);
		*ConnectSocket = INVALID_SOCKET;
		//WSACleanup();
		return VCONNECT_ERR;
	}
	return VOK;
}

int CloseSocket(uint64_t ConnectSocket) {
	int iResult;
	// close the socket
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		//WSACleanup();
		return VCLOSE_ERR;
	}
	//WSACleanup();
	return VOK;
}

int GetCamera(uint64_t ConnectSocket, std::string hexcmd, std::string *returnhex)
{
	int iResult;

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
			result = VOK;
			break;
		}
		else {
			// int wle = WSAGetLastError();
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
ValueField::ValueField(char field, std::string fmt) {
	startIndex = -1;
	nDigits = 0;
	skip = 0;
	int lastIndex = -1;
	for (int i = 0; i < fmt.length(); i++) {
		if (!isHex(fmt[i])) {
			if ((fmt[i] == field) || (field == ' ')) {
				if (startIndex == -1) {
					startIndex = i;
				}
				else if (lastIndex > -1) {
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

void ValueConverter::addField(char f) {
	if (f == ' ') return;
	ValueField field(f, fmt);
	fields[f] = field;
	nFields++;
}
short ValueConverter::getValue(char f, std::string replyString)
{
	auto it = fields.find(f);
	if (it == fields.end()) return 0;
	ValueField field = it->second;
	if ((field.nDigits == 0) ||
		(field.nDigits >= 30) ||
		(replyString.size() < field.startIndex + ((field.nDigits - 1) * field.skip)))
		return -1;

	char valueHex[30] = "";
	for (int d = 0; d < field.nDigits; d++) {
		valueHex[d] = replyString[field.startIndex + (d * field.skip)];
	}
	return static_cast<short>(std::stoi(std::string(valueHex, field.nDigits), nullptr, 16));
}

void ValueConverter::setValue(char f, int val)
{
	auto it = fields.find(f);
	if (it == fields.end()) return;
	ValueField field = it->second;
	char setCommand[30];
	std::stringstream ss1;
	ss1 << std::hex << (short)val;
	std::string valueHex = ss1.str();

	for (int i = 0; i < command.length(); i++) {
		setCommand[i] = command[i];
	}

	for (int d = 0; d < field.nDigits; d++) {
		char fillChar = '0';
		if (d >= field.nDigits - valueHex.length())
			fillChar = valueHex[valueHex.length() - field.nDigits + d];
		setCommand[field.startIndex + (d * field.skip)] = fillChar;
	}

	command = toUpper(std::string(setCommand, command.length()));
}
