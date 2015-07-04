#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "ii.h"
#include "b64.c"

char adress[100]="http://ii-net.tk/ii/ii-point.php?q=/";

int fsize(char *name) {
	struct stat st;
	stat(name, &st);
	return st.st_size;
}

struct msglist getLocalEcho(char* echoarea) {
	// эта функция меня смущает. Хотя она была написана самой первой в клиенте =)
	
	char echofile[80]="echo/";
	strcat(echofile, echoarea);

	FILE *file=fopen(echofile, "r");

	if (!file) {
		printf("W: Не могу прочитать файл %s\n", echofile);
		return (struct msglist){ NULL, 0 };
	}
	int size=fsize(echofile);
	int echocount=0;

	char* filestring=(char*)malloc(size+1);
	fread(filestring, size, 1, file);
	fclose(file);

	char** p=NULL;
	char* nextmsgid=strtok(filestring, "\n");

	while(nextmsgid!=NULL) {
		p=(char**)realloc(p, sizeof(char*)*(echocount+1));
		p[echocount++]=nextmsgid;
		nextmsgid=strtok(NULL, "\n");
	}
	
	struct msglist result = {p, echocount};
	return result;
}

char* file_get_contents(char* filename) {
	// аналог одноимённой функции в php
	FILE *file=fopen(filename, "r");
	if(!file) {
		printf("Не могу открыть файл %s\n", filename);
		return NULL;
	}
	
	int size=fsize(filename);
	
	char* result=(char*)malloc(size+1);
	fread(result, size, 1, file);
	fclose(file);
	result[size]='\0';
	
	return result;
}

char* getRawMsg(char* msgid) {
	char msgfile[40]="msg/";
	strcat(msgfile, msgid);
	
	return file_get_contents(msgfile);
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
	
	/* not implemented
	 	todo: добавить выделение памяти, дабы избежать переполнения
	 	ну ещё сделать парсинг самого сообщения (опять же, с памятью проверить)
	*/
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
		printf("Ошибка записи в файл %s\n", msgfile);
		return 1;
	}

	f=fopen(echofile, "a");
	if (f) {
		fputs(msgid, f);
		fputs("\n", f);
		fclose(f);
	} else {
		printf("Ошибка добавления в файл %s\n", echofile);
		return 1;
	}
	
	printf("message saved ok: %s\n", msgid);
	return 0;
}
