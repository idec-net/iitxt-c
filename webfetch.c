#include "network-functions.c"
#include "ii-functions.c"

char adress[]="http://ii-net.tk/ii/ii-point.php?q=/";
static int i;

int fetch_messages (char* adress, char** echoesToFetch, int echoesCount) {
	int echoesArray_length=0;

	for (i=0; i<echoesCount; i++) {
		echoesArray_length+=strlen(echoesToFetch[i]);
	}

	char* server_msglist_request;
	
	for (i=0; i<echoesCount; i++) {
		server_msglist_request=(char*)malloc(sizeof(char)*(strlen(adress)+strlen(echoesToFetch[i])+5));
		
		strcpy(server_msglist_request, adress);
		strcat(server_msglist_request, "u/e/");
		strcat(server_msglist_request, echoesToFetch[i]);
		
		FILE* cached=fopen("cache-first", "wb+");
		
		if (!cached) {
			printf("Не могу открыть файл кэша\n");
			exit(1);
		}
	
		int gotEcho=getFile(server_msglist_request, cached);
		
		int cache_size=ftell(cached);
		rewind(cached); // Сейчас читать будем
		
		char* raw_echobundle=(char*)malloc(cache_size);
		fread(raw_echobundle, cache_size, 1, cached); // прочитали, дальше начнётся какая-то магия
		fclose(cached);
		
		char* bundle_echoarea=strtok(raw_echobundle, "\n");
		if (bundle_echoarea!=NULL) {
			int ea_textsize=strlen(bundle_echoarea)+1;
			int remote_msglist_size=cache_size-ea_textsize;

			// todo: дальше парсим айдишники в структуру msglist
		}
		
		free(server_msglist_request);
	}
}

int main() {
	char** myEchoes=(char**)malloc(sizeof(char*)*2);
	myEchoes[0]=(char*)malloc(sizeof(char)*30);
	myEchoes[1]=(char*)malloc(sizeof(char)*30);

	strcpy(myEchoes[0], "ii.test.14");
	strcpy(myEchoes[1], "mlp.15");
	
	int fetched=fetch_messages(&adress[0], myEchoes, 2);
}
