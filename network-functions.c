#include <curl/curl.h>

static CURL *curl;
static CURLcode res;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

int getFile(char* request, FILE* outfile) {
	curl=curl_easy_init();

	if (!curl) {
		printf("Ошибка curl, выходим\n");
		return 1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, request);
	printf("fetch %s\n", request);
	
	if (outfile!=NULL) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
	}
	
	res=curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if(res != CURLE_OK) {
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return 1;
	}
	
	return 0;
}
