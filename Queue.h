// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__0550F12F_D2F7_11D3_928A_00C0F04E68A5__INCLUDED_)
#define AFX_QUEUE_H__0550F12F_D2F7_11D3_928A_00C0F04E68A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory.h> 
#include <windows.h>


enum QueueRetCode
{
	QUEUE_NO_ERROR,
	QUEUE_EMPTY,
	QUEUE_FULL,	
	QUEUE_INDEX_ERROR,
	QUEUE_END,
	QUEUE_START,
	QUEUE_NOT_FOUND
};


template < class T,short nMaxSize = 100 >
class CQueue 
{
public:
	QueueRetCode Remove(T* Elem);
	QueueRetCode Set(const T* Elem);
	CQueue();
	virtual ~CQueue();
	void Clean(void);
	bool Empty(void);
	QueueRetCode Get(long lIndex, T& Elem);
	QueueRetCode Update(long lIndex, T& Elem);
	long GetSize(void);  
	long GetHeadNum(void);  
	long GetTailNum(void);  
	QueueRetCode GetHead(T* Elem);
	QueueRetCode GetTail(T& Elem);
	QueueRetCode GetTop(T& Elem);
	BOOL IsActive() { return m_bActive ? TRUE : FALSE; }
	QueueRetCode UpdateHead(T& Elem);
	QueueRetCode UpdateTail(T& Elem);
	QueueRetCode GetNext(T& Elem, long *lPos);
	QueueRetCode GetPrev(T& Elem, long *lPos);

private:
	long m_lTail;  // The queue tail.
	long m_lHead;  // The queue head.
	T	m_tQueue[nMaxSize]; // The queue.
	long m_lCurrentSize;  // The current size of the queue.
	CCriticalSection m_csQueue;
	BOOL				m_bActive;

	QueueRetCode RangeCheck(long lIndex);
};


template <class T,short nMaxSize>
CQueue< T, nMaxSize >::CQueue()
{
	m_lTail = nMaxSize - 1;
	m_lHead = nMaxSize - 1;
	m_lCurrentSize = 0;
	
	for (int i = 0; i < nMaxSize; i++)
		memset(&m_tQueue[i], 0x00, sizeof(T));
	
	m_bActive = TRUE;

}

