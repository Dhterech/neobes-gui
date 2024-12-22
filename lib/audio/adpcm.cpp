#include "types.h"

static int clamp16(int32_t val) {
	if(val > 32767) return 32767;
	if(val < -32768) return -32768;
	return val;
}

static const double VAG_f[5][2] = {
        {   0.0        ,   0.0        },
        {  60.0 / 64.0 ,   0.0        },
        { 115.0 / 64.0 , -52.0 / 64.0 },
        {  98.0 / 64.0 , -55.0 / 64.0 },
        { 122.0 / 64.0 , -60.0 / 64.0 }
};

int decode_hist1 = 0;
int decode_hist2 = 0;

void decode_psx(uint8_t *buf, int16_t *outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {
	int predict_nr, shift_factor, sample;
	short scale;
	uint8_t flag;

	int framesin = first_sample/28;

	predict_nr = (int)(buf[framesin*16] >> 4);
	shift_factor = (int)(buf[framesin*16] & 0xF);
	flag = (int)(buf[framesin*16+1]);

	first_sample = first_sample % 28;

	for(int i = first_sample, sample_count = 0; i < first_sample + samples_to_do; i++, sample_count += channelspacing) {
		sample = 0;

		if(flag < 0x7) {
			short sample_byte = (short)(buf[(framesin*16)+2+(i/2)]);
			scale = ((i&1 ? (sample_byte >> 4) : (sample_byte & 0xf)) << 12);

			sample = (int)((scale >> shift_factor) + decode_hist1 * VAG_f[predict_nr][0] + decode_hist2 * VAG_f[predict_nr][1]);
		}

		outbuf[sample_count] = clamp16(sample);
		decode_hist2 = decode_hist1;
		decode_hist1 = sample;
	}
}
