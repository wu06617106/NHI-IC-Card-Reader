#pragma once
#include <iostream>
#include <string>

using namespace std;

#ifdef NHIICCARDREADER_EXPORTS
#define NHIICCARDREADER_API __declspec(dllexport)
#else
#define NHIICCARDREADER_API __declspec(dllimport)
#endif

struct NHIInfo
{
	string name = "";
	string id = "";
	string birthDay = "";
	string gender = "";
};

class NHIICCARDREADER_API NHIICCardReader
{
public:
	static int ReadSCardInfo(NHIInfo &icInfo);
private:
	static NHIInfo SplitInfoString(char const*info, int length);
	static string RetrieveStringByIndex(char const*info, int startIndex, int endIndex, int length);
};
