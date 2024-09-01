#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

template<class el, int basesize = 128>
class DynamicArray
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	DynamicArray();
	DynamicArray(const DynamicArray &_roArray);
	~						DynamicArray();

	DynamicArray &operator	=(const DynamicArray &_roArray);
	DynamicArray &operator	+=(const el &_roItem);
	DynamicArray &operator	+=(const DynamicArray &_roArray);

	el &operator			[](int _iIndex);
	const el &operator		[](int _iIndex) const;
	void					vAdd(const el &_roItem);
	void					vAdd(const el *_poItems, int _iSize);
	el						*poAdd(int _iSize);
	int						iGetSize() const;
	void					vResize(int _iSize);
	void					vSetSize(int _iSize);
	void					vClear();
	void					vDeleteAndClear();
	void					vFreeAndClear();
	void					vDelete(int _iPos);
	const el				*poGetData() const;
	el						*poGetData();
	void					vSetCanShrink(bool _bCanShrink);
	void					vInit();
	size_t					iWriteToFile(FILE *_poFile) const;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	el					*m_poData;
	int					m_iSize;
	int					m_iAllocatedSize;
	const static int	m_siBaseSize = basesize;
	bool				m_bCanShrink;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
DynamicArray<el, basesize>::DynamicArray()
{
	vInit();
}

template<class el, int basesize>

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DynamicArray<el, basesize>::vInit()
{
	m_iSize = 0;
	m_iAllocatedSize = 0;
	m_poData = NULL;
	m_bCanShrink = true;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
DynamicArray<el, basesize>::~DynamicArray()
{
	free(m_poData);
}

template<class el, int basesize>

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DynamicArray<el, basesize>::DynamicArray(const DynamicArray &_roArray)
{
	vInit();
	(*this) = _roArray;
}

template<class el, int basesize>

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DynamicArray<el, basesize> &DynamicArray<el, basesize>::operator=(const DynamicArray<el, basesize> &_roArray)
{
	if(this == &_roArray) return *this;

	vResize(_roArray.m_iSize);
	memcpy(m_poData, _roArray.m_poData, sizeof(el) * _roArray.m_iSize);
	return *this;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
DynamicArray<el, basesize> &DynamicArray<el, basesize>::operator+=(const DynamicArray<el, basesize> &_roArray)
{
	vAdd(_roArray.poGetData(), _roArray.iGetSize());
	return *this;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
int DynamicArray<el, basesize>::iGetSize() const
{
	return m_iSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vSetCanShrink(bool _bCanShrink)
{
	m_bCanShrink = _bCanShrink;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vResize(int _iSize)
{
	if(_iSize == 0)
	{
		if(m_bCanShrink)
		{
			free(m_poData);
			m_poData = NULL;
			m_iAllocatedSize = 0;
		}
	}
	else if(_iSize > m_iAllocatedSize)
	{
		if(m_iAllocatedSize < m_siBaseSize) m_iAllocatedSize = m_siBaseSize;
		while(_iSize > m_iAllocatedSize) m_iAllocatedSize *= 2;
		if(m_poData)
			m_poData = (el *) realloc(m_poData, sizeof(el) * m_iAllocatedSize);
		else
			m_poData = (el *) malloc(sizeof(el) * m_iAllocatedSize);
	}
	else if((m_iAllocatedSize > m_siBaseSize) && (_iSize <= m_iAllocatedSize / 2))
	{
		if(m_bCanShrink)
		{
			m_iAllocatedSize /= 2;
			if(m_poData)
				m_poData = (el *) realloc(m_poData, sizeof(el) * m_iAllocatedSize);
			else
				m_poData = (el *) malloc(sizeof(el) * m_iAllocatedSize);
		}
	}

	m_iSize = _iSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vSetSize(int _iSize)
{
	if(_iSize == 0)
	{
		free(m_poData);
		m_poData = NULL;
		m_iAllocatedSize = 0;
		m_iSize = 0;
	}
	else
	{
		m_iAllocatedSize = _iSize;
		m_iSize = _iSize;
		if(m_poData)
			m_poData = (el *) realloc(m_poData, sizeof(el) * m_iAllocatedSize);
		else
			m_poData = (el *) malloc(sizeof(el) * m_iAllocatedSize);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vDelete(int _iIndex)
{
	if(m_iSize == 1)
		vResize(0);
	else
	{
		if(m_iSize - _iIndex - 1 > 0)
			memmove(m_poData + _iIndex, m_poData + _iIndex + 1, (m_iSize - _iIndex - 1) * sizeof(el));
		vResize(m_iSize - 1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vClear()
{
	/*~~~~~~~~~~~*/
	free(m_poData);
	/*~~~~~~~~~~~*/

	m_poData = NULL;
	m_iAllocatedSize = 0;
	m_iSize = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vDeleteAndClear()
{
	for(int iIndex = 0; iIndex < m_iSize; iIndex++) delete m_poData[iIndex];

	/*~~~~~*/
	vClear();
	/*~~~~~*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vFreeAndClear()
{
	for(int iIndex = 0; iIndex < m_iSize; iIndex++) free(m_poData[iIndex]);
	vClear();
}

template<class el, int basesize>

/*
 =======================================================================================================================
 =======================================================================================================================
 */
el *DynamicArray<el, basesize>::poGetData()
{
	return m_poData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
const el *DynamicArray<el, basesize>::poGetData() const
{
	return m_poData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
el &DynamicArray<el, basesize>::operator[](int _iIndex)
{
	return m_poData[_iIndex];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
const el &DynamicArray<el, basesize>::operator[](int _iIndex) const
{
	return m_poData[_iIndex];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vAdd(const el &_roItem)
{
	vResize(m_iSize + 1);
	m_poData[m_iSize - 1] = _roItem;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
DynamicArray<el, basesize> &DynamicArray<el, basesize>::operator+=(const el &_roItem)
{
	/*~~~~~~~~~~*/
	vAdd(_roItem);
	/*~~~~~~~~~~*/

	return *this;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
void DynamicArray<el, basesize>::vAdd(const el *_poItems, int _iSize)
{
	/*~~~~~~~~~~~~~~~~~*/
	int iIndex = m_iSize;
	/*~~~~~~~~~~~~~~~~~*/

	vResize(m_iSize + _iSize);
	memcpy(m_poData + iIndex, _poItems, _iSize * sizeof(el));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
el *DynamicArray<el, basesize>::poAdd(int _iSize)
{
	/*~~~~~~~~~~~~~~~~~*/
	int iIndex = m_iSize;
	/*~~~~~~~~~~~~~~~~~*/

	vResize(m_iSize + _iSize);
	return m_poData + iIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
template<class el, int basesize>
size_t DynamicArray<el, basesize>::iWriteToFile(FILE *_poFile) const
{
	if(_poFile)
		return fwrite(m_poData, 1, m_iSize, _poFile);
	else
		return 0;
}

#endif
