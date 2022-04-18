#pragma once

class CSQLPumpSrvFile
{
public:
	CSQLPumpSrvFile();
	virtual ~CSQLPumpSrvFile();

public:
	static	CSQLPumpSrvFile &	GetInstance(void);

public:
	virtual long	LoadInfo(long lPumpNumber, BOOL bForMultiSync = FALSE); //4.0.12.513 (add param bForMultiSync)
	virtual long	SaveInfo(long lPumpNumber, LPCSTR szFunction, int LineNumber, BOOL bLocal = FALSE);//4.0.0.78
	virtual long	LoadServerInfo(void);
	virtual long	SaveServerInfo(void);
	virtual long	LoadShiftInfo(void);
	virtual long	SaveShiftInfo(void);
	virtual long	SavePAPInfo(long lPumpNumber, LPCSTR szFunction, int LineNumber);
	virtual long	LoadPAPInfo(long lPumpNumber);
	virtual long	SaveRealTimeInfo(long lPumpNumber, LPCSTR szFunction, int LineNumber, BOOL bLocal = FALSE);//4.0.0.78
};

