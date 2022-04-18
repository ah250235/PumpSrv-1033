
		 	
// 2.0.2   
//					OLA basic tests, 
//					Update fix that applied to pumpsrv 1 (shift process)
//					New methods in GPI to support transaction close time.
//					
// 2.0.3			
//					Watchdog methods GCI,GCIEvents
//					New retcode to POS  - GPI_CONTROLLER_NOT_AVAILABLE
//					
// 2.0.4			 
//					Support Trendar as External Controller, 
//					New flag for InitInterface2 - INIT_CONTROLLER_NO_SERVICES
//					
// 2.0.5			 
//    				Support Trendar as External Controller, 
//					Release method depending on thread state 
//					
//					
// 2.0.6			 
//					In case PumpSrv deleted transaction, by auto detect of 
//					cancel 0 val transaction , and after a while 
//					got update - final information, PumpSrv reInsert 
//					the transaction into the list and act like the 
//					transaction was there.
//					This rule will be correct for final of lost transaction.
//					
// 2.0.7			 
//					A. Implement GCI::GetRealTimeClock
//					B. GPI::SetTrsState(0,0,0) - release only POS owned transactions.
//					
// 2.1.0			
//					A. Threads created with _beginThread instead of _beginThreadex 
//						(for automatic handle closing)
//					B. New OLA session handling 
//					C. New PumpSrv flag : AutoReceiptCheck 
//					D. New PumpSrv flag : AutoForce
//					
// 2.1.1			
//					A. In case of region =  UK use OLA.DLL  interface (IService)
//					B. In case of region != UK use OLASrv.EXE interface (IServices)
//					
// 2.1.2			
//					OLA New parameters - special for UK (CardType, CardIssue, StartDate)
//					
//					
// 2.1.3			
//					OLA New flaw control - Any OLASrv commands are activated from background 
//					from DoAutomaticOla.
//					Split PAPSwipe() handling into sub functions.
//					
//					
// 2.1.4			
//					PrePay update: PumpSrv update any change of a PrePay transaction 
//					also in PrePay file.
//					
// 2.1.5			
//					"out of range" values protection for fields :Grade, level, nozzle.
//						GCI::PAKReadyTransaction()
//						GCI::PAPReadyTransaction()
//						GCI:: TransactionStatusCM()
//						GCI:: TransactionStatusCS()
//					
// 2.1.6			
//						Handle outstanding cases in 
//						PAPReadyTransaction (such as FAILED_IO,TRS_NOT_FOUND)
//						Add detach of OLASrv (SmartPointer) done in COLASrv destructor.
//						
// 2.1.7			
//						PumpSrv as Slave - Detect cancel operation , and send Final as 0 value
//						PumpSrv as Slave - Fix in Receipt structure that returned to controller
//					
// 2.1.8			
//						Bug Fix - In query session there was no release of 
//						(in.out)BSTR after function call
//						Bug Fix - time out counting wasnt save between iterations
//						In Case of Timeout of session Receipt_auth do not switch to new session (auth)
//						
// 2.1.9				Add the following Parameters to the force Request
//						ReferenceNumber[S]
//						AuthorizationNumber[S]
//						ExpirationDate[E]
//						Approval[S]
//						IsStored[B]
//						GPI new mathod :GetParam			
//						Tag Pump Status 2 - include mode info
//			
// 2.1.10				Change the DoAutomaticOLA function to be more readable.
//						Fix stacked prepay issue limit with proper value.
//						Fix - Do not force a failed authorise Card.
//						
// 2.1.10.2				Extend debug, 
//						Reload PAP info at PumpSrv InitServer procedure.
//						
//						Temporary change :  at OLA->QuerrySession , 
//											do not release buffer if result is not one of:
//											PROCESS_FINISHED, NEED_MORE_PARAMS
//						
// 2.2.0				New methods 
//						
//						GPI::QueryPAPTrs()
//						GPI::QueryPrePaidTrs()
//						Updated methods  
//						GPI::GetPrePayTrs2()				
//						
// 2.2.0.1				Add Read Next in OLAFile.
//						
// 2.2.0.2				Fix locking issue in GPI::InitInterface
//						
// 2.2.0.3				Set BSTR to empty string before calling to OLA->QuerrySession
//						Add configurable parameter for OLAWaitResponse timeout.
//						
// 2.2.1.0				New methods 
//						GCI::SetParam 
//						
// 2.2.1.1				Fix - In case of club card , the second card is the 
//						
// 2.2.1.2				Fix - In OLA Authorize request change track1  from ABC to real Track1 
//						
// 2.2.1.3				Fix - In OLA, if Authorize failed ,Clear transaction
//						Fix - In OLA, if in middle of authorize session get 
//						PAPReadytransaction , close the session 
//													
// 2.2.1.4				GCI::GetGradePrice - fill up function.
//						GPI::GetParam change to handle one [in] BSTR sTableNameAndFieldName
//						Add new flag ControllerManagePumpLock , and if thats true, pumpsrv 
//						do not ignore unexpected messages such as :
//							PumpTotals, TankReading, DeliveryReports
//						Fix - Limit pumpSrv operation in case of ControllerManagePumpMode
//								(for example, do not auto authorize PAP, even if mode said so) 
//						
// 2.3.0.0				New Methods 
//						GPI::SetProcess	
//						GPI::GetProcess
//						New parameter - AlternateServerAutoSwitch 
//						Fix - protect from swipe during force.
//						OLA "Force" Operation change to be "Complete"
//						
// 2.3.0.1 				Fix - temporary fix to support Force and Complete at the same time 
//							will be removed once all OLASrv support "complete".
//						
// 2.3.0.2 				Remove temporary fix 2.3.0.1
//						Update m_cLogger.LogGCIReport to include new strings
// 2.3.0.3				Remove the OLASrv.Release() command
//						
// 2.3.0.4 				Accept Q_ERROR as valid qdex return code in DBConnectionAvailable.for 
//						index file only.
//						
// 2.3.0.5 				Update pump mode only if pump in working state 
//						Change Pump warning names to be PUMP_DEVICE_WARNING  
//						Return GetNumberOfTrsInList() in minimal pump info.  instead of array size.
//						New external parameter - OLACheckingInterval
//						Change field size , decrease filler 
//						CARD_SALE_EXTRA_DATA.VatRate  - Changed from 3 bytes to 4 bytes 
//							(Vat rate of 17.5 % passed as 1750	Or 6% passed as 600)
//						Parse data such as AuthorizationTime, Odometer, CarRegistration, 
//									Return tank - for cases	of ticket recover.		
//						
// 2.3.0.6 				Return PAPSwipeResult in case of CancelInMiddleOfAuthorize.
//						OLA parse the incoming AuthorizationDate and AuthorizationTime
//						SetPumpStatus - reduce debug information 
//						
// 2.3.0.7 				New method in CTRL:GetParam1(TableAndFieldName...) 
//		 				New method in CTRL:SetParam1(TableAndFieldName...) 
//						
// 2.3.0.8				Treat Null Res as PAK in case of Israel 
//						m_lWatchdogCount - Change server state to IDLE after 3 watchdogs.
//						Implement grade reastrictions : 
//									ValidDepList[S] = G01|G02,
//									InValidDepList[S] = G02|G05|G07|G10,
//						Remove external Parameter - Accept0Transaction.
//						Reject grade 0 in Set Grade Price.
//						
// 2.3.0.9				Apply fix for server state watchdog count. (protect sever state in critical section) 
//						(bug found in non reseting the counter) 
//						
// 2.3.0.10				CGPI::AuthorizePump, if transaction reservesion isn't PAK, Attend or Test
//						then don't return GPI_NOT_VALID_IN_CURRENT_MODE, instead keep going and 
//						authorize the pump
//						
// 2.3.0.11				Open the option to SetTrsState(Paid) with different POSNumber that is on offer 
//						(For save and recall purposes) 
//						
//						Extend GPI::StopPump(0) to stop all pumps at one call
//						Extend GPI::RestartPump(0) to restart all stoped pumps at one call
//						
// 2.3.1.0				GCI::TranasctionStatusCM - use CenrtralUpdate Index in order to solve the 3 transactions bug.
// 2.3.1.1				GPI::AuthorizePump - if pump contain "In Progress" transaction - do not allow to start new one.
// 2.3.1.2				Change fields sValue, sVolume in TAG_MINIMAL_PUMP_2 to be spare fields
// 2.3.1.3				In the call CGPI::GetMinimalPumpsInfo2, The field sPumpMisc in TAG_MINIMAL_PUMP_2  
//						return new flag indicating : MISC_PUMP_AVAILABLE_FOR_PRE_PAY		0x00000400
//						
//						In the call CGPI::QueryPrePaidTrs , 
//							1. if the parameter lPumpNumber is a valid pump number , 
//							the Query command return only records belong to the required pump.
//							2. A new flag is also defined, that indicate "QUERY_CLOSE_TRS", in that case the query 
//							command return only closed transactions
// 2.3.1.4				Return the PrePaid value at the transaction body in case of PrePay transaction
// 2.3.1.5				In case of Slave - When establish connection to server clear all known transactions
// 2.3.1.6				In the call CGPI::QueryPrePaidTrs , 
//							1. A new flag is also defined, that indicate "QUERY_CLOSE_TIMEOUT", in that case the query 
//							command return only CLOSED and timeout prepaied transactions
// 2.3.1.7				New counter at Minimal Pump - indicate number of Unpaid trs that Onoffer or accepted.
// 2.3.1.8				New flags at pump level - AllowPrePayOnRequestingPump, 
//						Set default Grade prices to be 100 cents each 
//						Extend TAG_MINIMAL_ALL_PUMPS_2 structure to include the Server and convert layer version.
//						
// 2.3.2.0				New method in CTRL:GetParam2(TableAndFieldName...) 
//		 				New method in CTRL:SetParam2(TableAndFieldName...) 
//						
// 2.3.2.1				New external Parameter that determine the OLASrv in use.
//						Support of a new OLASRV (posware) 
// 2.3.2.2				Bug fix -  Reuse of the same Misc flag
//						 
// 2.3.2.3				Pumpsrv retrive convert layer version number from GCI::InitInterface , 
//						and destribute it in any GetMinimalPumpsInfo.
//						
//						The Format is: 
//						sControllerName = XX2.3.10.1 Meggitt Site controller...
//						
//						XX= Version string length 				
//						2.3.10.1 = the Convert layer version
//						Meggitt Site controller...= Controller description
//							
// 2.3.3.0				New method in GPI:GetParam2(TableAndFieldName...) 
//						
// 2.3.3.1				Bug Fix, Protection against rolling update after 
//						transaction final information received 
//						Bug Fix - GetVersion eat memory.  
//						
// 2.3.3.2				Change Relative File to work with "long" offset.
//						
// 2.4.0.0				Wide changes to support NEW_OLA_STRUCT
//						Including basic ServiceMode support in CPumpStatus.
//						
// 2.4.0.1				Automatic detection of NEW_OLA_STRUCT, and rebuild of pumpstat.qdx file.
//						Save declined OLA transactions,
//						
// 2.4.0.2				Minimize some logging information 
//						
// 2.4.0.3				Remove transponder field from OLA.h 
//						Add AVI support :
//							AVISystemCustomer[I]=%ld,\
//							AVICompanyId[I]=%ld,\
//							AVIUserId[I]=%ld,\
//							AVISpecialCodes[S]=%.8s,",
// 2.4.0.4				Start support AVI
//						Add ReSendWithExtraInfo that used in controllers that cant apply the 
//						flag SWIPE_EXTRA_INFO 
//						Change the build request to use the new OLASRv::SetParam functions.
//						
// 2.4.1.0				Add SetShift flag FORCE_NON_LOCK, in case of system works in Locking , 
//						This open the option to close on the fly (after x retries...)  
//						Update shift number on final PAK (already exist in PAP) //			
//						Bug Fix, in case of grade price, PumpSrv not any more look at 
//						the next grade price Valid/Not Valid, instead there is a GradeCount that used by the 
//						Price change function.
//						Support Help Key. 
//						
// 2.4.2.0				Remove Support of Reservation PAY_AVI, 
//						Add the AVI as Pay at Pump Reservation Extension = RES_EXT_AVI.
//						New Methods: GCI::AVIStart, GCI::AVIStop 
//						
// 2.4.2.1				Bug Fix, in case of Card authorization decline due to Need More Param, 
//						PumpSrv write OLA record - which is wrong. 
//						Add new fields to CARD_SALE_DATA structure FinalAuthNumber,FinalRefNumber. 
//						
// 2.4.2.2				Change Version PosPump Version report mechnisam
//						
//						When getting POSPump version in GPI::InitInterface(),
//						POSPump version = lVersion div 100000000
//						POS version = lVersion mod 100000000
//						
// 2.4.2.3				Add new fields to CARD_SALE_DATA structure sResInfoAMax8, sResInfoBMax16
//						
// 2.4.2.4				Special Patch to be remove afte OLASrv fix.
//						
// 2.4.2.5				Bug Fix, Help/Cancel Key report.
//						
// 2.4.2.6				In case of cancle in middle of Authorize , the result should be 
//						CD_NOT_OK and not CD_FORCE_NOT_OK 
//						CD_NOT_OK will lead pumpsrv to write declined record in OLA.Qdx.
//						
//						if(bCancelInMiddleOfAuthorize)
//						 _Module.m_server.SendSwipeResult(ola.m_byStat,p+1, CD_NOT_OK,info.cardData.sTranStatus, &info.cardData); 
//						
// 2.4.2.7				In check of final PAP information arrive in middle of session ...
//						PumpSrv return GCI_PAP_REQUEST_OUT_OF_SYNC to the 
//						convert layer and ignore the request, this fix will help double PAPreadyTransaction 
//						calls from interrupting one each other.
//						
// 2.4.3.0				New mwthods in CTRL 
//						CTRL::SetRecord(), CTRL::GetRecord()
//						
// 2.4.3.1				Add New event PUMP_MISC_EVENT_CARD_SWIPE_ERROR
//						reported in case of card swipe error 
//						
// 2.4.4.0				Add Gas Rewards Redeem support
//						Remove general parameter sResInfoBMax16 from CARD_SALE_DATA 
//						and remove parsing of ResInfoBMax16[S]
//						
// 2.4.4.1				Add special detectionin case of "need more param" for empty fields only:
//						like PIN[S],PromptRewards[B], 
//						in this example if PromptRewards[B]=Y that means that 
//						this parameter already available
//						
// 2.4.5.0				- Calculation of transaction amount after discount:
//						
//						Fuel Volume:			20G		Trs.RoundedVolume = 20000
//						Price Per Gallon: 		2$/G	Trs.GradePrice = 2000
//						Fuel Value: 			40$		Trs.RoundedValue = 40000
//						GR Discount Per Gallon	0.10	CARD_SALE_DATA.sGRDiscountPerGallon = 100 
//						GR Max Volume 			10		CARD_SALE_DATA.sGRMaxAmont = 	10000	
//						GR Total Discount 		1$		CARD_SALE_DATA.sGRTotalDiscount	= 1000	
//						Transaction Total 		39$		CARD_SALE_EXTRA_DATA.sTotalAmt	= 39000
//						
//						- In case of GRPrompt = NO , stop Authorize session and send 
//						Swipe result to Pump.
//						
// 2.4.5.1				Add new String field that passed from OLASrv to PumpSrv MerchantID[S] 
//						
// 2.4.5.2				Fix to GetRecord return code 
//						
// 2.4.5.3				Fix to Mode Schedular 
//						
// 2.4.5.4				Logic Fix - 
//						Pump which sets to PAK auto authorize and have timeout PrePay transaction, 
//						will prompt the operator to choose between new transaction (PAK) or Re-authorize the PrePay
//						
//						Bug Fix in mode mechnisam (daily,weekly) 
//						
//						New GeneralParam : AllowAuthOnNonRequest , to allow authorize on non requesting pump
//						
// 2.4.6.0				Price Change with New Optional Logic ( Nozzle level Information)
//						
//						New GeneralParam : NozzleLevelLogic
//						
// 2.4.7.0				New Function: add ability to remove trapped transactions 
//						via GPI::SetTrsState with new State :STATE_FORCE_REMOVE
//						This poweful option should be used only in case of switching servers, 
//						and get trapped trasactions.
//						CServerMain::WideRemoveTrs
//						
// 2.4.7.1				Bug Fix: Save Card_Sale_Data information in case of GR shortcut.
//						The case:
//						When User answer "NO" for prompt rewards, PumpSrv act as if 
//						Authorized arrived and send SwipeResult OK immidiatly, in that case   
//						Card_Sale_Data should be saved in memory and in PumpStat.
//						
// 2.4.8.0				New Funcionality : Protect Pumpsrv from unsolicited AVI requests,( CheckAviConditions on any pumpstatus update.)
//						PumpTotals with New Optional Logic ( Nozzle level Information), in this case 
//						PumpSrv use the Nozzle and position table to get the grade.
//						
// 2.4.8.1				Bug Fix: Gas Rewards , did not update corectly the total amount in Info.ExtraData
//						
// 2.4.8.2				Change in Gas Rewards logic, the conver layer calculates the GRTotalDiscount, and 
//						GRTotalAmount and send them to pumpsrv via CARD_SALE_EXTRA_DATA in the PAPReadyTransaction Method 
// 2.4.8.3				Add New field in OLA : ActualDiscountPerGallon 
//						
// 2.4.9.0				New Logic to handle Cancel in Middle of Authorization
//						
// 2.4.9.1				Add try and Catch whenever trying to access GCIEvents
//						
// 2.4.9.2				Bug Fix, Change Qdex Return code on read operations fron Q-Error, to ERR_NOT_FOUND
//						(Match PosPump for Albertson 1.2.624 and higher)
//						
// 2.4.9.3				New Pumplevel parameter : PAKAuthorizedTimeOut 
//						use the General param PAKAuthorizedTimeOut as default value for the above new one.
//						If PAKAuthorizedTimeOut is zero, than pumpsrv do not check timeout 
//						
// 2.4.9.4				Bug Fix , In the CExceptionLog() constructor , call Init();
//						
//						
// 2.4.9.5				Extend with all changes M.L 2.4.4.1 
//							Extend Critical section in CServerMain::DoAutomaticOLA() 
//							LogLocationIn and Out in any function tha used by backgroung ExTrans and POS PrePay processes.
// 2.4.9.6				system level Try and Except 
//						
// 2.4.9.7				Release version with C++\General No optimize 
//						
//						Settings to support BugTrapper:
//						Link\General\Generate Debug info
//						Link\Costomize\Use program Database
//						Link\debug\ Debug Info, Microsoft format, without seperate types. 
//						
//						Write to log "GPI thread ID" , "GCI Thred Id", "CmdTx Thread ID"
//						Write to log the calling thread ID for each line (GetCurrentThreadId())
//
// 2.4.9.8  9.3.00		Special protection in GPI against Thread switching
//						Special protection in CTRL against Thread switching
// 2.4.9.9  11.3.00		remove critical section over ServerState
//						
//						
// 2.5.0.0  11.3.00		CHANGE ALL WideCharToMultiByte access,
//						CHANGE ALL MultiByteToWideChar access,
//						CHANGE ALL CCOMBSTR access
//						Bug fix:	do not complete PAP transaction 
//									that failed on authorize.
//						Add AuthTerminal[S] to parsaData and complete string Buffer 
//						
//						Protect against *BSTR in CTRL 
//						Protect against *BSTR in GCI
//						Protect against *BSTR in GPI
//						
//						if(*sVal)
//							*sVal = strRetVal.AllocSysString(); 
//						Send error E_POINTER in case of bad pointer in all methods 
//						of GetParam() (GCI,GPI,CTRL)
//						
//						Replace try & except with try & catch(...) 
//						in all methods, and add it also in WinMain.
//						
//						Bug Fix - in function overloading COlaSrv::SetParam
//						Bug Fix - remove Multi Threaded Mashler from GCI interface. 
//						Bug Fix - remove Multi Threaded Mashler from GPI interface. 
//						
//						Add CoInitializeEx() in CmdTx thread, that will make sure that the
//						thread joined the same Appartment where the GCIEvents interface created.
//						
// 2.5.0.1  17.3.00		Remove SysFreeString in all [in] BSTR of GCI,GPI,CTRL
//						
// 2.5.0.2  19.3.00		Clean version = remove all try/catch(...) 
//						OLASrv::BuildRequest , remove 150 bytes, spare bytes added for testing
//						Bug fix: on cancel GR, there where 3 transaction created instead of one 
//						
// 2.5.0.3  22.3.00		BUG Fix, In OLA,Complete session,send transaction info at 
//						the PAPSwipeResult. (Recipt structure) , it use to work 
//						only for the GetReceipt/ReceiptAuth session.
//						
//						
// 2.5.0.4  26.3.00		1. Log Decline, in case we waiting for more data, and the user cancel the operation.
//						2. Detect Network failure after Approval and PromptRewards = Y .. which result with CD_DECLINE_ERROR.
//						   In that case change sTranStatus to CD_DECLINE_GR, to get the same treatment.
//						
// 2.5.0.5	27.3.01		1. In case of changes from STOPED_BY_OPERATOR to REQUESTING - Cancel the transaction.
//						2. GPI::SetTrsState accept the State :STATE_FORCE_REMOVE with no encoding.
//						   (Must be use with GPIClient version 2.0.2.3 and up)
//						
// 2.5.0.6	28.3.00		1. GPI::SetTrsState accept the State :STATE_FORCE_REMOVE with no encoding.
//						   (Must be use with GPIClient version 2.0.2.3 and up)
//						2. In case we reached timeout while waiting for GR response, send CD_DECLINE_GR to CL,
//						   This will cause zero competion when we will reach to SEND_END.
//						3. Changes in case of SEND_END, due to timeout in while waiting for response,.
//						4. In case we are waiting for GR response and we reached timeout, send CD_DECLINE_GR 
//						   to CL, this will cause zero competion when we will reach to SEND_END.
//						5. Same as 7, in case of cancel detection while waiting for more information from CL.
//						
// 2.5.0.7	29.3.00		Changes for Rozman
//						1. CGCI::PAPReadyTransaction, replace nGrade with nFinalGrade
//						2. CGCI::PAKReadyTransaction, replace nGrade with nFinalGrade
//						3. On PAP Request/Response, send/recveive the authorizer version ("AuthVersion")
//						   on COLASrv::BuildRequest(..)
//						4. Instead of writing the grade position at the pump in the Registry,
//						   try to read them first and save the information in a table  at the pump level, 
//						   that save the linked grade to nozzle. ( write them only if they do not exist ).
//						5. At ParseData in the OLA response, if the flag NozzleLevelLogic is set,
//						   then update the field "sGradeRestriction" of CARD_SALE_DATA with the allowed nozzle
//						   according to the new table at the pump level, that save the linked grade to nozzle.
//						6. If the REGION parameter is set to "ISR" then use the field "sAuthAmt" of CARD_SALE_DATA
//						   with no decimal point, so max amount that this field will hold will be 99999 instead of 99.999
//						
//						
// 2.5.0.8	02.4.00		Update the meaning of the parameter "MaxPrePayTrs", to be the number of stacked 
//						Pre-Paid transaction allow. (If it 0, PumpSrv will not allow to stacked pre-pay
//						while there is transaction running at the pump).
//						
//						
// 2.5.0.9	04.4.00		1. In case the finalize of PAK transaction, does not match the current transaction
//						   know by PumpSrv, accept the transaction and replace between them.
//						2. In case the parameter NozzleLevelLogic, If the grades allowed are not present
//						   at the pump, do not authorize although the card was OK.
//						
// 2.5.0.10	12.4.00		1. Fix to the change at version 2.5.0.8, the problem was conflict with the parameter AllowPrePayOnRequestingPump.
//						
//						
// 2.5.0.11 25.4.00		In case we authorize prepaid transaction, and the transaction status was changes to 
//						IN_PROGRESS although no dispensing was made, Then in case the pump is stopped,
//						the transaction status will be change to WAIT_TIME_OUT.
//						
// 2.5.0.12 27.4.00		When pump status change from AUTHORIZE to REQUESTING and the reservation is PAK, Cancel the current transaction	
//						
//						
// 2.5.1.0	30.4.00		At UDF with Mobil system PumpSrv do not doing the authorization with OLA.
//						instead, Allied will do it. therefore the following changes were made:
//						1. New parameter to the registry, "OLAByController", if it set to 1, 
//						   PumpSrv ignore OLA (If this parameter is used, then the parameter 
//						   "OLAAutoForce" must be 0).
//						2. PumpSrv will generate transaction number when GCI::GetCurrentTransactionNumber(..)
//						   is called and the reservation is PAP. (Only if the parameter "OLAByController" is set)
//						3. Accept PAP transaction with any sequence number received from Allied. 
//						   (Only if the parameter "OLAByController" is set)
//						4. At the method GCI::PAPReadyTransaction(), will check the parameter lFlags
//						   that define which struct was sent at the parameter sData,
//						   the default is CARD_SALE_EXTRA_DATA (for backward compatibility).
//						
//						
// 2.5.1.1	01.5.00		Support Fleet Card
//						Those are the changes that was done to support Fleet Card:
//						1. Save in memory the parameters "Code" from the Registry at the grade level.
//						   This parameter will be use for NACS codes.
//						2. Send the following fields to OLA on Complete: ( X - in the fields name, is a number starting from 0 to ItemCount[I]-1 )
//						ItemCount[I] - Number of item in the transaction
//						ItemCodeX[S] - NACS Product Code (the parameter "Code" form the Registry at the grade level).
//						ItemCodeAmtX[F] - Transaction amount.
//						ItemCodeQtyX[I] - Quantity sold (In case of fuel transaction the value is 1)
//						ItemCodeWeightX[F] - Transaction volume.
//						
//						
// 2.6.0.0	15.5.00		Wet Stock for Tesco.
//						1. New methods at GCI, to support extra field sends by Meggitt
//							a. GCI::PumpTotals2(..).
//							b. GCI::TankReading2(..).
//							c. GCI::DeliveryReport2(..).
//							d. GCI::Alarm(..).
//						2. New methods at Ctrl, to support extra field sends by Meggitt
//							a. Ctrl::GetPumpTotals2(..).
//							b. Ctrl::GetTankReading2(..).
//							c. Ctrl::GetDeliveryReport2(..).
//							d. Ctrl::GetAlarm(..).
//						3. New QDX file was added to the system to save alerts received from the 
//						   controller via the new method GCI::Alarm(..).
//						
//						
// 2.6.0.1	16.5.00		1. Some of the MISC macros was define incorrectly, that cause for unexpected behavior
//						   when using them. The function GetProcessStatus(..) didn't work for that reason.
//						2. The parameter "PAKAuthorizedTimeOut" was moved (but not deleted) from GeneralParam to the pump
//						   level at version 2.4.9.3, Because it bake to confusion, I removed it totally from "GeneralParam"
//						
//						
// 2.6.0.2	28.5.00		1. New methods to control services handle by PumpSrv
//							a. Ctrl::StartService(..) - Start specific service or all of them.
//							b. Ctrl::StopService(..) - Stop specific service or all of them.
//							c. Ctrl::GetServiceStatus(..) - Get specific service status.
//						   Status of service can be Off, Idle or In middle of opeartion
//						   In case the status is "In middle of opeartion", the Stop method will fail
//						   with retcode CTRL_SERVICE_IN_MIDDLE_OF_OPERATION.
//						2. New method for car wash that not for release yet.
//						
//						
// 2.6.0.3	31.5.00		Car wash
//						1. New methods were added at GCI, GCIEvents and GPI, in order to control 
//						   GPI
//							a. GPI::GenerateWashCode(..) - POS ask PumpSrv to get wash code from the car wash machine.
//							b. GPI::GetWashCode(..) - POS query PumpSrv for the wash code.
//							c. GPI::CancelWashCode(..) - POS ask PumpSrv to cancel wash code.
//						   GCI
//							a. GCI::CarWashStatus(..) - Controller update with the car wash machine status.
//							b. GCI::WashCodeResult(..) - Controller response for the request for wash code, of for the cancel.
//						   GCIEvents
//							a. GCIEvents::GetNewWashCode(..) - PumpSrv ask Controller to get wash code from the car wash machine.
//							b. GCIEvents::GetCarWashStatus(..) - PumpSrv ask Controller for car wash machine status.
//							c. GCIEvents::CancelWashCode(..) - PumpSrv ask Controller to cancel wash code.
//						2. New index file - the file CarWash.Qdx was added in order to save all the request from the POS
//						   and to the controller. PumpSrv responsible for deleting record from the file 24 hours after their last
//						   update. (usually that last update will be after the POS query for the car wash code).
//						3. New parameter table was added: CarWash (with 1 record) and WashPrograms (with 16 records)
//						4. New class - the class CCarWash was added to handle the list of requesting for code and for canceling.
//						   it is also responsible for creating the registry entries and load requesting from the Qdx on recovery.
//						
//						
// 2.6.0.4	01.06.00	 New Methods at Ctrl and GCIEvents for getting wet stock data from the controller.
//						  a. Ctrl::GetWetStockData([in] long lWetStockFlags, [out,retval] long *pVal);
//						  b. GCIEvents::GetWetStockData([in] long lWetStockFlags, [out,retval] long *pVal);
//						
//						
// 2.6.0.5	11.6.00		1. Bug Fix: The group of pump saved at the lMisc field, that was done incorrectly.
//						2. New function Bstr2Buffer(..).
//						3. More updates to the car wash.
//						4. New field at the struct CARD_SALE_DATA, for batch and batch sequence number (Clark)
//						5. The field sMerchantID at the struct CARD_SALE_DATA, was extend by one byte.
//						
// 2.6.0.6	18.6.00		New entry at GeneralParam - "PpdPath", stand for POSPump Designer Path.
//						
// 2.6.0.7	20.6.00		Bug Fix: The new parameter for batch and batch sequence number (ver 2.6.0.5),
//						results with Dr. Watson.
//						
// 2.6.0.8	26.6.00		In case OLAByController is on: (At Allied lab)
//						1. Ignore the function: DoAutomaticOLA
//						2. Fix the function GCI::PAPReadyTransaction(..)
//							a. The transaction that was sent create new one, instead of finalize
//							   the current running transaction. (The reason: we use lTrsNumber as transaction number,
//							   but this transaction is always zero).
//							b. In case of cancel to the PAP transaction, there is no transaction
//							   number yet, therefore we replay with FORCE_NOT_OK.
//							   but since we do not responsible for OLA we also not responsible for the receipt.
//						3. The number of the grade that sent to the convert layer, will be the ordinal number
//						   and not the "Code" value. (the "Code" value, is the NACS code).
//						4. Change the parameter "ShortName3" under WashPrograms to be CCH_Product_Code (CCH stand for Credit Card Host)
//						5. Fix bug: At the method Ctrl::RemoveAlarm there was attempt to remove a "Pump Total" record.
//						
// 2.6.0.9	29.6.00		Fix car wash (At Allied lab)
//						
//						
// 2.7.0.0	09.7.00		1. Send the sales item at the pump to OLA at the completion.
//						   That include: Fuel, Car wash and discount
//						2. Add macros for Alarm Flags.
//						3. Fix the way the Delivery date and time are save to the Qdx.
//						4. Fix: AuthorizationDate[D], was saved to sDateTime, and immediately was delete.
//						
// 2.7.0.1	11.7.00		Dynamically response to changes of parameter at the registry.
//						This apply to the following parameters:
//						
// 2.7.0.2	21.7.00		1. Apply the option "Authorize all", in case the Pump Number sent at the method
//						   GPI::AuthorizePump(..) is zero.
//						2. Add new parameter to the Registry at the pump level - TerminalNumber. 
//						   In case there are OPT at the site this parameter indicates the terminal number that 
//						   controls that pump. (in case there are IPT at the site the it always one to one)
//						3. The parameters OLACheckingInterval and OLAResponseTimeOut under GeneralParam use to 
//						   be in scale of second, at this version it was change to millisecond.
//						   In order to be backward compatible, PumpSrv check those parameters value, and if 
//						   they was set in second it automatically change that to millisecond.
//						
// 2.7.0.3	21.7.00		1. Fix - When formatting the field CARD_SALE_DATA::sDateTime, the year field was the first 2 digit
//						   instead of the last 2 digit. (03/05/2000 was format to 03/05/20)
//						2. Fix - The method GCI::GetParam(..), didn't return GCI_FIELD_EMPTY when the field was empty.
//						
// 2.7.0.4	25.7.00		In case pre paid transaction is stopped by operator, do not change the transaction status to WAIT_TIMEOUT,
//						because that might be a controller error. Instead, only if the pump status was change back to requesting
//						we change the transaction status to WAIT_TIMEOUT,  because only then we sure that the pump was de-authorize.
//						
// 2.7.0.5  26.7.00		Changes for supporting ADS (At UDF lab):
//						1. Change the OLA string ItemCodeWeightX to ItemWeightX
//						2. Fix - The parameter GeneralID was received from the pump, but wasn't sent to the OLA Server. (Because it wasn't saved to the Qdx)
//						
// 2.7.0.6  31.7.00		1. Spelling change to the parameter CCH_Prodact_Code to be CCH_Product_Code
//						2. New field at CARD_SALE_EXTRA_DATA, sFuelAmt. 
//						   When there are item sold at the Pump: sTotalAmt is the total for the all Trs, and
//						   sFuelAmt is just the fuel. (if not they will be the same)
//						
// 2.7.0.7  09.8.00		1. New decline message from OLA - CD_DECLINE_NO_MEMBER (= '9').	In case OLA expect club member first.
//						2. In case OLAByController is on, the fields sAccountNumber and sAuthNumber are sent from Allied.
//						   therefore we get those field at GCI::PAPReadyTransaction(..), and we save then at CARD_SALE_DATA.
//						
// 2.7.0.8  15.8.00		1. New parameter at the "GeneralParam" entry - OLADeclineMessage, According to it CL will use (or not) 
//						   the decline message that it received from OLA Server.
//						2. New parameter at the "Pumps" entry - TerminalMessageLength, The length of the 
//						   terminal display should enter in this field. 
//						   The CL need to know that length in order to format the decline message it received from the OLA Server.
//						3. New field at CARD_SALE_DATA - sHostTerminalID. This field is received from OLA and it indicates the host 
//						   terminal that authorizes the transaction.
//						4. Fix to the function - SubSystemTime(..), the function get two structure of type SYSTEMTIME,
//						   and it calculate in millisecond the difference. And that wasn't happen correctly.
//						
// 2.7.0.9  17.8.00		1. OLA Server can ask for new field in the MORE PARAM sequence. The field is MediaType, in that case PumpSrv pass the value that 
//						   was sent with that field ('1' to '9') to the CL. 
//						2. Create new table "ScreenLayouts", to define different screens at the terminal. 
//						3. If PAP is not valid in the current mode return decline code CD_DECLINE_MODE, instead of CD_DECLINE_USEALT.
//						
// 2.7.1.0  22.8.00		OLA Server can ask for new field in the MORE PARAM sequence. 
//						The field is MediaType, in that case PumpSrv pass the value as is to the CL.
//						
// 2.7.1.1  30.8.00		Log declined card only if they were decline, and not if the
//						user canceled the transaction (unless it was already approved).
//						
// 2.7.1.2  03.9.00		In case OLAByController is on, the field sCardName is sent from Allied.
//						therefore we get this field at GCI::PAPReadyTransaction(..), and we save it at CARD_SALE_DATA.
//						
// 2.8.0.0	04.9.00		New method Ctrl::ReConfigure(..), for dynamically change parameters.
//						Currently only the following field are supported:
//						RE_CONFIGURE_TERMINALS - Just passed it to CL
//						RE_CONFIGURE_CAR_WASH - Stop/Start car wash if required.
//						RE_CONFIGURE_All - Isn't that obvious.
//						
// 2.8.0.1	05.9.00		1. New method GPI::AuthorizePump2(..), the method allow the following
//							(a) Authorizing all the pumps at certain group.
//							(b) Authorizing PAK with limit (by value or volume)
//						2. New method GPI::StopPump2(..), the method allow to stop all the pumps at certain group.
//						
// 2.8.0.2	14.9.00		1. New method GPI::GetPOSInfo(..), to return information about connected POSs.
//						   The return information is a string in XML format.
//						2. Changes for SpeedPass:
//						   (a) Add new Pay at Pump Reservation Extension, RES_EXT_RFID_CT (for Vehicle Tag) and RES_EXT_RFID_HT (for Hand Held). 
//						   (b) Apply all the AVI rule on transaction with such Reservation Extension. 
//						
// 2.8.5.0	21.9.00		New method Ctrl::SetMaintenance(..), Allow to Lock or Unlock the site.
//						
// ********* Version	s 2.8.5.1 to 2.8.6.2 should not be use, because PumpStat.Qdx file was corrupted *********
// ********* The str	uct PUMP_INFO size was changed from 600 to 604 *********
//						
// 2.8.5.1	25.9.00		Support for SpeedPass.
//						1. New parameter at the pump level - AntennaNumber.
//						2. New available reservation extension for PAP was added in case of hand and car tag.
//						3. Messages at the pump level, the POS get the message number when calling the method
//						   GPI::GetMinimalPumpsInfo2(..) and GPI::GetPumpInfo2(..).
//						4. The message will be remove using the method GPI::SetProcess(PROCESS_REMOVE_MSG,..).
//						
// 2.8.5.2	16.10.00	 1. Call GCIEvents::SetPumpMode(..) even if the PumpSrv is mode master.
//						 2. Add new flags for wet stock record. The flag indicate that the record was an on line request and not as part as shift process.
//						
// 2.8.5.3	30.10.00	 1. Cent per Litter: There is a new parameter at the Registry entry GeneralParam - 'DiscountByVolume'
//							For every PAP transaction PumpSrv will check for this value, if its not zero it will send it to 
//							the CL. The CL will take this amount off from every Liter\Gallons sold.
//							The discount can be a parameter as describe below, and it can also come from OLA ("GRDiscountPerGallon[F]"),
//							In that case PumpSrv will send to CL the higher one.
//						 2. After reconnection to the controller, ask for pumps status twice. (Because some time CL miss the first one).
//						 3. Add the new RFID messages from Mobil.
//						 4. New automatic process for getting totals (Tanks, Pumps, Delivery and Alarm). New parameters were 
//							added to the Registry under the 'GeneralParam' entry, in order to control this process.
//							'WetStockMode' - Wet Stock mode -  (0) None. (1) every X minute (2) At time HHMM.
//							'WetStockInterval' - Wet Stock interval - (in second), (Enable only in case 'WetStockMode' = 1.)
//							'WetStockTime' - Wet Stock Time - (HHMM), (Enable only in case 'WetStockMode' = 2.)
//							'WetStockDone' - For Internal use.
//							'WetStockFlags' - The processes that will run Pump totals (=0x00000001), Tank reading (= 0x00000002) Delivery (= 0x00000004) Alarm (= 0x00000008) Note that they can all be together.
//						
// 2.8.5.4	05.11.00	 1. New field at CARD_SALE_DATA - 'sClubCardTrack2Data'. This field will save the first track2 that was enter at
//							PAP transaction. In case of club card, it will be the track2 of that club card.
//						 2. New field at CARD_SALE_DATA - 'sIsOnLine'. This field will save the value that OLA server send with the field IsOnLine[B],
//							The value can be 'Y' if the authorization was on line and 'N' if it was offline.
//							PumpSrv will send this value on COMPLATION session, this way OLA will know if the authorization was made off or on line
//						
// 2.8.5.5	05.11.00	  Change reading prameter's regestry from reading by function SaveParam to reading with function LoadParam
//						  The change made in function "InitServer"
//						
// 2.8.6.0	12.11.00	 1. Remove the call to GCIEvents::SetPumpMode(..) that was added at version 2.8.5.2.
//							This addition was harmless, but useless as well.
//						 2. New Reservation Extension - RES_EXT_PAP_CANCELED.
//						 3. Accept PAP transaction that was authorized by the controller, and was converted to PAK.
//						
// 2.8.6.1	21.11.00	  1. Price Change:
//							 1.1 Mark the process as done (PriceChangeDone=1) right after the unlock all pump, instead of a minutes later.
//							 1.2 Do not allow to SetGradePrice(..), while price change process is in progress.
//						  2. Hot Pumping. (CANCELED PAP - means that pump was Hot Authorize, but the card wasn't approved)
//							 2.1 New parameter at 'GeneralParam' - 'HotPumping'. (Can be 1 or 0, default 0).
//							 2.2 Convert CANCELD PAP transaction to PAK. (In such case return GCI_TRS_CONVERTED_TO_PAK)
//							 2.3 Mark the transaction as CANCELD PAP.
//							 2.4 New misc flag at the pump level for pumps with current transaction that was CANCELD PAP.
//							 2.5 New misc flag at the transaction level for transaction that was CANCELD PAP.
//							 2.6 Don't allow to restart pump in case it was CANCELD PAP. 
//							 2.7 New entry at the registry under the Receipt entry - HotPumpingCount, HotPumping1, HotPumping2, HotPumping3
//								 those entry allow the user to define up to 3 line that will be print on the receipt in case the trs was CANCELD PAP.
//						  3. New return code at GPI::SetTrsState(..) - GPI_WAIT_FOR_CONTROLLER_TRS_UPDATE. PumpSrv counts 12
//							 watchdogs (1 minute) after reconnection. And in case it didn't found the transaction that the
//							 POS was string to change. After that it will return GPI_TRS_NOT_FOUND as it use to. (Only if PumpSrv slave)
//						  4. Add new PumpMessage - PUMP_MSG_CARD_ENCRYPTION, when the controller report about encryption problem
//							 CL will send it as a message, instaed of warning - PUMP_DEVICE_WARNING_CARD_ENCRYPTION.
//						  5. Fix: In case we allow to authorize non requesting pump, must check first that the pump isn't already reserved.
//						
// 2.8.6.2	22.11.00	  1. Fix: From version 2.8.5.5 The parameter PumpXX\KeyboardType was accidentally changed to be created as DWORD, instead of string.
//						  2. Fix: In case of 'Hot Pumping', do not try to authorize the pump after the pump was DISPENSING and return to be READY
//						
// ********* Version	s 2.8.5.1 to 2.8.6.2 should not be use, because PumpStat.Qdx file was corrupted *********
// ********* The str	uct PUMP_INFO size was changed from 600 to 604 *********
//						
// 2.8.6.3	27.11.00	  Fix for the corruption that was done at version 2.8.5.1. 
//						  Change the struct PUMP_INFO size back to 600 (from 604) and loose the change that was added to control the message from
//						  SpeedPass and the encryption error.
//						
// 2.9.0.0	27.11.00	  Add the message field that was previously added at version 2.8.5.1 and then removed at version 2.8.6.2
//						  When PumpSrv start it will check for the signature at the PumpStat.Qdx file, and if the signature is
//						  for version 2.4 then convert it to match version 2.9 and up.
// 2.9.0.1	28.11.00	  Accept PAP transaction that was converted to PAK by the controller. And remove the OLA Link number from the transaction.
//						
// 2.9.0.2	29.11.00	  Husky (Husky Lab, Calgary)
//							1. Send to OLA the pre-authorized amount at the complation session.
//							2. Cent per Litter:
//								(a) Remove this feature from the store level.
//								(b) Add parameter at the grade level, but without implementation first.							
//							3. Add new return code that might be return from OLA server OLASRV_NO_HOST(= -7).
//							   and add default for OLA return code, because some time OLA server group add new return code without let us know.
//							4. New field at CARD_SALE_DATA - sReceiptRestriction, this field hold a bitmap of fields that should be excluded from
//							   the receipt, in case CL is formatting the Receipt.
//							5. Fix: PumpSrv throw an exception at the method GPI::RestartPump(..) when the pump number is 0 (for all pump).
// 2.9.0.3	12.12.00	 
//							1. Fix Exception that occur while requesting authorize card in "hotauthorize"
//							   (function DoAutoOLAWaitResponse case OLASRV_COM_ERROR)						
//							2. In Table WashPrograms//WashProgram?? change default field info of FullName from  WashProgram01 to Program01.	
//						
// 2.9.0.4	31.12.00	  New methods at Ctrl and GCI - to support one more level at the Registry.
//							Ctrl::GetParam3(..)
//							Ctrl::SetParam3(..)
//							GCI::GetParam3(..)
//							GCI::SetParam3(..)				
						
// 2.9.0.5	01.01.01	  Update the parameter GeneralParam\EncryptionKey at the Registry, and update CL, when an update received from OLA Server.
						
// 2.9.0.7	23.01.01	  Fix: Cancel trs form PrePay.Qdx when performing PREPAY_CHANGE_TO_PAY_AT_KIOSK
						
// 2.9.0.8	25.01.01	  Fix: In case that OLA is by the controller, we implant the sequence number to CARD_SALE_DATA at the pump level.
//							   The search for the transaction is fail when transaction is canceled, and because of that the transaction is stacked in WAIT_PROGRESS.
// 2.9.0.9	30.01.01	  Fix: Update the change in 2.9.0.8 to really work.
						
// 2.9.1.0  08.02.01	  Fix :In SetGradePrice function we put in  field Code the Grade number.(The filed code is use only by b/O)
// 2.9.1.1  12.02.01	  1. Add new option in pumpsrv that limit the suppressed driveOff, by adding new fileds  in GeneralParam (SuppressedDriveoffTimeout). 
//							 this option work only after pump was  suppressed by the cashier. 					 					
//						  2. Add new fileds in struct CARD_SALE_EXTRA_DATA name sTotalAmt_Msb witch contain the high value of Total Amount 5 BYTE 
//							 and the LSB will be at sTotalAmt 						
//2.9.1.2    15.02.0	1 Fix: bug in case the OLACheckingInterval equel to zero (execption happend by divide by zero).
//						  Doug for ask for  bypass to check & update encriptionkey in middle of session NEED_MORE_PARAMS.
//2.9.1.3	 18.02.0	1 Fix:Bug 
//							1. on process of Changing price if one of the pump is in PrePaY (W_TIME_OUT) we could not
//							   reauthorize the pump and continue in price change.
//							2. on Crash in middle of try of price change (one of the pump is PrePaY W_TIME_OUT).
//							   The pump that Pumpsrv sucssed to lock become visual ready NULL_RES but ,inside of pumpsrv we could not 
//							   reserved 
//							3. Add new message of Stack PrePay To PosPump SetMessage(PUMP_MSG_STOCK_PRE_PAY); 	
//							4. Bug in on method AuthorizePump that allowed authorize pump, witch not define in Pump In View.   
//2.9.1.4	28.02.01	 Fix:Bug:
//						    1. Bug in mode, 
//								when the value PAKValid in not valid , we could authorize pump in mode PAKAuto,
//						        (know if PAKValid not valid we not allowed to do pak transaction)	  
//							2. in Debit (DUKPT) pumpsrv pass only the pin without ExtendPin therfor we could not authorize pump in PAP. 
//2.9.1.5	01.03.01	 Fix:Bug:
//							 1. we have some pump that the customer select the mode of refueling, 
//						     in this case he select the mode of pap and before pass the  credit card if he select to cancel the transaction, 
//							 pumpSrv do not cancel the open transaction because he we wait for PapReadyTransaction(0,0,0);
//							 			
//						
//							 2. In method GetPatam3(...) in create mode the default value  DWORD & String not set.( the value always set to zero because it's define only [out] param)
//2.9.1.6	15.03.01	 add new option to lock/Open the forecourt use method Ctrl::SetMaintenance(..). 
//						 
//2.9.1.7	18.03.01	 bug:Cann't execute pap when we have 4 deal of pak unpaid and one deal of prepay in wait timeout.
//2.9.1.8   20.03.01	  
						
//2.9.1.9   10.04.01	 Support another SpeedPass error messages (PUMP_MSG_TAG_READ).
//2.9.2.0   15.04.01	 display SpeedPass messages. 
//2.9.2.1   17.04.01	 1. option to close ForeCourte at end of shift( add new flags to SetShift(... DO_FORCE_LOCK_PUMP) .		
//						 2. Allow PAK with limit (AuthorizePump2(....))
//						 3. Move PrePay form pump to Pump
//2.9.2.2   02/05/01	 use 2 decimal point value for the authorize limit in card_sale_data instead of 3
//2.9.2.3	08/05/01	 Update Shift number befor insert Record to Ola.qdx (tesoro).	
//2.9.2.4	15.05.01	 bug: Hot Pump: In case Ola decline Pap Transaction and no fueling start. we do not clear the  misc and the following pap get it..		 	
//2.9.2.5	31.05.01	 Hot Pumping: use limit from LinkSrv (if received) //added in 3.0.1.61
						
//2.9.2.7	10.06.01	 shift out of sync (bstr contain zero field)
//						
//2.9.3.0	13.06.01	 Install version
//						
//2.9.3.1   27.06.01	 1.Create new log level for fcc debug.
						
//2.9.3.3  10.07.01		 see update in 3.0.2.1.
						
//2.9.3.4  07.08.01		 1. on ola problem , do not full the logs with exception (new parmater OlaMaxDeclineMessToDisplay).
//						 2. Pre-Pay trs which was in status Dispensing (And not Fueling) and the nozzle was return back, 
//							the Pre-Pay trs stay in state IN_PROGRESS and therefore it's never get the state WAIT_TIME_OUT.
						

// 3.0.0.0				New version for Husky
//						1. Add new extra data to record PAY_AT_PUMP_INFO_ for saving more data (change struct of qdx PumpStat.qdx).
//						2. Add to Fileds for table GradeXX  , TaxRate , DiscountPerUnit , MaxUnitForDiscount 
//						3. New methods at Ctrl::SetGradePrice2 to support the Tax issue, and the ‘Cent Per Litre’ issue
//						4. Add those field to the new structure at the OLA structure. Those field should be prompt for the user at the pump when ever OLA ask for them.
//								BYTE sUnit[10]; (OLA string - Unit[S])
//								BYTE sTripNumber[10]; (OLA string - TripNumber[S])
//								BYTE sReferenceNumber[10]; (OLA string - CustomerReference[S])
//								BYTE sTractorHub[10]; (OLA string - TractorHub[S])
//								BYTE sTrailerNumber[10]; (OLA string -  TrailerNumber[S])
//								BYTE sTrailerHub[10]; (OLA string - TrailerHub[S])
//								BYTE	sCustomerMessage[80];	(OLA string - CustomerMessage[S])
//								BYTE	sCouponMessage[80];	 (OLA string - CouponMessage[S])
						
//								CustomerMessage
//						
//							Loyalty:
//						
//								BYTE sDeclinedLoyaltyFromte		---> OLA ask Fromte:
//								BYTE sDeclinedLoyaltyResponse	---> Convertlayer respomse to request.
//						
//								There is 3 loyalty define 		
//										a. "DeclinedLoyaltyInvalid[B]"
//										b. "DeclinedLoyaltyExpired[B]"
//										b. "DeclinedLoyaltyWithPayCard[B]"
//						
//						
//						
//								BYTE sAccountType;		(OLA string - AccountType[S])	
//						
//						
//						  Also Add  promte to table Messages.
//						5.Encryption Key  (EncryptionKey  , MACEncryptionKey  , KMEEncryptionKey ) 
//							a. Add encryptionKey  to Table Generalparam for Staition encryption.
//							b. Add encryptionKey  per PumpXX 	
//						    The Encryption Could Update from  OlaServer in middle process  -1 (PROCESS_NOT_FINISHED).
//						6. Generate old coustomer receipt.
//						   add new add new dataBase Receipt.qdx number 9. The file will save all close pap transaction for period tiem define. 	
//						   To get receipt we add some method in :
//										lGPI:GetReceipt(....)    // locket special receipt
//										lGPI:ReceiptUpdate(....) // update that receipt was printed.
//						  
//						7. new parameret :
//								CreditLimitByUser	 default 0 if 1 ask client input Creadit limit 
//								DebitLimitByUser	 default 0 if 1 ask client input Debit limit 
//								MaxReceiptCopies	 Max Copy receipt to get to customer		
//								ReceiptRecordTimeout Max Timeto save The Receipt in Minute.
//						
//						
//	3.0.0.1				start ...(2000.02.08)
//
//	3.0.0.2  13.02.01	Fix: Do not send PAP approved result if the pump is AUTHORIZED or DISPENSING (fix for AVI handling)
//  3.0.0.3  08.03.01	Update changing for version 2....]
//						see change in version : 
//										     2.9.1.1
//										 	 2.9.1.2 	
//											 2.9.1.3
//											 2.9.1.4
//											 2.9.1.5

//  3.0.0.4  15.03.01	Update changing for version 2....]
//											 2.9.1.6
//											 2.9.1.7
//  3.0.0.5  15.03.01	Update changing for version 2....]
//											 2.9.1.8			
//  3.0.0.6  29.03.01   In case we fail in middle of dispensing PrePay & we get SetPumpStatus(4,6) instead SetPumpStatus(4, 999) 
//					    PumpSrv release the pump instead to continue.  	
//  3.0.0.7  18.04.01    Update changing for version 2....] 
//											 2.9.1.9			
//											 2.9.2.0			

//	3.0.0.8  22.04.01	(1) DELEK - support VIT conflicts (PAK and PrePay)
//						(2) support AutoAuthorize parameter for PrePay

//  3.0.0.9  07.05.01    Update changing for version 2....] 
//											 2.9.2.1			
//											 2.9.2.2			
//						Changing for Husky	
// 
// 	3.0.0.91 14.05.01	
//						1. Foramt track2, remove leading char ';' and ending char '?' if exist.
//						2. In Operation OLA_BY_CONTROL add on compelet the field that came from C.L. 
//						3. update changing for version 2....] 
//											 2.9.2.3
//						4. create dynamic buffer that save latenss three pap Transaction					
// 
//	3.0.1.1	 21.05.01	add new method IGCI::GetCardInfo to get card info from OLA 
//						add new flags to AuthorizeAnd Limit GRADE_PRICE_IS_POSITION_NUM		
//						
//
//	3.0.1.22 23.05.01	1. add new method IGCI::GetReceiptCounter to get central receipt counter
//						2. add new method IGPI::GetPumpInfo3 (supports new TAG_PUMP3 structure) 
//
//	3.0.1.3 23.05.01	1. add extarnal parmater to determinate the size of MemPump Array (m_dwMaxMemCreditDisplay).
//						2. change place of reading the path of PumpSrv.log (HomeDir to LogDir).
//						//3. the Amount in sAuthamt always ###.## get with two digit
//						
//	3.0.1.3 28.05.01	1.Update changing for version 2....] 
//												 2.9.2.1			
//						
//	3.0.1.4 28.05.01		remove the change from version 3.0.1.3.   
//						
//  3.0.1.44 28.06.07	  add new field to CARD_SALL_DATA sAppCodeElectr					  
//  3.0.1.45 20.06.01	  bug: in delete record from Receipt.qdx (DoAutomaticReceiptCheck) 
//	3.0.1.5 03.06.01	1.On Update from PumpSrv2 to PumpSrv3 Update Automaticly the field PrePayAuto when the 
//							field PrePayValid valid.
//						2.Extract ModeScheme From 4 to 32 & load the Table to memory.
//							 Add new fields psPeriodDateTimeStart, psPeriodDateTimeEnd that limit the modecheme activity.
//							 add new flags in modeschme base on yearly operation. 
//							 put All modeschme into CArray instead to read all modescheme from registry
//							 ( reading all table from reqistry take about 1.0300 second).
//							 Add 2 interface to ctrl :
//							 CCtrl::GetPumpModeScheme2(...)
//							 CCtrl::SetPumpModeScheme2(...)
//						
//	3.0.1.6 03.06.01	 1. Relese version to ALB (the new develop of ModeSchem).
//						 2. add new field sAppCodeElectr to struct CARD_SALE_DATA (use for UDF)
//						
//	3.0.1.61 7.06.01		1. fix GetParam3 (GCI & CTRL)
//							2. add OLA SESSION_SALE for DELEK Conflicts
//							3. implement 2.9.2.5 - Hot Pumping: use limit from LinkSrv (if received)
//						
//	3.0.1.62  7.06.01	  change default time for checking Period time in modeschem from 01/01/1900 - 1/1/2000
//	3.0.1.63 13.06.01	  Update changing for version 2....] 
//												 2.9.2.7			
//	3.0.1.64 18.06.01	  1.Add new flags tointerface AuthorizeAndLimit  & PAPReadyTransaction (SERVICE_CHARGE_BY_POS)
//							2.Replace between SpecialFee to GeneralFee 
//						
//	3.0.1.67 19.06.01	  changing in method GetServicefee, if the pump in mode Self service do not charge fee.
//  3.0.1.68 24.06.01	  1.Update changing for version 3....] 
//												 3.0.1.45
//							2. remove debug message CustomerMessage & CouponMessage
//							3. Update ModeScheme buffer only if ModeSchem is Valid
//  3.0.1.69 26.06.01		get CardType[I] field from OLA.
//  3.0.1.70 26.06.01	  1. add new device to l2PumpDevice (RFID_ANTENNA)
//							2. when we cancel Trs on PapReadyTransaction we try to read Grade 0. 
//							3. remove Scanning mode Scheme every 6 min. 
//						
//  3.0.1.72 27.06.01	  1.move down the loading Mode table in method InitServer().
//							2.remove debug message CustomerMessage & CouponMessage
//						
//  3.0.1.73 27.06.01	   dead lock in ValidityCheck 
//  3.0.1.8  28.06.01	   Install version
//  3.0.1.81 28.06.01	   extract the data OlaMeminfo from part of ola info to all information of ola
//  3.0.1.82  2.07.01	   1.bug: on building Ola messages on field ClubCardTrack2 & ClubCardNumber. 
//							 2.Bug:We always print more one copy receipt 
//  3.0.1.83  3.07.01	   1.Bug: at Mainteance of Receipt.qdx
//						
//  3.0.1.84  4.07.01	   Support new flags at PapReadyTransaction PAP_DECLINE_INFO  
//						
//  3.0.1.85 08.07.01	   1.fix Bug: close shift when the forecourte is close.  
//							 2.fix bug: set mode default to all pump in SetPumpMode. 	
						
////  3.0.1.85 09.07.01		1.if the ForeCourt was close in Maintenance, Do not try to open the forecurt in middle shift process. 
//							
//							2.bug:on Update counter by ReceiptPrinted method.
//							
////  3.0.2.0  10.07.01		Install version
//							
//    3.0.2.1  10.07.01 b	ug: when the forecurt is close,and we changing prices we open the forecurt.
//							
//    3.0.2.2  10.07.01		1. bug: decline message on Pap saving without SeqNumber.(with flags PAP_DECLINE_INFO)
//							2. Remove field MaxRceiptRecord
//							
//    3.0.2.21  10.07.01	 1.Service fee with calender. 
//							 2. add new method for Service fee: 
//									CCtrl::GetPumpModeScheme3(...)
//									CCtrl::SetPumpModeScheme3(...)
//							 3. add new method for define more misc
//								 GetMinimalPumpsInfo3
//								 new misc are:
//											1.	MISC_VIT_CONFLICT						 
//											2.	MISC_CREDIT_CARD_NOT_AUTHORIZED			 
//											3.	MISC_NEED_MORE_PARAM					 
//							
//    3.0.2.3  19.07.01		add PUMP_MSG_TAG_DECLINED (UDF)	
//							
//    3.0.2.4  06.08.01		Pre-Pay trs which was in status Dispensing (And not Fueling) and the nozzle was return back, 
//							the Pre-Pay trs stay in state IN_PROGRESS and therefore it's never get the state WAIT_TIME_OUT.										
//							
//    3.0.4.0  14.08.01		1. start sale product with avi. 
//							2. add OLASRV_POSITIVE_EVENTS
//							3. add sFuelAmt_Msb
//							4.For saleing product with Avi we add two method:
//								A. GetOLAMemInfo2() --> read information on Credit Sale.
//								B. SetMemOla()      --> update use Credit record.
//							5.Revove the init of PumpSrv with Avi flags.
//							6.Update changing for version 3....] 
//														   3.0.2.4			
//							7. update registry entries in CServerMain::GetReceiptCounter					
//							8. add MISC_APPROVED_PAP
//							9. fix bug at GetOlaMemInfo2					
//							10. GetOlaMemInfoStatus
//							11. add TAG_PUMP4
//							12. Update changing for version 2....] 
//												//2.9.3.0
//												//2.9.3.1
//												//2.9.3.3
//												//2.9.3.4
//							13. add validity check to GetServiceFee__ (CTRL, GCI & GPI)
//							
//    3.0.4.1 19.08.01		1. send AmountLimit[F] as is (not divided by 1000)
//							2. add sWaitForReportFlag
//							3. new OLA fields : FuelNetAmt[F], TransTaxPercent[F]
//							4. always use sTotalAmt & MSB for OLA TransAmount[F] field
//							
//    3.0.4.2 19.08.01		1. Add 1 interface to GPI :
//								CGPI::GetTankInfo(...).
//							2. Add real time tank info mechanism. 
//							
//    3.0.4.21 20.08.01		1. ERC catch errors on OLA ICallBack creation
//			   21.08.01		2. YA - Pumps Manual and Auto Mode.(comment1)
//							3. YA - Fixed the flag byMemoInProcess.(comment2)
//							4. YA - Fixed the flag byMemoInProcess.(comment3) 		
//							5  MAR- Fixed On move PrePay from pump to pump checking that in 
//									 the target pump as nozzle with the same grade  	
//							6. MAR- continue fix issue Move PrePay. 
//							7. ERC  update COLASrv::GetParam to recognize similar fields 
//							10.MAR  in move pre pay we did not check the pump mode  
//							11.ERC  add MaxCounter field under each ReceiptXX entry
//							12.MAR  In Move PrePay,if no Grade was set. do not check validity on Grade of Target Pump. 
//							13.MAR  In Move PrePay we received nozzle at lGradePrice and not Grade
//							14.YA - Add new MISC to PumpInfo for Manual or Automatic Mode.				  	
//				28.08.01	15.MAR  In Move PrePay use the Previous Flags instead the new one
//							16 MAR  1.New parmeter in Table GeneralParm\PaidAllAviTrs which Use 
//									   to blocked all sale of the current used Avi.
//									 2.in Move PrePay Do not allowed to move PrePay which in Use. 	
//									 3.add ServiceFee parmeter TAG_PUMP4.  	
//							17 ERC  fix GPI::GetGradePrices & CGPI::GetLevelPrice
//							18 MAR  Allowed Move PrePay only if the Pump in State Wait Timeout  
//							19 MAR	 if the flags PaidAllAviTrs is set, Block all same Avi at all pump
//				29.08.01	20 MAR  Bug: Fix develop 19. 	
//							21	MAR  In Move PrePay check, Pump Service Mode.	
//							22 MAR  Bug in Move Pre-Pay 
//							
//    3.0.4.22 30.08.01		release for tesaro    
//							1. Remove Prefix from track2, for reprint receipt.
//							
//	  3.0.4.23 03.09.01		1.BUG: PumpSrv Create duplicate AuthorizeAndLimit.in case The Pump Mode is PrePay only.
//							2.BUG: PumpSrv do not save to the dataBase the information on ticket offer therefore on crash the status are deleted
//							
//	  3.0.4.24 03.09.01		1. delete unnecessary fields in SESSION_GENERATE_REPORT
//							2  add grade to wait PrePay. 
//							
//	  3.0.4.25 03.09.01		1. Add grade to GetPumpTransact2 in case PrePay state is Wait timeout.
//							
//	  3.0.4.3  03.09.01		release version
//							
//	  3.0.4.31 06.09.01		1. we do not clean the misc MISC_CREDIT_CARD_NOT_AUTHORIZED at SESSION_COMPLETE.
//							2. Get the correct prices in CGPI::GetLevelPrice and CGPI::GetGradePrices
//							3. Add new flags to GetOlaMemInfo2 (OLA_MEM_WITHOUT_CANCEL) 
//							4. Open new entry in ReportArray for non-registered report requests
//							5. Bug:in change form manual PumpMode to Automatic mode in case the chage was for all pump
//							6. Support new flags (OLA_MEM_WITHOUT_CANCEL) in method GetOlaMemInfoStatus.				
//							
//    3.0.4.31 11.09.01		7. YA Add ManualMode Misc initialization to the InitServer.
//							8. Add new fields under Pumps\PumpXX : "BaudRate","DeviceAddress","Simulator","CommPort"
//							9. Add sInvoiceNumber[10] to CARD_SALE_EXTRA_DATA2_ (DELEK)
//			   15.10.01 1	0. YA Add new parameter in GeneralParam - WetStockClearTime(milliseconds). According to this value
//								if wetstockflag not clear we cleared it. This to handle problem when not recieving data from CL
//								not abling to continue another process - timer check in CmdTx.
//						1	1. YA - Add all the CRC registry fields into the InitServer.
//						1	2. YA - Add implementation in Ctrl interface to SetRealTimeClock.
//									 This method is sending GCI event to CL.
//	 3.0.4.5   08.11.01			ERC - add ItemCode0[S] to Authorize request ONLY if there is a "real" product		
//	 3.0.4.6   06.11.01			MAR - Support Pos Void.
							
//
//    3.0.1.0  24.10.01	 MAR 1. Create Thread which handle all to connection to OLASRV.
//							 2. In region ISR at SESSION_COMPLETE send the Total amount with 2 digit. 	 
//						     3. Pass the number of Current Hot Pump TRs in the Pump to the pos.
//						     4. Multiply the GRDiscountPerGallon by 100 in regain ISR. 
//
//	  3.0.9.21  18.11.01 MAR  Add new field to OLA (sAuthorizationSerialNumber, sAuthorizerId, sAuthorizerFileNumber). 
//	  3.0.9.22  18.11.01 MAR  Bug: we do not check the field AllowPrePayOnRequestingPump when we accept authorizeAndLimit messages.	
//	  3.0.9.23  18.11.01 MAR  Change the name of ola field from 3.0.9.21

//	  3.0.9.24  20.11.01 MAR  Bug:when we working with VitConflict we allowed to create two transaction of PrePay.
//	  3.0.9.25  21.11.01 MAR  Pass ola field ServiceFee[F] with precente of two digit.  
//	  3.0.9.26  21.11.01 MAR  Add Service mode to Pump Transaction. 
//	  3.0.9.27  21.11.02 MAR  In session complete, pass to ola the date & time when the ticket produced. 
//	  3.1.0.1   21.11.02 MAR  OLA Thread Install
//	  3.1.0.11  27.11.02 MAR  1.Bug in OLA Thread (cause no watchDog).  
//							  2.Report to the POS about unbalance PrePay in the system 
//	  3.1.0.12  27.11.02 MAR  1.Do not allowed move PrePay to Pump lock. 
//							  2 Do not allowed move PrePay to Pump  fueling PrePay in case ths system working with vitconflicte.
//	  3.1.0.2	29.11.01 ERC  carry forward 2.9.3.5 (Add Numeric keys under ScreenLayouts (COSTCO ADA))
//	  3.1.0.21	04.12.01 MAR  1.OLA thread parameter not set.
//							  2.find Problem with Ola.h size.  	
//	  3.1.0.22	05.12.01 MAR  Filed TransactionData[D] do not display correct
//	  3.1.0.3   05.12.01 MAR  1.Close OLA Thread Handle.
//							  2.Install version delek. 
//	  3.1.0.31   05.12.01 MAR 1.Dug: Allow creating two PrePay on Pump, when the Vit conflict Flags is set.
//	  3.1.0.4    11.12.01 MAR Install...(delek).
//	  3.1.0.5    21.12.01 MAR Problem with format of  Track2Format for rePrint Receipt.
//    3.1.0.51   17.12.01 EYS Added GetStationStatus - new interface in GPI.
//	  3.1.1.0	 18.12.01 YA  Added CtrlEvent.
//	  3.1.1.1	 23.12.01 ERC Add HUSKY PAP features:
//							  1. Add LoyaltyApplicable field under CarWash
//							  2. Use OLA_EXTRA_DATA for PAP sessions (in addition to CARD_SALE_ALL2)
//							  3. Add Pin and MAC support for debit sessions
//							  4. Send ItemLoyalAmtX and LoyaltyTransAmount to OLA according to "LoyaltyApplicable" of each grade
//							  5. FIX: time and price Receipt Restriction
//							  6. Add LangID to OLA for supporting French cards
//    3.1.1.11   27.12.01 EYS Added FORCE_PREPAY
//	  3.1.1.12	 30.12.01 YA  1.Add CEventManager class for handling the events.
//							  2.Add the EventsThread to ServerMain.
//	  3.1.1.201.01.02 MAR 1.Use field Car registration instead Track2 as key, when saveing AVI transaction to Receipt.qdx 
//
//	  3.1.1.21	 01.01.02     1.Do validity check to Authorize amount at PAP.     
//
//	  3.1.1.22	 01.01.02	  1.Support service fee for Two level FullService and SelfService 
//	  3.1.1.3	 09.01.02     1 Multiply the  preAuthorize amount by 1000 when we check the authorize Validity amount
//	  3.1.1.4	 09.01.02     1. Bug in Update parameter    SlefPapService in SetServiceFee2 

//	  3.1.1.4	 17.01.02 MAR	  
//							  1. Support discount Per Grade.
//							  2. Set PumpSrv to be in high priority in the system	
//							  3. Support Reprint receipt from the OleMemInfo.
//							  4. Send event on final PrePay trs(Delek) 		
//							  5. Create new Table Terminal. 
//	 3.1.1.41	 31.01.02 YA  6. Add Automatic Real time clock mechanism.
//							  7. Add the FuelEncrypt class.	
//   3.1.1.42    31.01.02 EY  8. Added new OLAMemInfo2__ with CREDIT\DEBIT support.
//                            9. Added the method QueryReceipt to the GPI interface.
//                           10. Corrected CReceiptFile::Operation()
//
//   3.1.1.43    04.02.02	  1 .In session complete pass the fields AuthorizationDate[D] , AuthorizationTime[T].
//							  2 .Bug in Pump Total index, when the index turn over to one, 
//							  3 .Add new Monitor class (CComChecker) which help to Fcc to knew if PumpSrv was load. 
//							  4 .PumpSrv didn't open new transaction, when he get final PAK without Reservation.  
//   3.1.1.5    06.02.02      check version !!!!

//	 3.1.1.51    17.02.02	  1.Remove the filed sResInfoAMax40 from;
//							  2.Support only 16 Grade in Als,   	

//   3.1.1.52    19.02.02     1.Added call to ReleaseService to ~CServerMain(), in order to ensure death of OLA with events.
//	
//   3.1.1.6	 19.02.02     1.Qdx file name changed form 'MSC' to ‘RFS’  
//							  2.Add AVI type to shift log.
//
//   3.1.1.61	 19.02.02     1.Remove field TerminalNumber from Table terminal.
//							  2.Add mew field LoyaltySwipePrompt, LoyaltySwipePrompt to table messages 
//
//   3.1.1.62    21.02.02     1. Corrected default registry fields to suit RFS :
//                               a. Terminals - CommPort , DeviceAddress , Type , OfflineTimeOut
//                               b. Pumps - CommPort
//
//   3.1.1.63    24.02.02     1. Added Grades\Grade##\ConvertCode2
//
//   3.1.1.64    24.02.02	  1  bug in record size of EXTRA_INFO_REC.
//
//   3.1.1.65    26.02.02	  1.Add new Filed to OLA PromotionNumber[S].
//
//   3.1.1.66	 27.02.02	  1.Add new fileds “SignatureSlip[B]" , PriceLevel[I]  to Ola. 
//							  2.bug. Auto authorize of PAP. not working if exist waiting PrePay.
//							  3 Extend the size of "sPromotionNumber" field.
//	
//   3.1.1.67	 27.02.02	  1.Remove Misc Need MoreParam in final Transaction. 
//							  2.After printed first receipt, update in Receipt data that Original copy was printed.
//							  3.Change the definition parmeter "lIndexNumber" in method QueryExtraInfoDatato In/out.
//							  4.Remove track2 Messages from PumpSrv log. 	
//
//   3.1.1.68    03.03.02     1.Fixed bug:PrePay can be moved to locked pump (now not possible).
//						 
//   3.1.1.69    03.03.02     1.Add new field ReportNozzleOut to Pumps Table.
//
//   3.1.1.70    05.03.02     1.Remove Debug message,
//							  2.delete Loyalty flags in final Pak with loyalty.
//							  3.Allowed  GetReceipt only if the Server is IDLE.
//
//   3.1.1.71    06.03.02     1.Problem in received the actual receipt copys.
//	
//   3.1.1.72    06.03.02	  1.Addnew field TanksUnitsSetup to Tank table.
//							  2.Remove Track2 string form logs 
//
//   3.1.1.73    07.03.02	  1.Save field sClubCardTrack2Data in session Add param.
//							  2 add new file to Ola struct sResInfoAMax37.
//   3.2.0.0     07.03.02	  Install
//
//   3.2.0.1     11.03.02     1.Added support for 32 tanks, including backwards compatability
//
//   3.2.0.11    11.03.02     1.Bug:in the session Complete in Meggitt environment we do not Passing TransAnount.
//							  2.Convert ‘Y’ to ‘1’ and ‘N’ to ‘0’ in field sSignatureSlip.
//
//   3.2.0.2    11.03.02      Release version.  
//
//   3.2.0.21   11.03.02      1.Save field LoyalId in session Add param.
//
//   3.2.0.22   12.03.02      1.Remove Debugs messages.
//							  2.Change concept of Update Receipt Record &  Support new flags RECEIPT_ORIGINAL_PRINTED in GetReceipt.
//								
//	 3.2.0.23   12.03.02      1.GetTankInfo all tanks bug fixed.
//
//   3.2.0.3    13.03.02      1.Added TLSCommand method to Ctrl interface.
//                            2.Added TLSCommand method to GCIEvents interface.							  
//
//   3.2.0.31   13.03.02      1.Add new field "IdleIPT" to table Messages.
//
//   3.2.0.32   13.03.02	  1.Update loyalty activity from Pos.
//
//   3.2.0.33   18.03.02	  1.Bug ,we insert into field sSignatureSlip null.
//
//   3.2.0.34   19.03.02	  1.In session complete pass to Ola the filed sPriceLevel.
//							  2.Support receiving Wetstock information in middle of shift (Tankreading, DeliveryReport, PumpTotal.) 
//							  3.Bug Problem with wet stock ( Pump total , Tank reading , Delivery ) information when the index turn on form 99999 to 1. 
//							  4.Add new field to Ola sPriceRecalcDiscount, sAVIFuelCode. 
//	 3.2.0.40				YA -  5.Add new field to Ola sGradePrice.
//   3.2.0.5				YA -  1.Build the Paths Entry in PumpSrv Initiation.
//								  2.Add new field to Pump entry:PumpVolumePrecision default = 3.
							
//   3.2.0.51				Eyal  1.Added log writing to QueryReceipt.
//   3.2.0.51				Mar   2.Bug, in update receipt index record, at method PrintReceipt2.
//								  3.Support new Ola Tag "ReceiptMessage[S]"	,  
//							      4.Fix bug MovePrePay to pump with zero prepay allowed.
//							
//   3.2.0.6				Eyal  1.Added extra protection to QueryReceipt
//							      2.Removed log writing added in last release in QueryReceipt
//							      3.Removed temporary log writes in ReceiptPrinted.
//							
//   3.2.0.61				Marik 1.Add new field to Ola sTotalDiscount.   
//							
//   3.2.0.62				Marik 1.On final PAKReadyTransaction allowed to update only extData.
//   3.2.0.63				Marik 1.Remove changing from 3.2.0.63
//   3.2.0.64				Marik 1.Send completion Zero in decline card
//								  2.If PAK transaction was closed, and the POS try to pay again the transaction, PumpSrv will approved the operation (GPI_OK) insted decline (GPI_TRS_NOT_FOR_SALE).  
							
//   3.2.0.65				Marik 1.Create new terminal IPT table.
//								  2.In completion Zero pass to OLA  the field TransAmount[F].
//								  3.Bug in serviceFee
//   3.2.0.66				Marik 1.Bug at completion zero.
//							
//   3.2.0.67				Eyal  1.Changed BuildRequestReceiptInfo to return a long
//							      2.Added log writing in CReceiptFile::InsertReceipt()
//							
//   3.2.0.68				Marik 1.Change The scenarios or Zero completion 3.2.0.66.
//								  2 Fix bug, OLA index Pass The Max Counter number.
//   3.2.0.69				Eyal  1.BuildRequestReceiptInfo: added check for max receipt reprint
//							      2.Changed GetReceipt__ and GetReceipt2__ according to changes
//							        made to BuildRequestReceiptInfo
//							      3.Change registry: GeneralParam\OlaWaitingMessTimeOut changed to 3000
//   3.2.0.73				Marik 1.extand the size of PAP Pre authorization amount from 5 digit to 10 digit.
//							      2.Add new file to Ola sTotalBalance
//							
//   3.2.0.74				Eyal  1.WAIT_TIME_OUT now causes a transcation to be
//							        counted as a waiting pre pay.
//							      2.IsPumpIdle now checks for open transactions on pumps.
//							      3.GetStationStatus writes idle pump maps to log.
//							      4.CLogger creates log directory if it does not exist,
//							        according to path in PumpSrv\Paths\Long in registry
//							Marik 1. Add second to AuthorizationTime.  	
//							
//   3.2.0.76				Marik 1.Pay Attend...
//									In region ISR divide TransAmount by 10.
							
//   3.2.0.9				Marik 1  Send to OLA the completetion Time.
//   3.2.1.0				Marik  1. Fix Bug in discount per Grade Module.
//   3.2.1.10				Marik  1. continue Attend develop .
//   3.2.1.11				Marik  1. Bug In Cancel Transaction in middle of authorize. 
//							
//   3.2.1.2				Marik  1.Release version    
//   3.2.1.21				Marik  1.Transaction that decline by PumpSrv on out of balance  (Transaction value - service fee <= 0) remaind as W_progress.
//   3.2.1.22				Marik  2. see remark 3.2.1.11
//   3.2.1.23				Marik  1.New falgs In PumpSrv (GeneralParam\ForceCreditLimitCheck) that limited The credit limit to low value between Credit Limit and the Device Limit
							
							
//   3.2.1.31				       1.See 4.0.0.5.
//   3.2.1.32				Marik  1.bug, in module limited site by Value see 3.2.1.23
							
//							
//							Marik  2.bug, in module limited site by Value see 3.2.1.23.
//								   3.divide TransAmount by 10 in Region= ISR when Pumprv send data to OLA , 	
							
//   3.2.1.36				Marik  1 see 4.0.0.57
//								   2 see 4.0.0.57
//								   3 Support change from MaxVolumeForVITConflict to MaxVolumeForAVIConflict.	
							
							
//	 3.2.1.41				Marik  2.Bug,when we use the flags ForceCreditLimitCheck and the limit request is zero we need to take the deafult site limit  
							
//	 3.2.1.43				Marik  1.When the Pump Status is change, save this status to dataBase as well.
//   3.2.1.44				Marik  1.Copy the reference number in case the customer cancel transaction in middle of authorize,   
//								   2.Drive off appears on restart pump after stoped.
//								   3.Accept force prepay final information in recovery Pump.  
//								   4.Vit conflict that occurs in middle of OLA process OR fail in SALE session will convert to PAK transaction    	
//   3.2.1.46				Marik  1.Report to POS about any onoffer transaction in Pump. 
//								   2.Allowed to close the site in middle of Memo wet stock process.
//   3.2.1.49				Marik  1.Allowed to Offer to difference transaction in the same PUMP.
							
//   3.2.1.50				Marik  1.Vit conflict Transaction not signed as AVI transaction. 
//								   2.Do not close shift if the Pump is reserved for dynamic memu.	
//								   3.bug, Prepay transaction that was charge service fee, sign as unbalance prePay  
//   3.2.1.51				Marik  1.Parse Media type in usual session.
							
//   3.2.1.52				Eyal   1.Modified CReceiptFile::Reorganize() to delete MAX_RECEIPT_INDEX / 2
//							         records if the first free index is greater than 240
//   3.2.1.53				Eyal   1.Reset the receipt event before entering wait, so that
//							         if the event was signalled by some unexpected receipt
//							         we won'y print old info strored in pos.
//   3.2.1.54				Marik  1.Accept vit conflict transaction with no Transaction number. 
							
//   3.2.1.58				Marik  1.After OLA exception PumpSrv closes the connection to OLA, but not try to recreate the connection again.
//   3.2.1.59				Marik  1.Fix, Post void.
//								   2.Decline Authorize Pump when transactin in IN_PROGRESS state.
//   3.2.1.60				Marik  1.PAP Transaction which was decline with nozzle up,cause to Pump to remain in reservation PAK . 
							
//   3.2.1.63				Marik  1.Clear any waiting trs when Pump return to IDLE state.
							
//	 3.2.1.71				Marik  1.FIX,force  PAP transaction to forecourt limit 
//	 3.2.1.72				Marik  1.FIX,force  PAP transaction to forecourt limit 	
//							
//	 3.2.1.73				Marik  1.In AVI Transaction Take the Transaction limit form device and not form the creditLimit
//	 3.2.1.74				Marik  1.Add new reservation extension RES_EXT_CASH_AVI_CANCELED. to prevent auto authorize pump. 
//	 3.2.1.75				Marik  1.When PAK buffer is Full, and the PosPump try to  Authorize Pump with flags Avi cionflict, PumpSrv will decline the authorization 
//	 3.2.1.76				Marik  1.Remove MISC "Creadit Card not Authorize" when Pump stat change to authorize 
							
							
//   4.0.0.2				New version for with MultiSync support.
//   4.0.0.21				YA -  1.Add the support in MultiSync.
//								  2.Cut the InitServer to tables.
//							
//   4.0.0.22				Eyal  1.Added Polymorphic OLA classes
//							      2.Added SwitchServer to IGPI
//							      3.Added new CLog and revised CLogger.
//   4.0.0.23				YA	  1.Add ConsoleMsg(...) call in CLogger writing methods. 
//   4.0.0.24				YA	  1.Add New Methode to CTRL:ResetQdxInfo(...).
//								  2.Add all the support for this methode in ServerMain.
//								  3.Cut into Mehtode some InitServer data.
//							
//   4.0.0.25				Eyal  1.Revised SwitchServer__():
//							        a.CServerMain::SwitchServer__(long) now called from CmdTX thread.
//							        b.CGPI::SwitchServer__(long , long) checks parameter validity only.
//							      2.Added long CServerMain::lNewOLAServer.
//							      3.Added CRITICAL_SECTION CServerMain::m_csNewOLAServer.
//							      4.Added GetNewOLAServer(), SetNewOLAServer(long), with critical 
//							        section protection of m_lNewOLAServer.
//	4.0.0.26				YA    1.DoAutomaticPrePay__ only if server is in IDLE state
//	4.0.0.27				YA    1.Update version form PumpSrv till 3.2.0.68
//							
//  4.0.0.28				Eyal  1.COLAPositiveEventSrv : enabled running with no events
//  4.0.0.29				YA    1.Add some missed code from PS 3.2.0.68
//							
//  4.0.0.30				Eyal  1.Added some missing code from PS 3.2.0.67
//							      2.Reset OlaWaitingMessTimeout default value to 3000
//							      3.Changed CCtrl::SetServiceFee2__ to include changes from 3.2.0.65
//							
//  4.0.0.31				Yaron 1.In ParseData for GRDiscountPerGallon[F] replace the l2a to l2a0
//								  2.Fixed the bug with WeeklyModeScheme.
//								  3.YA - Replace the Queue.h.
//  4.0.0.32				Yaron 1.Add support to OLA32.exe with events.
//  4.0.0.33				Eyal  1.Modified OLA32.exe support to support late binding
//							      2.Changed CServerMain::SwitchServer__ and CServerMain::CreateOLAServer
//							        to work with with new method, CServerMain::OLAClAssFactory.
//  4.0.0.34				Eyal  1.Added checks to SwitchServer related methods.
//							      2.Changed reg entry used by TCP OLA to PSRemote_OLA.
//  4.0.0.35				Yaron 1.Add methods to CHydraPS class for monitoring HydService statuses.
//								  2.Not allowing PS to become idle if Hydservice not completed updating data.
//									Setting watchdog count back to 1. 
//  4.0.0.36				Yaron 1.Fixed the bug with the SetStatus(UNKNOWN_STATUS) while Loading PumpStat after
//									the HydService event.
//								  2.CParam:GetRecord(...) set the default flag value to HOP_DIRECT_LOCAL.
//								  3.GCI:SetPumpStatus(...) add SavePumpInfo(...).
//								  4.Change the HydService interfaces by adding support to lClientEventData. 
//  4.0.0.37				Eyal  1.Added log writing and changed receipt deletion condition
//							        from AND to OR.
//							      2.Corrected READ_SEQ in CReceiptFile::Operation.
//							      3.Corrected QueryReceipt to comply with the above
//  4.0.0.38				Yaron 1.Fixed the bug in the return codes of CParam when Hydra is active.
//							Eyal  1.Fixed bug in CLogger so that log dir can be changed from FCC
//							
//  4.0.0.39				Marik 1. Bug in Startsession in COlaRoutSrv.
//								  2. Update Husky verion. 
//								  3. Support init debit
//  4.0.0.40				Yaron 1. Update also the FormalShiftNumber when getting the event. 		
//  4.0.0.41				Marik 1 Update Husky develops      
//							Marik 2 Support New decline for Speed Pass ( RES_EXT_RFID_CT_CANCELED , RES_EXT_RFID_CT_CANCELED)   
//  4.0.0.42				Eyal  1.Changed warnings in CLogger from miscs to SystemError
//  4.0.0.43				Marik 1. Do not save decline transaction on receipt Qdx. 
//								  2. Extend Odometer Field to 8 digit.
//  4.0.0.44				Eyal  1.Fixed "pre-pay when mode is not auto auth" bug.
//							      2.Fixed bug in AuthorizePump and AuthorizePump2
//							      3.Disk checking interval shortened when disk is full
//							      4.Removed loading of 4 params in Receipt in registry
//  4.0.0.45				Yaron 1.Add IsLocalOnlyParameter check in CParam.
//								  2.Change CmdTx processes only if server is in IDLE state.
//								  3.New method in Ctrl interface:CCtrl::UpdateControllerSoftware(...).
//								  4.Cut the RT event to 3:PumpStat,Server and Shift info.
//							
//  4.0.0.46				Marik 1. Update PumpSrv with changing form PumpSrv 3.. (
//								  until	 3.2.0.9. withount Develop from Attend
//	4.0.0.47				Yaron 1.Change the Special registry field check mechanism.
//							Eyal  2.Copied changes from versions 3.2.0.92 and 3.2.0.93	
//							Yaron 3.Check in ResetQdxInfo(...) PUMP_NOT_VALID only if lFlags1 & ResetPumpInfo. 
//								  4.Add the MaxMessageLineLen key to the Messages table.
//								  5.Delete the TerminalMessageLength key from Terminals and IPT tables.
//								  6.Add the MaxReceiptLineLen key to the Receipt table.
//	4.0.0.48				Marik 1.Add Speed Pass decline.   
//								  2.Update version 3.2.1.0 
//							Yaron 3.Change the Tax in WashProgram to TaxRate. 
//								  4.Change in the method CCarWash::LoadCarWashDefinition()- Remove the check if
//									if the tables exsits and change SaveParam to LoadParam with bCreate=TRUE.
//								  5.Marked out the "BOOL OLAStopped()" in the middle of CCtrl::StartService__(...).
//							
// 	4.0.0.49				Marik 1.  Update PumpSrv with changing form PumpSrv 3..
//										3.2.1.10		
//										3.2.1.11
//										3.2.1.2
//										3.2.1.21
//										3.2.1.22
//							
//  4.0.0.5					Eyal   1.Revised CServerMain::DoAutomaticPrePay(), in order to
//							         stop transaction authorized on a requesting pump from
//							         getting stuck in W_RES or W_AUTH
// 	4.0.0.51				Marik 1. Update PumpSrv with changing form PumpSrv 3..
//									 3.2.1.23.
//									 3.2.0.76			
//							
// 	4.0.0.52				Marik 1. Update PumpSrv with changing form PumpSrv 3..
//									3.2.1.31
//									3.2.1.32
//								  2. change field name CheckVITConflict to CheckAVIConflict. 		
//							Eyal  3.Updated more changes made in version 3.2.1.32
//							      4.Added conditions to CGPI::IsPumpIdle()
//							
// 	4.0.0.53				Marik 1. Init Debit module.
//								  2. Add missing field AVIFuelCode. 
//							Eyal  3.Changed directory creation in CLogger::Init()
//							      4.Changed directory creation in CServerMain::LoadPathsTable()
//							
// 	4.0.0.54				Marik 1.Fix bug in PAK Preset.
//								  2 Signed session that was recover after OLA fail. (OLA field SessionOffLine[B]) 	
//							
//  4.0.0.55				Eyal  1.Support change from CheckVITConflict to ChangeAVIConflict
//  4.0.0.56				Yaron 1.Add m_cHydra.CloseConnectionToHydra() to PumpSrv distructor.								
//  4.0.0.57				Marik 1.Bug PumpSrv do not send Session Number in debit shift.
//								  2.Bug,QueryReceipt did not display all  Receipts
//								  3 bug, Allowed to move PrePay to pump without checking that the target pump has the specific 
//							
//  4.0.0.58				Marik 1 Update PumpSrv with changing form PumpSrv 3..
//									3.2.1.36  
//							
//  4.0.0.58				Marik 1.Remove option to do Setshift with debit flags.
//							
//  4.0.0.59				Eyal  1.Revised CGPI::QueryReceipt__()
//							
//  4.0.0.60				Marik 1.Bug.in Update CarWash parameter from the BO.  
//  4.0.0.61				Eyal  1.Continued bug fix from 4.0.0.52 section 3
//							
//  4.0.0.62				Eyal  1. Added BOOL with default TRUE, signifying whether to
//							         write a message to the console as well as to the log.
//							      2. Reduced number of console messages with the help of
//							         change outlined in (1).
//							      3. Console now opens AT THE END of CServerMain::InitServer()
//							      4. Changed CServerMain::BuildReceiptRequest()
//							      5. Fixed bug in CGPI::QueryPrePaidTrs__(), to comply
//							         with FCC->Viewers->PrePay->"Paid timeout trs" button
//  4.0.0.63				Yaron 1. Fixed the bug in CCarWash::LoadCarWashDefinition():Enabled with dTmp = 0 after reading.
							
//  4.0.0.64				Marik 1.In Session PreAuthorize send to OLA flags ItemIsFuel. 
							
// 	4.0.0.65				Marik 1.Add session Capture - Attenadant cash paid
//								  2.Add session Clock   - Attendant clock. 		
//								  3.Change the concept of open log File, the change was done in class Clog in method OpenLogFile.
//								  4.Add new method GetReceiptRawData3 to Gci interface. 
//									
//  4.0.0.66				Yaron 1.Support Database new configuration for the backup.
//								  2.Fixed the Move Prepay Trs duplication when Trs not found - 3.2.1.42
//								  3.Add the Group Number when saving shift number while shift process.	
//							Marik 4.Add debug information on "Force remove" transaction to exception file. 
//								  5.Update changing 3.2.1.311 & 3.2.1.312
//							Yaron 6.Fixed the shift change bug using MultiSync.
//								  7.Fixed the Lock and Unlock bug using MultiSync.
//								  8.Add registry events to all Qdex indexes.
//							Marik 9.Clog did not raise event on diskfull. 
//								 10.Support OLA string TimeOut.  
//							
// 	4.0.0.69				Marik 1.Convert Attendant Transaction to PAK Transaction.
//							Yaron 2.Change that all pump number in the log file will be 2 digits (Pump 01,02...)
//							
// 	4.0.0.70				Marik 1.Add new field to OLA struct sToken. 
// 	4.0.0.71				Marik 1.Reorganize the GCI interface to be backwards compatibility.(CL 4.0.4.3 (Allied)
// 	4.0.0.72				Marik 1.Change Attendant transaction to PAK.
//								  2.Add new file (sTotalBalance) to Ola.h struct to be backwards compatibility.   	
//							
// 	4.0.0.73				Eyal  1.Added support for no OLA configuration.
//							      2.DoAutomaticOLA exits immediately if no OLA configured.
//							      3.Added registry param PrePayEnabled.  DoAutomaticPrePay()
//							        exits immediately if this param is set to zero.
//								  4.Generate receipt with buy@bump items.  	
//							Marik 5.Update PumpSrv with changing form PumpSrv 3..
//							        3.2.1.43,
//									3.2.1.44.
//									3.2.1.46			
//									3.2.1.49	
// 	4.0.0.74				Marik 1. bug, init debit parameters
//							
// 	4.0.0.75				Marik 1.Return Buffer ALL_CARD_SALE2 in method GetCardInfo.
//								  2.Update PumpSrv with changing form PumpSrv 3..	
//									3.2.1.50
// 	4.0.0.76				Marik. Continue develops bay at pump. 
// 	4.0.0.77				Marik.1.Update PumpSrv with changing form PumpSrv 3..
//							        3.2.1.51,
//								  2. Allowed cancel Attendant sale session.
//								  3. In Attendant vit conflict always  charge AVI. 
//							
// 	4.0.0.78				Marik.1.Support Tank information TcVolume. (Tankreading3)
//							Eyal  2.Import changes from 3.2.1.52 (CReceiptFile::Reoragnize)
//							Marik 3.We did not report correct on Hande Tag.  
//							Yaron 4.Update the Shift number in the Pump data when the 
//									pumpstat is not configure to update pump info.
//								  5.Add new OLA decline. (OLASRV_Decline_NotInValidRang)		
//							
//  4.0.0.79				Eyal  1.Support 64 pumps!
//								  2.Add comma char to end of buffer in method GetCardInfo. 	
//								  3.Add new ola fields sBarCode, & sMaskShow.
//								  4.Add fields ReWardPerCentUnit, ReWardDays to Grades table , and Buy@PumpActive to GeneralParam. 	
//								  5.Update PumpSrv with changing form PumpSrv 3..
//							        3.2.1.53,
//							        3.2.1.54,
//							
// 	4.0.0.80				Marik.1.Update PumpSrv with changing form PumpSrv 3..
//							        3.2.1.58,
//								  2.In session need more param save the filed sReserved, sEntryMethod as well.
//							
//  4.0.0.81				Eyal  1.Added protection against bad registry config, where
//							        last valid pump is greater than max pumps.
//							Marik 2.When we use PostVoid, we didn't allocate special transaction for using Postvoid.     
//							Marik 3.Creating Real-time DB number in database root, instead in the directory file.
//	4.0.0.82				Yaron 1.Not saving Tankreading to qdx if came from the MemoInterval process.
//							Eyal  2.AuthorizeAndLimit returns GPI_NOT_VALID_IN_CURRENT_MODE if prepay
//							        not enabled.
//								  3.Update OLAMemInfo with Attendant sale.
//	4.0.0.83				Marik 1.Add log writting in Relative write.
//								  2.Update PumpSrv with changing form PumpSrv 3..
//							        3.2.1.59,
//							        3.2.1.60,
//							Yaron 3. 3.2.1.61
//								  4.Add Data entry in the Path table.
//							Eyal  5.If HomeDir exists in Paths Table, copy it to DataDir
//							      6.Added protection against invalid version info.
//							      7.Added protection against UNPAID PREPAY - a transaction
//							        created as a result of a call to PAKReadyTransaction
//							        will now end as a PAK transaction no matter what.
//4.0.0.84					Marik 1.Add new flags "NotAllowedWaitingPrePay" to  general\Param table.  When this flags is set every waiting PrePay will be set to WAIT_TIME_OUT
//							
//4.0.0.85					Marik 1.change field name  from NotAllowedWaitingPrePay to SetWaitingPrePayToTimeOut.
//								  2.On created field DataDir add to the path the directory "Data".
//4.0.0.86					Marik 1.Add new OLA string DeclinePaymentRetry[B],  
//								  2.Support change price by with xml file. 
//							
//4.0.1.0					Eyal  1.Fix bugs related to Pumpsrv 64 pumps backwards 
//							        compatability, to ensure proper work with old versions
//							        of FCC and POSPump.
//							Marik 2.Add new OLA string RemainingBalance[F] , thie filed will contain the remaining  card balance.   
//							Marik 3.Convert Vit conflict transaction which fail to PAK. 
//							Marik 4. Add Sleep between table Pump & Modeschme when Registration is done.   
//							
//4.0.1.1					Marik 1.Add OLA string CahrgeTo[I]. 
//								  2.Remove old debug message.	
//								  3.POS not in view success to give authorize to PAK preset.
//							Eyal  4.Fixed bug in ver 2 --> ver 4 (64 pumps) update.
//							      5.Fixed bug in 64 pump update, where pumps 33 to 64
//							        would get unknown shift number because shift info
//							        was not updated yet.
//							
//4.0.1.2					Marik 1.FIX bugs related to HOT pump, we did not signed the hot Pump cancel falgs   
//4.0.1.3					Marik 1.OLA string "Chargeto" send value 0 , instead empty buffer.
//							
//4.0.1.31					Marik 1.Rewrite function Track2Format to be pure C without any use of MFC CString. 
//								  2.Add new OLA string ZipCode[I], 
//								  3.Bug in function Track2Format when the control character  are in middle of track2 
//								  4.Update PumpSrv with changing form PumpSrv 3..
//									3.2.1.63	
//							Eyal  5.Changed MAX_PUMPS back to 32, and defined MAX_PUMPS_64
//							Yaron 6.Add tank number to log writing Delivery & Tank reading "not available".
//4.0.1.32					Yaron 1.Add the Price Poles registry table.
//								  2.Add to CTRL::ReloadPricePolesData(...).
//								  3.Add to GCIEvents::GeneralSystemEvent(...).
//								  4.Support two GeneralSystemEvent:GCI_EVENT_SHIFT_STARTED				1
//																   GCI_EVENT_RELOAD_PRICE_POLES_DATA	2
//							Yaron 5.Price in the xml with decimal point.
//								  6.Fixed the price change xml.
//								  7.Add support new flag in GPI::GetTankInfo(...) - REFRESH_TANK_INFO.
//								  8.Add to CTRL::SetRealTimeClock support for time recieved in the sDateAndTime (YYYYMMDDHHmmss).
//							Marik 9.Fixed,PrePay Trs that closed after dispensing and before getting report on Rolling update(Allied).not closing correct. 	
//								 10.Create new table taxs.
//								 11.Add new ola string Reversal.
//								 12.Open attendant transaction cause to PumpSrv to craete duplicate transaction. 
//								 13.Remove OLA string Reversal.
//								 14.PumpSrv did not clear the Memo interval flags when TankReading get TANK_READ_NOT_AVAILABLE
//								 15.Fix: The method ResetQdxInfo did not clear server info.
//							Eyal 16.Added support for ShiftTanks.qdx , ShiftPumps.qdx , ShiftDelivery.qdx
//							     17.Added class CRelativeDoubleRecord to encapsulate writing to above QDEX's
//							     18.Added following methods to ICTRL:
//										a.GetPumpDevices
//										b.GetTankInfo
//										c.GetPumpTotalsInfo
//							            d.GetDeliveryInfo
//								 19.Fix:If receipt buffer returned from reprint request is empty, return
//							        GPI_NOT_FOUND (48).
//							     20.Fix:Remote OLA looked in wrong registry entry.
//								 21.Pass Avi fuel code in session  SALE .
//								 22.Add new Taxes field to OLA.h.
//								 23.Track2 will be always the key to reprint Attendant receipt.
//								 24.Change site device configuration in run time, (dispenser, avi, price ploes,tank gauge)...
//								 24.Add new ola string ForamtedAcct[S]. 	 	
//4.0.1.33					Marik  1.Version send to Q.A.
//							
//4.0.1.34					Marik  1.expand Qdx item to be 3 record.
//								   2 Fix :vit conflict with attendant transaction.
//								   3 Allowed Attendant transaction according to Mode attendant Valid
//								   4.Attendant Authorize trs will move to timeout according to parameter PAKAuthorizedTimeOut 
							
//4.0.1.35					Marik  1.Pass pump total sale in Method GetOLAMemInfo2.
//								   2.Update the parameter m_cPrevSysTime in initserver.	
//								   3.Add new Event  GCI_EVENT_SHIFT_END to General System Event.  	 
//								   4.on creating new Attendant trs , cheak is there space for new incomming trs. 
//							
//4.0.1.36					Eyal   1.Fix: prepay not allowed on pump if trs list is full, if system
//							         AVI conflict configured
//							       2.Replaced all declerations of CString in AuthorizeAndLimit()
//							         with one central decleartion of a single CString
//							
//4.0.1.37					Marik  1.Support Attendant mode Valid/auto authorize.
//								   2.Clear PAK authorization while conflict with PAP Transaction. and change the transaction to PAP.  
//4.0.1.38					Yaron  1.Fixed in SetParam(...) AmountLimit[F] - %03ld to %02ld	
//							Eyal   2.Added string table resource to log
//							       3.Changed return codes in QueueHandle for reprint receipt
//							       4.Changed one call to LogResult in QueueHandle to support
//							         new string table usage
//							       5.Added calls to CSingleLock::Unlock for UpdateLastCmd() and
//							         RemoveCmdFromQ()
//							       6.Changed RestartPump__() so a restarted pump with a trs
//									 IN_PROGRESS doesn't update the trs time, so we get
//							         a drive off warning instantly.
//      					Marik  7.Add new ola string "BarCode[S]" 
//								   8.Support cash acceptor.	
//							Yaron  9.Add VP connection and Disconnection managment.
//							Eyal  10.Added call to AllowAccess() in _tWinMain, effectively
//							         turning security off (no authentication, no impersonation)
//							Yaron 11.Add backup (MultiSync) support to:
//									 A.Mode definitions updates.
//									 B.ModeScheme difinitions updates.
//									 C.Moving from Automatic Mode to Manual and bakc.
//								  12.Fixed the bug:when asking for delivery report using GetWetstock(...),
//									 data didn't enter into the qdex file - WDM 876.
//								  13.Block the ctrl event sending for tank read if was due to MemoInterval process						 
//								  14.Update PumpSrv with changing form PumpSrv 3..
//									 3.2.1.71 	
//									 3.2.1.72 	
//									 3.2.1.73 	
//							Eyal  15.Fix bug in logs (disk warnings were immediately erased)
//							      16.Fix bug so that space checking is done where log is
//							         being written, even if log is default.
//							      17.Fix bug: if log is default, shift changes and changes in
//							         date do not create new log files.
//							      18.Added pure method CLog::InitWorkingDirectory() to force
//							         future developers to initialize m_sWorkingPath member
//							      19.Added support for Nozzle Left Out:
//							         a. New method CServerMain::CheckNozzleLeftOutTimeout().
//							         b. Call new method from CmdTx every 5 seconds.
//							         c. New parameter: GeneralParm\NozzleLeftOut(Min).
//							         d. New miscs: MISC_NOZZLE_LEFT_OUT_TIMEOUT and
//							                       MISC_NOT_NOZZLE_LEFT_OUT_TIMEOUT.
//							      20.Changed misc MISC_NOT_PREPAY_TIMED_OUT from !(0x00400000)
//							         to ~(0x00400000)
//							Marik 21.Add new OLA string CompletedSessID.
//							Eyal  22.Added support for forced pump maintenance unlock.
//							      23.Fixed bug in allocation of array in CServerMain::SendGradePrice().
//							Yaron 24.Add support to RFS & TankSrv fields not to backup when working with MultiSync.					 
//							marik 25.Update PumpSrv with changing form PumpSrv 3..
//									 3.2.1.74 	
//									 3.2.1.75 	
//									 3.2.1.76	
//							
//							marik  26.Create new fields in table DataBase ShiftDeliveryTotals,ShiftPumpTotals, ShiftTankTotals 
//4.0.1.39					Marik  1. Release version
//4.0.1.391					Marik  1.fix spelling mistak in OLA string FormattedAcct[s]. 	
//							
//4.0.1.40					Eyal   1.Added support for pump diplay info at POS
//							       2.Changed GetPumpInfo3():
//										a.Shortened the code.
//							            b.Added support for terminal message events.
//							       3.Fix: CServerMain::ResetPSPumpInfo() - changed memset from 0 to spaces
//							Yaron  4.Implement the GCI::SetAlarm(...)
//								   5.Add new Method QueryData to interface GCI.
//							Yaron  6.Add support to warning at the pump via the SpeedPass simulator.
//							Marik  7.Remove Support region ISR from multiply\divide value. 
//								   8.Connect to AlarmSrv via Thread handle,instead direct connection.
//								   9.Define new patmeter value AlarmSrvActive=2, for sending direct Alarm/Warning to AlarmSrv.
//							      10.When we using parameter Lock shift,lock server fro geting any transaction 
//								  11.Remove debug message "(5)PUMP_WAIT_FIRST to PUMP_NONE_FIRST" 
//								  12.Update fix form 4.0.1.391. 
//								  13.Set in GetParam3Level(...) to work with HOP_DIRECT_LOCAL) flag for RFS.
//4.0.2.0					Eyal   1.Fix: Clear terminal messages when end messages event is sent
//4.0.2.1					Marik  1.release version, this version is good to RFS version (2.1.2.0)  
//4.0.2.11					Marik  1.Send to OLA string LangID  
//							
//4.0.2.12					Eyal   1.Added DELETE share mode to CLog::OpenLogFile()
//4.0.2.12					Marik  2.Convert LangID value form alfa to number
//							
//4.0.2.13					Marik  1.SavePAPInfo with default flags,instead direct flags
//							Yaron  2.When working with MultiSync not backuping "UseBackUp" for DataBase\OLA.
//							Marik  3.When we  Parsing the fields MarginUnitPrice, ScripUnitPrice we lost the accuracy value 
//							Marik  4.Remove the validation on ReConfigure method. 
//							Eyal   5.Added new MISC: MISC_AVI_NOT_AUTHORIZED, in order to allow
//							         POSPUMP to know which decline icon to display without 
//									 checking the pump's reservation.		
//							Marik  6.Add new method GetPumpModeScheme3, SetPumpModeScheme3 to GPI.
//							Marik  7.Add new flage (GPSD_FOR_CURRENT_PAP) to method GetPAPSwipeData to retrieved debit info.
//							Eyal   8.Fix: do not raise MISC_CREDIT_CARD_NOT_AUTHORIZED if 
//							         session type == COMPLETE.
//							       9.Fix: CServerMain::UpdateParam() reset wrong parameter
//							         for WetStockMode
//4.0.2.14					Marik  1.Release version
//							
//							
//4.0.2.15					Marik  1.Fix,log message  in method SetPumpModeScheme3.
//							Yaron  2.Not updating other computer with the field "GeneralParam\IsOlaRemote" when using MultiSync.
//      					Marik  3.When we have Authorize PrePay and vit conflict happens, PumpSrv not clear the vit conflict flags. 	
//							Eyal   4.Added method CQdexFile::CheckPointerValidity()
//							       5.Corrected calls to delete.
//							       6.Added calls to delete to methods in COLATcpComBase.
//							       7.Fix:check for PIN[S] and make sure ot isn't ExtendPIN[S]
//							       8.DB.h: corrected file number definitions
//							       9.Fix: CLogger::RaiseDiskWarning() - prevent call to CServerMain::WriteToLogMsg()
//							         to avoid having two many open file requests, which leads to
//							         no logs being written and occasional crashes at system shutdown
//							         See PrjFuel\Work\Documents\Generic Log Classes\Log_and_AlarmSrv.doc
//							      10.Fix: temp message buffer in CLog::OpenLogFile too small, and may
//							         truncate original message.
//							      11.Fix: BSTR buffers allocated and not released properly
//							      12.Fix: CPosInfo::SetReceiptText() now passes CComBSTR by ref instaed of by val.
//							Marik 13.Add new parameter DebitSingleSession for allowed only one session in the system.   
//								  14.Accept all init debit parameter in one package. 	
//								  15.Add new fields for Table Pump AutoPayOff, SaleStacking , 	
//								  16.Add new field for Table terminal, IPAddress. 
//								  17.Fix: Enable Tank read in specific time with MemoInterval working - WDM 1009.
//								  18.Seperate log writing if not available for all tanks or specific tank - WDM - 1011.
//								  19.Fix:allocate wrong Avi size buffer (GetOlaMemInfoStatus).
//							Eyal  20.Fix - check pointer validity in OLATCPBase::InitInterface
//							         and OLATCPBase::Call
//							      21.New:Limit number of OLA reties
//							             a. Added new parameter - GeneralParam\OLARetries, with default value 10.
//										 b. Added new methods in COLABase to support new param.
//							             c. DoAutomaticOLA exits immediately if param is 0.
//							             d. OLA restarts if registry param is updated.
//4.0.2.16					Marik  1. Release version
//							Eyal   2. Fix: Reset OLARetries if OLA performs a successful action before OLARetries reaches 0
//							       3. Fix: Reset OLARetries if CServerMain::SwitchServer() called successfully
//4.0.2.161					Eyal   1. Fix: COLABAse::ResetRetryAttempts() exits immediately if COLABase::m_byRetryAttempts
//							          is MAX_OLA_RETRIES
//							       2. DeliveryReport - different messages for report failure
//							          if request is for all tanks or if request is for a specific tank.
//							       3. Fix: mistakenly called LoadParam instead of SaveParam 
//							          in COLABase::COLABase
//							       4. NozzleLeftOut checked once every 30 secs. in CmdTx, unless
//							          MISC has been raised.  If misc has been raised, checks occur
//							          once every 10 seconds.
//4.0.2.162					       1. Fix: after 10 retries OLA dies leaving trs in W_PROGRESS
//							       2. Fix: derived TCPOLA classes call AddParam instead of AddParams 
//							          (there's an s at the end of the second one).
//							Marik  3. Save the lLoyaltyTransAmount to  OLA field sLoyaltyTotalAmount.			
//4.0.2.163					Eyal   1. Release for testing.
//4.0.2.2					Eyal   1. Release version
//							
//4.0.2.21					Eyal   1. Fix:  attendant transactions were not written to ExtraInfo.qdx
//							       2. New:  added method GCI::GetOLABuffer().
//							Marik  1. Set shift from GCI interface.
//								   2.Define new parmeter "MinFullingToChargeServiceFee" for minimue charge service fee
//								   3.Added Pump service mode to table ModeScheme.
//									 a.Define new method GCIEvent::SetPumpMode2 fro update the controller 
//								   4.Add new field sPriceRecalcDiscount_Msb[2] to OLa.h to Expand field sPriceRecalcDiscount. 
//								   5.Added new OLA string MediaMasterType[I]	
//								   6.Added new parmeter under table terminals\baudRate.	
//								   7.Allowed Swipe Attendant card & Payment Card in Pre Authorize state 
//							
//4.0.2.22					Marik  1.Release version 
//4.0.2.23					Eyal   1.Added support for PAK finalization even when server in STRATUP.
//							               a. Moved trs state handling to new methods in
//							                  CPumpTransact
//							               b. Transaction finalization done through CmdTx only
//							                  when server is SLAVE, otherwise trs finalized immediately
//							               c. Added CServerMain::ISPAKFinalizationAllowedWhenSystemDown()
//							                  and CServerMain::SetPAKFinalizationAllowedWhenSystemDown()
//							       2.CGPI::SetTrsState() now calls new CPumpTransact methods.
//							       3.Allowed CGPI::SetTransactionState , CGPI::GetGradePrices ,
//							         and CGPI::GetLevelPrice to work when system is in STARTUP
//							         mode.
//							       4.Changed CPump::GetTagMinimal() - if PAK finalization allowed
//							         when system in STARTUP mode, send COMMS_TERMINATED status
//							         to POSPump, so user will know something is wrong.
//								   5.Fix :Pump Service mode not save correct.
//							       6.New: support STOPPED_BY_CONTROLLER pump status
//											a.Pump cannot be stopped or restarted by POS
//							                b.AuthorizeAndLimit (prepay reauthorization) 
//							                  and AuthorizePump/AuthorizePump2 have been blocked.
//							                c.New GPI return code, GPI_PUMP_STOPPED_BY_CONTROLLER (63)
//							marik  7.Added new registry key  Paths\long\TerminalImagesPath.
//								   8.Reorginaize SetShift..	module.
//								   9. Clear Misc for device TERMINAL_CARD_READER , PUMP_MSG_CARD_ENCRYPTION	 every seipe tries.
//								  10.Attendant can sale product only when attendant mode valid. 
//								  11.Add to GCI::SetTrsState option to pay Attendant transaction from different pump. 	
//								  12.Add delayed PrePay. 
//								  13.Remove debug message from setmodescheme3/2 function. 	
//								  14.When DataKey not exist create the new key with default PumpSrv directory or The Path in HomeDir  
//								  15.Pass the  Car wash loyalty ItemLoyalAmt1[F]  to OLA
//								  16.Bug in parsing sOdometerExt,  
//								  17 Added Flags to method GetOLABuffer , ( Authorize , complete). 					
//								  18.when we using the flags in generalParam\ForceCreditLimitCheck, to limit credit transaction to site limit. 
//									 PumpSrv divides the limit site by 10. 
///								  19.In meggitt environment do not send double accepted trs to controller.    	
//								  20.On final transaction clear  MAC inforamtion.
//								  21.in case no limit was set in debit transaction, set customer limit request to transaction limit  
//								  22.Fixed, load table PricePoles by GetRecord
							
//4.0.2.24					Marik  1.Remove the offline parameter from terminal registry + avi.
							
//4.0.2.25					Marik  1.Release version
							
//4.0.2.251					Marik 1.Create default system path CRegTable::LoadPathsTable.
//								  2.Add new event to GeneralSystemEvent, GCI_EVENT_GET_VERSION.
//									Define new method in CTRL::GeneralSystemRequest, to received sepecial client events
//							      3.Attendant AVI conflict that as failed, should remained as Attandant transaction. 
//								  4.Set threads EventThread & AlrmSrvThread to work with low priority
//							
//4.0.2.26					Marik 1.Release version, 
//							
//4.0.2.27					Marik 1 Fix: wrong parameter in method  CTRL::GeneralSystemRequest.
//							      2.Change the envet GCI_EVENT_GET_VERSION number from 4 to 6     
//4.0.2.28					Marik 1 Fix:Create default system path CRegTable::LoadPathsTable.
//							
//4.0.2.29					Marik 1.Fix: GetServiceFee for attendant trs return PAK service fee.
//								  2.Support New PAP flags STRUCT_IS_UPDATE_RECEIPT to force saving information to Receipt.qdx			   
//							      3.Fix: debit EOD process.  
//							Eyal  4.Fix: OLA retry mechanism active only when OLA is remote.
//							      5.Added IGCIEEvents::ReConfig2().  Support for runtime configuration
//							        completed.
//4.0.2.30					Eyal  1. Release for testing.
//4.0.2.31					Eyal  1. Fix:  WSM_BY_INTERVAL performed wet stock checks every 
//							         5 minutes no matter what interval we configured.
//							      3. Added CTank::m_lLowTankLevelLimit and CTank::m_lHighTankLevelLimit.
//							      4. Added support for above fields for POSPump queries.
//							Marik 5.Parse OLA string MediaMasterType[I] in method ParseCardData.
//							Yaron 6.Add to the Log writing the reason for Middle of Process failure.
//							Marik 7.Add support for delayed PrePay for exhibition.
//							
//4.0.2.32					Marik 1.On debit decline add trs data to recipet Qdx. 
//								  2.clear controls char from track2 in method CGCI::GetReceiptRawData3  	
//							
//4.0.2.33					Marik 1.Update wrong Pump service mode from mode scheme schedular.
//								  2.OLA string TransactionDate[D] format, taking the year from 
//							        the Cl2PumpSrv data and not from the local system date. 
//							Eyal  3.Cancel fix #4 from 4.0.2.29.
//							      4.OLA retries reset to MAX_OLA_RETRIES if count is zero
//							        and a card has been swiped (the swipe doesn't have to be
//							        successful).
//4.0.2.34					Eyal  1.Release for testing.
//							
//4.0.2.35					Marik 1.Support init debit from OLA32 event. 
//							      2.debit decline data not save correct in receipt qdx. 
//							Eyal  3.Added sActiveShiftNumber to TAG_EXTRA_FILE_INFO. 	 
//							
//4.0.2.36					Marik 1.Add new method GCI::SetMaintenance
//								  2.Fixed bug in Service Fee in case we working with Pump Service mode scheme
//							
//4.0.2.37					Eyal  1.Added support for Meggitt controller for Reconfigure2
//							Marik 2.Added support to new OLA decline.
//								  3.Added new GPI::GetAlarmByCategory
//							Eyal  4.Fix: reconfiguring OLA server did not cause server to
//							        change.
//							      5.Fix: do not allow OLA reconfiguration in the middle of P@P
//							        transaction.  If attempted, return CTRL_IN_MIDDLE_OF_PROCESS
//								  6.Fix: AllowParallelAVIUsage not read correctly.
//							      7.Fix: Support AVI param reconfiguration from the RFS table and 
//							        the RFS\AVI subtable as well as the GeneralParam\"AVI trs"
//							        subatble
//							Marik 8.Added new flags GeneralParam\m_dwSaveDebitZeroCompleteData to force saving debit information.
//  						Marik/Eran 9.Added logs for down time  	
//							Marik 10.Add new method to GPI:GetAlarmById , CGPI::GetAlarmByCategory.  		
//							Eyal  11. Added failsafes against configuring an OLA server
//								that doesn't exist or isn't registered:
//							a. Added new member COLABase::m_byConnectionAttempts
//							   and COLABase::m_bNoMoreConnectionRetries.
//                          b. Added manipulation of these members to Init functions
//                             of derived OLA classes.
//							12. Removed OLARetries from registry.  OLARetries is now
//							completely internal, and can only be reset from swipes (old
//							FCC button has no effect).
//
//4.0.2.38			 Marik 1.Fixed Items.qdx never been clean, attach cleaning Items.qdx to receipt.qdx maintenance process.  
//					       2.Support new OLA decline messages. (OLASRV_Decline_Transaction2, OLASRV_Decline_NoMoney2) .
//							 3.The suppress Trs timeout works only on one trs instead all trs buffer.  
//							 4.When we working with Max PrePay 0 and tries to Move prepay from Pump1 to Pump2 that is in dispensing, the prepay is disappear 
//							 5.When working with AlarmSrvActive =2 send OLA error through AlarmSrv. 
//							 6.Wrong report to AlarmSrv on DISK error.
//							 7.Fixed,When the disk percent is 99%, no log is write, When the parameter is  change  0, and reconfig operation is dine. the logger never start write again.  
//							 8.Change PumpSrv version to 4.0.2.40. 
//					 Eyal  9.ActiveLoyaltyDiscount now read in CRegTable::LoadActivesTable
//							   instead of CRegTable::LoadInternalPumpSrvParams
//					Marik 10.Added new OLA  string  GradePrice1 (Full Credit price)  , GradePrice2 (Self Credit price)	 
//					      11.Increase the Waiting timer For Pump total from 30 sec to 2 minute 
//							12.Wrong report on error /warnning diskspace 
//					
//					
//4.0.2.40.			 Marik 1.Change PumpSrv verion for PosPump Use. The versio "4.0.3" will indicate to PosPump to Get Alarm from AlarmSrv.
//					 Yaron 2.Add support in ModeScheme when Period/Period + Yearly active and daily active too, 
//4.0.3.0			 Marik 1.Release version
//					
//4.0.3.1			 Marik 1.Fixed: Do validation check on attendant Transaction only if start as PAP
//							 2.Remove logs message (DoAutomaticReadTotal).
//4.0.3.11			
//					 Eyal  1.Reconfiguring pumps did not move system to STARTUP (we must
//							   switch to startup to prompt for pump statuses and in order
//							   to prevent new transactions from starting).
//					 Marik 2.Support new AlarmSrv interface. add new field in  CGAIEventsHandler::UpdateAlarmMap, 
//4.0.3.12			 Yaron 1.Add new general system event:GCI_EVENT_UPDATE_CONTROLLER_SOFTWARE.
//						     2.Change the CTRL:UpdateControllerSoftware() to send the GeneralSystemEvent and not
//							   the UpdateParam() event.
//					 Eyal  3.Changed CCtrl::Reconfigure2() so that reconfiguring
//					         MODES_TABLE returns CTRL_OK instead of CTRL_FAILED_IO
//							   (so that FCC will reconfigure other tables marked)
//						Marik4.Added new flags PAP_FORCE_CLOSE_SESSION to method PAPReadyTransaction,this flags is trigger to force close OLA session  
//							 5.Change the default parmeter OlaWaitingMessTimeOut to be 7 second 
//							 6.fixed:do retries on sending alarm to AlarmSrv in case exception occur.
//					
//4.0.3.13			 Marik 1.Change the attendant (POS) number range from 100 to 900.
//4.0.3.13			 Yaron 1.Fixed:Mode scheme process for Period limit + Auto yearly.
//4.0.3.14			 Eyal  1.Fixed: when POS sends the terminal message flags, the pumpStat
//					         section of the TAG_PUMP_INFO struct returned by GetPumpInfo3
//					         gets bad data
//					
//					 marik 2.Support encryption device error, when AlarmSrvActive==1.
//							 3.Fixed:Problem in  recover mecanizim between PumpSrv and AlarmSrv, 
//							 4.Redesign the AlarmSrv command queue,
//							   This queue handle in/out alarm form,to AlarmSrv. 
//					
//4.0.3.15			 Marik 1.Split AlarmSrv Alarm to two queue, one for received and other to Send.
//					 Yaron 2.Fixed: Mode scheme period limit + yearly to work from the start time to end
//							   end time, and not from time 00:00:00 to 23:59:59.
//4.0.3.16			 Marik 1.Fixed: Reconfigure   AlarmSrv from AlarmSrvActive=2 to 0 , not work correct, we cannot create alarm in the old way.
//					
//4.0.3.17			 Marik.1.Clog, when we close log file we did not check that the file handle is valid. 
//4.0.3.18			 Marik.1.Allocate bstr in SendAlarmToAlarmSrv without free the bstr.
//4.0.3.19			 Marik.1.Fixed:When we reorganize Receipt qdx and delete Receipt record, delete linkage Item record as well
//4.0.3.20			 Marik.1.Use table Primary Language Identifiers in selected langauge.  
//						     2.Add two new OLA decline return code,   
//					
//4.0.3.21			 Marik.1.Move the ServerState enum from ServerMain to Pump.h
//							 2.When working with hydService, convert hydService return code to GCI retcode.
//						     3.When hydService Active do not allowed to Pay PAK when server down.
//					 Eyal  4.Stop pumps locks forecourt if PointOfSale\PumpSrv\LockOnStopAll == 1
//					 Marik 5. Parse OLA string  MediaType[S]  in method, ParseCardData 
//					 Eyal  6. Added CGPI::GetPAKQuery() - retrieves all UNOAID PAK trs
//					       7. Fixed CServerMain::DoAutomaticReadTotals(), so that if a 
//								pump does not send a result, PumpSRv does not enter an 
//								infinite loop.
//						     8. Fixed CGPI::AuthorizePump & CGPI::AuthorizePump2, so that
//								authorize all does not authorize pumps that are not in 
//								view.
//					 Marik 9. Fixed:Get GetCardinfo method in Remote OLA not working correct.We geting  garbage card information. 
//						Eyal 10. Fixed: CPump::DecreaseUnBalancePrePayCounter - when counter
//										is zero, do not decrement
//							 11. Fixed: CGCI::PAKReadyTransaction when prepay trs is in WAIT_AUTH
//								 is processed, instead of being treated as an error
//							 12. Fixed: CGCI::PAKReadyTransaction checks whether POS has already
//								 calculated service fee before adding it
//							 13. Fixed: CGPI::RemovedPrePaidTrs checks whether POS has already
//								 calculated service fee before adding it
//							 14. Send string VolumeLimit[F] to OLA in session completion, 
//							 15. Create new OLA session INQUIRY, to get card detail.  
//							 16. Report new Alarm, Qdx failed, and Pump total failed, when working with AlarmSrv
//					
//4.0.3.22			 Marik.1.Fixed,bug in new session INQUIRY which cause to PumpSrv  not saving the PAP data to OLA mem info..
//4.0.3.23			 Yaron 1.Fixed:GetRecord fail if one of the registry parameters was a binary type.
//							 2.Fixed:Do not check authorization timeout on Attandant transaction.
//4.0.3.24			 Yaron 1.Fixed:If there is no ServiceFee at the pump PumpSrv return garbish values - WDM #6119.
//							 2.Added:Save OLA string CompleteTime[T] & CompleteData[D] to ola qdx, for recovery. 
//					
//4.0.3.25			 Marik 1.Added new OLA string DisplayResultMessage[I],
//							 2.Allowed swipe attendant when PAP not valid.  
//					
//4.0.3.26			 Marik 1.Allowed AVI transaction when PAP not valid
//4.0.3.27			 Yaron 1.Pass the UnbalancePrePayCounter as event to other 
//							   the second PumpSrv via the Multisync, and support the event
//							   when comes.
//							 2.Add log writing to CServerMain::SetPumpModeScheme3(...).
//					 Marik 3.Added new keys to CarWash table
//							 4.Added new ResExt= RES_EXT_SCANNER.
//							 5.Fixed:When attendant not valid in mode , do not allowed to sale items.  	
//							 6.Support Ash2000. (israeli OLA).
//					 Eyal  7.Fix: when locking forecourt due to "stop all pumps", idle pumps
//							   with timed out prepaid trs were not stopped locked and not locked
//					 Marik 8.Clear misc non specific Misc on any swipe.
//					
//4.0.3.28			 Marik 1.Notify PosPump on AutoAuthorize transaction. 			
//					
//4.0.3.29			 Marik 1.Added new field "CashAcceptorMaxAmount" to IPT table. 
//					 Eyal  2.Unbalanced prepay created only if we fuelled for less
//					         than what we paid for.  If we fuel for more, it is not
//							   considered an unbalanced prepay.
//							 3.Fixed: Record in receipt.qdx were deleted not on time. 
//					
//4.0.3.30			 Marik 1.Fixed:Attendant reservation not saved when the fisrt status report as PAK with  res ext attendant. 
//							 2.Fixed:Call from  _tWinMain to method to close all conection instead from CServerMain::~CServerMain();
//							 3.Fixed:Qdx alarm not send to AlarmSrv.
//					
//4.0.3.31			 Marik 1.Fixed:Widest  OLA field chargeto from 4 byte two 9 byte.
//					       2.Fixed:PumpSrv generate Qdx Alarm while VP sample the Qdx and no record exist 
//					
//4.0.3.32			 Marik 1.Add new feature: PrePay limit. This includes:
//							   a. New parameter in GenralParam\PrePayLimit which limit the PrePay transaction.
//							   b. Compares the prepay value to this limit. If bigger return new GPI return code:
//								  GPI_PREPAY_REQUEST_PASSED_FORECOURT_LIMIT = 64.
//4.0.3.33			 Yaron 1.Notify PosPump on AutoAuthorize transaction when card was swiped or AVI was detected.
//4.0.3.34			 Yaron 1.Add to the ConstructAndSaveCardData the sTotalAmt_Msb too.
//4.0.3.35			 Yaron 1.Fixed: SystemDown.log doesn't report "system up"
//4.0.3.36			 Yaron 1.Fixed:Add break to the CMD_DEBIT_PROCESS command. The missing break caused to send the
//							   CMD_GENERAL_SYSTEM_EVENT after the CMD_DEBIT_PROCESS, e.g. if the Pump number was 7 it
//							   sent the "Allied software download event".
//							 2.Fixed:add more checks before moving the prepay - WDM#8498
//							 3.Fixed:Handle in the ParseData:AuthorizationDate both date formates:MMDDYYYY & MMDDYY
//							 4.Merge with India source (4.0.3.321) - Add unit factor for tank fuel level.
//4.0.3.37			 Yaron 1.Extends the CGPI::GetPAKQuery(...) to retreive unbalance prepay as well according to the flags.
//							 2.Fixed: If the method ResetPumpStatQdxData(...) runs it sets the server state to 'START_UP',
//							   and this causes the CmdTx not to start - also in special version for Tesco 4.0.3.191
//							 3.Fixed:If in PAT Trs in the Pre Authorize PumpSrv receives a decline, the Trs stays
//							   'Wait Progress' - WDM # 8666
//					
//4.0.3.38			 Marik 1.Fixed:When member card is swiped before the debit payment the we do not stop on the customer limit select. 
//							 2.Fixed:The OLA string RemainingBalance[F] should be multiply by 100 instead by 1000 
//							 3.Fixed:PumpSrv get into dead lock after OLA recovery process. 
//							 4.Fixed: Inform cl2PumpSrv on change in flags "ActiveLoyaltyDiscount".  
//					
//4.0.3.39			 Marik 1.Fixed:The OLA string RemainingBalance[F] should be multiply by 100 instead by 1000 
//4.0.3.40			 Yaron 1.Add support to PumpSrv new transactions log file (CR 8662)
//					 marik 1.Added new field PumpGroup01 & PumpGroup02 to table "Terminals" 	
//					 Marik 2.Added new Flags (FORCE_SHIFT_OLA_NOT_EMPTY) in setshift, 
//							   This flag allowed  force set shift in spite exists open OLA records.  
//					 Marik 3.Added:ability to get PAP details in middle PAK dispensing. 
//    					       To this purpose added  new OLA flags SWIPE_PAK_TO_PAP  (CR 8118) ,  	
//4.0.3.41			 Marik 1.Fixed:The swap file header in PumpSrv.log not write correct the wMilliseconds.
//						     2.Added new fields "Included" to tax table that describe if tax is Included or Excluded (CR 8959)
//							 3.Added: new field MapTaxLink  to table WashPrograms. (CR 8959).
//							 4.Fixed: Fill the field sTotalBalance with sero not space.
//					
// 4.0.3.42					 1.Fixed:When we moving PrePay We did not moved the Extrainfo records  	
//							 2.Fixed:Move the method DeleteLInk2Ola form class extraInfo to be on ServerMain.  
// 4.0.3.43			 Yaron 1.Fixed:Pump number when moving prepay with loyalty.	
//					
// 4.0.3.44			 Marik 1.Remove Wrong message form logs when we moving PrePay.
//							 2.Added: add new flage in GeneralParam\ForcePreSetToLimitSite to limited the Preset to site limit		
//					       3.Added:Send the resultCode to Cl2PumpSrv while need more param for DeclinePaymentRetry. for debit receipt information. 
//					
// 4.0.3.45			 Marik 1.Added:Generate new misc MISC_PAK_TO_PAP_FAILED, while we failed to convert PAK to PAP transaction. 
//					 Yaron 2.Remove the MISC_PAK_TO_PAP_FAILED when staus return to READY, NULL_RESS.
//							 3.Change the ending log writing, 'Server Ended' will be the last line.
//							 4.Remove all the ReleaseService from the OLA distruction, it is already called in the ServerCleanUp.
//							 5.Move the m_bValid = FALSE in the OLA release to the beggining of the release and not the end
//							   to void a case when after releasing the pointer if the OS gives time to the ola thread it may try
//							   to work with the pointer (bValid only at the end).
//							 6.Fixed: Reconfigure in the logs table damage the swap file mechanism - WDM#9333.
//							 7.Add support to waiting prepay as well in the CGPI::GetPAKQuery(...).
//							 8.Check in CPump::GetNumberOfHotPumpTransactions(...) that hot pumping active - WDM#9380.
//							 9.Clean the need more param misc, WDM#9382.
// 4.0.3.46					 1.Add new Trs misc:MISC_TRS_PAK_TO_PAP_FAILED - this misc is set in Trs PAK->PAP that was decline
//							   and waits as PAK unpaid.
//							 2.Fixed:CGPI::GetPAKQuery, the received only the unpaid that not onoffer - WDM#9398
//							 3.Fixed:Set the Trs reservation to PAY_AT_PUMP and the link number to TRS_LINK_OLA_NUMBER
//							   + sequence number to stop the dispensing timeout and receive the PAP service fee, WDM# 9374,9378
// 4.0.3.47			       1.Send the OLA field RemainingBalance in the session complete. 
//							 2.Expand buffer sAuthorizeSessionID & sCompletedSessionID to be 6 digit instead 4 digit WDM#8929 
//							 3.Fixed:SetParam3 ,the require field name was cleard before calling to registry information. 
//							 4.Added new OLA string RedeemPoints[B], this field indicate flow with point. (WDM 9280)
//							 5.Fixed:OLA session was not clear while  change Transaction Form PAK to PAP and  press cancel in state need more param,   WDM 9382  	
//					
//4.0.3.48			Marik  1.Fixed: Press cancel in middle need more param not clear the Misc MISC_CREDIT_CARD_NOT_AUTHORIZED (WDM 9532)
//4.0.3.49			Marik  1.Update prepay while moving instead of delete and insert (WDM 9151).
//					Yaron  1.Start AlarmSrv,VP and MultiSync connection only if runs and not during registration.
//							 2.Finished the Ash2000 development_.
					
//4.0.3.49			Marik  3.Fixed:The Parameter AutoTimeCheck not been initialize when system loaded. 
//					Yaron	 4.Fixed:Remove the colon from the Trs,PTotals and Tank reading for the Trs log.
//							 5.Add support to pump lock by modes process, WDM#9490
//					 Marik 6.Fixed:when attendant transaction was ended we did not delete the record form extraInfo. (WDM 9596)
//							 7.Added:Force send session complete in cash Paz cancel. (WDM 9487)    
//							 8.Update ongoing version with frozen PAZ version
//							   a.4.0.3.382 -   Added: Add new flag sSlipFlags to ola.h, this flag describe which field not display on slip. (WDM-8981)	
//							   b.4.0.3.385 -   New scheme for ticket numbers (WDM 7943)
//							 9.Added new field to Position table that will accumulate Pump Total volume.
//					
					
//4.0.3.50			Marik 1.Fixed: When PumpSrv as two transactions in wait progress on Pump, he did not located the correct Transaction (WDM9735)
//							2.Fixed: On start  EOS  check all Pumps in the forecourt include pump in status Out of service status  for unpaid transaction	(WDM9750)
//							3.Fixed: in EOS with lock forecourt to maintenance, PumpSrv did not update the forecourt on new reservation  (maintenance) , So, when  we try to unlock  the forecourt the controller reject the operation        (WDM9332)
					
//4.0.3.51			Marik 1.Fixed:Change OLA Slip restriction  to NoOdometerSlip , NoCarRegistrationSlip.(WDM 9869)
//							2.Added:add new key TemperatureFactor to Tank table, this flage is factor to  Temperature value (WDM 9951). 
//							3.Fixed:When the key  ("PSTransactionD,ir") of Transaction log not exist, We create wrong key.	   
//					Eyal	4.Fixed:CCarWash not thread safe. (WDM 9949)
//							5.Fixed:large tables reconfigured in CmdTX instead  (WDM 9833).
//									 of during COM call to CCtrl::Reconfigure.   
//					Marik	6.Fixed:Decline PAP misc was deleting to quickly.(WDM 9924)
//					
//4.0.3.52			Marik 1.Fixed:PumpSrv shift recovery is missing the Pump process (increasing Shift number).
//4.0.3.53			Marik 1.Fixed:Wrong reference to slip restriction
//4.0.3.54			Eyal  1.Fixed:it is possible to create a prepay on pump A, reauthorize it
//								    but not to fuel it, carete a prepay on pump B
//									and leave it in a wait state, move the prepay from B to A,
//					              wait for the authorized trs to time out, and get
//									two waiting prepays.  The fix is to not allow a user
//									to move a prepay to a pump if that pum has a non-dispensing
//									trs (WDM 10044).
//							2.Fixed:The parameter LockPumpByMode did not have proper reconfiguration option (WDM10048)
//							3.Added:Add new OLA string ConfirmPrompt[B]
//4.0.3.55			Eyal	1.Fixed:Once a prepay has moved to IN_PROGRESS, we can
//									create another perpay on the same pump.  If the
//									first prepay times out, we get two prepay transactions
//									waiting on the same pump (WDM 10091).
//					Marik 2.Fixed:Increase OlaWaitingMessTimeOut to be 8 sec, (WDM 10033)
//							3.Added:Report to pos  on status server maintenance. (GetMinimalPumpsInfo) (WDM9835)
//							4.Fixed:After lock pump by mode it is impossible to unlock the forecourt .(WDM#10092)
//					
//4.0.3.56			Marik	1.Fixed: Reorganize header PumpSrvdb.h, the function CAsh2000File where in other include PumpSrvDb..h
//					Eyal  2.Fixed: make sure two prepaid transactions cannot be made on same pump (WDM#10141).
					
//4.0.3.57			Marik 1.Fixed: PumpSrv  sends only 4 digit  for session  Preauthorize , instead 6 digit
//4.0.3.58			Yaron 1.Fixed: Not updating other computer with the field "Paths\Long\PSTransactionDir" when using MultiSync, WDM 9979.
//					Eyal  2.Added: CTrsLog inherits from CQLogger.
//					Marik 3.Added: Support Init debit one by one. (WDM#9850)  	
//					marik 4.Fixed: When PAP is cancel we create Grade00 table (WDM#10216)
//					
//4.0.3.59			Marik 1.Added: On attendant transaction saved the track2 & track1 into TAG_EXTRA_FILE_INFO. (WDM#10258)
//							2 Fixed: Set track1 size to be define TRACK_1_DATA_SIZE.
//							3.fixed: The strct TAG_EXTRA_FILE_INFO was longer the actually qdx size,as result of changing in Ash2000 buffer
//					
//4.0.3.60			Eyal  1.Added support for GCI_EVENT_CLOSE_RECONCILIATION_SHIFT flag (WDM 10175):
//									a.Added registry param under Shifts\Shift## called ReconcileTankSrv.
//									b.Added field in SHIFT_REC: BYTE byReconcileTankSrv.
//									c.Added check for registry param in CServerMain::FindCurrentShift,
//									  result stored in SHIT_REC's new field.
//									d.Added check for SHIFT_REC's new field in CServerMain::ShiftProcess
//							2.Added new alarm if pump dispensed OVER prepay TRS limit (WDM 7466).
					
//					Marik 3.Add new Mode CashAcceptor Valid. 
//					      4.Add new Table BankNote.
//							5.Addded new flage to pump which define the pump as Terminal PumpsXX\\PumpAsTerminal. 
//							6.Added new Flags "NotesLinkMap" to TerminalXX table which define table of notes link to terminal 
//					Eyal  7.Fix: CRegTable::LoadDispensersTable - moved creation of CumulateVolume
//							  a	fter device per grade count (creation of parameter was done
//					        in such a way as to make all device counts zero).
//					Yaron 8.Change the build to be 1004.
//					
//4.0.3.61			Marik	1.Fixed,While parsing OLA string and convert those sting to long type  we lost the accuracy. 
//							2.Fixed:Change default parameter "SaleStacking" to be zero. (WDM#10352) 
//							3.Fixed:Allowed to do GetRecord to all registry table  (WDM#10348)
//							3.Added:Add support to coupon printing at the pump in PAP and Prepay transactions. (WDM#10383)	
//					
//4.0.3.62			Marik 1.add support to Method GetPrePaidTrs2 for coupon rewards.  
//					
//4.0.3.63			Marik 1.Do not process pump status after the server is IDLE, While we working in system with backup.
//					
//4.0.3.64			Marik 1.while we moving to IDLE state in system with backup , PumpSrv will request all pump status, 
//						  	  to be update with the real status of the forecourt.
//4.0.3.65			marik	1.Force accept PAP final transaction with missing parameter. 
//							  When we are working with secondary machine. 	
//					
//					
//4.0.5.0			Marik 1.Increase PumpSrv Card data to be 3 records len instead of 2.(WDM#8857) 
//							2.Update version 4,0,3,64 & 4.0.3.63 & 4.0.3.65 from froze PS version.
//							3.Fixed: PrePay auto authorize was not working
//							4.Added new state to  force paid attendant trs, "STATE_FORCE_PAID" 
//							5.Attached Final PAK transaction that come  with trs= 0 to current progress transaction if exist.
//					Eyal	6.Fix: restart all pumps when forecourt in lock process or already
//							  locked leads to prepay transactions being with GPI retcode 28
//							  (pump locked).  Recreated by locking forecourt for maintenance
//							  and then clicking the Stop All button on POSPump twice.
//					marik	7.Added:deauthorize PrePay when the Nozzle left out report (WDM#10790).  	
//							8.Added:New Key to Grade table MinUnitForCoupon, MaxUnitForCoupon.
//							9.Added:Do not process pump status after the server is IDLE, While we working in system with backup.
//					
//4.0.5.1			Marik	1.Added: Expose to interface CTRL method to retrived current tank details
//						    2.Fixed: After expanded the Ola to three record the QueryReceipt did not work.
//							3.Fixed: support OLA string MediaMasterType[I]. 	]
//					
//4.0.5.11			Marik	1.Fixed: reprint form the terminal 
//					Yaron	2.In IsLocalOnlyParameter(...), checkes if IPAddress exists. If exists
//							  updates the other computer with the commport.
//					Marik	3.Fixed: On closing shift, check exist ola records although all pump are out of service.
//					Yaron	4.Add support to have automatic reconfigure in second machine when working with MultiSync.
//					
//4.0.5.12 09/12/03	Marik	1.Added:new OLA string ClearanceID,	(WDM 10931)
//							2.Added pending queue for init debit on EOD.
//							3.Encrypt/Mask the track2 data in PumpSrv log (WDM 10944)

//4.0.5.13 28/12/03	Marik	1.Fixed:bug in Encrypt account number string
//							2.Increase ola buffer to be 4096
//							3.Fixed:There is not ability to authorize pump after AVI fail.(IGONRE AVI) WDM 10543 
//4.0.5.14 31/12/03	Marik	1.Fixed: PAK transaction that ended before getting  reports rolling update, the PAK trs disappears. (WDM 11176)
//							2.Added new Media type 'T'.  	
//
//4.0.5.15 04/01/04	Marik	1.Fixed:Support three OLA record in attendant payment. 
//							2.Added:New method to CGCI::GetCardinfo3, support three ola records.   	

// * minimume RFS matching should be 2.5.1.80   					

//4.0.5.15 04/01/04	Marik	1.Fixed:Need more param with three OLA record.

//4.0.5.17 04/01/04	Marik	1.Fixed:Attendant reprint not work (WDM 11245) 
//4.0.5.18 04/01/04	Marik	1.Fixed:Not all item data has been saved into item.qdx  (WDM 11268)
//
//4.0.5.19 11/01/04	Marik	1.Fixed:The ZipCode[I] field send to OLA as 4 digit instead 5 digit. 
//4.0.5.20			Yaron	1.Fixed:Pumpstatus was update by remote computer althought local was in
//							  IDLE state, WDM 11390.
//4.0.5.21 11/01/04	Marik	1.Added:Support credit by points, WDM 11370
//4.0.5.22 21/01/04	Yaron	1.Add support to new CTRL event:CTRL_EVENT_MEMO_TANK_READING = 7, WDM 11565.
//4.0.5.23 26/01/04	Marik	1.Added:Give ability to Cl2PumpSrv to storage PAP details in PumpSrv. (WDM 11566)
//							2.Fixed: PumpSrv report on half Pump information in GetPumpInfo, as result of that fcc report regular pump as Terminal number
//4.0.5.24 27/01/04	Yaron	1.Prevent 2 PumpSrv instances by using new CompChecker method.
//							  If already running, the second PumpSrv is stopping.
//					Marik   2.When the forecourt is locked by mode, PumpSrv not report this status to the POS , as result of that the button Stop isn't in his currect state (WDM 11174) 
//							3.In completion send MediaMasterType[I] 
//					Yaron	4.Add the ability to PumpSrv to sync with PumpStat qdex while running,WDM 11779:
//								a. Change the PSSwitchAllow to be CanMoveToIDLE.
//								b. Add more abilities to the PumpStat file class(CPumpFile).
//								c. If not ready GetMinimalPumpsInfo3 returns new GPI return code:
//								   GPI_SYSTEM_DOWN_QDEX_NOT_READY = 92.
//
//                  Udi     5.Added a possibility to lock/unlock a single pump exclusively, (WDM 11598)
//                            including adding the files: SinglePumpLock.h,cpp 
//					Yaron	6.Fixed:GCIEvents return code translation was as GPI.
//
//			
//					Marik   7.Add new qdx file for emulation gank gauge.
//							8.Expose  mew method in GCI & CTRL for update manually Tankreading & delivery reports.
//							9.Support GetCardInfo by session in RFS environment,(WDM)11612
//						   10.Use tank reading without use real Tank gauge in the system, (WDM 10388).
//						   11.Fixed:In need more param the RedeemPoints not saved at all,  
//                  Udi    12.Fixed: Cominations of locking the forecourt by mode and by maintenance
//                            in Allied failed and caused the controller to be out of sync (WDM 10321).  
//					Marik  13 Fixed: When the system is in state shutdown , do not to turn on OLA discconect alarm.  	
//						   14.Fixed:Prepay Trs stuck with IN_PROGRESS status (WDM 11548 )(DOMS environment) 
//					Yaron  15.Fixed:AlarmSrv recovery process didn't send alarms. 	
//					Udi:   16.Added the Delivery Emulation process (WDM 10388). Including:
//                            a.New Methods: A new GCI interface - SetProcess
//											 A new function to CCommonFunction - SetProcess 
//											 A new function to ServerMain which does the automatic procedures regarding the Delivery Emulation - DoAutomaticEmulationDelivery
//											 A new function to ServerMain which conducts the Delivery Emulation State Machine - SetDeliveryFlags	
//							  b.New Properties: To SERVER_INFO: byDeliveryEmulationProcess, byDeliveryEmulationStatus 
//                                              To ServerMain:  m_dwEmulationDeliveryTimeOut - which retrieves the value located in the Registry in: Tanks\\EmulationDeliveryTimeOut
//
//							  c.New Defines: PROCESS_DELIVERY To the Process List
//                                           WSM_BY_START_DELIVERY, WSM_BY_END_DELIVERY - to the WetStock modes
//                                           Delivery Emulation Defines
//4.0.5.25 26/01/04	Marik	1.Fixed: Debug line in method GetPumpInfo3 cause to crash in PumpSrv while reading terminal message. 
//							2.Fixed: In receipt session PumpSrv didn't send the third ola record 
//							3.Fixed: Wrong calculation of PAP  approval amount  (WDM #12206) 
//
//4.0.5.26 18/02/04 Udi     1.Fixed: Pump not locked with the whole forecourt after it has been exclusively locked and unlocked (WDM 12223)
//                          2.Fixed: When locking the forecourt by mode and then releasing it a pump which had been exclusively locked stays locked (WDM 12225)
//                          3.Fixed: When locking a pump, taking the system down and then bringing it up - you're now able to unlock the pump (WDM 12251)
//					Yaron	4.Fixed: CarWash::RemoveRequest(),add a check if list is empty before removing the data. 
//								     Caused PumpSrv to remove from list that was empty and to crush, WDM 11120.
//					Marik   5 Irving loyalty card(WDM 12273)                              
//							  a.Added:Support need more pram of Loyalty card. (OLA string ClubCardTrack2). 
//							  b.Added:Added new fields to extrainfo CustomerName, TotalBalance , those fields contain details on the promotion  given.     
//	                        6.Fixed:The third OLA record hasn't read correct.
//							7.Added:Send OLA string ShortCardName  in session complete 
//							8.Fixed:We removing car wash request without checking that the request is in the queue, (WDM 11120)
//							9.Fixed:When transaction was ended before dispensing data received PumpSrv consider this trs as cancel, 
//						  		    but after he cancle trs we got final data and new trs was created without shiftnumber 
//					Yaron	10.Add support to component connectivity, WDM 10641 
//                  Udi     11.Fixed: Problems in certain scenarios when mixing lock by mode with lock by maintenance (WDM 10321).
//					Yaron	12.Change the 2 instance check by adding second check if another instance exists.
//                  Udi     13.Fixed:Updating the tax tables and the grades' MapTaxLink parameter when upgrading from older versions (WDM 12480)
//					Marik	14.Added:Accumulation shift totals. WDM 12590 
//							15.It is possible to fuel although the wrong Trs status. (WDM 12195) 
//
//4.0.5.27 01/03/04 Marik   1.Fixed:ModeScheme in weekly period  as bug that cause to pumpsrv to select wrong mode  (WDM 12530)
//                  Udi     2.Fixed:Taxes generated from grades with none-valid entries (WDM 12480).
//					Marik	3.Fixed:Avi conflict not accepted when the PAP mode not valid. (WDM 12548).
//					Yaron	4.Fixed:PumpSrv continue with payment process without qdex,WDM 12644.		
//					Marik	5.Added:Price poles new key IPAddress,Simulator.(WDM 12276 )
//                  Udi     6.Fixed:Locking a single pump, taking the system down and up, locking and unlocking the forecourt (WDM 12621). 
//                          7.Fixed:Locking and unlocking exclusively and by mode and then locking the whole forecourt(WDM 12607).
//					Marik   8.Fixed:support extended BAP record
//
//4.0.5.28 07/03/04 Marik   1.Fixed:The BAP extended record flags  not saved correct, as result of that reprint receipt process failed
//                  Udi     1.Added events fired when finishing the Start/End Delivery process, indicating the process' success/failure.
//
//
//4.0.5.29 07/03/04 Marik   1.Fixed:The fourth Item.qdx record haven't read correct. 
//4.0.5.3099 07/03/04 Udi   2.Fixed:The Delivery Emulation Process when TankSrv was terminated (WDM 12761). 
//						    3.Fixed:Disabling the forecourt's Lock/Unlock by Maintenance while the Delivery Emulation Process is going on (WDM 12648). 
//
//4.0.5.3002				1.Fixed:Remove log message from the Tank emulation procedure.
//
//4.0.5.3003				1.Fixed:We did not passed the new parameters  of TAG_DISCOUNT_LOYALTY_INFO2 in the QueryExtraInfo.
//
//4.0.5.3004 15/03/04   Udi   1.Added a Critical Section defending all of the  Server's Info which is a general solution both for the
//                            Delivery Emulation problem (in which the Server Info's Delivery Emulation data was overridden, thus causing the event not to be sent
//                            in the end of the process - WDM 12882), as well as  for other processes in PumpSrv which weren't defended until now.                             
//					  Marik 2.Fixed : When Setmaintenance process started and immediate stoped by ForceUnlock, sometimes some pumps remains lock.  		
//							3.Fixec : Change the concept of Server database not available, 
//									  in the old way we looked for  signature in Qdx  PumpStat  to verify that database is valid,        
//                                      in the new way we checking that we are communicate  with the PumpState database ( this 
//                                      is done by reading record from Pumpstate). 
//  
//4.0.5.31 15/03/04   Marik 1. Release version
//4.0.5.31115/03/04   Marik 1.Added some more cretical section on server info.
//							2.Added Pass OLA string MediaMasterType in session authorize.  
//
//4.0.5.31216/03/04			1.Added factor to Tank reading and delivery reports. 
//4.0.5.33            Udi   1.Fixed:Releasing pumps exclusively when the forecourt is locked (WDM 12828).
//                          2.Fixed:Locking a pump when it's in a Requesting state and then dispensing(WDM 12830).                           
//							3.Fixed:Wrong passing OLA string ClubCardTrack2[S] in "session need more param",  
//							4.Added:In spit PumpSrv is master on transaction, The closing operation of PAK transaction should done by the Controller.
//							5.Fixed:wrong generation accumulate shift total report 12619
//
//4.0.5.34 28/03/04   Marik 1.Added:Save Controller transaction number in extrainfo qdx, so in final transaction he will gets his transaction number and not the PumpSrv number.
//							2.Added:Allowed authorize pump in state ERROR_TERMINATED.
//					  Udi   3.Added:Blocking the option to start a new shift while the forecourt is emergency locked (such as when pressing the PosPump's StopAll button) - 
//                            irrelevant and causes many unnecessary problems.
//					  Marik	4.Fixed:PumpSrv didn't retrieved the current trs number while transaction in state W_Auth.
//                    Udi   5.Fixed:Requesting, Locking Forecourt and Locking Exclusively (WDM 13097).
//					  Marik 6.Fixed:Move Tank gauge database factors to TankSrv.he will handle the multiply operation. 
//					  Marik 7.Fixed:Use incorrect flags to inform on final PAP transaction that was changed from PAK,the new flags is TRS_PAK_CHANGE_PAP. 
//
//4.0.5.35 29/03/04   Udi   1.Fixed:Set shift that was failed as result of emergency locked will return new error code FORECOURT_EMERGENCY_LOCKED, 
//					  Marik 2.Fixed:The key "DefaultLanguage" hasn't created while creating table terminal.  
//
//4.0.5.36 01/04/04   Marik 1.Added:Send to OLA the field BankTermId in session complete  (WDM 13039) (WDM)13191	
//							2.Added:Support fictive "need more param"  to save OLA buffer(OLA string ReSend). 
//							3.Added:New terminal key SignInToTerminal, (WDM 13167) 
//
//4.0.5.37 01/04/07  Marik	1.Added:Create key IPAddress under pump table.
//					 Marik	2.Fixed:Remove the ability to resend session data , When session clock is failed in start session (WDM 13220)
//					 Marik  3.Fixed:Attendant transaction that was canceled was stuck in W_progress (13678)


//4.0.5.38 01/04/05  Marik	1.Fixed:PrePay transaction that was cancel by controller causes to PumpSrv stuck with transaction in_progress. (WDM 13311)  (WDM 13085)
//							2.Added:New key PresetMode under each terminal table. (WDM 13363)
//							3.Fixed:When PumpSrv is register change the log level to zero to decrease logs writing.(WDM 13349)  
//
//4.0.5.39 01/04/05  Marik	1. Support new OLA type OLAIFSF (Product restriction at the pump) (WDM 13370)	
//					 Udi    2. Added the new MessageDisplay feature which receives messages to display through OLA events and passes 
//                             the parsed parameters to the ConvertLayer/RFS	(WDM 13374)				
//                          3. Added the Pump Login feature which identifies when a pump needs to be logged in or logged out and performs
//                             it through OLA - active only when the OLA Server supports the IFSF protocol (WDM 13376)
//					Marik   4. Loyalty card information wasn't clear on return pump to IDLE.  	
//						    5. Fixed:The parser of need more param, added the mark '?' to OLA string despite these strings were only to be delivery to controller
//							6. Add new property to connectivity struct TankSrv -> TankGauge 
//							7. Fixed:Interval wetstock was multiply by 1000, this will be done by FCC (WDM 13576) 
//							8. Fixed:Attendant transaction was stuck in accepted state as result of receipt full. (WDM 13489)	
//							9. Added:Give ability to client to controls on lock/unlock shift.  
//							10.Fixed:The flags that indicate working with three record wasn't saved, When using GCI::GetCardinfo2
//							11.Added: Open  'Need more Param' module (WDM 13372)  
//							12.Add new flags to InitInterface that support the method SetPumpMode2	 				
//							13.Fixed:There isn't any limitation on open PAK transaction while working with PAK auto authorize.   
//							14.Added:New event to controller that inform him that pump data was reset.   			
//				    Udi     15.Added: The Offer Prepay for single POS feature- which locks a Prepay transaction once it has been recalled
//                             by a certain POS  so that a different POS won't be able to recall or cancel it.
//					Marik   16.Added:Add support to aditional open string for the receipt (WDM 13377) (WDM 171)	
//
//4.0.5.40 16/05/04 Marik	1.Fixed:Move the VP process of disconnect from VP to cleanup() function. 
//					Marik   2.Fixed:support OLA event in OLAIFSF
//		   17/05/04	Udi		3.PumpSrv to send startup data to DataExportSrv for the IFSF protocol.
//
//4.0.5.41          Udi     1.Fixed the following bug: if the system was up with "garbage" data in the Delivery Emulation parameters
//                           (from the Qdex) it wouldn't be possible to start a Delivery Process.
//					Marik   2:Added:enlarge the OLA driver id to 8 characters 
//							3.Fixed:The open need more params does not reach to OLA. (WDM 233)
//							4.Fixed:Allowed to reprint receipt that contain only Items. 					
//							5 Added:Support need more param to OLA string RemoteId
//					Udi     6.Added the Ignore Avi in Authorize Pump Feature - which enables POS clients to ignore an Avi device in advance
//                            and create a PAK transaction instead.(WDM 225 227)
//							7.Fixed:No zone at the reprint receipt.WDM 253. 
//4.0.5.42					
//					Marik	1.Fixed:PAK transaction that was in state Authorize couldn't convert to PAP transaction, 
//							2.Added:support Delek Malan remote control (WDM 261)
//					Yaron	3.Fixed:FILEVERSION was not sync with the FileVersion in the PumpSrv.rc. Caused the DataExportSrv
//							  in the EPICS to write version 4.0.5.40 instead of 4.0.5.41, WDM 335.
//					Marik	4.Added: Enlarge the buffer of GetRecord\SetRecord so Fcc could read in one shoot the entry of languages 
//							5.Added new OLA string "sLogicShiftNumber" for request need more param to logic shift number 
//					Udi     6.Added support for the Mode Related Prompt feature - which presents an IDLE prompt at the pump according to 
//                            the mode currently active on the pump. WDM #150
//					Marik   7.Move initInterface2 definition to Pump.h 	
//					Udi     8.Fixed:PumpSrv crash when changing prices and IFSFOla is active. WDM #377 
//					Marik	9.Added:Fuel new loyalty device with AVI (WDM 245)
//					       10.Added:Query to attendant transaction through GCI
//					Yaron  11.Fixed:PumpSrv is not coming down in Allied ENV(WDM 290).		
//					Marik  12.Fixed: AVI conflict that was decline by OLA added to OLA.qdx by mistake (WDM 387). 
//
//4.0.5.43 10/06/04 Marik	1.Added: New option to convert OLA transaction to PAK using PAPReadyTransaction	
//					Udi     2.Added: The new Languages Ctrl Interface  which is used by the BOS for conducting 
//                            the Languages issues through PumpSrv instead of through the CL/RFS - the new method.							 
//					Marik   3.AVI transaction got stuck after ATT AVI conflict ( WDM 491) 
//
//4.0.5.44 10/06/04 Marik	1.Remove the exit(0) from method CTRL::DBTankGauge
//                  Udi     2.Added the Printer Offline feature which updates the DOMS forecourt controller with the 
//                            status of the POS printers in the site in order to enable/disable buffering of transactions.   (WDM 508)
//					Marik	3.Support LAM module (WDM 508).
//							4.Added new flags to post void that indicate the LAM status ( valid not valid) 
//					Udi     5.Fixed the following bug: when the system went up, PumpSrv wouldn't notify the CL/RFS of a mode change
//							  using the SetPumpMode2 GCI Event.
//
//4.0.5.45 17/06/04 Marik	1.Add new function to report on the system version (GPI::GetVersion)
//					Udi     2.Added logs to the CGPI::AuthorizeAndLimit__ function.
//							3.Fixed bug #719 - unneccessary logs related with the PrinterOffline feature. 
//
//4.0.5.46 23/06/04 Marik	1.Fixed:When working with mode that transaction must closed by controller, 
//								    And we don't have connection with the controller we should reject any try to paid the PAK trs.(WDM 862)
//					Udi		2.Fixed the following bug: in the FCC's Processes Screen when one process' Progress Bar advanced other Progress Bars
//                            would advance too - only the relevant one should advance. 
//                          3.Added:When printers are connected one after another the GCI_EVENT_PRINTER_ONLINE event will be sent only once.
//							4.Fixed: Use regular BSTR in method CCtrl::DBTankGauge , instead BSTR that can pass binary data.
//4.0.5.47			Yaron	1.Change the enum TankGaugeData to enum TankGaugeDataFlags due to conflict with Doms struct.
//					Udi		2.Added more details to some logs in CGCI::SetPumpStatus and CGPI::AuthorizeAndLimit__, so that if a problem
//                            occurs during a transaction we'll have more details about it. 
//							3.Changed the mode related prompt name in the registry (under "modexxx") from MessageNum to IdlePromptNum.
//4.0.5.48					1.Fixed bug #1092 - sending incorrect service modes in the CMD_SET_PUMP_MODE2 event sent down to the CL/RFS.
//							2.Fixed the following bug: when changing the Mode Determination from Manual to Automatic, the Misc would appear
//							  as Manual the next time the System went up. 
//							3.Enable to overwriting Trs data if Trs was dispense over the limit and the rest is used as waste. 	
//
//4.0.5.49					1.Fixed:AVI conflict that charge on the AVI device did not insert to OLA.
//							2.Fixed:Clear misc AVI conflict in status read, NULL_TRS  
//
//4.0.5.50         Marik	1.In POS void transaction set LAM information to space.
//4.0.6.0	                2.Release version 
//4.0.6.501		   Yaron	1.Fixed:Pos void transaction with corrupted LAM not handle proper,(WDM 1184),merge from 4.0.6.10
//							2.Fixed:Added validation check on pump number in method RemovePrePaidTrs (WDM 1220),merge from 4.0.6.10
//							3.Fixed: When reset pump data delete any record that link to extrainfo, merge from 4.0.6.11
//							4.Add support for different DataExport components, WDM 1622.
//					Marik	5.Fixed:Cancel prepay clear wrong trs.  
//							6.Fixed:Request GetCurrentTransactionNumber while attendant trs open cause to open new trs instead use the current one.
//							7.Fixed:In session need more param clear the previous field data.
//					Yaron	8.Fixed:Pump lock process retry mechnism didn't work. If caused the process to enter
//							  into endless loop if pump was not locked, WDM 1434.
//					Yaron	9.Add to the ModeSchemeProcess the ability to send changes to the CL/RFS
//							  in cases that the active mode wasn't change, only the ServiceMode,WDM 1091.
//						   10.Change the DataExport behavior in PumpSrv.
//	Patch 30-40		Marik  11.Fixed:Request GetCurrentTransactionNumber while attendant trs open cause to open new trs instead retrieved the current one.
//						   12.Added:Added new flags to PAPReadyTransaction STRUCT_IS_PAP_CANCEL, This flage will be trigger to return new retrurn code on PAPSwipeResult CD_CANCEL_OK. 
//						   13.Added sCouponMsg field to buffer that create in method GCI::GetOLABuffer (WDM 1944)
//						   14.Added Added Add more logs to track real Qdx error in low level class.
//					Yaron  15.Pass transaction authorize amount Method GetOLAMemInfo2.	 
//						   16.Add new method to CTRL::GetGeneralData, to be used by DataExportSrv for polling the Startup data.
//						   17.Fixed:Volume limit was not multiply by 1000, WDM 2249.
//
//4.0.6.502 11/08/04Marik	1.Added to DataExport thread the attribute that support COM. 
//							2 Added:Added new flags to delivery report that force accept the incomming delivery data.  
//	  Patch 70				3.Fixed:PAK to PAP transaction that was in middle of need more param and customer press cancel,we did not close the pap session correct. (WDM 2079)
//							4.Added:Support discount by Value/percentage in PrePay and PAP.( WDM 1905).
//							5.Added:Support Offline Fleet card parameters (WDM 2069).
//4.0.6.503	Patch 70 Yaron	1.Added:Parse field sClearanceID from row card sale data field to OLA buffer in method GCI::GetOLABuffer,WDM 2500.
//							2.Add the creation of "Simulator" key under each pump.
//                   Udi    3.Added more detailed information regarding errors occuring when trying to perform Delivery Emulation illegally (TD - # 2617).
//			Patch 80 Yaron	4.Improve the memory allocation handling in the GetPAPTrs3(...) and remove the exit(1) from the interface.
//							  It caused PumpSrv to exit although  it succeeded at the second trial,WDM 2832.
//4.0.6.504          Udi    1.Added logs to CServerMain::SetShift giving us more information in case the End Of Shift process fails.
//							2.In the COLABase::BuildRequest function - added the AuthorizationNumber parameter to the parameters which are
//                            always sent, regardless of the session type.
//
//4.0.6.505          Marik  1.Fixed:The flags "UseServiceFee" was set in wrong logic table, it was in table "LoadForecourtLimitSettings" instead table  " ActivesTable". (WDM 3024)
//							2.Fixed:ScripUnitPrice being rounded by PumpSrv WDM 3083	
//							3.Fixed:Pump stuck "IN Progress" in case Hand handle is offline and we try to connected with different Hand handle. (WDM 2944) 
//
//4.0.6.506          Marik  1.Added:Remove all card info details from log.   (WDM 2141)
//							2.Connectivity:Block the CL to FC updates from CL/RFS. Related on PumpSrv only.
//							3.Added:ability to reprint last receipt without track2 from terminal. (WDM 2207)
//							4.Added:Exposed method GetStationStatus in interface GCI (WDM 3308)
//	Update Patch 10			5.Fixed:Allow to Unlock the site while Mode change 
//							6.Fixed:Stop creating table "Messages".
//							7.Added:New registry key MinReceiptLines under "Receipt" table
//	Update Patch 11			8.Added:Attendant recovery process needs extra fields (OLA old session, short card name,Clearance Id , Madia Master Type)  to be able to send final transaction through PMT.( WDM 2605)
//							9.Added:Support "Yoman Rechev" device  (WDM 341)
//	
//4.0.6.507          Marik  1.Fixed:bug in UploadBuffer in server main. 
//					 Marik  2.Added:Save Expire date into ExtraInfo Qdx. WDM 1058).
//					 Yaron	1.Fixed:Card info removale from the logs was removed not 
//							  releated to the registry configuration,WDM 3506.
//							2.Support site limits that connected to mode.
//							3.Fixed:LogSystemDownData(...) condition syntex.
//							4.Fixed:SetModeDef2(...) trims only right spaces. Add left spaces trim too.	
//							5.Fixed:Report that were failed in generation did not clear from queue dur to that the queue got full. (WDM 3403)
//							6.Fixed:DataExport handling when it is not register. It caused PumpSrv not 
//							  stop when shuting system down, WDM 3787.
//							7.Fixed:OLA AuthorizationNumber[S] is missing in complete session  (WDM 3745) 
//                   Udi    8.Fixed: A crush which has occured due to a lack of protection on the validity of the
//							  pump number in the CServerMain::GetLimit function. I also added this protection in
//                            the CGCI::PAPSwipe__ function. 
//					Yaron	9.Add support to "pump out of service" alarm and "No controller" alarm.
//
//4.0.6.508         Marik   1.Fixed:When PAP final trs is failed, used the incoming  Cards sale data as correct data instead using of preauth Card sale data. (WDM 3811)
//							2.Decrease timer check process automatically wetstock from 5 min to 20 sec. 
//							3.Modescheme that has service mode  'Not Used' the pump behave like it is in Self Service (WDM 2475)
//4.0.6.509			Yaron	1.Fixed:DataExport handling when it is not register. It caused PumpSrv not 
//							  stop when shuting system down, WDM 3787.
//							2.Fixed: Replace the ',' with '.' in the PumpSrv version in the EPICS report, TD 3910.
//4.0.7.0			Yaron	1.1007 formal release.
//4.0.7.500			Yaron	1.Change the default creation of the parameters:XML_PriceChangeFile and TerminalImagesPath.
//							2.Put the DBTankGauge logs under log level 4 instaed of 2.
//		Update from 1006	3.Decrease timer check process automatically wetstock to 10 sec.
//							4.Fixed:Wet stock by date is not working at midnight.
//							5.Fixed Periodic Mode Scheme acting as Daily Mode Scheme, TD 4237.
//							6.Add new flag in the PAPReadyTransaction(...):PAP_WITH_CARWASH.
//							  If set PumpSrv should add the carwash value to the fuel value.
//							7.Support grade restriction in IFSF Flow, TD 2074. 
//							8.Fixed: decline PAP details aren't saved correct in the memory, TD 3811.
//		Update from 1007
//							9.Added:Add new OLA string "RequiredOlaData[B]" ,This fields tells VP if he needs to pull OLA data.
//						   10.Added new flags sUnitPriceIncludeDiscount to card sale data , If this flags set it means price change on discount.
//				Yaron	   11.Fixed: PumpSrv loads and saved PumpInfo after receiving the event from MultiSync.
//							  It caused infinite data updates from one computer to the other and vice versa.
//						   12.Fixed:When no Service Mode is defines in the ModeScheme, takes the pump default
//							  mode of service instead of Full Service as default, TD 4747
//						   13.Expend the data send to DataExportSrv for BP EPICS to include site number too, TD 4768.
//				Yaron	   14.Expose the rolling update data in the GetMinimalPumpInfo3 to enable PosPump display it
//							  on the pump button. It is display according to minimal value set in PumpSrv registry, TD 3552.
//Update from 1006Marik	   15.Fixed:Added condition that chack that list of "request car wash" is full , before delete any elament form list.

//						   16.Added: Search receipt by car number WDM (4461).
//
//4.0.7.501         Marik   1.Move  flags ActiveReceiptExtraIndex from  table GeneralParam to receipt.
//					Yaron	2.Fixed:Alarm prepay over limit was not send with correct data, TD 5262.
//							3.Fixed:Alarm dynamic description was hardcoded "Test".
//							4.Add support to new alarm:'No Convert Layer", TD 5262.
//							5.Added:new OLA fields for threshold values.
//							6.Add ATG delivery information to Tank memo data, and exposed two methods (GetTankMemoryData/GetWetStockData) in interface GCI, 
//							  in order to give convert layer to generate tank gauge reports.  
//4.0.7.502			Yaron	1.Add support to DataExport reconfigure, TD 5470.
// Update from 1006	Marik	2..Fixed:Added condition to Check that the list of request car wash command is full before any try to remove element from list.
//	Up to 170				3.Wrong removing  element from car wash request list cause to exception.
//							4.Allowed only one session  ( generate/cancel) car wash) to Car wash device.
//							5.Fixed: The sZipCode data not saved in stage of need more param. 
//Update from 1007	Marik   6.Do not allowed to POS recall PrePay that is progress.WDM 5129
//	Up to 54				7.When working with prepay with offer mechanism, do not allowed to reauthorize prepay unless the trs is not offered 
// 						    8.When locking single Pump, loading the data also when using MultiSync, TD 5253.
//							9.Fixed:PumpSrv did not return the trs status CD_CANCEL_OK. ( i missed one line code from patch 4.0.6.30)
//							10.Fixed:Final PAP ( with flags cancel)  that was decline in the preauth return wrong flags
//
//4.0.7.503         Marik   1.Added new OLA string ClockRequired[B] (WDM 5310). 
//							2.Fixed:Recent delivery process did not send events at end process. 
//							3.Fixed:The registry field GeneralParam\RemoveCardDataFromLog has extra space in name. (WDM 5717)  	
//							4.Fixed:The Final PAP transaction not saved due to that on crash the data is lost. (WDM 5545)
//4.0.7.504         Marik   1.Fixed:Max reprint copies not checked in the flow of reprint receipt by car registration(WDM 5922)
//							2.Fixed:When POS application is reconnected ,Create all POS internal parameters as it connect in the first connection.

//Update from 1007	Marik	3.Added extra logs around eod process to following after (WDM 5762).
//Up to 71					4.Fixed :Bug in eod recovery process.( WDM 6051 )
//							5.Fixed :Bug in eod recovery process,Windows recovery return to registry old shift date, that cause to pumpSrv start new shift (WDM 6051)
//4.0.7.505			Yaron	1.Set the PAK Auto Reconcile to PAID.
//							2.Send the 'New OLA record' event to CTRL events (VP).
//							3.Add a description to the return codes string table.
//Update from 1007/ 72		4.PumpSrv to ignore pump totals if status is COMMS_TERMINATED.
//					Marik   5.Fixed:Allowed to get delivery report on fly in method GCI::DeliveryReport2 (WDM 6297) 
//							6.Added:Update receipt counter by car registration (WDM  5922)	
//
//4.0.7.506			Marik	1.AVI trs stuck in state W_PROGRESS while we canceled the trs in middle of authorize session while ola wasn't responses.(WDM 6336)
//							2.Added PAK auto reconcile trs to  accumulation shift .
//							3.Hot Pump decline due to timeout doesn't stop the Pump. (WDM 6373)
//Update from 1007/ 81		4.Shift process didn't start due bug in critical section that overwrite server info with wrong details  (WDM 6286,6336)		
//							5.Marik	1.Qdx recovery process after reconnect to Qdx, saved garbage data. (WDM 6493 )
//Update from 1007/100		6.Marik	1.Do not save Attendant zero trs to OLA qdx (WDM 6168).
//
//4.0.7.507			Marik	1.Parse NACS code that come from OLA in session preauth in OLA COLAPositiveEventSrv. and convert them to Grade code
//							2.Delivery Report didn't  saved at qdx.
//							3.Let convert layer to control PumpSrv log Level  (WDM 3998)
//4.0.7.508			Marik	1.Double PAP cancel in same trs cause to stuck trs (WDM 6671)
//						    2.The Misc PAP card not authorized isn't  display when mode isn't valid. (WDM 6884)
//
//4.0.8.0			Marik	1.formal release
//
//4.0.8.500			Marik 
//Update from 1008/10		1.The amount in OLA record is multiply by 10 in  PAK auto reconcile flow. 
//					Yaron	2.Expent the component conectivity support, TD 5514.
//							3.Fixed:in the method CPosInfo::IsPOSConnected() GetTotalSeconds instead of GetSeconds.
//							4.PumpSrv abort pump totals and tank reading if received "not available" for 1 device. (WDM 6840). 
//							5.Clear stuck transaction during shift process (WDM 6131)
//					Yaron	6.Add support new key under pumps table: UsePumpKeypadPreset, TD 8058.
// update  form  1008/30	7.The regular OLASrv type doesn't supports discount per grade (WDM 8297).
//							8.Reorganize server info (WDM 8263)
//
//4.0.8.501			Marik   1.Improved price change process to be immediately(WDM 7404) 

//4.0.8.502			Marik   1.Add new ola string "ReceiptMode[S]" ,this flags will notify to convery about print mode /="Y" / "N" / "W" , (WDM 8037)
//							2.Cancel PAP transaction while need more param take place , PumpSrv does not send PAPSwipeResult.8901
//4.0.8.503			Marik   1.Move Runtime parameters from the registry to Ini file WDM 5848
//Update from 1006-220		2.the amount displayed on the PosPump reprint screen was multiplied by 10 WDM 9299
//							3.Added ability to PumpSrv to works with manual mode service fee, without been depending on the Service fee in Pump level WDM 7871  								
// 	Update from 1008/11		4.Add date and time to OLA record , in PAK auto reconcile flow. 
//							5 PAK transaction can't be paid due to Server status Start up. (WDM 7125). // Update 1007 (120,130).
//							6.PrePay with AVI conflict isn't change to W_TIMEOUT , instead of that the Pump reserved automatically to requesting PrePay.(WDM 6415)  
//							7.AVI conflict details aren't cleared from PumpSrv memory, incase pump status after AVI conflict is change to requesting (WDM 7046)
//							8.The regular OLASrv type doesn't supports discount per grade (WDM 8297).
//							9.PumpSrv sends decline on AVI as loyalty transaction when the current mode not support AVI  transaction WDM 8659.
//							10.Set OLA query session to be accurate  and run evry X time.
//
//4.0.8.504			Marik   1.The entry GradeXX\ValidEntry was moved by mistake to RunTime INI  as result of that the price wasn't download to RFS. (WDM 9429). 
//							2.String parameters were not read correct from  ini File. (WDM 5848)

//4.0.8.505			Marik	1.Add new parameter pumpXX\SatellitePump  for each pump	 (WDM 7974)
//							2 Fixed: The Method GetStationStatus doesn't report correct on  pump status, due to fact, that the resext data has not check. (WDM 8330)
//							3.Fixed: Save the nozzle number in Post Void transaction, TD 8650.
//							4.Fixed: Due to problem with critical section the transaction PAK not closed.
//
//4.0.8.506			Marik	1.Fixed:PumpSrv dosn't authorize PAP with grade restriction. due to bug in runtime parameters feature (WDM 9497)
//4.0.8.507			Marik	1.The defualt regsitry value of DataDir was set to C:\\ instead to PumpSrv Path. (WDM 9672)
//Update from 1006-220		2.Clear track2 from Controll character (GetReceiptRawData2)
//Update from 1008-110		3.Use delivery report end time instead start delivery in Report delivery.	
//							4.Nozzle down after swiping credit card get terminal stuck in wait for authorization. (WDM 9865)
//							5.Add new OLA string CustomerID[I] (WDM 8528)	
//							6.Add new OLA string PreviousMediaMasterType[I]

//4.0.8.508			Marik	1.Improved reprint by car registration flow so in update existing receipt won't be raised alarms. 
//							2.bug in initialization of manual service fee parameter, due to that the manual service fee not chanrge (WDM 10787) 
//							3.Clean the RFS connectivity bits on loosing RFS connection. (WDM 9671)	
//4.0.8.509		     Marik  1.Clear track2 from Controll character  in update receipt counter (WDM 10801) 
//
//4.0.8.510		     Marik  1.Fix:PumpSrv open multiply transaction as result of overwriting falgs SWIPE_PAYMENT in the second time of swiping payment card(WDM 10877).
//
//4.0.8.511		     Marik  1.Fixed:: PAPSwipeResult command not send to controller when Cancel PAP transaction is done while OLA offline (WDM 11003)
//4.0.9.0		     Marik  1.Release version Build 1009

//4.0.9.500		     Marik  1.Add new field sManagerID to ola.h in order to save the manger ID number for refund transaction  the field will be fill by RFS (WDM 8123) 
//							2.Added new fields sGRDiscountType , sGRDiscountPerGallon to extrainfo file for recovery process.
//							3.Parse ola string BarCode[S] (WDM 11049)
//	Update 1006-270			4.
//								a.Add new condition to  Detecting "Cancel" of PAP transaction when working with OLA_BY_CONTROLLER.  
//								b.Fixed:PAP transaction that change to PAK doesn't display on PosPump.(WDM 10806)	
//								c.Fixed:Receipt copies exceeded the max receipts number ,in case receipt was generate by POSPump  (WDM 10528)
// Update 1008-200			5.When authorize all pump at once the limit flags was not send to Convert layer WDM 11392 	
//							6.Add new OLA string DoSessionAuth , this flags indicates if required session Authorize. (WDM 9043)

//4.0.9.501			Amram   1.	Add Option that delete car wash the exist only in the device  and not in QDX (TD 8313)
//								a.  Add new method CancelWashCode2 that return seq number and get flog that said if 
//									And get flag that said if we on the new cancel 
//								b.  change CancelWashCode__ that will return seq number if we on the new cacnel
//					Marik	2.Save attendant information in PumpSrv using method GCI::SetPAPSwipeData. (TD 9043)
//							3.Add -  Exposed method GetAttendant to retrieved attendant detalis to convert layer (TD 9737)
//							4.Remove stacking ability on PrePaid trs TD 7919
//							5.Only Attendant that open the trs can close his transaction TD 11107.
//							6.Enable / Disable the carwash button when the device is offline TD 9224	
//
//4.0.9.502 24/05/05Marik   1.Can't reprint PAP Transaction include carwash without fuel TD 12519
//
//4.0.9.503 24/05/05Marik   1.- Do not print ATG delivery report at terminals before ATG delivery informaion received TD 10037
//							2 - Support reports Ullage & Delivery in POS TD 9889.
//							3 - PCI implementation  TD 12258.
//							4 - Add new  field	sPMTReferenceNumber to OLA struct TD 9890.

//4.0.9.504 05/06/05Marik   1.Change  the default value sends  to POSpump on Car wash status   1- on line 0- off line.    
//							2.Discount per grade values doesn't sent to convertlayers through GCI::Querydata TD 10914
//4.0.9.505 Update 1006 -220
//						    1.Parse Voucher number that come from OLA.   
//
//							2.Add new condition which not let move prepay to pump that has PAK unpaid    TD - 12943
//							3.Add new condition which not allow create new PrePay while pump is fueling. TD - 12942	

//							4.Fix: When Controller not avalible the GetStationStatus status retun garbage information in parameter  *lPumpMap2.
//
//4.0.9.506					1.Add new fields sManagerID , sPMTReferenceNumber to Data retrieved in GCI::Querydata 
//							2.Reset input parameter's *lPumpMap2 in method CGCI::GetStationStatus
//							3. Add check if mode can change according to flags from  registry
//							   if the pump idle and no prp on pump
//							4. add treatments in tank level protection - posPump will show only valid grade 
//								and rfs will update the map TD  13161
//							5.Add enrypt dll com for pci
//							6.Add method to gpi GetReceiptRawData and return convert the reslut to xml 
//								for pos
//							7.Add the cardSaleData XML Class
//							8.Add CPumpTransact::GetRes,CheckIfAllUnpaidArePAK()	to get the res of trans
//							  use for checking if there are unpiad trans on the pump
//							  that PAY_AT_KIOSK.	TD		12202
//							
//	
//4.0.9.507		marik		1.Add new ResExt - RES_EXT_FLEET. TD  11907
//							2.Create two PrePay Transaction base on One authorzation 11923
//							3.Report on Valid Grades on single Pump  GetPumpGradeInfo TD - 11922 
//							4.Post auth price must be same as reported in the authorize level. TD-12126
//							5.Update Transaction details from POS while Pump dispesning . TD- 11909
//							6.Send receipt row data to POS for reprinting TD - 11913
//							7.Create two PAP transaction base on single authorization  TD - 11906 & TD 11918	 	
//							8.Support open need more param for muliple prompts	TD 11908
//							9.Show All PAK & PrePay on One screen	TD - 13500  
//	
//4.0.9.508		marik		1.Add new flags in card sale data for cl2PumpSrv recovery  TD 14976.
//4.0.9.508		Amram		2.Add new flag to support FuelLoyalty
//4.0.9.508		Amram		3.Add New Struct to general struct for tank level TD 12202
//4.0.9.508		Amram	    4.Add new method to gci SetGradesRestriction to set the map of the valid tanks 
//4.0.9.508		Amram		5.Create new class EncryptionComDll the hold the dll that encrypt the account number
//4.0.9.508		Amram	    6.Add new method to CommonFunction GetTransListQuery that return the trans according to flags
//4.0.9.508		Amram		7.Insert all treatment in RTAPS ( state machine,method to start end ticket).
//4.0.9.508		Amram	    8.Create new struct to hold all the treatment in the rtaps DLL
//4.0.9.508		Amram		9.Insert new class(CRTIEventHandler) that hold all the event from the fuelloyalty
//4.0.9.508		Marik		10.AVI transaction got stuck in case AVI transaction was cancel by cl2PumpSrv while PumpSrv is Pending sending  AVI authorization. TD 14530
//4.0.9.509		Marik		1.Fixed - Service mode + price level controlled manually TD 12200
//							2.Add Cash Acceptor support TD 12197 	
//4.0.9.509     Amram       1.Fixed QueryData -  the method we s crashed in the write to log TD 14717

//4.0.9.510		Amram		1.Add new flag PriceChangeLockEx TD 12233
//4.0.9.510		Amram		2.Add check for price change if PriceChangeLockEx is on
//							  Check if we don't have unpaid trans
//							  Make the PriceChangeLock stronger (lock pump when it is idle and without any unpaid transactions).
//4.0.9.511		Amram		1.Change TankLevel flag  that if the flag off then all the 
//							  Grades are valid. (TD 14904).
//				Marik		2.Cancel PAP transaction in middle of Authorize while session is timeout , PAPSwiperesult command not sent to cl2PumpSrv TD. 14532 , 14977.  
//
//4.0.9.512		Marik		1.The price change flags "PriceChangeDone" wasn't update in the ini file as result of that every system loading makes price change process TD -14086.
//							2.Wrong PAP cancel in middle of session authorize , in multisync enviroment. TD 15004.
//							3.Accept  authorizePump2 with  IGNORE avi even if Pump is STOPPED_BY_CONTROLLER. TD 14953
//							4.Parse OLA string ServiceFee TD- 14091	
//				Amram		5.Add treatment in stuck trs that accrued because
//							  cancel had been pressed before start ola session TD 14881

//4.0.9.513		Marik		1.Support new ola prompt OperationalPrompt[B] TD-12127.
//4.0.10.0		Marik		1.Formal release
//4.0.10.10     Amram		1.Add new copy to QureyData sLoyalId,sClFlags TD 15453
//				Marik		2.Add  Service Fee to Extrainfo data TD 15485			
//							3.Price change process prform whenever System was launched TD 15491
//							4.Marge Pilots code .
//							5. Fix SetGradePrice2 in SaveParam Td 15358
//					
//4.0.10.500	Marik       1.Marge Pilot fixed.
//				Amram		2.Fix SetGradePrice2 in SaveParam Td 15358
//              Amram       3.Add new copy to QureyData sLoyalId,sClFlags TD 15453
//				Amram       4.Change the flow of Shift change CR 14081 Add new registry Flag EOSMaintenanceLockForecourt
//				Amram       5.Return  GCI_PAK_RECONCILE_TRS in PAKReadyTransaction  if the timer = 0 
//				Amram		6.  4.0.10.14  Change GPI::SetModeDef to check if we can change the mode to new mode
//							  According the the new flags from agip  if the pump idle and no prp on pump
//							  TD 13161.
//4.0.10.503				1.Add treatment restriction/position to olabase treatment TD
//4.0.10.504				1.Add function that check if we work on position
//							  change the ParseData to check it( sGradeDiscount )
//							2.Add check in SetCashAcceptor if the mode supported.
//							3.Add new OLA flags 	
//
	//Marge 1010

	
	//					

	//4.0.10.11    Marik		1.Controller Seqnumber was not saved in PrePay Transaction into extarinfo.therefore PrePay transaction stuck in  accepted state. TD 15756 , 15306.

	//							2.Add Prompt type to COLATravelCenter class.

	//4.0.10.12    marik		1 - Add extar log lines

	//4.0.10.13	   marik		1- XML file corrupted in Transaction which convert to PAK

	//4.0.10.15	   marik		1.Cash acceptor transaction was authorize to site limit instead to limit insert in the device (TD 15957)

	//4.0.10.15    amram		2.Change GPI::SetModeDef to check if we can change the mode to new mode

	//							  According the the new flags from agip  if the pump idle and no prp on pump

	//							  (Mode change enabled on Pump with PrP)15988.

	//							3.Cash acceptor transaction which authorize and then nozzle replace will move to Wait timeout status (TD 16094)

	//4.0.10.22	   marik		1.Cash acceptor transaction was authorize with barbig volume.

	//4.0.10.23	   marik		1.Add device id to cash acceptor transaction.

	//4.0.10.24    Marik		1.Transaction stuck  INProgress TD 15755. 

	//4.0.10.24					2.PrePay transaction which can not accepted by the controller will move automaticlly to wait time  TD 14730 , 15959
	//	
	//4.0.10.24					3.PAK transaction which send to PumpSrv while recovery without qdx , insert invalid pump number to transact. (TD 16515)
	//4.0.10.251				1.Add date and time to Cash acceptor OLA record.
	//4.0.10.30					1.Cancel cash  acceptor transaction not creat PAY in Transaction TD 16549 
	//4.0.10.31					1.Add new flags QUERY_MASK2 to get Prepay's record for special Pump TD 16677
	//4.0.10.32					1.Add flags PREPAY_PROMPT_DISABLE to diesel Prepay  , in order not display the Prepay limit 
	//4.0.10.40					1.Fixed , Service fee parse the field by 1000 instead by 100  TD 16716
	//4.0.10.41					1.Attendant trs that was convert to PAP charge service fee in zero trs (TD 16348)  	
	//							2.Avi conflict value OVERWRITE  the OLA data TD 16516 
	//4.0.10.42.				1.Support status W_time out in setcashaceeptor
	//4.0.10.43					2.Return Get Current transact for cash acceptor transaction.
	//4.0.10.44					2.Cash acceptor transaction can not be printed on the pospump TD 16541.

//
//4.0.10.505	Marik			1.Add new field sPositionRestrictionActive to OLA struct indicate that PumpSrv working with postion restriction.
//4.0.10.505	Amram			2.Add treatment  EOS in pre pay.
//								change struct  SHIFT_REC TD 16952
//				Amram			3.Add method In CServerMain::ModeSchemeProcess() we didnt save  in our data the value from the registry.
//								  Add method m_cPumpArray[p-1].SetModeService(lNewServiceMode);//amram 4.0.10.505
//								  after the reading from the registry. TD 16857
//4.0.10.506	Marik			1.marge pilot changes.
//								2.PIC report TD 14757
//4.0.10.507    Amram			1.Fix Patt Trs can't be paid after system failure recovery while dispensing,  add save PapInfo to SetPaPSwipeData TD 17464  
//								2.Fix bug in GetGradePosition method casue to crash TD 17857. 
//Updates						3.Update Pilot development up to 4.0.10.52 
//								4.Fix Return SaveCPumpFile by Ref ,PumpSrv Crush during PATT with AVI loyalty (TD 18023) 
//Updates						3.Update Pilot development up to 4.0.10.54 
//								5.Fix TD 16857 save the levelPrice in case the system in manual 
//									
//4.0.11.0    release 1011 
//4.0.11.500					1.Fix PakReadyTrs in avi conflict copy the new value to sFuelAmt TD 18147	
//								2.Fix Car wash add new byte to struct in case the operation failed
//								3.Fix PCI from PosPump.if PCI is on the copy the account to track2
//								4.support TRACK3 - add new method GetCardInfoEx() TD 16612.
//								5.Change the mechanism  of GetPrePaidTrs2__ that will return first and next according to flag TD 18887
//								6.Add new flag TRS_ATT_TO_PAP to UpdateAttSale to save the data to OLA QDX for vp TD 18445
//								7.Add new flag to CMD_STRUCT_AUTHORIZE_LIMIT to show in cash acc trs 
//							      To show 'Authorize/Prepay at Pos-Cash acceptor' in Pos TD 16880
//								8.Change the return of tank level : return 0 if grade not valid TD 18512
//4.0.11.501					1.Update RTAPS : 
//								1.1 add check in olabase::ParseData to MediaMasterType 
//								1.2 add Parse to Point 
//4.0.11.502					1.Avi conflict while status is authorize case to memeory overrun in pumpSrv. TD 19086	
//4.0.11.502					2.update ongoing version
//								  a .Merge Pilot version 4.0.10.93 to 1010 on going .
//								  b. Agip - 
//										1.Add ability to recall all Prepay/CashAcceptor trs through GetPrePaidTrs2 (TD 18512) BP TD 18887
//										2.Allowed recall only not offer Cash acceptor/ prepay trs  TD 18512
//										3.Avi conflict while status is authorize case to memeory overrun in pumpSrv. TD 19086	
//										4.Pay out not create on Cash acceptor over dispensing TD 18200
//										5.Over dispensing PrePay/ cash acceptot not update the unbalance counter TD 15962
//
//4.0.11.502					4.Fix the log print in pci TD 18932
//								5.Fixed Cash acceptor over dispensing decrease the unbalance PrePay by mistake TD 15962    	
//								6.Add Save to Zone in receipt extra file,Add new flag TRS_INCLUDE_ZONE  TD 14562.
//								7.Send to ola ItemPriceLowX[S]  - low ,ItemPriceHighX[S] - high.
//4.0.11.503					1.In Case the trs is att than save in track2 the att id TD 19197.
//								2.Add new Res - RES_EXT_LOYALTY_AVI TD 11982.
//								3.Change the Code of lock trs in GetPrePaidTrs2__ TD 19365
//								4.Develop CR 17627 PAID TRS
//4.0.11.504					1.Save in SetPapSwipeData the att id in att trs TD 19179
//								2.Add new Flag TD 19179
////							3.Update Pilot devlopoments form 1010					
//								4.Add check to AuthorizeAndLimit only if the pump idle after time out we authorize the pump TD 19896.
//								5.Change the type of att trs to cash in case they paid in injenco TD 19434
//								6.Fix AddedPreAuthInformation the ola buffer was incorrect TD 14868
//	merge 1010 until 4.0.10.141
//	
//merge 1011 until   
					//								1.PumpSrv Memory Leak while Pos request Alarms by category TD 19689.
					//4.0.11.81			marik		2.In case RemoveDataFromLog buffer Get data without end delimiter ',' PumpSrv was crashed. 
					//4.0.11.90			Marik		3.Carwash Purchase Cancelled At POS create Pay@kiosk TD - 17542 , TD 19803		 
					//4.0.11.96			Marik		4.Clear ExtraInfo before create new Cash acceptor record
					//4.0.11.111		marik		5.PIC report does not accumulate  the"Actual section" by day TD 14757
					//					Marik       6.Clear automatically fee charge in case PAP zero transaction TD- 20177 
					//					Marik		7.Create registry key "TransmitDir" under paths\long
//4.0.11.505		Marik		1. Add new ola field CustomerValidationType[I]  TD 17316
//								2  Add new flags TRS_OLA_FORCE_CLOSE_AS_UNPIAD  to method PAPReadyTransactrion, this flags closed the OLA trs as UNPAID  TD 17316	 
//					Amram		3. Fixed the ola rec that save in case trs paid in ingenco TD 20487
//								4. Remove the check in AuthorizeAndLimit to Cash acceptor TD 20516
//4.0.11.506		Marik		1. Support new OLA string OLABatchId[I] 
//					Marik       2. Change the initialized Carwash status to ON_LINE TD 20598			
//								3. Cancel Cash Acceptor Transaction will close as PAID trs. and the customer can get his money back inside the kiosk. TD 20510
//Update 4.0.11.116				4. On fail Generate PIC report Return list of open drawer.
//4.0.11.507		Marik		1.Fixed the saved data to ola rec. Add 0 before the number TD 20487
//4.0.11.509		Amram		1.Add save to price level in case we work on ManualModes TD 19434
//								2.In case the trs was paid in injenco the type of the trs will handle  by RFS TD 21016
//					marik		3.Add grade id in zero complation in Diesel pump
//								4Cash acceptor that cancel due to timeout can not be pulled  TD - 21084	
//4.0.11.510		Amram		1.Send to ola the high price as credit and the low price as cash
//
//4.0.11.511		Amram		1.Fixed GetTransListQuery .return to RFS the paid and unpaid trs in the right order.
//4.0.11.512		Amram		1.Fix AddedPreAuthInformation the ola buffer was incorrect TD 14868,21685
//4.0.11.513		Marik		1.Fixed , When working with grade restriction and no grade limit was return from OLA, we set wrongly the grade limit TD 21963 
//4.0.11.514		marik		1.Fixed, PosVoid was not happens due to wrong database validation check  TD 21034 
//Update 4.0.10.191
//	
//								2.Add grade id in zero complation in Diesel pump
//								3.Lock Prepay in process trough method GetPrepaid()
//								4.Block the option to recall fleet Prepay between Trs diesel And reefer .
//								5.PAP that was got network failure should convert to PAK trs
//								6.Reefer cancelation was not detect when Idle state was report while authorized of reefer.
//								7.Remove extra logs 
//								8.Return POS Prepay buffer in case trs is lock
//Update 4.0.11.150
//								9.Send to ola grade price credit low price cash
//								10.PrePay with grade discount convert to Nozzle restriction discount. TD 19658 
//								11.Attenant trs with payment was stack in PumpSrv TD  19324 
//								12.PrePay with grade discount convert to Nozzle restriction discount. TD 19658 
//								13.PrePay cancel while the trs is in middle of process not allowed. TD 21635 
//								14.Fix , When working with grade restriction and no grade limit was return from OLA, we set wrongly the grade limit TD 21963 
//4.0.11.515					1.Add new stop condition to stop the loop in  GetTransListQuery TD 21147.
//4.0.12.0 release 1012 
//4.0.12.501        Ravit       1.Init the Alarms counters after recovery of the AlarmSrv TD 22006     
//			        Ravit       2.Add array of Timers, send misc to PosPump in case of PAP failure as long as the timer of that pump is running. TD 21342     
//					Marik		3.Cash acceptor restriction was not set
//					Amram		4.Fix AddedPreAuthInformation in COLAPositiveIFSF that send to ola TD 22629.
//					Amram		5.Add new flag PREPAY_WITH_XML_BUFF TD 21584
//					Ravit		6.Check the length of AccountNumber before encrypt TD 23010.
//								7.Authorize second reefer transaction only if diesel was fueled.
//								8.Force Save ResExt fleet.  
//					Amram		9.Develop CR 21584 - support XML that send from pos to rfs.
//								  Save the data in OlaTmp QDX.
//4.0.12.502		Amram		1.Develop option to work with cash acceptor and PRP trs together TD 22070
//								2.Develop option to show decline PRP on the pos pump CR 22685
//4.0.12.503   				    1.Check the status of the system in case restart pump buttom is press.TD 23269
//								2.Add ola string TruckingCompanyName[S]. TD 22961
//								3.Fix cancel ca in case the pump not in valid mode TD 23553.
//								4.develop option to buy car wash with out fuel trs TD 21583.
//4.0.12.504   				    1.Raise event on price change Success /failure , when working with Price change acknowledge from controller TD 13090
//4.0.12.505					1.Add new ola to Tesco TD 21187.
//								2.Add new ResExt RES_EXT_AUTO_AUTH and authorize the pump is case this resExt is on TD 21585.
//								3.Fix the print to log in PumpTotals TD 24147.
//								4.In case of save IniFile param with MultySync active, send to HydService the flag HOP_CFG_INI_FILE and the right file path.(TD 24211)        
//4.0.12.506					1.Add support in new regions: Europe + Australia. (TD 23311)
//								2.Fix the transfer of the transaction number from one computer to another in MultiSync (TD 24211)
//4.0.12.507					1.Send new alarm ALARM_FORECOURT_IN_MAINTENANCE when the forecourt is locked. (TD 21586)
//4.0.12.508					1.Car wash IPT transaction is not saving to OLA qdx at the end of transaction
//4.0.12.509					1.Add new flag to registry TLS_CaptureFile TD 24776.
//								2.move the send of the Alarm ALARM_FORECOURT_IN_MAINTENANCE to the start of the shift change (TD 21586)  
//								3.In case there att trs in the system we will check the status of the ola in case his 
//								   different from OLA_NONE we will return error code TD 24645.
//								4.For MultiSync - update the transactions of the pumps from the qdx (received from the other computer) only if the server is not in IDLE mode (TD 24588).
//4.0.12.510					1.In set shift if we got DO_MAINTENANCE_LOCK_FORECOURT we will update the flags to lock 
//								  the forecourt TD 24807.								
//								2.Add extra log for Carwash IPT Flow
//4.0.12.511					1.For MultiSync: if the transaction record from the qdx is older from the transaction in PumpSrv memory - the trs will not update (TD 24912).  
//4.0.12.512					1.In delivery report if the flag DELIVERY_REPORT_INVOICE_DATA is on - the data will be save to the qdx (TD 18028).  
//								2.Add support in GetReceipt2 from Ctrl interface (TD 21192) 
//4.0.12.513					1.Add Check when loading transact information from the qdx, only if the load is for MultiSync, then if the trs in the memory is more updated, it will not be taken from the qdx (TD 24912) 
//								2.Init temp pump with the current information of the pump before putting it back to the memory (TD 24912).
//4.0.12.514					1.When working with Car wash as IPT do not send PAPSwipeResult
//4.0.12.515	Ravit			1.Add support in rollback on failure of price change. send relevant alarms on price change process (TD 21629)
//				Marik			2.Trs stuck W_process When working whith Controller authorized PAP TD 22521.
//				Ravit			3.Allow transaction that waits for reservation to be moved (TD 25461)
//				Amram			4.Change the status of cash acceptor trs  to paid in case cancel was send. TD 24740.  
//				Ravit			5.If a record of extra info for a new transaction is already exist, update this record with the new information (instead of insert). TD 25487.
//				Marik			6.Add new C&P OLA string TD 21186
//4.0.12.516	Ravit			1.Update the transaction time when the transaction is been paid. TD 25572   		
//				Amram			1.In case avi conflict we check if the  status of the trs different from IN_PROGRESS or WAIT_PROGRESS if so we create new trs. TD 25534. 
//4.0.12.517	Amram			1.Add option to post void trs in pilot.parse the xml that return from the pos and insert into qdx CR 24328.
//				Ravit			1.Add support in rollback on failure of price change. (TD 21629)
//				Ravit			2.Allow TankNumber and EndTime to be 0 in delivery report if the flag DELIVERY_REPORT_INVOICE_DATA is on. TD 18028.  
//				Amram			3.Save the discount in card_sale_data at the end of the trs. TD 25507.
//								4.Fixed RemoveCardInfoFromMessage that cause PumpSrv to crashed. TD 23615.
//				Marik			5.Transaction wasn't send to Controller in case controller is master on Transaction ,in Prepay Transaction 
//4.0.12.518	Marik			1.Update Pilot changes (short card name) , Add car  was sale to xml File TD  25289 
//				Ravit			2.Save information aboult - if we are In Price change Process to the qdx. TD 25967.  
//				Ravit			3.Change the category of price change alarms. TD 21629.
//4.0.12.519	Marik			1.Service fee  not charged in a PATT Trs when the manual sevice fee is turned on TD - 26087
//4.0.12.520	Marik			1.Cash acceptor Reprint does not have the Preauth limit TD - 23292	
//4.0.13.0		Marik			1.Formal Release.
//4.0.13.500	Marik			1.Add GetProcessStatus second condition to Indicate that maintenance is in Progress TD 26768	
//				marik			2.ADD Car wash IPT Sale to OLA only if transaction was approved.
//				Ravit			3.Count the number of severity alarms. TD 23530. 
//				Ravit			4.Support in maximum of transactions and in timer of closing PrePay transaction. TD 23050. 
//				Ravit			5.Save ModeScheme definitions only if the entry is valid and the new values are different from the current values. 
//4.0.13.501	Marik			1.Add new type of MediaMasterType CARD_TYPE_TEST_CARDS  for test card. TD 21190.
//				Ravit	        2.Add parameter "DivisionFactor" to PricePole in the registry. TD 26573  
//				Marik			3.Add new registry key ReceiptPaddingBlanks  to terminal Table TD - 26334
//								4.Support status Price change Status.TD 23050	
//								5.Support OLA prompt timeout tag PromptTimeoutSec0[S].
//								6.Cancel Sale Transaction not close OLA session TD 27203
//				Ravit			7.Add parameter "LeakThresholdVolume" to Tank level in the registry. TD 24997
//				Marik			8.PAP result buffer corrupted  TD 27375.
//4.0.13.502	Marik			1.Add new OLA tag LoyaltySessionID[I] 	
//4.0.13.503	Ravit			1.Add interface methods of TankReading that receives and sends Xml data of Tank information. TD 24618   	
//				marik			2.Overwrite zone in case exist one.
//				Ravit			3.If the pumps are in stop all status, no pump will be authorized. TD 27001
//				Marik			4.Add new field sCVMVerificationMethod OLA.h , This field indicate Pin operation mode in C&P 
//								5.Reprint cash acceptor display amount divided by 10  TD -23292
//				Ravit			6.Support the process event - terminal download. TD 21191
//4.0.13.504	Ravit			1.Add HostOnLineID field to Ola buffer. TD 24971
//4.0.13.505	Ravit			1.Move OlaSrv type definitions to OLA.h 
//				marik			2.Get Delivery emulation status return wrong value 
//				Ravit			3.If min severity is 0 (off), don't increase the severity for the alarm. TD 29110          
//4.0.13.506	Ravit			1.If we got the flag EXPECT_ALL_DETAILS_IN_NEED_MORE_PARAM from the convert layer, 
//								  copy all of the card sale data buffer in need more params.    
//4.0.13.507	Ravit			1.Insert zone field to the receipt qdex.   
//				Ravit			2.Change VP initialization number to 5.   
//4.0.13.508	Ravit			1.Change the delimiter of the record in Get/Set record. TD 25999  
//				marik			2.Transaction with PrePay Receipt was not removed form qdx on end transaction
//								3.Prompts record remained in the qdx when  PAP transaction fail to get approved.
//				Ravit			4.Add fields to tank level registry. TD 24618.
//				Marik			5.When working with OLAProgress call direct GetCardinfoEx  
//				Ravit			6.Fix copy of ola out buffer. 
//4.0.13.509	marik			1.Add new OLA field ReasonOnLineCode[I]
//4.0.13.510	Marik			1.Send to Ola tag PINEncryptionKey[S]
//4.0.13.511    marik			1.Send to Ola tag CardType[I].
//								2.Update fixed of connection to  RTAPS.
//4.0.13.512	Ravit			1.Add field "SimTb" to GeneralParam in the registry.
// Update before release 1013
//1. PumpSrv stored  corrupted data inside  Extrainfo qdx .TD 26247.& 26251  & 26253 & 26279 & 26281
//2 .Move the encrypt part of the account number to param2 in OLA Qdex before encryption.TD 26473
//3. PAP Transaction wasn't convert to Attendant TRS. TD 26536
//4. Remove condition that checks if RequiredMaintenanceProcess is on, than Restart pump not allowed will return.TD 26572.
//5. Support in PAK authorize timeout in case of ResExt that is auto authorized. TD 26615.
//6. For manual shift change, take the current date with the configurate hour. TD 26625
//7.  Accumulation shift total won't summarizer car wash sale into accumulation data TD 26636.
//8.  Add GetProcessStatus second condition to Indicate that maintenance is in Progress TD 26768
//9.  Encrypt track2 before save PAP transaction. TD 26804
//10. Copy the encrypt part from the account number at the end of PAP transaction.  TD 26805 
//11.Memory overrun on paid Loylaty card from Pos TD 26828.  
//12.Encrypt the account number before send the swipe buffer to RFS. TD 26876
//13.If the pumps are in stop all status, no pump will be authorized. TD 27001
//14.For manual shift mode, even if the current shift date is earlier from the current time, init the new shift with the current shift. TD 27710. 
//15. Move Accepted trs to Paid if we got requesting status. TD 28079.
//16.Decrease number of accepted transactions before send numerator to PosPump. TD 28203

//4.0.14.0		Ravit			1.Formal release. 
//4.0.14.500	Ravit			1.If PCI is on convert Track2 to encrypted account number in get receipt.
//				Ravit			1.Add flags to mode info in get scheme mode.
//				Ravit			2.Remove only PrePay transaction in accepted state in counter that received by PosPump. TD 31217
//				Turj            3.add support to OLAInterface (new OLA for TESCO) exe rapper
//								  identified as olaprogressEx = 10 TD - 31417	
//				Ravit			4.Convert CardSaleData struct to Xml buffer. TD 11695 
//				Ravit			5.Add RollbackAfterTrsDelay field to Pump level. TD 25856  
//4.0.14.501	Ravit			1.Add protection for lock the forecourt after unbalanced PrePay. TD 32676
//				Ravit			2.Support delete trs in paid qdex by a flag. TD 29507
//4.0.14.502	Ravit			1.Support devices versions in xml format. TD 29590 
//				Ravit			2.Add IsMaster field to the information PosPump get. TD 27810   
//				Marik			3.Add new OLA string FuelTrs[I] for report Fuel transaction to OLA TD 32679
//Update 1011	Marik		    4.Send Grade price in complete TD - 32228		
//4.0.11.410					5.Add extra protection in paid  attendant transaction  TD -30347.	
//								6.When new transaction create check if exist record in Extrainfo qdx with same key , In case exist delete it ,TD 31060.	
//								7.PumpSrv skips on Request shift Pump total Whenever shift started after process of shift with lock forecourt.TD 28180
//								8.Add extra protection to extra info. TD 30636
//				Ravit			9.Check the struct in swipe result. TD 33070
//4.0.14.503	Ravit			1.For UPS lock: unlock the forecourt only by unlock UPS or by force unlock. TD 30414 
//				Ravit			2.Add log lines for Xml problems.    
//				Ravit			3.Add support in Unmanned mode. TD 28304
//Update 1014	Marik			4.Add two new OLA string OLAQualifySpend0[I] OLAQualifySpend1[I].  TD 32633
//								5.Add new OLA string IssuerScriptData[S] TD - 33260	
//								6.GetCardinfEx  did not return to Cl2PumpSrv flags indicate that Session was open 
//								7.Parse ola string ClubCardTrack2Data.
//								8.Track2 need more param was not detected when OLA string ClubCardTrack2 exist in buffer.
//								9.GetCardInfo was not expose in ola COLAProgressSrvEx TD - 31804.
//								10.Support ola fields , PromotionNumber2 & AmountByPoints2 
//								11.Change ola fields name , PromotionNumber1 & AmountByPoints1 
//								12.Add new OLA string  IssuerAuthenticationData[S]"
//								13.PAP result code should not be overwrite.
//								14.Add two new OLA string OLAQualifySpend0[I] OLAQualifySpend1[I].  TD 32633
//								15.Add new OLA string IssuerScriptData[S] TD - 33260
//				Ravit			16.Don't allow more then maximum number of Unpaid + Timeout PrePay. TD 33494
//4.0.14.504	Ravit			1.Fix delete of ola records. TD 33384	
//4.0.14.505	Marik			1.TD - 33745 , Transaction service mode was not stored correct in PAID qdx. 
//								2.TD -33829 Add support for new ola code ResultCode[S]= 231
//				Ravit			3.Xml value of SlipFlags will be converted to ascii char. TD 33732   
//				Ravit			4.When getting receipt with PCI, check that the encrypt part match to the returned receipt. TD 32885   
//				Ravit			5. TD - 33769  Before encrypt loyaltyId, move the encrypted part to param1.    
//4.0.14.506	Marik			1. TD - 33976/33852  Cash Acceptor transactions were insert to PAID Qdx with POS ID.
//4.0.14.507	Ravit			1. TD - 29590  Change the format of the versions. 
//				Ravit			2. TD - 31281  Add ctrl event of reconfigure for data export.       
//4.0.14.508	Ella			1. TD - 34038  On AVI Conflict before reaching the conflict limit, the TranStatus wasn't saved correctly.
//				Ella			2. TD - 33858  CardSaleData was not updated properly from XML and the data was not copied.
//				Ella			3. TD - 34157  Flag(GPI_PUMP_TRS_LIST_FULL) was not updated properly on Authorize pump when checking the maximum allowed unpaid transactions.
//				Ravit			4. TD - 32885  When getting receipt with PCI, check that the encrypt part match to the returned receipt in Pos function. 
//				Amit			5. TD - 33778  Fix a bug in OperationEx REC_UPDATE. The MaxCopies was override with zero.
//				Marik			6  TD - 32090 ,28144,Transaction that ended in middle of shift process Gets new shift ID. .
//Update from 4.0.11.451/452	7. TD - 32205  Add extra logs for trace after missing media mastertype.  
//								8. TD - 31108  Attendant Transaction should not open while shift was in process.   , 
//								9. TD - 29502  OLA buffer was not would in case shift in progress  
//								10.TD - 32842  Attendant Transaction would not open while session clock in process 
//								11.TD - 33795 , 29681 When moving PrePay with discount from Pump to Pump Discount limits were deleted  
//								12.TD - 29666  PAK PreSet transaction will be block in case PrePay in Progress 
//								13.TD - 30765  When PAP transaction ended save all pump information to QDX 
//								14.TD - 31326  Add receipt index to buffer return Cl2PumpSrv whenever AVI reprint receipt been request  
//								15.TD - 33795 ,29681 When moving PrePay with discount from Pump to Pump Discount limits were deleted 	
//4.0.14.509	Marik			1. TD - 33793  Reprint receipt missing Transaction details.
//4.0.14.510    Marik			1. Create registry key Tanks\ActiveTankConnectedPumpsTable  .	
//								2. TD - 34493  ATT transaction could not reprint
//								3. XML file was corrupted when working with PCI.
//4.0.15.0						1. Formal release 1015
//4.0.15.500	Ella			1. TD - 34496  Add field to QueryData 'sNonDiscountTotalAmount'
//				Marik			2. OLA field IssuerScriptData is send to cl2PumpSrv	 	
//								3. TD - 22963  Add new OLA field	(OverfillAdjusted[B]) to indicate PAP Overfill transaction.
//								4. Attempt to read Prepay transaction through QueryPrePaidTrs Locked all Active Prepay's  	
//								5. Alarms buffer was allocate without free
//								6. Cent off discounts in position restriction are not send correct to Cl2PumpSrv  TD - 35527.
//								7. Update Mode change also when pump is offline. TD - 35177	
//				AmitH			8. TD - 34487 The TAG_EXTRA_FILE_INFO was not fully copied (sGRDiscountPerGallon)
//				AmitH			9. TD - 33133 Do not call GetPAPInfo in CGCI::SetPAPSwipeData before converting the xml
//				AmitH			10. "Transaction" section was more than once in XML.
//				AmitH			11. Reprint of the same transaction could be done more than Max number of reprints prm 
//				AmitH			12. the reprint of Last AVI transaction did not work.
//				AmitH			13.TD 35538 - Reprints some times where empty.
//				AmitH			14.TD 34487 - One More field was not copied in TAG_EXTRA_FILE_INFO 
//				AmitH			15.TD 34067 - CheckAviConditions - Do not block AVI update in case pump is in REQUESTING , NULL_RES
//				AmitH			16.Set shift failed because of xml convert error. 
//				Marik			17.TD - 33638 Memory Overrun on calling method GetPumpTotalInfo, for get memory Pump total information.
//				Marik			18.TD - 36091 Attendant Trs could not paid while Shift take place.  .
//								19.TD - 35193 35242 On create PAP new transaction check if transaction already opened and if it is use it , instead open new transaction.	
//								20.TD - 34954 Service fee charge on pump without service fee. 	
//								21.TD - 35466 While PrePay in progress block attempt PAK transaction.  
//								22.TD - 36392 Request for need more param for club card get PumpSrv for endless loop.   		
//								23.TD - 36203 Delivery xml file converts wrongly Time stamp.  
//								24			  Add new OLA tag CardHolderVerifyMethod[S].	
//								25 Extend sIssuerApplicationData & sAID  fields TD 36904
//				Ella			26 TD 19675 - User PreSet values will be stored as part of transaction data
//
//4.0.15.501	Marik			1. TD - 36983 Delivery XML data were parse incorrectly.  
//								2.			  Add two functional ability to Delivery emulation
//												a.When delivery emulation timeout define as zero , Let process to be perform without locking forecourt.
//												b.Expose delivery emuldation status to clinet using method GetProcess.			
//								3.TD - 36902 Loatlty session does not parse discount value .
//								4.TD - 32227 OLA discounts were not parse appropraite 
//Merge pilot version
//								5.Prompts record remained in the qdx when  PAP transaction fail to get approved.
//								6.Support of Second fuel trans Timeout TD 27910
//								7.Post void transaction return corrupted transaction value & volume
//								8.Send command reservation PrePay to pump that in status Ready , Prepay
//								9.Reefer details corrupted in PosVoid 
//								10.Carwash status was set as defulat off line TD  - 31122
//								11.Decline misc was set whenever we get misc decline Prepay without check existing to Transacgtion   TD - 31502	
//								12.Reefer limit was not force on Prepay "Both" TD -31585
//								13.cancel reefer PrePay transaction was not perform when second transaction change to timeout. 
//								14.Add debug line with account number in need more param in order track after missing account number.
//				AmitH			15 TD - 37471 - Memeory leak in CGPI::GetAlarmByCategory
//				Marik			16.Change OLA string from  AmountByPoints1 to AmountByPoints2
//								17 TD - 34437 Disable double preset command 
//				Ella			18 TD 35841 - 'SlowDownVolume' registry parameter was added
//				Marik			19.Send OLA string AuthTerminal[S] to ola always		
//				Ella			20 TD 19675 - User PreSet values
//				Marik			21 TD 32030 , Cancel PAP transaction in middle of need more param should not perform session complete 
//				Amit H          22 TD continue 37471 - Memeory leak CGPI::GetAlarmByCategory
//								23 Repair main thread for handle precise timers !!!!! TD - 36062. 	
//           	Amit H  		24.Set alarm OFF also if pump status is COMMS_TERMINATED and not only by OUT_OF_SERVICE as today (CAlarmSrvPS::PumpStatusAlarm)

//4.0.15.502	Ella			1.Fix for DataExport, PumpTotals were not read correctly (indexes problem)
//								2.Fix for TD#10856 - AuthorizeSessionID and OldSession were not passed correctly to OLA and RFS
//								3.New field 'PONumber[S]' was added to need more param
//				AmitH           4. PumpSrv crash when caling encryption dll.
//4.0.15.503    AmitH			1. GPI::GetParam(...) - ActiveShiftNumber will be taken from ini file and not from registry. TD 33846 , 38407 , 38498 

//4.0.16.0		Ella			1.'sTaxIncluded' field was added to CARD_SALE_EXTRA_DATA3_ for Taxes CR.
//								2.Prevent open new Attendnt transaction while session pre authorized in progress TD -35904
//								3.PrePay discounts were not given in case record exist inside qdx , Add protection clear old record.  TD - 38189 ,  38098 , 38775
//								4.Taxi card discount reported to Pos as nozzle instead as grade position TD - 37867	
//				Ella			5.TD 38453 - New flag GeneralParam\PrintZeroTrsReceipts was added to force saving zero transactions to receipt.qdx
//								6.ItemTaxRate0[F] field was added to Ola buffer, which contains the tax rate (for Tax CR)
//								7.PumpSrv crashed when PCICompliant is on. Terminating Null was missing in bEncryptAccountPumpSrv buffer.
//				AmitH			8. no TD - Do NOT write Pump totals to Qdex when Memo wetstock flags are on. (same logic as in tank reading) 
//				AmitH           9. 1016 Formal release
//
//4.0.16.500	Ella			1. TD 38127 - Synchronize shift number between POS/VP and PumpSrv. In order to get shift status and current fuel shift number, 
//								   POS will call GPI::GetProcess method with PROCESS_GET_SHIFT_GPI process number and VP will call CTRL::GetProcesStatus with PROCESS_GET_SHIFT_CTRL process number.
//								   In case shift is in progress return POS/VP the current status(non-zero value).
//								2.Fix CTRL::SetParam method for "Taxes" registry table. "es" string should be removed to refer to the taxes sub-table, for example "Tax01", instead of just 's' as in other cases(TD #30944).
//								3. TD 37522 - Create the following pump precision keys in the registry: PPGScaleFactor and ValueScaleFactor
//								4. Internal fix in CBaseReceipt::SetPapInfoStruct function related to cTmpXmlInterface (move it from member variable to function's local).
//				AmitH			5.  CGCI::PAPReadyTransaction__ do not fill missing final transaction data in case of flag STRUCT_IS_PAP_CANCEL
//									and and HydraSrv running.This casued out of sync incase of zero trs.
// Merge pilot version	AmitH	6.TD - 22684. Move prepay only if the source and the target pumps are of the same type (according to prm diesel_pump)
// Merge pilot version	AmitH	7.TD - 39646. Add new field to the xml for pilot in CCardSaleAllData::CreateXMLFromStruct  : sItemRestriction
// Merge pilot version	Ella    8.TD - 10718. Two new registry keys were added under each pump: Station_Sales_Display_Mode and Station_Sales_Display_Delay
//						Ella	9. Fix for Tesco. Scorpio files are not created now when pressing the WetStock button in PosPump, as a result of wetstock memo event.(Merge from 1014)
//						Ella	10.TD 34815 - Car Wash Auto Update. This development enables imported Car wash items to automatically create/update Car wash programs in the fuel system.
//			Amit H  29/01/07    11.OLA was not released correctly uppon shut down.
//			Amit H	29/01/07    12.In CTRL GetParam3 method, grades were taken from the registry instead of from the INI file. 
//			Amit H  29/01/07	13.Fix ReSend[B] - field was not saved to xml currectly.
//			Amit H  29/01/07	14.TD 39439  - Clean W_progress trs when a loyalty card swiped, but cancel was presed.
//			Ella	29/01/07	15.TD 34675 - PrePay Pump Test. When there is no PAK defined in pump mode, force PAK in case we get the flag FORCE_PAK_WHEN_NOT_IN_MODE(new) in AuthorizePump2.
//
//4.0.16.501    Amit H  16/02/07    1. PumpSrv crushed when working with RouteSrv.exe - The buffer was too short in 
//									class CCmdInfo , member m_Data (was MAX_CMD_DATA_SIZE MAX_XML_BUF_LEN)
//				Ella	19/02/07	2.TD 38806 - Allow to show authorized PrePay trs in POSPump all trs screen.
//4.0.16.502	Ella	18/03/07	1. Beta Release
//4.0.16.503	Ella	21/03/07	1. TD 40180,49011 - P@P Trs stayed in progress after Cancel.Merge from 1015. 
//									2. Improvement of the maintenance algorithm of the receipt Qdex. Merge from 1015. 
//									3. TD 42665 - PAP receipts were not consistently printed. Merge from 1015. 
//									4. TD 42666 - Pumpsrv log was showing card account numbers in the clear. Merge from 1015. 
//4.0.16.504	Liat	27/03/07	1. TD 37823 - Update Yoman with lost Dallas keys (Add new fields)
//				AmitH				2. TD 42883 - For OLA RoutSrv INQUERY session - in GCI PapSwipe send a new Return code to Cl2PumpSrv - GCI_SERVER_IN_THE_MIDDLE_OF_OLA_SESSION
//												  So it will retry PapSwipe go out to OLA SEND_END.
//				AmitH				3. TD 42660 - Support one new open Need more Param : KeyboardMask[I]
//				Liat				4. Fixed in reprint scenarios, TD 41878, 43204 (Merge from 4.0.15.91)
//              AmitH	29/03/07	5. TD 42883 - Continue - Do not send SwipeResult to Cl2PumpSrv in case of retry of PAPSwipe()
//			 	Yaron	10/04/07	6. Fixed Pinpad action for debit, TD 41657.
//				Yaron   10/04/07	7. Add support to OLA.Net, change GetFunctionPointers(...) mehtod.
//				AmitH	10/04/07    8. TD - 39645.(Also 43725) On Cancel PrePay trs only PrePay qdx was cleaned and so it still was seen in PosPump trs screen, to solve this problem now the trs is cleaned also from PumpStat qdx. 
//				AmitH   11/04/07    9. TD - 43710 max number of reprint did not work when printing from POS.
//				AmitH   11/04/07    10. Reduce cpu time in TrsLinkOperation method by reducing the xml converstions. (merge from 1015)
//				AmitH	11/04/07	11. Fixed in reprint endless loop scenarios. Protect from illigal xml from entering the Receipt Qdex. (merge from 1015)
//				Liat	19/04/07	12. TD 39201, 37925 Trs stuck in 'In progress', Do not charge unauthorized AVI when in conflict (Add authorize session)
//4.0.17.0		AmitH	19/04/07	1. TD 43858 - AmountLimit[F] - was not sent in Complete session
//				AmitH   22/04/07    2. PromotionNumber2[S] field was not copied from OLA buffer.
//				Liat	25/04/07	3. (TD 37244, 38651) CRD 7Eleven YX Rolling Update Enhancement, Display Fuel amount (for PAK) on PosPump until transaction is tendered. (FRN)
//4.0.17.0  	Liat	02/05/07	1. (TD 38654, 40492) Add start time & end time per transaction to shift log
//										New interface method CGPI::GetPAPTrs5__ returnes XML to VP
//										Changing numeric version for VP to 6.
//				Ella	02/05/07	2. Merge from 1015:
//										- Fixed problem in FindFirstFreePlace in which receipt was inserted to a smaller gap than required
//										- Fixed problem in Reorganize method. The algorithm of finding the gap was improved
//										- Problem in receipt xml was fixed, where the size of the xml on insert to receipt qdx was different than the size on update receipt qdx.
//				AmitH   03/05/07   TD - 44826 - Receipts with ? in the Track2 could not be printed at the POS. (merge from 1016)
//				AmitH   07/05/07   No TD - Reduce PumpSrv cpu time when IDLE
//4.0.17.10     AmitH   09/05/07   TD -  continue TD 38654, 40492
//4.0.17.11     AmitH   10/05/07   TD 44435 - In case we got set process of price change failure - PROCESS_PRICE_CHANGE_FAILURE 
//											  Do not send RFS the flag: EXPECT_PRICE_CHANGE_RESULT by resetting the original price change flag
//				AmitH   10/05/07   TD 45208 - Could not reprint receipt from POS when in PCI complient mode.
//				AmitH   11/05/07   TD 38654, 40492 - Use the sFiler17[17] field to pass VP the sTrsStartDateTime[14] instead of in the XML
//4.0.17.20		Ella	16/05/07   TD 45270 - When PumpSrv does not act as master - do not update shift number in SetShift function.
//				Ella	22/05/07   TD 26384 - PrePay Unique Reference CR.
//				Marik	28/05/07   TD 45526 - Clear PINBlock field from Qdx and log.

//4.0.17.30		Ella	31/05/07	1. TD 35765 - PIC Force Batch Report CR.
//				Ella				2. TD 26384 - Fixes for PrePay Unique Reference CR.
//4.0.17.500	Liat	03/06/07	1. TD 39361 - StorePoint & Fuel Integration with LPE
//				Ella	11/06/07	2. TD 26384 - Fixes for Unique PrePay CR
//				Liat	14/06/07	3. TD 39361 - Update LPE code with the PAK flow.
//				Ella	17/06/07	4. IndexNumber was not written into the new xml for VP
//4.0.17.501	Liat	21/06/07	1. TD 39361 - Update LPE code
//				Marik	25/06/07	1. TD - 45604 Amount limit parts were switched as result of that the value reported  wrongly.
//									2  TD - 46843 Remove Pin data from xml log.   
//				Ella	27/06/07	3. TD 35765 - PIC Force Batch Report CR (Fixes)
//				Ella	04/07/07	4. Track2 was fully exposed when working with PCICompliant.
//			    AmitH   09/07/07    5. TD 48229 - Post void does not work

//              Amit H  10/07/07 Merges from 1015:

//									1 . CR 44000 - Scorpio additions
//									2.  TD 45340 - The OLA field AmountLimit[F] was not copied currectly in Xml convertor.
//									3.  TD 45340 - continue
//									4.  TD 44000 - Scorpio additions : PumpSrv will save the Pump totals to the RunTime ini file.
//									5.  TD 44868 -Mode setting changed after switch
//									6.  TD 45563 - Add a new counter WetstockMemoCounter[I] to the run time inin file for Scorpio agent ( a DataExport agent) 
//												This counter get ++ every WSM_BY_MEMO occurence
//									7.  TD 45564, 45650 - support parameters in INI file for multi sync redundency
//											[DataExportSrv\SCORPIOAgent] :	PumpReadingCounter[I] , TankReadingCounter[I] 
//									8.  TD 44867 - Fix the method CTRL::IsUnmanned to return the right site mode status.
//									9.  TD 44868 - Mode setting changed Immidiatly after switch
//									10. TD 46676 - Scorpio files: not all records produced in 1st read after switch or after start
//									11. Scorpio Fine tune: when all tank are not avaliable, Do not throw 2 Tank memo readings events.
//									12. TD 46793 - set the wetstock memo according to the clock time and not interval.
//									13. Add protection in division by zero in CServerMain::IsTimeToProcess method. In case MemoInterval smaller then 60 seconds, set default value to 60 sec.
//									14. Scorpio - In PumpTotals2 - count the number of end of list only when event is from Memo.
//									15. Scorpio - add log in CServerMain::SetPumpModeScheme3
//									16. TD 45564  Mode scheems were not sent to the other side.
//									17. In PApReadyTransaction when getting cancel pap after switch - there was a W_Progress stuck transaction.
//									18. site id was not sent to ola if club card swiped twich before payment card
//									19. PromotionNumber2[S] field was not copied from OLA buffer.
//									20. TD - 44410 - IssuerAuthenticationData[S] was not in XmlConvertor, thus did not pass to Cl2PumpSrv.
//
//				Liat	10/07/07	7. TD 39361 - Update LPE code, merge after USA integration
//				
//				Marik   10/07/07	8.Overwrite site limit with debit limit selected.
//				Ella	17/07/07	9. Reprint didn't work with PCICompliant
//									10.TD 38162 - Update discount attributes CR 
//				Liat	18/07/07	11.TD 39361 - Update LPE code to include encrypt account, 
//										on 23/07/07 changing the xml saving files to be according to log level, and updating the force input xml to include, again, the original unit price
//				Ella	24/07/07	12.TD 48586,48678 -  Cancel PAP trs the moment OLA approved the card but before it was notified to RFS (Merge from 1017 Murphy)
//				Liat	24/07/07	13.TD 39361 - Add a field LoyaltyOnOff indicating the status of communication to LPE
//				Liat	01/08/07	14.TD 49399 - Shift process failed as a result of SetMaintenance LOCK/UNLOCK process not closing correctly.
//												Protection was added to prevent starting SetMaintenance LOCK process as long as another LOCK/UNLOCK process is not finished 
//									15.TD 48870- LPE PAK trs - auto authorize is not working after swiping credit card - update ATT_TRAN_TYPE instead of CL

//	4.0.17.502	Ella	02/08/07	1. TD 35765 - PIC Force Batch Report CR (Fixes) 
//									2. TD 48819 - PumpSrv didn't return the drawers list which were not removed on printing the PIC report.
//				Liat	05/08/07	3. TD 42728	- Sainsbury's- RFS, add StoreLine OLA.
//				Ella	05/08/07	4. TD 19675 - PreSet values were not passed to VP correctly
//				Ella	07/08/07	5. TD 49812 - Track2 data available in Database(Merge from 4.0.14.850).
//				Liat	14/08/07	6. TD 48194	- YX Wayne Forecourt Controller XML & Fall Back Handling
//				Liat	15/08/07	7. TD 49339 - Need to have Convert Layer for Allied to allow for Open Prompts.
//									8. TD 40564 - Unrecognized/Recognized bill CR. (Merge from 1017).
//
//	4.0.18.0	Ella	20/08/07	1. TD 40564 - Fix for Unrecognized bill CR
//				Liat	23/08/07	2. (TD 38654, 40492 continue) add start time to TAG_PUMP_TRANSACT3 and return it from CGPI::GetPrePaidTrs2__
//									3. TD 46349 - AVI conflict with pre pay - trs closing time wasn't updated in tra. (Merge from 1015)
//				Liat	28/08/07	4. TD 46512 - Support Zip Code prompting with MTXLinkSrv with 9 digits (FRN)
//				Liat	29/08/07	5. TD 50269, 46682 - Transaction with Texi Card didn't get discount
//	4.0.18.10	Liat	04/09/07	1. TD 50742	- Terminal cannot find receipts
//									2. TD 42465	- 'Card Limit' is displayed instead of Grade when selecting "AllTrs" after prepay for a selected grade.
//									3. TD 51147 - PumpSrv crashed when POS send wrong parameter in method SetTrsState (Merge from 1016 4.0.16.220)
//									4. TD 49812 - Track2 data was available in Qdex on PAP decline when pci compliant is on. (Merged from 1014)
//									5. TD 33898 - QDX maintenance - NOT FINISHED
//				marik	13/09/07	6.   Incorrect flags send to cl2PumpSrv on PAK transaction with volume limit.  
//				Liat	18/09/07	7. TD 48194	- YX Wayne Forecourt Controller XML & Fall Back Handling - continue 
//	4.0.18.500	Liat	20/08/07	1. TD 33898	- QDX maintenance - Not Finished
//				AmitH				2. TD 42728	- Sainsbury's- RFS, add StoreLine OLA.
//  4.0.18.501	AmitH	24/09/07    1. Do not call HydService in LoadParam and CheckValidEntry methods ! Get the parameters locally. Change in param.h
//				Liat			    2. TD 52009 memory leak in GetCardinfoEx method.
//									3. CarWash code was always sent 7 to VP.
//				Liat	21/10/07	4. TD 52335 - OLATmp qdx get full in PrePay transaction - merge from Murphy special version 
//									5. TD 53597 - PrePay transaction, do a move, times out and does not respond to re-auth - merge from Murphy special version
//						07/11/07	6. TD 54576 (same as 29964 - merge from 4.0.13.205/201/204) - Inside Reprint (POS Pump Function) not available for Declined Transactions
//				AmitH	20/11/07	7. Merge from 4.0.15.291 - 
//										7.1 Fix more memory leak in CPosInfo (call GPI::GetReceipt2 lead to the leak)
//										7.2 Add new parmanter GeneralParm->ActiveReOrganizeReceiptQdx (default 1)
//											if 0 - do NOT activate the reorganize reciept qdx mechanism
//				Liat					7.3 MSXml - CreateInstance to CLSID_DOMDocument40 instead of CLSID_DOMDocument30
//									8. Merge from 4.0.15.280 - 
//										8.1 TD 52009 - Add new paramter in GeneralParam to indicate how many times we let PumpSrv to
//												  Check if HydService is updated complete in CanMoveToIDLE method
//										8.2 Add Protection from crashing if FormatReceipt returned empty buffer.
//									9. Merge from 4.0.15.285 - 
//										9.1 Memory leaks - cont. fixing using _bstr_t
//										9.2 Release all the child elements in xml infra - MSXMLInt.cpp
//										9.3 Memory leak in SetAlram method.
//										9.4 Enance XmlConvertor and XmlConvertorEx "CreateXml" methjods.
//										9.5 Import msxml4 instead of 3
//									10. Merge from 4.0.18.15 - 4.0.18.40
//										10.1 TD 33898	- QDX maintenance
//											10.1.1 Add an alarm indicating reload will occur next shift.
//											10.1.2 Fix path of Q-Load32
//											10.1.3 Add protection from starting qdx maintenance to the receipt qdxs when we are 
//													in the middle of reorganize or doAutoReceiptCheck
//											10.1.4 Wrong qdx number was loaded + update reconfigure to include changing of LoadEveryXShifts.
//											10.1.5 Fix ReceiptQdx could not insert data from bin files back to Qdx.
//													add logs to the process, move m_lSeqNumber to uppr parent class.
//										10.2 TD 42728	- Sainsbury's- RFS, StoreLine OLA - add support to GetCardInfo. When getting GetCardInfoEx from RFS, lead OLASRV_STORE_LINE to call GetCardInfoEx  
//										10.3 Add 2 new flags to Server main to signal if we are in the middle of receipt ReOrganize or in the middle of DoAutomaticReceiptCheck process. 
//									11. Merge from 4.0.15.287 - Mem leak in CXMLInterface::GetXmlString
//									12. TD 54425 - Reverse XML internally in PumpSrv.
//	4.0.18.502	Liat	27/11/07	1. TD 38162 (CR) cont.- Update fuel with discount attributes (FRN)	. 
//										ExtraInfo qdx record was increased from 600 to 800 bytes.
//									2. TD #38654 cont.- increase PrePay qdex file size to include start Time. Merge from 4.0.17.23
//									3. TD - 52335  OLATmp qdx get full in PrePay transaction.
//										Increase number of records in OlaTmp qdex. Merge from Murphy version 4.0.17.22
//	4.0.18.503	Liat	27/11/07	1. TD #25853 - Upload Avi Device Type In TR file and to the central online authorizer.
//	4.0.18.504	Liat	29/11/07	1. TD 54295 - Pump Test Cashier Authorization.
//									2. Add LoyaltyEntryMethod field and send it to OLA. Merge from 1018.
//				Liat	04/12/07	3. TD 55687 - PAP transaction becomes PAK transaction after EFT card had been approved at OPT. - Merge from 1018
//									4. TD 38162 (CR) cont.- Update fuel with discount attributes (FRN) - Update the counter of the discounts to count maximum 4.
//				Gena	05/12/07	5. Merge from 4.0.16.280 - Account number expose to PumpSrv client. 
//									6. Merge from 4.0.16.310 - Support discount type  flage in Prepay with discount.	
//									7. Merge from 4.0.13.820 - TD 53423  Encrypt all PCI data in Database.
//									8. Move to Msxml4.dll Take new MSXMLInt.cpp/h after it was relinked to PumpSrv in source safe. 
//						09/12/07	9.	Merge from 4.0.17.191 - 
//										9.1	-	TD 51147,51466 - continue Fix 
//										9.2	-	TD 51320 - Save the Receipt of ubrecognized Bill Pic trs to the reciept Qdx.
//									10.	Merge from 4.0.17.192 - TD 51320 - Fix for saving the receipt for unrecognized bill CR
//									11.	Merge from 4.0.17.210 - TD 49817 - Pumps were unlock on their own after locking up at End of Day 
//									12.	Merge from 4.0.17.220 - TD 51031 - WOR PIC - Multiple issues with Armor report when re-printed inside at POS
//									13.	Merge from 4.0.17.240 - TD 51966 - It is not possible to Re-Authorize PrePay transaction when MaxTrsNumber = 1.
//									14.	Merge from 4.0.17.330 - 
//										14.1	-	TD 56064 - When doing lock all pumps and then UnLock there was a chance that pumps remmained locked.
//										14.2	-	TD 52381 - Fully dispensed pic transactions can't be reprinted from the Pos. New log lines were added.
//										14.3	-	TD 55857 - Not possible to recall unbalanced prepay when the pump is locked (NMI).
//									15.	Merge from 4.0.17.340 - TD 56221 - can not perform parallel Prepay recalls when 1st created prepay  recalled before the 2nd prepay
//												  In GPI:QueryPrePaidTrs__ - allow also locked transactions data to be as a result.
//	4.0.18.505	Gena	11/12/07	1.	Merge from 4.0.14.1010
//										1.1.	Support events in OLA  type OLADotNet. 
//										1.2.	TD - 55418 Failed PAP time , merge fixed from version 1013.	 
//										1.3.	TD - 55419 Receipt not printed. merge fixed 1013. 							
//										1.4.	TD - 55420 Receipt can not print from opt.
//										1.5.	TD - 56224 Receipt can not print from POS.
//									2.	Merge from 4.0.140 (HUSKY)
//										2.1.	Add new parameter in GCIRetCode: GCI_TERMINAL_SRV_IN_USE.		
//										2.2		Add new parameter in PumpResExt: RES_EXT_PAP_EXTERNAL.
//	4.0.18.506	Gena	13/12/07	1.	Merge from 4.0.14.1020
//										1.1.	TD - 50519 The OLA Tag AuthorizationDate[D] was send corrupted to OLA.
//										1.2.	TD - 56738 PCI Compliant issue QDEX show Card number in the clear Fuel 1014. 
//				AmitH				2.	CR - 42563  Archiving log file at the end of day failed due to O.S. issue, after failing once the archiving did not tried again.         
//				                           reducing the use in CreateFile and CloseHandle for better performances. 
//	4.0.18.507	Gena	18/12/07	1.	Merge from 4.0.14.1021 - 17064 Unlimited receipts on OPT reprint.
//				Liat	20/12/07	2.	TD 57050 - After  PAP/AVI transactions we suppose to have a records in the receipt QDX in FCC.
//				AmitH	20/12/07	3.	TD 56250 - CR - If we got the ew flag to return only original in GPI::GetReceipt2 (RECEIPT_GET_ORIGINAL)- just for this time, we 
//													override the sReceiptPrinted flag with zero before asking the receipt 
//				AmitH   20/12/07    4.  No TD - Insert receipt string was not printed to log if PCI is off.
/*				Liat	26/12/07	5. TD 46512 Cont.- Support Zip Code prompting with MTXLinkSrv with 9 digits (FRN)
									6. TD 55466 - Add LoyaltyEntryMethod field and send it to OLA, cont. Parse the field on session authorize.
						27/12/07	7. TD 51066 - US WOR PIC - Issues with receipt layout when PIC (outdoor) receipt reprinted at POS - merge from 1017
	4.0.19.0	AmitH	27/12/07    1. No TD - send ClubcardScheme[I] also in the complete session.
				AmitH   27/12/07    2. No TD - create a new critical section to protect GetCardInfoEx (merge from 4.0.15.360)
				Liat	31/12/07	3. TD 56398 - Q-DX maintenance is stuck in an endless loop
				Liat	01/01/08	4. TD 57454 - RePrint by ATT doesn't work after ATT transaction
									5. TD 57050 Cont. -  After attendant transactions we suppose to have a records in the receipt QDX viewer in FCC.
									6. TD 55366 - Carwash Connection on Fuel Server is set incorrectly to Tcp/IP, instead of Serial - Merge from 1017
									7. 1019 Formal release
	4.0.19.500	Liat	07/01/08	1. TD 53190 - InProgress status for PAP transaction.
									2. TD 48586,48678 -  Cancel PAP trs the moment OLA approved the card but before it was notified to RFS (Merge from Murphy 1017) 
				Gena	17/01/08	3. TD - 57823 PAP discount Item weren't report to VP. Merge from 4.0.7390.1700.
									4. TD - 57824 The field sTotalAmt pass incorrect to VP. Merge from 4.0.7390.1700.
									5. TD - 57966 While EOD process with open forecourt do not pass VP new OLA record which ended in the new shift.
									6. TD - 58093 Force Accept Rolling update for PrePaid fleet.
				Ella	11/02/08	7. TD 58492 - Xml failure on system startup, when Hydra is active and after first card swipe. Merge from 4.0.17.420
									8. TD 58768,58798,58755 - Transaction was canceled after CheckAndReserve, it caused CL to remove PRP reservation. Merge from 4.0.17.430
									9. TD 58480 - In case PumpSrv in middle of process (SetMaintenance for example), CheckAndReserve for PrePay trs will fail. Merge from 4.0.17.450
									10.TD 58916 - PaidIn Amount incorrect for Unrecognized Bill scenario. Merge from 4.0.17.450
				Ella	13/02/08	11.TD 59171 - highlight lifted nozzle (GPN Customer). Merge from 4.0.141.10 Special version
				AmitH	13/02/08	12.TD 58094 - Set grade price with time directly to Cl2PumpSrv from CTRL.
				Liat	24/02/08	13.TD 55286	- Copient loyalty CR.
									14.No TD - Loyalty integration with storeLine. OLA RouteSrv was updated to call LoadAllProductSoldOnPump.
									15.TD 58863 - Fuel stuck after one pap trs with RouteSrv- Merge from 4.0.19.33.
				AmitH   24/02/08    16.TD 59273 - PumpSrv was sending too many requests for WetStockData in case both Memo and Regular interval are activated.
				Liat	25/02/08	17.TD 59557 - Press cancel on Prompt Swipe payment card leave stuck Transaction in PumpSrv
						25/02/08	18.TD 55286	- Copient loyalty CR- Continue.
				AmitH	25/02/08	19.TD 58094 - Change flag SEND_PRICE_DIRECT_TO_CONVER_LAYER value from 0x8000 to 0x0100
				AmitH   25/02/08    20 TD 59575 - While pumpsrv is waiting for wetstcock readings , it is impossible to do CheckAndReserve.
										          There is no need to check if we are in wetstock process or delivery process.


	4.0.19.501	AmitH	26/02/08	1. TD 58094 - Change logs and method intenal names.
									2. No TD    - Remove one too many SavePumpInfo call in SetPumpStatus Method.

	4.0.19.502	Gena	03/03/08	1. TD 55438, 52520 - Sometimes we got Duplicate receipt numbers when printing on 3 pumps simultaneously (MERGE FROM 1015:)
									   Change the Counter to be a member (m_lReceipt00Counter) of ServerMain only for Receipt\Receipt00.(Merge From 1015)
									2. TD 39254 - Optimisation of writing to PumpStat qdx. (Merge from 1015)
									3. Optimization of writing to INI/REG file. (Do not read parameters using HydService in LoadPrarm()).(Merge from 1015)
									4. Tesco LAB - PumpSrv crashed with Exception COM Result: 0xc0000005 - Extra logs were added
									5. TD 59546 - PumpSrv did not call SetServerRecoveryProcess(TRUE) when moving to SERVER_STARTUP or in .
								    6. TD 59273 - PumpSrv was sending too many requests for WetStockData in case both Memo and Regular interval are activated. (backward merge from 1017)
									7. TD 57732 - (CR) Add two new parameters for each Pump. (DisablePulsesMechanism & TransactionDeviation)
				AmitH	06/03/08	8. TD 58096 - (CR) Add new GCI interface method: SetPumpMisc to let Cl2Pumpsrv change the miscs by itself. 
				AmitH	06/03/08    9. TD 59037 - On offline transactions (ReadyOfflineTRS), Pumpsrv did not take the right field from the xml, thus VP did not get the Wight (volume)	 (merge from 1018)
				AmitH   06/03/08    10.TD 59036 - On offline transactions (ReadyOfflineTRS),take ControllerSeqNum from CardData. Need a change in Cl2PumpSrv too. (merge from 1018)
				AmitH   06/03/08    11.TD 58669 - On ReadyOfflineTRS - mark the transaction as offline to VP. (new field to XML - "FallBackTrs") (merge from 1018)
				Yaron	06/03/08	12.TD 58448 - PumpSrv was not canceling the previous trs from PAP requesting to PAK requesting (merge from 1018)
				AmitH   06/03/08    13.No TD      PumpTestPassword[I] - was not sent back to OLA if sent from converlayer (merge from 1019)
				AmitH   09/03/08    14.TD 59928 - add new field to Ola.h - PreAuthAmount[F] - merge from 4.0.18.181
				Ella	10/03/08	15.TD 58096 - P@P support with Postec
				Liat	11/03/08	16.TD 47235 - Forecast Sales Input (merge from 1019),
				Ella	12/03/08	17.TD 58649 - Reset pump CR
				AmitH   10/03/08    18.TD 59424 - Add new field to Ola.h - CustomerEftWaitTime[I] and TotalEftNetworkTime[I]. ]
				Liat	13/03/08	19.TD 55286	- Copient loyalty CR- Not finished.
				Liat	19/03/08	20. No TD - Loyalty integration with storeLine - add 2 parameters to the registry: VirtualPosID and LoyaltyUniqueClient.
				Liat	20/03/08	21.TD 55286	- Copient loyalty CR- Not finished. 
										21.1 Handle StartTicketWithAddItem, CancelTicketWithAddItem and AddFuelAndNonFuelItem.
										21.2 Add flow control functionality to the loaylty flow - Not finished.
										21.3 Add volume limit received from the loyalty server and applying it if needed.
										21.4 Add a flag to CancelTicket indicating cancel was called from the end of PAK flow.
				Gena	24/03/08	22.	Add more protection for strlen functions.
									23.	Add Exception handler in all GPI,GCI and Servermain classes. Add methods to logger.cpp
				YaronA  24/03/08    24. TD 60266 - PumpSrv didn't clear MISC_NOZZLE_LEFT_OUT_TIMEOUT misc when pump return to IDLE. 
				AmitH   24/03/08    25. TD 60638 - Add new field to mark a stolen card by OLA : HotCard[I] 
				Ella	25/03/08	26. CR 57863 - Remove track2 and account data from log according to new registry parameter - RemoveCardDataFromLog (Merge from 1015)

   4.0.19.503	AmitH	25/03/08	1. CR 59898 - Add 2 new fields to Ola.h and xml convertor : SponsorNum, PanSeqNum (merge from 1015)
				Ella	26/03/08	2. CR 57863 - Remove track2 and account data from log according to new registry parameter - RemoveCardDataFromLog, another fix
				Liat	26/03/08	3. TD 55286	- Copient loyalty CR- Not finished.
										3.1 Add DepCode parameter to the registry under FuelLoyalty\SysParam.
										3.2	Changing PAP flow to swipe payment first
										3.3 Update FlowControl's Prompt NMP.

   4.0.19.504	Ella	30/03/08	1. CR 57477 (original CR 47559) - Husky EMV (Merge from special 1014 version)
									2. TD 59460 - In GCI::GetPumpInfo - adda new flag (IGNORE_SERVER_IN_MIDDLE_RECOVERY)from conver lyaer (Allied) to ignore server in the middle of recovery.(Merge from 1017)
									3. TD 59905 - Incorrect receipt information on unrecognized bill (Merge from 1017)
									4. TD 59547 - Check&reserve cancelation during Lock all didn't work.(Merge from 1017)
									5. TD 60450 - CarWashRestriction was not passed to RFS correctly and therefore RFS didn't show the C/W promt on the Crind (Merge from 1017)
									6. TD 60613 - PumpSrv crashed as a result of trying to move PrePay with incorrect pump number. Protection was added.(Merge from 1017)
									7. TD 59574 & 60460 - Trying to release pumps with Lock/Unlock after StopAll(with Lock) caused PumpSrv to try and release pump forever. Protection was added. Emergency lock cannot be released by Maintenance Lock/Unlock (Merge from 1017)
				AmitH   31/03/08    8. TD 59928 - continue - get the PreAuthAmount from RFS and send to OLA in BuildRequest method
				AmitH   31/03/08    9. TD 59171 - Take grade and the position from pump stat copy to the TAG_MINIMAL_PUMP_5 to return to PosPump
												  Also set TAG_MINIMAL_PUMP_5 filer from 200 to 50 to reduce buffer size over COM
				Liat	01/04/08	10.TD 55286	- Copient loyalty CR- Beta version.
	4.0.19.505	Liat	02/04/08	1. TD 55286	- Copient loyalty CR- 
										1.1 Add fields LoyaltyEngineType and UsingComInterface,
										1.2 Add interface function GPI:GetPAPTrs6.
											A new flag GET_PAP_AS_STRUCT (= 4) indicates a struct TAG_OLA_REC5 will be returned. Otherwise, an xml will be returned.
											A new ret code GPI_NO_LOYALTY_SUMMARY indicates no loyalty xml found.
										1.3 Add Functionality to remove loyalty xmls as part of RemovePAPTrs and setShift process.
									2. No TD - Fix CParam::GetParam3Level in case of table name has 's' in its name, like SysParam.
				AmitH   02/04/08    3. TD 59928 - continue 
										3.1 Get the PreAuthAmount from RFS and send to OLA in BuildRequest method
										3.2 Do not parse PreAuthAmount[F] from OLA.
	4.0.19.506	Liat	06/04/08	1. TD 55286	- Copient loyalty CR.
										1.1 PAK flow changes.
										1.2 PrePay flow changes.
										1.3 PAP flow changes.
										1.4 Change the default LoyaltyDataPath to be the same as Paths\Long\HomeDir
									2. TD 59557 Cont.- Press cancel on Prompt Swipe payment card leave stuck Transaction in PumpSrv (Merge from 1019).
				Ella	10/04/08	3. CR 57477 (original CR 47559) - Husky EMV (Merge from special 1014 version) Cont.
	4.0.19.507	Liat	13/04/08	1. TD 55286	- Copient loyalty CR - Cont. - Fix a bug of calling CheckIfLoyaltyFuel with an xml parameter instead of struct.
				Gena	13/04/08	2.	TD 61311 - Cease using strtok() in XMLInterface; 
						13/04/08	3.	TD 61048,61107,47892 Change the GetAlarmByCategory method to be more efficiant.
										Added new flag : GABC_WITHOUT_SOFTWARE_GENERAL_OLA_ERR - alarmSrv will screen the OLa alerts for PumpSrv.(Merge from 1017)
	4.0.19.508	Liat	14/04/08	1.	TD 55286	- Copient loyalty CR - Cont. - Add comments.
									2.	TD 59473 - PAP transaction is not created when swipe payment card only on LPE system (Merge from 1019).	
				AmitH	14/04/08	3.	TD 56040 - ICCardPrepay.	(Gena)
									4.	TD 49115 - Change Fuel Product In Tank (Gena)
									5.	TD 56040 - ICCardPrepay - add a new interface method to CTRL: GetStationStatus 
				Liat	16/04/08	6.	TD 55286	- Copient loyalty CR - Cont. - Fix delete command of LoyaltySummary xmls.
				Gena	17/04/08	7.	TD 56001 - Choose grade on PAK scenarios. (Merge from 1019).	
				Liat	23/04/08	8.  TD 60508 - Inprogress status for PAVI transaction, merge from 1015.
									9.  TD 55286	- Copient loyalty CR - Cont. - Fix AddCarWash before fueling (was added twice to loyalty ticket).
				Ella	23/04/08	10. TD 54600 - 0 Trs was not written in TRA (Merge from Husky EMV 4.0.140.50)
									11. No TD - Transaction stayed in IN_PROGRESS status in OLA qdex (special PAP) (Merge from Husky EMV 4.0.140.50)
									12. TD 61696 - in case there was fueling, send SetTrsState(PAID) to CL and do not cancel (Merge from Husky EMV 4.0.140.50)
				Gena	27/04/08	13. TD 59878 /58840   Receipt showed always original receipt sign. (Merge from 4.0.16)
									14. TD 60848 - Corrupted OLA Index generate. (Merge from 4.0.16)
									15. TD 61740 - Cancel PAP while trs while OLA approved the card. (Merge from 4.0.16)
				AmitH   28/04/08    16. TD 62048 - Send OLA 3 new fields: IssActOnLinel[S] IssActDefault[S] IssActDenial[S] (merge from 4.0.18.290)
				Gena	06/05/08	17.	TD 61994 - POS 48 cannot reprint receipts, but POS45 can.(Merge from 1016)
				Liat	07/05/08	18.	TD 55286 - Copient loyalty CR - Cont. - integration in the US.
	4.0.19.509	Gena	11/05/08	1.	TD 62335 - Low performance in Prepay transactions. (Merge from 1017).
									2.  TD 62233 - Credit Card # displays in the in buffer in VP logs. (Merge from 1017).    
									3.  TD 58492 - Every PAP sale is reported in the Exception.log file and never goes into QDEX. (Merge from 1017).   
				Liat				4.	TD 55286 - Copient loyalty CR - Cont. Fix NMP validEntry property when Order > 0.
	4.0.20.0	Liat	13/05/08	1.	TD 55286 - Copient loyalty CR - Cont. Fix TotalAmt calculation when a total ticket discount is given.
				Ella	14/05/08	2. Merge from Husky EMV 1014:
										a. TD 61778 - Shift log wrote the same trsaction twice.
										b. TD 61995 - PAP transaction was stuck as a result of CL sending READY/NULL_RES and running over internal PumpSrv reservation for PAP, a moment before reserving the pump for PAP.
										c. Enlarge the queue for GPI events
										d. New general event is defined - GCI_EVENT_FORCE_RELEASE_TRS, will be raised by TerminalSrv with SetProcess(PROCESS_FORCE_RELEASE_TRS), when transaction or reservation are stuck in the Allied box 
										e. TD 62219 - It was possible to make more PAK transactions then the Max Post Pay was set to (when the pump is auto-authorized).
										f. TD 59750/60335 - Break scenarios with pulling channel 1 when pump authorized for PAP and plug it back when finished dispensing. The pump got stuck in reservation or PumpSrv tried to cancel transaction internally, instead of closing it.
				Liat	14/05/08	3. 1020 Formal release.
	4.0.20.10	Liat	15/05/08	1. TD 62663 - In FCC\FlowControl the flow checkboxes are grayed out. Default setup was given to the FlowControl registry entries.
						19/05/08	2. TD 55286 - Copient loyalty CR 
										2.1 Fix the time to call NMP carwash.
						20/05/08		2.2 Add default message to NMP in case of an empty message.
										2.3 Message can be a message name from language01 in te registry.
									3. TD 62944 - Can't make pak trs with loyalty number. DiscountType wasn't passed to RFS in PAK.
	4.0.20.11	Liat	22/05/08	1. TD 63123 Transaction ended with negative discount.
	4.0.20.20	AmitH	22/05/08	1. TD 62895 - Get the IssuerScriptData[S] from ola and save in a new array: m_cExtraChipInfo . 
												   This array will be sent to convet layer(RFS) in GCI::GetPapSwipeData method with new flag: GPSD_GET_EXTRA_CHIP_INFO
									2. TD 62753 - 1. In case Conver layer(RFS) sends a new flag: SWIPE_INCLUDE_EXTRA_CHIP_INFO in PapSwipe method, the data saved in m_cCvmListInfo and will be sent to OLA in new: CVMList[S] in both AUTHORIZE and COMPLETE session
												  2. Send Ola a new TransactionStatusMode[I] field
				Liat	26/05/08	3. No TD - Loyalty integration with storeLine - creating the new transaction before startTicket for Copient caused double transaction in PAK with Allied because of the reservation difference.
	4.0.20.40	Liat	27/05/08	1. TD 62663 - Cont. change the default value of the "Repeat" property to 1.
				Liat	28/05/08	2. TD 59680 - Can not recall undispensed prepay when the pumps are stopped. Merge from 1017.
				Liat	29/05/08	3. TD 62796 - Tank reading Exception - merge from 1019.
				Liat	01/06/08	4. TD 63430 - PAP completions are not sent with Copient.
				Liat	03/06/08	5. TD 63627 - Promotion number is missing in the fuel record on PAK recall.
									6. TD 63629 - The registry key GetTargetDiscounts is not created in registry.
				AmitH  	03/06/08	7. TD 58096 - P@P support with Postec continue: Zones were not copied before saving to receipt extra info qdx
	4.0.20.50	Liat	05/06/08	1. TD 63781 - Fuel needs to pass Scan(barcode) and manual entry of loyalty card to RouteSrv for validation.
									2. TD 63628 - LoyaltyFuel is not sending promotion force at the end of ticket.
	4.0.20.51	Liat	11/06/08	1. TD 63930 - Copient PAP with Manual Loyalty Entry does not send correct data from PumpSrv.
									2. TD 63931 - Copient PAP with Barcode entry does not send the correct data from PumpSrv.
									3. TD 63932 - Copient PAP with Swipe Entry does not send the correct data from PumpSrv.
									4. TD 63626 - LPE session remained open when payment got declined and next transaction failed to get discount.
	4.0.20.52	AmitH	12/06/08	1. TD 58220 - Merge from 1019 - IC card outdoor payment : Add new empty GPI interface methods: FinalizePrePayTrs and AuthorizeAndLimit3
				Liat	15/06/08	2. TD 63936 - Copient , can not start PAK + loyalty.
				Gena	16/06/08	3. TD 62818, 63602, 63661 - Fuel Delivery Emulation.(Merge from 1019)
	4.0.20.53	Liat	18/06/08	1. No TD - Initialize track2 before searching in the qdex in GetReceipt2 and BuildRequestReceiptInfo.
				Liat	19/06/08	2. TD 64295 - Flow problems with Loyalty (Copient and Legacy) - Legacy PAP with Manual.
	4.0.20.54	AmitH	19/06/08	1. TD 46943 - Support Offline transactions for convert layer IFSF. Add new prm in ini file :OfflineTrsInShift (merge from 1019)
				AmitH	19/06/08	2. No TD  - REC_READ_NEXT operation from OLA qdx was done with wrong indexNumber (merge from 1019)
				Ella/Yaron 23/06/08	3. TD 58220 - IC card outdoor payment support. (AmitH - Merged from 1019)
				Liat	24/06/08	4. TD 64528 - CR - Add Support for Fuel Deferral Discount at PAP_PAK_PREPAY.
				Liat	25/06/08	5. TD 64348 - Loyalty - Fuel - Loyalty Offiline message at pump is not displayed.
						26/06/08	6. TD 64635 - Loyalty Fuel - PAP Ticket Total is incorrect on Receipt (change in AddItem xml).
									7. TD 64636 - Loyalty Fuel - No entry on PAP receipt for tickit total promotions (change in Force xml).
	4.0.20.55	Liat	29/06/08	1. TD 64735 - The reward amount that prints on the receipt and the amount sent in the AddItem XML are not the same.
	4.0.20.56	AmitH	29/06/08    1. No TD - The Enter critical section in SetPumpStatus and ModeSchemeProcess methods is not on the same scope as the Leave critical section.
									2. No TD - Protect ClearPAPInfo() and SavePumpInfo() methods from getting illegal PumpNumber
									3. No TD - Pumpsrv will call new HydService methods: IRT->RelWrite2 IRT->Insert2 IRT->Update2 IRT->Delete2 
									4. No TD - in OLAProgressEx do not call any more the Init and the Finilize methods. ()
				Gena	29/06/08	5. No TD - Remove critical section from CGAIEventsHandler::UpdateAlarmMap.
									6. No TD - Cancelling save of data in SwipeRequest.
									7. No TD - Cancelling write "PAPSwipe XML message" to log file.
									8. No TD - Delete m_cPapInfo, from BaseReceipt class.
				Liat	01/07/08	9. TD 64734 - Need to add logic to determine correct auth limit - Not finished.
						02/07/08	10.TD 64903 - AddItem XML is wrong for Amount, Quantity, and RewardAmount fields on a PAK.
	4.0.20.57	Marik	02/07/08	1. TD 64904 - Negative discount value sent on PAP. 



**************** 1021 Beta Start ********

	4.0.20.500	Liat	06/07/08	1. TD 64734 - Need to add logic to determine correct auth limit - Not finished.
									2. TD 60236 - PCI DSS For Tesco- not finished.
						07/07/08	3. TD 64904 cont.- Negative discount value sent on PAP. 
				Shuki   13/07/08	4. TD 64417 - Carwash Cents off from ALL fuel grades. merge from 1014.1330
				Shuki	14/07/08    5. TD 65030 - In CGPI::GetPrePaidTrs2 method - return trs is locked also if it is In ACCEPTED status (merge from 1017.690)
				Shuki	14/07/08    6. TD 65165 - In PapreadyTransaction check also that OLA_BY_PUMPSRV before returning "in the middle of recovery" (merge fron 1017.690)
				Shuki	14/07/08	7. TD 60205 - delete new prm "QdxDir" instead, under Paths\\Long set  2 new parameters for the location of qdx engine : "QdxEngineDir" and "QdxFilesDir" (merge from 1018.323)
				Shuki	14/07/08	8. TD 65299 - Delivery start and end time in TRA file. (merge 1019.270)
				Shelly  15/07/08	9. no TD    - Add define: GCI_EVENT_DISABLE_REPRINT to Pump.h
				Shelly  20/07/08   10. TD 27377 - Added a new OLA object, COLASimLinkSrv, which enables to work directly with SimLink, as the OLA server, and generate the source for PumpSrv to activate it.
				Yaron	14/07/08   11. TD 65383 - Send first rolling update to second machine, for better recovery (merge from 1020)
				AmitH   15/07/08   12. TD 65383 - Continue - GetCurrentTransactionNumber :  Special treatment for Postec CL: on switches in the middle of PAK dispensing it starts with Ready Null res.
												  If there were PAK dispensing transaction during the switch - it needs the IN_PROGRESS PAK trs number even in NULL_RES...  
				Gena	24/07/08   13. TD 63936 - (Fix) Copient , can not start PAK + loyalty.
								   14. No TD	- (Fix) Initialize track2 before searching in the qdex in GetReceipt2 and BuildRequestReceiptInfo.
				Ella	28/07/08   15. TD 65255 - after locking the pump and restaring the system, CL sends Ready/Null_res but PumpSrv still holds the SINGLE_LOCK flag and therefore the SetShift fails. (merge from 1019)
								   16. TD 64928 - - PumpSrv was not creating default TankMinVolumeLimit registry key under each tank. (merge from 1019)
								   17. TD 58220 - Fuel outdoor. Add sOverfillAdjusted field to XML convertor, to report it to VP, in case of waste transaction (overfueling). (merge from 1019)
								   18. TD 58220 - Fuel outdoor. Add new flag PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY used to get only paid PrePay transactions. PREPAY_GET_BY_PURCHASE_ID flag is used for others (WAIT_TIME_OUT or W_PROGRESS). (merge from 1019)
								   19. TD 65887 - PumpSrv could not return current PrePay transaction after Ready/Null_Res status was sent (merge from 1019)
                Shelly  28/07/08   20. TD 64967 - CR: Support a "caption" field to each pump, which is configured in the registry, and is passed to PosPump (to be presented on the pump, instead of PumpNumber) (merge from 1019). 
    4.0.20.501  Shelly  30/07/08    1. TD 66046 - Changed the default of "Caption" field in the registry from 12 spaces to empty string (merge from 1019).   
				                    2. TD 66092 - Caption was deleted when its lenght was 12 characters => increase the inner caption's field's length from 12 to 13 (merge from 1019).
									3. TD 62354 - Expand AVI Special Code to 25 Chars
						06/08/08	4. TD 63114 - Exapnd information of the Delivery record: add an import of 2 new fields from RFS XML to the delivery record, and add an export of 3 new fields from the delivery record to the XML to VP.
						                          ENLARGE BOTH records' size of Delivery.qdx and EODDeliv.qdx QDX FILES from 100 to 200. 
				AmitH   17/08/08    5.  TD 66692- Block stop single pump when server in EMERGENCY_LOCK 
				AmitH				6.  No TD    - In case of Catch(...) connection with AlarmSrv was not closed correctly.
				Liat				7.	TD 65330 - Inprogress status for PAVI transaction (merge from 1015).
				Gena				8.	TD 60236 - PCI DSS For Tesco/Sainsbury.
									9.	TD 62335 - When CL crashes, Pumpsrv will not be in enless loop of cpm ecxeptions.
									10.	TD 52440 - Sainsbury - CR  - Fuel (OPT re-print)  PCI compliance.
									11. TD 65128 - PCI Strictness - If ON (RemoveProhibitedData prm) Pumpsrv will remove the track2,track1, accountNum etc.. from qdx.
									12. TD 60203 - Reprint Receipt (Add a lookup method by accountnumber along with current method by Track2).
									13. TD 63194 - ADSBG PCI Issue, prevent storing prohibited data post authorization. (Merge from old versions)
				Ella	19/08/08	14. TD 65908 - Post payment outdoor CR (merge from 1019).
	            Shelly  19/08/08    15. TD 65174 - CR: Support connecting different tanks in the same delivery to different document reference numbers (merge from 1019).
				Ella	19/08/08	16. TD 66068 - Protection was added, in case new PRP is started while another one is fueling, the new PRP will become WAIT_TIME_OUT automatically (merge from 1014)
	4.0.20.502	Liat	21/08/08	1.	TD 65282 - Failed to change reservation to Pay Attendant - Authorize session was not initialized properly (merge from 1015).
				Gena	25/08/08	2.	no TD    - Log messages with masking although PCI is off.
	4.0.20.503	Liat&Gena 26/08/08	1. 	TD 60236 Cont.- PCI DSS For Tesco/Sainsbury. Change the EncryptStruct methods to EncryptStructForTrs when encryption is needed through the whole trs 
													and EncryptStructForDB when encryption is needed only for saving to DB.
				Liat				2.  TD 66960 - CR - Remove inside use of XML in PumpSrv.
						28/08/08	3. Merge from loyalty 1020
										3.1 TD 64734 - Need to add logic to determine correct auth limit.
										3.2 TD 64904 cont.- Negative discount value sent on PAP. 
										3.3 TD 65210 - Deferral prompt causes stuck transaction.
										3.4 TD 66143 - Bad Data and Missing data passed to ExtTrs32 from Fuel.
										3.5 TD 66871 - Loyalty Database in FCC clears values too soon causing double discount on recalled Prepays.
									4. Merge from 1015
										4.1 Memory leak in PumpSrv.	(Merge from 4.0.15.282).
										4.2	TD 67102,67126 - Some reads from QDX were done through Hydervice instead of local.

	4.0.20.504	Ella	28/08/08	1. Merge from Special 1014
										1.1 No TD - When Authorizing a pump from POS without pressing payInside first, a wrong flag is sent to TerminalSrv on SetPreSetLimit
										1.2 TD 63316 - Carwash totals are calculating in to the Wetstock totals and are not showing on the BOS EJ
										1.3 TD 65870 - After channel 5 break scenario, trs got stuck in W_PROGRESS
										1.4 TD 66976 - During channel 5 break scenario pump stays authorized after cancel is pressed and the transaction is removed.

									2.	TD 65134 - Wayne CL - Site Mode. Send END_OF_LIST flag on the last pump to CL in SET_PUMP_MODE2 and SET_PUMP_MODE3 event (Merge from 1018)
				AmitH   02/09/08 	3.  TD 67374 - PUMP_FILE was using old xml buffer (PAP_INFO_XML_BUFF) instead of PAY_AT_PUMP_INFO
	4.0.20.505	Liat	04/09/08	1.  TD 66221- Added keyboard support under Allied (merge from 1016).
						07/09/08	2.  No TD - AlarmClass.h was updated with alarms from Postec CL.
				Gena	07/09/08	3.	No TD - less messages will be writen to PumpSrv log. 
	4.0.20.506	Gena	08/09/08	1.	No TD - PCI/DSS. Even that DSS is disconnected, PumpSrv reprint receipt due to card swipe at the terminal (but the a wrong receipt).
									2.	No TD - PCI/DSS. Do not attempt to re-init DSS when encryption success but hashing failed.
									3.	No TD - PCI/DSS. Alarm on DSS failure should be raised only if the interval is not activated (there is no point of raising alarm every re-init attempt)
									4.	No TD - Failure in qdx update in ReceiptPrinted2, wrong receipt index was written.
				Liat	09/09/08	5.	TD 66960 - CR Cont.- Remove inside use of XML in PumpSrv.
				
	4.0.20.507	Ella	09/09/08	1.  TD 67547 - Recall can be done more than once from the same POS
				Gena	11/09/08	2.	No TD - PCI/DSS. ExtTrs fails to process the XML it gets from PumpSrv.									
									3.	TD 67287, 67732 - Fixed. can't copy Track2 in GetCardInfoEX().
				Liat	14/09/08	4.  TD 67232 - TotalDiscount is inconsistent in format (merge from 1020).
									5.  No TD - Fields from CardSaleExtraData are not sent to Extrans (merge from 1020).
									6.  TD 67763 - AddItem XML has wrong data for Amount of Fuel (merge from 1020).
									7.  TD 67761 - Wrong discount amount is prompted at the pump.						
	4.0.20.508	Gena	18/09/08	1.	TD 67608 - Can't make Re-print with any card.

  **************** Formal 1021 release  ********

				Ella	21/09/08	1.  TD 67881 - CustomerMessage field was not sent back to CL on reprint.
						22/09/08	2.	No TD - Remove track2 from log when writing/deleting/updating qdex (when log level higher or equal to 4)
				AmitH   25/09/08    3.  No TD - On start - Kill DataExportSrv and DataExportAgent if they are already running
	4.0.21.0	Liat	28/09/08	4. TD 68079 - Can't reprint items in PATT trs.
									5. Merge from loyalty 1020
										5.1 TD 67388 - PAP is missing member card from receipt.
										5.2 TD 68008 - PumpSrv is limiting the PAP to the gallon limit from Copient even if discount is not taken.
										5.3 TD 67854 - Different amounts printed on receipt and saved to XML.
										5.4 TD 67856 - Reprint at POS is showing wrong total savings amount.
										5.5 TD 68105 - AddItem XML does not have fields updated on a PAK.
									6. TD - 67387 CR - FLS Customer Record Locking (merge from 1014).
	4.0.21.10	Liat	29/09/08	1. TD 64119 - CR - Debit encouragement.
				AmitH	29/09/08    2. TD 68335 - Postec (OLA_BY_CONTROLLER) Pumpsrv did not save the trs.m_lNumber in PapReadyTransaction, so transaction->Number was empty in xml for VP.
				Liat	05/10/08	3. TD 68240 - reprint b@p trs by att doesn't work well.
	4.0.21.11	Amith	12/10/08	1. TD 68620 - Merge Original Storeline Ola event issue TD 57746 - offline mode, but still showing P@P on the OPT - Cont. Xml was parsed incorrectly.
	4.0.21.12   Amith	12/10/08	1. TD 66982 - PC_ExceptionHandlingForPumpPowerOff
				Liat	13/10/08	2. TD 68429 - PAK discounts are not written to XML file (merge from 1020).
									3. No TD - Extra logs were added for loyalty issues (merge from 1020).
									4. TD 68431 - PumpSrv fills sDiscountType[1] without initializing sDiscountType[0] to a space (merge from 1020).
				Gena	15/10/08	5. No TD -  PCI/DSS. Create new instance of COM object in case it fails to initialize.			
	4.0.21.13	Liat	19/10/08	1. TD 68756 - Pap avalible even DisablePapOnDssFail=1.
									2. TD 68660 - Loyalty card was not sent to LPE in PAP (merge from 1020).
	4.0.21.14	Liat	20/10/08	1. TD 68250 - SetTrsState blocked PAK finalization when server was in startup state (merge from 1015).
						26/10/08	2. TD 69073/68976 - Loyalty Card was declined when swiping during transaction.
	4.0.21.15	Liat	26/10/08	1. TD 69073/68976 Cont.- Loyalty Card was declined when swiping during transaction.
	4.0.21.20	Liat	27/10/08	1. TD 68756 Cont. - Add more log messages.
						28/10/08	2. TD 68079 Cont. - Reprint according to car id didn't work - logs were added and ReceiptExtraIndex qdx record size was changed to 256.									
				Gena	29/10/08	3. TD 65903 - New Tags added due to PCI/DSS.
									4. TD 69204 - SVC card stay look after PaP transaction cancel. (Merge from 4.0.17.840)
	**************** 1022 Beta Start ********
	4.0.21.500	Gena	03/11/08	1. TD 69287 - Reconfigure Postec from the FCC. (Merge from 4.0.20.430)
				Liat    06/11/08    2. TD 68979 - Copient - In AddItem of carwash we need to take the CCH_Product_Code from the registry according to the given program id given from RFS and send it in "PluCode" attribute (merge from 1020).
									3. TD 68855 - Copient - Only if user chose manual loyalty swipe - we need to disable the Scanner although it is in flow control !(merge from 1020).
									4. TD 67763 - Copient - Do not add the carwash price (in case there was car wash) to the lNonDiscountTotalAmount and in AddItem xml (merge from 1020).
						11/11/08	5. TD 69203 - Limit fueling to 8(eight) concurrent nozzles at one time - not finished (merge from 1014).
						12/11/08	6. TD 69415 - Unable recall PIC pre-paid transaction (merge from 1017).
									7. TD 69053 - Stuck PrePay on Pump#3- unbalanced pre pay counter was 1 even though pre pay qdx was empty (merge from 1017).
				Gena	12/11/08	8. TD 69389 - PCI\DSS Tesco - pci compliant -1 and DSSActive - 0, no reprint. Add GetCardInfo to OLAProgressSrvEx. (merge from 1021)
				Ella	13/11/08	9. No TD    - Endless loop when sending alarm to AlarmSrv fails
				Gena	19/11/08	10. TD 69722 - PCI/DSS.Tesco - dss try to get init when pci off. (Merge from 1021).
									11. TD 69654 - PCI/DSS.Tesco - Remove the key "DSSDeclineMSG" from GeneralParam.(Merge from 1021).
									12. TD 69692 - PCI/DSS.Tesco - no reprint after fueling with clubcard+visa.(Merge from 1021).
									13. TD 69683 - Pumps allowing pre-authorisation.(Merge from 1020).
				Liat	19/11/08	14. TD 69203 - cont. - Limit fueling to 8(eight) concurrent nozzles at one time (merge from 1014).
				Shelly  20/11/08	15. TD 66935, 69374 - New mechasism, Delivery Emulation per tank (Merge from 1019).
				Liat	25/11/08	16. TD 69203 - cont. - Limit fueling to 8(eight) concurrent nozzles at one time (merge from 1014).
									17. TD 69525 - 2 AVI Trs stucked 'In Progress'. The system crashed in PapSwipe and data wasn't saved to qdx (merge from 1015).
				Gena	25/11/08	18.	TD 70106 - PCI/DSS.PAP transactions doesn't have the ticket's image on TT/ EJ. (Merge from 1021).
						30/11/08	19.	TD 69684 - Delivery cannot finish in case locking four court before end delivery.(Merge from 1019).
									20.	TD 69430 - Delivery cannot be used in case of power failure during delivery.(Merge from 1019).
                Shelly  30/11/08    21. TD 66935 - Added two new classes, in a new file (DeliveryEmulationSingleTankServerInfo), for the Delivery Emulation per tank (to be used instead of ServerInfo arraies).
				Liat    30/12/08    22. TD 69955 - OLA_BY_CONTROLLER - PAP receipt not available if transaction total is zero becasue the discount amount equals  the total amount (merge from 1020). 
						03/12/08	23.	Formal release - beta version
				Ella	04/12/08	24. TD 70312 - EOS stuck on 'In middle of process' error after SetMaintenance. (Merge from 1019)
									25. TD 14778 - PIC CarWash functionality CR (Merge from 1017)
									26. TD 59314,58746 - when recalling unbalanced Cash Acceptor transaction (no rolling update,small amount of fuel trs), Gray 'P' stays on the pump. (Merge from 1017)

	4.0.21.501	Ella	09/12/08	1.	TD 70420 - GetReceipt from VP failed because record with index number 0 was created in OLA qdex (Merge from 1017)
	            Shelly  10/12/08    2.  TD 67443 - PAPSwipe: in a case of a payment card: check if PAP mode is valid at first place (not finished - not checked yet).
				Liat	11/12/08	3.	TD 68857 - Ability to Scan Member Card at the Pump with LPE (merge from 1020).
									4.	TD 66977 - Loyalty Limit in rtaps (merge from 1020).
									5.	TD 65268 (CR) - Pay at pump phase 2 (merge from 1020).
				Ella	14/12/08	6.	TD 70634 - Cannot Line Void car wash item from POS, because of wrong car wash code (Merge from 1017)
				Liat	15/12/08	7.	TD 70752 - problem with the last pumpsrv(21.500).after we swipe any card we don't get authorize

    4.0.21.502  Shelly  16/12/08    1.  TD 67443 - PAPSwipe: in a case of a payment card: check if PAP mode is valid at first place.
				Liat	24/12/08	2.	TD 70932 - Loyalty number is missing when sending Start ticket to LPE (merge from 1020).
				Ella	24/12/08	3.  TD 71040 - GetStationStatus always returns 0 (merge from 1019).			
				Liat	24/12/08	4.	TD 68689 - Controller Sequence Number in PAK (merge from 1018).
	4.0.22.0	Liat	01/01/09	1.	TD 66977 Cont. - Loyalty Limit in rtaps (merge from 1020). 
										1.1 Cancel after swiping loyalty was not ended correctly with the complete session.
										1.2 Prepay new buffer wasn't parsed correctly.
									2.	TD 68304 - Cont. CR - Gas Member Validation (merge from 1014).
									3.	TD 69922 - CR - Discount Stacking for Carwash Cents Off (merge from 1014).
									4.	TD 70776 - CR - Adding Grade Restrictions to fuel (FRN) (merge from 1014).
				AmitH   01/01/09	5.  TD 70911 - In case of card blocked error from Ola - send also the zones to CL. (err 251, CARDSRV_Decline_Card_Blocked) (merge from 1014).
 
				Gena	04/01/09	6.	**************** Formal 1022 release  ********
				Gena	05/01/09	7.	No TD	 - Wrong log message in UpdateReceiptIndex function.

**********************************************************1023 Beta Start**********************************************************

	4.0.22.500	Ella	05/01/09	1.  TD 70815/68055 - Protection was added for grade price 0 in Ctrl::SetGradePrice (Merge from 1012)
				AmitH   13/01/09	2.  TD 70459 - Copy ReceiptType[I] also in complete session.
				Gena	15/01/09	3.	TD 69173 - POSPump ‘All transaction’ Screen and GUI  Enhancement (SL 57737). Add new GetPumpsTrsInfo interface to GPI.
				Ella	18/01/09	4.  TD 70351 - PAP transaction was stuck. Trs was not cancelled after authorize timeout and moving to REQUESTING/PAK. (Merge from special 1014)
									5.  Support new general event GCI_EVENT_FORCE_RELEASE_TRS, will be raised by TerminalSrv with SetProcess(PROCESS_FORCE_RELEASE_TRS), when transaction or reservation are stuck in the Allied box (Merge from special 1014)
				Liat	19/01/09	6.	TD 70766 - CR - Enhanced Messaging And Printing Functions At The Pump - create a new message under languages - not finished.
				Ella	19/01/09	7.  TD 69907 - Htec Convert Layer, support new flag INIT_CONTROLLER_SUPPORT_STOP_ALL_PUMPS when controller supports stop all pumps
				Gena	20/01/09	8.	TD 72216 - PumpSrv do not pass loyalty details to OLA on complete session and exttrans.(Merge from 1020).
				Liat	22/01/09	9.	TD 70767 - Voucher Support at the Pump - start new flow control - not finished.
				Liat	01/02/09	10. TD 72486 - LPE_Fuel - Limit reward to minimum new price still sends full reward amount to fuel instead if adjusted one (merge from 1020).	
									11. TD 72485 - Fuel causes POS to give repeated reward on Prepay (merge from 1020).
									12. TD 72412 - LPE_Fuel - Is adding value of last known reward to PPG on all non member PAK (merge from 1020).
									13. TD 72527 - Fuel gets no connection to Loyalty Server after previous transaction has timed out or canceled (merge from 1020).
									14. TD 72559 - Fuel displays track 2 data in ClubcardTrack2 field in PumpSrv.log (merge from 1020).
				Ella	01/02/09	15.	TD 70353 - Support Outdoor PrePay Receipts for Allied (Merge from 1016)
				AmitH	01/02/09	16.	TD 69173 - Continue - add a third argument to new interface method CGPI::GetPumpsTrsInfo
									
	4.0.22.501	Gena	03/02/09	1.	TD 70956 - Limit dispensing pumps per till.
									2.	TD 69203 - Fixed and new implementation.
				Liat	04/02/09	3.	TD 64120 - Support of Retalix Loyalty “Unique ID” coupons by Fuel - not finished.
						09/02/09	4.	TD 72731 - Force reward logic doesnt work in LPE (merge from 1020). 
									5.	TD 72673 - PaP doesnt update LPE MA when using Limit reward to minimum new price: 0.10 - grade price was wrong (merge from 1020).
									6.	TD 69344 - CR - Option of delaying acceptance of RL Promotion at the Pump - not finished.
				Ella	11/02/09	7.	TD 69905 - Wet stock report printing CR
						15/02/09	8.	TD 69904 - Support Pump Test with Htec CR
				AmitH	15/02/09	9.	TD 72894 - Add new Ola type 14 : OLASRV_POSITIVE_US : it is similar to OLASRV_POSITIVE.
				Ella	15/02/09	10. TD 70353 - Support Outdoor PrePay Receipts (with RFS) - Fixes
				Ella	18/02/09	11.	TD 71055 - Attendant tagging interfacing to DOMS. (merged from 4.0.22.10. 4.0.22.11)
				AmitH	18/02/09	12. TD 73062 - Create a new parameter : MaxAttendantTrs under each pump (default = 1) to allow more than one stacking Attendant trs.
				Liat	23/02/09	13. No TD - FCC calls QueryExtraInfo with PumpNumber = 990 (PosNumber) which causes a crash when nozzle restriction is supported. 
				Gena	01/03/09	14.	TD 72716 - add a 6th record to Recipt qdx (RECEIPT_REC6) to include Extra_data5.(Merge from 4.0.22.13)
				Gena	01/03/09	15.	TD 72716 - PCI AsymetricEncryption, Reorginazing current Pumpsrv's PCI class structure to fit the new requirment. (Merge from 4.0.22.13)
				Liat	02/03/09	16. TD 71055 - Cont. Attendant tagging interfacing to DOMS. 
										Adding string fields to CPumpTransact (m_sAttendantID and m_sAttendantName) influenced memset trs to 0 in ResetPSPumpInfo, and caused incomplete buffer to be sent to PosPump in GetPumpInfo3.
	4.0.22.502	Liat	05/03/08	1.	TD 73612 - Wayne CL - Wrong tax calculated when tax amount is higher than 1000.00 (merge from 1018).
						08/03/09	2.	TD 71801 - Fuel price change - fix GetPumpGradeInfo method.
	4.0.22.503	Ella	11/03/09	1.  TD 72189 - IFSF Specification for change production in tank CR
				AmitH	16/03/09	2.  TD 72716 - Add CardRangeNo[S] to cardsaledata
				Ella	16/03/09	3.	TD 69905 - Wet stock report CR (fixes)
				Liat	17/03/09	4.  TD 69173 - AllTrs - Pumpsrv addition - return unpaid PAK trs which is on offer according to a flag.
				Liat	18/03/09	5.	TD 72394 - Prepay volume limit - Pass the preset type to PosPump.
				AmitH   22/03/09    6.  No TD - Pumpsrv crashed when initialized when prm TerminalBaseNumber = 0 - Number of pumps was treated as 64 even when it was 32.
				Ella	22/03/09	7.  TD 73366 - On SetGradePrice from BOS, PumpSrv does not inform IFSFTerminalSrv on price changes (Merge from 1019)

	4.0.22.504	Gena	24/03/09	1.	TD 70956 - Limit dispensing pumps per cashier.
				Ella	26/03/09	2.  TD 70956 - Limit dispensing pumps per cashier - add 2 new processes for GPI::SetProcess: PROCESS_CASHIER_SIGN_IN and PROCESS_CASHIER_SIGN_OUT
				Ella	01/04/09	3.	TD 72710 - Report offline pumps to POS in GPI::GetStationStatus method (Merge from 1019)
				Liat	16/04/09	4.	TD 69344 - CR - continue - Option of delaying acceptance of RL Promotion at the Pump. 
				Liat	21/04/09	5.  TD 72936 - Fuel passes discount to Prepay when discount is not available in Member Account after PAP (merge from 4.0.20.640).
									6.	TD 73359 - Prepays Not Stopping Correctly. Reauthorized (time out) pre pay was mixed up with pak trs on the same pump (merge from 1020).
									7.	TD 73878 - Fuel is giving Non-Member card Prepays a discount. Update PakReadyTransaction for PrePay trs - only update a record in extra info qdx if one 
												already exists (don't insert a new one) - merge from 1020.
	4.0.22.505	Liat	27/04/09	1.	TD 69344 - CR - continue - Option of delaying acceptance of RL Promotion at the Pump - 
										Save the approvalMessage flag from POS in Prepays, and change value 0 to indicate customer approved. 
									2.	No TD - sDateTimeExt wasn't properly initialized and filled.
	4.0.22.506	Liat	30/04/09	1. TD 75154 - CheckNozzleLeftOutTimeout icon appers on fueling transaction - Rolling update wasn't updated due to OLA State OLA_LOYALTY_WAIT_FUELING while fueling (merge from 1020).
									2. TD 74977 - Transaction details are incorrect, showing customer owes less then they are charged for PAP. Cancel loyalty ticket flag wasn't initialized after cancel (merge from 1020).
									3. Memory leak on CFuelLoyaltyPS::ParseReceiptTextXml , ParseReceiptPrintingFromRtaps and msg buffers were decreased.
									4. TD 67786 - CR - The Pantry BP loyalty interface (FRN).
				AmitH	10/05/09	5. TD 75373 - Add sControllerSeqNum to TAG_EXTRA_FILE_INFO so it will be retured to POSTEC CL in GCI::QueryData
				Gena	11/05/09	6. TD 70956 - Add PROCESS_RESET_CASHIER and save the casier counter to ini file.
				Liat	12/05/09	7. TD 75407 - After a Credit Card is swiped at the pump and the response to Gas eward Voucher is No then a $0.00 value is sent to MTX (merge from 1020).
				AmitH   13/05/09    8. TD 74605 - On each ola session, send ola UnattendedMode[B]='Y' , in case the pump is on Unmanned mode. 
				Liat	14/05/09	9. TD 74604 - CR - Mask Zip Code on all Logs.
	4.0.22.507	Ella	21/05/09	1. TD 75752 - Clean the buffer on UpdateCashAcceptorData. New cash acceptor trs was getting the total deposit of the previous trs
									2. TD 75706 - Need to add another condition on authorize timeout of cash acceptor transaction. In case there was a deposit instead of deleting the transaction we need to move it to WAIT_TIMEOUT status.
									3. TD 72675 - PC_IFSF Specification for OPT authorization. Support new flag PAK_QUERY_UNPAID_PAK_BY_PUMP_NOZZLE on GetPAKQuery2 in order to pull PAK transactions by pump number and nozzle id (will be called by TerminalSrv)
				Liat	27/05/09	4. TD 74810 - LPE payment first flow - not finished.
									5. TD 73628 - In PapSwipe method in case there was a failure in DB connection - do not allow P@P (merge from 1022).
				AmitH   28/05/09    6. TD 75922 - DSS encrypt failure alarm was sent instead of Hash failure alarm. (merge from 1022)
				Liat	31/05/09	7. TD 75592 - Failed Cat Processing Enhancement - not finished.
				AmitH   31/05/09    8. TD 75089 - Add a new type of Open need more param : PT_SUSPEND_IDLE=13
				AmitH   01/06/09    8. TD 75930 - In case PCI ON - PumpSrv sent account number instead of track2 to POSPump In GetOlaMemInfo method, therefore reprint failed.
	4.0.22.508	Liat	04/06/09	1. TD 74810 - LPE payment first flow.
									2. TD 75592 - Failed Cat Processing Enhancement.
									3. TD 76060 - When loyalty is not valid transaction is cancelled.
	4.0.22.509	Yaron	04/06/09	1. TD 76106 - Unlock pump process was stuck in case pump was in Offline (COMMS_TERMINATED) status (merge from 1019)
				Liat	07/06/09	2. TD 38162 - Continue - Update fuel with discount attributes (FRN) - Update the grades of the discounts and ItemDiscountType (merge from 1022) - not finished.
						07/06/09	3. TD 74387 - CR - Distinguish and support the new Shopersal club -Yesh (merge from 1022) - not finished.									
						08/06/09	4. TD 75755 - PumpSrv crashes on card swipe in Loyalty if RouteSrv is not running.
									5. TD 76060 - Cont. - When loyalty is not valid transaction is cancelled.
				Liat	09/06/09	6. TD 76185 - PCI DSS: Barcode containing number of scanned clubcard fully exposed in logs - merge from 1022.
									7. TD 76061 - Press no on loyalty prompt prompt flash twice loylaty.
				Ella	10/06/09	8. TD 76096 - PrePay receipt is not printed on terminal on final Prepay transaction. PCI is on.
	4.0.22.510	Liat	14/06/09	1. No TD - m_sFiller in CPumpTransact was not initialized.
									2. TD 76232 - Wrong GPI return code was reported to Extrans in getPapTrs6.
				AmitH	15/06/09	3. TD 75605 - PCI AsymetricEncryption, Account Number doesn't show on EJ receipt. Upgrade the BuildRequestReceiptInfo() to TAG_OLA_REC10 from TAG_OLA_REC9_EX. (merge from 4.0.22.61)
								    4. TD 74387 - Continue the new field : sLoyaltyName did not pass to CL .
												   On CGCI::GetReceiptRawData3 - Return TAG_OLA_REC10 instead of TAG_OLA_REC9 (merge from 4.0.22.70)
								    5. TD 74387 - Continue Also move UpdateAttendandSale to work with TAG_OLA_REC10 instead of TAG_OLA_REC9  (merge from 4.0.22.70)
									6. TD 75693 - Hash and Encrypt fail alerts were set twich : once with trs number (device) and once without
								    7. no TD - remove unnecessary log messages of delivery emulation per tank
									8.  No TD - Remove Zones data from log if PCI is on.
	4.0.22.511	Liat	17/06/09	1. TD 72606 - WET STOCK ENHANCEMENTBUILD PHASE - add start fueling time field - not finished.
				Gena	17/06/09	2. TD 76419 - PCI AsymetricEncryption, Hashing always fails after initialize failed 
									3. TD 76418 - PCI AsymetricEncryption, change store mode from bos sending Reconfigure to alternate when 'BackUp'=0.
				Gena	22/06/09	4. TD 76455 - PCI AsymetricEncryption, Alert of init failure is incorrectly pops up when store mode=0 and ErrCode is between -1 to -9.
									5. TD 76462 - PCI AsymetricEncryption, There is no initialise when changing store mode. (CBaseEncodingAlg derived from CConfigurationLoader)					
				Liat	23/06/09	6. TD 74797 - CR - New WEX Prompts for Conoco - not finished.
									7. TD 76482 - ClubCardNumber is populated with exposed track data and ClubCardTrack2 contains garbage.
				Gena	24/06/09	8. TD 76537 - PCI AsymetricEncryption, store mode=3, PAN req. protection=0, account is masked in pumpsrv log according to card validation.
									9. TD 76545 - PCI AsymetricEncryption, Drwatson of PUMPSRV in Alternate till when I changed store mode through the FCC.
									10.No TD	- PCI AsymetricEncryption, Change configuration XML file structure.
				Ella	24/06/09	11.TD 74794 - EMV Handle Overfill on Pay at Pump and Prepay Transactions with EPS
									12.TD 74795 - EMV Support for Coupon Printing at the Pump with EPS
									13.TD 74796 - EMV Print Completed Prepay receipt on the Pump
				Liat	28/06/09	14.TD 71868 - CR - Credit Card Hashing and Card Holder Encryption (merge from 1020).
									15.TD 76614 - Fuel deletes LoyaltyID causing PAP transactions not to go to Loyalty on completion.
				Yaron	10/06/09	16.CR 75479 - Postec Pump power down handling - Add a new flag to GCI initInterface: INIT_CONTROLLER_ALLOW_AUTH_ON_STOP_BY_CONTROLLER
				AmitH   18/06/09    17.TD 76441 - Postec - New Reservation Extension - RES_EXT_PAK_CANCELED.
												  handle "fill 0" transactrions. CL will raise a special RES ext in SetPumpStatus and pumpsrv will remove the trs altough it has rolling update.
				Gena	25/06/09	18.	TD 76577 - PCI AsymetricEncryption, Record 04 is masked in 3rd transaction before reinitialize.
									19.	No TD    - PCI AsymetricEncryption, ReInitialization to Dss changed.
				AmitH   29/06/09    20. TD 76535 - In case MaxPostPay = 2 - Could not authorize a 3rd trs even if one of the 2 UNPAID are paid,
									   becasue GCI_PUMP_TRS_LIST_IS_FULL was raised in Set pump status PAK , NULL RES , REQUESTING.
				Gena	01/07/09	21. TD 77125 - PCI AsymetricEncryption, Application Error message to PumpSrv.exe   
									22. TD 76407 - Support DSP encryption module.(not finished)
				Liat	08/07/09	23. TD 74810 - Cont. LPE payment first flow - support FlowControlActive parameter under PumpSrv\FlowControl in the registry, and update copying messages procedure.
				
	4.0.22.512	Ella	09/07/09	1.  TD 75378 - Wet stock report is not printed consistently.

	4.0.23.0	Gena	12/07/09	1.	TD 76577 - PCI AsymetricEncryption, Record 04 is masked in 3rd transaction before reinitialize(Finished).(Merge 1022)
									2.	TD 76536 - Pumpsrv sends duplicated PCI alerts from Alternate till.(Merge 1022)
									3.	No TD	 - PCI AsymetricEncryption, m_sEncryptionKeyRef was erased after Reinit imstead of before...(Merge 1022)
									4.	TD 77125 - Cont. - Drwatson of PUMPSRV when changed store mode - fix re init.(Merge 1022)
				Liat	12/07/09	5.  TD 74810 - Cont. LPE payment first flow - support FlowControlActive parameter under PumpSrv\FlowControl in GetParam method.
									6.	TD 76407 - Support DSP encryption module.(cont)

	4.0.23.20	Gena	14/07/09	1.	TD 77416 - There is no reset of the counter (max cards before re-entry) during initialise.(1022)
				Ella	15/07/09	2.	TD 77424 - Can't finish fuel grade change in tank (Merge from 1019)
				AmitH   16/07/09    3.  No TD    - enlarge qdx files size definisions of : PUMP_PRE_PAY_FILE_SIZE ,FUEL_DELIVERIES_FILE_SIZE,
											      EXTRA_INFO_FILE_SIZE,RECEIPT_EXTRA_INDEX_FILE_SIZE.
				Gena	19/07/09	4.	TD 77536 - Reprint of a PAP receipt fails.	
				Liat	19/07/09	5.	TD 77534 - PAP receipt shows payment account number when no loyalty is used.Clear loyalty ID and ClubCardTrack2 in case payment is swiped.
				Liat	20/07/09	6.	TD 75376 - Reprint attendant sale doesn't print zones (merge from 1015).
									7.	TD 77298 - Update FL_EXTRA_RECEIPT_REC flag only when insertion to receipt extra data qdx succeeded and take the deletion from RECEIPT_EXTRA_DATA out of remark.
													Update qdx in case insert retcode is 5 (exist)- (merge from 1015).
				AmitH   20/07/09    8.  No TD    -  Change qdx files size definisions to be sizeof(XX_REC)     					
				Gena	20/07/09	9.	TD 76619 - Pump authorization exceed the max cashier limit after first authorization reject.
						21/07/09	10.	TD 77501 - There is no encryption when initialize is failed with reason of hash failure(-10).
				Liat	22/07/09	11.	TD 75234 - CR - Conoco Phillips Applied Discounts- not finished.
						22/07/09	12. TD 76383 - CR - Loyalty First Payment Flow at the Pump
				AmitH	21/07/09	13.	TD 77553 - ALARM_FORECOURT_IN_MAINTENANCE was never closed - close it in UNLOCK when MAINTENANCE is finished.
									14. TD 77298 - Continue - do not save Zones to Receipt extra data qdx in ParseData authorize session (only on complete). 
												   This is because in case of decline or cancel they would stay for ever in the Receipt extra data qdx. (merge from 1015)
				Gena	23/07/09	15.	TD 77627 - Reprints from POS does not work.												

  **********************************************************1024 Beta Start**********************************************************

   4.0.23.500   AmitH	29/07/09	1.	No TD	 - add OverfillAdjusted to the xml convertor.
				Gena	30/07/09	2.	TD 39254 - PumpSrv redundancy improvments. PumpSrv will Write\Insert\Delete\Update local and HydService remote.
				AmitH   30/07/09    3.  TD 77826 - CustomerName[S] was exposed in the logs. 
				Liat	02/08/09	4.	Merge from 1023
										4.1 No TD - add StructCardSaleExtraData5 to convert from and to xml of CardSaleAll3.
										4.2. TD 75234 - Cont. Conoco Phillips Applied Discounts. Add the new fields to xmlConvertor- not finished.
										4.3. TD 77818 - "Loyalty First": PumpSRV does not change reservation to PAK after selecting MOP in terminal.
										4.4. TD 77845 - Transaction stucked in W_Progress.
				Ella	28/07/09	5.  TD 76395 - The customer was charged for car wash transaction even when wash code generation has failed (Merge from special 1014)
				Ella	02/08/09	6.	TD 74964 - Add functionality of cancel delivery single tank process (Merge from 1019)
				Gena	03/08/09	7.	TD 75951 - Create new registry key under: SOFTWARE\Pointofsale\PumpSrv\Receipt\ExtraDataCount, default is 0. 
				Liat	03/08/09	8.	TD 74797 - CR - New WEX Prompts for Conoco - not finished.
				Ella	03/08/09	9.	TD 77843 - Price change request is not sent to CL on change product in tank process (Merge from 1019)
				Ella	03/08/09	****** TANK_READING QDEX ENLARGED FROM 100 to 200 bytes ********
				Ella	03/08/09	10. TD 72606 - WetStock Data Export CR (Merge from 1021)
				Ella	03/08/09	11. TD 77765 - On pump totals and tank readings no indication of EOS process for Extrs.(Merge from 1021)
				Gena	09/08/09	12.	TD 77501 - There is no encryption when initialize is failed with reason of hash failure(-10).(Merge from 1022.110)
				Liat	09/08/09	13.	TD 72717 - New function in POSPump - resume pump (SL 64832) (FRN)- not finished.
				Liat	10/08/09	14. TD 75234 - Conoco Phillips Applied Discounts - send total discount to ola on complete also when the discount is percent off, 
										change log message of total discount to include pump number and add extra logs.
				Gena	11/08/09	15.	TD 78089 - Limit Dispensing> Pump authorization exceed the max cashier limit.
									16.	TD 78096 - Problem with PCI alerts when trs number is greater than 999.(Merge from 1022)
				Liat	12/08/09	17. TD 78023 - Offline transactions are displayed for reprint at the POS.	
				Ella	13/08/09	18. No TD	 - In Meggitt environment there is no indication of EOS for tank readings (Merge from 1021).
				Liat	13/08/09	19. TD 78062 - Wet Stock - Fuel (Meggitt) - No start fuel time in the 96 record (merge from 1021).
				Liat	19/08/09	20. No TD (CR) - Support a new field DoNotPrintExpirationDate[B] from OLA (merge from 1021).
				Ella	20/08/09	21.	No TD	 - Doms CL was getting an xml instead of struct in PAPSwipeResult (merge from 1022)
				Gena	20/08/09	22.	TD 78302 - PCI Alerts should get ticket number instead of transaction number.(merge from 1022)
									23.	TD 69914 - Support ANPR with htec.

  4.0.23.501	Gena	26/08/09	1.	TD 78374 - CFI - Red triangle does not disappear from the pos pump.
									2.	TD 78380 - Ticket number is not being syncronized during Multisync.(merge from 1022)
				Liat	27/08/09	3.	TD 78421 - Limit fueling to 8(eight) concurrent nozzles at one time doesnt work (merge from 1023). 
				Yaron	02/09/09	4.  TD 78654 - PD Error was not clearing from POS
				Liat	03/09/09	5.  TD 78146 - Impossible to pay second PAK transaction if first was selected in another POS.
				Gena	04/09/09	6.	TD 77933 - When do the EOS,There is no PUMP Reading,but if you do read PUMP reading by manual,it is work. (merge from 1019)
				Ella	07/09/09	7.	TD 78465 - Do Tank Grade Setup,when some pump is offline, it causes tank grade setup failure. (merge from 1019)
				Ella	07/09/09	8.  TD 76330 - Sync BOS & Fuel Grades - Adding new grades CR (merge from 1019)
  4.0.23.502	Ella	08/09/09	1.	TD 75947 - Crind dry goods receipt CR
				Liat	08/09/09	2.  TD 78545 - 'Drive Off' animation appears after using Stop and Restart functions from POSPump.
				AmitH   09/09/09    3.  No TD    - Fix logic error in PapReadytransaction: 	if (retCode == GCI_OK) instead of: 	if (retCode = GCI_OK) 
				Liat	09/09/09	4.  No TD	 - Loyalty cancel ticket method was called when loyalty is not active.
  4.0.23.503	Liat	13/09/09	1.  TD 72229 - GMV_Carwash transactions do not complete (merge from 1014).
				Gena	14/09/09	2.	TD 78620 - Delivery Reference number recorded into 'Delivery Volume' field.(merge from 1022)
									3.	TD 78597 - New QDX error alarm.(merge from 1022)
				Ella	15/09/09	4.  No TD	 - Exception when trying to read IndexNumber from the XML
				AmitH   15/09/09    5.  TD 78063 (Old TD) - Pump test did not work.
				AmitH	09/09/09    6.  TD 78769 - Allow getting 6 digits seq number from cl (the max is 5) in case we got INIT_CONTROLLER_REPORTS_SIX_DIGITS_SEQ_NUMBER (merge from 1020)
  4.0.23.504
				Ella	27/08/09    1.	TD 73758 - After running change product in tank process, prepay on POS gets the old price. (Merge from 1019)
									2.	TD 75226 - When do tank grade setup,we select a new grade and change grade successful,but the new grade price don't send to FCC.(Merge from 1019)
									3.	TD 73759 - when there is one pump offline(not the last one),the change product in tank process will be failed.(Merge from 1019)
						29/08/09	4.	TD 73059 - Unable to move PrePay transaction. (Merge from 1017)
				Liat	06/10/09	5.  TD 72717 - New function in POSPump - resume pump (SL 64832) (FRN).
									6.	TD 76438 - Override Pump Block.
				Ella	06/10/09	7.	TD 79647 - Protection in SetPumpStatus, when there is prepay in wait_progress and status changed from Authorized to Requesting do not send AuthorizeAndLimit to CL (as if there was stop pump) - Merge from 1017
				Liat	08/10/09	8.	TD 76432 - Car Wash Special Functions. (CR)
				Liat	11/10/09	9.	TD 66705 - Not Finished - Support in fuel promotion.
				Gena	14/10/09	10.	TD 78942 - Fuel Central Setup in HQ - POC - Pumpsrv config Thread development.
				AmitH	08/10/09	11. TD 78825 - Ulladge and delivery reports did not print.
				Ella	19/10/09	12.	TD 82017 - Grey P issue. Wash code field in extra info contained 0 and therefore the buffer was cut off, As a result POS could not recall the transaction (Merge from 1017)
				Liat	27/10/09	13.	TD 85955 - PumpSrv should use method GetCardInfoEx instead of GetCardInfo (merge from 1023). 
				Liat	28/10/09	14.	No TD - Pak with LPE - when canceling trs (Pak, Ready -> Ready, Null res) CancelTicket is not sent to LPE (Merge from 1023).				
						29/10/09	15.	TD 87285 - PumpSrv crash on loyalty PAP.
						01/11/09	16.	TD 85970 - PumpSrv authorized wrongly pump (merge from 1020).
									17.	TD 85971 - Customer receiving higher cents off discount than Loyalty is awarding (merge from 1020).
						02/11/09	18.	TD 87373 - Expand taxAmt to 8 digits(merge from 1018).
						03/11/09	19.	TD 85954 - Fixed the creation of the keys Sales_Display_Mode and Sales_Display_Delay under each pump in the registry. (merge from 1017)
									20.	TD 87260 - Gift Card stay locked when used at the pump. (merge from 1017)
	4.0.23.505	Ella	05/11/09	1.	HOT FIX for Morrisons integration - NEED TO BE RESOLVED IN ANOTHER WAY
	4.0.23.506	Liat	06/11/09	1.	TD 76432 - Car Wash Special Functions - sending car wash request to CL was marked as a comment.
	4.0.23.507	Gena	08/11/09	1.	TD 89166 - Can't make trs due to Limit Dispensing.
									2.	TD 89047 - Improve recovery capabilities. (merge from 1020)
									3.	No TD    - Move the save param of ShiftsToMaintenance from InitServer to regtable. sometimes the save was called befiore Hydra was ready (WW).(merge from 1020)
									4.	TD 85948 - PumpSrv logs shows Account number in Logs (merge from 1022)
				Liat	17/11/09	5.	TD 89548 - Swipe loyalty card on loyalty flow is not recognized correctly (merge from 1023).
						18/11/09	6.	TD 91135 - Fuel crashes on completion of PAP with large XML (merge from 1020).
									7.	TD 90446 - The XML files from promotion server in PAK transactions that are paid inside should be deleted (merge from 1019) - not finished.
									8.	TD 66705 - Support in fuel promotion.
				Liat	22/11/09	9.	No TD - change PAK_INCLUDE_EXTRA_INFO_REC value to 0x8 (as a flag).
	4.0.23.508	Liat	23/11/09	1.	TD 88350 - LPE Stand Alone (merge from 1023).\
									2.	TD 89149 - Fuel Pump Start with loyalty - not finished.
				AmitH	24/11/09	3.  TD 71362 - Memory leak - remove first chance Exceptions from method Xml2TankReadingInfo.
				Liat	24/11/09	4.	TD 86952 - PRP cant void or be paid (merge from 1015).
									5.	TD 90090 - No Gray P for unbalanced Prepay by Volume (merge from 1022).
				Ella	25/11/09	6.	TD 91930 - Pay At Pump transaction is not passed to VP correctly (merge from 1022).
									7.  TD 90446 - The XML files from promotion server in PAK transactions that are paid inside should be deleted (merge from 1019).
									8.	TD 89646 - In case TerminalSrv (POS 801) tries to set trs state to ON_OFFER do not allow to set it more then once. The second time and on PumpStv will return GPI_TRS_LOCKED (merge from 1019)
									9.  TD 78775 (old TD 77560) - Price Change feedback CR (merge from 1019)
				AmitH	25/11/09	10. TD 92528 - In case Pre authorize failed by OLA - Flags were wrong and caused passing a currupted
												   xml to CL on PapSwipeResult. This caused RFS to memory leak.
				Liat	30/11/09	11.	TD 69914 - Support ANPR with htec - controllerSeqNum isn't sent to CL when flag PAK_INCLUDE_EXTRA_INFO_REC is sent in PakReadyTransaction.
				Liat	02/12/09	12.	TD 92295 - When requesting setup ticket or Sync, the device number is always 0.
				Gena	10/12/09	13.	TD 89049 - Redundancy architecture changes and switch improvements.	(Merge from 1020 WW)
									14.	TD 90359 - When CL crashed PumpSrv cmdTx is stopped after RPC timeout (~9 min).(Merge from 1020 WoW)
									15.	TD 95378 - Redundancy architecture changes and switch improvements: in case IsRedundancyByCL == TRUE, do not allow PAK payment while Server is not IDLE. (GPI_SYSTEM_DOWN_PAK_ENABLED) this casued POSPump not to find the active pumpsrv on switches.(Merge from 1020 WoW)
									16.	TD 94548 - When DPS is set track2 and account number exposed in database. (Merge from 1023)
									17.	TD 93615 - On payment first, when loyalty down transaction is cancel after payment approval. (Merge from 1023)

	4.0.23.509	Gena	13/12/09	1.	TD 95864 - Pump calibration not authorized. In case existing command in the queue, CmdTx sleeping time will be zero.(Merge from 1022)
									2.	TD 96962 - POSPump is not going down when communication to Controler has been cut off (merge from 10220 Sasol).
				Ella	24/12/09	3.  TD 98204  - add position and grade and support preset by position for PAK (merge from special 1014 GPN)
									4.  TD 98204 - 2 UNPAID Paks will not allow Re-Authorizing a timed out prepay. (transaction stacking - configuration: 0,2,6) (merge from special 1014 GPN)
	4.0.23.510	Liat	28/12/09	1.	TD 98465 - missing tax properties in avi conflict transaction (merge from 1022).
				Liat	30/12/09	2.	TD 97935 - ExtraInfo records were not deleted in Attendant trs that was cancelled or converted 
													to PAP due to avi conflict (merge from 1022).
				Liat	06/01/10	3.	TD 97360 - PC_IFSFSpecificationforSupportMultipleIP - Start and End counters (merge from 1019).
				Liat	13/01/10	4.	TD 100885 - Wrong tax calculation when tax amount is over 100 nis (merge from 1022).
				Gena	13/01/10	5.	TD 97894  - PosPump Messages instead of grey out.(Merge from 1020).
									6.	No TD	  - (Gap from TD 89049) The old data from the ExtraInfo is not deleted. in case PumpSrv get the PUMP_RESET command in SetProcess the ExtraInfo Qdx will be reset always. (Merge from 1020).
									7.	TD 100241 - PUMPSRV 99% CPU. (Merge from 1022)
	

	4.0.24.0	AmitH	28/01/10	**************** Formal 1024 release  ********
				Ella	14/01/10	1.  TD 100854 - Reprint from POSPump is not working in EMV environment (Merge from 1022)
				Gena	21/01/10	2.	TD 97680 - unable to authorize pump.

	4.0.24.10	AmitH	31/01/10    1.  TD 91376 - In Offline transactions (ReadyOfflineTrs) the sFuelAmt was devided by 10.(merge from 1019)
									2.  No TD - In case OfflineTrs, ignore recovery process	(merge from 1019)
									3.  TD 69655 - ReadyOfflineTRS with RFS IFSF: RFS initialized Pumpsrv to work with XML, but sends a structure in ReadyOfflineTRS.(merge from 1019)
									4.  TD 102983 - Add active shift number to the MinimalPumpsInfo3 method (TAG_MINIMAL_ALL_PUMPS_5)
	4.0.24.60	Liat	08/02/10	1.	TD 106462 - US - WOR - Grey P and sales showing zero. Uninitiated data caused fake deposit in cash acceptor transactions. 
						11/02/10	2.  TD 104610 - Loyalty number should be returned only in ClubCardNumber (merge from 1023). 
									3.  TD 105087 - LPE(SA) PrePay with discount buffer fill up incorrect data in ExtraInfo (merge from 1023).
				AmitH	13/02/10	4.  TD 105269 - Add 2 fileds to OLA.h and TAG_EXTRA_INFO :sGRTotalDiscountCarWash and sGRTotalDiscountFuelItem
				Liat	14/02/10	5.	No TD - VP recovery.
				Gena	15/02/10	6.	TD 106586 - add protection to decrypt empty fields.(merge from 1023)
						16/02/10	7.	TD 106586 - PumpSrv is not clear the track's account data.
				Ella	17/02/10	8.	TD 107311 - Define new configuration parameter to ignore non-valid grade in CTRL::SetGradePrice (IgnoreNonValidGradesOnPriceChange)
				AmitH   17/02/10	9.	TD 106586 - Continue 
				Liat	18/02/10	10.	TD 105272 - BP Loyalty CR updates (merge from 1023).
 				Marik/Liat 21/02/10	11.	TD 109168 - Press cancel transaction leaves a stuck transaction (merge from 1023).
 				Liat	23/02/10	12.	TD 109690 - TaxIncluded is not sent to vp.
									13.	TD 109103 - Ola debit extra data is not sent through xml (merge from 1023).
									14.	TD 109616 - BP Loyalty - Key Manual entry is not working. 
													Skip GetCardInfo call through the flow and move HandleLoyalty method to GCI (merge from 1023).
				Amith	23/02/10    15. no TD     - PD error treatment continue : In order to always clear PD error allow authorize in any case of STOPPED_BY_CONTROLLER and m_AllowAuthOnStopByController.
				Gena	28/02/10	16.	TD 106586 - Remove confidences data from OLA qdx. (Merge from 1023).	
				Liat	28/02/10	17. TD 103546 - Fuel  to support PLU item in communication with LPE (Merge from 1023).
				Ella	01/03/10	18. No TD - PumpSrv sends wrong 'AVISpecialCodes' field to OLA, therefore OLA decline AVI.
				Gena	02/03/10	19.	TD 105272 - BP Loyalty CR updates - create ManualLoyaltyOnIdle parameter.(Merge from 1023).
									20.	No TD - sDeferralAccepted was copied from a wrong field.(Merge from 1023).
				Amith	03/03/10    21. TD 107518 - IsRedundancyByCL - Allow getting a PAK transaction in the middle of a PAY_AT_PUMP reservation.
												    Also, clear Pump status data in ResetPSPumpInfo method !  (Merge from 1020).
				Amith	04/03/10    22. no TD     - PD error treatment continue : In order to always clear PD error allow authorize in any case of STOPPED_BY_CONTROLLER and m_AllowAuthOnStopByController. (Merge from 1020).
				Ella	07/03/10	23  TD 111641 - In case insert to prepay qdex fails in CGCI::SetCashAcceptorData when starting cash acceptor trs, return an error code GCI_FAILED_IO to CL (Merge from 1017)
				Liat	14/03/10	24.	TD 111746 - Support Grade restriction on PrePay transaction (merge from 1023).
									25.	TD 111407 - BP Loyalty Cert. - Prepay flow w/ Loyalty Tender Cash- Pump is not stopping at the correct Gal limit(20) (merge from 1023).
									26. TD 110902 - Traget message to added to receipt (merge from 1023).
									27. TD 111318 - Loyalty PAP Trs Number Consistency (merge from 1023).
				Gena	14/03/10	28.	TD 112773 - In case DPS encrypt algorithm in use remove confidences data from the memory in complete session.(Merge from 1023)
				Gena	16/03/10	29.	TD 112315 - Grey P doesn't disappear after recall transaction.
				Liat	16/03/10	30. TD 113751 - PumpSrv clear loylaty card data when we get sMediaMasterType 1 (credit + loylaty) (merge from 1023).	
	4.0.24.70  AmitH   16/03/10    1.  TD 113017 - Could not start delivery single tank while one of the pumps are in UNKNOWN Status This is a legal status in IFSF pumps... (merge from 1019)
				Liat	21/03/10	2.	NO TD - fix TAG_DISCOUNT_LOYALTY_INFO3 size- add 33 bytes to the filler.
				Gena	21/03/10	3.	TD 114207 - Unable to reprint PAP receipt from POS after CL exception.(Merge from 1023)				
				AmitH   22/03/10    4.  TD 113017 - Continue - Could not start delivery single tank - continue - should also mark non active pumps as Locked.
				Ella	23/03/10	5.	TD 110193 - Prices were not sent to CL in price change process. (Merge from 1019)
				Gena	24/03/10	6.	TD 114655 - Multisync PAK and Prepay dissapear while switching to slave.
				Liat	25/03/10	7.	TD 115660 - CANADA 1023 Language (French/English) does not appear in xml (merge from 1023)
				Ella	28/03/10	8.	TD 113020 - On change product in tank, new grade got an empty pure product A.(Merge from 1019)
				Ella	28/03/10	9.  TD 111683 - On change product in tank in case pure product with the requested name already exists, do not create a new one, use existing.(merge from 1019)
				Ella	28/03/10	10. TD 106064 - When changing single mode, Terminalsrv was not updating the prices for pumps according to the active mode. (Merge from 1019)
				Ella	01/04/10	11.	TD 115975 - RFS-IFSF price changed by the self service price - Price change feedback failed on timeout, instead of failing on Offline. UNKNOWN_STATUS should considered as offline, similar to COMMS_TERMINATED (Merge from 1019)
				AmitH	06/04/10	12. TD 107655 - Add Server locked status flag to TAG_MINIMAL_ALL_PUMPS_5 to be reported to POS. (merge from 1021) 			
				Gena	08/04/10	13.	TD 114988 - PumpSrv dosen't create UseBackUp keys
									14.	TD 72545  -	PumpSrv redundancy improvments.
	4.0.24.71	Liat	08/04/10	1.	TD 116500 - Speed Pass Red X on pumps on register during auth (merge from 1017).
				Gena	12/04/10	2.	No TD	  - Prepay with discounts and XML receipt. not finished. PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA.
				Liat	12/04/10	3.	TD 115660 - CANADA 1023 Language (French/English) (merge from 1023).
									4.	TD 117472 - SystsemID is send back to OLA (merge from 1023).
				Gena				5.	TD 117664 - PaP trs stuck in PAP reservation after switch. (rem HOP_HYDRA_REMOTE_ONLY)
				Gena				6.	TD 117540 - Unpaid trs disappear after switch. (rem HOP_HYDRA_REMOTE_ONLY)
	4.0.24.72	Gena	14/04/10	1.	No TD	  - Prepay with discounts and XML receipt. not finished. PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA.
	4.0.24.73	Ella	15/04/10	1.	TD 117927 - Change tank grade,add a new grade for FuelSvr,Pure products is failed after changing. When adding new pure product there was no validation on valid entry.
				Gena	15/04/10	2.	No TD	  - Prepay with discounts and XML receipt. PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA.
	4.0.24.74	Liat	17/04/10	1.	No TD	  - Data Export RTLX Agent - change SalesXmlFileCounter to be in the INI file instead of the registry.
				Gena	19/04/10	2.	TD 118884 - Failed to insert PrePay Trs. (replace DB.cpp file from 23.20)
	4.0.24.75	Gena	19/04/10	1.	No TD	  - Prepay with discounts and XML receipt.(cont)
	4.0.24.76	Gena	21/04/10	1.	No TD	  - Prepay with discounts and XML receipt.
	4.0.24.90	Gena	25/04/10	1.	TD 119728 - BP-Loyalty, If 'PromptType' = PT_CARD_READER or PT_BOOL_AND_CARD_READER -> remove the Track1 and Track2 from 'ParamResult'.
				Liat	26/04/10	2.	TD 120241 - Not all Tanks shown on POS (merge from 1022).
				Gena	26/04/10	3.  no TD     - Tesco Hot Fix!!! - illigal char in xml (0x90) Add - new parameter in GeneralParam - AddEncodingStringToVPXml (default zero) if 1 - add
												    Add endoding string to the xml header sent to VP in GetPapTrs5.(Merge from 1022)
									4.  no TD     - Tesco Hot Fix!!! - illigal char in xml (0x90) Add - new flag (GPI_REMOVE_PAP_TRS_REMOVE_FIRST) to CGPI::RemovePAPTrs. 
												    Also in this case, send alert 03 01 023.	(Merge from 1022)
				AmitH	06/04/10	5.  TD 106227 - Support working with single core. New parameter in General Param - SingleCoreActive default = 1  
				Liat	27/04/10	6.	TD 117473 - DriverID needs to be expanded to 10 bytes for OLA (merge from 1023). 
									7.  TD 103166 - PAP hangs on "Wait for card to be authorized" (merge from 1023).\
									8.	TD 116544 - Full Payment Card Number showing up in Member Card field on PAP transactions. Don't send an invalid loyalty card to LPE (merge from 1023).
									9.	TD 119090 - PAP Receipt in EJ not showing Fuel Discount Amount (merge from 1023).
									10.	TD 115613 - Paymnet card Track1 is not senT to OLA (merge from 1023).
				AmitH	29/04/10	11.	No TD     - Tesco Hot Fix!!! continue - Problem in RemovePapTrs				
				Gena	03/05/10	12.	TD 120918 - VP fail to process GetReceipt2 transaction and cause PumpSrv to create hugs file.
									13.	TD 121218 - When working with Check and reserved Discount is not apply to pump.
				Liat	03/05/10	14.	TD 120603 - No Way to Skip (By-Pass) Member Card Prompt at Pump when Yes/No property is off for swipe loyalty card (merge from 1023).
									15.	TD 121090 - Combined card data is exposed in PAP (merge from 1023).
									16. TD 120915 - On PrePay transaction with Loyalty discount, sTotalDiscout field is sent to POS with garbage discount (merge from 1023).
									17.	TD 118124 - BP-Loyalty -  Stored new ola fields  for complete session in BPIFSF - not finished.
	4.0.24.91	Gena	05/05/10	1.	No TD	  - In PrePay trs with XML, Gallon limit not be multiply by 1000.
	4.0.24.92	Gena	05/05/10	1.	No TD	  - In PrePay trs with XML, Gallon limit not be multiply by 1000.cont
	4.0.24.93	Liat	06/05/10	1.	TD 118124 - BP-Loyalty -  Stored new ola fields for complete session in BPIFSF.
									2.  No TD     - Incase of "ERR_EXISTS" (5) error in CIndexFile::Insert(...) method - do not print "Exception 1" to log, write "Failure" instead.
	4.0.24.500	Liat	10/05/10	1.	TD 122327 - PAP with loyalty-  PumpSrv does not send the Loyalty card data to LPE. First track2 char is 'B' (merge from 1023).
									2.	TD 122449 - When Pressing Pay inside the prompt for swipe loyalty is not displayed (merge from 1023).
									3.	1025 Beta release.
	4.0.24.501	Ella	10/05/10	1.	SR 118680 - In AuthorizeAndLimit wuth PREPAY_CANCEL flag protection is added to prevent from sending "Release pump" to CL in case the reservation did not change by CL to PREPAY_AT_POS.
				Liat	12/05/10	2.	TD 123025 - BP Loyalty - Stuck transaction.
				Gena	13/05/10	3.	TD 75230  - memory over run  in method CBasicProtectData::ReplaceBufferUseSize when sending the Zone[S] bigger then 256.	
	
							**************** Formal 1025 release  ********				

	4.0.25.0	Marik	17/05/10	1.	TD 123916 - Prepay transaction stuck in PrePay qdx.		
				AmitH	25/05/10	2.	No TD     - When PumpSrv calls "GetCardInfo" internally, (GCI::HandleLoyaltyResult + CCommonFunction::GetAccountNumByTrack2), 
													do not use xml.
				Liat	27/05/10	3.	TD 124743 - Add Tender reports wrong to LPE (merge from 1023).
									4.	TD 125390 - Loyalty card authorized PAP transaction wrongly and give  customer "Free gas" (merge from 1023).
				Gena				5.	TD 124689 - First check if OLA is on before calling GetCardInfo internally from GetAccountNumByTrack2.

  	4.0.25.30	Gena	27/05/10	1.	TD 125675 - PumpSrv will not verify if it's in middle of recovery process and will allow to run SetCashAcceptorData. (merge from 1017)			
				AmitH	13/05/10    2.  TD 126014 - In case the res ext is PATT already, allow swiping payment cards although
													PAP is not valid in current mode.
				Ella	10/06/10	3.	TD 111737 - Dispenser Overfill error in PumpView (Merge from 1014)
									4.	TD 119691 - CardFinancialAdvice responded with failure as a result of PumpSrv failure in GPI:FinalizePAKTrs (Merge from 1019)
									5.	TD 111474 - In opt, in over dispensing prepay - send SPPS the pre authorized amount. (Merge from 1019)
									6.	TD 126154 - Unexpected failure in chage product in tank. (Merge from 1019)
									7.	TD 127181 - In some cases wrong grade name and pure product name appeared after change product in tank process. (Merge from 1019)
				AmitH	17/06/10    8.  TD 129545 - Insert data to Receipt extra index qdx - showed Track2 data in the log even when RemoveCardDataFromLog is ON. (merge from 1022)
								    9.  TD 128663 - Gas sale at not showing up in BOS  (merge from 1017) 
									10. TD 128767 - Site Unable to Reserve Pump (merge from 1017) 
						24/06/10    11. TD 129043 - alarms 3,1,7 & 3,1,8 will be with device number = 0 instead of 999.
				Ella	01/07/10	12.	TD 126194 - Car wash code was not sent to TerminalSrv when generated. (Merge from 1022)
									13.	TD 118690 - Additional Support for Pay Inside Button Active with FCC PayInside Flag Turned Off (Merge from special 1014)
				Gena	01/07/10	14.	TD 132787 - Receipt not found when receipt key is hash.
				Liat	05/07/10	15.	TD 128024 - Fuel promotion can't work if the grade id of fuel item is different from extended grade id (merge from 1019).  
									16.	TD 131250 - AVI stuck Inprogress - PumpStat data is reset after restarting fuel (merge from 1022). 
									17.	TD 126199 - PAP started with lifting nozzle and then swiping card causes Free Gas (merge from 1023).
						06/07/10	18.	TD 125623 - When LPE Loyalty Card is used first in flow, PumpSrv is sending a corrupted Track 1 in the Complete (merge from 1023).
									19. TD 126185 - Loyalty Flow Pay inside - Cl2pumpSrv does not send  PumpSrv response if customer  Press cancel (merge from 1023).
									20. TD 127468 - Fuel needs to support Store & Forward for P@P loyalty transactions (merge from 1023).
									21. TD 97890  - Support loyalty on commercial fueling version - add only new qdx defines.
									22.	TD 132711 - AVI Decline icon on PosPump is wrong.
	4.0.25.40	AmitH	15/07/10	1.	TD 134650 - In case of Ola by controller and lSeqNumber bigger than 99999 - copy only the lst 5 digits to cardData.sTranNbr.
	4.0.25.41	Liat	19/07/10	1.	TD 127275 - Divide the PreAuthAmount by 10 in case of a value limit (merge from 1019).
				Ella	19/07/10	2.	TD 122535 - Mode loaded very slow
	4.0.25.50   AmitH   20/07/10    1.  TD 136037 - In case of DSS hash or encrypt failure alarms (3,1,28 3,1,29) - the device number will be the 
													last 2 digits of the transaction number instead of 3. also, DSS init failure alarm - (3,1,27) device number will be zero instead of 999 (merge from 1022)s
				AmitH   22/07/10    2.  (Marik) TD 134572 - Fuel stops you at Loyalty limit if you say No to deferral prompt. (merge from 1023)
				Ella	25/07/10	3.	TD 108311 - Compare actual and calculated fuel balance CR - not finished
				Liat	25/07/10	4.	TD 136174 - Tender Amount does not match Dispensed Amount on PAP Trans (merge from 1023).
						26/07/10	5.	TD 136530 - Loyalty card swipe on PAP transaction without  discount case to OLA corrupted record (merge from 1023).
						29/07/10	6.	TD 133966 (CR) - Support Trendar  (commercial fueling) Transaction (FRN) (merge from 1023).
									7.	TD 137641 - PAK Stuck Transaction (after cancel) and PAP not completed (merge from 1023).
				Ella	01/08/10	8.	TD 136915 - Fuel product name need to be sent to EPS to print on outside Completed prepay receipt (Merge from 1022)
									9.	TD 133961 - Car Wash details was not passed correctly to TerminalSrv (Merge from 1022)
						03/08/10	10.	TD 136918 - 9002 POS needs to send Prepay transaction number to Fuel to pass to EPS (Merge from 1022)
				AmitH   04/08/10    11	TD 134650 - Continue to 134650 - In case lSeqNumber bigger than 99999 - copy only last 5 digits to the receipt extra data key. 
	4.0.25.80   Ella	22/08/10	1.	TD 140655 - 9002 Not Fuel amount or product code sent to EPS for Prepay printed at pump receipt (Merge from 1022)
				Gena	26/08/10	2.	TD 142857 - Memory leak. Change cast of _bstr_t from (char*) to (const char*). Do not pass the OlaBase member m_pbsrOlaBuffer to COM interface. (Merge from 1022)
									3.	TD 142081 - QDX maintenance full path support and logs addition.
									4.	TD 108311 - Compare actual and calculated fuel balance.
									5.	TD 141363 - Club card account(loyalId) contain cleared PAN in the TRA file when PCI is on. Clear LoyalId.(merge from 4.0.24.201)
	4.0.25.81	Gena	30/08/10	1.	TD 142081 - QDX maintenance full path support and logs addition.
	4.0.25.90	Ella	31/08/10	1.	TD 142798 - Support more than one tax for C/W and fuel product in EMV environment.
				Gena	31/08/10	2.	TD 143987 - Q-DEX 25 Exception in PumpSrv log.

	4.0.25.500	Gena	01/09/10	Formal release 1026 beta.
	4.0.25.501	Liat	01/09/10	1.	TD 140984 - TotalDiscount is not sent to POS in prepay trs (merge from 1023).
				Gena				2.	TD 137509 - Sassol - Wetstock - attendant tagging.
				AmitH               3.  TD 138045 - Copy of receipt from PosPump doesn't include dry items in attendand sale.
				AmitH	         	4.	TD 140738 - Avi cancel puts the wrong misc (Declined PAP)
				AmitH				5.  TD 142653,142654 - In TRA missing receipt data for "Delek credit card". These cards are with track2 
											with '?' at the end - add call to "Track2Format" in CCommonFunction::GetReceipt2 method.
				Marik				6.  TD - 114999 PumpSrv Reads wrong grade value from registry,
				Liat				7.	TD 133580 - 161 - Yermiaho - cant close transaction after AVI conflict (merge from 1022).
				AmitH			    8.  TD 136887 - UnBalance Cash Acc trs with no rolling update did not have gray P.
				Liat				9.	TD 144101 - Fuel - Discount doubled on receipt (merge from 1023).
				AmitH              10.  TD 114999 - PumpSrv Reads wrong grade value from registry,
	4.0.25.502	AmitH	02/09/10	1.	TD 144507 - merge CR 135212 - Support Add GUI for automatic shift settings in FCC
	4.0.25.503	AmitH	05/09/10	1.	TD 144507 - merge CR 135212 - continue
				Liat	12/09/10	2.	TD 144870 - Shift change function corrupted.
									3.	TD 144328 (111746 Cont.) - Support Grade restriction on PrePay transaction (merge from 1023).

				**************** Formal 1026 release  ********
  
	4.0.26.0	Ella	15/09/10	1.	No TD - Rounding issue with tax amount in EMV environment. (Merge from 1022)
									2.	No TD - Taxes were not reported in correct fields to EPS in PAP transaction in EMV.(Merge from 1022)
									3.	TD 145615 - TransAmount field does not include taxes in the total sent to EPS. (Merge from 1022)
									4.	TD 146072 - Coupon amount does not make it to the Back Office to populate the revved up reward report (Merge from 1022)
									5.  TD 126195 - On overfill in Pay At Pump OverfillAdjusted flag was not passed to VP.
				Gena	21/09/10	6.	TD 146525 - Duplicate receipt doesn't display VAT
				Liat	27/09/10	7.	TD 144665 - Incorrect default value for GeneralRequestsTimeout (merge from 1025).
				Liat(Amit)			8.  TD 147338 - PumpSrv crashed on GetCardInfo.
				Ella				9.	No TD - Attendant Tagging info was not coppied to pump transact in SetTransactionState method. (Merge from 1022)
				AmitH   03/10/10    10. Release 1026.0
	4.0.26.500	Liat	07/10/10	1.	TD 152967 - Issues With Price/Discount With Price Change During Transaction (merge from 1023).
				AmitH   13/10/10    2.  TD 154354 - PumpSrv took Date time from PC also in Complete session and put in AuthorizationDate[D] and AuthorizationTime[T] (Merge from 1024)
				Ella	13/10/10	3.	TD 154350 - Receipt could not be found in Cash Acceptor transaction (GetReceipt2 called by VP) since Track2 was removed. (Merge from 1024)
				Gena	18/10/10	4.	TD 144219 - Reprint receipt from POS. (Tesco DSS asymmetric algorithm).
				Gena	21/10/10	5.	TD 144221 - Car wash integration.
									6.	TD 156842 - PumpSrv failing to RemoveProhibitData.(Merge from 1023)
	4.0.26.501	Ella	27/10/10	1.	TD 156649 - Total amount was not reported correctly to VP in pay at pump transaction with carwash (taxes were not included). (Merge from 1022)
									2.	TD 140673 - In OPT , PreAuthAmount is not cleared from previous transaction (Merge from 1019)
									3.	TD 116227 - Support Extended grade for 999 grades CR.
				Liat	02/11/10	4.	TD 144221 - CR - Car wash Validity.
				Gena				5.	TD 154354 - PumpSrv took Date time from PC also in Complete session and put in AuthorizationDate[D] and AuthorizationTime[T](merge from 1024)
						09/11/10	6.	TD 144220 - Reduce Down time when QDX load.
									7.	TD 143919 - local override of OPT to P@P.
	4.0.26.502  Ella	17/11/10	1.  TD 163315 - POS sent AuthorizePump a moment after a PAP started at the terminal. Protection was added not to allow authorizing the pump when starting a PAP. In special PAP authorization request should be sent by EPS only.(Merge from special 1014)
				Ella	18/11/10	2.	TD 108311 - Compare actual and calculated fuel balance CR - provide rolling update info on request
				Liat	22/11/10	3.	TD 153441 - Card Chip&Pin not accepted.
	4.0.26.503	Gena	28/11/10	1.	TD 158047 - Unable to dispense Fuel after a controller switch has been undertaken. (Merge from 1025.Morrisons)
									2.	TD 108311 - Compare actual and calculated fuel balance CR - integration fixes. (Merge from 1025.TNK)
									3.	TD 164518 - 9002 Bank batch Reconcile Detail Report Format Should contain OLA batch number.(Merge from 1022.FCL)
				Liat	30/11/10	4.	TD 157073 - 1653 ANPR (FRN) - not finished.
									5.	TD 163818 - Cancel Trs by using button <Cancel> stuck the Fuel system (merge from 1023).
									6.	TD 157216 - Pump has stuck transaction #1 - cancel was pressed while waiting for discounts from loyalty server (merge from 1023).
				Gena	07/12/10	7.	TD 167039 - Receipt printed more times then defined in  "Maximum number of copies".
									8.	TD 165971 - GPIEventsQueue full cause to Refueling oftenappearblack P,and the black P cannot cancel and cannot refueling.(merge from 1019)
	4.0.26.504	AmitH	13/12/10	1.	TD 137509 - Continue: Add OfflineForceAcc to XmlConvertor.cpp
	4.0.26.505	Liat	13/12/10	1.	TD 160508 - Cancelled cash acceptor trs with 0 deposit should not generate a PayIn (ola record) (merge from 1024).
									2.	TD 62457 - A gray P is displayed on PosPump, but there's no UnbalancePrePay. Cash acceptor trs with total deposit 0 wasn't canceled (merge from 1017).
									3.	TD 68360 - Stuck Prepays on pumps. Close cash acceptor trs with non zero deposit as unbalanced (merge from 1017).
									4.	TD 157073 - 1653 ANPR (FRN) - includes - ExtraInfo QDX expansion to 1024.
				Gena	15/12/10	5.	TD 168981 - Only original receipts reprint from POS,while didn't take receipt from OPT.
	4.0.26.506	Liat	20/12/10	1.	TD 157073 Cont.- 1653 ANPR (FRN) - ANPR failed transactions shouldn't get timedout.
	4.0.26.507	Liat	29/12/10	1.	TD 157555 - Can't cancel the PRP while in the middle of EOS (merge from 1022).
				AmitH	03/01/11	2.	No TD - Add a new field for Tesco: RestrictPoints[B]. (get in authorize, return in complete).
	4.0.26.508	Liat	03/01/11	1.	TD 147183 - APC - 1665 - Automated Price Change (FRN) - Story 1 - manual price change.
				Gena	10/01/11	2.	TD 146575 - FCC Configuration Export.
				AmitH	12/01/11	3.	No TD - ValidateWasCode - add code to the log. 
				Liat	12/01/11	4.	TD 213099 - Expand the buffer holds the receipt printings xml (merge from 1023).
				Gena	16/01/11	5.	TD 211018 - no need to convert to extended grade. Prepay in opt, the amount is wrong. (merge from 1019)
									6.	TD 211718 - GPIEventsQueue full. change event thread to work all the time if queue not empty. Change m_cEventsQueue size to 500.(merge from 1019)
									7.	No TD     - In case EPS send cancel transaction while there is PAID transaction in fuel server, Fuel server send success to EPS.(merge from 1019)
									8.  TD 177984 - Cancel previous fix. copy sLoyalId from XML to ExtraInfo qdx and from ExtraInfo qdx to SALE_EXTRA_DETAILS record for TerminalSrv.(merge from 1022)
	4.0.26.509	Gena	18/01/11	1.	No TD	  - QueryReceipt__ not return receipt with PCI\CashAcceptor key.(merge from 1024)
				Liat				2.	TD 147183 - APC - Cont. - Automated Price Change (FRN) - Story 1.
	4.0.26.510	AmitH	24/01/11	1.	TD 137509 - Continue: return TAG_OLA_REC12 in GetPapTrs5 to add CARD_SALE_EXTRA_DATA5_ to the returned struct/xml to VP/Extrs
	4.0.26.511	Liat	25/01/11	1.	TD 147183 - APC - Cont. - support reconfigure on price pole existence.
				Gena				2.	No TD      - Change pRec parameter from InOut to Out in CTRL::GetPumpTotalsEx interface.(Merge from 1025)
	4.0.26.512	AmitH	26/01/11	1.	TD 215073 - Let CheckAndReserve Start fail immediately if there is already IN_Progress or Wait_progress trs on the pump.(Merge from 1024)
				AmitH	26/01/11	2.	TD 216783 - in case of price change integrity manual, pumpsrv did not sent the "commit" price change alarm when received the event from POSPump. 

	4.0.27.0	AmitH	31/01/11  	**************** Formal 1027 release  ********

	4.0.27.20   Shuki	03/02/11    1.  TD 215136 - Price change did not occur if PRICE_END_OF_LIST was sent on a grade with price 0
	            AmitH	03/02/11    2.  TD 216465 - A lot of "Dispenser overfill" alarms are generated with no reason, when limit by volume.
				Marik(A)26/01/11	3.  TD 217493 - US SR6 - When fuel goes down in the middle of a CA trs, no PayIn entry is found in VP TRA; even though the transaction closed properly (merge from 1024)
				OmerT	09/02/11	4.	TD 215887 - Reprint of reciept using car registration is unlimited. The bug was fixed by changing initialization of the record serached in the Qdx from 0x0 to 0x20(' ') in gci.cpp in ReceiptPrinted__ method. (merge from 1022)
				OmerT	09/02/11	5.	TD 219348 - Reprint receipt from PosPump is printed with original label. in GPI.cpp in RecieptPrinted2__ added recopy of the updated PAPInfo with "oringal printed" bit turn on.
				OmerT	15/02/11	6.	TD 219520 - Pap with TAXI CARD does not update discount price. It's a two swipe scenario. loyalty swipe and payment swipe. The lDiscountType was delted in the 2nd swipe in PumpSrv hence RFS didn't update the price. The fix is in OlaBase.cpp in ParseDate method.
				Marik(A)15/02/11    7.  TD 219740 - Cannot do a prepay with a fuel restricted card (merge form 4.0.23.851). 
				Shuki	15/02/11    8.  TD 147183 - APC - Automatic Price change - not finished.
				AmitH   20/02/11    9.  TD 219520 - continue.
	4.0.27.30	Liat	01/03/11	1.  TD 222093 - Swiped member card number is sent for PAP completion instead of the credit card number (merge from 1023).
				AmitH   02/03/11    2.  TD 157086 - Support chip Arval card: Add new fields with OLA (Tesco CR) 
				AmitH               3.  TD 223183 - In case of APC - when got price process failure from RFS , pumpsrv will send the Price process failure alrm (ALARM0211066) instead of process ended.
				Shuki               4.  TD 222751 - Prices were not sent to RFS after commit (failure increas).
				AmitH               5.  TD 219348 - Reprint receipt from PosPum printed with original label - ReceiptPrinted did not succeded. 
	4.0.27.31	Liat	03/03/11	1.	TD 212541 - Flow Control Options For Response To Method Of Payment Prompt (merge from 1023). 
				Liat (Amit)07/03/11 2.  TD 223052 - PrePay Receipts printed after Fuel dispense on the CRIND are printing with out Date or time on them.(merge from 1024)
				AmitH   07/03/11    3.  TD 223042 - PrePay Receipts, if the receipt extra data record already exist qdx - update it.  (merge from 1024)
				Shuki   08/03/11    4.  TD 224614 - If grade price is 0, on price change end of list, batch number is not saved             
				Shuki(Amit)         5.  TD 224361 - Clean buffer of Extra_info with ANPR data before inserting to Extra info qdx.
				Marik				6.  TD 223847 - US - SR6 .59.02 Testing - Pos gets into a loop if recalling a prepay from an Offline Pump
	4.0.27.32	AmitH	10/03/11	1.  No TD     - Add Load the LoadAllProductSoldOnPump to OlaProgressSrvEx in order to be able to report to it the ItemCodes
									2.  TD 223052 - cont. - copy the year from the xml as 2 digits.
									3.  TD 157086 - Cont. Support chip Arval card: Add new fields with OLA. 
	4.0.27.40	Shuki   15/03/11    1.  TD 165585 - Support Flow Rate
	4.0.27.41	Shuki   16/03/11    1.  TD 165585 - change prm name to FlowRateActive - read by RFS as well.
	4.0.27.42	AmitH   16/03/11    1.  TD 147183 - APC - Automatic Price change Integritiy: use new field: byPriceChangePOSMsg just to inform the POS that process has ended,
	4.0.27.43	AmitH   16/03/11    1.  TD 226166 - APC - Automatic Price change Integritiy: if switch during process, Batch id was zero after switch.
									2.  TD 219297 - EGift - in OlaProgressEx - in GetCardInfo - if got new flag - RECEIPT_TRACK2_IS_BARCODE copy track2 to barcode and fill 'B' in entrymethod.
													Add more ola fields for EGift.
	4.0.27.44   AmitH   22/03/11    1.  TD 226700 - Add one byte to ola field "sChargeTo_MSB" (merge from 1023 US customer)
				AmitH   23/03/11    2.  TD 226263, 224899 - Prepay - Protect against sending Auto Auth Prepay to CL DOMS, in case it reported moving from DISPENSING to REQUESTING (merge from 1025)
				Gena	23/03/11	3.	TD 142857 - Memory leak. Change cast of _bstr_t from (char*) to (const char*).cont (Merge from 1023)
				Liat	28/03/11	4.	TD 219297 - EGift continue.
				Gena	31/03/11	5.	TD 179125 - EFT OPT support - not finished (merge from 1021)
				Shukiy  31/03/11	6.  No TD     - APC - recovery flags were not saved in Qdx pumpstat

	4.0.27.45   AmitH   04/04/11    1.  TD 219297 - EGift continue: SendReceipt did not send extradata4 and extradata5 to CL.
									2.  No TD     - do not use XML internally in CMD_SWIPE_RECEIPT and CMD_SWIPE_RESULT.
	4.0.27.46   AmitH   05/04/11    1.  TD 219297 - Support BuildRequestReceiptInfo to return TAG_OLA_REC12 instead of TAG_OLA_REC10
	4.0.27.47   AmitH   05/04/11    1.  TD 219297 - Support CGCI::GetReceiptRawData3 to return TAG_OLA_REC12 as xml instead of TAG_OLA_REC10 as xml
	4.0.27.60	Liat	06/04/11	1.	TD 230527 - Allow dispensing while qdx maintenance is running (merge from 1024).
				Gena	11/04/11	2.	TD 179125 - EFT OPT support - finished (merge from 1021)
				Omer	12/04/11	3.  TD 231472 - Support Pump Test (merge from 1025)
				Liat	20/04/11	4.	TD 232988 - CA transaction with car wash, undispensed, retrieve in POS for change back to customer, leaving a stuck trs behind (merge from 1024).
									5.	TD 223052 cont. - copy the year from the xml as 2 digits (merge from 1024).
									6.	TD 230527 cont. - Allow dispensing while qdx maintenance is running (merge from 1024).
				AmitH	03/11/10	7.  TD 233125 - Unable to authorise pumps transactions after switch - the SignInCashiersCounter was not reloaded to memory after switch.
				OmerT	21/04/11	8.  TD 231475 - Support Tank Id passing to pos (PAK, Prepay) and VP/Extrs (PAP).
				AmitH               9.  TD 230978 - Front office should get an error in case PumpSrv is not ready to take prices SetGradePrice2 - Integritiy prccess.
				Liat	26/04/11	10.	TD 231255 - Site that appears to be getting flooded with pump reservation messages which is causing major fuel issues (merge from 1024).
				AmitH               11. TD 226263, 224899 - Continue - Prepay - Protect against sending Auto Auth Prepay to CL DOMS, in case it reported moving from DISPENSING to REQUESTING
				AmitH               12. No TD     - EGift, send PreAuthResponseCode[I] also in complete. 
	
	****************************************************************************
						1028 Beta:
	****************************************************************************

	4.0.27.500	AmitH	01/05/11	1.	TD 178809 - (merge) Loyalty PPU Transaction Details Report Incorrect (Merge from BP 4.0.124)
									2.	TD 178811 - (merge) Discount per gallon was not reported to VP when working with discounts array. (Merge from BP 4.0.124)
									3.	TD 215339 - (merge) Zones were not printed for prepay with receipt xml with discounts.(Merge from BP 4.0.124)
				Shuki				4.	TD 231498 - EGift, Recalculate TaxAmt without the overfill
				OmerT				5.  TD 109624 - Working TLG VeederRoot with Density (FRN) - support most recent delivery
				OmerT			    6.  TD 236009 - (merge) Add Qdx empty and trs force remove alarms 
				OmerT				7.  No TD - Add null to the end of the Run_Query path - fix memory overrun.

	4.0.27.501	Omer				1.  TD 235737 - Trs with Club Card and Arval Card is impossible. Send item codes to specific ola also in AddParams
				Gena				2.	TD 233308 - Gift Card: card number not displayed at Electronic Journal receipt.
				AmitH				3.  TD 236805 - APC - Automatic Price change - Return error to POS - if we already got one commit process, or if we are not in a relevant status to receive a commit.
				Omer				4.  TD 228117 - Added one byte (sCardRangeNoExt) to sCardRangeNo in CARD_SALE_EXTRA_DATA4 so it will be 4 bytes size.
				Liat				5.	TD 236976 - No limit to pump sale stacking - auto authorized PAK passed max number of allowd transaction (merge from 1022).
				Omer				6.  TD 236747 - APC - Automatic Price change Integritiy: Reset prices in RunTime.ini file. All the valid "old" prices are replaced with the new "prices". 
				AmitH				7.  TD 232586 - Register 99 generating sales into a temporary dept	- OLA by controller- merge from 1023

	4.0.28.0	AmitH	23/05/11  	**************** Formal 1028 release  ********

 	****************************************************************************
						1029 Beta:
	****************************************************************************
 
	4.0.28.500  OmerT	26/25/11	1.  TD 236756 - Site 12004495 Missing transactions in the PAP TRA'S. (merge from 4.0.24.371)
									2.	TD 235858 - Open cash accepter  transaction in  first bill insert. (merge from 4.0.24.372)
									3.	TD 235858 - re-fix. (merge from 4.0.24.374)
									4.	TD 238520 - PIC Transaction are not locking the pump so you can do a prepay inside (merge from 4.0.24.374)
									5.	TD 238258 - Recall PrePay transaction cancel current  Pump reservation (PAP\cashAcceptor) (merge from 4.0.24.374)
									6.  TD 238261 - PIC Report is not correct, it is doubling the amount. (merge from 4.0.24.374)
				AmitH   30/05/11	7.  TD 240021 - Gift Card: in case of Force complete from OLA - PumpSrv will generate the InvoiceNumber instead of RFS (was zero)
				Liat	16/06/11	8.	TD 244515 - VP Trs - tax is divided by 10 when above 100 NIS (merge from 1022).
				Ella	23/06/11	9.	TD 237976 - Zero completion was not sent to OLA on cancel in middle of need more param session. (Merge from special BP 1024)
				Ella	23/06/11	10. TD 242205 - Error when trying to close day on POS, Lock state got stuck in RESPONSE_RECIEVED instead of changing to UNLOCK (Merge from 1023)
				AmitH	23/06/11	11.	TD 236571 - US - SR6 - Site 12004495 PIC Report doesn't match Pay In's - call UpdateBillsCurrent as well.(Merge from special BP 1024)
				AmitH	23/06/11	12.	TD 236572 - US - SR6 - Unrecognized Bills ignored if Pump number is 0.(Merge from special BP 1024)
				marik	23/06/11    13. TD 238829 - US - Loyalty - outside loyalty transactions have incorrect sale item unit price amounts.
				Liat	27/06/11	14.	TD 231113 - Don't clear AccountNumber in cancel ticket when called from complete session (merge from 1023).
				Shuki   27/06/11    15. TD 224805 - GetPAKQuery2 did not convert to extended grade when some flags were used (merge from 1019)
				Omer	29/06/11	16. TD 243282 - Added creation of VaporRecoveryMonitoringIstalled key to registry.
				Omer	29/06/11    17. TD 241956 - US - 2TP - SVS cards do not have zone data on outside PIC or CRIND transactions. (merge from 4.0.124.402)
				Omer                18. TD 179277 - After reboot Fuel sending masked card number (merge from 23.992)
				Omer				19. TD 234091 - Stuck Prepay after transaction completed (merge from 23.992)
				Omer				20. TD 240409 - If Loyalty card is swiped first at pump and then credit, ADSLinkserver stays in Wait for Complete status (merge from 4.0.23.992)
				Omer			    21. TD 238845 - P@P sales with the temp item going into unknown department. (merge from  4.0.17.1290)
				Liat	05/07/11	22.	TD 224805 - PumpSrv should return an empty string in GetParam4 for a non valid grade. (merge from 1019)
				Gena				23.	TD 224543 - when perform Fuel Delivery and get abort in the middle of process,the sales_QTY was incorrect.(merge from 1019)
				Ella				24.	TD 242013 - Sales volume should start calculating only after pumps are locked and not when the process started (merge from 1019)
				Gena				25.	TD 236644 - PC_OutdoorBankIMP. Add <POSData OutdoorPositionStatus="Ready"/> to GradeResponse.(merge from 1019)
				Shuki   10/07/11    26. TD 250092 - Add density and weight to fuel delivery emulation xml (merge 1025)
				Omer	17/07/11	27. TD 216043 - Add codes for AVI and CashPaz zero trs.
				Ella	18/07/11	28.	TD 248476 - When pump status is PAK/Requesting and AllowPrePayOnRequesting parameter is turned off need to send OutdoorPositionStatus NOT READY to EPS on GradeRequest (Merge from 1019)
				Omer	17/07/11    29. TD 227394 - CSS: Shift log analyzer does not work in FCC. The shift log is opened by pumpSrv. It's kept opened so FCC can't open the file and display it on the shift log analyzer. To fix this issue we merged the fix for TD 166354 from 1024.
				Gena	21/07/11	30.	TD 243638 - PAN encryption records (eg 70c2) missing from Arval OPT transactions.
				Alex    24/07/11    31  TD 212899 - Added support to OLA EServices - NOT FINISHED
				OmerT	24/07/11    32. TD 254948 - Add Qdx empty and trs force remove alarms. Don't send alarm when trs is on PAID status.

  4.0.28.501	Ella	26/07/11	1.	TD 240006/251326 - Mobile Payment CR
				Omer	26/07/11	2.  TD 254219 - Send POS the active process when the system is in the middle of process. (On GPI::GetProcess)
				Omer	28/07/11    3.  TD 254959 - Add Qdx empty and trs force remove alarms. Don't send an alarm when the trs is on authorized status.
				Shuki	28/07/11	4.  No TD - increase size of m_sSysDownLogFileName to prevent memory overwrite. 
				Omer	28/07/11	5.  No TD - Add logs to buildRequest. 
  
  4.0.28.502	Omer    02/08/11	1.  TD 253789 - Density fields are empty in end delivery.
			    Omer    02/08/11    2.  TD 257668 - do not allow shift change in the middle single tank delivery.
				Ella	07/08/11	3.	TD 69518  - When there are two single tank deliveries in process, the accumulated sales for the second delivery is 0. (Merge from 1019)
				Omer    08/08/11	4.  TD 255105 - Pump Head has stuck Loyalty transaction after receipt prints. The trs was not clear.
				Omer	08/08/11    5.  TD 258652 - EndTankWeight is missing in the tra. - Set the end tank weight to the tag field. (merge from 1025)
				Omer    08/08/11    6.  TD 258646 - Fuel delivery unknown pumpsrv error code - Add logs (Merge from 1025)
				Omer    08/08/11    7.  NO TD - HandleSingleTankReguest - first parse the xml and only then check for middle of delivery process. (merge from 1025)
				Omer    11/08/11	8.  TD 258608 - Multi-Grade fuel delivery. Weight data before, after delivery and result are wrong (merge from 1025)
				Ella	15/08/11	9.	TD 258931 - Most recent delivery flag is not turned off after the delivery was finished.
									10.	TD 258976 - WetStockTime parameter was set in the registry instead of ini file
				AdirH	16/08/11    11. TD 260513 - PumpSrv was adding "19" at the beginning of the ExpirationDate for Y2K.
				Alex    18/08/11    12. TD 256429 - E2E Encryption. Add support to terminal "LogOn"  operation.
				Omer    29/08/11	13. TD 256101 - US Loyalty - transaction stuck on pump head when PAK not allowed. (merge from 124)
				OmerT   05/09/11	14. TD 264119 - US - LoyaltyPrepay transaction only rolls the premium price back to $0.426 instead of $0.109. (merge from 124)
				OmerT   06/09/11    15. TD 264116 - US - CR 157650 Car Wash Discount - Carwash + Prepay trs. TRS locked on the fuel side and could not be recalled/reconciled. Create GradePriceMinimumLimit parameter to the registry. (merge from 124)
				Ella	08/09/11	16.	TD 263774/264315 - In some cases in PrePay with loyalty discounts transaction status was not updated and therefore was locked when POS tried to pull it. (Merge from 1024)
				Liat	08/09/11	17.	TD 136062 - PIC trs is not reported correctly (merge from 1024)
									18.	TD 130693 - BP - Loyalty - P@P Partial Failure on Payment(merge from 1024)
									19. TD 127928 - BP Loyalty enhancement for Allied environment (merge from 1024).
				AlexM   08/09/11    20. TD 243954 - Q-dex Extra info sends error to Event Viewer fix: to prevent raise alarm in case that record not found in QDX on DELETE operation.
				OmerT	07/09/11	21. CR 233242 - Fuel auto-reconciles transactions (merge from 1023)

  4.0.29.0  	AmitH   15/09/11    1.  TD 257858 - GCI Set process - do not verify "in the middle of process/recovery" in case PROCESS_CHANGE_PRICES.
													in order to let the process end even in case all pumps are offline (server Startup).merge from 1028
				Liat	18/09/11	2.  TD 265566 - If a customer scans their Martin's Card and then swipes their Fleet card, the pump display goes blank.	(Merge from 1023)
				AmitH	20/09/11	3.  TD 254219 - continue change flag PROCESS_ANY value from 0 to 409
				Ella	22/09/11	4.	TD 263766 - Discount array was not cleared on PAP to PAK completion zero.(Merge from 1024)
				Ella	22/09/11	5.	TD 254219 - Send POS the active process when the system is in the middle of process. Change product in tank process was missing and emulation single tank process didn't work (Merge from 1019)
				AlexM   25/09/11    6.  TD 268644 - PAK - Protect against sending Auto Auth PAK to CL DOMS, in case it reported moving from DISPENSING to REQUESTING
	                                7.  Refactor: - moved protect auto authorized conditions to separate function IsPermitAutoAuthorize
				AmitH   29/09/11	8.  TD 266841 - CParam::SetParam3Level method wrote [Pumps\Pump01\Position100] instead of [Pumps\Pump01\Position1], thus causing the 
												    Scorpio nozzle totalisers to be read the same value all the time. (merge from 1028)
.               AlexM   02/10/11    9.  TD 268744 - OPW - Most Recent Delivery Failure while request second time from FCC. 
				Ella	04/10/11	10.	TD 247268 - Support Husky loyalty CR (merge from 1022)
				Ella	05/10/11	11.	TD 269203 - Pay inside from terminal is allowed even if max unpaid transactions have been reached
				Ella	11/09/11	12.	TD 256419 - When pump is READY / NULL_RES and maintenance not in process, in case pump is LOCKED, the state will be changed to UNLOCK.
				AmitH	20/09/11	13. TD 254219 - continue change flag PROCESS_ANY value from 0 to 409
				Ella	20/09/11	14.	TD 254219 - Send POS the active process when the system is in the middle of process. Change product in tank process was missing and emulation single tank process didn't work (Merge from 1019)
                AlexM   05/10/11	15. TD 264908 - Not updating other computer with the field "[Tanks\TankXX]\DeliveryReference" when using MultiSync.	
				Ella	05/10/11	16. TD 241916 - Shift was not changed correctly after stop all with lock. (Merge from 1019)

  4.0.29.10		Ella	06/10/11	1.	TD 256429 - E2E Encryption CR. Add pump number to login session sent to EServices.
						10/10/11	2.	TD 264032 - ePump Post Pay flows CR. (not finished)
                AlexM   11/10/11    3.  TD 264447 - POS00015861:For OPT transaction 60x5B record offset 7 – 11 is 0. (merge from 1028)
				Ella	17/10/11	4.	TD 271775 - Grey P doesn't disappear after recall transaction. (merge from 1024)
				OmerT   18/10/11    5.  TD 270145 - Fuel does not send Price to LPE that matches the Service Mode of the Pump (merge from 1023)
									6.  TD 270124 - If RewardAmountNeedCashierConfirmation = 0 on Hose Position 1, Approval Message prompt is skipped (merge from 1023)
									7.  TD 265281 - (Cop!) Wesco: Problem with cents burned up to the discounted fuel price. (merge from 1023).
									8.	No TD - Offset of 99 need to be added for a few specific GPI codes, because of GPI_IN_MIDDLE_OF_QDX_MAINT that is equal to 200 in 1019 and starting from 1021 it's 101. Need to make the offset so the BOS will get the same retcodes.

  4.0.29.500	Ella	03/11/11	1.	TD 245565 - OPW Reconciliation Support, support new section and parameter in RunTime ini - TankSrv\SequenceNumber
						06/11/11	2.	TD 275511 - Reprint receipt from POS doesn't work (Merge from 1022)
				AlexM	08/11/11    3.  TD 277379 - protect from duplicate ControllerSeqNumber in PAK recovery. 	
				AlexM	08/11/11    4.  TD 276618 - Remove the redundant key TankMinVolumeLimit in GeneralParam for tank min volume
				AlexM	10/11/11	5.	TD 276913 - Reprint receipt from POS didn't work (Merge from 1023)
	            AlexM   10/11/11    6.  TD 264908 - added VP recovery.
				OmerT   02/11/11    7.  TD 276656 - US - Loyalty - Card Data not masked in VP.log when used with loyalty card (merge from 124)
				OmerT	11/11/11    8.  TD 278992 - US - Loyalty - In an outside trs(with loyalty), if press no to reward, it locks the loyalty card. (merge from 124)
				Ella	21/11/11	9.	TD 273171 - Can't Auth PAK FillUp Trs (Merge from 1022)
				AlexM   27/11/11    10. TD 264908 - Send new ALARM_VP_IS_DOWN when the PumpSrv failed to VP recovery. 
				AlexM   27/11/11    11. TD 281609 - Return GCI_TRS_DUPLICATE_UPDATE in PAKReadyTransaction if duplicate ControllerSeqNumber in PAK recovery.
				Ella	01/12/11	12.	TD 248476 - CheckAndReserve for Outdoor payment didn't work properly (Merge from 1019)
				OmerT	18/12/11	13. TD 283329 - Many $0 Failed completions at Beta site.(merge from 1023)
									14. TD 258004 - Pump hangs in IN_Progress when transaction canceled. (merge from 1023)
									15.	TD 284709 - POS auto-reconciles- merge from 1019.
				OmerT   20/12/11    16. TD 243282 - Fixed vapor recovery registry key to VaporRecoverySupport.
				Liat	22/12/11	17.	TD 278543 - Controller sequence number was missing in prepay with xml when TrsClearedByPOS key is set (merge from 1024)
                AlexM   22/12/11    18. TD 287958 - Fixed Not updating other computer with the fields "[Tanks\TankXX]\" {StartDeliveryVolume,StartDeliveryTemperature,EndDeliveryVolume and etc} when using MultiSync.(merge from 1025)
				OmerT   20/12/11    19. TD 287153 - When trying to pay a PAK transaction the system added a huge amount. (merge from 1023)
				Liat	27/12/11	20. TD 287252 - Single tank delivery process wasn't completed when pump was exclusively locked (merge from 1019).


  4.0.29.501	OmerT   03/01/12	1.  TD 211528 - Extended number of languages from 4 to 6. (merge from 1022)
				AlexM   03/01/12	2.  TD 288369 - MultiSync: Server not switched when failed to write to QDX. (merge from 1025)
													added support for extension server state: VP is failed, QDX error.
				Liat	08/01/12	3.	TD 291147 - Debit or Credit Prompting for injected screen (merge from 1023).
				AlexM	08/01/12	4.	TD 290905 - MultiSync:  FCC: Field of "VpRecoveryRetries" is missing in FCC. 
				AlexM   11/01/12    5.	TD 290189 - Fuel price change stuck with status "Received by Fuel". Problem: Alarm "Price ended" is not sent. not enough space in the Alarms cmd queue.
													Fixed: increased AlarmSrv cmd queue. (merge from 1028)
  4.0.30.0		Liat	18/01/12	1.	TD 292569 - Grey "P" are not removed on the pumps after performing pre-pay transaction - Error NOT_Found (1) should be ok for delete (merge from 1024). 
				OmerT   26/01/12	2.  TD 211528 - Rollback number of languages from 6 to 4. (merge from 1022)
				AdirH	??/??/12	3.  CR 292502 - Clean Energy - Safety Prompts enhancements.
				Liat	07/02/12	4.  TD 270322 - PAP Receipt Reprints from PosPump show "Original" (merge from 1124)

  4.0.30.1      OmerT	15/02/12    1.  NO TD     - Remove unnecessary exception write to the log.

  4.0.30.40     IraB    22/02/12    1.  TD 300280 - Receipt Prints as "Original"  twice from Terminal.\
				OmerT   01/03/12    2.  TD 298855 - PC,Support Tank Volume Reading When All Pump are offline.
                AlexM   18/03/12    3.  TD 277030 - CSS Site Spb 013: Only first PAK transaction appeared at POS, second PAK transaction is missing SR#268644.
	                                                (merge from 1022 TD#303934)   
				Liat	19/03/12	4.	TD	??- SPOT - Support a new field sScreenMsgNumber (merge from 1022).
				OmerT   21/03/12    5.  TD	       -  Pump authorizes to value site limit  in PAK while in FCC selected preset to volume site limit	
				
			    OmerT   29/03/12    6.  TD 309771 - PAK is impossible while selects Site Limit by Volume in FCC
			    Liat	05/04/12	7.  TD 310103/ 310218 - Stuck trs when canceling PAK with loyalty trs (merge from 1023).
									8.  TD 299992/ 308922 - Pump has 'in progress' transaction when canceling in the middle of authorize (merge from 1023)
									9.  TD 306140 - POS does not get  Member card on PAK  (PumpStart transaction)(merge from 1023).
				
				Haimk   14/05/12   10.  TD 317615 - Track2 displays in Pumpsrv log during  PAP authorization.

  4.0.30.500	IraB	17/05/12    1.  TD 316205 - Mismatch Media type between PumpSrv and RFS due to cancel in the middle of authorize.
			    Shuki   10/06/12    2.  TD 321138 - LockForeCourt was turned on when shift change was performed while server in maintenance state (merge from 1027.130)
				OmerT   11/06/12    3.	TD 321602 - Loyalty Barcode details are not passing to VP
				Shuki   17/06/12    5.  TD 320068 - Support slow authorizing pumps - add flag IndicateAuthorizePumpSen
				AlexM   19/06/12    5.  TD 322185 - DOMS:Spring: Extra PAK transaction arose at POSPump panel by unknown reason (merge from 1025)
				Liat	25/06/12	6.  TD 323583 - PAK with Pump Start and discount available is canceled (merge from 1023).
				AdirH   05/07/12	7.  CR 291185 - MTX FuelEPS Interface.
                AlexM   10/07/12    8   TD 325958 - PrePay reconcile issue. fixed so PumpSrv will check max volume or value base on the limit type the prepay was created with (merge from 1025)
				OmerT   31/07/12    9.  TD 275612  - BP US  - Remove stuck trs to allow insert of cash acceptor trs.

  4.0.31.0	    OmerT   26/07/12    1.  TD 325761 - Prepay Move while nozzle lifted - in this case auto authorized for PAK value after stop pump (Refix)
			    Shuki   06/08/12    2.  TD 326234 - When changing mode to all pumps, pump 0 was sent to alternate server preventing updating the modes in the alternate memory
				OmerT   07/08/12    3.  CR 329049 - Fuel: Support changes for FRONT authentication
				OmerT	19/08/12    4.  TD 331900 - Timed Out PrePay becomes AutoAuth'd as a PAK
				OmerT	30/08/12    5.  TD 332329 -	Club Id not recorded in VP and PMT Trs.
				AdirH				6.  TD 333508 - PosPump not showing rolling value for P@P on Gas pumps when loyalty LPE is turned on.
				Shuki   24/09/12    7.  TD 324520 - Canceling CheckAndReserve for preset leaves pump in requesting state
				Liat	24/09/12	8.	TD 274401 - Support EMV- RFS flow.
 
  4.0.31.500	Liat	08/10/12	1.  TD 274401 - US integration EMV- RFS flow.
				AdirH   24/10/12    2.  TD 337636 - Pumps locking randomly as well as troubles getting all pumps to lock/unlock at the start/end of day.
				AlexM   13/11/12    3.  TD 341239 - Wrong waste receipt.
				AlexM   22/11/12    4.  TD 342402 - US - WoR site that had site controller unavailable where a reboot of POS 1 resolved the issue.(merge from 1024)
				Liat	05/12/12	5.	TD 334724 - Transaction started and completed immediately so CL rejected the PAPSwipeResult causing transaction to become stuck (merge from 1023)
									6.  TD 346929 - Not able to use PAK after swiping member card
				Shuki	06/12/12    7.  TD 335186 - FlexiLane outdoor
			    OmerT   19/12/12    8.  TD 343588 - pmt transaction open failure
				OmerT   19/12/12    9.  No Td     - Print the total reward received in the trigger prom xml
						23/12/12	10.  TD 348622 - PC_PumpTotalsLogicChangeForRFS - support pump reading in EOS for IFSF Pump
				Liat	25/12/12	11.	TD 342701 - Unable to recall PIC cash prepay that was canceled at PIC (merge from 1124).
									12.	TD 342690 - Prepay timeout should not be applied for a cash acceptor trs (merge from 1024).
				OmerT   26/12/12    13. TD 348577 - fuel failed to authorize pump vendor
						06/12/13    14. TD 349356 - FuelSvr did not send promotion details to EPS.
				Liat	21/01/13	15.	TD 349844/ 350483/ 352587 - BP US WOR - Pumps showing random transactions of trs num = 0 and value > 0. (merge from 1124)
				OmerT   04/02/13    16. TD 353515 - RFS: do prepay at pump ,but can't pay.
				AdirH	06/02/13	17. TD 355138 - PosPump not showing Company Name.
				OmerT   12/02/13    18. TD 332389 - Expose Full UDF Gift Card Number in Section 21 of Daily Trans File (Continue)
				OmerT   03/03/13    19. CR 327153 - Tripe A
				OmerT   14/03/13    20. TD 356629 - Gilbarco PMT-Pump get authorized and start dispensing Automatically
				OmerT	17/03/12	21. TD 359309 -  Pump authorizing without hitting Postilion for $0 (merge from 4.0.12390.2500 - pilot)
				Maxim   21/03/2013  22. TD 274401 Reprint Last receipt
	            AlexM   09/01/13    23. TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state. (merge from 1025)
				AlexM   12/03/13    24. TD 338501 - Unable complete Pump Test transaction.
				OmerT   24/03/13    25. TD 339257 - PayPal.
				AdirH				26. No TD     - PumpSrv was removing the Flow Control messages in case of a space or a '|' in the beginning of the message.
				AssafW  07/04/13    27. TD 358952 - Sasol: Stuck transactions on POS related to thread safety around CPumpStatus
				MaximB  10/04/13    28. TD 344747 - Pospump missing Audible voice when new PAP transaction started

				
				// merge from 1023:
				Omer	16/01/13    1.  TD 351703 - Promo ID is missing in PAK with Loyalty flow.
				Liat	08/01/12	2.  TD 276886 - Account number was cleared before complete when canceling ticket.
						11/01/12	3.  TD 273210 - Account number is masked in complete with PCI (merge from 1017).
									4.  TD 252311 - PrePay trs status will be changed to WAIT_TIME_OUT when ready, null res status is raised
				Liat	11/04/12	5.  TD 310599 - CancelTicket wasn't sent when payment card was declined by OLA.
				Omer    28/05/12    6.  TD 319058 - LRE PAK-POS prompts for Loyalty after LRE card swiped at pump & discounted fuel
				Liat                7.  TD 326452 - Fuel sends session complete on PAK Transaction
				Liat	08/01/12	8.  TD 276886 - Account number was cleared before complete when canceling ticket.
				Marik	25/01/12    9.  TD 300622\301912 - Fleet card does not display the text for the Odometer prompt if Loyalty is swiped first.
				Marik	07/03/12    10. TD 305131 - PAK with discount doesn't stop at the lowest limit.
	            Marik   21/02/12    11. TD 308902 - PAK fuel sale with a discount is not sending back the original price. 
				Marik	13/04/12	12. TD 312695 - Sequence number is being created w 6 digits unexpectedly
									13. TD 310220 - Fuel  should encrypted account number in field sEncryptedAccount.
				Marik	19/04/12	14. TD 314063 - Customer swiped loyalty card at pump and after that pump gets stuck in loop.
    			Marik	27/04/12	15. TD 314959  - If the cancel button is pressed at certain times during the prompting process at the Pump during a Loyalty PAP transaction
									16. TD 310155  - Account number send incorrectly when working with encryption DPS 	
									17. TD 316024 - LRE - Loyalty Swipe for P@P and PAK isn't sending the Loyalty info to PaymentSrv
				Marik	24/05/12	18. TD 318539 - Pump does not prompt for Manual Entry
				Marik	30/05/12	19. TD 319514 - Convert Layer sends Cancel on manual entry of member card
				Marik	30/05/12	20. TD 320504 - LRE - Fuel need to send GasLoyaltyDiscountPerGallon for all nozzle positions
				Marik	30/05/12	21. TD 321469 - LRE - When doing PAK with Loyalty Card at the pump transaction will not stop at 20 gallon
				AdirH	22/06/12	22. TD 323545 - LRE - Record 12 Must Have ADJ or Discount Value per pump.	
				Liat	03/07/12	23. TD 325089 - PumpSrv does not pass LoyaltySummary xml to ExtTrs on PAP (undo merge for issue #321602).
				Marik   05/07/12	24. TD 329584 - COLAConnectedPayments: - End session send incorrectly. 
				Marik   05/07/12	25. TD 326258 - In night mode pump auto authorize pay inside.
				Marik   09/08/12	26. No TD  Pump authorized without request payment card	
				Marik   31/08/12	27. TD 333745 - PumpSrv report wrong batch number on price change
				Omer	05/09/12	28. TD 332488 - When using Loyalty points at the pump only cannot use Wesco Stored Value Card as payment 
				Marik   18/10/12    30. TD 338379 - Credit or Debit Card not authorizing after Loyalty Card swipe at the pump.
				Marik   17/12/12    31. TD 321885 - UDF Loyalty Alt ID
				Marik   20/12/12    32. TD 348868 - Balance on a loyalty card is being removed without the customer receiving the discount on their fuel
				Liat	31/12/12	33. TD 349768 - Loyalty stuck transaction, when canceling in the middle of authorize. (4.0.23.1661)
				Marik   01/10/12	34. TD 329408 - Costco receiving incorrect processing codes in bit 3 when using gift cards (4.0.23.1570)
				Liat	20/01/12	35. TD 349003 -  Hy-vee member card not on receipt
				Liat    20/01/12    36. TD 347668 - Stuck prepay in the middle of PAK with loyalty (TD 352598)
				OmerT	15/12/11	37. CR 155754 - Phone Number CR
			    Liat    21/02/12	38. TD 302792 - Fuel Pump Start with loyalty - Cont.- cancel trs for non pump start card.
				Marik	02/03/12	39. TD 302788 - CPG points are locked and not burned with a PAK transaction. (4.0.23.1211)
				Marik   20/01/12    40. TD 294628 - PAP completion failed as result of wrong OLA tag ItemPriceLow0[S] (4.0.23.1170)
				Marik   18/10/12    41. TD 338379 - Credit or Debit Card not authorizing after Loyalty Card swipe at the pump. (4.0.23.1591)
				Marik   18/01/12    42. TD 292831 - PumpSrv will clear message to display on need more param when response return from controller.


				OmerT   23/04/13    43. TD 274401 - Costco Canadian tax - Move parameter to HKEY_LOCAL_MACHINE\SOFTWARE\Pointofsale\PumpSrv\GeneralParam\TaxRegulation
				MaximB  23/04/13    44. TD 356420 - Credit data found in 'C:\Office\Pumpsrv\Log\RFS\Exception001.DMP'
				AssafW	02/05/13	45. CR 351687 - Enhanced post void transactions
				Liat	06/05/13	46.	TD 365865 - Carwash discount cent off  is not reporting to VP - Support a new OLA field (merge from 1124).
                Alex    19/05/13    47. No TD     - Add ability to remote HW reset terminal.
				Haimk   29/05/13    48. No TD     - Change define of FINALIZE_PREPAY_CREATE_RECEIPT_REC to 2.(Releaseed to PC DEMO as 4.0.31.9991)
	            AlexM   25/06/13    49.  TD 367952 - 131-Trs cancel after rolling.(merge from 1022)
				AmitH   30/06/13    50. R10 User Story - 71146 - Fuel P@P- Swipe MSR card at the pump and start fueling - for any card (Add new OLA type : OLASRV_EPS)
				OmerT   01/07/13    51. TD 363041 - LNG Sales
				AmitH   11/07/13    52. No TD - m_lTankID was not captured in PakReadyTransaction.
				AmitH   16/07/13    53. R10 User Story - 76018 - On PapReadyTransaction, Copy nozzle to cardsaledata, and export it to OLA buffer on Complete. ( example: Nozzle[I]=3 )
				NahumN  31/07/13    54. CR 356194     -  Upload FuelDleveryDriver Id in daily 
				OmerT   07/08/13    55. TD 373856     -  Multiple DPS on ongoing
				AssafW	07/08/13	56. TD 371850	  - PCI violation: Exception includes card information (Merges from RFS TD 356420)
				OmetT   20/08/12    57. TD 378403		Prepays stopping at $0.04 on all pumps (merge from  4.0.23.1960)
				OmerT   25/08/13    58  TD 357023	  - UDF gift card locked at Pump (4.0.23.1970)
				AlexM	26/08/13	59. TD 378454     - Prepay Recall triggered FuelSaver discount again. (merge from 1023)
				AmitH   29/08/13    60. R10 User Story - 80969 - On PapReadyTransaction, Copy TransFlowRate[I] , StartFuelingDateTime[D], EndFuelingDateTime[D] to Ola
				AlexM	02/09/13	61	TD 377336 - Fuel Delivery Emulation Extensions.
				AlexM	12/09/13	62.	TD 379474 - PumpSrv runtime error. Fix: change cast of _bstr_t from (char*) to (const char*). 
				OmerT   16/09/13    63. TD 380760 - reuturn GPI_TRS_ALREADY_IN_PROGRESS if the prepay is in progress in GetPrePaidTrs2__
				AlexM	24/09/13	64.	TD 381216 - PAK Recall shows wrong amount
				AlexM   24/09/13    65. TD 381791 - PAK with Loyalty is recalling double discount. Fix: use PAK_INCLUDE_EXTRA_INFO_REC as a flag.
				OmerT	03/10/13	66. CR 295237 - Stuck fuel reward (United Supermarkets)
				OmerT	03/10/13	67.	CR 305114 - Fuel Discount based on Tender (United Supermarkets)
				OmerT   14/10/13    68. TD 381449 - Stuck In-Progress and Stuck Prepay with Allied GGA6004 REV5 update
				AmitH   17/10/13    69. R10 User Story - 92780 - Fuel P@P - ext. point to disable PAP button if card provider is offline. (Add PROCESS_OLA_ONLINE PROCESS_OLA_OOFFLINE process)
				OmerT   21/10/13    70. TD 384668 - PumpSrv didn't complete transaction
				AlexM   04/11/13	71.	R10 User Story - 92780 -  Fuel P@P - Ola Recovery.
				OmerT	05/11/13	72. TD 385971 -  Pay At Pump Showing Total Sale as $0.00 even though fuel was dispensed and customer charged. solution: cancel in the middle of authorize is per pump and not global
				OmerT   05/11/13    73. NO TD     -  Support timeout for StartTicket, AddMember, AddItem and EndTicket events from loyaltyFuel - timeout for StartTicket and AddMember is 30 seconds Timeout for AddItem and EndTikcet is LoyaltyTransactionCompleteTimeout
				OmerT   12/11/13    74. CR 385254 - Support MaxPumpFlowRate
				AlexM	13/11/13	75. TD 347994 -  UDF - Debit transaction error on completion. implement PCI decoding DSM.dll  (merge from 1023)
				OmerT   17/11/13    76. TD 379709 - RFS:can't do fuel trs at pump
				AdirH	03/12/13	77. TD 389177 - Pumpsrv able to run twice.
				AlexM	08/12/13	78.	TD 384225 - VP crashing with "GPI_FAILED_IO OR  GPI_FAILED_PREPAY_IO Terminate APP"	
				AlexM	08/12/13	79.	TD 389915 - PumpSrv should better handle XML when MSXML has exceptions.  
				OmerT	08/12/13	80. TD 387091 - Fuel is not sending the complete authorization # back
				OmerT	08/12/13	81. TD 389050 - SmartPay Fleet Cards - Odometer padding issue
				AdirH   12/12/13    82. TD 387091 - ReFix - Fuel is not sending the complete authorization # back.
				Haimk   26/12/13    83. NO TD     - Support PumpTest password flow.
				AssafW  09/01/14	84.	SR 362184 - PumpSrv freeze on COLABase destructor
				OmerT   16/02/14    85. TD 389480 - Husky - Rewards tender incorrect on P@P
				AlexM	18/03/14	86	R10 User Story - NNNNN - increase filler from 1019 to 2043 (merge from 1028_R10)
				OmerT   23/03/14    87. TD 396017 - CL:Do eos ,it take a long time to end shift of fuel.
									88. TD 396019 - CL:do eos ,fuel didn't send 3D to some pumps.
				OmerT   24/03/14    89. TD 398329 - Transaction Pak with loyalty doesn't contain loyalty information
				OmerT				90. TD 396530 - Pumpsrv.log rolled in the middle of the day causing a crash
				OmerT               91. CR 295237 - Display the maximum deferral in manual deferral screen (%s should be used in the flow control message in FCC).
				AdirH				92. TD 394562 - PCI compliance issues on Canadian debit P@P transaction.
				AdirH				93. TD 395907 - TA : Convert Layer losing connection with Allied.
				OmerT               94. CR 370688 - Auth MPD by Product.
				OmerT				95. CR 305114 - Force multiple promotions to LPE.
				AlexM	25/05/2014	96.	No TD	  - Tank Readings Issue. improve loading TankRead record. (merge from 1028)
				AlexM	25/05/2014	97	No TD	  -	support Petrotec TCP-IP pump.
				OmerT   25/06/2014  98. CR 393826 - Pricing with Price Pole Signs.
				OmerT   25/06/2014  99. TD 403075 - New message at Fuel for "Stacked Fuel Rewards" does not show the Members available amount at the Pump. (manual deferral was not displayed in case there were 2 promotions and the 2nd one is not manual deferral promotion).
				OmerT   25/06/2014  100.TD 406362 - (Martin's) Fuel hanging at the pumps
				OmerT   25/06/2014  101.TD 402427 - Unable to dispense approved prepay fuel on the pump
				AlexM	26/06/2014	102.TD 380607 - UDF- Multiple issues with pumps showing out of service- Support loyalty receipt flags. (merge from 1023)
				NahumN  29/05/14    103 CR 369122 - PAZ Update RFS Registry Key By Change Shift
				AdirH   07/11/14    104.TD 409416 - Interface to Excentus Reward Marketplace - Incorrect ReferenceId in RedemptionRequest.
				AdirH	07/11/14	105.TD 404430 - Mosaic fails on StartSession when converting fuel from Allied to RFS.
				OmerT   22/07/14    106.CR 409564 - Extend Tank Delivery information for PetroChina.
				AdirH   25/07/14    107.TD 410631 - Under-dispensed Loyalty transaction has incorrect Tender Reported.
				AdirH			    108.TD 410829 - Limits Mapping issue on MPD (2-wire) environment.
				MaximB  01/07/14    109.TD 403522 - Add Magnetic Stripe and Contact EMV Support For MasterCard
				MaximB  08/07/14    110.TD 403522 Cont.	- Add Magnetic Stripe and Contact EMV Support For MasterCard 
				MaximB  23/07/14    111.TD 403522 Cont.	- Add MasterCard EMV PAN checking  
				ShimiP 06/08/14		112.TD 411428 - 1.EXTRA_INFO_REC.info.sCarWashProgram was assigned as l2a instead of a memcpy (byte to byte).
									113.NO TD     - EXTRA_INFO_REC.info.lCarWashAmount needed to be in 10 factor
				NahumN  13/08/14    114.CR 369122 - Support 3rd Party reconfigure request by setting a registry
				ShimiP  31/08/14    115.CR 404591 - suncor enable\disable price display
				OmerT   01/09/14    116.CR 404911 - Succor Provide support for 3 decimal places under tax rates
				AdirH   04/09/14    117.TD 413477 - Husky - Rewards points are not being redeemed when the customer elects to redeem points.
				AdirH	09/09/14    116.NO TD     - PumpSrv needs to create the "ManufacturerName" key on registration.

	4.0.31.510	OmerT   22/09/14    1.CR 404911 - Succor Provide support for 3 decimal places under tax rates  

	4.0.31.511	OmerT   22/09/14    1.CR 404911 - Succor Provide support for 3 decimal places under tax rates
	
	4.0.31.512	Marik   10/01/14    1.QC 414628 - Fuel is not sending cancellation request after cancellation or timeout of a Pay-at-Kiosk transaction.
	
	4.0.32.000
		1. Official release
		2. AdirH   03/10/14    1.TD 415542 - TerminalSrv buffers to AJB/EPS contain outdated grade price info after performing a price change.
				**************** Formal 1032 release  ********
	4.0.32.010
        		AlexM	22/10/14    1.  CR 405462 - Send Meter Read when Price Changes
				ShimiP	22/10/14	1.	CR 407467 - added support to configuration thread to be able to handle multiple data entries in FuelConfiguration.xml files.
				OmerT   22/10/14    3.  No TD - Fix a memory leak in CreateXMLFromStruct – Use const OLECHAR * for initialization of CComBSTR objects.
	4.0.32.020
				ShimiP - CR 407467 - added an alarm to raise if error occurred while processing the xml
				ShimiP - CR 404903	- Support display order for car wash programs
				ShimiP - NO TD - in CheckIfStrIsReal if the str was not found the index returned was -1 and that caused crashes
				OmerT  - CR 396348 – Support a dynamic buffer when creating a PAP xml.  
				OmerT  - No TD - Fix an issue that caused pumpsrv to crash while trying to encrypt a pap card's account number.	
				OmerT  - No TD - Update loyalty swipe result commands to work with struct instead of xml.
				AlexM  - No TD - Update MSXML interfaces to fix a memory leak.

	4.0.32.021  OmerT  - TD 418715 - Tax details are missing from VP Tra.

	4.0.32.100  OmerT  - Drop 1: Release for Suncor.

	4.0.32.101  OmerT  - Fix an issue that caused an non-loyalty pap trs to get stuck.

	4.0.32.102  AlexM	 	TD 419618 - PFS - Pump Icons Coming up with  Lock Symbol after q-load every monday morning.	Added Exception handler to ConfigurationThread::Operation
				OmerT   CR 404913 - Allow tenders in a sequence (3rd Phase).
				AlexM	CR 418937 - Fuel to Support new EpsilonLinkSrv connection
				ShimiP	CR 407467 - delete a file that cant be processed to another folder raise the alarm if the file could not be processed.

	4.0.32.150  OmerT   CR 404913 - Allow tenders in a sequence (4rd Phase).
				ShimiP	CR 407467 - add support to non valid fields received in the xml
				AlexM	TD 422282 - SUNCOR CR 405462 - Fuel : PumpReading not trigger when cashier change price in BOS.

	4.0.32.200  OmerT   CR 404754 Print Loyalty Messages On The Fuel Receipt.

	4.0.31.201  OmerT   TD 425138 - Terminal stuck on PAP transaction cancellation.

	4.0.32.210  OmerT   CR 404754 Print Loyalty Messages On The Fuel Receipt - Support barcode description and endDate.

	4.0.32.211	ShimiP	CR 404902 Alert cashier if there are unpaid pak and swipe was made in terminal (new alarm 0202005)

	4.0.32.220  AlexM	CR 405435 Support Linked Card Encryption. (support CARD_TYPE_CIBC)
				AlexM	TD 426940 Suncor - CR404752 - Unable to change fuel prices from BOS. added protection against using PriceChangeLock && PriceChangeTotalsRequired					
				AlexM	TD 426830 SUNCOR - P@P transactions don't get to VP. fixed "sLanguage" from BYTE to DEC.
				OmerT   CR 409107 Language Selection Support for Loyalty Messages. 
				OmerT   CR 405631 Display on receipt the updated Petro-Points balance at the end of each transaction
				AlexM	TD 426985 - Suncor - CR404591 - Price updates and displays new price when price change is initiated and price sign is in a blank status.


	4.0.32.230  OmerT   NO TD     - Extend Card Sale data
						CR 404910 - Support different FSR cards in the same transaction (not finished).
						CR 405461 - support CCM device (send details to VP)

	4.0.32.300  Marik	TD  429607 - SUNCOR - FCC crashes when Credit Card is inserted in Full Serve Pump
						Drop 3 - Beta version.

    4.0.32.320 OmerT    No TD      - Handle cases where corrupted xml received on car wash promotion message fetch from LPE.
						No TD      - Extend result message to 200 bytes.

	4.0.32.321 OmerT    No TD	   - Support BarcodeType.
						No TD	   - Open ONMP_YesAsEnter (ok key) in pre authorize screen.
						No TD	   - Fix car wash price on pre-authorize screen.

	4.0.32.322 OmerT    No TD      - Add car wash type to car wash details.

	4.0.32.323 OmerT    No TD      - LPE receipt messages was counting blank lines resulting not sending all zones to Generator.

	4.0.32.324 OmerT    No TD      - Fix keyboard mask and max keys on pre-authorize screen.

	4.0.32.330 OmerT    No TD      - Change car wash unit price according to the HKEY_LOCAL_MACHINE\SOFTWARE\Pointofsale\PumpSrv\CarWash\ScaleFactor

	4.0.32.330 OmerT    No TD      - Fix issues in the pre-authorize screen.

	4.0.32.350 OmerT    CR 407618   - Suncor Energy Services - Support Fuel Terminal and Fuel Receipt Messaging

	4.0.32.360 OmerT    CR 404908 -  Allow acceptance of Petro-Canada prepaid cards at CRIND. 

	4.0.32.361 OmerT    CR 404908 -  Allow acceptance of Petro-Canada prepaid cards at CRIND. (fix some issues)

	4.0.32.362 OmerT    CR 404908 -  Allow acceptance of Petro-Canada prepaid cards at CRIND. (Added items data on compilation)
						TD 437218 -  When Loyalty down, error message display until message reach to timeout

	4.0.32.363 OmerT    CR 404908 -  Allow acceptance of Petro-Canada prepaid cards at CRIND. (Added more data to complete session)

	4.0.32.370 OmerT    No TD     -  Send decline to RFS when CARD_TYPE_INVALID (999) received in GetCardInfo
						CR 404908 -  Added more required fields to complete session
						CR 404908 -  Fuel did not send the second tender's track 2.

	4.0.32.371 OmerT    No TD     -  fuel did not send the second tender's track1 and track2
	
	4.0.32.372 OmerT    No TD     -  Don't send account number of second tender to EPS

	4.0.32.380 OmerT    No TD     -  Start Transaction items that are sent to epsilon from index 0 
						No TD     -  Don't send "legacy" Transaction items if "TwoTendersInTransaction" is set to 1
						No TD	  -  Print loyalty barcode number.
 
	4.0.32.390 OmerT    No Td     - Moved pre-authorize screen to be handled by RFS
								  - Removed sResultMsgExt from card sale data 6.

	4.0.32.400 OmerT    1. TD 431756 - Suncor - Pay-at-Kiosk transaction does not show up on POSPump after Post Void.
					    2. No TD     - Suncor - Fixed issue with track2

	4.0.32.410 OmerT    1. CR 404465 - Tender based UOM promotion - Limit condition by Tender amount
						2. No TD     - PreAuthorize limit was not saved in case FSR card was swiped prior to payment card.
						3. No TD     - Handle unauthorized card in partial payment flow.

	4.0.32.420 OmerT    1. TD 441311 - Pump Stopped Fueling on incorrect limit.
						2. No TD     - Partial approval - support continue with authorized amount.

	4.0.32.421 OmerT    1. TD 441312 - PAP missing zero completion.  

	4.0.32.422 OmerT    1. No TD     - Support CARD_TYPE_LOYALTY_AND_CREDIT (mediaMasterType 1)

	4.0.32.423 OmerT    1. NO TD     - Send RFS screen numbers in need more param sessions.
						2. No TD     - Fixes in Epsilon interface.

	4.0.32.424 OmerT    1. No TD     - Fixes in Epsilon interface.
						2. No TD     - extend registry fields from 256 to 512 bytes.

	4.0.32.425 OmerT    1. TD 442279 - On a Partial Approval transaction - Press cancel should terminate transaction. 

	4.0.32.426 OmerT    1. TD 442400 - ItemCount[I] Missing on report items.
						2. TD 442278 - FSR screen is missing few details
						3. TD 442280 - Error handling on FSR flow.

	4.0.32.427 OmerT    1. No TD     - PumpSrv did not send complete in case FSR swiped twice and trs canceled.
						2. No TD     - Reduce FSR total amount from pInfo->CardSaleInfo.extraData.sTotalAmt (total amount in the receipt).
						3. TD 442280 - Error handling on FSR flow

	4.0.32.428 OmerT	1. TD 442280 - Error handling on FSR flow

	4.0.32.429 OmerT	1. TD 442280 - Support multiple taxes for car wash item.

	4.0.32.431 OmerT    1. TD 443187 - When saying no to car wash fuel is still sending a car wash Item.

	4.0.32.432 OmerT    1. TD 442282  - Support car wash taxes in the receipt.

	4.0.32.433 OmerT    1. No TD      - Fsr card amount paid was not saved to the receipt. 

	4.0.32.434 OmerT    1. TD 442281  - Receipt  payment message doesn't display on receipt

	4.0.32.435 OmerT    1. TD 442281  - Receipt  payment message doesn't display on receipt

	4.0.32.436 OmerT    1. CR 424478   - EFT Phase 7_Logon Log off.
						2. No TD       - Send car wash plu code to loyalty according to the key HKEY_LOCAL_MACHINE\SOFTWARE\Pointofsale\PumpSrv\CarWash\CarWashCode

	4.0.32.437 OmerT    1. No TD       - If payment card was declined zero complete in case FSR was authorized.

	4.0.32.438 OmerT    1. TD 443183   - Fuel is not displaying RoutrSrv's decline messages when DisplayOLAResultMessage == '1'

	4.0.32.439 OmerT    1. No TD       - Get car wash tax name from card sale data.
						2. NO TD       - change open need more params max keys to 1 for car wash promotion

	4.0.32.450 OmerT    1. No TD       - Change log members to CString

	------------------------------------------------------------- Drop 3a-----------------------------------------------------------

	4.0.32.460 OmerT    1. No TD       - car wash plu code: get the code according to the device type(ccm and non-ccm sites)

	4.0.32.465 OmerT    1. CR 405282   - Support Tender Based Promotions from LPE
						2. CR 404753   - CRIND to supports communicate 1:1 message from Loyalty (car wash promotion)

	4.0.32.470  OmerT   1. CR 404753   - Print car wash and tender promotions.
						2. NO TD       - Concatenate loyalty messages to VP xml.

	4.0.32.471  OmerT   1. No TD       - Fix buffer overflow in partial a405462pproval flow.
						2. No TD       - Removed grade res405462triction in pre-authorize details sent to Epsilon.

	4.0.32.472  OmerT   1. No Td       - PartialApproval value was overunned.405462

	4.0.32.474  OmerT   1. NO TD       - change partial approval & carwasg promotion keyboard setting.

	4.0.32.600  PolinaA 1. CR 404913   - Allow tenders in a sequence - Tender cards data added to XML for VP.
				OmerT   2. No TD       - Send car wash (AddItem) unit price with 2 digits after decimal point to LPE
						3. No TD       - Send TransTaxAmount tag to epsilon
						4. No TD       - Add support in preset from terminal

	4.0.32.601  OmerT   1. No TD       - ConvertStringsToLong method was not outputting correct value
	4.0.32.602  OmerT   1. No TD       - Authorize the card based on sPreSetlimit (user's limit) or lPreAuthorizedAmount (card's limit)

	4.0.32.603  Shahaf  1. No TD	   - sent ShiftNumber on Authorize and Complete.
				
	4.0.32.604  Shahaf  1. No TD	   - added EMVExtendedBuffer flag

	4.0.32.605  PolinaA 1. TD 429143   - Some default registry keys not created & not displayed in FCC

	4.0.32.610 OmerT    1. No TD       - Merge fixes form drop 3a.

	4.0.31.611 OmerT    1. CR 405435    - change ClubCardHashNumber[S] tag to ClubCardNumberHash[S]

	4.0.32.612 MaximB   1. TD 454500   - Suncor - Using Reprint function on POSPump causes Fuel to crash.
			   ShimiP	2. CR 428562   - Suncor - in pre authorize screen allow the customer to enter a different amount.
			   OmerT	3. No TD       - Merge from 3a.

	4.0.43.613 OmerT    1. CR 405435   - Extend ClubCardNumberHash[S] tag to 64 bytes. Send AddMember for Linked card.

	4.0.32.614 ShimiP   1. TD 454422    - CPL not calculating correctly for PAP
			   OmerT    2. CR 405435    - Combo card (media type 17) - Call GetMemberInfo and use card with status 12 as the  member id.

	4.0.32.615 Shahaf   1. No TD        - All EMV cards decline due to missing account number field. 

	4.0.32.616 ShimiP	1. CR 404753	-CRIND to supports communicate 1:1 message from Loyalty
						2. TD 455809 - Fuel crushes when import carwash xml files.

	4.0.32.617 ShimiP	1. CR 404753	-CRIND to supports communicate 1:1 message from Loyalty

	4.0.32.618 ShimiP	1.NO TD - The EFT limit in contactless should include the carwash limit.

	4.0.32.619 OmerT    1.No TD - When car wash attribute defined as 2 in DynamicPapFlow.xml pumpsrv will skip to the next step.
	
	4.0.32.620 ShimiP	1. CR 456425	-Customer member point are not showed ate END of transaction 3B

	4.0.32.621 ShimiP	1.TD 456298 - Fuel show confirmation screen for $ 50   but request authorization for $12

	4.0.32.622 OmerT    1.TD 456223 - LPE French Languages are not received.

	4.0.32.623 OmerT    1.No TD     - Contactless card flow will start by authorizing the card. 

	4.0.32.625 OmerT    1.TD 456795 - Tap card on IDLE sends request limit of zero to EFT
						2.TD 454424 - Use virtual member in loyalty summary

	4.0.32.626 OmerT    1.TD 456792 - Pump displays incorrect message when invalid Linked card is swiped on the pump.
			            2.TD 456793 - FSR already used cancels the transaction.

	4.0.32.627	ShimiP	1.TD 457134 - TransAmount is zero in a complete. 
						2.TD 457300 - no receipt messages on receipt

	4.0.32.628	ShimiP  1.TD 456792 - added a new graphic screen

	02/06/2016 4.0.32.629 ShimiP - TD 456792 - added a new graphic screen for loyalty offline\declined

	08/06/2016	4.0.32.630
		1.ShimiP - TD 457658 -Fuel crashed on set PrePay fromPOS.

	13/06/2016	4.0.32.631
		1.ShimiP - TD 457658 -Fuel crashed on set PrePay fromPOS - increased the spare size to 202 in extra_info

	14/06/2016	4.0.32.632
		1.MaximB - TD 457888 - EMV contactless transaction, Fuel sends garbage in account Number

	22/06/2016	4.0.32.633
		1.ShimiP - TD 457745 - Two screens when loyalty is offline

	22/06/2016  4.0.32.634
        1.OmerT  - CR 405082 - Fuel Price Change Management. Batch number was not sent in alarm 0211070 (price change ended alarm).

	29/06/2016  4.0.32.640
		1.OmerT  - No TD     - Support GetCardInfoEx when working with Epsilon.

	30/06/2016  4.0.32.641
	   1.OmerT  - TD 457745 - Two screens when loyalty is offline

	04/07/2016  4.0.32.642
	   1.Omer   -  No TD	- on call AddGetCardInfoEx add AID value.
	   2.Maxim  - TD 458938 - Account number is missing on Contact\Contactless transaction.
	
	04/07/2016 4.0.32.643  
		1.OmerT  - CR 405082 - Fuel Price Change Management. Batch number was not sent in alarm 0211069 (price change not applied alarm).

	26/07/2016 4.0.32.644
		1.Shahaf - TD 460199 - Fuel not sending correct tax information to ELS
		2.OmerT  - TD 460209 - Contactless use wrong limit to authorized card.

	28/07/2016 4.0.32.645
		1.MaximB - TD 460484 -	Contactless reader is not active after swiping an FSR card.

	03/08/2016 4.0.32.646
		1.OmerT  - CR 405456     - Send MediaType to VP.
							     - Send encrypted account number to VP.
				   TD 460382     - On a PPC transaction followed with an EMV card, the EMV card gets declined immediately.
	
	10/08/2016  4.0.32.649
		1.OmerT    TD 459621     - Pump Receipt reprinted at the POS has incorrect total, $/L, and volume values.

	01/09/2016  4.0.32.650
	    1.OmerT    No TD         - Use AdditionalCode as int64
		2.OmerT    No TD         - Fix issue with inserting receipts into receipt qdx.
		3.OmerT    No TD         - Fix fuel total sales value passed to VP.
		4.OmerT    No TD         - Add encoding to VP xml (set HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Pointofsale\PumpSrv\GeneralParam\AddEncodingStringToVPXml to enable)

	06/09/2016	4.0.32.660	
		1.ShimiP - TD 462314	- Prepaid Receipt DryItem.

	07/09/2016 4.0.32.661       
		1.OmerT  - CR 405456    - Suncor - EFT Phase 8_Support Refund of FSR, PPC or PPD.	

	23/09/2016 4.0.32.662
		1.Shahaf - CR 405082    - Fuel Price Change Management. price change success alarm (alarm number 68) was not sent.

	04/10/2016 4.0.32.663
		1.Shahaf - No TD        - Alarm will be set to PosPump on failed/successful Init Debit.

	06/10/2016 4.0.32.670
		1.OmerT  - TD 463453    - For PAP transactions Tax Calculation is Done before LPE CPL discount is applied

	13/10/2016 4.0.32.671       
	   1.OmerT   - TD 464219    - Suncor -Tapping card Fuel crashed.

   18/10/2016 4.0.32.672
	   1.Marik	- TD  428562	- FSD AFD EMV Transactions - Support OLA Tag AuthorisationResponseCode.

   26/10/2016 4.0.32.673
	   1.OmerT  - TD  464915    - Contactless sequence working incorrectly

   27/10/2016 4.0.32.674
	   1.OmerT  - TD  465060    - PAP - PPD/EMV Insert split tender flow is not processing the EMV tender during Auth.

   28/10/2016 4.0.32.675
	   1.OmerT  - TD  465060    - PAP - PPD/EMV Insert split tender flow is not processing the EMV tender during Auth.

   30/10/2016 4.0.32.676
	   1.OmerT  - No TD		    - Contactless was closed on partial approval screen.

	01/11/2016 4.0.32.677
	   1.OmerT  - TD 465060 	- Partial approval summary screen doesn't show 2nd card name.

	02/11/2016 4.0.32.678
	   1.Haimk	- TD 465190		- Suncor Grade restriction not enforce.

	15/11/2016	4.0.32.690 
	   1. Shimip - CR 457947 FSD_Support Tax ID range and Expansion of Tax
				 - CR 462785 Fuel Support 8 digit Tender ID
		         - CR 463204 Fuel Support CPL Tax Type and Four Taxes on Items

	22/11/2016	4.0.32.691	
		1. ShimiP   - No TD - PumpSrv sends ‘0’ tax to ELS

	05/12/2016 4.0.32.692  
		1. OmerT    - No TD - Added missing fields to xml convector.
		2. AlexM	- TD 462238  - Fuel Saver Doubling after prepay was moved.  Fix:  wrong index in MovePrepayGradesDiscounts. (merge from 1031)
		3. ShimiP	- TD 457947  - send to EFT  the Tax number in EFT buffer.
		4. OmerT    - TD 466954  - Suncor - Preset amount selected during CRIND idle screen less than car wash amount then authorizing amount goes to negative.


	08/12/2016 4.0.32.693
		1. OmerT	- TD 457947  - send to EFT  the Tax number in EFT buffer - support car wash items.

	12/12/2016 4.0.32.694
		1. OmerT	- TD 467180  - send to EFT  the Tax number in EFT buffer - support car wash items.
	
	19/12/2016 4.0.32.695
		1.OmerT     - 467566    - On partial approval if customer decides not to swipe a second card and the limit on the first card is not enough for carwash + carwash promotion the transaction should be canceled.

	26/12/2016 4.0.32.696
		1.MaximB    - TD 467646 - When downloading PCD, EMV tables and graphics at the same time the SPOT's internal font files get deleted.

	10/01/2017 4.0.32.700
		1.OmerT    - TD 467852 - Receipt displaying wrong balance for a second PPC/FSR card swiped at pump when not applying second card savings to the PAP transaction.

	17/01/2017 4.0.32.701
		1.OmerT    - TD 468878 - Fuel tax calculated on total transaction amount including carwash.

	20/01/2017 4.0.32.702
		1.Shahaf   - TD 469020 - Zero item quantity sent for carwash item on PAP completion to Mobila

	24/01/2017 4.0.32.703
		1.Shahaf   - TD 469299 - Carwash Total amount not sent in Auth Request when PAP with Carwash

	30/01/2017 4.0.32.704
		1.Shahaf   - TD 469490 - Fuel crashing when running Init Pumps Debit from POS or Debit/Credit EMV at the pump 

	31/01/2017 4.0.32.705
		1.Shahaf   - TD 469021 - Fuel tax amount not correct on the receipt for PAP
		2.OmerT    - TD 469637 - Suncor - Fuel needs to be stopped and started for tax changes to be updated.

	03/02/2017 4.0.32.706
		1.Shahaf   - TD 469021 - Fuel tax amount not correct on the receipt for PAP
		2.Shahaf   - TD 469830 - when using Ryko car wash POS reports car wash as offline and is unable to sell tickets, but AFD can. 

	15/02/2017 4.0.32.710 
	    1.OmerT    - NO TD    - Fix CPL tax calculation.
				   - NO TD    - Fix issue with Tax name with length of 20.
				   - No TD    - Send VP the rate of CPL tax.

	17/02/2017 4.0.32.711
	    1.Shahaf  - NO TD    - Fixed CPL tax calculation. support decimal point

    26/02/2017 4.0.32.712
		1.OmerT    - TD 470156 - Loyalty + Debit tap at AFD is failing with error message on CRIND as "PPT swiped first"

	28/02/2017 4.0.32.713
		1.Shahaf	- No TD	 - Added Tax Type support for CPL tax.

	06/03/2017 4.0.32.714 
	   1.OmerT      - TD 471245 - Member card # isn't been send to LPE on swipe CIBC as second payment.

	03/03/2017 4.0.32.715
	   1.Shahaf	  - No TD	 - Fixed CPL tax calculation.

	16/03/2017 4.0.32.716
	   1.Shahaf	  - TD 471693 - Suncor - CR463204 - TaxID sent to Epsilon is wrong

	28/03/2017 4.0.32.717
	  1.Shahaf   - TD 471634 - CPL tax information is missing in a PAP card_sale_all information sent to VP 

	30/03/2017 4.0.32.718
	  1.OmerT   - TD 452590  - Send transaction details - support DecimalRewardAmount

	31/03/2017 4.0.32.719
	  1.Shahaf  - TD 472399  - Suncor - CR463204 - Fuel is not handling taxes correctly when 5 taxes are attached to a fuel grade during a P@P transaction.

	18/04/2017 4.0.32.720
	  1.Shahaf  - TD 473127  - Suncor - Tax rounding issue with CPL and other taxes

	25/04/2017 4.0.32.722
	  1.Shahaf  - TD 452590  - Send transaction details

    01/05/2017 4.0.32.723
	  1.Shahaf  - TD 452590  - Send transaction details

	08/05/2017 4.0.32.724
	  1.MaximB  - TD 473974  - PPV for PAP with FSR,PPC and Debit completed without prompting for Debit card

	08/05/2017 4.0.32.725
	  1.Shahaf  - TD 473776  - Item UPC is not sent to Mobila in Auth request
	  2.Omer	- TD 473778  - PPD balance printed on the PAP receipts is not correct

	09/05/2017 4.0.32.726
	  1.Shahaf  - TD 473776  - Item UPC is not sent to Mobila in Auth request

	23/05/2017 4.0.32.727
	  1.Shahaf  - TD 452590  - Send transaction details

	01/06/2017 4.0.32.729
	  1.OmerT   - TD 474997  - Certain CIBC cards cause PumpSrv to crash upon attempted Auth during PAP.

	12/06/2017 4.0.32.730
	  1.Haimk   - TD 470646  - Support Debit Card as Linked Card .

	27/06/2017 4.0.32.731
	  1.Shahaf	- TD 474602  - Pump receipt issue - PAP receipt layout issue.

	03/07/2017 4.0.32.732
	  1.Maxim	- TD 476769  - Total owed is incorrect when we have multiple fuel savings
	  2.Maxim	- TD 476762  - FSR and PPC tenders are not applying when LPE offline

   03/07/2017 4.0.32.733
	  1.Maxim	- TD 475451  -  Tap is not enabled when limit of FSR is reached but insert and swipe are enabled

   03/07/2017 4.0.32.750
     1.OmerT    - TD 478137  - Returns for PAP transaction with PPD and another payment card fails PPV.

   01/09/2017 4.0.32.751
	 1.Shahaf    - TD 476769  - Total owed is incorrect when we have multiple fuel savings

   08/09/2017 4.0.32.752
	 1.Shahaf    - TD 479850  - Taxes not sent to Mobila in PAP capture request

   03/10/2017 4.0.32.753
	 1.Shahaf    - TD 480815 - PPTS + FSR + EMV DEBIT Account Selection screen displays twice - Pay At Pump fails to pre-auth

   11/12/2007 4.0.32.???
     1.AlexM	-  No TD	merge Mobile solution (start)

	 merge from Trunk
		 NahumN  21/10/14	2.  CR 414100 - Theft alarm expansion details in alarms
		 AlexM	22/10/14    3.  CR 405462 - Send Meter Read when Price Changes
		 AlexM   27/10/13    4.  TD 325383 - ATG additional alarms.  added method to interface CCtrl::GetConfiguration. (merge from 1025)
		 AlexM   27/10/14    5.  TD 407287 - IBT: Lock/UnLock nozzles by Tank number. (merge from 1025)
		 AlexM	26/11/14	6.	TD 419618 - PFS - Pump Icons Coming up with  Lock Symbol after q-load every Monday morning.
		 AlexM	30/11/14	7.	CR 418937 - Fuel to Support new EpsilonLinkSrv connection
		 AlexM	05/01/15	8.	TD 422282 - SUNCOR CR 405462 - Fuel : PumpReading not trigger when cashier change price in BOS.
		 AlexM	01/02/15    9.	TD 412765 - Incorrect data in Lock/Unlock pump test report.(merge from 1025)
		 AlexM	01/02/15   10.	TD 420361 - IBT: Lock/UnLock nozzles by Tank number(merge from 1025, continue)
		 AlexM	09/02/15   11.  TD 426940 Suncor - CR404752 - Unable to change fuel prices from BOS. added protection against using PriceChangeLock && PriceChangeTotalsRequired
		 AlexM	09/02/15   12.	TD 426830 SUNCOR - P@P transactions don't get to VP. fixed "sLanguage" from BYTE to DEC.
		 AlexM			   13.  TD 426985 - Suncor - CR404591 - Price updates and displays new price when price change is initiated and price sign is in a blank status.
		 AlexM			   14.  TD 427833 - PFS - (Outside Payment Terminal) OPT TRANSACTION DETAILS NOT COMPLETE IN EJ.
		 AdirH   22/06/15   15.  TD 437944 - Car Wash has transactions stuck in "W_Progress".
		 PolinaA	21/09/15   16.  CR 442586 - Reconfigure device (Pump or Terminal) by re-reading parameters in it's thread, without killing all threads
		 PolinaA	03/11/15   17.  TD 417989 - Support of MaxShiftNumber change
		 PolinaA	07/12/15   18.	CR 414103 - TankSrv reconfiguration
		 PolinaA 30/12/15   19.  CR 443035 - Ullage report - printing last Tankreading data & "No connection" (instead of ullage volume) if tank offline (new Report001.xml required)
		 PolinaA 02/06/16   20.	CR 443524 - Interface & function CGPI::GetOfflineTanks added
		 PolinaA 14/03/16   21.  TD 429143 - Some default registry keys not created & not displayed in FCC.
		 AdirH   05/04/16   22.  CR 407579 - Automatic authorization for a UDF loyalty customer.
		 AdirH   05/04/16   23.  CR 433449 - Alternate Loyalty ID with Encore 700 Fuel.
		 AdirH   26/08/16   24.  QC 461948 - Stuck transaction due to cancel in middle of authorize.
		 AdirH   30/08/16   25.  CR 433449 - Alternate Loyalty ID with Encore 700 Fuel - Enhancement Request to support Yes/No response in the PhoneLookUp prompt.
		 AdirH   08/09/16   26.  QC 462152 - PAP transactions are not reporting Weight of fuel purchased.
		 AlexM   07/12/16   27.  TD 462238 - Fuel Saver Doubling after prepay was moved.  Fix:  wrong index in MovePrepayGradesDiscounts.
		 AdirH   20/01/17   29.  QC 465917 - PCI - Card holder data is exposed in the clear in fuel QDX.

	 04/01/2018		4.0.32.780 
		 Manik 		1.  NO TD -  Compiled in VS2015


	09/01/2018		4.0.132.780
		AlexM		1. No TD -  support UseXmlBuffer flag with FuelMobileSrv  (merge from 1022 Delek)

	17/01/2018		4.0.132.781
		AlexM		1. No TD - fix Mobile receipt flow: replace save class TRS to QDX.receipt6 to TRS_TAG
	27/02/2018		4.0.132.782		
		Manik		1.	CR_475695- FSD BP 475695 Tank Reading Type (merged from 1024)

	08/03/18        4.0.32.810
		OmerT       1. QC 486392 - IN#85934961 - MARTINS - multiple issues with the Pay Inside function after LoyaltyFuel.dll 1031.1080 upgrade (merge from 1031)
		OmerT       1. No TD (Emerald) - Fix Issue with TransAmount field for a single tender transaction.

	17/07/18		4.0.32.840
		JackD	-	No TD	- Updated database code to match Pilot and include field validation code.
		OmerT   -   No TD   - PumpSrv was crashing in insertReceipt (CServerMain::ConvertReceiptToOla)
		AlexM	-   No TD	- merge from 1023 
							  CFI SCO -- Prepays. add interface method GPI::QueryPumpInfo 

		PolinaA -   CR 472673 - Save Hardware version in registry (merge from 1022)
		ShimiP  -   No TD     - Fix endless loop in GetReceiptRawData3.
		OmerT   -   No TD    - Emerald - GetPAKQuery2 - send in progress prepaid transactions.
		OmerT	-   No TD    - remove call to _Module.m_server.m_cBuildLine2PosPump.GetFormatedLine that causing PumpSrv to crash.
		JackD	-   No TD    - Merged in some changes from the Pilot version. This is an ongoing effort.


	08/08/2018		4.0.32.850
		Shimip  - merge from loves CBuildLine2PosPump

	20/08/2018		4.0.32.853
		JackD	-   No TD     - Added PS::PumpTransactBase to OLA SQL calls.
		AlexM	-	No TD	  - remove sAccountNumber from log.
		OmerT   -   No TD     - 1. replaced EnterCriticalSection with CSingleLock
					No TD     - 2. added CPumpSrvRealTimeInfoRecordSet to support real time table - holds the pumps data - PAP and PumpInfo
					No TD     - 3. The pumps were locked to prepay when starting fuel.
					No TD	  - 4. invalid pump cause exception in CServerMain::SendPumpMode2Events()

	18/12/2018     4.0.32.854
		OmerT  -    No TD     - Reading/Writing/Deleting from Delivery,OLA and TankGauge tables caused an exception.

	25/12/2018     4.0.32.855
		OmerT -     TD 492925 - After mobile installation - VP transactions written twice in TRA

	13/01/2019     4.0.32.856  
		OmerT -     No TD     - Updates in receipt table.

	17/01/2019     4.0.32.857
		OmerT -     No TD     - SQL TankGauge table fix	
		OmerT       No TD     - Delivery report is missing bos reference number
	
	03/03/19	   4.0.32.880
		AlexM   -  No TD    - Emerald - added XML tag "NacsAdditionalCode" - use Grade/AdditionalCode.		
		OmerT   -  NO TD    - Emerald - fix issue with receipt printing, added missing pump number to receipt table.

	11/03/19	   4.0.32.881
		OmerT   -  No TD    - Emerlad - Fix reprint. 

	**************************************************************
	03/04/19	   4.0.34.0
	 32 branch + merge from Trunc:
	QC 493899 - WESCO: Loyalty rewards deducted more than total for gas discount.
	added  CD_TYPE_MOBILE    'W' (merge Delek)
	TD 493551 - PumpSrv did not send update to RFS tables on ReconfigureOnEOS
	QC 502830 - UDF: Auto Auth Functionality is not working in an Epsilon system.

	14/04/19	   4.0.33.0 
		PolinaA - No TD    - Compiled in VS 2017

	18/04/19       4.0.32.001
	1.  ShimiP	-	NO TD - support zone printing in SQL
	2.  OmerT   -   No TD - Emerald-Epsilon integration.

	27/05/2019		4.0.33.004
	1.ShimiP - NO TD - epsilon merge.

	28/05/2019		4.0.33.005
	1. AdirH     -  TD 504179 - Car wash was selected but wasn't reported to EPS on the complete.
	2. OmerT     -  NO TD     - Fix issue with Complete session to Epsilon
	3. MichaelPo -	US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	4. OmerT     -  NO TD     - Protection for non valid mode.
	5. Shimi     -  EMV CONFIG merge from Sainsbury's
	6. Shimi     -  EMV Tread merge fix
	7. OmerT     -  RFUEL-84  - RFUEL should Query for grade promotions from Emerald loyalty server

	21/07/2019		4.0.33.10
	1. OmerT     -  RFUEL-172 - Reprint receipt is not functioning.

	22/07/2019		4.0.33.11
	1. OmerT     -  RFUEL-172 - Reprint receipt is not functioning.
	
	08/08/2019		4.0.33.12
	1. PolinaA	 - 	TD 507052 - add CardSaleData6 to GetReceiptRawData3
	2. OmerT     -  No TD     - add logs to gci:lockPump

	08/09/2019		4.0.33.13
	1. MichaelPo -	RFUEL-375 - Reprint last receipt doesn't print transaction data

	15/09/2019      4.0.33.14
	1. OmerT     -  RFUEL-292 - SQL version of Fuel has reference to QDX engine
	2. OmerT     -  NO TD     - support LPE SA testing

	19/09/2019	    4.0.33.15
	OmerT        -  RFUEL-292 - SQL version of Fuel has reference to QDX engine
	EyalP		 -  RFUEL-229 - Recovery of PAP transaction (was checked in before)

	02/10/2019		4.0.33.16
	JackD  1. Updated FuelCommon for GetAttributeToByteArrWithConvert. This function now supports typesafe templates.
	Maxim  1.	RFUEL-426 RFS - Support Terminal Capabilities per Card Brand for Contact EMV on OPTIC
				RFUEL-396 Optic R&D script - Terminal does not show the PIN bypass option when it should be
				RFUEL-470 Fuel experienced a crash on the Optic Environment
				RFUEL-457 R&D script Optic - Terminal prompts to enter PIN which is should not
	23/10/2019		4.0.33.17 
	marik  1. RFUEL-575 Wayne - Approval Number is not printing the receipt
	24/10/2019		4.0.33.19
	Marik  1. Modify PumpSrv dependency for Struck member Alignment to be default to fit to FuelSQLDB.dll &  RFS.
	31/10/2019		4.0.33.20
	Marik  1. Add support for for new OLA tag QuickChipTrs[B] 

	10/11/2019		4.0.33.21
	EyalP			1. No TD - CLog::RedirectIOToConsole merge from Trunk

	09/01/2020		4.0.33.999
	EyalP			1. RFUEL-744 - QuickCheck - 1023 to 1033 sync

	15/01/2020		4.0.33.???	
	EyalP			1. merging CR 312183 missing parts
	
	02/02/2020		4.0.33.???	
	MariaB			2. RFUEL-569 U-Drive Swipe Declines Offline and does not Auto-Arm for PAK

	04/02/2020		4.0.33.22
	AdirH			1. CR 455755 - Support Excentus Link Server after Epsilon Integration.
	2. CR 501164 - Casey's Punchh Loyalty Integration.
	3. RFUEL-381 - PAP Dispensing without Credit Authorization.
	JackD			4. No TD - Fixed runtime errors with uninitialized variables.
	EyalP			5. validation of SQL Tables

	11/02/2020      4.0.33.???	
	EyalP			1. NO TD - CFuelLoyaltyPS::ParsePromotionsFromRtaps, CheckMemberStatus flag added to registry

	13/02/2020		4.0.33.23
	AdirH			1. RFUEL-969 - Outdoor EMV - PINBlock data is not being sent to the network.

	26/02/2020		4.0.33.24
	EyalP			1. RFUEL-978 pump reading in XML is wrong (CCtrl::GetPumpTotalsEx) - PumpSrvPumpTotals table changed volume and valueA and ValueB columns to char (used to be binary).
					2.				NO TD - CParam::GetParam3Level Exception fixed (lKey2 can be 100 or higher. Doesn't fit array size of 3)
					3. NO TD - Printing FuelMSSQL version to the log
					4. NO TD - Omer removed Q-dex maintenance
					5. RFUEL-194 - When stopping Fuel using FCC, PumpSRV has exception error - omer removed release of m_pbsrOlaBuffer (let it die with PumpSrv)
					6. NO TD - initialization of PumpSrvDatabase occurs only during running

	05/03/2020		4.0.33.25
	Shahaf			1. RFUEL-1039 - Gift Card completion causes GENERAL_ERROR/Locks gift card.

	##############################################################################################################
	###### QuickCheck Version ###### QuickCheck Version ###### QuickCheck Version ###### QuickCheck Version ######
	##############################################################################################################

	27/02/2020		4.0.33.120
	EyalP			1. NO TD - merging from PumpSrv 1033 change sets 151590-159220 (4.0.33.22-24)

	17/03/2020		4.0.33.121
	Shahaf			1. RFUEL-1039 - Gift Card completion causes GENERAL_ERROR/Locks gift card
	OmerT           2. RFUEL-1087 - Optic - Outdoor EMV - Fuel does not initiate GetCustInfo when loyalty card is swiped during a P@P transaction

	18/03/2020		4.0.33.122
	OmerT           1. RFUEL-1087 - Optic - Outdoor EMV - Fuel does not initiate GetCustInfo when loyalty card is swiped during a P@P transaction
	
	24/03/2020		4.0.33.125
	OmerT           1. RFUEL-1087 - Optic - Outdoor EMV - Fuel does not initiate GetCustInfo when loyalty card is swiped during a P@P transaction

	25/03/2020		4.0.33.126
	OmerT			1. RFUEL-1087 - Optic - Outdoor EMV - Fuel does not initiate GetCustInfo when loyalty card is swiped during a P@P transaction

	27/03/2020		4.0.33.127
	OmerT			1. RFUEL-1087 - Optic - Outdoor EMV - Fuel does not initiate GetCustInfo when loyalty card is swiped during a P@P transaction

	30/03/2020		4.0.33.129
	OmerT + EyalP	1. RFUEL-1087 - Optic - Outdoor EMV - Fuel does not initiate GetCustInfo when loyalty card is swiped during a P@P transaction

	03/04/2020      4.0.33.130 
	OmerT           1. RFUEL-1149 - UDF EMV - Terminal prompts for payment card swipe two times.

	06/04/2020      4.0.33.131
	OmerT           1. RFUEL-1149 - UDF EMV - Terminal prompts for payment card swipe two times.
					2. RFUEL-1146 - UDF EMV - Fuel get stuck in a loop when trying to cancel a P@P without loyalty

	07/04/2020      4.0.33.132
	OmerT           1. RFUEL-1149 - UDF EMV - Terminal prompts for payment card swipe two times.

	08/04/2020      4.0.33.133
	OmerT           1. RFUEL-1149 - UDF EMV - Send Screen number for Alt Id and Alt Id retry
	
	09/04/2020      4.0.33.134
	OmerT           1. RFUEL-1149 - UDF EMV - Send Screen number for Alt Id and Alt Id retry

	12/04/2020      4.0.33.135
	OmerT           1. RFUEL-1169 - Receipt is not inserted to DB table 
					2. RFUEL-1149 - UDF EMV - Handle loyalty decline (get_cust and set_cust)
					3. NO TD	  - Update pump number in the receipt table

	21/04/2020      4.0.33.136
	OmerT           1. RFUEL-1210 - UDF Outdoor EMV - Loyalty card receives Invalid Card message if Debit card is swiped first

	22/04/2020      4.0.33.137
	OmerT           1. RFUEL-1237 - UDF Outdoor EMV - P@P to P@K transaction has incorrect flow

	24/04/2020      4.0.33.138
	OmerT           1. RFUEL-1250 - UDF Outdoor EMV - Entering an incorrect ALT ID causes terminal to be stuck

	29/04/2020      4.0.33.139
	Marik           1. PumpSrv freeze when set Prepay on Allied system.  

	17/05/2020		4.0.33.140
	AdirH			1. RFUEL-1132 - Configuration command needs to be sent when convert layer connects to NexGen.
					2. RFUEL-1311 - P@P transaction stuck on pump in "In Progress" status.
					3. RFUEL-1312 - P@P transaction stuck on pump in "In Progress" status.
	EyalP			4. merge from PumpSrv-QuickCheck

	17/05/2020		4.0.33.141
	OmerT           1. RFUEL-1265 - ENCOR Fuel EMV setup - Can't recall PrePay transaction.

	31/05/2020      4.0.33.142
	OmerT           1. RFUEL-1367 - Sensitive Data is exposed in PumpSrv log.
	OmerT           2. RFUEL-1347 - PCI DSS - Credit Card numbers appeared in Fuel_log.ldf and Fuel.Db
	Omert           3. No TD      - QueryReceipt compatible to SQL receipt table.

	2/6/2020        4.0.33.143
	OmerT           1.RFUEL-1371  - Impossible to complete Prepay transaction. PumpSrv is crashing.

	17/6/2020        4.0.33.144
	OmerT           1.NO TD      - Coverity fixes.
	OmerT			2.RFUEL-1367 - Sensitive Data is exposed in PumpSrv log.
	Shahaf			3.RFUEL-1410 - EG/CFI OEMV - Pumpsrv sends 'ReferenceNumberField' instead of 'ReferenceNumber'

	18/06/2020		4.0.33.150
	EyalP			1. RFUEL-1375 - UDF Outdoor EMV & Optic - Fuel does not reconnect to SQL server 
						after SQL is stopped and restarted

	22/06/2020      4.0.33.151
	OmerT           1. NO JIRA	  - Save FuelTransactionData.xml only on debug.
					2. RFUEL-1347 - PCI DSS - Credit Card numbers appeared in Fuel_log.ldf and Fuel.Db
	
	22/06/2020      4.0.33.153
	Maxim           1. RFUEL-1347 - PCI DSS - Credit Card numbers appeared in Fuel_log.ldf and Fuel.Db

	29/06/2020      4.0.33.154
	Shahaf			1. RFUEL-1485 - Loyalty Balance is staying at $0.01 when balance is used at the pump.

	02/07/2020      4.0.33.155
	AdirH			1. NO JIRA	  - PumpSrv is getting stuck if UseModeRelatedPrompt is set.

	07/07/2020		4.0.33.160
	EyalP			1. RFUEL-1610 - PumpSrv GetAlarm() method returns 0 when no alarm records present
					2. coverity changes in Ctrl.cpp

	09/07/2020		4.0.33.161
	EyalP			1. removed unneccesary printings from CGPI::GetMinimalPumpsInfo2__

	16/07/2020		4.0.33.162
	OmerT			1. RFUEL-1629 -  Merge Emerald Fuel Support from 1032 to 1033
					2. RFUEL-1711 - P@K transactions disappear when Fuel is restarted
					
	20/07/2020      4.0.33.163
	Shahaf			1. RFUEL-1556 - Swipe Udrive card first, then a gift card the transaction pumps but does not complete and the gift card is not charged the amount

	26/07/2020      4.0.33.170
	AdirH			1. RFUEL-1668 - Prepay transaction causes convert layer to crash.
	Shahaf			2. RFUEL-1720 - CFI OEMV - RFS crashed, Track data size modified to Track1 = 79, Track2 = 40 and Track3 = 107


	27/07/2020		4.0.33.171
	Shahaf          1.RFUEL-1787 - MC Fleet Card caused "Unknown Card" error, Track data size modified to Track1 = 79, Track2 = 40 and Track3 = 107

	30/07/2002      4.3.33.172
	OmerT           1. RFUEL-1795 - CFI OEMV - Car Wash - Code Generation throws ODBC Error.

	30/07/2020		4.0.33.180
	Shahaf          1. version number change

	04/08/2020		4.0.33.181
	Shahaf          1.RFUEL-1787 - MC Fleet Card caused "Unknown Card" error, Track data are back to their original size - Track1 = 76, Track2 = 37 and Track3 = 104

	06/08/2020		4.0.33.190
	Shahaf          1. version number change.

	12/08/2020      4.0.33.191
	AdirH			1.RFUEL-1734 - Fuel_ENCOR-Can enter Loyalty Card at Pump, but it doesn't appear to do anything with the member.

	13/08/2020		4.0.33.200
	EyalP			1. NO TD - version number updated

	13/08/2020		4.0.33.201
	Shahaf			1. RFUEL-1895 - Fuel 1033 StoreLine - QuickChip flag not sent to Epsilon

	14/08/2020		4.0.33.202
	Shahaf			1. NO TD - version number updated

	21/08/2020		4.0.33.203
	OmerT			1. RFUEL-1734 - Fuel_ENCOR-Can enter Loyalty Card at Pump, but it doesn't appear to do anything with the member
					2. RFUEL-1877 - SpartnNash - Fuel 1033 broke backward compatibility of car wash

	25/08/2020		4.0.33.204
	Shahaf			1. NO TD - PumpSrv log file doesn't print anymore PS & FuelMSSQL versions on the same line
					2. RFUEL-1949 - PAP Partial approval Flow disabled for Fuel version 1033

	27/08/2020		4.0.33.???
	charl			1. RFUEL-1923 -  Corrected parameter passed to get completion data into capture request for OLAEpsilon so UnitPrice (UAmt) is filled in

	27/08/2020		4.0.33.220
	MariaB			1. RFUEL-1927 - QA Emerald 1032 -1033 bug support part II . Added TransactionID and CustomerIdentifiers for P@P and Pap2Pak
					2. RFUEL-1745 - Emerald - Add Support For Gallon Limit with Loyalty - Pump Stop

	27/08/2020		4.0.33.221
	OmerT			1. RFUEL-1474 - Cannot reprint a receipt on POS.  Get Error 548 No receipt to print
					2. RFUEL-1917 - ENCOR_FUEL-Trying to "Reprint" receipt from POSPump gives "Failed to Retrieve Receipt".
	
	31/08/2020      4.0.33.222
	OmerT           1. RFUEL-2009 - Fuel is sending multiple authorizations for single outdoor payment

	01/09/2020      4.0.33.223
	OmerT           1. RFUEL-2009 - Fuel is sending multiple authorizations for single outdoor payment
		            2. RFUEL-1959 - Fuel 1033 - Canceling the phone number lookup prompt keeps the transaction in progress
					3. RFUEL-2006 - Getting invalid Loyalty decline prompt when selecting No to redemption  

	08/09/2020		4.0.33.230
	Shahaf			1. NO TD - version number updated

	08/09/2020		4.0.33.231
	Shahaf			1. RFUEL-2006 - Getting invalid Loyalty decline prompt when selecting No to redemption

	11/09/2020      4.0.33.240
	OmerT           1. RFUEL-2055 - PAP prompt disabled, transaction completes, discount not applied
					2. RFUEL-2067 - ENCOR_Fuel-PAP Transactions with Loyalty Promo is missing Loyalty Summary information.

	14/09/2020		4.0.33.241
	MariaB			1.RFUEL-2046 - card info is not passed to loyalty in new released version

	15/09/2020		4.0.33.242
	PolinaA			1.RFUEL-2093 - Add sGradeDiscountID to XmlConvertor

	15/09/2020		4.0.33.243
	PolinaA			1.RFUEL-2117 - Add WaitForFinalizeResponse parameter

	15/09/2020		4.0.33.244
	MariaB			1.RFUEL-2046 - card info is not passed to loyalty in new released version - CustomerInformation fix in PAP buffer
					2.RFUEL-2117 - Add WaitForFinalizeResponse parameter - Moved LoadParam to ServerMain

	15/09/2020		4.0.33.246
	MariaB			1.RFUEL-2117 - Add WaitForFinalizeResponse parameter - Fix. Init to 0.

	21/09/2020		4.0.33.250
	NoamA			1.RFUEL-2046 - add cancel ticket support for Emerald Loyalty

	23/09/2020		4.0.33.251
	NoamA			1.RFUEL-2147 - wrong TransactionId, in buffer RewardServiceRequest 
					2.RFUEL-2144 - wrong RequestId in RewardServiceRequest buffer 

	30/09/2020		4.0.33.252
	Shahaf			1.RFUEL-2043 - Phone number retry not working for PAP	

	07/10/2020      4.0.33.254
	OmerT           1.RFUEL-2184 - Pay at pump transaction with RewardsGet is failing will cause fail next swipe.
	Shahaf			2.RFUEL-2209 - Paytronix: Prepays are not removing the discount from an account when redeeming.
					3.RFUEL-2193 - PAP with NO selected for payment, converts to PAK. no discounts applied for loyalty

	11/10/2020		4.0.33.262
	MariaB			1.RFUEL-2187 - On Pump 2, Pap transaction with loyalty does not continue than Rewards Get is failing
					2.RFUEL-2200 - Pap to Pak transaction - wrong TriggerProm Amount values in ExtraInfo buffer
					3.RFUEL-2261 -	1033 with Cl2PumpSrv does not handle declined EMV Pay at Pump transaction receipts

	22/10/2020		4.0.33.263
	AdirH			1.RFUEL-2236 - If Loyalty Card is declined, transaction doesn't decline if customer choose not to continue.
	MaximB			2.RFUEL-2262 - UDF OEMV Beta - PAP w ALT ID first 'Card Balance 0'
					3.RFUEL-2302 - Prepay with loyalty does not honor 25 Gallon Limit
					4.RFUEL-2303 - Loyalty Price Protection Not Appearing For Prepay
					5.RFUEL-2300 - No card data on receipt for P@P /w loyalty scan - Missing complete

	26/10/2020		4.0.33.264
					1.RFUEL-2334 - Card not authorized 1 error after PAP fueling.

	28/10/2020		4.0.33.265
					1.RFUEL-2339 - No complete on sale when using Attendant Card.

	01/11/2020		4.0.33.270
	MariaB			1.RFUEL-1732 - Add Support for Discount Deferral Prompt with Loyalty
					2.RFUEL-2306 - Triggered promotion discount is not calculate correctly- should 2 decimal points
					3.RFUEL-2141 - Update finalize message to provide carwash details and related promotions
					4.RFUEL-2142 - Send selected carwash only after wash selection and apply received promotions

	09/11/2020		4.0.33.271
	NoamA			1.RFUEL-2393 - Pap To Pak Discounts - Trigger promotions Amount = 0 in Extra info buffer
					2.RFUEL-2378 - Pap To Pak - with promotions - Fuel is sending too many Triggered Promotions lines in ExtrInfo Buffer
					3.RFUEL-2206 - FUEL returns wrong Wash Program on GenerateWashCode

	11/11/2020		4.0.33.272
	MariaB			1.RFUEL-2423 - Emerald: PAP FuelTransactionData xml doesn't include DeferredAccepted field

	13/11/2020		4.0.33.273
					1. RFUEL-2395 - WinHex shows Card data on in Fuel_log.ldf
	15/11/2020		4.0.33.274
					1. RFUEL-2424 - QueryExtraInfoData2 returns zero as totaldiscount on transactions with discount
					2. Gather common loyalty definitions to one common file to avoid redudancy. 
	
	16/11/2020		4.0.33.275
	MariaB			1. RFUEL-2399 - EntryMethod does not indicate that barcode scanner was used when a loyalty barcode is scanned using the barcode reader on the pump.

	24/11/2020		4.0.33.277
	NoamA			1. RFUEL - 2299 If loyalty limit exists and no nozzle is selected, set it to all nozzles
					2. RFUEL - 2445 Minimum Reward limit applied irrespective of dispensed fuel product
					3. RFUEL - 2463 Save ExtraInfo to DB on regular PAK (Changes are added but commented out for now)

	25/11/2020		4.0.33.278
	OmerT           1. RFUEL-2395 - WinHex shows Card data on in Fuel_log.ldf
					2. RFUEL-2499 - Remove commercial fueling logic from PromptName0 need more param.

	29/11/2020      4.0.33.279
	OmerT           1. RFUEL-2501 - Loyalty Discount not removed from card account. Note: Need to delete extra info table before starting PumpSrv.
					2. NO JIRA    - Removed includes of pumpsrvDb.h, removed QDX files.
	NoamA			3. - PRFUEL-2514  Incorrect Carwash Price in RewardServiceRequest message sent to Emerald	

	6/12/2020		4.0.33.280
	MariaB			No Jira. Emerald SpartanNash release

	10/12/2020		4.0.33.281
	Naga Raju P		1. RFUEL-2332 - Optic: Price protection when NO is selected for loyalty is not working

	15/12/2020		4.0.33.282
	OmerT 		    1. RFUEL-2609 - Carwash not selling correct product

	16/12/2020      4.033.284
	OmerT           1. NO JIRA - Recompile as 284.

	18/12/2020		4.0.33.285
	Naga Raju P		1. NO JIRA - Recompile as 285

	21/12/2020		4.0.33.286
	Naga Raju P		1. RFUEL-2611 - Merge to 1033** Paytronix - Incorrect price going to paytronix - PAP Flow

	21/12/2020      4.0.33.291
	NoamA			1. RFUEL - 2628 Fuel do not send Customer information in PAP / PAK transaction with loyalty
					2. RFUEL-2586   Incorrect TotalRewardAmount when a carwash item is bought along with fuelling and an extra TriggeredPromElement element

	MariaB			3. RFUEL-2399 - EntryMethod does not indicate that barcode scanner was used when a loyalty barcode is scanned using the barcode reader on the pump

	22/12/2020		4.0.33.292
	MariaB			1. RFUEL-2399 - fix for EntryMethod does not indicate that barcode scanner was used when a loyalty barcode is scanned using the barcode reader on the pump

	24/12/2020      4.0.33.293
	Marik			1. RFUEL-2649 - Fuel database ReceiptsExtraInfo table has a record count of over 10k which is causing issues with reprinting receipts from the POS

	25/12/2020      4.0.33.294
	NoamA			1. RFUEL-2651 -	In FuelTransactionData Fuel sends the grade in the PLUcode while Emerald needs the AdditionalCode.
	
	24/12/2020      4.0.33.295  
	Marik			1. RFUEL-2649 - Fuel database ReceiptsExtraInfo table has a record count of over 10k which is causing issues with reprinting receipts from the POS

	25/12/2020      4.0.33.296
	NoamA			1. RFUEL-2628   Fuel do not send Customer information in PAP / PAK transaction with loyalty
					2. RFUEL-2271   Loyalty Cancel request from Fuel to Emerald
					3. RFUEL-2586   Incorrect TotalRewardAmount when a carwash item is bought along with fulling and an extra TriggeredPromElement element
					4. RFUEL-2650   The discounts are not updated at the Pump when grade has an Additional Code different than OLA Code

	08/01/2021		4.0.33.300
	Maria			No Jira - Emerald Spartan Nash release 33.300.

	18/01/2021		4.0.33.301
	Maria			RFUEL-2732 - Unable to Scan loyalty card and payment at the pump

	26/01/2021		4.0.33.302
	Naga Raju P		1. RFUEL-2347 - Loyalty Receipt message buffer is replacing the Epsilon Receipt message buffer on Receipt
						
	01/02/2021		4.0.33.303
	Sang			1. RFUEL-2795 - Idle screen is not updating with mode changes

	15/02/2020      4.0.33.310
	NoamA			1. RFUEL-2673   PAP with loyalty card  - fix flow for server Loyalty is Offline
					2. RFUEL-2693   PAP carwash promotion not Showing on receipt + don't apply carwash discount if bigger than carwash unit price.
					3. RFUEL-2682   Scanning loyalty and trying to pay with personal Fleet that requires input does not work
					4. NO Jira		Add missing member (sWashExpireTime)  to CARD_SALE_EXTRA_DATA6_  to align with OLA.H and other definers of this struct
					5. NO Jira		Fix Pumpsrv crash -  extra dummy NMP declared then when clearing array deleted twice.

	17/02/2020		4.0.33.311
	NoamA			1. RFUEL-2673   SpartanNash - Unable to Scan loyalty card and payment at the pump
					2. RFUEL-2729	SpartanNash - Select 'No' @ Car wash prompt, will get a second car wash prompt

	22/02/2021		4.0.33.312
	Shahaf			1. RFUEL-2732: Unable to Scan loyalty card and payment at the pump

	23/02/2021		4.0.33.313
	Naga Raju P		1. RFUEL-2834 - TRA file generated with CardSaleAllXML=1 records incorrect CarWash price
					2. RFUEL-2835 - Gate: Fuel is reporting the sOriginalUnitPrice with 2 decimal places instead of 3
	
	01/03/2021		4.0.33.314
	Karun D			1. RFUEL-2138 - Pay at pump Buffer - Tank ID always = 0

	04/03/2021		4.0.33.316
	Shahaf			1. RFUEL-2732 - Unable to Scan loyalty card and payment at the pump

	03/03/2021		4.0.33.315
	Marik 			1. RFUEL-2834 - TRA file generated with CardSaleAllXML=1 records incorrect CarWash price - (Production Critical Issue!)

	04/03/2021		4.0.33.316
	Marik 			1. RFUEL-2834 - TRA file generated with CardSaleAllXML=1 records incorrect CarWash price - (Production Critical Issue!)

	04/03/2021		4.0.33.317
	Shahaf			1. RFUEL-2964 - SpartanNash-Prompt to scan loyalty card a second time
	
	04/03/2021		4.0.33.318
					1. RFUEL-2859 -	Site 4555 - Fuel Pump Readings are not coming in

	22/03/2021		4.0.33.319
	cemd			1. RFUEL-3075 [merge RFUEL-2884] - reworked receipt message handling to prevent exceptions

	09/03/2021		4.0.33.320
	Naga Raju P		1. RFUEL-2299 - P@P with loyalty does not honor 25G limit

	30/03/2021		4.0.33.330
	Shahaf			1. RFUEL-2680 - CFI OEMV - Reports that all PAP Fail auth where Preset is done prior to using Customer Payment Card

	31/03/2021		4.0.33.331
	Naga Raju P		1. RFUEL-3055 - Fuel 1033 is not sending ItemData for Carwash to EFT

	07/04/2021		4.0.33.340
	Sang			1. RFUEL-3032 - No Dots for AlarmSrv in 1033 version of FCC

	07/04/2021		4.0.33.341
	AdirH			1. RFUEL-3089: POS not price change confirmation
	
	07/04/2021		4.0.33.342
	marik			1. RFUEL-3225 	GATE - unable to approve P@P transaction


	12/04/2021		4.0.33.343
	MariaB			1.RFUEL-3144 - Spartannash - After scanning loyalty card and letting pump time out, Pump will not accept loyalty or payment cards
					2.RFUEL-3195 - PAK without loyalty but with 10 cents off discount - cause double dip in POS - missing extra info buffer

	12/04/2021		4.0.33.350
	NoamA			1. RFUEL-3194   Emerald - limit minimal unit price per G/L after loyalty discounts are applied


	12/04/2021		4.0.33.360
	marik			1. RFUEL-2981 Non chip failing due to items being sent and therefore pump not authorizing due to restrictions - recompile with new version as PS 350 was released.
	
	13/04/2021		4.0.33.361
	Sang			1. RFUEL-3028 - Pumps Locked Shortly After Changing Loyalty Program

	14/04/2021		4.0.33.362
	NoamA			1. RFUEL-3194 task RFUEL-3257  Change premise - Make unit price limit comparing to lowest unit price.

	19/04/2021		4.0.33.363
	cemd			1. RFUEL-3267 - fixed handling of chip card insertion when prompting for loyalty/altid
	MariaB			1. RFUEL-3144 - Spartannash - After scanning loyalty card and letting pump time out, Pump will not accept loyalty or payment cards

	19/04/2021		4.0.33.364
	cemd			1. RFUEL-3274 - fixed prepay 'pump is locked' after clearing db by 

	19/04/2021		4.0.33.370
	Marik			1.RFUEL-3389 - 	ENCOR_FUEL-Account Number is being sent to Fuel, but Fuel expects ClubCard Number

	09/05/2021		4.0.33.371 
	SergeyB			1. RFUEL-3317 - (RFUEL-672) [Merge to 1033] Grade prices do not receive fuel discount on consecutive transactions.
					2. RFUEL-3313 - (RFUEL-856) [Merge to 1033] Casey's: Pay at Pump transactions getting stuck on pumps
					3. RFUEL-3312 - (RFUEL-1100) [Merge to 1033] Update Maximum Length of Excentus Reference ID to 20 characters
					4. RFUEL-3311 - (RFUEL-1213) [Merge to 1033] Since Epsilon implementation, pumps don't work properly after Pump configuration (FCC).  A reboot of Fuel or re-load of Epsilon is required.
					5. RFUEL-3314 - (RFUEL-696) [Merge to 1033] Casey's FPCS - Result file was not created after the price change was sent to the store
					6. RFUEL-3315 - (RFUEL-694) [Merge to 1033] Casey's FPCS -Prices on the pumps reverted back but result file indicate success status.

	11/05/2021		4.0.33.371
	NoamA			1.RFUEL-3309 - 	Share Maximum Discountable Amount With Emerald					

	
	17/05/2021		4.0.33.380
	Sang			1. RFUEL-3028 - reworked to raise alarm when database problem detected

	17/05/2021		4.0.33.390
	Sang			1. RFUEL-2817 - Gate - Support Gift Card Fuel Discount and PES Loyalty Fuel Discount [Fuel]

	??/05/2021		4.0.33.???
	SergeyB			1. RFUEL-3093 - On EMV transaction, Reprint receipt is not working
	Karun			2. RFUEL-3452 - Site 5402 - Fuel Crashes During Price Change.
					3. RFUEL-3556 - Run code analysis on PumpSrv 1033
	AdirH			4. RFUEL-3290 - Loyalty Declined, Continue = NO. Pump enabled to dispense.
					5. RFUEL-3049 - Fuel 1033 Having issues with LREX discounts.
					6. RFUEL-3705 - Pump displays "Loyalty card was declined, continue? when loyalty prompt timed out. 

	02/06/2021      4.0.33.???
	NoamA			1. RFUEL-3729 - SpartanNash-PAP fuel not rolling back to $0.010 on the pumps.
					   Discounts were ignoring third digit after decimal point.

    6/6/2021        4.0.33.391
	Svetlana        1. RFUEL-3621 - Gate PAP receipts not printing  

	08/06/2021		4.0.33.392
	Jalil			1. RFUEL-3534 Gift Card tender at Pump where the available gift card balance is the exact amount of the selected car wash program allows overdispensing

	08/06/2021		4.0.33.393
	AdirH			1. RFUEL-3049 - Fuel 1033 Having issues with LREX discounts.

	10/06/2021		4.0.33.393
	Sang			1. RFUEL-2817 - Gate - Fixed couple of EPSILON conditions to include EPSILON DISCOUNT
									Added additional fields to EpsilonDiscount by Request
					2. No Jira    - Refactored NMP to add detailed debugging information
									Added DebugDump for NMPDetail object.
	AdirH			3. RFUEL-3049 - Fuel 1033 Having issues with LREX discounts.

	17/06/2021		4.0.33.394
	AdirH			1. RFUEL-3740 - Big Y - Pay-at-pump canceled after scanning loyalty card and swiping payment card
					2. RFUEL-3757 - BigY - Loyalty Prompts for Scan Twice.
					3. RFUEL-3708 - Big Y - Under dispensed Prepay cannot be processed properly.

	18/06/2021		4.0.33.395
	Sang			1. RFUEL-3871 - Fixed the Loyalty Alt-id not working

	4/07/2021       4.0.33.396
	Svetlana        1. RFUEL-3868 - OEMV - MediaMasterType field is incorrect for all outdoor EMV cards

	09/07/2021		4.0.33.397
	Naga Raju P		1. RFUEL-3874 - OEMV - Loyalty promotion missing lPromotionNumber resulting in missing 0x03 trs

	16/07/2021		4.0.33.398
	Sang			1. RFUEL-3939 - Support getting up to date with PES protocol
	
	12/08/2021		4.0.33.400
	Sang			1. No-Jira - PumpServiceMode class created to abstract the logic
					2. RFUEL-3804 - Support stacking of gift card discount and new payment discount -By Entering Loyalty ATD ID (Phone Num)
					3. RFUEL-4260 - PAP- Loyalty Discount amount is not showing in TRA

	12/08/2021		4.0.33.401
	Sang			1. RFUEL-4328 - PAP- Tender discount is not applying on the transaction

	14/08/2021		4.0.33.402
	Marik			1.RFUEL-4338 Allied- PAP getting "See Cashier" message if previous transaction is canceled.		
					2.RFUEL-4327 PAP- Lowest volume or amount logic is not trigger correctly

	18/08/2021		4.0.33.403
	Sang			1.RFUEL-4351 PAP- getting "Card not authorized" message if previous transaction is canceled.

	24/08/2021		4.0.33.404
	Sang			1. RFUEL-4277 - PAP- Loyalty Card Entry Method is not passing to TRA

	??/11/2021		4.0.33.???
	AdirH			1. RFUEL-5095 - [Marge from 1033 OG] Brookshire Bros: unknown card error when swipe a gift card at pump
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _ENCOR_DEBUG 0

#define PUMPSRV_VERSION				(CString("Server Version ") + GetVersionInfoAsString() + " " + __DATE__) // YYYY.MM.DD.Fix	

#define PUMPSRV_NUMERIC_VERSION		6		//For VP initialization

