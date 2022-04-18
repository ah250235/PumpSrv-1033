# Microsoft Developer Studio Project File - Name="PumpSrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PumpSrv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PumpSrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PumpSrv.mak" CFG="PumpSrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PumpSrv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "PumpSrv - Win32 Release MinDependency" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Fuel/PumpSrv 4.0", JGKRAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PumpSrv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\HydService\Source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_ATL_STATIC_REGISTRY" /D "_AFXDLL" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Qlnkc.lib /nologo /subsystem:windows /map /debug /machine:I386 /libpath:"..\..\Qdex32\\"
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\PumpSrv.exe
InputPath=.\Debug\PumpSrv.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PumpSrv - Win32 Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Od /I "..\..\HydService\Source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_ATL_STATIC_REGISTRY" /D "_AFXDLL" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib ..\ThirdParty\QLNKC.LIB /nologo /subsystem:windows /map /debug /machine:I386 /libpath:"..\..\Qdex32\\"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\PumpSrv.exe
InputPath=.\ReleaseMinDependency\PumpSrv.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "PumpSrv - Win32 Debug"
# Name "PumpSrv - Win32 Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AccountReceipt.cpp
# End Source File
# Begin Source File

SOURCE=.\AccumulationShiftTotal.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmData.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmSrvPS.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseFuelingRestriction.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseReceipt.cpp
# End Source File
# Begin Source File

SOURCE=.\BasicProtectData.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildLine2PosPump.cpp
# End Source File
# Begin Source File

SOURCE=.\CardSaleAllData.cpp
# End Source File
# Begin Source File

SOURCE=.\CarWash.cpp
# End Source File
# Begin Source File

SOURCE=.\CashAcceptor.cpp
# End Source File
# Begin Source File

SOURCE=.\CashAcceptorFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CFGEventsHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangeProductInTank.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\CompChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\CompConnectivity.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigurationThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DataExport.cpp
# End Source File
# Begin Source File

SOURCE=.\DB.cpp
# End Source File
# Begin Source File

SOURCE=.\DebitInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DelayedPrePay.cpp
# End Source File
# Begin Source File

SOURCE=.\DeliveryEmulationSingleTankServerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\EventManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ExceptionHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtendedGradeMap.cpp
# End Source File
# Begin Source File

SOURCE=.\FleetCard.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowRate.cpp
# End Source File
# Begin Source File

SOURCE=.\ForecourtFuelingRestriction.cpp
# End Source File
# Begin Source File

SOURCE=.\ForecourtProductList.cpp
# End Source File
# Begin Source File

SOURCE=.\FuelingRestriction.cpp
# End Source File
# Begin Source File

SOURCE=.\GAIEventsHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\GCI.cpp
# End Source File
# Begin Source File

SOURCE=.\GPI.cpp
# End Source File
# Begin Source File

SOURCE=.\GPIEventManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Grade.cpp
# End Source File
# Begin Source File

SOURCE=.\Hydra.cpp
# End Source File
# Begin Source File

SOURCE=.\HydraPS.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\InitDebitHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Logger.cpp
# End Source File
# Begin Source File

SOURCE=.\MSXMLInt.cpp
# End Source File
# Begin Source File

SOURCE=.\Param.cpp
# End Source File
# Begin Source File

SOURCE=.\POSFuelingRestriction.cpp
# End Source File
# Begin Source File

SOURCE=.\PosInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PriceChangeFlags.cpp
# End Source File
# Begin Source File

SOURCE=.\PriceChangeWithAckPerPump.cpp
# End Source File
# Begin Source File

SOURCE=.\Promotion.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtectedDataHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtectedReceipt.cpp
# End Source File
# Begin Source File

SOURCE=.\Pump.cpp
# End Source File
# Begin Source File

SOURCE=.\PumpLock.cpp
# End Source File
# Begin Source File

SOURCE=.\PumpLockbyTank.cpp
# End Source File
# Begin Source File

SOURCE=.\PumpSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\PumpSrv.idl

!IF  "$(CFG)" == "PumpSrv - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing MIDL step
InputPath=.\PumpSrv.idl

