struct msglist {
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
