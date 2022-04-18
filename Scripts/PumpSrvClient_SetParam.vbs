

'First Get object to PumpSrv:
Set PumpSrvInt = WScript.CreateObject("PumpSrv.CTRL.1")

'*******************************
'Example of setting pure products code
'*******************************
sTableName = "PureProducts"
key1 = 1     'Pure product number 1
key2 = 0
sFiledName = "Code"  'Parameter name (DWORD)
nFiledType = 0    '0 - DWORD, 1 - String
sVal = 0
lVal = 123
bCreate = 0

	result  = PumpSrvInt.SetParam(sTableName,key1,key2,sFiledName,nFiledType,sVal,lVal,bCreate)
		
msgbox  sTableName & " Setup result = " & result & " "

'*******************************
'Example of setting Tanks - Tank 1 capacity
'*******************************
sTableName = "Tanks"
key1 = 1      'Tank number 1
key2 = 0     
sFiledName = "FullHeightVolume"  'Parameter name (Capacity in FCC) (DWORD)
nFiledType = 0   '0 - DWORD, 1 - String
sVal = " "
lVal = 123000
bCreate = 0

	result  = PumpSrvInt.SetParam(sTableName,key1,key2,sFiledName,nFiledType,sVal,lVal,bCreate)

msgbox  sTableName & " Setup result = " & result & " "

'*******************************
'Example of setting Grades - Grade 3 Fullname
'*******************************
sTableName = "Grades"
key1 = 3      'grade number 3
key2 = 0
sFiledName = "FullName" 'Parameter name
nFiledType = 1   '0 - DWORD, 1 - String 
sVal = "MYGrade" 'String
lVal = 0
bCreate = 0

	result  = PumpSrvInt.SetParam(sTableName,key1,key2,sFiledName,nFiledType,sVal,lVal,bCreate)

msgbox  sTableName & " Setup result = " & result & " "

'*******************************************
'Example of setting ValidEntry Pump 1 Position 2 - 
'*****************************************
sTableNameAndFiledName = "Position,ValidEntry"
key1 = 1      'pump number 1
key2 = 2      'position number 2
nFiledType = 0   '0 - DWORD, 1 - String
sVal = " " 'String
lVal = 1
bCreate = 0

	result = PumpSrvInt.SetParam2(sTableNameAndFiledName,key1,key2,nFiledType,lVal,bCreate)

msgbox  sTableNameAndFiledName & " Setup result = " & result & " "


'*******************************************
'Example of setting Grade number assignment of Pump 1 Position 2 - 
'*****************************************
sTableNameAndFiledName = "Position,Grade1"
key1 = 1      'pump number 1
key2 = 2      'position number 2
nFiledType = 0   '0 - DWORD, 1 - String
sVal = " " 'String
lVal = 1
bCreate = 0

	result  = PumpSrvInt.SetParam2(sTableNameAndFiledName,key1,key2,nFiledType,lVal,bCreate)

msgbox  sTableNameAndFiledName & " Setup result = " & result & " "