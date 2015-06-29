#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "ii.h"
#include "b64.c"

int fsize(char *name) {
	struct stat st;
	stat(name, &st);
	return st.st_size;
}

struct msglist getLocalEcho(char* echofname) {
	char echofile[80]="echo/";
	strcat(echofile, echofname);

	FILE *file=fopen(echofile, "r");
	char** p;

	if (!file) {
		printf("Не могу прочитать файл %s\n", echofile);
		exit(1);
	}
	int size=fsize(echofile);
	int echocount=size/21;
	p=(char**)malloc(sizeof(char*)*echocount);

	int i;

	for (i=0; i<echocount; i++) {
		p[i]=(char*)malloc(sizeof(char)*21);
	}

	char c;
	// i is current number of lines
	i=0;
	int n=0; // symbol
	while(c=fgetc(file)) {
		if (c==EOF) break;
		if (c!='\n') {
			p[i][n++]=c;
		} else {
			p[i][n]='\0';
			i++;
			n=0;
		}
	}
	
	fclose(file);
	struct msglist result = {p, echocount};
	return result;
}

char* getRawMsg(char* msgid) {
	char msgfile[40]="msg/";
	strcat(msgfile, msgid);
	FILE *file=fopen(msgfile, "r");
	if(!file) {
		printf("Не могу открыть файл %s\n", msgfile);
		exit(1);
	}
	
	int size=fsize(msgfile);
	
	char* result=(char*)malloc(sizeof(char)*size);
	fread(result, size, 1, file);
	fclose(file);

	return result;
}

char* getRepto_from_str(char* tagstring) {
	// not implemented
	char* result=tagstring;
	return result;
}

struct message getMsg(char* msgid) {
	struct message result;
	char* rawmsg=getRawMsg(msgid);
	
	char* nextstr=strtok(rawmsg, "\n");
	result.tags=(nextstr!=NULL)?nextstr:"";
	
	result.repto=getRepto_from_str(result.tags);

	nextstr=strtok(NULL, "\n");
	result.echoarea=(nextstr!=NULL)?nextstr:"null";
	
	nextstr=strtok(NULL, "\n");
	char* msgdate=(nextstr!=NULL)?nextstr:"0";
	sscanf(msgdate, "%lli", &(result.date));

	nextstr=strtok(NULL, "\n");
	result.msgfrom=(nextstr!=NULL)?nextstr:"";
	
	nextstr=strtok(NULL, "\n");
	result.addr=(nextstr!=NULL)?nextstr:"";

	nextstr=strtok(NULL, "\n");
	result.msgto=(nextstr!=NULL)?nextstr:"";

	nextstr=strtok(NULL, "\n");
	result.subj=(nextstr!=NULL)?nextstr:"";
	
	// not implemented

	return result;
}

int savemsg(char* msgid, char* echo, char* text) {
	char msgfile[40]="msg/";
	strcat(msgfile, msgid);

	char echofile[80]="echo/";
	strcat(echofile, echo);

	FILE *f=fopen(msgfile, "w");
	if (f) {
		fputs(text, f);
		fclose(f);
	} else {
		printf("Ошибка записи в файл %s", msgfile);
		return 1;
	}

	f=fopen(echofile, "a");
	if (f) {
		fputs(echo, f);
		fputs("\n", f);
		fclose(f);
	} else {
		printf("Ошибка добавления в файл %s", echofile);
		return 1;
	}
	
	printf("message saved ok: %s", msgid);
	return 0;
}
