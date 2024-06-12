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
#pragma comment(lib, "iphlpapi.lib")
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int OpenSocket(UINT_PTR *ConnectSocket, std::string IP, int port = 5678);
int CloseSocket(UINT_PTR ConnectSocket);
int GetCamera(UINT_PTR ConnectSocket, std::string hexcmd, std::string *returnhex);
int SetCamera(UINT_PTR ConnectSocket, std::string hexcmd);
class ValueConverter
{
public:
    ValueConverter(std::string format, char field1 = ' ', char field2 = ' ');
    ~ValueConverter();
    int getValue1(std::string returnhex);
    int getValue2(std::string returnhex);
    std::string setValue(int val1, int val2 = -1);
    int nDigits1 = 0;
    int nDigits2 = 0;
private:
    int startIndex1 = -1;
    int skip1 = 0;
    int startIndex2 = -1;
    int skip2 = 0;
    std::string fmt;
};