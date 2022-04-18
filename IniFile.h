// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__DBA60686_3277_474C_9E8D_686E796E3669__INCLUDED_)
#define AFX_INIFILE_H__DBA60686_3277_474C_9E8D_686E796E3669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  INI_FILE_NAME "RunTimeFile.INI"

class CIniFile  
{
public:
	void ResetIniParameters();
	long ReadParam(char *sSection, char *sfieldName, DWORD *dwVal, long lFlags=0, BOOL bCreate=TRUE);
	long WriteParam(char *sSection, char *sfieldName, DWORD *dwVal, long lFlags=0, BOOL bCreate= TRUE);
	long ReadParam(char *sSection, char *sfieldName, char *sVal, long lLen ,long lFlags=0, BOOL bCreate=TRUE);
	long WriteParam(char * sSection , char *sfieldName , char  *sVal , long lFlags=0, BOOL bCreate=TRUE);
	CString GetFilePath() {return m_sFileName;} //4.0.12.505
	CIniFile();
	virtual ~CIniFile();
private:
	void SetFileName();
	CString m_sFileName;

};

#endif // !defined(AFX_INIFILE_H__DBA60686_3277_474C_9E8D_686E796E3669__INCLUDED_)
