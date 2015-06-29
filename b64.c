#include <string.h>

static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static size_t encode24(const void *data, size_t len, char *out) {
	unsigned char udata[3] = {};
	memmove(udata, data, len <= 3 ? len : 3);

	out[0] = charset[udata[0] >> 2];
	out[1] = charset[((udata[0] & 0x03) << 4) | ((udata[1] & 0xf0) >> 4)];
	if (len > 1)
		out[2] = charset[((udata[1] & 0x0f) << 2) | ((udata[2] & 0xc0) >> 6)];
	else
		out[2] = '=';
	if (len > 2)
		out[3] = charset[udata[2] & 0x3f];
	else
		out[3] = '=';
	return len <= 3 ? len : 3;
}

static size_t base64_encode(const void *data, size_t len, void *out) {
	for (size_t pos = 0; pos < (len + 2) / 3; ++pos)
		encode24((char*)data + pos * 3, len - pos * 3, (char*)out + pos * 4);
	return (len + 2) / 3 * 4;
}

char* b64c(char* str) {
	int rawtext_size=sizeof(char)*strlen(str);
	char* encoded=(char*)malloc(rawtext_size*4/3);

	base64_encode(str, rawtext_size, encoded);
	return encoded;
}
