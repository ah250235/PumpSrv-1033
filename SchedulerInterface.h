// SchedulerInterface.h: interface for the CSchedulerInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCHEDULERINTERFACE_H__4EE46231_B549_4029_90A7_3A428E6E298E__INCLUDED_)
#define AFX_SCHEDULERINTERFACE_H__4EE46231_B549_4029_90A7_3A428E6E298E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<class T>
class CSchedulerInterface
{
public:
	CSchedulerInterface() { }
	virtual ~CSchedulerInterface() { }

	long AddTask(DWORD dwTaskID , TaskParameters & cTaskParams , BYTE * pExtraData , short nExtraDataLen);
	
	HRESULT Connect(LPSTR sArchive);
	HRESULT Disconnect();

	T * GetSinkPointer() { return m_pSink; }
	operator T*() { return m_pSink; }
	
private:
	ITasksPtr m_pTasksPtr;

	T * m_pSink;
};

template<class T>
HRESULT CSchedulerInterface<T>::Connect(LPSTR sArchive)
{
	HRESULT hRes = m_pTasksPtr.CreateInstance(CLSID_Tasks);

	if(SUCCEEDED(hRes))
	{
		IUnknownPtr pConnectionPointUnknown = m_pTasksPtr;

		m_pSink  = new T;

		if(m_pSink)
			hRes = m_pSink->DispEventAdvise(pConnectionPointUnknown);

		m_pTasksPtr->Activate(sArchive);
	}

	return hRes;
}

template<class T>
HRESULT CSchedulerInterface<T>::Disconnect()
{
	HRESULT hRes = S_OK;

	if(m_pTasksPtr != NULL)
	{
		try
		{
			m_pTasksPtr->Deactivate();
			
			if(m_pSink)
			{
				IUnknownPtr pConnectionPointUnknown = m_pTasksPtr;

				hRes = m_pSink->DispEventUnadvise(pConnectionPointUnknown);

				delete m_pSink;
			}

			m_pTasksPtr.Release();
			m_pTasksPtr.Detach();

			m_pTasksPtr = NULL;
		}

		catch(...)
		{
			hRes = DISP_E_EXCEPTION;
		}
	}

	return hRes;
}

template<class T>
long CSchedulerInterface<T>::AddTask(DWORD dwTaskID , TaskParameters & cTaskParams , BYTE * pExtraData , short nExtraDataLen)
{
	long lRetVal = 0;

	try
	{
		if(m_pTasksPtr != NULL)
		{
			lRetVal = m_pTasksPtr->AddTask(sizeof(cTaskParams),
										   (BYTE*)&cTaskParams,
										   dwTaskID,
										   nExtraDataLen,
										   pExtraData);
		}
	}

	catch(...)
	{

	}

	return lRetVal;
}

#endif // !defined(AFX_SCHEDULERINTERFACE_H__4EE46231_B549_4029_90A7_3A428E6E298E__INCLUDED_)