BuildCmds= \
	midl /Oicf /h "PumpSrv.h" /iid "PumpSrv_i.c" "PumpSrv.idl"

".\PumpSrv.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\PumpSrv.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\PumpSrv_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PumpSrv - Win32 Release MinDependency"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing MIDL step
InputPath=.\PumpSrv.idl

BuildCmds= \
	midl /Oicf /h "PumpSrv.h" /iid "PumpSrv_i.c" "PumpSrv.idl"

".\PumpSrv.tlb" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\PumpSrv.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\PumpSrv_i.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PumpSrv.rc
# End Source File
# Begin Source File

SOURCE=.\PumpSrvDB.cpp
# End Source File
# Begin Source File

SOURCE=.\PumpTotalsXml.cpp
# End Source File
# Begin Source File

SOURCE=.\QDXMaintThread.cpp
# End Source File
# Begin Source File

SOURCE=.\QLogger.cpp
# End Source File
# Begin Source File

SOURCE=.\ReceiptExtraIndexFile.cpp
# End Source File
# Begin Source File

SOURCE=.\RegTable.cpp
# End Source File
# Begin Source File

SOURCE=.\RelativeDoubleRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\Report.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\RTEventsHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\RTIEventsHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ShiftDelivery.cpp
# End Source File
# Begin Source File

SOURCE=.\ShiftProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\ShiftTanks.cpp
# End Source File
# Begin Source File

SOURCE=.\ShiftTotals.cpp
# End Source File
# Begin Source File

SOURCE=.\SinglePumpLock.cpp
# End Source File
# Begin Source File

SOURCE=.\SingleShiftAccumMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SingleShiftAccumulations.cpp
# End Source File
# Begin Source File

SOURCE=.\SiteConfigurationExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StoreLineOLA.cpp
# End Source File
# Begin Source File

SOURCE=.\Tank.cpp
# End Source File
# Begin Source File

SOURCE=.\TankGaugeFactor.cpp
# End Source File
# Begin Source File

SOURCE=.\TankLevelProtection.cpp
# End Source File
# Begin Source File

SOURCE=.\TankXML.cpp
# End Source File
# Begin Source File

SOURCE=.\TerminalMessages.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeOutManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TotalPumpInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Track2Receipt.cpp
# End Source File
# Begin Source File

SOURCE=.\TrsLog.cpp
# End Source File
# Begin Source File

SOURCE=.\WetStockDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\XmlConvertor.cpp
# End Source File
# Begin Source File

SOURCE=.\XmlConvertorEx.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\XmlParser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AccountReceipt.h
# End Source File
# Begin Source File

SOURCE=.\AccumulationShiftTotal.h
# End Source File
# Begin Source File

SOURCE=.\AlarmConvert.h
# End Source File
# Begin Source File

SOURCE=.\AlarmData.h
# End Source File
# Begin Source File

SOURCE=.\AlarmSrv.h
# End Source File
# Begin Source File

SOURCE=.\AlarmSrvPS.h
# End Source File
# Begin Source File

SOURCE=.\BaseEncodeHandler.h
# End Source File
# Begin Source File

SOURCE=.\BaseEncodingAlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseFuelingRestriction.h
# End Source File
# Begin Source File

SOURCE=.\BaseReceipt.h
# End Source File
# Begin Source File

SOURCE=.\BasicProtectData.h
# End Source File
# Begin Source File

SOURCE=.\BuildLine2PosPump.h
# End Source File
# Begin Source File

SOURCE=.\CardSaleAllData.h
# End Source File
# Begin Source File

SOURCE=.\CarWash.h
# End Source File
# Begin Source File

SOURCE=.\CashAcceptor.h
# End Source File
# Begin Source File

SOURCE=.\CashAcceptorFile.h
# End Source File
# Begin Source File

SOURCE=.\CFGEventsHandler.h
# End Source File
# Begin Source File

SOURCE=.\ChangeProductInTank.h
# End Source File
# Begin Source File

SOURCE=.\CmdInfo.h
# End Source File
# Begin Source File

SOURCE=.\commonDef.h
# End Source File
# Begin Source File

