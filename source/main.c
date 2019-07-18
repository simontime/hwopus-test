#include <stdio.h>
#include <stdlib.h>

#include <switch.h>

#define SWAP(x) (((x>>24)&0xff)|((x>>8)&0xff00)|((x<<8)&0xff0000)|((x<<24)&0xff000000))

int main()
{	
	romfsInit();
	
	FILE *in = fopen("romfs:/mnf.opus", "rb");
	fseek(in, 0, SEEK_END);
	int sz = ftell(in);
	rewind(in);
	
	u8 *buf = malloc(sz);
	fread(buf, 1, sz, in);
	fclose(in);
	
	audoutInitialize();
	audoutStartAudioOut();
	
	HwopusDecoder dec;
	hwopusDecoderInitialize(&dec, 48000, 2);
	
	u8 *end = buf + sz;
	
	while (buf < end)
	{
		s16 abuf[0x4000];
		
		s32 size = SWAP(*(s32 *)buf) + 8, outsize, outsmp;
		
		hwopusDecodeInterleaved(&dec, &outsize, &outsmp, buf, size, abuf, 0x4000);
		
		buf += size;
		
		AudioOutBuffer *ptr = NULL, ao = 
		(AudioOutBuffer) {
			.next        = NULL,
			.buffer      = abuf,
			.buffer_size = outsmp * 4,
			.data_size   = outsmp * 4,
			.data_offset = 0
		};

		audoutPlayBuffer(&ao, &ptr);
	}

	hwopusDecoderExit(&dec);

	romfsExit();
	
	audoutStopAudioOut();
	audoutExit();
	
	return 0;
}