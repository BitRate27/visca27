#pragma once
#include <string>
#include <cstdio>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include "ViscaAPI.h"
//#include <Iphlpapi.h>
#include <Assert.h>
#include <map>
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
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
typedef unsigned long long UINT_PTR;
#endif

int OpenSocket(UINT_PTR *ConnectSocket, std::string IP, int port = 5678);
int CloseSocket(UINT_PTR ConnectSocket);
int GetCamera(UINT_PTR ConnectSocket, std::string hexcmd, std::string *returnhex);
int SetCamera(UINT_PTR ConnectSocket, std::string hexcmd);

class ValueField {
public:
    ValueField() {};
    ValueField(char field, std::string fmt);
    int nDigits;
    int startIndex;
    int skip;
};

class ValueConverter
{
public:
    ValueConverter(std::string format, char f0 = ' ', char f1 = ' ', char f2 = ' ');
    ~ValueConverter();
    void addField(char f);
    short getValue(char f, std::string replyhex);
    void setValue(char f, int val);
    std::string getCommand() {
        return command;
    };
private:
    std::map<char,ValueField> fields;
    int nFields;
    std::string fmt;
    std::string command;
};