SOURCE=.\CommonFunction.h
# End Source File
# Begin Source File

SOURCE=.\CommonStruct.h
# End Source File
# Begin Source File

SOURCE=.\CompChecker.h
# End Source File
# Begin Source File

SOURCE=.\CompConnectivity.h
# End Source File
# Begin Source File

SOURCE=.\ConfigurationThread.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl.h
# End Source File
# Begin Source File

SOURCE=.\DataExport.h
# End Source File
# Begin Source File

SOURCE=.\DB.h
# End Source File
# Begin Source File

SOURCE=.\DebitInfo.h
# End Source File
# Begin Source File

SOURCE=.\DelayedPrePay.h
# End Source File
# Begin Source File

SOURCE=.\DeliveryEmulationSingleTankServerInfo.h
# End Source File
# Begin Source File

SOURCE=.\DeviceRecord.h
# End Source File
# Begin Source File

SOURCE=.\EventManager.h
# End Source File
# Begin Source File

SOURCE=.\ExceptionHandler.h
# End Source File
# Begin Source File

SOURCE=.\ExceptionHandlerMsg.h
# End Source File
# Begin Source File

SOURCE=.\ExtendedGradeMap.h
# End Source File
# Begin Source File

SOURCE=.\FleetCard.h
# End Source File
# Begin Source File

SOURCE=.\FlowRate.h
# End Source File
# Begin Source File

SOURCE=.\ForecourtFuelingRestriction.h
# End Source File
# Begin Source File

SOURCE=.\ForecourtProductList.h
# End Source File
# Begin Source File

SOURCE=.\FuelingRestriction.h
# End Source File
# Begin Source File

SOURCE=.\GAIEventsHandler.h
# End Source File
# Begin Source File

SOURCE=.\GCI.h
# End Source File
# Begin Source File

SOURCE=.\GeneralStruct.h
# End Source File
# Begin Source File

SOURCE=.\GPI.h
# End Source File
# Begin Source File

SOURCE=.\GPIEventManager.h
# End Source File
# Begin Source File

SOURCE=.\Grade.h
# End Source File
# Begin Source File

SOURCE=.\Hydra.h
# End Source File
# Begin Source File

SOURCE=.\HydraPS.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\InitDebitHandler.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\Logger.h
# End Source File
# Begin Source File

SOURCE=.\MSXMLInt.h
# End Source File
# Begin Source File

SOURCE=.\Param.h
# End Source File
# Begin Source File

SOURCE=.\POSFuelingRestriction.h
# End Source File
# Begin Source File

SOURCE=.\PosInfo.h
# End Source File
# Begin Source File

SOURCE=.\PriceChangeFlags.h
# End Source File
# Begin Source File

SOURCE=.\PriceChangeWithAckPerPump.h
# End Source File
# Begin Source File

SOURCE=.\ProjectRemark.h
# End Source File
# Begin Source File

SOURCE=.\Promotion.h
# End Source File
# Begin Source File

SOURCE=.\ProtectedDataHandler.h
# End Source File
# Begin Source File

SOURCE=.\ProtectedReceipt.h
# End Source File
# Begin Source File

SOURCE=.\Pump.h
# End Source File
# Begin Source File

SOURCE=.\PumpClass.h
# End Source File
# Begin Source File

SOURCE=.\PumpLock.h
# End Source File
# Begin Source File

SOURCE=.\PumpLockByTank.h
# End Source File
# Begin Source File

SOURCE=.\PumpSrv.h
# End Source File
# Begin Source File

SOURCE=.\PumpSrvCP.h
# End Source File
# Begin Source File

SOURCE=.\PumpSrvDB.h
# End Source File
# Begin Source File

SOURCE=.\PumpTotalsXml.h
# End Source File
# Begin Source File

SOURCE=".\Q-DEX.H"
# End Source File
# Begin Source File

SOURCE=.\QDXMaintThread.h
# End Source File
# Begin Source File

SOURCE=.\QLogger.h
# End Source File
# Begin Source File

SOURCE=.\Queue.h
# End Source File
# Begin Source File

