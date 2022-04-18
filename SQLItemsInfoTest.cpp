#include "stdafx.h"
#include "SQLItemsInfoTest.h"
#include "PumpSrvStructures.h"
#include "PumpSrvDeclarations.h"
#include "SQLItemsFile.h"
#include "SQLExtraInfoFile.h"

CSQLItemsInfoTest::CSQLItemsInfoTest()
{
}


CSQLItemsInfoTest::~CSQLItemsInfoTest()
{
}

long CSQLItemsInfoTest::TestInsert(ITEMS_INFO temp)
{
	

	CSQLItemsFile cItemsInfo;
	cItemsInfo.Operation(REC_INSERT, &temp);

	return TRUE;
}

long CSQLItemsInfoTest::TestLoad(ITEMS_INFO& temp)
{
	CSQLItemsFile cItemsInfo;
	cItemsInfo.Operation(REC_READ, &temp);
	return TRUE;
}

long CSQLItemsInfoTest::TestDelete(ITEMS_INFO temp)
{
	CSQLItemsFile cItemsInfo;
	cItemsInfo.Operation(REC_DELETE, &temp);
	return TRUE;
}

long CSQLItemsInfoTest::TestLoadFirst(ITEMS_INFO &temp)
{
	CSQLItemsFile cItemsInfo;
	cItemsInfo.Operation(REC_READ_FIRST, &temp);
	return TRUE;
}

long CSQLItemsInfoTest::TestManager()
{
	CString sExtraInfoXml;
	ITEMS_INFO temp;
	PUMPSRV_EXTRA_INFO_REC temp2;
	CSQLExtraInfoFile cExtraInfo;
	memset(&temp2, 0x00, sizeof(PUMPSRV_EXTRA_INFO_REC));
	cExtraInfo.Operation(REC_READ, &temp2, sExtraInfoXml);
	memset(&temp, 0, sizeof(temp));
	temp.ItemRec.lIndexNumber = 222;
	TestInsert(temp);
	temp.ItemRec.lIndexNumber = 231;
	TestInsert(temp);
	TestLoad(temp);
	TestDelete(temp);
	TestLoadFirst(temp);
	return TRUE;
}