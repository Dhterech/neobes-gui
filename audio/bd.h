#ifndef BD_H
#define BD_H

#include "types.h"

extern int decode_hist1;
extern int decode_hist2;

void decode_psx(uint8_t *buf, int16_t *outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do);

int getbdlen(char *bd);
int getnsamplesfrombdlen(int len);

#endif // BD_H
