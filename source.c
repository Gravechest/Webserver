#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


WSADATA data;
SOCKET sock;
unsigned char clientC;
SOCKET client[128];
SOCKADDR_IN adres;
SOCKADDR_IN adres2;
char rbuf[9999];
char command[40];
char *htmlFile;

char *loadHtml(char *name){
	HANDLE h = CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	char *file = HeapAlloc(GetProcessHeap(),8,fsize+3);
	ReadFile(h,file,fsize,0,0);
	file[fsize] = '\n';
	file[fsize+1] = '\r';
	file[fsize+2] = '\0';
	CloseHandle(h);
	return file;
}

void sendHtml(unsigned char n){
	recv(client[clientC-1],rbuf,sizeof(rbuf),0);
	printf("%s\n",rbuf);
	send(client[clientC-1],htmlFile,strlen(htmlFile),0);
	closesocket(client[clientC-1]);
	clientC--;
}

int main() {
	htmlFile = loadHtml("source.html");
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
    adres.sin_family = AF_INET;
	adres.sin_port = htons(80);
	adres.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock,(SOCKADDR*)&adres,sizeof(adres));
	for(;;){
		listen(sock,SOMAXCONN);
		client[clientC] = accept(sock,0,0);
		CreateThread(0,0,sendHtml,&clientC,0,0);
		clientC++;
	}
}