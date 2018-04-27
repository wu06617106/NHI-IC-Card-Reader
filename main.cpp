#include "NHIICCardReader.h"

int main(int argc, char *argv[])
{
	NHIInfo info;
	if (NHIICCardReader::ReadSCardInfo(info) == 0)
	{
		cout << endl;
		cout << "Name: " << info.name << endl;
		cout << "ID: " << info.id << endl;
		cout << "Birthday: " << info.birthDay << endl;
		cout << "Gender: " << info.gender << endl;
	}
	system("pause");
	return 0;
}
