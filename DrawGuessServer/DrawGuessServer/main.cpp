#include <iostream>
#include "TCPKernel.h"
using namespace std;

int main(){
	TCPKernel *pKernel = new TCPKernel;
	if(pKernel->Open())
		cout<<"服务器开启成功，正在运行....."<<endl;

	system("pause");
	return 0;
}