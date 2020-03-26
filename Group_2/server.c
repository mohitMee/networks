#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
 #include<time.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <math.h>
#define PORT 28223
#define PACK 512

 


char* fil2str(FILE *fp){	
	
	fseek(fp, 0, SEEK_END);	
	int n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(n == 0) return NULL;
	char *str = (char *)malloc((n+1)*sizeof(char));
	fread(str, sizeof(char), n, fp);
	str[n] = 0;
	return str;
	}



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
	str = tmp;
	while(*tmp!='\0') printf("%c",*tmp++);
	return str;
}

long isvalid(char* amt,char* ent,char* tok){
	char* end;	
	if(*ent=='C'){
		long num1 = strtol(amt,&end,10) + strtol(tok,&end,10);	
		return num1;
	}
	else{
		long num1= strtol(tok,&end,10);	
		long num2 = strtol(amt,&end,10);
		num1 = num1 - num2;
		return num1;
	}
}



void ministat(char* un,int socket){
	FILE* fp;
	fp = fopen(un,"r");
	if(fp==NULL){
		sendmesg("Customer doesn't Exist\n",socket);
		return;	
	}
	char* str = fil2str(fp);
	fclose(fp);
	char* ptr;
	char delim[2] = {',','\n'};
	char *tmp = strtok_r(str,delim,&ptr);
	char buffer[2048]={0};
	int i=0;
	char* prevbal = "0";
	do{
	while(*tmp!='\0') buffer[i++] = *tmp++ ;
	buffer[i++]=' ';	
	tmp = strtok_r(NULL,delim,&ptr);	 	
	while(*tmp!='\0') buffer[i++] = *tmp++ ;
	buffer[i++]=' ';
	tmp = strtok_r(NULL,delim,&ptr);
	long int diff = abs(isvalid(tmp,"D",prevbal));	
	char buf[256] = {0};
	int j=0;
	while(diff){
		buf[j++]=diff%10 + '0';
		diff = diff/10;	
	}
	j--;
	while(j>=0){
		buffer[i++] = buf[j--];
	}	
	buffer[i++]='\n';
	}
	while((tmp=strtok_r(NULL,delim,&ptr))!=NULL);
	sendmesg(buffer,socket);
	return;
}

void Alldet(int socket){
	FILE* fp;
	fp = fopen("login.txt","r");
	char* str = fil2str(fp);
	fclose(fp);
	char delim[2] = {',', '\n'};
	 char *ptr;
	char* token1 = strtok_r(str,delim,&ptr);
	char* token2;
	char buffer[2048]={0};
	int i=0;
	do{
		  token2 =  strtok_r(NULL,delim,&ptr);
		token2 = strtok_r(NULL,delim,&ptr);
		if(strcmp(token2,"P")==0) continue;
		else if(strcmp(token2,"A")==0) continue;
		char* tmp = token1;
		 while(*tmp!='\0'){
			buffer[i]=*tmp;
			++i;	
			++tmp;
	}
		buffer[i]=' ';
		buffer[i+1]=' ';
		i = i+2;			
		FILE* fp2;
		fp2 = fopen(token1,"r");
		char* str2 = fil2str(fp2);
		fclose(fp2);
		char* point;
		strtok_r(str2,delim,&point);
		char* tok1;	
		do{
			strtok_r(NULL,delim,&point);	
			tok1=strtok_r(NULL,delim,&point);
		}
		while(strtok_r(NULL,delim,&point)!=NULL);
		 while(*tok1!='\0'){
			buffer[i]=*tok1;
			++i;	
			++tok1;
	}
		buffer[i]='\n';
		i = i+1;		
	}
	while((token1=strtok_r(NULL,delim,&ptr))!=NULL);
	sendmesg(buffer,socket);

}
void update(int sock,char* un,char* ent,char* amt){
	if((strcmp(ent,"C")!=0)&&(strcmp(ent,"D")!=0)){
		sendmesg("Fill the entries in given format\n",sock);
		return;	
	}	
	FILE* fp = fopen(un,"r");
	if(fp==NULL){
		sendmesg("Wrong customer name\n",sock);
		return;	
	}
	char* str = fil2str(fp);
	fclose(fp);
	char delim[2] = {',','\n'};
	char* ptr;
	char* tok = strtok_r(str,delim,&ptr);
	do{
		strtok_r(NULL,delim,&ptr);
		tok = strtok_r(NULL,delim,&ptr);	
	}
	while(strtok_r(NULL,delim,&ptr)!=NULL);	
	long newamt = isvalid(amt,ent,tok);
	if(newamt<0){
	sendmesg("NOT SUFFICIENT BALANCE\n",sock);
	return;	
	}
	time_t t = time(NULL);
  	struct tm t1 = *localtime(&t);
	char* retun = (char *)malloc(40);
	sprintf(retun,"%d/%d/%d,%c,%ld\n",t1.tm_mday,t1.tm_mon+1,t1.tm_year+1900,*ent,newamt);
	fp = fopen(un, "a");
	fwrite(retun, sizeof(char), strlen(retun), fp);
	fclose(fp);
	sendmesg("UPDATED SUCCESSFULLY\n",sock);
	return;
}


