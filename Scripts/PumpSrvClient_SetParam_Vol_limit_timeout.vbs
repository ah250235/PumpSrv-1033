'First Get object to PumpSrv:
Set PumpSrvInt = WScript.CreateObject("PumpSrv.CTRL.1")


           
Limit = 10        ' put desired limit figure here this is the new site voulme in liters (10 liters in this example)
Timeout = 5000   ' put desired timeout figure here


For pump = 1 to 32
	result  = PumpSrvInt.SetParam("Pumps",pump,0,"ExtraOfflineTimeout",0," ",Timeout,0)
Next	

	result  = PumpSrvInt.SetParam("GeneralParam",0,0,"VolumeLimit",0," ",Limit,0)

	
	result  = PumpSrvInt.reconfigure2(256,1,0,0)
	result  = PumpSrvInt.reconfigure2(1048576,194,0,0)
