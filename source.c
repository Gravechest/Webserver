#include <stdio.h>
#include <winsock2.h>

WSADATA data;
SOCKET sock;
unsigned char clientC;
SOCKET client;
SOCKADDR_IN adres;

unsigned int clientId;

char *input;

unsigned int lsockaddres = sizeof(SOCKADDR);

typedef struct{
	char *name;
	char *p1;
	unsigned char type;
}COMMAND;

unsigned int cmdsC;
unsigned int cmdsOC;
COMMAND *cmds;
COMMAND *cmdsO;

typedef struct{
	unsigned int webpage;
	long long ip;
}ACCOUNT;

unsigned char accountsC;
ACCOUNT *accounts;

typedef struct{
	unsigned int i;
	char *p;
}PSTRING;

typedef struct{
	int size;
	char *content;
}WEBFILE;

typedef struct{
	char *name;
	int fC;
	WEBFILE *f;
}DATABASE;

typedef struct{
	char *name;
	WEBFILE f;	
}TFILE;

unsigned int databaseC;
DATABASE *database;

unsigned int filesC;
TFILE *files;

WEBFILE tscriptI,tscriptO;

unsigned int getCmdSz(unsigned int itt,char *script){
	unsigned int itt2 = itt;
	for(;script[itt2] != ',' && script[itt2] != '\n' && script[itt2] != '\0';itt2++){}
	return itt2-itt;
}

unsigned int getRequestSize(char *request){
	unsigned int r;
	for(;request[r] != ' ';r++){}
	return r;
}

WEBFILE loadFile(char *name){
	WEBFILE file;
	HANDLE h = CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	file.size = GetFileSize(h,0);
	file.content = HeapAlloc(GetProcessHeap(),8,file.size+1);
	ReadFile(h,file.content,file.size+1,0,0);
	CloseHandle(h);
	return file;
}

TFILE loadFile2(char *name){
	TFILE file;
	file.name = HeapAlloc(GetProcessHeap(),8,strlen(name)+1);
	memcpy(file.name,name,strlen(name));
	HANDLE h = CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(GetLastError() == 2){
		file.f.size = 0;
		CloseHandle(h);
		return file;
	}
	file.f.size = GetFileSize(h,0);
	file.f.content = HeapAlloc(GetProcessHeap(),8,file.f.size+1);
	ReadFile(h,file.f.content,file.f.size+1,0,0);
	CloseHandle(h);
	return file;
}

