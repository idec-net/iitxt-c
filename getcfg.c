static char* adress;
static char* authstr;

static struct list subscriptions;

char __indexdir[]="echo/";
char __msgdir[]="msg/";
char __datadir[]=".local/share/iitxt-c/";
char __tossesdir[]="out/";
char __sentdir[]="sent/";

char homedir[100];
char datadir[110];
char configfile[120];
char msgdir[120];
char indexdir[120];
char tossesdir[120];
char sentdir[120];

char __configfile_default[]="/etc/xdg/iitxt-c/config.default.cfg";
char __configfile_default_fallback[]="config.default.cfg";
char __configtext_default[]="http://ii-net.tk/ii/ii-point.php?q=/\nyour_authstr\nmlp.15\nii.test.14\npipe.2032";

/*
	эта функция отвечает за получение конфигурации, а конкретно
		адреса станции, строки авторизации и подписок;
		имён каталогов для сохранения данных
	также она создаёт недостающие каталоги, если их нет
	а ещё копирует дефолтный конфиг из нескольких мест, если его тоже нет

	без неё не будет ничего работать
*/
void ii_base_init() {
	int i;

	char* __homedir=getenv("HOME");
	if (__homedir==NULL) {
		homedir[0]='\0'; // если у юзера отсутствует домашний каталог, то держим все данные прямо здесь, рядом
	} else {
		strcpy(homedir, __homedir);
		strcat(homedir, "/"); // на всякий случай добавляем слэш
	}

	// заполняем переменные путей нужными значениями
	
	strcpy(datadir, homedir);
	strcat(datadir, __datadir);

	strcpy(configfile, datadir);
	strcat(configfile, "config.cfg");

	strcpy(msgdir, datadir);
	strcat(msgdir, __msgdir);

	strcpy(indexdir, datadir);
	strcat(indexdir, __indexdir);
	
	strcpy(tossesdir, datadir);
	strcat(tossesdir, __tossesdir);

	strcpy(sentdir, datadir);
	strcat(sentdir, __sentdir);
	
	char* dirs_to_check[4]={msgdir, indexdir, tossesdir, sentdir};
	for (i=0; i<4; i++) {
		if (!dir_exists(dirs_to_check[i])) {
			printf("Каталог %s не существует, пытаемся создать...\n", dirs_to_check[i]);
			mkdir_p(dirs_to_check[i], 0700);
		}
	}

	if (access(configfile, R_OK) == -1) {
		printf("Конфиг не существует, пытаемся скопировать дефолтный...\n");
		char* default_config=file_get_contents(__configfile_default);
		if (!default_config) {
			printf("Что ж, не получилось, пробуем из текущей директории...\n");
			default_config=file_get_contents(__configfile_default_fallback);

			if (!default_config) {
				printf("Даже в текущем каталоге нет конфига! Берём из исходника.\n");
				default_config=&__configtext_default[0];
			}
		}
		
		FILE *f=fopen(configfile, "w");
		if (!f) printf("Не получается записать дефолтный конфиг! =(\n");
		fputs(default_config, f);
		fclose(f);
	}

	char* rawfile=file_get_contents(configfile);
	struct list lines=split(rawfile, "\n");

	adress=(lines.length>0) ? lines.index[0] : "http://ii-net.tk/ii/ii-point.php?q=/";
	authstr=(lines.length>1) ? lines.index[1] : "your_authstr";
	
	subscriptions.length=(lines.length>2) ? lines.length-2 : 0;
	subscriptions.index=(subscriptions.length>0) ? &(lines.index[2]) : NULL;
}
