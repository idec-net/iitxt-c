#include <string.h>

static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

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

static void decodeblock( unsigned char *in, unsigned char *out ) {   
	out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
	out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
	out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

static int decode( FILE *infile, FILE *outfile ) {
	int retcode = 0;
	unsigned char in[4];
	unsigned char out[3];
	int v, i, len;

	*in = (unsigned char) 0;
	*out = (unsigned char) 0;
	while( feof( infile ) == 0 ) {
		for( len = 0, i = 0; i < 4 && feof( infile ) == 0; i++ ) {
			v = 0;
			while( feof( infile ) == 0 && v == 0 ) {
				v = getc( infile );
				if( v != EOF ) {
					v = ((v < 43 || v > 122) ? 0 : (int) cd64[ v - 43 ]);
					if( v != 0 ) {
						v = ((v == (int)'$') ? 0 : v - 61);
					}
				}
			}
			if( feof( infile ) == 0 ) {
				len++;
				if( v != 0 ) {
					in[ i ] = (unsigned char) (v - 1);
				}
			}
			else {
				in[i] = (unsigned char) 0;
			}
		}
		if( len > 0 ) {
			decodeblock( in, out );
			for( i = 0; i < len - 1; i++ ) {
				putc( (int) out[i], outfile );
			}
		}
	}
}

char* b64c(char* str) {
	int rawtext_size=sizeof(char)*strlen(str);
	char* encoded=(char*)malloc(rawtext_size*4/3);

	base64_encode(str, rawtext_size, encoded);
	return encoded;
}