template <class T,short nMaxSize>
CQueue< T, nMaxSize >::~CQueue()
{
	m_bActive = FALSE;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::Set(const T* Elem)
{
	QueueRetCode lRetCode = QUEUE_NO_ERROR;
	
	if (m_bActive)
	{
		CSingleLock slQueue(&m_csQueue, TRUE);

		if (m_lCurrentSize >= nMaxSize)
			lRetCode = QUEUE_FULL;
		else
		{
			if (++m_lTail >= nMaxSize)
				m_lTail = 0;

			memcpy(&m_tQueue[m_lTail], Elem, sizeof(T));
			m_lCurrentSize++;
		}
	}
	else
		lRetCode = QUEUE_END;

	return lRetCode;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::Remove(T* Elem)
{

	QueueRetCode lRetCode = QUEUE_NO_ERROR;

	if (m_bActive)
	{
		CSingleLock slQueue(&m_csQueue, TRUE);

		if (Empty())
			lRetCode = QUEUE_EMPTY;
		else	
		{
			if (++m_lHead >= nMaxSize)
				m_lHead = 0;

			memcpy(Elem, &m_tQueue[m_lHead], sizeof(T));
			m_lCurrentSize--;
		}
	}
	else
		lRetCode = QUEUE_END;
	
	return lRetCode;
}

template <class T,short nMaxSize>
bool CQueue< T, nMaxSize >::Empty(void)
{
	bool bRetVal = false;

	CSingleLock slQueue(&m_csQueue, TRUE);

	if (m_lCurrentSize <= 0)
		bRetVal = true;

	return(bRetVal);
}

template <class T,short nMaxSize> 
void CQueue< T, nMaxSize >::Clean(void)
{
	m_lTail = nMaxSize - 1;
	m_lHead = nMaxSize - 1;
	m_lCurrentSize = 0;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::Get(long lIndex, T& Elem)
{
	QueueRetCode lRetCode = QUEUE_NO_ERROR;

	CSingleLock slQueue(&m_csQueue, TRUE);

	if (Empty())
		lRetCode = QUEUE_EMPTY;
	else if (lIndex < 0)
			lRetCode = QUEUE_INDEX_ERROR;
	else if (lIndex > nMaxSize)
			lRetCode = QUEUE_INDEX_ERROR;
	else
		lRetCode = RangeCheck(lIndex);

	if (lRetCode == QUEUE_NO_ERROR)
	{
			//lIndex--;
			Elem = m_tQueue[lIndex];
	}

return lRetCode;
}
	
template <class T,short nMaxSize>
long CQueue< T, nMaxSize >::GetSize(void)
{
	return(m_lCurrentSize);
}

template <class T,short nMaxSize>
long CQueue< T, nMaxSize >::GetHeadNum(void)
{
	long lHead;
	
	lHead = m_lHead + 1;

	if (lHead >= nMaxSize)
		lHead = 0;

	return (lHead);
}

template <class T,short nMaxSize>
long CQueue< T, nMaxSize >::GetTailNum(void)
{
	return(m_lTail);
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::Update(long lIndex, T& Elem)
{
	QueueRetCode lRetCode = QUEUE_NO_ERROR;

	CSingleLock slQueue(&m_csQueue, TRUE);

	if (Empty())
		lRetCode = QUEUE_EMPTY;
	else if (lIndex < 0)
		lRetCode = QUEUE_INDEX_ERROR;
	else
	lRetCode = RangeCheck(lIndex);

	if (lRetCode == QUEUE_NO_ERROR)
	{
			m_tQueue[lIndex] = Elem;
	}

return lRetCode;
	
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::GetHead(T* Elem)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	if (Empty())
		RetCode = QUEUE_EMPTY;
	else
	{
		if (m_lHead + 1 >= nMaxSize)
			memcpy(Elem, &m_tQueue[0], sizeof(T));
		else
			memcpy(Elem, &m_tQueue[m_lHead + 1], sizeof(T));
	}

return RetCode;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::GetTail(T& Elem)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	if (Empty())
		RetCode = QUEUE_EMPTY;
	else
		Elem = m_tQueue[m_lTail];

return RetCode;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::GetTop(T& Elem)
{
	return (GetHead(Elem));
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::UpdateHead(T& Elem)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	long lHead = GetHeadNum();
	Update(lHead, Elem);

return RetCode;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::UpdateTail(T& Elem)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	long lTail = GetTailNum();
	Update(lTail, Elem);

return RetCode;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::GetNext(T& Elem, long *lPos)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	if (Empty())
		RetCode = QUEUE_EMPTY;
	else if (*lPos == m_lTail)
		RetCode = QUEUE_END;
	else
	{
		RetCode = RangeCheck(*lPos);

		if (RetCode == QUEUE_NO_ERROR)
		{
			*lPos += 1;
			if (*lPos == nMaxSize)
				*lPos = 0;

			if (RetCode == QUEUE_NO_ERROR)
			{
					Elem = m_tQueue[*lPos];
			}
		}
	}

return RetCode;
}

template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::GetPrev(T& Elem, long *lPos)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	if (Empty())
		RetCode = QUEUE_EMPTY;
	else if (*lPos == m_lHead)
		RetCode = QUEUE_START;
	else
	{
		RetCode = RangeCheck(*lPos);

		if (RetCode == QUEUE_NO_ERROR)
		{
			*lPos -= 1;
			if (*lPos < 0)
				*lPos = nMaxSize - 1;

			if (RetCode == QUEUE_NO_ERROR)
			{
					Elem = m_tQueue[*lPos];
			}
		}
	}

return RetCode;
}

	 
template <class T,short nMaxSize>
QueueRetCode CQueue< T, nMaxSize >::RangeCheck(long lIndex)
{
	QueueRetCode RetCode = QUEUE_NO_ERROR;

	if (m_lHead < m_lTail)
	{
		if (lIndex < m_lHead && lIndex > m_lTail)
			RetCode = QUEUE_INDEX_ERROR;
	}
	else //Tail < Head
	{
		if (lIndex < m_lHead && lIndex > m_lTail)
			RetCode = QUEUE_INDEX_ERROR;
	}

	return RetCode;
}

#endif // !defined(AFX_QUEUE_H__0550F12F_D2F7_11D3_928A_00C0F04E68A5__INCLUDED_)

