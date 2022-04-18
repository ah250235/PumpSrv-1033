#pragma once
class CSQLItemsInfoTest
{
public:
	CSQLItemsInfoTest();
	~CSQLItemsInfoTest();
	long TestInsert(ITEMS_INFO temp);
	long TestLoad(ITEMS_INFO &temp);
	long TestDelete(ITEMS_INFO temp);
	long TestLoadFirst(ITEMS_INFO &temp);
	long TestManager();
};

