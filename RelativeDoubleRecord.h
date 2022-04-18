// RelativeDoubleRecord.h: interface for the CRelativeDoubleRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELATIVEDOUBLERECORD_H__257C8638_AE17_4DF0_8286_65FAAD3E2644__INCLUDED_)
#define AFX_RELATIVEDOUBLERECORD_H__257C8638_AE17_4DF0_8286_65FAAD3E2644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VALID_SIGNATURE        "File is valid       "

#define GET_LAST_SHIFT         0x00000001
#define GET_CURRENT_SHIFT      0x00000002

#include "DB.h"

class CRelativeDoubleRecord : public CRelativeFile  
{
public:
	CRelativeDoubleRecord(BYTE byFileIndex);
	virtual ~CRelativeDoubleRecord();

	long Write(DWORD lWritePartOffset,
		       WORD wWritePartLength,
			   void *pBuff,
			   long lFlags,
			   long lPCNumber,
			   long lTimeStampOffset);

	long Read(DWORD lReadPartOffset,
		      WORD wReadPartLength,
			  void *pBuff,
			  long lFlags,
			  long lPCNumber,
			  long lTimeStampOffset);

};

#endif // !defined(AFX_RELATIVEDOUBLERECORD_H__257C8638_AE17_4DF0_8286_65FAAD3E2644__INCLUDED_)
