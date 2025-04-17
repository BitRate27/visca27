#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <map>
typedef int ssize_t;
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
typedef int SOCKET;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (SOCKET)(~0)
typedef struct sockaddr SOCKADDR;
#define NO_ERROR 0L
#endif
#include <string>
#include <cstddef>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

int OpenSocket(SOCKET *ConnectSocket, std::string IP, int port,
	       int32_t &sequence, bool udp = false);
int CloseSocket(SOCKET ConnectSocket);
int GetCamera(SOCKET ConnectSocket, std::string hexcmd, std::string *returnhex,
	int &sequence, bool encapsulated = false);
int SetCamera(SOCKET ConnectSocket, std::string hexcmd, int32_t &sequence,
	bool encapsulated = false);

class ValueField {
public:
	ValueField() {};
	ValueField(char field, std::string fmt);
	size_t nDigits;
	size_t startIndex;
	size_t skip;
};

class ValueConverter {
public:
	ValueConverter(std::string format, char f0 = ' ', char f1 = ' ',
		       char f2 = ' ');
	~ValueConverter();
	void init();
	void addField(char f);
	int getValue(char f, std::string replyhex);
	void setValue(char f, int val);
	std::string getCommand() { return command; };

private:
	std::map<char, ValueField> fields;
	int nFields;
	std::string fmt;
	std::string command;
};
