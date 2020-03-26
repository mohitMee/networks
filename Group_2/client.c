#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include <netinet/in.h>
 #include<arpa/inet.h>
#include<stdlib.h>

#define PORT 28223
#define PACK 512

void sendmesg(char* str,int sock){
	int numpack = (strlen(str)-1)/PACK +1;
	write(sock,&numpack,sizeof(int));
	char* msg = (char *)malloc(numpack*PACK);
	strcpy(msg,str);
	for(int i=0;i<numpack;++i){
		int n=write(sock,msg,PACK);
		msg  += PACK;	
	}
}


char* readmesg(int sock){
	int n;	
	read(sock,&n,sizeof(int));
	char* str = (char *)malloc(n*PACK);	 
	char * tmp = str;
	for(int i=0;i<n;++i){	
		read(sock,str,PACK);
		str += PACK;
	}
	return tmp;
}

void usr(int sock){
	printf("For mini-statement type MINI\n For balance-enquiry type BAL\n To close connection type EXIT\n");
	while(1){	
	char inp[512]={0};
	scanf("%s",inp);
	sendmesg(inp,sock);
	if(strcmp(inp,"EXIT")==0) break;
	char* str = readmesg(sock);	
	while(*str!='\0') printf("%c",*str++);
	}
	close(sock); 
}

void plc(int sock){
	
	char* str = readmesg(sock);
	while(*str!='\0') printf("%c",*str++);	
	char buff[64]={0};	
	while(1){
		printf("Enter customer name for mini statement\n");
		scanf("%s",buff);	
		sendmesg(buff,sock);
		if(strcmp(buff,"EXIT")==0){
		close(sock);
		break;	
		}
		char* rep = readmesg(sock);
		 while(*rep!='\0') printf("%c",*rep++);
	}	
	
	return;
}
void adm(int sock){
	while(1){
	char buff[128] = {0};	
	char* str = readmesg(sock);
	while(*str!='\0') printf("%c",*str++);
	scanf("%s",buff);
	sendmesg(buff,sock);
	if(strcmp(buff,"EXIT")==0) break;
	str = readmesg(sock);
	while(*str!='\0') printf("%c",*str++);
	memset(buff,0,sizeof(buff));	
	scanf("%s",buff);
	sendmesg(buff,sock);
	str = readmesg(sock);
	while(*str!='\0') printf("%c",*str++);
	memset(buff,0,sizeof(buff));	
	scanf("%s",buff);
	sendmesg(buff,sock);
	str = readmesg(sock);
	while(*str!='\0') printf("%c",*str++);
	}
	close(sock);
	return;
}

int main(){
	
	int sock;
	char buf[512] = {0};
	if((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		printf("Unable to create socket\n");
		return 0;	
		}
 	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT); 
	
	if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0){
		printf("Invalid Address\n");
	       return 0;
	}
	 
	 if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ==-1) 
    { 
        printf("Connection failed \n"); 
        return 0; 
    }
	char* retun;
	retun = readmesg(sock);
	while(*retun!='\0') printf("%c",*retun++);
	memset(buf,0,sizeof(buf));
	scanf("%s",buf);
	sendmesg(buf,sock);
	retun = readmesg(sock);
	while(*retun!='\0') printf("%c",*retun++);	
	memset(buf,0,sizeof(buf));
	scanf("%s",buf);
	sendmesg(buf,sock);
	retun = readmesg(sock);
	if(strcmp(retun,"INVALID")==0){
		printf("INVALID CREDENTIAL\n");
		close(sock);	
	}
	if(strcmp(retun,"USER")==0){
		usr(sock);	
	}
	if(strcmp(retun,"POLICE")==0){
		plc(sock);	
	}
	if(strcmp(retun,"ADMIN")==0){
		adm(sock);	
	}
	
	
return 0;
}