SOURCE=.\ReceiptExtraIndexFile.h
# End Source File
# Begin Source File

SOURCE=.\RegTable.h
# End Source File
# Begin Source File

SOURCE=.\RelativeDoubleRecord.h
# End Source File
# Begin Source File

SOURCE=.\Report.h
# End Source File
# Begin Source File

SOURCE=.\ReportGenerator.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RTEventsHandler.h
# End Source File
# Begin Source File

SOURCE=.\RTIEventsHandler.h
# End Source File
# Begin Source File

SOURCE=.\ServerInfo.h
# End Source File
# Begin Source File

SOURCE=.\ServerMain.h
# End Source File
# Begin Source File

SOURCE=.\ShiftDelivery.h
# End Source File
# Begin Source File

SOURCE=.\ShiftProcess.h
# End Source File
# Begin Source File

SOURCE=.\ShiftTanks.h
# End Source File
# Begin Source File

SOURCE=.\ShiftTotals.h
# End Source File
# Begin Source File

SOURCE=.\SinglePumpLock.h
# End Source File
# Begin Source File

SOURCE=.\SingleShiftAccumMgr.h
# End Source File
# Begin Source File

SOURCE=.\SingleShiftAccumulations.h
# End Source File
# Begin Source File

SOURCE=.\SiteConfigurationExporter.h
# End Source File
# Begin Source File

SOURCE=.\StaticString.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StoreLineOLA.h
# End Source File
# Begin Source File

SOURCE=.\Tank.h
# End Source File
# Begin Source File

SOURCE=.\TankGaugeFactor.h
# End Source File
# Begin Source File

SOURCE=.\TankLevelProtection.h
# End Source File
# Begin Source File

SOURCE=.\TankXML.h
# End Source File
# Begin Source File

SOURCE=.\TerminalMessages.h
# End Source File
# Begin Source File

SOURCE=.\Thread.h
# End Source File
# Begin Source File

SOURCE=.\TimeOutManager.h
# End Source File
# Begin Source File

SOURCE=.\TotalPumpInfo.h
# End Source File
# Begin Source File

SOURCE=.\Track2Receipt.h
# End Source File
# Begin Source File

SOURCE=.\TrsLog.h
# End Source File
# Begin Source File

SOURCE=.\WetStockDetails.h
# End Source File
# Begin Source File

SOURCE=.\XmlConvertor.h
# End Source File
# Begin Source File

SOURCE=.\XmlConvertorEx.h
# End Source File
# Begin Source File

SOURCE=.\XmlDefinitions.h
# End Source File
# Begin Source File

SOURCE=.\XMLInterface.h
# End Source File
# Begin Source File

SOURCE=.\XmlParser.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Ctrl.rgs
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\GCI.rgs
# End Source File
# Begin Source File

SOURCE=.\GPI.rgs
# End Source File
# Begin Source File

SOURCE=.\PumpSrv.rgs
# End Source File
# End Group
# Begin Group "Encryption"

# PROP Default_Filter ""
# Begin Group "DSS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DSSBothAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DSSBothAlg.h
# End Source File
# Begin Source File

SOURCE=.\DSSBothAsymmetricAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DSSBothAsymmetricAlg.h
# End Source File
# Begin Source File

SOURCE=.\DSSEncodeHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\DSSEncodeHandler.h
# End Source File
# Begin Source File

SOURCE=.\DSSEncodingAlg.h
# End Source File
# Begin Source File

SOURCE=.\DSSEncryptAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DSSEncryptAlg.h
# End Source File
# Begin Source File

SOURCE=.\DSSHashAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DSSHashAlg.h
# End Source File
# End Group
# Begin Group "DPS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DPSEncodingAlg.h
# End Source File
# Begin Source File

SOURCE=.\DPSEncryptAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DPSEncryptAlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BaseEncodeHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseEncodingAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PCIEncodeHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\PCIEncodeHandler.h
# End Source File
# End Group
# Begin Group "OLA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OLA.h
# End Source File
# Begin Source File

SOURCE=.\ola32.cpp
# End Source File
# Begin Source File

SOURCE=.\ola32.h
# End Source File
# Begin Source File

