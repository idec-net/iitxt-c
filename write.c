#include <time.h>
#include <assert.h>
#include "ii-functions.c"

char template[100];
char full_msgfile[200];
char msgfile[40];
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

	strcat(full_msgfile, tossesdir);
	strftime(msgfile, 60, "%G%m%d%H%M%S.toss", mytime);
	strcat(full_msgfile, msgfile);

	FILE *f=fopen(full_msgfile, "w");
	if (!f) {
		printf("Не могу открыть файл %s для записи\n", full_msgfile);
		exit(1);
	}
	fputs(text, f);
	fclose(f);

	execl("/usr/bin/vim", "vim", full_msgfile, (char*)0);
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
	
	ii_base_init();

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
