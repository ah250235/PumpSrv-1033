#include "StdAfx.h"
#include "TankLevelProtection.h"



#define SHIFT_MUSK(bMask) bMask = bMask<<1

 

CTankLevelProtection::CTankLevelProtection()
{
	m_lNumOfPumps=0;
	m_lProtectTrsLimit=0;
	m_lTankMinVolumeLimit=0;
	InitializeCriticalSection(&m_csTankLevelMap);
}

CTankLevelProtection::~CTankLevelProtection()
{
	DeleteCriticalSection(&m_csTankLevelMap);
}


/******************************************************************************
 Description:	when the system is start up and the flag ProtectTrsLimit is on	
 Returns:      	we  will init the map for each pump we will hold all the grades 
 Parameters:    None. 	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			20/06/2005   11:21		Start
******************************************************************************/
void CTankLevelProtection::LoadTankMapPerPump()
{
	DWORD	dwTmp = 0;													 	
   	long    p=0;
	
//	if(m_lProtectTrsLimit)
//	{
		for(long lPumpNumber=1; lPumpNumber<= MAX_PUMPS_64;lPumpNumber++)  
		{
			p = lPumpNumber-1;
			_Module.m_server.m_cParam.GetParam("Pumps",lPumpNumber,0,"ValidEntry",FIELD_DWORD,NULL,0,&dwTmp,FALSE);

			if(dwTmp)
			{
				m_cTankLevelMap[p].lPumpNumber = lPumpNumber;
				m_lNumOfPumps++;
				for(int n=1;n<=MAX_POSITIONS_PER_PUMP;n++)
				{
					dwTmp = 0;
					_Module.m_server.m_cParam.GetParam("Position",lPumpNumber,n,"ValidEntry",FIELD_DWORD,NULL,0,&dwTmp,FALSE);

					if(dwTmp)
					{
						_Module.m_server.m_cParam.GetParam("Position",lPumpNumber,n,"Grade1",FIELD_DWORD,NULL,0,&dwTmp  ,FALSE);
						m_cTankLevelMap[p].GradeInfo[n-1].SetGradeValid(FALSE);
						m_cTankLevelMap[p].GradeInfo[n-1].SetGradeNumber(dwTmp);
					}
					else
					{
						m_cTankLevelMap[p].GradeInfo[n-1].SetGradeValid(TRUE);
					}
				}
			}

		
		}// end pump
//	}
	_LOGMSG.LogMsg("CTankLevelProtection::LoadTankMapPerPump Finish load all grades per pumps");
}





/******************************************************************************
 Description: Function that get pump number and return according to the map 
			  all the valid grades and the grade name 
			  use for posPump
 Returns:      	
 Parameters:    None. 	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			20/06/2005   11:21		Start
******************************************************************************/
void CTankLevelProtection::GetValidGradesTankLevelForOnePump(long lPumpNumber,MAP_GRADE_TANK_LEVEL & cRec)
{
	int i=0,countValidGrade=0;
	int p= lPumpNumber-1;
	CString str , sTable("Grades\\Grade");
	char sNumber[8],sGradeName[GRADE_NAME];
	

	
	EnterCriticalSection( &m_csTankLevelMap);
	
	cRec.lNumOfValidGrades =0;
	cRec.lPumpNumber = lPumpNumber;
//	while (i<m_lNumOfPumps)//until we found the pump
	while (i<MAX_PUMPS_64)//52558 4.0.23.400 120241 //4.0.24.90
	{
		if(m_cTankLevelMap[i].lPumpNumber==lPumpNumber)//we found the pump in the arr
		{
			for(int n=1;n<=MAX_POSITIONS_PER_PUMP;n++)//run on all the grades if valid insert to the out arr
			{	//if false then the grade is valid
				if(!(m_cTankLevelMap[i].GradeInfo[n-1].IfGradeValid()))	
				{
					cRec.GradeInfo[countValidGrade].SetGradeNumber(m_cTankLevelMap[i].GradeInfo[n-1].GetGradeNamber());
					//get the grade name
					memset(sNumber,0,sizeof(sNumber));
					sprintf(sNumber,"%02d",n);
					str = sTable + sNumber;
					_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "FullName", (char *)sGradeName, GRADE_NAME ,"Unleaded",0);
					cRec.GradeInfo[countValidGrade].SetGradeName(sGradeName);
					countValidGrade++;
				}
				else
				{
					//4.0.11.500
					cRec.GradeInfo[countValidGrade].SetGradeNumber(0);
					cRec.GradeInfo[countValidGrade].SetGradeName("NA");
					countValidGrade++;
					_LOGMSG.LogMsg("CTankLevelProtection::GetValidGradesTankLevelForOnePump Grade Not Av");
				}

			}
			break;
			
		}
		i++;
	}
		
	cRec.lNumOfValidGrades = countValidGrade;
	LeaveCriticalSection( &m_csTankLevelMap );
	
}





