#include <dirent.h>
#include <unistd.h>
#include "network-functions.c"
#include "ii-functions.c"
#include "getcfg.c"
// toss file format is raw msgline, NOT base64

char** dirs;
DIR *tossesdir;
struct dirent *filename;
int filescount=0, i, result;
int MAXFILES=512; // максимальное количество файлов в out/

int comp(const void *a, const void *b) { 
	const char *u = (const char *)a;
	const char *v = (const char *)b;
	return strcmp(u, v);
}

int main() {
	getcfg();
	
	// выделяем ещё памяти для адреса, чтобы засунуть туда u/point
	char* send_adress=(char*)malloc(sizeof(char*)*(strlen(adress)+8));
	
	strcpy(send_adress, adress);
	strcat(send_adress, "u/point");
	
	tossesdir=opendir("out/");
	dirs=(char**)malloc(sizeof(char*)*MAXFILES);
	
	while((filename=readdir(tossesdir))!=NULL) {
		if (
			(strcmp(filename->d_name, ".")!=0) &&
			(strcmp(filename->d_name, "..")!=0)
		) {
			dirs[filescount]=(char*)malloc(sizeof(filename->d_name));
			strcpy(dirs[filescount++], filename->d_name);
		}
	}
	qsort(dirs, filescount, sizeof(char*), comp); // сортируем тоссы; точнее, пытаемся
	
	if (filescount==0) return 0;
	
	char* encoded_authstr=curl_easy_escape(curl, authstr, 0);

	for (i=0;i<filescount;i++) {
		char* tossfname=(char*)malloc(sizeof(char)*(strlen(dirs[i])+5));
		strcpy(tossfname, "out/");
		strcat(tossfname, dirs[i]);
		
		char* rawtext=file_get_contents(tossfname);
		
		if (rawtext==NULL) {
			continue;
		}
		
		char* code=curl_easy_escape(curl, b64c(rawtext), 0);
		free(rawtext);

		char* request=(char*)malloc(sizeof(char)*(strlen(code)+strlen(encoded_authstr)+13));
		strcpy(request, "tmsg=");
		strcat(request, code);
		strcat(request, "&pauth=");
		strcat(request, encoded_authstr);

		curl_free(code);
		
		result=getFile(send_adress, NULL, request);
		printf(" %s: %d\n", tossfname, result);
		
		if(result == 0) {
			// перемещаем отправленное сообщение в sent/ (ну не удалять же его)
			char* newfname=(char*)malloc(sizeof(char)*(strlen(dirs[i])+6));
			strcpy(newfname, "sent/");
			strcat(newfname, dirs[i]);
			
			rename(tossfname, newfname);
			free(newfname);
		}
	}
	// освобождаем память... только вот смысл?
	for (i=0;i<filescount; i++) {
		free(dirs[i]);
	}
	free(dirs);
	curl_free(encoded_authstr);

	return 0;
}
