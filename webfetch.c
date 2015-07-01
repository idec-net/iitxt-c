#include "network-functions.c"
#include "ii-functions.c"

char adress[]="http://ii-net.tk/ii/ii-point.php?q=/";
int bundle_maxsize=20;

static int i, j;

struct intarr {
	int* numbers;
	int size;
};

struct intarr messages_difference(struct msglist first, struct msglist second) {
	int* indexes=NULL;
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
			count++;
			indexes=realloc(indexes, sizeof(int)*count);
			indexes[count-1]=j;
		}
	}

	struct intarr result = { indexes, count };
	return result;
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
			struct intarr difference=messages_difference(remote_msglist, local_msglist);

			if (difference.size>0) {
				int divideCount;
				if (difference.size<=bundle_maxsize) {
					divideCount=1;
				} else {
					if (difference.size%bundle_maxsize==0) divideCount=difference.size/bundle_maxsize;
					else divideCount=difference.size/bundle_maxsize+1;
				}
				
				int a;
				int** divided=(int**)malloc(sizeof(int*)*divideCount);
				for (j=0; j<divideCount; j++) {
					divided[j]=(int*)malloc(sizeof(int)*bundle_maxsize);
					for (a=0; a<bundle_maxsize; a++) {
						if (divideCount*bundle_maxsize+a == difference.size) break;
						divided[j][a]=difference.numbers[j*bundle_maxsize+a];
					}
				}
				for (j=0; j<divideCount; j++) {
					// for (a=0; a<bundle_maxsize; a++) {
					//	if (j*bundle_maxsize+a==difference.size) break;
					//	printf ("%d\n", divided[j][a]);
					// }
					a=(difference.size-j*bundle_maxsize < bundle_maxsize) ? difference.size-j*bundle_maxsize : bundle_maxsize;

					char* server_bundle_request=(char*)malloc(sizeof(char)*(strlen(adress)+21*a+a));
					// в предыдущей строке, вероятно, может быть утечка

					strcpy(server_bundle_request, adress);
					strcat(server_bundle_request, "u/m");

					for (a=0; a<bundle_maxsize; a++) {
						if (j*bundle_maxsize+a==difference.size) break;
						
						strcat(server_bundle_request, "/");
						strcat(server_bundle_request, remote_msglist.index[divided[j][a]]);
					}
					// здесь опять будет *что-то
					
					FILE *bundle_cached=fopen("cache-bundle", "w+");
					if (!bundle_cached) {
						printf("%s\n", "Не могу открыть файл кэша бандла");
					} else {
						// скачиваем бандл сообщений
						int gotBundle=getFile(server_bundle_request, bundle_cached);

						fclose(bundle_cached);
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

	strcpy(myEchoes[0], "ii.test.14");
	strcpy(myEchoes[1], "mlp.15");
	
	int fetched=fetch_messages(&adress[0], myEchoes, 2);
}
