#include <stdlib.h>

#include "dynarray.h"

typedef DynamicArray<unsigned char>	CharArray;

#define M_writeByte(b)	m_aucBytes[m_ucBytecount++] = b

static int g_iMaxOffset;

static unsigned char	*m_pucData;
static unsigned char	*m_pucDataEnd;

static unsigned char	m_ucBitData;
static unsigned char	m_ucBitCount;
static unsigned char	m_aucBytes[8];
static unsigned char	m_ucBytecount;

static int				*g_aiHashFirst;
static int				*g_aiHashLast;
static int				*g_aiHashNext;
static int				g_iHashPosition;

static CharArray		*g_poResult;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int uiEncode_FindMatches(unsigned char *position, unsigned char **_match_offset)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiResult;
	unsigned int	maxlength;
	unsigned int	match_length;
	int				iLastHashPos;
	int				iHashPos;
	unsigned char	*current;
	unsigned char	*position2;
	unsigned char	*current2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uiResult = 1;

	if(position >= m_pucData + g_iMaxOffset)
	{
		g_iHashPosition &= (g_iMaxOffset - 1);
		g_aiHashFirst[position[-g_iMaxOffset]] = g_aiHashNext[g_iHashPosition];
		g_aiHashNext[g_iHashPosition] = -1;
	}

	iHashPos = g_aiHashFirst[*position];
	if(iHashPos < 0)
	{
		g_aiHashFirst[*position] = g_aiHashLast[*position] = g_iHashPosition++;
	}
	else
	{
		if(_match_offset)
		{
			uiResult = 1;
			maxlength = m_pucDataEnd - position;

			do
			{
				current = position - (g_iHashPosition - iHashPos);
				if(current >= position) current -= g_iMaxOffset;

				position2 = position;
				current2 = current;
				while((current2 < current + maxlength) && (*position2 == *current2))
				{
					current2++;
					position2++;
				}

				match_length = position2 - position;

				if(match_length >= uiResult)
				{
					*_match_offset = current;
					uiResult = match_length;
				}

				iHashPos = g_aiHashNext[iHashPos];
			} while(iHashPos >= 0);
		}

		iLastHashPos = g_aiHashLast[*position];
		g_aiHashLast[*position] = g_aiHashNext[iLastHashPos] = g_iHashPosition++;
	}

	return uiResult;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vEncode_WriteBit(unsigned char _ucBit)
{
	if(m_ucBitCount == 8)
	{
		g_poResult->vAdd(m_ucBitData);
		g_poResult->vAdd(m_aucBytes, m_ucBytecount);
		m_ucBytecount = 0;
		m_ucBitCount = 0;
	}

	m_ucBitData <<= 1;
	if(_ucBit) m_ucBitData++;
	m_ucBitCount++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vEncode_WriteEliasGamma(unsigned int _uiLength)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiBitMask;
	unsigned int	uiLength2;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	uiLength2 = _uiLength;
	uiBitMask = 1;

	while(uiLength2)
	{
		vEncode_WriteBit(1);
		uiBitMask <<= 1;
		uiLength2--;
		uiLength2 >>= 1;
	}

	vEncode_WriteBit(0);

	_uiLength++;
	while(uiBitMask >>= 1)
	{
		vEncode_WriteBit((_uiLength & uiBitMask) != 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vEncode_EncodeMatch(unsigned int _uiOffset, unsigned int _uilength)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiBitMask;
	unsigned char	ucIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	_uiOffset -= 1;
	_uilength -= 2;

	vEncode_WriteBit(1);

	if(_uiOffset > 127)
	{
		M_writeByte(_uiOffset | 128);
		_uiOffset >>= 7;
		uiBitMask = 8;

		for(ucIndex = 0; ucIndex < 4; ucIndex++)
		{
			vEncode_WriteBit(_uiOffset & uiBitMask);
			uiBitMask >>= 1;
		}
	}
	else
	{
		M_writeByte(_uiOffset);
	}

	vEncode_WriteEliasGamma(_uilength);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vEncode_Init(int _iMaxOffset)
{
	g_iMaxOffset = _iMaxOffset;
	g_aiHashFirst = (int *) malloc(sizeof(int) * 256);
	g_aiHashLast = (int *) malloc(sizeof(int) * 256);
	g_aiHashNext = (int *) malloc(sizeof(int) * g_iMaxOffset);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vEncode_Release()
{
	free(g_aiHashFirst);
	free(g_aiHashLast);
	free(g_aiHashNext);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CharArray * poEncode_Encode(unsigned char *_pucData, unsigned int _uiSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*pucPosition;
	unsigned char	*uiMatchOffset;
	unsigned int	uiMatchLength;
	char			cIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	g_poResult = new CharArray();
	g_iHashPosition = 0;

	memset(g_aiHashFirst, 0xFF, sizeof(int) * 256);
	memset(g_aiHashNext, 0xFF, sizeof(int) * g_iMaxOffset);

	m_pucData = _pucData;
	m_pucDataEnd = _pucData + _uiSize;
	m_ucBytecount = 0;
	m_ucBitCount = 0;

	pucPosition = _pucData;

	while(pucPosition < m_pucDataEnd)
	{
		uiMatchLength = uiEncode_FindMatches(pucPosition, &uiMatchOffset);

		if(uiMatchLength > 1)
		{
			vEncode_EncodeMatch(pucPosition - uiMatchOffset, uiMatchLength);
			pucPosition++;
			
			while(--uiMatchLength)
			{
				uiEncode_FindMatches(pucPosition++, NULL);
			}
		}
		else
		{
			vEncode_WriteBit(0);
			M_writeByte(*pucPosition++);
		}
	}

	vEncode_WriteBit(1);
	M_writeByte(0);
	for(cIndex = 0; cIndex < 16; cIndex++) vEncode_WriteBit(1);
	for(cIndex = 0; cIndex < 17; cIndex++) vEncode_WriteBit(0);

	do
	{
		vEncode_WriteBit(0);
	} while(m_ucBitCount != 1);

	return g_poResult;
}
