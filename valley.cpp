#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


union Base     
{  
    DWORD   address;  
    BYTE    data[4];  
};

int* getNext(unsigned char *markCode,int charlen){
	unsigned char *p = new unsigned char[charlen+1];
	p = markCode;
	int *next = new int[charlen+1];
	next[0]=-1;
	int j=0;
	int k=-1;//next[j-1]
	while(j<charlen-1){
		if(k==-1||p[j]==p[k]){
			if(p[++j]==p[++k]){
				next[j]=next[k];
			}
			else{
				next[j]=k;	
			}
		}
		else{
			k=next[k];
		}
	}
	return next;
}

DWORD StoryScanAddress(HANDLE process, char *markCode){  	
    const DWORD beginAddr = 0x00000000;  
    const DWORD endAddr = 0x06FFFFFF;  
    const DWORD pageSize = 4096;  
    if (strlen(markCode) % 2 != 0) return 0;  
    int len = strlen(markCode) / 2;  
	int *next=new int[len];	
    BYTE *m_code = new BYTE[len];  
    for (int i = 0; i < len; i++){  
        char c[] = {markCode[i*2], markCode[i*2+1], '\0'};  
        m_code[i] = strtol(c, NULL, 16); 
    }  
    next=getNext(m_code,len);
    BOOL _break = FALSE;  
    int curPage = 0;  
    int curIndex = 0;  
    Base base;  
    BYTE page[pageSize];  
    DWORD tmpAddr = beginAddr;  
	while (tmpAddr <= endAddr - len){  
        if(ReadProcessMemory(process, (LPCVOID)tmpAddr, &page, pageSize, 0)==0){
        	curPage++;  
        	tmpAddr += pageSize;
		}
		else{
			int m=0;
			int n=0;	
			while(m<pageSize && n<len){
				if(n==-1||page[m]==m_code[n]){
					m++;
					n++;
				}
				else{
					n=next[n];
				}
			}
			if(n==len){
				cout<<"matching"<<endl;
				_break=TRUE;
				curIndex = m;
			}
			if (_break) break; 
			curPage++;  
        	tmpAddr += pageSize;
		}
	}
	//delete[] page;
	cout<<hex<<tmpAddr+curIndex-0x1<<endl;
    return tmpAddr+curIndex-0x1;
}  

void Write(HANDLE process,DWORD Address,char *ch){
	int len = strlen(ch)/2;
	BYTE *bases = new BYTE[3];
	for (int i = 0; i < len; i++){  
        char c[] = {ch[i*2], ch[i*2+1], '\0'}; 
        bases[i] = (BYTE)strtol(c, NULL, 16); 
    }  
	if(!WriteProcessMemory(process,(LPVOID)Address,(LPVOID)bases,len,0))
					{
						printf("failed\n");
					}
					else
					{
						printf("succeed\n");
					}
}

int main(){
	HWND hq = FindWindow(NULL,"Stardew Valley");
	RECT rect,rect2;
	DWORD pid;
	GetWindowRect(hq,&rect);
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;
	cout<<"game height : "<<h<<endl<<"game width : "<<w<<endl;
    GetWindowThreadProcessId(hq,&pid);
    if(!pid){
    	cout<<"wrong"<<endl;
	}
    cout<<pid<<endl;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    cout<<hProcess<<endl;
    
    DWORD MoneyAddress,EnergyAddress,TimeAddress;
    MoneyAddress = StoryScanAddress(hProcess,"000000000100000002000000000000000C43")-0x4D;
    EnergyAddress = StoryScanAddress(hProcess,"0100000000000000000000000000000000000100000000000000000000000000000000000000DCC8")-0xC5;
    TimeAddress = StoryScanAddress(hProcess,"0000FFFFFFFFFFFFFFFF000000000000000016000000")-0x17;
    
    
	BYTE *money = new BYTE[100];
	Write(hProcess,(LPVOID)MoneyAddress,"A08601");
	Write(hProcess,(LPVOID)EnergyAddress,"000087");
	Write(hProcess,(LPVOID)TimeAddress,"5802");
	return 0;
}
