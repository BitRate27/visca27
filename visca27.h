#pragma once
#include <string>
std::string GetCamera(std::string IP, std::string hexcmd);
std::string SetCamera(std::string IP, std::string hexcmd);
class ValueConverter
{
public:
    ValueConverter(std::string format, char field1 = ' ', char field2 = ' ');
    ~ValueConverter();
    int getValue1(std::string returnString);
    int getValue2(std::string returnString);
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