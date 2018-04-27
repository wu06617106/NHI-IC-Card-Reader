#include "NHIICCardReader.h"

int main(int argc, char *argv[])
{
	NHIInfo info;
	if (NHIICCardReader::ReadSCardInfo(info) == 0)
	{
		cout << endl;
		cout << "姓名:" << info.name << endl;
		cout << "身分證字號:" << info.id << endl;
		cout << "生日:" << info.birthDate << endl;
		cout << "性別:" << info.gender << endl;
	}
	system("pause");
	return 0;
}
