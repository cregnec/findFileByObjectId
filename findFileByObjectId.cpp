// findFileByObjectId.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <WinBase.h>
#include <Ole2.h>
#include <tchar.h>
#include <stdio.h>

#define BUFSIZE 255
#define DATA4_SIZE 8

int _tmain(int argc, _TCHAR* argv[])
{
	FILE_ID_DESCRIPTOR fileId;
	fileId.Type = ObjectIdType;
	fileId.dwSize = sizeof(fileId);
	
    /* read the GUID from user. If not, initialize a predefined one*/
	if (argc > 1){
		if (!SUCCEEDED(CLSIDFromString(argv[1], &fileId.ObjectId))){
			return 0;
		}
	}
	else{
		fileId.ObjectId.Data1 = 0x220b8c54;
		fileId.ObjectId.Data2 = 0x27b0;
		fileId.ObjectId.Data3 = 0x11e3;
		BYTE data4[DATA4_SIZE] = { 0x90, 0xa9, 0x0, 0x50, 0x56, 0x93, 0x5a, 0x93 };
		for (int i = 0; i < DATA4_SIZE; i++){
			fileId.ObjectId.Data4[i] = data4[i];
		}
	}

	/* print GUID */
	WCHAR strGUID[40];
	StringFromGUID2(fileId.ObjectId, strGUID, 40);
	_tprintf(TEXT("GUID is %s\n"), strGUID);

	WCHAR lpFileName[] = L"\\\\.\\C:";
	/* open the volume on which the file resides*/
	HANDLE diskHandle = CreateFile(
		lpFileName,
		FILE_READ_ATTRIBUTES, //enough to get the file path
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (diskHandle != INVALID_HANDLE_VALUE){
		HANDLE fileHandle = OpenFileById(
			diskHandle,
			&fileId,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			0);

		if (fileHandle != INVALID_HANDLE_VALUE){
			TCHAR path[BUFSIZE];
			DWORD dwRet;
			/* get the file name by its handle*/
			dwRet = GetFinalPathNameByHandle(
				fileHandle,
				path,
				BUFSIZE,
				VOLUME_NAME_NT);
			if (dwRet < BUFSIZE){
				_tprintf(TEXT("\nThe final path is %s\n"), path);
			}
			CloseHandle(fileHandle);
		}
		else{
			puts("Error opening file by Id\n");
		}
		CloseHandle(diskHandle);
	}
	else{
		puts("Error opening volume\n");
	}
	return 0;
}
