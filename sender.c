#include <curl/curl.h>
#include <dirent.h>
#include <unistd.h>
#include "ii-functions.c"
// toss file format is raw msgline, NOT base64

char authstr[]="your_authstr";
char adress[]="http://ii-net.tk/ii/ii-point.php?q=/u/point";

char** dirs;
DIR *tossesdir;
struct dirent *filename;
int filescount=0, i;
FILE *f;
CURL *curl;
CURLcode res;

int comp(const void *a, const void *b) { 
	const char *u = (const char *)a;
	const char *v = (const char *)b;
	return strcmp(u, v);
}

int main() {
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

	curl=curl_easy_init();
	if (!curl) {
		printf("Ошибка curl, выходим");
		return 1;
	}
	
	curl_easy_setopt(curl, CURLOPT_URL, adress);

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

		char* rawtext=(char*)malloc(sizeof(char)*size1);
		fread(rawtext, size1, 1, f);
		fclose(f);
		
		char* code=b64c(rawtext);

		char* request=(char*)malloc(sizeof(char)*(strlen(code)+strlen(authstr)+13));
		strcpy(request, "tmsg=");
		strcat(request, code);
		strcat(request, "&pauth=");
		strcat(request, authstr);
		
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
		res = curl_easy_perform(curl);
		printf(" %s: %d\n", tossfname, res);
		
		if(res != CURLE_OK) {
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {
			unlink(tossfname);
		}
	}
	curl_easy_cleanup(curl);
	return 0;
}
