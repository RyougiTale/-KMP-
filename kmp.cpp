#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

const vector<int> * kmp_next(string &m) // count the longest prefex string ; 
{ 
	static vector<int> next(m.size()); 
	next[0]=0; // the initialization of the next[0]; 

	int temp; // the key iterator...... 

	for(int i=1;i<next.size();i++) 
	{ 
		temp=next[i-1]; 

		while(m[i]!=m[temp]&&temp>0) 
		{ temp=next[temp-1]; 
		} 

		if(m[i]==m[temp]) 
			next[i]=temp+1; 
		else next[i]=0; 
	}
	return &next;
} 


int kmp_search(string m,int start,int end) 
{ 
	const vector<int> * next=kmp_next(m); 

	int tp=start; 
	int mp=0; // text pointer and match string pointer; 

	for(tp=start;tp<end;tp++) 
	{ 
		while(*((char *)tp)!=m[mp]&&mp) 
			mp=(*next)[mp-1]; 

		if(*((char *)tp)==m[mp]) 
			mp++; 

		if(mp==m.size()) 
		{  
			return tp-mp+1; 
		} 
	} 

	if(tp==end) 
		return 0; 
} 

string HexChartoChar(string &src)	//16 to char
{
	string des;
	int len=src.size();
	des.resize(len/2);
	unsigned char hex=0;
	unsigned char num=0;

	for (int i=0,j=0;i<len;i++)
	{
		if (src[i]>='A'&&src[i]<='F')
		{
			num=src[i]-55;
			hex+=num<<(1-i%2)*4;
		}
		else if(src[i]>='a'&&src[i]<='f')
		{
			num=src[i]-87;
			hex+=num<<(1-i%2)*4;
		}
		else if (src[i]>='0'&&src[i]<='9')
		{
			num=src[i]-48;
			hex+=num<<(1-i%2)*4;
		}
		if (i%2==1)
		{
			des[j]=hex;
			j++;
			hex=0;
		}
	}
	return des;
}

int main(int argc, char *argv[])
{
	//long t1 = GetTickCount();
	int start=0x400000;
	int end=0x7fffffff;
	//string s="000000000100000002000000000000000C43";
	string s="000000000100";
	string t=HexChartoChar(s);
	int pos =0 ;
	for (int i=0;i<2;i++)
	pos=kmp_search(t,start,end);
	cout<<hex<<pos<<endl;
	//long t2 = GetTickCount();
	//cout<<t2-t1<<"ms"<<endl;
	return 0;
} 
