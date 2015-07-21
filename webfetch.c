#include "network-functions.c"
#include "ii-functions.c"

int bundle_maxsize=20; // скачиваем по 20 собщений за раз

static int i, j;

struct list messages_difference(struct list first, struct list second) {
	struct list result={NULL, 0};
	int j, a, found;

	for (j=0;j<first.length;j++) {
		found=0;
		for (a=0; a<second.length; a++) {
			if (strcmp(first.index[j], second.index[a])==0) {
				found=1;
				break;
			}
		}
		if (found!=1) {
			list_append(&result, first.index[j]);
		}
	}

	return result;
}

struct list saveBundle (char* echoarea, char* raw_bundle) {
	char** lines=NULL;
	int linescount=0, i;
	struct list result={NULL, 0};

	char* nextline=strtok(raw_bundle, "\n");
	
	while (nextline!=NULL) {
		if ((strstr(nextline, ":"))!=NULL) {
			lines=(char**)realloc(lines, sizeof(char*)*(linescount+1));
			lines[linescount++]=nextline;
		} else {
			// printf ("бандл не содержит ':'\n");
		}
		nextline=strtok(NULL, "\n");
	}
	
	for (i=0;i<linescount;i++) {
		char fname[160]="\0";
		strcat(fname, msgdir);

		char* next_part=strtok(lines[i], ":");
		char* msgid=next_part;

		strcat(fname, next_part);

		if (access(fname, F_OK)!=-1) {
			printf("E: Файл %s уже существует!\n", fname);
		} else {
			if((next_part=strtok(NULL, ":"))!=NULL) {
				// расшифровываем base64 и сохраняем бандл
				int saved=savemsg(msgid, echoarea, b64d(next_part));

				if (saved) {
					// если сообщение сохранили, то добавляем его в список
					list_append(&result, msgid);
				}
			} else {
				printf("E: бандл %s повреждён\n", msgid);
			}
		}
	}
	return result;
}

struct list fetch_messages (char* adress, struct list echoesToFetch) {
	char* server_msglist_request;
	struct list saved_messages={NULL, 0}; // список сообщений, которые в итоге будут сохранены

	// инициализируем имена файлов для кэша

	char indexcache_fname[100]="\0";
	char bundlecache_fname[100]="\0";

	strcat(indexcache_fname, datadir);
	strcat(bundlecache_fname, datadir);

	strcat(indexcache_fname, "cache-first");
	strcat(bundlecache_fname, "cache-bundle");

	for (i=0; i<echoesToFetch.length; i++) {
		server_msglist_request=(char*)malloc(sizeof(char)*(strlen(adress)+strlen(echoesToFetch.index[i])+5));
		
		strcpy(server_msglist_request, adress);
		strcat(server_msglist_request, "u/e/");
		strcat(server_msglist_request, echoesToFetch.index[i]);
		
		FILE* cached=fopen(indexcache_fname, "wb+");
		
		if (!cached) {
			printf("Не могу открыть файл кэша\n");
			return saved_messages;
		}
	
		int gotEcho=getFile(server_msglist_request, cached, NULL);
		
		int cache_size=ftell(cached)-1;
		rewind(cached); // Сейчас читать будем
		
		char* raw_echobundle=(char*)malloc(cache_size);
		fread(raw_echobundle, cache_size, 1, cached); // прочитали, дальше начнётся какая-то магия
		fclose(cached);
		
		char* bundle_echoarea=strtok(raw_echobundle, "\n");
		if (bundle_echoarea!=NULL) {
			struct list remote_msglist;
			remote_msglist.index=(char**)malloc(sizeof(char*));

			char* nextmessage;

			remote_msglist.length=0;
			while ((nextmessage=strtok(NULL, "\n"))!=NULL) {
				remote_msglist.index=(char**)realloc(remote_msglist.index, sizeof(char*)*(remote_msglist.length+1));
				
				remote_msglist.index[remote_msglist.length]=(char*)malloc(sizeof(char)*21);
				strcpy(remote_msglist.index[remote_msglist.length], nextmessage);
				remote_msglist.index[remote_msglist.length++][20]='\0';
			}

			struct list local_msglist=getLocalEcho(bundle_echoarea);
			struct list difference=messages_difference(remote_msglist, local_msglist);

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
					
					FILE *bundle_cached=fopen(bundlecache_fname, "w+");
					if (!bundle_cached) {
						printf("Не могу открыть файл кэша бандла\n");
					} else {
						// скачиваем бандл сообщений
						int gotBundle=getFile(server_bundle_request, bundle_cached, NULL);
						
						int bundle_cache_size=ftell(bundle_cached);
						rewind(bundle_cached); // опять подготовка к чтению (идём к началу)
		
						char* raw_bundle=(char*)malloc(bundle_cache_size+1);
						fread(raw_bundle, bundle_cache_size, 1, bundle_cached); // friendship^Wfread is magic
						raw_bundle[bundle_cache_size]='\0';
						fclose(bundle_cached);
						
						struct list bundle_success=saveBundle(bundle_echoarea, raw_bundle); // а эта функция распарсит бандл и попытается сохранить
						if (bundle_success.length>0) {
							list_merge(&saved_messages, &bundle_success);
						}
					}
					free (server_bundle_request);
				}
			}
		}
	}
	return saved_messages;
}

int main() {
	ii_base_init();
	
	struct list fetched=fetch_messages(adress, subscriptions);

	if (fetched.length>0) {
		char newmsg_filename[130];
		strcpy(newmsg_filename, indexdir);
		strcat(newmsg_filename, "newmsg");

		FILE* newmsg=fopen(newmsg_filename, "wb");
		int i;
		for (i=0; i<fetched.length; i++) {
			fputs(fetched.index[i], newmsg);
			fputs("\n", newmsg);
		}
		fclose(newmsg);
	}
}
