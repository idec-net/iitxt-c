#include <unistd.h>
#include "network-functions.c"
#include "ii-functions.c"

char adress[]="http://ii-net.tk/ii/ii-point.php?q=/";
int bundle_maxsize=20;

static int i, j;

struct msglist messages_difference(struct msglist first, struct msglist second) {
	char** index=NULL;
	int count=0, j, a, found;

	for (j=0;j<first.length;j++) {
		found=0;
		for (a=0; a<second.length; a++) {
			if (strcmp(first.index[j], second.index[a])==0) {
				found=1;
				break;
			}
		}
		if (found!=1) {
			index=(char**)realloc(index, sizeof(char*)*(count+1));
			index[count++]=first.index[j];
		}
	}

	struct msglist result = { index, count };
	return result;
}

void saveBundle (char* echoarea, char* raw_bundle) {
	char** lines=NULL;
	int linescount=0, i;

	char* nextline=strtok(raw_bundle, "\n");
	
	while (nextline!=NULL) {
		printf ("Мы здесь");
		if ((strstr(nextline, ":"))!=NULL) {
			lines=(char**)realloc(lines, sizeof(char*)*(linescount+1));
			lines[linescount++]=nextline;
		} else {
			// printf ("бандл не содержит ':'\n");
		}
		nextline=strtok(NULL, "\n");
	}
	
	for (i=0;i<linescount;i++) {
		char fname[80]="msg/";
		char* next_part=strtok(lines[i], ":");
		char* msgid=next_part;

		strcat(fname, next_part);

		if (access(fname, F_OK)!=-1) {
			printf("E: Файл %s уже существует!\n", fname);
		} else {
			if((next_part=strtok(NULL, ":"))!=NULL) {
				// расшифровываем base64 и сохраняем бандл
				// увы, содержимое этого блока - жуткий костыль =(
				FILE *b64cache=fopen("nextmessage", "w+");
				fwrite(next_part, strlen(next_part), 1, b64cache);
				rewind(b64cache);
				FILE *message=fopen(fname, "w");
				decode(b64cache, message);

				fclose(b64cache);
				fclose(message);
				char echofile[80]="echo/";
				strcat(echofile, echoarea);
				FILE* echo=fopen(echofile, "a");
				fwrite(msgid, strlen(msgid), 1, echo);
				fwrite("\n", 1, 1, echo);
				fclose(echo);
			} else {
				printf("E: бандл %s повреждён\n", msgid);
			}
		}
	}
}

int fetch_messages (char* adress, char** echoesToFetch, int echoesCount) {
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
			struct msglist remote_msglist;
			remote_msglist.index=(char**)malloc(sizeof(char*));

			char* nextmessage;

			remote_msglist.length=0;
			while ((nextmessage=strtok(NULL, "\n"))!=NULL) {
				remote_msglist.index=(char**)realloc(remote_msglist.index, sizeof(char*)*(remote_msglist.length+1));
				
				remote_msglist.index[remote_msglist.length]=(char*)malloc(sizeof(char)*21);
				strcpy(remote_msglist.index[remote_msglist.length++], nextmessage);
			}

			struct msglist local_msglist=getLocalEcho(bundle_echoarea);
			struct msglist difference=messages_difference(remote_msglist, local_msglist);

			if (difference.length>0) {
				int divideCount;
				if (difference.length<=bundle_maxsize) {
					divideCount=1;
				} else {
					if (difference.length%bundle_maxsize==0) divideCount=difference.length/bundle_maxsize;
					else divideCount=difference.length/bundle_maxsize+1;
				}
				
				// следующий цикл отвечает за разделение запросов по 20 сообщений и их выполнение О_о
				char* server_bundle_request;
				int a;
				for (j=0; j<divideCount; j++) {
					server_bundle_request=(char*)malloc(sizeof(char)*(strlen(adress)+3+bundle_maxsize*22));
					// в предыдущей строке, вероятно, может быть утечка

					strcpy(server_bundle_request, adress);
					strcat(server_bundle_request, "u/m");

					for (a=0; a<bundle_maxsize; a++) {
						if (j*bundle_maxsize+a==difference.length) break;
						
						strcat(server_bundle_request, "/");
						strcat(server_bundle_request, difference.index[j*bundle_maxsize+a]);
					}
					
					FILE *bundle_cached=fopen("cache-bundle", "w+");
					if (!bundle_cached) {
						printf("%s\n", "Не могу открыть файл кэша бандла");
					} else {
						// скачиваем бандл сообщений
						int gotBundle=getFile(server_bundle_request, bundle_cached);
						
						int bundle_cache_size=ftell(bundle_cached);
						rewind(bundle_cached); // опять подготовка к чтению (идём к началу)
		
						char* raw_bundle=(char*)malloc(bundle_cache_size);
						fread(raw_bundle, bundle_cache_size, 1, bundle_cached); // friendship^Wfread is magic
						fclose(bundle_cached);
						
						saveBundle(bundle_echoarea, raw_bundle); // а эта функция распарсит бандл и попытается сохранить
					}
					free (server_bundle_request);
				}
			}
		}
	}
}

int main() {
	char** myEchoes=(char**)malloc(sizeof(char*)*2);
	myEchoes[0]=(char*)malloc(sizeof(char)*30);
	myEchoes[1]=(char*)malloc(sizeof(char)*30);

	strcpy(myEchoes[0], "mlp.15");
	strcpy(myEchoes[1], "ii.test.14");
	
	int fetched=fetch_messages(&adress[0], myEchoes, 2);
}
