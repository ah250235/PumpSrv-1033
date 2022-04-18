// Param.h: interface for the CParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAM_H__ADD085E3_8CF0_11D2_9FBF_004005615A06__INCLUDED_)
#define AFX_PARAM_H__ADD085E3_8CF0_11D2_9FBF_004005615A06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define REC_VALUE_DELIM		(LPSTR)",|"

char const REC_VALUE_DELIM_LANGUAGES[] = {26,'|'};  //4.0.13.508 
#define LANGUAGE_STR		"Languages" //4.0.13.508

#define SAVE_PARAM		1
#define SET_RECORD		2

#define FIELD_NOT_EXISTS	0
#define FIELD_EXISTS		1
#define FIELD_REMOVED		2


class CParam  
{
public:
	int LoadParam(char *szServerPath, char *pszSection,char *pszName, char *pszVal, DWORD dwLen , char *pszDefaultVal,BOOL bCreate,long lGroupNum=0,long lFlags=0); // TD 329049
	long GetRegistryValues(char * sKeyName , char * sValName , DWORD * pdwNumericMember , BOOL bCreateInRegistry , BOOL bCompare);
	long GetRegistryValues(char * sKeyName , char * sValName , char * sStringMember , BOOL bCreateInRegistry , BOOL bCompare);
	DWORD RegDeleteKeyNT(HKEY hStartKey , LPTSTR pKeyName );
	int DeleteParam(char *pszSection,char *pszName, long *pVal);
	LONG SetParam3Level(char *szSubjectName, char *szTableName,char *szSubTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType,  char *szFieldVal, DWORD dwVal ,BOOL bCreate,BOOL bIniFile=FALSE);  //4.0.8.503
	LONG GetParam3Level(char *szSubjectName, char *szTableName,char *szSubTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType,  char *szFieldVal, long lBufferLen, DWORD *pVal ,BOOL bCreate ,BOOL bIniFile = FALSE); //4.0.8.503

	//!!!!!YKL 01/12/99
	LONG GetRecord(char *szTableName, long lKey1, long lKey2, char *szBuff, long lBufferLen, long *lWrittenLen, long *lNumberOfFields, BOOL bWriteToLog = TRUE,long lFlags = 1);//long lFlags = HOP_DIRECT_LOCAL
	LONG SetRecord(char *szTableName, long lKey1, long lKey2, char *szBuff, BOOL bCreate, BOOL bWriteToLog = TRUE,long lGroupNum = 0,long lFlags = 0);	
	//!!!!!YKL 01/12/99


	LONG GetParam(char *szTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType,  char *szFieldVal, long lBufferLen, DWORD *pVal ,BOOL bCreate);
	LONG GetParam(LPCSTR szTableName, long lKey1, long lKey2, LPCSTR szFieldName, short nFiledType, CString & szFieldVal, DWORD *pVal, BOOL bCreate);
	LONG SetParam(char *szTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType,  char *szFieldVal, DWORD dwVal ,BOOL bCreate,long lGroupNum = 0,long lFlags = 0,long lClientEventData = 0);//4.0.1.38	
	LONG SetParam(char *szTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType, std::string szFieldVal, DWORD dwVal, BOOL bCreate, long lGroupNum = 0, long lFlags = 0, long lClientEventData = 0)
	{
		return SetParam(szTableName, lKey1, lKey2, szFieldName, nFiledType, (char*)szFieldVal.c_str(), dwVal, bCreate, lGroupNum, lFlags, lClientEventData);
	}

	CParam();
	virtual ~CParam();

	BOOL CheckValidEntry(char *pszSection,long lFlags = 1); //4.0.18.501 change lFlag from 0 to 1 (HOP_DIRECT_LOCAL)

	int LoadParam(LPCSTR pszSection, LPCSTR pszName, DWORD *pVal, DWORD dwDefaultVal, BOOL bCreate, BOOL bWriteToLog = TRUE, long lGroupNum = 0, long lFlags = 0, BOOL bIniFiled = FALSE);  //4.0.8.503
	int LoadParam(LPCSTR pszSection, LPCSTR pszName, char *pszVal, DWORD dwLen, LPCSTR pszDefaultVal, BOOL bCreate, BOOL bWriteToLog = TRUE, long lGroupNum = 0, long lFlags = 0, BOOL bIniFiled = FALSE); //4.0.8.503
	int LoadParam(LPCSTR pszSection, LPCSTR pszName, CString & csVal, LPCSTR pszDefaultVal, BOOL bCreate, BOOL bWriteToLog = TRUE, long lGroupNum = 0, long lFlags = 0, BOOL bIniFiled = FALSE); //4.0.8.503

	
	int	SaveParam(char *pszSection,char *pszName,DWORD dwVal, BOOL bCreate = TRUE,long lGroupNum = 0,long lFlags = 0,long lClientEventData = 0,BOOL bIniFiled =FALSE);//4.0.1.38 //4.0.8.503
	int	SaveParam(char *pszSection,char *pszName,char *pszVal,BOOL bCreate = TRUE,long lGroupNum = 0,long lFlags = 0,long lClientEventData = 0,BOOL bIniFiled =FALSE);//4.0.1.38 //4.0.8.503
	
	BOOL IsPumpSrvTable(char *szTableName);
protected:
	//4.0.0.47 start
	long RemoveAndSaveParam(char* szTableName,char *szBuff, char *szPSSpecialFieldName,BOOL bCreate);
	long CheckPumpSrvParam(char* szTableName,char* szFieldName, char* szBuff, char* szPSSpecialFieldName, long lFuncType,BOOL bCreate);
	long IsLocalOnlyParameter(char* szTableName, char* szFieldName, char* szBuff, long lFuncType,BOOL bCreate);
	//4.0.0.47 end
	CCriticalSection m_csParamDB;

private:
   //!!!!!YKL 01/12/99
	long GetFieldValue(char *szBuff, char **pszNextBuff, CString *pstrValueName, long *plValueType, CString *pstrValue, DWORD *pdwValue);
	long GetRecValues(char *strPathName, CString *pstrVals,long *lNumFields, BOOL bWriteToLog=TRUE);
	CString GetRecordName(char *szTableName, long lKey1, long lKey2);
   //!!!!!YKL 01/12/99
};

#endif // !defined(AFX_PARAM_H__ADD085E3_8CF0_11D2_9FBF_004005615A06__INCLUDED_)
