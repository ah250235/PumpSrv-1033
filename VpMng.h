// VpMng.h: interface for the CVpMng class.
//
// CVpMng class manage  VP connection and interfaces.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPMNG_H__AB0C13D3_566D_4277_8C65_79F12E3719F8__INCLUDED_)
#define AFX_VPMNG_H__AB0C13D3_566D_4277_8C65_79F12E3719F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "VP.tlb" no_namespace named_guids 
enum VpStatus
{
	    VP_OFF, 
		VP_OK,				
		VP_FAIL_START,		
		VP_FAIL_STOP,		
};
		 

class CVpMng  
{
public:
	VpStatus Connect(long lPumpSrvVersion);
	VpStatus Disconnect();
	CVpMng();
	virtual ~CVpMng();

private:
	IServersPtr pIServersPtr;
	HRESULT		m_hRes;
};

#endif // !defined(AFX_VPMNG_H__AB0C13D3_566D_4277_8C65_79F12E3719F8__INCLUDED_)
