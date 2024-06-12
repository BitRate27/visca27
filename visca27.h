#pragma once
#include <string>
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
#include "ViscaAPI.h"
#include <map>
#pragma comment(lib, "iphlpapi.lib")
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

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