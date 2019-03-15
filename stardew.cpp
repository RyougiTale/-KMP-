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

void Getall(){
    HWND hd=GetDesktopWindow();  
    hd=GetWindow(hd,GW_CHILD);  
    char s[200]={0};  
    while(hd!=NULL)  
    {  
        memset(s,0,200);  
        GetWindowText(hd,s,200);  
        /*if (strstr(s,"QQ2012"))  
        {  
            cout<<s<<endl;  
            SetWindowText(hd,"My Windows");  
        }*/  
        cout<<s<<endl;  
          
        hd=GetNextWindow(hd,GW_HWNDNEXT);
    }
}



DWORD ScanAddress(HANDLE process, char *markCode,   
                  DWORD distinct = 1, DWORD findMode = 1,   
                  LPDWORD offset = NULL)  
{  

    const DWORD beginAddr = 0x00000000;  

    const DWORD endAddr = 0x05FFFFFF;  

    const DWORD pageSize = 4096;  
  

    if (strlen(markCode) % 2 != 0) return 0;  
    //cout<<strlen(markCode)<<endl<<endl;

    int len = strlen(markCode) / 2;  

    BYTE *m_code = new BYTE[len];  
    for (int i = 0; i < len; i++){  
        char c[] = {markCode[i*2], markCode[i*2+1], '\0'};  
        //m_code[i] = (BYTE)::strtol(c, NULL, 16);  
        m_code[i] = strtol(c, NULL, 16); 
        //cout<<m_code[i]<<endl;
        //m_code[i] = markCode[i];
    }  
    /*for(int i=0;i<strlen(markCode);i++){
    	cout<<markCode[i]<<"  ";
    	printf("%d\n",m_code[i]);
	}*/
    
    BOOL _break = FALSE;  

    int curPage = 0;  
    int curIndex = 0;  
    Base base;  
    base.data[0] = 0;  
    base.data[1] = 0;  
    base.data[2] = 0;  
    base.data[3] = 0;  
    //4096
    BYTE page[pageSize];  
    DWORD tmpAddr = beginAddr;  
    //cout<<hex<<tmpAddr<<endl<<endAddr-len<<endl<<len<<endl;
    while (tmpAddr <= endAddr - len){  
    	//cout<<hex<<"???"<<tmpAddr<<" "<<endl;
        int h = ReadProcessMemory(process, (LPCVOID)tmpAddr, &page, pageSize, 0);  

        //cout<<"justwe"<<h<<endl;
        for (int i = 0; i < pageSize; i++){  
        	//Sleep(500);
        	//cout<<i<<" :"<<hex<<page[i]<<endl;
        	//printf("%d  :  %d     %d\n",i+1,m_code[i],page[i]);
            for (int j = 0; j < len; j++){   
            	//printf("%d  :  %d     %d\n",j+1,m_code[i],page[i+j]);
                //cout<<m_code[j]<<"!!"<<endl;
                if (m_code[j] != page[i + j])break;  

                if (j == len - 1){  
                	cout<<"matching"<<endl;
                    _break = TRUE;  
                    distinct = 0;
                    if (!findMode){  
                        curIndex = i;  
                        base.data[0] = page[curIndex-distinct-4];  
                        base.data[1] = page[curIndex-distinct-3];  
                        base.data[2] = page[curIndex-distinct-2];  
                        base.data[3] = page[curIndex-distinct-1];  
                    }else{  
                        curIndex = i + j;  
                        base.data[0] = page[curIndex+distinct+1];  
                        base.data[1] = page[curIndex+distinct+2];  
                        base.data[2] = page[curIndex+distinct+3];  
                        base.data[3] = page[curIndex+distinct+4];  
                    }  
                    //cout<<"? "<<curPage<<" "<<curIndex<<endl;
                    //cout<<hex<<tmpAddr+curPage*4096+curIndex<<endl;
                    break;  
                }  
            }  
            if (_break) break;  
        }  
        if (_break) break;  
        curPage++;  
        tmpAddr += pageSize;  
    }  
    if(offset != NULL){  
        *offset = curPage * pageSize + curIndex + beginAddr;  
    }  
    //cout<<tmpAddr<<endl<<curPage<<endl<<curIndex<<endl;
    return tmpAddr+curIndex;
    //return base.address;  
}  

