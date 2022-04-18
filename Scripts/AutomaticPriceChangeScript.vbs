
'Script for price change 
'Creator : shuki yakar 
'Date :    11/01/2011
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

'SetGradePrice2(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long lTaxRate, long lDiscountPerUnit, long lMaxUnitForDiscount)

'Flags values, do not change !
PRICE_CHANGE_INTEGRITY = &H8000										'remarking here make price change to work the normal way (not tesco)
PRICE_END_OF_LIST = &H1000
MAX_UNIT_IS_BATCH_NUM = &H2000

'First Get object to PumpSrv:
Set PumpSrvInt = WScript.CreateObject("PumpSrv.CTRL.1")

sTimeValid = "              "
nFlags = PRICE_CHANGE_INTEGRITY

lBatchNum = Second(Now) + (Minute(Now) * 60) + (Hour(Now) * 3600)	'To change batch number do it here
lPrice = 20000														'To change price do it here (price is in 1000)

For nGrade = 1 to 4					'Change grade price for grades 1, 2 ,3
	For lMode = 0 to 1				'0 = Self   ,1 = Full
		For nLevel = 1 to 2			'1 = Cash   ,2 = Credit
			result = PumpSrvInt.SetGradePrice2(0, 0, nGrade, lMode, nLevel, lPrice, sTimeValid, nFlags, 0, 0, 0)
			lPrice = lPrice + 10
		Next 
	Next 
Next 

'End of List
nFlags = nFlags Or MAX_UNIT_IS_BATCH_NUM Or PRICE_END_OF_LIST		'to change end of list flags do it here			
result = PumpSrvInt.SetGradePrice2(0, 0, 1, 0, 1, lPrice, sTimeValid, nFlags, 0, 0, lBatchNum)	'to disable end of list remark this line
