#include "stdafx.h"
#include "SQLItemsFile.h"
#include "PumpSrvDatabase.h"

CSQLItemsFile::CSQLItemsFile()
{
}


CSQLItemsFile::~CSQLItemsFile()
{
}

long CSQLItemsFile::Operation(eFileOperation OpCode, void * pInfo) //4.0.18.501
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = OK;
	ITEMS_INFO *pItemsInfoToQdx = NULL;                      // TD 347994 
	if (pInfo)
		pItemsInfoToQdx = (ITEMS_INFO*)pInfo;

	switch (OpCode)
	{
	case REC_INSERT:
	{
		lRetVal = PumpSrvDataBase.SaveItemsInfo(pItemsInfoToQdx->ItemRec.lIndexNumber, *pItemsInfoToQdx);
		break;
	}
	case REC_READ:
	{
		lRetVal = PumpSrvDataBase.LoadItemsInfo(pItemsInfoToQdx->ItemRec.lIndexNumber, *pItemsInfoToQdx);
		break;
	}
	case REC_READ_FIRST:
	{
		lRetVal = PumpSrvDataBase.LoadFirstItemsInfo(*pItemsInfoToQdx);
		break;
	}
	case REC_DELETE:
	{
		lRetVal = PumpSrvDataBase.DeleteItemsInfo(*pItemsInfoToQdx);
		break;
	}
	}

	return lRetVal;
}