int* getNext(unsigned char *markCode,int charlen){
	//char p[charlen] = markCode;
	
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
	//or(int i=0;i<charlen;i++) cout<<next[i]<<endl;
	return next;
}

/*int KMP(BYTE *ts,BYTE *ps){
	int i=0;j=0;
}*/

DWORD StoryScanAddress(HANDLE process, char *markCode,   
                  DWORD distinct = 1, DWORD findMode = 1)  
{  	
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
		//cout<<hex<<tmpAddr<<" "<<endAddr<<" "<<len<<endl;
        if(ReadProcessMemory(process, (LPCVOID)tmpAddr, &page, pageSize, 0)==0){
        	curPage++;  
        	tmpAddr += pageSize;
		}
		else{
			int m=0;
			int n=0;	
			while(m<pageSize && n<len){
				if(n==-1||page[m]==m_code[n]){
					//printf("%d %d %d %d %d\n",n,page[m],m_code[n],m,n);
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
        	//cout<<hex<<tmpAddr<<endl;
		}
	}
	//delete[] page;
	//cout<<tmpAddr<<" "<<curIndex<<endl;
    return tmpAddr+curIndex-0x1;
}  


int main(){
	HWND hq = FindWindow(NULL,"Stardew Valley");
	RECT rect,rect2;
	DWORD pid;
	GetWindowRect(hq,&rect);
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;
	cout<<"game height : "<<h<<endl<<"game width : "<<w<<endl;
	/*
	HWND hq2=GetDesktopWindow();  
    GetWindowRect(hq2,&rect2);      
    int w2=rect2.right-rect.left;  
    int h2=rect2.bottom-rect.top;  
    cout<<"windows height : "<<h2<<endl<<"windows width : "<<w2<<endl;  
    */
    GetWindowThreadProcessId(hq,&pid);
    if(!pid){
    	cout<<"wrong"<<endl;
	}
    cout<<pid<<endl;
	//HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, ProcessId)
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    cout<<hProcess<<endl;
    DWORD MoneyAddress = 0,EnergyAddress = 0;
    char s[]="000000000100000002000000000000000C43";
    MoneyAddress = StoryScanAddress(hProcess,"000000000100000002000000000000000C43")-0x4D;//0x3C
    //0000FFFFFFFFFFFFFFFF000000000000000016000000 time
    EnergyAddress = StoryScanAddress(hProcess,"0100000000000000000000000000000000000100000000000000000000000000000000000000DCC8")-0xC5;
    //ReadProcessMemory(pid, (LPVOID)moueyAdress, &MoneyAdress, sizeof(MoneyAdress), 0);
	//cout<<hex<<MoneyAdress-0x7313D654<<endl;
	cout<<hex<<MoneyAddress<<endl;
	cout<<hex<<EnergyAddress<<endl;
	BYTE *money = new BYTE[100];
	money[0]=0xA0;
	money[1]=0x86;
	money[2]=0x01;
	money[3]=0x00;
	money[4]=0x00;
	money[5]=0x87;
	if(!WriteProcessMemory(hProcess,(LPVOID)MoneyAddress,(LPVOID)money,3,0))
					{
						printf("failed\n");
					}
					else
					{
						printf("succeed\n");
					}
	if(!WriteProcessMemory(hProcess,(LPVOID)EnergyAddress,(LPVOID)(money+3),3,0))
					{
						printf("failed\n");
					}
					else
					{
						printf("succeed\n");
					}
	return 0;
}
