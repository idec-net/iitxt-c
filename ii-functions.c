/*
Copyright (C) 2015 Виктор Феденёв

Этот файл - часть iitxt-c.

iitxt-c - это свободное программное обеспечение; Вы можете
распространнять и/или модифицировать его в соответствии с
GNU General Public License версии 3.

Вы должны были получить копию Стандартной общественной лицензии GNU
вместе с этой программой. Если это не так, см. <http://www.gnu.org/licenses/>.
*/

struct list {
	char** index;
	int length;
};

struct message {
	struct list tags;
	char* repto;
	char* echoarea;
	long long int date;
	char* msgfrom;
	char* addr;
	char* msgto;
	char* subj;
	char* msg;
};

#include "file-functions.c"
#include "b64.c"
#include "getcfg.c"

const struct message nomessage = {{NULL, 0}, NULL, "none", 0, "", "", "", "", NULL};

struct list getLocalEcho(char* echoarea) {
	char echofile[200]="\0";
	strcat(echofile, indexdir);
	strcat(echofile, echoarea);

	char* filestring=file_get_contents(echofile);

	if (!filestring) {
		printf("Эха, наверное, пуста: %s\n", echoarea);
		return (struct list){ NULL, 0 };
	}
	
	return split(filestring, "\n");
}

char* getRawMsg(char* msgid) {
	char msgfile[160]="\0";
	strcat(msgfile, msgdir);
	strcat(msgfile, msgid);
	
	return file_get_contents(msgfile);
}

struct list parseTags(char* tagstring) {
	struct list result={NULL, 0};

	result=split(tagstring, "/");
	if (result.length%2 == 1) {
		result.length--;
		free(result.index[result.length-1]);
	}

	return result;
}

char* getRepto_from_tags(struct list tags) {
	int i;
	if (tags.length <= 1) return NULL; // так вменяемого результата точно не будет

	for (i=0; i<tags.length-1; i+=2) {
		if (strcmp(tags.index[i], "repto") == 0) {
			// выхода за границы массива не будет из-за условия цикла i<tags.length-1
			return tags.index[i+1];
		}
	}
	return NULL;
}

struct message parseMessage(char* rawmsg) {
	struct message result;
	struct list strings=split(rawmsg, "\n");

	if (strings.length >= 8) {
		// значит сообщение, наверное, нормальное
		// парсим
		
		int last=strings.length-1;
		int i;
		// делаем заготовку для пустой строки. \0 нужен для strcat, чтобы не лезть куда попало
		char* full=(char*)malloc(1);
		*full='\0';
		int last_length=1;

		for (i=7; i<strings.length; i++) {
			last_length += strlen(strings.index[i])+1;
			full=(char*)realloc(full, last_length+1);
			strcat(full, strings.index[i]);

			if (i!=last) strcat(full, "\n"); // но если строка последняя, то перенос не добавляем
			else full[last_length-1] = '\0';
		}

		if (strlen(full)==0) {
			free(full);
			result.msg="no message";
		} else {
			result.msg=full;
		}
		
		result.tags=parseTags(strings.index[0]);
		result.repto=getRepto_from_tags(result.tags);
		result.echoarea=strings.index[1];
		sscanf(strings.index[2], "%lli", &(result.date));
		result.msgfrom=strings.index[3];
		result.addr=strings.index[4];
		result.msgto=strings.index[5];
		result.subj=strings.index[6];
	} else {
		result = nomessage;
	}

	return result;
}

int savemsg(char* msgid, char* echo, char* text) {
	char msgfile[160]="\0";
	strcat(msgfile, msgdir);
	strcat(msgfile, msgid);

	char echofile[200]="\0";
	strcat(echofile, indexdir);
	strcat(echofile, echo);

	FILE *f=fopen(msgfile, "w");
	if (f) {
		fputs(text, f);
		fclose(f);
	} else {
		printf("Ошибка записи в файл %s\n", msgfile);
		return 0;
	}

	f=fopen(echofile, "a");
	if (f) {
		fputs(msgid, f);
		fputs("\n", f);
		fclose(f);
	} else {
		printf("Ошибка добавления в файл %s\n", echofile);
		return 0;
	}
	
	printf("msg ok: %s\n", msgid);
	return 1;
}