/******************************************************************************
 Description: Function that get pump number and grade number and set
			   the status of the grade
			  use for RFS
 Returns:      	
 Parameters:    lPumpNumber - if 0 then run over all map and change the grade. 	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			20/06/2005   11:21		Start
******************************************************************************/
void CTankLevelProtection::SetStatusPerTankWithPump(BSTR sTagPumpsNuzzlesRestrict, long lFlags)
{
	

	TAG_TANK_LOW_LEVEL_RESTRICT_DATA cTagPumps;


	CString str;
	
	memset(&cTagPumps,' ',sizeof(cTagPumps));

	int iLen = Bstr2Buffer(sTagPumpsNuzzlesRestrict, (void *)&cTagPumps,sizeof(TAG_TANK_LOW_LEVEL_RESTRICT_DATA));

	int		i=0,n=1,countValidGrade=0;
	long	linfectedPositions = 0,lIsRestricted=0;
	BOOL	bChangeStatus = FALSE;
	BYTE	bMask = 0x01;

	EnterCriticalSection(&m_csTankLevelMap);

	for(i=0;i<MAX_PUMPS_64;i++)
	{
		linfectedPositions = a2l(cTagPumps.cAffectedPumps[i].byAffectedPositions  ,sizeof(cTagPumps.cAffectedPumps[i].byAffectedPositions));
		lIsRestricted	   = a2l(&cTagPumps.byIsRestricted,sizeof(cTagPumps.byIsRestricted));
		bMask = 0x01;
		if(linfectedPositions == 0)
			continue;
		for(int y=1;y<=MAX_POSITIONS_PER_PUMP;y++)
		{
			if(linfectedPositions & bMask)
			{
				//bChangeStatus = (BOOL)cTagPumps.byIsRestricted;
				str.Format("CTankLevelProtection::SetStatusPerTankWithPump Pump %02ld, change status of tank level from %ld to %ld in grade Position %ld ",i+1,
					m_cTankLevelMap[i].GradeInfo[y-1].IfGradeValid(),lIsRestricted,y);
				_LOGMSG.LogMsg(str);
				m_cTankLevelMap[i].GradeInfo[y-1].SetGradeValid(lIsRestricted);
			}
			SHIFT_MUSK(bMask);
		}
	
	
	}
	LeaveCriticalSection(&m_csTankLevelMap);

	/*if(lPumpNumber==0)//must run all over arr and change the grade for all pumps
	{
		for(i=0;i<m_lNumOfPumps;i++)
		{
			while(n<=MAX_POSITIONS_PER_PUMP)//run on all the grades 
			{
				if(m_cTankLevelMap[i].GradeInfo[n-1].GetGradeNamber() == lGrade) 
				{
					m_cTankLevelMap[i].GradeInfo[n-1].SetGradeValid(bStatus);
					break;
				}
			}
		}
	}
	else 
	{
		while (i<m_lNumOfPumps)//until we found the pump
		{
			if(m_cTankLevelMap[i].lPumpNumber==lPumpNumber)//we found the pump in the arr
			{
				//we have to find the grade and set his status
				while(n<=MAX_POSITIONS_PER_PUMP)//run on all the grades 
				{
					if(m_cTankLevelMap[i].GradeInfo[n-1].GetGradeNamber() == lGrade) 
					{
						m_cTankLevelMap[i].GradeInfo[n-1].SetGradeValid(bStatus);
						break;
					}
				n++;
				}
				break;
			}
			i++;
		}
	}*/

}


