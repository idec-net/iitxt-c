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
	char* tags;
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
