#include "NHIICCardReader.h"
#include <winscard.h>

int NHIICCardReader::ReadSCardInfo(NHIInfo &icInfo)
{
	SCARDCONTEXT hContext = 0;
	LONG rv = 0;
	LPTSTR mszReaders;
	DWORD dwReaders;
	SCARDHANDLE hCard = 0;
	DWORD dwActiveProtocol = 0;
	if (SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext) == 0)
	{
		cout << "SCard Establish Context." << endl;
		if (SCardListReaders(hContext, NULL, NULL, &dwReaders) == 0)
		{
			mszReaders = (LPTSTR)malloc(sizeof(char)*dwReaders);
			if (SCardListReaders(hContext, NULL, mszReaders, &dwReaders) == 0)
			{
				cout << "SCard List Readers." << endl;
				LPCSCARD_IO_REQUEST ioRequest = SCARD_PCI_T0;
				bool isConnected = false;
        
        /*Try to connect with T0 protocol.
          If it is failed, then try with T1 protocol.
        */
        if (SCardConnect(hContext, mszReaders, SCARD_SHARE_EXCLUSIVE,
					SCARD_PROTOCOL_T0, &hCard, 0) == 0)
				{
					ioRequest = SCARD_PCI_T0;
					cout << "SCard Connect by SCARD_PROTOCOL_T0." << endl;
					isConnected = true;
				}
				else if (SCardConnect(hContext, mszReaders, SCARD_SHARE_EXCLUSIVE,
					SCARD_PROTOCOL_T1, &hCard, 0) == 0)
				{
					ioRequest = SCARD_PCI_T1;
					cout << "SCard Connect by SCARD_PROTOCOL_T1." << endl;
					isConnected = true;
				}
				if (isConnected)
				{
					DWORD selectAPDULength, dwRecvLength, readProfileAPDULength, profileRecvLength;
					SCARD_IO_REQUEST pioRecvPci;
					pioRecvPci.cbPciLength = 8;
					pioRecvPci.dwProtocol = 0;
					BYTE selectAPDU[] = { 0x00, 0xA4, 0x04, 0x00, 0x10, 0xD1, 0x58, 0x00, 0x00, 0x01, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00 };
					BYTE readProfileAPDU[] = { 0x00, 0xca, 0x11, 0x00, 0x02, 0x00, 0x00 };
					BYTE profileRecvBytes[59];
					BYTE pbRecvBuffer[100];
					selectAPDULength = sizeof(selectAPDU);
					dwRecvLength = sizeof(pbRecvBuffer);
					profileRecvLength = sizeof(profileRecvBytes);
					readProfileAPDULength = sizeof(readProfileAPDU);
          
          /* Transmit select profile APDU to smart card.
          */
					if (SCardTransmit(hCard, ioRequest, selectAPDU, selectAPDULength,
						&pioRecvPci, pbRecvBuffer, &dwRecvLength) == 0)
					{
						cout << "Select Profile APDU success." << endl;
            
            /* Transmit read profile APDU to smart card.
            */
						if (SCardTransmit(hCard, ioRequest, readProfileAPDU, readProfileAPDULength,
							&pioRecvPci, profileRecvBytes, &profileRecvLength) == 0)
						{
							cout << "Read Profile APDU success." << endl;
							icInfo = SplitInfoString(reinterpret_cast< char const* >(profileRecvBytes), profileRecvLength);
						}
						else
						{
							cout << "Read Profile APDU failed." << endl;
							return -1;
						}
					}
					else
					{
						cout << "Select Profile APDU failed." << endl;
						return -1;
					}
				}
				else
				{
					cout << "SCard Connect failed." << endl;
					return -1;
				}
			}
		}
	}
	SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
	SCardReleaseContext(hContext);
	return 0;
}

NHIInfo NHIICCardReader::SplitInfoString(char const*info, int length)
{
	/*
	健保IC卡 各項基本資料 Index
	健保卡ID   0 - 11
	姓名      12 - 31
	身分證字號 32 - 41
	生日      42 - 48
	性別      49 - 49
	發卡日期   51 - 56
	*/
	NHIInfo icInfo;
	icInfo.name = RetrieveStringByIndex(info, 12, 31, length);
	icInfo.id = RetrieveStringByIndex(info, 32, 41, length);
	icInfo.birthDate = RetrieveStringByIndex(info, 42, 48, length);
	icInfo.gender = RetrieveStringByIndex(info, 49, 49, length);
	return icInfo;
}

string NHIICCardReader::RetrieveStringByIndex(char const*info, int startIndex, int endIndex, int length)
{
	string str = "";
	if (length != 0
		&& startIndex >= 0 && startIndex <= (length - 1)
		&& endIndex >= 0 && endIndex <= (length - 1))
	{
		for (int i = startIndex; i <= endIndex; i++)
		{
			str += info[i];
		}
	}
	return str;
}