SOURCE=.\OLA32Events.cpp
# End Source File
# Begin Source File

SOURCE=.\OLA32Events.h
# End Source File
# Begin Source File

SOURCE=.\OLA32EventSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLA32EventSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLA32Srv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLA32Srv.h
# End Source File
# Begin Source File

SOURCE=.\OLABase.cpp
# End Source File
# Begin Source File

SOURCE=.\OLABase.h
# End Source File
# Begin Source File

SOURCE=.\OLAConnectedPayments.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAConnectedPayments.h
# End Source File
# Begin Source File

SOURCE=.\OLADotNet.cpp
# End Source File
# Begin Source File

SOURCE=.\OLADotNet.h
# End Source File
# Begin Source File

SOURCE=.\OLAEps.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAEps.h
# End Source File
# Begin Source File

SOURCE=.\OLAEserviceSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAEserviceSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLAEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAEvents.h
# End Source File
# Begin Source File

SOURCE=.\OLAHashAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAHashAlg.h
# End Source File
# Begin Source File

SOURCE=.\OLAPositive_FIPAYSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAPositive_FIPAYSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveEventSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveEventSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveIFSF.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveIFSF.h
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveUS.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAPositiveUS.h
# End Source File
# Begin Source File

SOURCE=.\OLAProgressSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAProgressSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLAProgressSrvEx.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAProgressSrvEx.h
# End Source File
# Begin Source File

SOURCE=.\OLARouteSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLARouteSrv.h
# End Source File
# Begin Source File

SOURCE=.\OLAService2EventsHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\OLAService2EventsHandler.h
# End Source File
# Begin Source File

SOURCE=.\OLASimLinkSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLASimLinkSrv.h
# End Source File
# Begin Source File

SOURCE=.\OlaSimulator.cpp
# End Source File
# Begin Source File

SOURCE=.\OlaSimulator.h
# End Source File
# Begin Source File

SOURCE=.\OLASkeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\OLASkeleton.h
# End Source File
# Begin Source File

SOURCE=.\OLATcpBase.cpp
# End Source File
# Begin Source File

SOURCE=.\OLATcpBase.h
# End Source File
# Begin Source File

SOURCE=.\OLATcpPositiveSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\OLATcpPositiveSrv.h
# End Source File
# Begin Source File

SOURCE=.\OlaTravelCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\OlaTravelCenter.h
# End Source File
# End Group
# Begin Group "convert"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Convert.cpp
# End Source File
# Begin Source File

SOURCE=.\Convert.h
# End Source File
# Begin Source File

SOURCE=.\ConvertXML.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertXML.h
# End Source File
# End Group
# Begin Group "VP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VP.tlb
# End Source File
# Begin Source File

SOURCE=.\VpMng.cpp
# End Source File
# Begin Source File

SOURCE=.\VpMng.h
# End Source File
# Begin Source File

SOURCE=.\VPThread.cpp
# End Source File
# Begin Source File

SOURCE=.\VPThread.h
# End Source File
# End Group
# Begin Group "Loaylty"

# PROP Default_Filter ""
# Begin Group "NMP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DeferralDiscountNMPDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\DeferralDiscountNMPDetails.h
# End Source File
# Begin Source File

SOURCE=.\NmpDetalis.cpp
# End Source File
# Begin Source File

SOURCE=.\NmpDetalis.h
# End Source File
# Begin Source File

SOURCE=.\phoneLookUpNmpDetalis.h
# End Source File
# Begin Source File

SOURCE=.\phoneLoopUpNmpDetalis.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CommandExecuter.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandExecuter.h
# End Source File
# Begin Source File

SOURCE=.\CopientCommandExecuter.cpp
# End Source File
# Begin Source File

SOURCE=.\CopientCommandExecuter.h
# End Source File
# Begin Source File

SOURCE=.\FuelLoyaltyPSR.cpp
# End Source File
# Begin Source File

SOURCE=.\FuelLoyaltyPSR.h
# End Source File
# Begin Source File

SOURCE=.\LoyaltyCommandExecuter.cpp
# End Source File
# Begin Source File

