#ifndef ENCODE_H
#define ENCODE_H

#include "dynarray.h"

typedef DynamicArray<unsigned char>	CharArray;

void	vEncode_Init(int _iMaxOffset);
CharArray * poEncode_Encode(unsigned char *data, unsigned int data_size);
void	vEncode_Release();

#endif
