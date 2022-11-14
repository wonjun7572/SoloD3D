#include "..\Public\Base.h"

CBase::CBase()
{
}

unsigned long CBase::AddRef()
{
	return ++m_dwRefCnt;
}

unsigned long CBase::Release()
{
	if (m_dwRefCnt == 0)
	{
		Free();

		delete this;

		return 0;
	}
	else
	{
		return m_dwRefCnt--;
	}
}