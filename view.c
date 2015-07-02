#include "ii-functions.c"

int main(int argc, char** argv) {
	char* echoname;
	char* second;
	int i;
	char* rawmsg;
	int start, end;
	
	if (argc>1) {
		echoname=argv[1];

		if (argc==3) {
			second=argv[2];
		}
		else second=NULL;
	} else {
		printf("Usage: viewer <echoarea>\nor viewer <echoarea> len\nor viewer <echoarea> <number> (count from 0)\nor viewer <echoarea> <format>\n    where format is\n    l10 - last 10 messages\n    f5 - first 5 messages\n    r2:4 - from 3rd to 5th (including)\n");
		return 1;
	}

	struct msglist buf=getLocalEcho(echoname);
	int last=(buf.length)-1;

	if (second) {
		if (strcmp(second, "len")==0) {
			printf("%d\n", buf.length);
			return 0;
		} else {
			int r;
			r=sscanf(second, "%d", &start);
			if (r==1) end=start;

			r=sscanf(second, "l%d", &start);
			if (r==1) { start=buf.length-start; end=buf.length-1; }

			r=sscanf(second, "f%d", &end);
			if (r==1) { start=0; end--; }

			r=sscanf(second, "r%d:%d", &start, &end);
		}
	} else {
		start=0;
		end=last;
	}
	
	if (start>end || start<0 || end<0 || end > last) {
		return 1;
	}

	for (i=start;i<=end;i++) {
		printf("n=%d; msgid: %s\n", i, buf.index[i]);
		rawmsg=getRawMsg(buf.index[i]);

		printf("%s\n\n", rawmsg);
	
	}
	return 0;
}