void getBAL(char* un,int socket){
		FILE* fp;
	fp = fopen(un,"r");
	if(fp==NULL){
		sendmesg("Customer doesn't Exist\n",socket);
		return;	
	}
	char* str = fil2str(fp);
	fclose(fp);
	char buffer[2048]={0};
	char* temp,*point;
	char delim[2]={',','\n'};
	temp=strtok_r(str,delim,&point);
	char* tok1;	
		do{
			strtok_r(NULL,delim,&point);	
			tok1=strtok_r(NULL,delim,&point);
		}
		while(strtok_r(NULL,delim,&point)!=NULL);
		int i=0;
		 while(*tok1!='\0'){
			buffer[i]=*tok1;
			++i;	
			++tok1;
	}
		buffer[i]='\n';

	sendmesg(buffer,socket);
	return ;
}

void usr(char* un,int sock){
	while(1){
	char* tmp = readmesg(sock);
	if(strcmp(tmp,"MINI")==0)  ministat(un,sock);
	else if(strcmp(tmp,"BAL")==0)  getBAL(un,sock);
	else if(strcmp(tmp,"EXIT")==0) break;
	else sendmesg("Please enter valid input\n",sock);
	}
	close(sock);
	return;
}



void plc(int socket){
	 Alldet(socket);
	while(1){	
	char* str=readmesg(socket);
	if(strcmp(str,"EXIT")==0){
		close(socket);
		break;	
	}
	ministat(str,socket);
	
	}
	return;
}


void adm(int sock){
	while(1){
	sendmesg("Customer name:",sock);	
	char* un = readmesg(sock);
	if(strcmp(un,"EXIT")==0) break;
	sendmesg("What would you like to do(C/D)\n",sock);
	char* ent = readmesg(sock);
	sendmesg("amount in rupees\n",sock);
	char* amt = readmesg(sock);
	update(sock,un,ent,amt);
	}
	close(sock);
	return;
}


int othenticate(char* un,char* ps){
	FILE* fp;
	fp = fopen("login.txt","r");
	char* str = fil2str(fp);
	fclose(fp);
	char delim[2] = {',', '\n'};
	 char *ptr;
	char* token = strtok_r(str,delim,&ptr);
	do{
		
		if(strcmp(un,token)==0){
		token =  strtok_r(NULL,delim,&ptr);
		if(strcmp(ps,token)==0){
			token = strtok_r(NULL,delim,&ptr);
			if(strcmp(token,"C")==0) return 0;
			else if(strcmp(token,"P")==0) return 1;
			else return 2;
		}
		else token = strtok_r(NULL,delim,&ptr);	
		}
		else{	
		token = strtok_r(NULL,delim,&ptr);	
		token = strtok_r(NULL,delim,&ptr);	
		}	
	}
	while((token=strtok_r(NULL,delim,&ptr))!=NULL);
	return -1;
}

void talk(int new_socket){
	sendmesg("      Welcome To Our Bank\n\nusername:",new_socket);
	char* un = readmesg(new_socket);
	sendmesg("password:",new_socket);
	char* ps = readmesg(new_socket);
	int k = othenticate(un,ps);
	 if(k==-1){ 
		sendmesg("INVALID",new_socket);	
		close(new_socket);
		}
	else if(k==0){ 
		sendmesg("USER",new_socket);	
		usr(un,new_socket);
	}
	else if(k==1){ 
		sendmesg("POLICE",new_socket);		
		 plc(new_socket);
	}
	else if(k==2){ 
		sendmesg("ADMIN",new_socket);	
		 adm(new_socket);
	}
}


int main(){

	int server_fd,new_socket,valread;
	int opt = 1; 
 
	if((server_fd = socket(AF_INET,SOCK_STREAM,0))==0){
		printf("socket failed\n");	
		return 0;
	}
	
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	 if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))==-1) 
    { 
        printf("bind failed"); 
        return 0;
    } 
	  
	if(listen(server_fd,3)==-1){
		printf("listen\n");
		return 0;

	}
 	while(1){ 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))==-1) 
    { 
        printf("accept\n"); 
        return 0;
    }	
	int k = fork();
	if(k==-1){
		sendmesg("Error\n",new_socket);
		close(new_socket);
	}
	else if(k==0){
		talk(new_socket);	
		close(new_socket);
		break;
	}
}
return 0;
}
