#include <time.h>
#include <unistd.h>
#include "ii-functions.c"

char template[100];
char msgfile[50];
char subj2[250];

char* frmSubj(char* subj) {
	if (strncmp("Re: ", subj, 4)==0) {
		return subj;
	} else {
		strcat(subj2, "Re: ");
		strcat(subj2, subj);
		return &subj2[0];
	}
}

void edit (char* text) {
	time_t rawtime;
	struct tm * mytime;
	time(&rawtime);
	mytime=localtime(&rawtime);
	
	strftime(msgfile, 60, "out/%G%m%d%H%M%S.toss", mytime);

	FILE *f=fopen(msgfile, "w");
	if (!f) {
		printf("Не могу открыть файл %s для записи\n", msgfile);
		exit(1);
	}
	fputs(text, f);
	fclose(f);

	execl("/usr/bin/vim", "vim", msgfile, (char*)0);
}

void writeNew (char* echoarea) {
	strcat(template, echoarea);

	strcat(template, "\nAll\n...\n\n");
	edit(&template[0]);
}

void answer (char* echoarea, char* msgid) {
	struct message msg=getMsg(msgid);

	strcat(template, echoarea);
	strcat(template, "\n");
	strcat(template, msg.msgfrom);
	strcat(template, "\n");
	strcat(template, frmSubj(msg.subj));
	strcat(template, "\n\n@repto:");
	strcat(template, msgid);
	strcat(template, "\n");

	edit(template);
}

int main (int argc, char** argv) {
	if (argc==1) {
		printf("Usage: write <echoarea> <msg number>\n");
		return 1;
	}
	
	char* echoarea=argv[1];

	if (argc==2) {
		writeNew(echoarea);
	} else {
		int number=0;
		struct msglist msgids=getLocalEcho(echoarea);
		sscanf(argv[2], "%d", &number);
		assert(number>=0 && number<=((msgids.length)-1));
		
		answer(echoarea, msgids.index[number]);
	}
}
