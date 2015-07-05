static char* adress;
static char* authstr;

static struct msglist subscriptions;

void getcfg() {
	char* rawfile=file_get_contents("config.cfg");
	struct msglist lines=split(rawfile, "\n");

	adress=(lines.length>0) ? lines.index[0] : "http://ii-net.tk/ii/ii-point.php?q=/";
	authstr=(lines.length>1) ? lines.index[1] : "your_authstr";
	
	subscriptions.length=(lines.length>2) ? lines.length-2 : 0;
	subscriptions.index=(subscriptions.length>0) ? &(lines.index[2]) : NULL;
}