DATABASE loadFile3(char *name){
	DATABASE d;
	d.fC = 0;
	HANDLE h = CreateFile(name,GENERIC_READ,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	d.name = HeapAlloc(GetProcessHeap(),8,strlen(name)+1);
	memcpy(d.name,name,strlen(name));
	char *fi = HeapAlloc(GetProcessHeap(),8,GetFileSize(h,0)+1);
	ReadFile(h,fi,GetFileSize(h,0),0,0);
	unsigned int strC = 0;
	for(int i = 0;i < GetFileSize(h,0);i++){
		if(fi[i] == '\n'){
			strC++;
		}
	}
	d.f = HeapAlloc(GetProcessHeap(),8,sizeof(WEBFILE)*strC);
	for(int i = 0;i < GetFileSize(h,0);i++){
		unsigned int v = getCmdSz(i,fi);
		d.f[d.fC].content = HeapAlloc(GetProcessHeap(),8,v+1);
		memcpy(d.f[d.fC].content,fi+i,v);
		d.f[d.fC].size = v;
		d.fC++;
		i+=v;
	}
	HeapFree(GetProcessHeap(),8,fi);
	CloseHandle(h);
	return d;
}

unsigned int getInput(char *name){
	int s = strlen(name);
	unsigned char p;
	for(int i = 0;input[i]!='\0';i++){
		if(input[i] == name[p]){
			p++;
			if(p == s){
				return i+2;
			}
		}
		else{
			p = 0;
		}
	}
	return 0;
}

int searchDatabase(char *name,unsigned int C,DATABASE *db){
	for(int i = 0;i < C;i++){
		if(!memcmp(name,db[i].name,strlen(name))){
			return i;
		}
	}
	return -1;
}

void sendPage(char *name){
	printf("%s\n",name);
	unsigned int fI = searchDatabase(name,filesC,(DATABASE*)files);
	if(fI == -1){
		files = HeapReAlloc(GetProcessHeap(),8,files,sizeof(DATABASE)*(filesC+1));
		files[filesC] = loadFile2(name);
		if(files[filesC].f.size == 0){
			return;
		}
		fI = filesC;
		filesC++;
	}
	unsigned int b = 0;
	accounts[clientId].webpage = fI;
	send(client,"HTTP/1.1 200 OK\r\n",17,0);
	send(client,"Content-Type: text/html\r\n",25,0);
	send(client,"Server: TMserver2022\r\n",22,0);
	send(client,"\r\n",2,0);
	if(strlen(files[fI].name)>5&&(!memcmp(files[fI].name+strlen(files[fI].name)-5,".html",5))){
		for(int i = 0;i < files[fI].f.size;i++){
			if(i > files[fI].f.size-9){
				continue;
			}
			if(!memcmp(files[fI].f.content+i,"<tscript=",9)){
				i+=9;
				int sz = 0;
				for(;files[fI].f.content[i+sz]!='>';sz++){}
				for(int i2 = 0;i2 < cmdsOC;i2++){
					if(!memcmp(files[fI].f.content+i,cmdsO[i2].name,strlen(cmdsO[i2].name))){
						switch(cmdsO[i2].type){
						case 1:{
								send(client,files[fI].f.content+b,i-b-9,0);
								send(client,"<p>",3,0);
								int di = searchDatabase(cmdsO[i2].p1,databaseC,database);
								if(di == -1){
									database = HeapReAlloc(GetProcessHeap(),8,database,sizeof(DATABASE)*(databaseC+1));
									database[databaseC] = loadFile3(cmdsO[i2].p1);
									if(database[databaseC].fC == 0){
										break;
									}
									for(int i3 = 0;i3 < database[databaseC].fC;i3++){
										send(client,database[databaseC].f[i3].content,database[databaseC].f[i3].size,0);
										send(client,"<br>",4,0);
									}
									databaseC++;
								}
								else{
									for(int i3 = 0;i3 < database[di].fC;i3++){
										send(client,database[di].f[i3].content,database[di].f[i3].size,0);
										send(client,"<br>",4,0);
									}
								}
								send(client,"<p>",3,0);
								break;
							}
						}
					}
				}
				i += sz + 1;
				b = i;
			}
		}
		send(client,files[fI].f.content+b,files[fI].f.size-b,0);
	}
	else{
		send(client,files[fI].f.content,files[fI].f.size,0);
	}

	send(client,"\r\n\r\n",4,0);
}

PSTRING getPstr(int i,char *script){
	PSTRING result;
	unsigned int s = getCmdSz(i,script);
	result.p = HeapAlloc(GetProcessHeap(),8,s+1);
	memcpy(result.p,script+i,s);
	result.i = i+s+1;
	return result;
}

int main() {
	database = HeapAlloc(GetProcessHeap(),8,sizeof(DATABASE));
	files    = HeapAlloc(GetProcessHeap(),8,sizeof(DATABASE));
	input 	 = HeapAlloc(GetProcessHeap(),8,30000);
	accounts = HeapAlloc(GetProcessHeap(),8,0);
	files[0] = loadFile2("source.html");
	tscriptI = loadFile("source.tsi");
	tscriptO = loadFile("source.tso");
	cmds  = HeapAlloc(GetProcessHeap(),8,sizeof(COMMAND)*(cmdsC+1));
	cmdsO = HeapAlloc(GetProcessHeap(),8,sizeof(COMMAND)*(cmdsOC+1));
	int i = 0;
	int s = 0;
	for(int i2 = 0;i < tscriptI.size;i2++){
		cmds = HeapReAlloc(GetProcessHeap(),8,cmds,sizeof(COMMAND)*(cmdsC+1));
		int sz = 0;
		s = getCmdSz(i,tscriptI.content);	
		cmds[i2].name = HeapAlloc(GetProcessHeap(),8,s+1);	
		for(int i3 = 0;i3 < s;i++,i3++){
			cmds[i2].name[i3] = tscriptI.content[i];
		}
		i++;
		s = getCmdSz(i,tscriptI.content);
		switch(s){
		case 3:
			if(!memcmp(tscriptI.content+i,"ADD",3)){
				cmds[i2].type = 1;
				i+=s+1;
				PSTRING pstr = getPstr(i,tscriptI.content);
				i = pstr.i;
				cmds[i2].p1 = pstr.p;
			}
			break;
		}
		cmdsC++;
	}
	i = 0;
	for(int i2 = 0;i < tscriptO.size;i2++){
		cmdsO = HeapReAlloc(GetProcessHeap(),8,cmdsO,sizeof(COMMAND)*(cmdsOC+1));
		int sz = 0;
		s = getCmdSz(i,tscriptO.content);
		cmdsO[i2].name = HeapAlloc(GetProcessHeap(),8,s+1);
		for(int i3 = 0;i3 < s;i++,i3++){
			cmdsO[i2].name[i3] = tscriptO.content[i];
		}
		i++;
		s = getCmdSz(i,tscriptO.content);
		switch(s){
		case 3:
			if(!memcmp(tscriptO.content+i,"ADD",3)){
				cmdsO[i2].type = 1;
				i+=s+1;
				PSTRING pstr = getPstr(i,tscriptO.content);
				i = pstr.i;
				cmdsO[i2].p1 = pstr.p;
			}
			break;
		}
		cmdsOC++;
	}
	printf("parseComplete\n");
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
    adres.sin_family = AF_INET;
	adres.sin_port = htons(7778);
	adres.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock,(SOCKADDR*)&adres,sizeof(adres));
	for(;;){
		printf("ready\n");
		listen(sock,SOMAXCONN);
		client = accept(sock,0,0);
		printf("busy\n");
		unsigned int packetSize = 0,tp = 1;
		while(packetSize!=tp&&packetSize%1024==0){
			tp = packetSize;
			packetSize += recv(client,input+packetSize,1024,0);
		}
		unsigned int clientIp = 0;
		SOCKADDR adresIp;
		getpeername(client,&adresIp,&lsockaddres);
		clientIp += adresIp.sa_data[2];
		clientIp += adresIp.sa_data[3] << 8;
		clientIp += adresIp.sa_data[4] << 16;
		clientIp += adresIp.sa_data[5] << 24;
		getsockname(client,&adresIp,&lsockaddres);
		clientIp += adresIp.sa_data[2] << 32;
		clientIp += adresIp.sa_data[3] << 40;
		clientIp += adresIp.sa_data[4] << 48;
		clientIp += adresIp.sa_data[5] << 56;
		for(int i = 0;i < accountsC;i++){
			if(accounts[i].ip == clientIp){
				clientId = i;
				goto foundClientId;
			}
		}
		accounts = HeapReAlloc(GetProcessHeap(),8,accounts,sizeof(ACCOUNT)*(accountsC+1));
		accounts[accountsC].ip = clientIp;
		clientId = accountsC;
		accountsC++;
	foundClientId:
		if(!memcmp("POST",input,4)){
			for(int i = 0;i < cmdsC;i++){
				unsigned int v = getInput(cmds[i].name);
				unsigned int s = strlen(input+v);
				switch(cmds[i].type){
				case 1:{
					HANDLE h = CreateFile(cmds[i].p1,GENERIC_WRITE|GENERIC_READ,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
					for(int i2 = 0;i2 < databaseC;i2++){
						if(!memcmp(cmds[i].p1,database[i2].name,strlen(cmds[i].p1)-1)){
							database[i2].f = HeapReAlloc(GetProcessHeap(),8,database[i2].f,sizeof(WEBFILE)*(database[i2].fC+1));
							database[i2].f[database[i2].fC].size = s;
							database[i2].f[database[i2].fC].content = HeapAlloc(GetProcessHeap(),8,s+1);
							memcpy(database[i2].f[database[i2].fC].content,input+v,s);
							database[i2].fC++;
							goto foundDatabase;
						}
					}
					database = HeapReAlloc(GetProcessHeap(),8,database,sizeof(DATABASE)*(databaseC+1));
					database[databaseC].fC = 0;
					database[databaseC].name = HeapAlloc(GetProcessHeap(),8,strlen(cmds[i].p1)+1);
					memcpy(database[databaseC].name,cmds[i].p1,strlen(cmds[i].p1));
					unsigned int fSize = GetFileSize(h,0);
					unsigned int strC = 0;
					if(fSize != -1){
						char *fi = HeapAlloc(GetProcessHeap(),8,fSize+1);
						ReadFile(h,fi,fSize,0,0);
						for(int i = 0;i < fSize;i++){
							if(fi[i] == '\n'){
								strC++;
							}
						}
						for(int i = 0;i < GetFileSize(h,0);i++){
							unsigned int v = getCmdSz(i,fi);
							database[databaseC].f[database[databaseC].fC].content = HeapAlloc(GetProcessHeap(),8,v+1);
							memcpy(database[databaseC].f[database[databaseC].fC].content,fi+i,v);
							database[databaseC].f[database[databaseC].fC].size = v;
							database[databaseC].fC++;
							i+=v;
						}
						HeapFree(GetProcessHeap(),8,fi);
					}
					database[databaseC].f = HeapAlloc(GetProcessHeap(),8,sizeof(WEBFILE)*(strC+1));
					database[databaseC].f[database[databaseC].fC].size = s;
					database[databaseC].f[database[databaseC].fC].content = HeapAlloc(GetProcessHeap(),8,s+1);
					memcpy(database[databaseC].f[database[databaseC].fC].content,input+v,s);
					database[databaseC].fC++;
					databaseC++;
				foundDatabase:
					SetFilePointer(h,GetFileSize(h,0),0,0);
					WriteFile(h,input+v,s,0,0);
					WriteFile(h,"\n",1,0,0);
					CloseHandle(h);
					break;
					}
				}
			}
			sendPage(files[accounts[clientId].webpage].name);
		}
		else{
			char *page;
			int ps = getRequestSize(input+5);
			if(ps){
				page = HeapAlloc(GetProcessHeap(),8,ps+1);
				memcpy(page,input+5,ps);
			}
			else{
				page = HeapAlloc(GetProcessHeap(),8,12);
				memcpy(page,"source.html",11);
			}
			sendPage(page);
		}
		closesocket(client);
		memset(input,0,packetSize);
	}
}
	




 	







