#include <dirent.h>
#include <unistd.h>
#include "network-functions.c"
#include "ii-functions.c"
// toss file format is raw msgline, NOT base64

char authstr[]="your_authstr";

char** dirs;
DIR *tossesdir;
struct dirent *filename;
int filescount=0, i, result;
FILE *f;

int comp(const void *a, const void *b) { 
	const char *u = (const char *)a;
	const char *v = (const char *)b;
	return strcmp(u, v);
}

int main() {
	strcat(adress, "u/point");
	tossesdir=opendir("out/");
	dirs=(char**)malloc(sizeof(char*)*512);

	while((filename=readdir(tossesdir))!=NULL) {
		if (
			(strcmp(filename->d_name, ".")!=0) &&
			(strcmp(filename->d_name, "..")!=0)
		) {
			dirs[filescount]=(char*)malloc(sizeof(filename->d_name));
			strcpy(dirs[filescount++], filename->d_name);
		}
	}
	qsort(dirs, filescount, sizeof(dirs[0]), comp);
	
	if (filescount==0) return 0;
	
	for (i=0;i<filescount;i++) {
		char* tossfname=(char*)malloc(sizeof(char)*(strlen(dirs[i])+5));
		strcpy(tossfname, "out/");
		strcat(tossfname, dirs[i]);
		free(dirs[i]);
		
		f=fopen(tossfname, "r");
		if (f==NULL) {
			printf("Не могу открыть файл %s\n", tossfname);
			continue;
		}
		
		int size1=fsize(tossfname);

		char* rawtext=(char*)malloc(size1+1);
		fread(rawtext, size1, 1, f);
		fclose(f);
		
		char* code=b64c(rawtext);

		char* request=(char*)malloc(sizeof(char)*(strlen(code)+strlen(authstr)+13));
		strcpy(request, "tmsg=");
		strcat(request, code);
		strcat(request, "&pauth=");
		strcat(request, authstr);
		
		result=getFile(adress, NULL, request);
		printf(" %s: %d\n", tossfname, result);
		
		if(result == 0) {
			unlink(tossfname);
		}
	}
	return 0;
}
