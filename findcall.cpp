// BaseAddrTools.cpp : Defines the entry point for the DLL application.
//
 
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
BOOL GetProcessModuleHandle(DWORD PID,const char*szModuleName,MODULEENTRY32 *pModule);//???????
BOOL StringToByte(const char *InBuff,unsigned char *OutBuff);//??????????
BYTE *MemoryFind(BYTE *Buff1,BYTE *Buff2,DWORD Buff1Size,DWORD Buff2Size);//??????
void FindCallAddr(const char *Buff,int OffsetSize,const char *ModuleName,char *Regexp);//??CALL??
void FindFunctionAddr(const char *Buff,int OffsetSize,const char *ModuleName,char *Regexp);//???????
void FindConstAddr(const char *Buff,int OffsetSize,const char *ModuleName,char *Regexp);//??????
 
BOOL WINAPI DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	
	FindConstAddr("000000000100000002000000000000000C43",0XD,"Stardew Valley","0x%08X\n");
	return TRUE;
}
 
void FindConstAddr(const char *Buff,int OffsetSize,const char *ModuleName,char *Regexp)
{
	MODULEENTRY32 Module32;
	if (GetProcessModuleHandle(GetCurrentProcessId(),ModuleName,&Module32) == FALSE)
		return ;//???????????
	DWORD BuffLen = strlen(Buff)/2;//??????????
	BYTE *OutBuff = new BYTE[BuffLen];//????????????,+3???
	if(!StringToByte(Buff,OutBuff))
	{//?????????????
		delete []OutBuff;
		return ;
	}
	BYTE *Temp = MemoryFind(Module32.modBaseAddr,OutBuff,Module32.modBaseSize,BuffLen);//???????????????
	while (Temp)
	{
		char DbgOutBuff[MAX_PATH] = {0};
		sprintf(DbgOutBuff,Regexp,*(DWORD *)((int)Temp+OffsetSize));
		OutputDebugString(DbgOutBuff);
		Temp = MemoryFind(Temp+1,OutBuff,Module32.modBaseSize - (Temp - Module32.modBaseAddr),BuffLen);
	}
	delete []OutBuff;
}
 
void FindFunctionAddr(const char *Buff,int OffsetSize,const char *ModuleName,char *Regexp)
{
	MODULEENTRY32 Module32;
	if (GetProcessModuleHandle(GetCurrentProcessId(),ModuleName,&Module32) == FALSE)
		return ;//???????????
	DWORD BuffLen = strlen(Buff)/2;//??????????
	BYTE *OutBuff = new BYTE[BuffLen];//????????????,+3???
	if(!StringToByte(Buff,OutBuff))
	{//?????????????
		delete []OutBuff;
		return ;
	}
	BYTE *Temp = MemoryFind(Module32.modBaseAddr,OutBuff,Module32.modBaseSize,BuffLen);//???????????????
	while (Temp)
	{
		char DbgOutBuff[MAX_PATH] = {0};
		sprintf(DbgOutBuff,Regexp,(int)Temp+OffsetSize);
		OutputDebugString(DbgOutBuff);
		Temp = MemoryFind(Temp+1,OutBuff,Module32.modBaseSize - (Temp - Module32.modBaseAddr),BuffLen);
	}
	delete []OutBuff;
}
void FindCallAddr(const char *Buff,int OffsetSize,const char *ModuleName,char *Regexp)
{
	MODULEENTRY32 Module32;
	if (GetProcessModuleHandle(GetCurrentProcessId(),ModuleName,&Module32) == FALSE)
		return ;//???????????
	DWORD BuffLen = strlen(Buff)/2;//??????????
	BYTE *OutBuff = new BYTE[BuffLen];//????????????,+3???
	if(!StringToByte(Buff,OutBuff))
	{//?????????????
		delete []OutBuff;
		return ;
	}
	BYTE *Temp = MemoryFind(Module32.modBaseAddr,OutBuff,Module32.modBaseSize,BuffLen);//???????????????
	while (Temp)
	{
		DWORD CallAddr = *(DWORD*)(Temp+OffsetSize+1) + (int)Temp+OffsetSize + 5;
		char DbgOutBuff[MAX_PATH] = {0};
		sprintf(DbgOutBuff,Regexp,CallAddr);
		OutputDebugString(DbgOutBuff);
		Temp = MemoryFind(Temp+1,OutBuff,Module32.modBaseSize - (Temp - Module32.modBaseAddr),BuffLen);
	}
	delete []OutBuff;
}
 
BYTE *MemoryFind(BYTE *Buff1,BYTE *Buff2,DWORD Buff1Size,DWORD Buff2Size)
{
	if (Buff1Size < Buff2Size)
		return NULL;
	for (DWORD Count1 = 0 ; Count1 + Buff2Size <= Buff1Size; Count1++)
	{
		for (DWORD Count2 = 0;Count2 < Buff2Size;Count2++)
		{
			if (Buff2[Count2] == 0)
				continue;
			if (Buff1[Count1 + Count2] != Buff2[Count2])
				break;
		}
		if (Count2 == Buff2Size)
		{
			return &Buff1[Count1];
		}
	}
	return NULL;
}
BOOL GetProcessModuleHandle(DWORD PID,const char*szModuleName,MODULEENTRY32 *pModule)
{
	BOOL FunctionRetn = FALSE;
	HANDLE handle;
	MODULEENTRY32  Module32;
	memset(&Module32,0,sizeof(Module32));
	Module32.dwSize = sizeof(Module32);
	BOOL bRet = FALSE;
	handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID);
	if(handle == INVALID_HANDLE_VALUE) return FunctionRetn;
	bRet = Module32First(handle,&Module32);
	while(bRet)
	{
//		printf("%d\t%s\t%s\n",Module32.th32ModuleID,Module32.szModule,Module32.szExePath);
		if(strcmp(szModuleName,Module32.szModule) == 0)
		{
			memcpy(pModule,&Module32,sizeof(MODULEENTRY32));
			FunctionRetn = TRUE;
			break;
		}
		//		OutputDebugString(buf);
		bRet = Module32Next(handle,&Module32);
	}
	CloseHandle(handle);
	return FunctionRetn;
}
BOOL StringToByte(const char *InBuff,unsigned char *OutBuff)
{
	DWORD BuffSize = strlen(InBuff);
	if (BuffSize%2 != 0)
	{
		MessageBox(0,"????","",0);
		return FALSE;
	}
	//????????????,+3???sscanf????4???,????????????
	BYTE *TMPBUF = new BYTE[BuffSize/2+3];
	memset(TMPBUF,0,BuffSize/2);
	for (DWORD Index = 0 ; Index < BuffSize ; Index+=2)
	{
		char buf[3] = {0};
		buf[0] = InBuff[Index];
		buf[1] = InBuff[Index+1];
		sscanf(buf,"%X",&TMPBUF[Index/2]);
	}
	memcpy(OutBuff,TMPBUF,BuffSize/2);
	delete []TMPBUF;
	return TRUE;
}