SOURCE=.\LoyaltyCommandExecuter.h
# End Source File
# End Group
# End Target
# End Project
# Section PumpSrv : {8B0F68C6-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:18:CLASS: CServerPump:CServerPump
# 	2:14:ServerPump.cpp:ServerPump.cpp
# 	2:12:ServerPump.h:ServerPump.h
# 	2:19:Application Include:PumpSrv.h
# End Section
# Section PumpSrv : {8B0F68C1-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:15:CLASS: CCmdInfo:CCmdInfo
# 	2:13:ENUM: Command:Command
# 	2:11:CmdInfo.cpp:CmdInfo.cpp
# 	2:9:CmdInfo.h:CmdInfo.h
# 	2:19:Application Include:PumpSrv.h
# End Section
# Section PumpSrv : {8B0F68C5-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:21:ENUM: PumpReservation:PumpReservation
# 	2:16:ENUM: PumpStatus:PumpStatus
# 	2:14:ENUM: PumpMode:PumpMode
# 	2:20:CLASS: CPumpTransact:CPumpTransact
# 	2:15:ENUM: TrsStatus:TrsStatus
# 	2:16:ENUM: PumpDevice:PumpDevice
# 	2:8:Pump.cpp:Pump1.cpp
# 	2:18:CLASS: CPumpStatus:CPumpStatus
# 	2:6:Pump.h:Pump1.h
# 	2:19:Application Include:PumpSrv.h
# 	2:18:ENUM: PostPayState:PostPayState
# 	2:12:CLASS: CPump:CPump
# End Section
# Section PumpSrv : {8B0F68C4-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:21:ENUM: PumpReservation:PumpReservation
# 	2:16:ENUM: PumpStatus:PumpStatus
# 	2:14:ENUM: PumpMode:PumpMode
# 	2:20:CLASS: CPumpTransact:CPumpTransact
# 	2:15:ENUM: TrsStatus:TrsStatus
# 	2:16:ENUM: PumpDevice:PumpDevice
# 	2:8:Pump.cpp:Pump.cpp
# 	2:18:CLASS: CPumpStatus:CPumpStatus
# 	2:6:Pump.h:Pump.h
# 	2:19:Application Include:PumpSrv.h
# 	2:18:ENUM: PostPayState:PostPayState
# 	2:12:CLASS: CPump:CPump
# End Section
# Section PumpSrv : {5C848061-2D0C-11D1-B8A5-0000E8D0CBF8}
# 	2:14:CLASS: CLogger:CLogger
# 	2:10:Logger.cpp:Logger1.cpp
# 	2:19:Application Include:PumpSrv.h
# 	2:8:Logger.h:Logger1.h
# End Section
# Section PumpSrv : {8B0F68C8-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:14:ThreadInfo.cpp:ThreadInfo.cpp
# 	2:12:ThreadInfo.h:ThreadInfo.h
# 	2:18:CLASS: CThreadInfo:CThreadInfo
# 	2:19:Application Include:PumpSrv.h
# End Section
# Section PumpSrv : {8B0F68C3-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:21:ENUM: PumpReservation:PumpReservation
# 	2:16:ENUM: PumpStatus:PumpStatus
# 	2:14:ENUM: PumpMode:PumpMode
# 	2:20:CLASS: CPumpTransact:CPumpTransact
# 	2:15:ENUM: TrsStatus:TrsStatus
# 	2:16:ENUM: PumpDevice:PumpDevice
# 	2:8:Pump.cpp:Pump2.cpp
# 	2:18:CLASS: CPumpStatus:CPumpStatus
# 	2:6:Pump.h:Pump2.h
# 	2:19:Application Include:PumpSrv.h
# 	2:18:ENUM: PostPayState:PostPayState
# 	2:12:CLASS: CPump:CPump
# End Section
# Section PumpSrv : {8B0F68C2-1A4A-11D1-B8A5-0000E8D0CBF8}
# 	2:9:PosInfo.h:PosInfo.h
# 	2:15:CLASS: CPosInfo:CPosInfo
# 	2:11:PosInfo.cpp:PosInfo.cpp
# 	2:19:Application Include:PumpSrv.h
# End Section
