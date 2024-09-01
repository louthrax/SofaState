#include <cstdio>

#include "encode.h"

#define MIG_BLOCK_SIZE	8192
CharArray		*g_poMIGData;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int main(int argc, char *argv[])
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*pcCurrent;
	int             iRemainingBytes;
    unsigned char   ucC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	vEncode_Init(2048);

	g_poMIGData = new CharArray();
    
    freopen(NULL, "rb", stdin);
    freopen(NULL, "wb", stdout);

    while(fread(&ucC, 1, 1, stdin))
    {
    	g_poMIGData->vAdd(ucC);
    }


	iRemainingBytes = g_poMIGData->iGetSize();
	pcCurrent = g_poMIGData->poGetData();

    fprintf(stderr, "%d\n", iRemainingBytes);

	while(iRemainingBytes)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int			iBytesToWrite;
		int			iCompressedSize;
		CharArray	*poCompressedData;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		iBytesToWrite = iRemainingBytes;
		if(iBytesToWrite > MIG_BLOCK_SIZE) iBytesToWrite = MIG_BLOCK_SIZE;

		poCompressedData = poEncode_Encode(pcCurrent, iBytesToWrite);

		fwrite(&iBytesToWrite, 2, 1, stdout);
		iCompressedSize = poCompressedData->iGetSize();
		fwrite(&iCompressedSize, 2, 1, stdout);

		fwrite(poCompressedData->poGetData(), iCompressedSize, 1, stdout);
        fprintf(stderr, "%d\n", iCompressedSize);

		delete poCompressedData;

		pcCurrent += iBytesToWrite;
		iRemainingBytes -= iBytesToWrite;
	}

	fwrite(&iRemainingBytes, 2, 1, stdout);

	vEncode_Release();
}
