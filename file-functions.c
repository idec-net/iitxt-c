#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int MAX_FILENAME_SIZE=250;

int fsize(char *name) {
	struct stat st;
	stat(name, &st);
	return st.st_size;
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

void list_append(struct list *elements, char* string) {
	(*elements).index=(char**)realloc((*elements).index, sizeof(char*)*((*elements).length+1));
	(*elements).index[(*elements).length++]=string;
}

void list_merge(struct list *dest, struct list *src) {
	int i;
	for (i=0; i<(*src).length; i++) {
		list_append(dest, (*src).index[i]);
	}
}

struct list split(char* string, char* token) {
	struct list result={NULL, 0};
	char* nextstr=strtok(string, token);

	while(nextstr!=NULL) {
		list_append(&result, nextstr);
		nextstr=strtok(NULL, token);
	}
	
	return result;
}

struct list split_empty(char* string, char token) {
	/*
		версия функции split для односимвольного token
		и, внимание, без выкидывания пустых строк
	*/
	struct list result={NULL, 0};
	char* last_ptr=string;
	char* nextstr;
	char* end_ptr=string+strlen(string);
	int len=0;

	for (char* p=string; p<end_ptr; p++) {
		if (*p==token || p==end_ptr-1) {
			len=p-last_ptr;
			nextstr=(char*)malloc(len+1);
			memcpy(nextstr, last_ptr, len);
			nextstr[len]='\0';
			list_append(&result, nextstr);
			last_ptr=p+1;
		}
	}
	return result;
}

int dir_exists(char* dir) {
	struct stat st;
	
	if (stat(dir, &st) == -1) { // значит каталог не существует
		return 0;
	} else return 1;
}

static void mkdir_p(char* dir, int mode) {
	char tmp[MAX_FILENAME_SIZE];
	char *p=NULL;
	size_t len;
	
	snprintf(tmp, sizeof(tmp), "%s", dir);
	len=strlen(tmp);
	
	if (tmp[len-1]=='/') tmp[len-1]=0;
	for (p=tmp+1; *p; p++) {
		if (*p=='/') {
			*p=0;
			mkdir(tmp, mode);
			*p='/';
		}
	}
	mkdir(tmp, mode);
}
