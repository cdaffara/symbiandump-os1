// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// @file ctlbsstepverifyloginfo.cpp
// This is the class implementation for the log verification test step
// 
//
 
#include "ctlbsstepverifyloginfo.h"

_LIT(KLogType, "log_type");

/**
Public, non-leaving, static constructor
*/
CT_LbsStep_VerifyLogInfo* CT_LbsStep_VerifyLogInfo::New(CT_LbsServer& aParent)
	{
	CT_LbsStep_VerifyLogInfo* self = NULL;
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	TRAPD(err, self = NewL(aParent));
	if(KErrNone != err)
		{
		self = NULL;
		}
	return self;
	}
/**
 * Static Constructor that may leave
 */
CT_LbsStep_VerifyLogInfo* CT_LbsStep_VerifyLogInfo::NewL(CT_LbsServer& aParent)
	{
	CT_LbsStep_VerifyLogInfo* verifyLog = new(ELeave) CT_LbsStep_VerifyLogInfo(aParent);
	CleanupStack::PushL(verifyLog);
	verifyLog->ConstructL();
	CleanupStack::Pop(verifyLog);

	return verifyLog;
	}
/**
2nd phase Constructor that may leave
*/
void CT_LbsStep_VerifyLogInfo::ConstructL()
	{

	// Create everything for the right hand side of all compares
	_LIT(KTheDest,"07712345678");
	TPtrC theDest(KTheDest);
	
	iX3PLogIntiailisedFromValues = CLbsTransmitLocationLogEvent::NewL(TUid::Uid(0), NULL, theDest);
	iSelfLocateLogIntiailisedFromValues = CLbsSelfLocateLogEvent::NewL(TUid::Uid(0), NULL);
	iExternalLocateLogIntiailisedFromValues = CLbsExternalLocateLogEvent::NewL(TUid::Uid(0), NULL, NULL);
	iNetworkLocateLogIntiailisedFromValues = CLbsNetworkLocateLogEvent::NewL(TUid::Uid(0), NULL);
	iAssistanceDataLogIntiailisedFromValuesFirst = CLbsAssistanceDataLogEvent::NewL(0, 0);	
	
	//Setup other reference position
	iRefPosTwo = new TPositionInfo();
	TPosition refpos;
	refpos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	refpos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	refpos.SetCurrentTime();
	iRefPosTwo->SetPosition(refpos);
	
	
	// read logs using CLogViewEvent
	TInt ErrorCode = itheFs.Connect();
	if(ErrorCode)
		{
		INFO_PRINTF1(_L("\t Error: Open FileServer failed."));
		SetTestStepResult(EFail);
		}
		
    iclient = CLogClient::NewL(itheFs, CActive::EPriorityStandard);
	iview = CLogViewEvent::NewL(*iclient, CActive::EPriorityStandard);
	
	iActiveWaiter = CT_LbsAsyncWaiter::NewL();			// NEW iActiveWaiter
	
	}
/**
 * Destructor
 */
CT_LbsStep_VerifyLogInfo::~CT_LbsStep_VerifyLogInfo()
	{
	iview->Cancel();
	iclient->Cancel();
	delete iview;
	delete iclient;
	itheFs.Close();
	delete iActiveWaiter;
	// Delete everything for the right hand side of all compares
	delete iRefPosTwo;
	delete iSelfLocateLogIntiailisedFromValues;
	delete iExternalLocateLogIntiailisedFromValues;
	delete iNetworkLocateLogIntiailisedFromValues;
	delete iAssistanceDataLogIntiailisedFromValuesFirst;
	delete iX3PLogIntiailisedFromValues;

	}


/**
 * Constructor
 */
CT_LbsStep_VerifyLogInfo::CT_LbsStep_VerifyLogInfo(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_VerifyLogInfo);
	}


TVerdict CT_LbsStep_VerifyLogInfo::doTestStepPreambleL()
	{

	return TestStepResult();
	}

TVerdict CT_LbsStep_VerifyLogInfo::doTestStepPostambleL()
	{

	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_VerifyLogInfo::doTestStepL()
	{
	// Helper test step used to Verify the Position Information Array.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_VerifyLogInfo::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		
		// Left and right parameters of compare functions.
		if (GetIntFromConfig(ConfigSection(), KLogType, iLogType))
			{


			if(iParent.iSharedData->iTestModuleInUse) 
				{
				iWhatAccuracyToUse = T_LbsUtils::EExactAccuracy;
				}
			else
				{
				iWhatAccuracyToUse = T_LbsUtils::ERoughAccuracy;
				}
				
			InitialiseRHS();
			
			switch(iLogType)
				{
				case EAutonomousSelfLocateRequest: // Enum 0
	        		{
	        		AutonomousSelfLocateLogTest();
	        		}
				break;
				
				case EMOLRSelfLocateRequest: // Enum 1
		        	{
					MOLRSelfLocateRequest();
		        	}
		        break;
				
				case EMOLRCellBasedLocationRequest: // Enum 2
					{
					MOLRCellBasedLocationRequest();
					}
				break;
				
				case EMOLRTransmitTo3rdPartyLocationRequest: // Enum 3
				    {
					MOLRTransmitTo3rdPartyLocationRequest();
				    }
		       	break;
		       	
				case EMTLR: // Enum 4
		       		{
					MTLR();
		       		}
		       	break;
		       				
				case EMOLRTrackingLocationRequest: // Enum 5
			       	{
		       		MOLRTrackingLocationRequest();
		       		}
		       	break;
				
				case EAutonomousTrackingLocationRequest: // Enum 6
		       		{
		       		AutonomousTrackingLocationRequest();
		       		}
		       	break;
							
				default:
					{
					INFO_PRINTF1(_L("<FONT><B>Failed to match a test case Id.</B></FONT>"));
					SetTestStepResult(EFail);				
					}
				break;
				}// End of switch
			
			ClearLog();
			}// End of GetIntFromConfig()...
				
		} // End of "If TestStepResult..."


	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_VerifyLogInfo::doTestStepL()"));

	return TestStepResult();
	}
TVerdict CT_LbsStep_VerifyLogInfo::ReadFromLogL(TInt aUidForTheLogType)
	{		
	
	/**
	 * Filter the type of log we are looking for based on UID 
	 */
	CLogFilter* filter = CLogFilter::NewL();			// NEW FILTER
	CleanupStack::PushL(filter);
	filter->SetEventType(TUid::Uid(aUidForTheLogType));
	
	/*
	 * Obtain the last added event from LBS logger 		
	 */
	TBool res = iview->SetFilterL(*filter, iActiveWaiter->iStatus);
	if (!res)
		{
		INFO_PRINTF1(_L("\t Error: Set filter failed."));
		return EFail;
		}

	iActiveWaiter->StartAndWait();
	if (iActiveWaiter->iStatus != KErrNone)
		{
		INFO_PRINTF1(_L("\t Error: Create Log iview failed."));
		return EFail;
		}

	/* 
	 * Count how many of these events there are and 
	 * store each event in the relevant array 
	 */	 
	
	TInt count = iview->CountL();
	TLinearOrder<CLbsSelfLocateLogEvent> selfLocLogOrder(SelfLocateLogEventTimestampCompare);
	for(int i=0 ; i<count ; i++)
		{
		// Start at first event to have occured...
		if(i==0 && count > 0)
			{
			// Get first event in order of occurance
			if(!iview->LastL(iActiveWaiter->iStatus))
				{
				INFO_PRINTF2(_L("\t Error: Read last log failed with error %d."), iActiveWaiter->iStatus.Int());
				return EFail;
				}
			}
			
		if(i>0 && i<count)
			{
			// Cancel outstanding request...
			iActiveWaiter->Cancel();
			iActiveWaiter->StartAndWait();
			if (iActiveWaiter->iStatus != KErrNone)
				{
				INFO_PRINTF1(_L("\t Error: Failed to cancel outstanding request."));
				return EFail;
				}
				
			// Move onto the next event in order of occurance and store it in succession
			if(!iview->PreviousL(iActiveWaiter->iStatus))
				{
				INFO_PRINTF2(_L("\t Error: Read previous log failed with error %d."), iActiveWaiter->iStatus.Int());
				return EFail;	
				}
			}

		CLogEvent* logEvent = CLogEvent::NewL();		// NEW LOGEVENT
		CleanupStack::PushL(logEvent);
		logEvent->CopyL(iview->Event());
		
		// Pass ownership to the relevant array
		switch(aUidForTheLogType) 
		{
		case KLogLbsSelfLocateEventType:
			{
			RPointerArray<CLbsSelfLocateLogEvent>& logInfoArr = iParent.iSharedData->iSelfLocateLogInfoArr;	
			CLbsSelfLocateLogEvent* selfLocateEv = static_cast<CLbsSelfLocateLogEvent*>(CLbsLogEvent::NewL(*logEvent));
			CleanupStack::PushL(selfLocateEv);
			// SelfLoc Log events seem to be returned out of order, so wee need to sort them
			logInfoArr.InsertInOrder(selfLocateEv, selfLocLogOrder);
			CleanupStack::Pop(selfLocateEv);
			}
			break;

		case KLogLbsNetworkLocateEventType:
			{
			RPointerArray<CLbsNetworkLocateLogEvent>& logInfoArr = iParent.iSharedData->iNetworkLocateLogInfoArr;	
			CLbsNetworkLocateLogEvent* networkLocateEv = static_cast<CLbsNetworkLocateLogEvent*>(CLbsLogEvent::NewL(*logEvent));
			CleanupStack::PushL(networkLocateEv);
			logInfoArr.AppendL(networkLocateEv); // Store it in the array
			CleanupStack::Pop(networkLocateEv);			}
			break;

		case KLogLbsExternalLocateEventType:
			{
			RPointerArray<CLbsExternalLocateLogEvent>& logInfoArr = iParent.iSharedData->iExternalLocateLogInfoArr;			
			CLbsExternalLocateLogEvent* externalLocateEv = static_cast<CLbsExternalLocateLogEvent*>(CLbsLogEvent::NewL(*logEvent));
			CleanupStack::PushL(externalLocateEv);
			logInfoArr.AppendL(externalLocateEv); // Store it in the array
			CleanupStack::Pop(externalLocateEv);
			}
		break;

		case KLogLbsTransmitLocationEventType:
			{
			RPointerArray<CLbsTransmitLocationLogEvent>& logInfoArr = iParent.iSharedData->iX3PLocateLogInfoArr;			
			CLbsTransmitLocationLogEvent* x3pLocateEv = static_cast<CLbsTransmitLocationLogEvent*>(CLbsLogEvent::NewL(*logEvent));
			CleanupStack::PushL(x3pLocateEv);
			logInfoArr.AppendL(x3pLocateEv); // Store it in the array
			CleanupStack::Pop(x3pLocateEv);
			}
		break;

		case KLogLbsAssistanceDataEventType:
			{
			RPointerArray<CLbsAssistanceDataLogEvent>& logInfoArr = iParent.iSharedData->iAssistanceDataLogInfoArr;	
			CLbsAssistanceDataLogEvent* assistanceLogEv = static_cast<CLbsAssistanceDataLogEvent*>(CLbsLogEvent::NewL(*logEvent));
			CleanupStack::PushL(assistanceLogEv);
			logInfoArr.AppendL(assistanceLogEv); // Store it in the array
			CleanupStack::Pop(assistanceLogEv);
			}
		break;	

		default:
			{
			INFO_PRINTF1(_L("\t Error: Couldnt match UID to a testable logtype."));
			i = count; // Force loop exit
			SetTestStepResult(EFail);
			}		
		}
		
		// Cleanup scoped variables
		CleanupStack::PopAndDestroy(logEvent);
		}// End of for loop
		
		// Cancel outstanding request...
		iActiveWaiter->Cancel();
		iActiveWaiter->StartAndWait();
		if (iActiveWaiter->iStatus != KErrNone)
			{
			INFO_PRINTF1(_L("\t Error: Failed to cancel any outstanding requests."));
			return EFail;
			}

	// Cleanup local variables
	CleanupStack::PopAndDestroy(filter);
				
	return EPass;	
	}
	
void CT_LbsStep_VerifyLogInfo::SelfLocateLogTest()
	{

	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	INFO_PRINTF1(_L("SelfLocateLogTest>>"));
	TVerdict res;
	TBool compareResult;
	
	//	Empty SharedData values between update options tests
	iParent.iSharedData->iSelfLocateLogInfoArr.Reset();
			
	// reads all self-locate log entries into iSelfLocateLogInfoArr:
	res = ReadFromLogL(KLogLbsSelfLocateEventType);
	
	if (res == EFail) // if readfromlog fails...
		{
		INFO_PRINTF1(_L("<FONT><b>SelfLocateRequest > ReadFromLog Failed</b></font>"));
		SetTestStepResult(EFail);
		}
	else
		{
				
		// Compare DB log event and expected log event
		INFO_PRINTF1(_L("SelfLocateRequest > About to compare..."));
		
	
		RPointerArray<CLbsSelfLocateLogEvent>& logInfoArr = iParent.iSharedData->iSelfLocateLogInfoArr;
		
		TInt sizeOfArray = logInfoArr.Count();
		
		if(	((sizeOfArray != 2) && (iLogType == EAutonomousTrackingLocationRequest)) ||
			((sizeOfArray != 3) && (iLogType == EMOLRTrackingLocationRequest))
			)
			{
			INFO_PRINTF2(_L("<FONT><b>Update Log test failed, there were %d Self Locate events</b></font>"),sizeOfArray);
			SetTestStepResult(EFail);
			}
		else
			{
			//	Change reference location to be expected value
			TPositionInfo* posInfoSecond = new TPositionInfo();
			SecondPositionFromArray(posInfoSecond);
			
			for(TInt i = 0 ; i < sizeOfArray ; i++)
				{
				// note that events are in reverse order, therefore the reference position is last rather than first of 2
				if(iLogType == EMOLRTrackingLocationRequest && i == 2)
					{
					iSelfLocateLogIntiailisedFromValues->SetPositionInfo(posInfoSecond);		
					}
				
			
				if(iLogType == EMOLRSelfLocateRequest && i == 1)
					{
					iSelfLocateLogIntiailisedFromValues->SetPositionInfo(posInfoSecond);
					}
				
				compareResult = iUtils.Compare_SelfLocateLog(	*(reinterpret_cast<CLbsSelfLocateLogEvent*>(logInfoArr[i])) , 
																*iSelfLocateLogIntiailisedFromValues
															);	
				if(!compareResult)
					{
					INFO_PRINTF2(_L("<FONT><b>SelfLocateRequest > compare returns false for element %d...</b></font>"),i);
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("SelfLocateRequest > compare returns true..."));
					}
				}
				
			delete posInfoSecond;
		
			}
		}
	
	INFO_PRINTF1(_L("SelfLocateLogTest<<"));
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	}


void CT_LbsStep_VerifyLogInfo::NetworkLocateLogTest()
	{
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	INFO_PRINTF1(_L("NetworkLocateLogTest>>"));
	TVerdict res;
	TBool compareResult;

	res = ReadFromLogL(KLogLbsNetworkLocateEventType);
	
	if (res == EFail) // if readfromlog fails...
		{
		INFO_PRINTF1(_L("<FONT><b>NetworkLocateRequest > Readfrom log failed</b></font>"));
		SetTestStepResult(EFail);
		}
	else
		{
		// Compare DB log event and expected log event
		INFO_PRINTF1(_L("NetworkSelfLocateRequest > About to compare..."));
		
		RPointerArray<CLbsNetworkLocateLogEvent>& logInfoArr = iParent.iSharedData->iNetworkLocateLogInfoArr;
		
		TInt sizeOfArray = logInfoArr.Count();
		
		
		for(TInt i = 0 ; i < sizeOfArray ; i++)
			{
			
			compareResult = iUtils.Compare_NetworkLocateLog(	*(reinterpret_cast<CLbsNetworkLocateLogEvent*>(logInfoArr[i])) , 
																*iNetworkLocateLogIntiailisedFromValues
														);	
			if(!compareResult)
				{
				INFO_PRINTF2(_L("<FONT><b>NetworkLocateRequest > compare returns false for element %d...</b></font>"),i);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("NetworkLocateRequest > compare returns true..."));
				}	
			}

		}
		
	INFO_PRINTF1(_L("NetworkLocateLogTest<<"));
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	}

void CT_LbsStep_VerifyLogInfo::ExternalLocateLogTest()
	{

	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	INFO_PRINTF1(_L("ExternalLocateLogTest>>"));
	TVerdict res;
	TBool compareResult;
	
	res = ReadFromLogL(KLogLbsExternalLocateEventType);
	
	if (res == EFail) // if readfromlog fails...
		{
		INFO_PRINTF1(_L("<FONT><b>ExternalLocateRequest > ReadFromLog Failed</b></font>"));
		SetTestStepResult(EFail);
		}
	else
		{
		// Compare DB log event and expected log event
		INFO_PRINTF1(_L("NetworkSelfLocateRequest > About to compare..."));

		RPointerArray<CLbsExternalLocateLogEvent>& logInfoArr = iParent.iSharedData->iExternalLocateLogInfoArr;
		
		TInt sizeOfArray = logInfoArr.Count();
		TPositionInfo* posInfoSecond = new TPositionInfo();
		TPositionInfo* commonPosinfo = CommonPositionInfo();
		for(TInt i = 0 ; i < sizeOfArray ; i++)
			{			
			if(iLogType==EMOLRTrackingLocationRequest && i%2==1)
				{
				TInt gatewayId = 271064388;	//	KLbsGpsLocManagerUidValue	
				TUid gatewayUid = TUid::Uid(gatewayId);
				iExternalLocateLogIntiailisedFromValues->SetRequestOriginator(gatewayUid);
							
				TLbsRequestOutcome tweakedOutcome = ELbsRequestOutcomeInconclusive;
				iExternalLocateLogIntiailisedFromValues->SetRequestOutcome(tweakedOutcome); 	

				SecondPositionFromArray(posInfoSecond);
				iExternalLocateLogIntiailisedFromValues->SetReferenceLocation(posInfoSecond);
				iExternalLocateLogIntiailisedFromValues->SetPositionInfo(commonPosinfo);
				}

			// logtype 5
			if(iLogType==EMOLRTrackingLocationRequest && i%2==0)
				{
				TInt gatewayId = 271064388;	//	KLbsGpsLocManagerUidValueS	
				TUid gatewayUid = TUid::Uid(gatewayId);
				iExternalLocateLogIntiailisedFromValues->SetRequestOriginator(gatewayUid);
				
				//	Change outcome to expected value			
				TLbsRequestOutcome tweakedOutcome = ELbsRequestOutcomeSuccess;
				iExternalLocateLogIntiailisedFromValues->SetRequestOutcome(tweakedOutcome);
				
				//	Change reference location to be expected value
				// Set the reference position, which also be used for any required verification.
				TPosition refPos;
				TPositionInfo* posInfo = new TPositionInfo();
				refPos.SetCoordinate(REFPOS_LAT_OTHER, REFPOS_LONG_OTHER, REFPOS_ALT_OTHER);
				refPos.SetAccuracy(REFPOS_HORZ_ACCURACY_OTHER, REFPOS_VERT_ACCURACY_OTHER);
				refPos.SetCurrentTime();
				static_cast<TPositionInfo*>(posInfo)->SetPosition(refPos);

				iExternalLocateLogIntiailisedFromValues->SetReferenceLocation(posInfo);
				delete posInfo;
				}
				
			// logtype 1
			if(iLogType==EMOLRSelfLocateRequest)
			{
				TInt locManagerId = 271064388;	//	KLbsGpsLocManagerUidValue
				TUid locManagerUid = TUid::Uid(locManagerId);
				iExternalLocateLogIntiailisedFromValues->SetRequestOriginator(locManagerUid);
				
				//	Change outcome to expected value			
				TLbsRequestOutcome tweakedOutcome = ELbsRequestOutcomeSuccess;
				iExternalLocateLogIntiailisedFromValues->SetRequestOutcome(tweakedOutcome);

				//	Expect position to be NULL for first event
				//	Set expected outcome of second External event
				if(i==1)
					{
					TLbsRequestOutcome tweakedOutcome = ELbsRequestOutcomeInconclusive;
					iExternalLocateLogIntiailisedFromValues->SetRequestOutcome(tweakedOutcome); 	
					iExternalLocateLogIntiailisedFromValues->SetPositionInfo(commonPosinfo);
					iExternalLocateLogIntiailisedFromValues->SetReferenceLocation(iRefPosTwo);
					}
			}
			
			if(iLogType==EMOLRTransmitTo3rdPartyLocationRequest && i==1)
				{
				TLbsRequestOutcome tweakedOutcome = ELbsRequestOutcomeInconclusive;
				iExternalLocateLogIntiailisedFromValues->SetRequestOutcome(tweakedOutcome);
				SecondPositionFromArray(posInfoSecond);
				iExternalLocateLogIntiailisedFromValues->SetReferenceLocation(posInfoSecond);			 	
				}
				
			if(iLogType==EMOLRTransmitTo3rdPartyLocationRequest && i==1)
				{
				iExternalLocateLogIntiailisedFromValues->SetPositionInfo(commonPosinfo);
				}

				
			if(iLogType == EMTLR)
				{
				iExternalLocateLogIntiailisedFromValues->SetPositionInfo(commonPosinfo);
				InitialiseRHSforMTLR();
				}

			
			compareResult = iUtils.Compare_ExternalLocateLog(	*(reinterpret_cast<CLbsExternalLocateLogEvent*>(logInfoArr[i])) , 
																*iExternalLocateLogIntiailisedFromValues
															);	
			if(!compareResult)
				{
				INFO_PRINTF2(_L("<FONT><b>ExternalLocateLog > compare returns false for element %d...</b></font>"),i);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("ExternalLocateLog > compare returns true..."));
				}
			} // End for loop
		delete commonPosinfo;
		delete posInfoSecond;	
		}
		
	INFO_PRINTF1(_L("ExternalLocateLogTest<<"));
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));

	}
	
void CT_LbsStep_VerifyLogInfo::X3PLocateLogTest()
	{
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	INFO_PRINTF1(_L("X3PLocateLogTest>>"));
	TVerdict res;
	TBool compareResult;

	res = ReadFromLogL(KLogLbsTransmitLocationEventType);
	
	if (res == EFail) // if readfromlog fails...
		{
		INFO_PRINTF1(_L("<FONT><b>X3PLocateRequest > ReadFromLog Failed</B></FONT>"));
		SetTestStepResult(EFail);
		}
	else
		{
		// Compare DB log event and expected log event
		INFO_PRINTF1(_L("X3PLocateLog > About to compare..."));
		
		RPointerArray<CLbsTransmitLocationLogEvent>& logInfoArr = iParent.iSharedData->iX3PLocateLogInfoArr;

		TInt sizeOfArray = logInfoArr.Count();
		
		for(TInt i = 0 ; i < sizeOfArray ; i++)
			{
			compareResult = iUtils.Compare_TransmitLocationLog(	*(reinterpret_cast<CLbsTransmitLocationLogEvent*>(logInfoArr[i])) , 
													*iX3PLogIntiailisedFromValues
														);	
			if(!compareResult)
				{
				INFO_PRINTF2(_L("<FONT><b>X3PLocateLogTest > compare returns false for element %d...</b></font>"),i);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("X3PLocateLogTest > compare returns true..."));
				}
			}	
		}

	INFO_PRINTF1(_L("X3PLocateLogTest<<"));
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	}

void CT_LbsStep_VerifyLogInfo::AssistanceDataLogTest()
	{

	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	INFO_PRINTF1(_L("AssistanceDataLogTest>>"));
	TVerdict res;
	TBool compareResult = ETrue;

	res = ReadFromLogL(KLogLbsAssistanceDataEventType);
	
	if (res == EFail) // if readfromlog fails...
		{
		INFO_PRINTF1(_L("<FONT><b>AssistanceDataLog > ReadFromLog Failed</b></font>"));
		SetTestStepResult(EFail);
		}
	else
		{
		// Compare DB log event and expected log event
		INFO_PRINTF1(_L("AssistanceDataLog > About to compare..."));
		
		RPointerArray<CLbsAssistanceDataLogEvent>& logInfoArr = iParent.iSharedData->iAssistanceDataLogInfoArr;
		
		TInt sizeOfArray = logInfoArr.Count();
		TPositionInfo* commonPosinfo = CommonPositionInfo();
		for(TInt i = 0 ; i < sizeOfArray ; i++)
			{
		
			if(	iLogType == EMOLRTrackingLocationRequest || 
				iLogType ==EMOLRTransmitTo3rdPartyLocationRequest )
				{
				iAssistanceDataLogIntiailisedFromValuesFirst->SetRequestAssistanceDataType(0);	
				}
							
			
			/*
		 	 *	If we run X3P log test on its own we get logs that are different than when its run 
		 	 *	in sequence with other tests.
			 */
			if(sizeOfArray > 1 && iLogType ==EMOLRTransmitTo3rdPartyLocationRequest) 
				{
				if(i==1)
					{
					//	Second log has a mask of 0
					iAssistanceDataLogIntiailisedFromValuesFirst->SetRequestAssistanceDataType(0);
					compareResult = iUtils.Compare_AssistanceDataLog(	*(reinterpret_cast<CLbsAssistanceDataLogEvent*>(logInfoArr[i])) , 
																		*iAssistanceDataLogIntiailisedFromValuesFirst
																	);
					}
				else
					{
					iAssistanceDataLogIntiailisedFromValuesFirst->SetRequestAssistanceDataType(8);
					compareResult = iUtils.Compare_AssistanceDataLog(	*(reinterpret_cast<CLbsAssistanceDataLogEvent*>(logInfoArr[i])) , 
																		*iAssistanceDataLogIntiailisedFromValuesFirst
																	);											
					}	
				} 

			if(!compareResult)
				{
				INFO_PRINTF2(_L("<FONT><b>AssistanceDataLogTest > compare returns false for element %d...</b></FONT>"),i);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("AssistanceDataLogTest > compare returns true..."));
				}
			}//	End of for loop
			delete commonPosinfo;
		}
	
	INFO_PRINTF1(_L("AssistanceDataLogTest<<"));
	INFO_PRINTF1(_L("* * * * * * * * * * * * * * * * * * * * * *"));
	}



// Top level functionality

void CT_LbsStep_VerifyLogInfo::AutonomousSelfLocateLogTest()
	{
	SelfLocateLogTest();
	}

void CT_LbsStep_VerifyLogInfo::MOLRSelfLocateRequest() 	// ** Similar **
	{
    ExternalLocateLogTest();
	AssistanceDataLogTest();
	SelfLocateLogTest();
	}

void CT_LbsStep_VerifyLogInfo::MOLRCellBasedLocationRequest()
	{
	NetworkLocateLogTest();
	}

void CT_LbsStep_VerifyLogInfo::MOLRTransmitTo3rdPartyLocationRequest()
	{
	AssistanceDataLogTest();
		
    ExternalLocateLogTest();
    	
    X3PLocateLogTest();    
	}

void CT_LbsStep_VerifyLogInfo::MTLR()	// ** Similar **
	{
	//	SelfLocateLogTest();
	ExternalLocateLogTest();
	AssistanceDataLogTest();
	}

void CT_LbsStep_VerifyLogInfo::MOLRTrackingLocationRequest()	// ** Similar **
	{
	SelfLocateLogTest();
	AssistanceDataLogTest();
	ExternalLocateLogTest();
    }

void CT_LbsStep_VerifyLogInfo::AutonomousTrackingLocationRequest()
	{
	SelfLocateLogTest();
	}




void CT_LbsStep_VerifyLogInfo::InitialiseRHS()
	{
	//	Variables needed for all reads
	//	CommonToAll
	TBool direction; // Inbound or outbound
	
	// wont be filled in components ELbsRequestOutcomeSuccess/ELbsRequestOutcomeFailed/...Inconclusive etc
	TLbsRequestOutcome theSelfLocateRequestOutcome;
	TLbsRequestOutcome theX3PRequestOutcome;
	TLbsRequestOutcome theExternalLocateRequestOutcome;
	TLbsRequestOutcome theNetworkLocateRequestOutcome;
	TLbsRequestOutcome theAssistanceDataRequestOutcome;

	//	Common to 4/5 but not assistance
	TInt theCost;	// not implemented

	//	Common to 4/5 but not external
	TUid theLocalAppUid; 	// work out from PID

	//	Common only to External Locate
	TInt theOriginator;	// From network gateway
	TInt theSessionNum;	// TNetLocSessionID?
	TInt theSessionOwner;	// TNetLocSessionID?
	TInt theAction;		// Set within privacy request
	TInt theAdvice;		// Set within privacy request
	TPtrC theClientName;	// Set within privacy request
	TPtrC theExternalId;	// Set within privacy request
	TPtrC theRequestor;	// Set within privacy request
	TInt theResult;		// ENum within privacy response

	//	Common only to X3P Locate
	TUid theX3PUid;	 	// work out from PID
	
	//	Common only to Assistance
	TInt theFirstRequestMask;	// Set within GPSlocmanager	// Set within GPSlocmanager for wanted
	TInt theReceiveMask;	// Set within GPSlocmanager	// Set within GPSlocmanager for recieved
	

	//	Specialised variables
	theCost = 0;
	theLocalAppUid = TUid::Uid(RProcess().SecureId()); // Get this from ProcessId each time
	// The X3PUid used to be hard-coded to 271065697, but this is not valid anymore, since there are 
	// two test servers using this code. So using the UID of the current process instead
	theX3PUid = theLocalAppUid;
	TInt locServerAppId = 271064387;
	TUid locServerAppUid = TUid::Uid(locServerAppId);

	direction = EFalse;
		
	theSelfLocateRequestOutcome = ELbsRequestOutcomeSuccess;	// or ELbsRequestOutcomeFailed or Inconclusive etc
	theX3PRequestOutcome = ELbsRequestOutcomeSuccess;// Set it to this for X3P test
	theExternalLocateRequestOutcome = ELbsRequestOutcomeSuccess;
	theNetworkLocateRequestOutcome = ELbsRequestOutcomeSuccess;
	theAssistanceDataRequestOutcome = ELbsRequestOutcomeInconclusive; // Set it to this for X3P test

	TPositionInfo* commonPosinfo = CommonPositionInfo();
	
	
	//	Set up relevant 5 Log variables for comparison using position info etc
	//	SELF LOCATE LOG
	iSelfLocateLogIntiailisedFromValues->SetPositionInfo(commonPosinfo); 
	iSelfLocateLogIntiailisedFromValues->SetDirection(direction);
	iSelfLocateLogIntiailisedFromValues->SetLocalApp(theLocalAppUid);	// This apps UID
	iSelfLocateLogIntiailisedFromValues->SetCostInformation(theCost); // set cost information if there are any;
	iSelfLocateLogIntiailisedFromValues->SetRequestOutcome(theSelfLocateRequestOutcome); // by passing the location request outcome;	
	
	
	//	EXTERNAL LOG
	iExternalLocateLogIntiailisedFromValues->SetDirection(direction);
	iExternalLocateLogIntiailisedFromValues->SetCostInformation(theCost);
	iExternalLocateLogIntiailisedFromValues->SetRequestOutcome(theExternalLocateRequestOutcome); // by passing the location request outcome;	
	//	No local app id to set for external log
	//	Common only to External Locate
	theOriginator = 271064389;		//	From Network request handler
	theSessionOwner = 0;	//	268440115 TNetLocSessionID?
	theSessionNum = 0;	// TNetLocSessionID?
	theAction = 0;		// Set within privacy request 0
	theAdvice = 0;		// Set within privacy request 0 = ERequestAdviceNotUsed, 1 = ERequestActionAllow
	
	_LIT(KClientName,"");
	_LIT(KRequestor,"");
	_LIT(KExternalId,"");
	theClientName.Set(KClientName);	// Set within privacy request
	theRequestor.Set(KRequestor);	// Set within privacy request
	theExternalId.Set(KExternalId);	// Set within privacy request
	theResult = 0;		// ENum within privacy response, 1 = ERequestAccepted
	
	// set privacy request parameters
	TLbsLoggingPrivacyRequestParams reqParams; 
	TLbsNetSessionId sessionId(TUid::Uid(0), (TUint32)theSessionNum);
	reqParams.iSessionId.SetSessionOwner(TUid::Uid(theSessionOwner));
	reqParams.iSessionId.SetSessionNum(theSessionNum);
	reqParams.iRequestPrivacy.SetRequestAction((TLbsNetPosRequestPrivacy::TLbsRequestAction)theAction);
	reqParams.iRequestPrivacy.SetRequestAdvice((TLbsNetPosRequestPrivacy::TLbsRequestAdvice)theAdvice);
	
	TPtrC8 ptrReq((TUint8*)theRequestor.Ptr(), theRequestor.Size());			
	TPtrC8 ptrClient((TUint8*)theClientName.Ptr(), theClientName.Size());			
	TPtrC8 ptrId((TUint8*)theExternalId.Ptr(), theExternalId.Size());			
	reqParams.iRequestInfo.SetRequesterId(reinterpret_cast<const TDesC8&>(ptrReq));
	reqParams.iRequestInfo.SetClientName(reinterpret_cast<const TDesC8&>(ptrClient));
	reqParams.iRequestInfo.SetClientExternalId(reinterpret_cast<const TDesC8&>(ptrId));

	//	set privacy response parameters
	TLbsLoggingPrivacyResponseParams respParams;
	respParams.iSessionId.SetSessionOwner(TUid::Uid(0));
	respParams.iSessionId.SetSessionNum(0);
	respParams.iResult = static_cast<CLbsNetworkProtocolBase::TLbsPrivacyResponse>(theResult);

	iExternalLocateLogIntiailisedFromValues->SetRequestOriginator(TUid::Uid(theOriginator));
	iExternalLocateLogIntiailisedFromValues->SetPrivacyRequestParams(reqParams); // set privacy request parameters from request originator
	iExternalLocateLogIntiailisedFromValues->SetPrivacyResponseParams(respParams); // set privacy response parameters from privacy controller
	iExternalLocateLogIntiailisedFromValues->SetReferenceLocation(commonPosinfo);

	
	//	X3P LOG	
	iX3PLogIntiailisedFromValues->SetPositionInfo(commonPosinfo); 
	iX3PLogIntiailisedFromValues->SetDirection(direction);
	iX3PLogIntiailisedFromValues->SetCostInformation(theCost); // set cost information if there are any;
	iX3PLogIntiailisedFromValues->SetRequestOutcome(theX3PRequestOutcome); // by passing the location request outcome;	
	//	X3P log only
	iX3PLogIntiailisedFromValues->SetX3PApp(theX3PUid);	// This apps Uid

	
		
	//	FIRST ASSISTANCE DATA LOG
	iAssistanceDataLogIntiailisedFromValuesFirst->SetDirection(direction);
	iAssistanceDataLogIntiailisedFromValuesFirst->SetRequestOutcome(theAssistanceDataRequestOutcome); // by passing the location request outcome;	
	//	No cost to set for assistance log
	//	Common only to Assistance
	theFirstRequestMask = 8;	//	Set within GPSlocmanager for wanted (8 for first in array, 0 for second)
	theReceiveMask = 8;	//	Set within GPSlocmanager for recieved
	iAssistanceDataLogIntiailisedFromValuesFirst->SetRequestAssistanceDataType(theFirstRequestMask);
	iAssistanceDataLogIntiailisedFromValuesFirst->SetReceiveAssistanceDataType(theReceiveMask);



	//	NETWORK LOCATE LOG
	commonPosinfo->SetModuleId(TUid::Uid(0));
	
	iNetworkLocateLogIntiailisedFromValues->SetPositionInfo(commonPosinfo); 
	iNetworkLocateLogIntiailisedFromValues->SetDirection(direction);
	iNetworkLocateLogIntiailisedFromValues->SetCostInformation(theCost); // set cost information if there are any;
	iNetworkLocateLogIntiailisedFromValues->SetRequestOutcome(theNetworkLocateRequestOutcome); // by passing the location request outcome;	
	iNetworkLocateLogIntiailisedFromValues->SetLocalApp(locServerAppUid);	// This apps UID
	
	delete commonPosinfo;			
	}
	
void CT_LbsStep_VerifyLogInfo::InitialiseRHSforMTLR()
	{
	//	For MTLR, most of the expected external logs values need to be completely different.

	TInt theOriginator;	// From network gateway
	TInt theSessionNum;	// TNetLocSessionID?
	TInt theSessionOwner;	// TNetLocSessionID?
	TInt theAction;		// Set within privacy request
	TInt theAdvice;		// Set within privacy request
	TPtrC8 theClientName;	// Set within privacy request
	TPtrC8 theExternalId;	// Set within privacy request
	TPtrC8 theRequestor;	// Set within privacy request
	TInt theRequestResult;		// ENum within privacy response

	theOriginator = 271064432;		//	From Protocol Module
	theSessionOwner = 271064432;	//	Protocol Module
	theSessionNum = 1;	// TNetLocSessionID?
	theAction = 1;		// Set within privacy request 0
	theAdvice = 1;		// Set within privacy request 0 = ERequestAdviceNotUsed, 1 = ERequestActionAllow
	
	_LIT8(KMTLRClientName,"MT-LR test client name");
	_LIT8(KMTLRRequestor,"MT-LR test requester id");
	_LIT8(KMTLRExternalId,"MT-LR test client external id");
	theClientName.Set(KMTLRClientName);	// Set within privacy request
	theRequestor.Set(KMTLRRequestor);	// Set within privacy request
	theExternalId.Set(KMTLRExternalId);	// Set within privacy request
	theRequestResult = 1;		// ENum within privacy response, 1 = ERequestAccepted


	// set privacy request parameters
	TLbsLoggingPrivacyRequestParams reqParams; 
	TLbsNetSessionId sessionId(TUid::Uid(0), (TUint32)theSessionNum);
	reqParams.iSessionId.SetSessionOwner(TUid::Uid(theSessionOwner));
	reqParams.iSessionId.SetSessionNum(theSessionNum);
	reqParams.iRequestPrivacy.SetRequestAction((TLbsNetPosRequestPrivacy::TLbsRequestAction)theAction);
	reqParams.iRequestPrivacy.SetRequestAdvice((TLbsNetPosRequestPrivacy::TLbsRequestAdvice)theAdvice);
	
	TPtrC8 ptrReq((TUint8*)theRequestor.Ptr(), theRequestor.Size());			
	TPtrC8 ptrClient((TUint8*)theClientName.Ptr(), theClientName.Size());			
	TPtrC8 ptrId((TUint8*)theExternalId.Ptr(), theExternalId.Size());			
	reqParams.iRequestInfo.SetRequesterId(reinterpret_cast<const TDesC8&>(ptrReq));
	reqParams.iRequestInfo.SetClientName(reinterpret_cast<const TDesC8&>(ptrClient));
	reqParams.iRequestInfo.SetClientExternalId(reinterpret_cast<const TDesC8&>(ptrId));

	//	set privacy response parameters
	TLbsLoggingPrivacyResponseParams respParams;
	respParams.iSessionId.SetSessionOwner(TUid::Uid(theSessionOwner ));
	respParams.iSessionId.SetSessionNum(theSessionNum );
	respParams.iResult = static_cast<CLbsNetworkProtocolBase::TLbsPrivacyResponse>(theRequestResult);

	iExternalLocateLogIntiailisedFromValues->SetRequestOriginator(TUid::Uid(theOriginator));
	iExternalLocateLogIntiailisedFromValues->SetPrivacyRequestParams(reqParams); // set privacy request parameters from request originator
	iExternalLocateLogIntiailisedFromValues->SetPrivacyResponseParams(respParams); // set privacy response parameters from privacy controller
	}
	
void CT_LbsStep_VerifyLogInfo::ClearLog()
 	{
	// Setup a time in the future, before which all events will be deleted
 	_LIT(KDateCorrect1,"20900101:"); 
 	TTime time;
	TBuf <10> theDate(KDateCorrect1);
	TInt err=time.Set(theDate); 
	
	// Ensure time has been set correctly
	if(err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to set time"));
		}
		
	if(err == KErrGeneral)
		{
		INFO_PRINTF1(_L("Time syntax is incorrect"));
		}
	
	// Set the filter to view ALL logs
	CLogFilter* filter = CLogFilter::NewL();			// NEW FILTER
	CleanupStack::PushL(filter);
	TBool res = iview->SetFilterL(*filter, iActiveWaiter->iStatus);
	if (res == EFalse)
		{
		INFO_PRINTF1(_L("\t No such events to filter in the view."));
		iclient->Cancel();
		}
	else
		{
		// else If there are logs, flush them
			
		// Cancel outstanding requests
		iclient->Cancel();	
		iActiveWaiter->StartAndWait();
		if (iActiveWaiter->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("\t Error: Cancel returned error %d."),iActiveWaiter->iStatus.Int());
			SetTestStepResult(EFail);
			}

		// Clear ALL logs
		iclient->ClearLog(time, iActiveWaiter->iStatus);
		iActiveWaiter->StartAndWait();
		if (iActiveWaiter->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("\t Error: Clear log returned error %d."),iActiveWaiter->iStatus.Int());
			SetTestStepResult(EFail);
			}	
			
		//Confirm log is cleared
		if(iview->CountL() != 0)
			{
			INFO_PRINTF2(_L("<FONT><B>ERROR: Log still has %d entries</B></FONT>"),iview->CountL());
			SetTestStepResult(EFail);
			}
				
		}
	
	CleanupStack::PopAndDestroy(filter);	

	}
void CT_LbsStep_VerifyLogInfo::SecondPositionFromArray(TPositionInfo* aPosInfoSecond)
{

	//All this in TPosInfo Structure
	TUid theModuleId;	// use from VerifyLogInfo array
	TPositionModuleInfo::TTechnologyType theTechType;
	TPositionModeReason theModeReason;
	TReal32 theVerAccuracy;
	TReal32 theHorAccuracy;
	TReal64 theLat;
	TReal64 theLong;
	TReal theAltitude;
	
	//	Initialise variables from Verify array
	RPointerArray<TAny>& logInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* tPosInfo = (reinterpret_cast<TPositionInfo*>(logInfoArr[0]));
	TPosition tPos;
	tPosInfo->GetPosition(tPos);
		
	theModuleId = 		TUid::Uid(271064388); // 271064388;
	theTechType = 		tPosInfo->PositionMode();
	theModeReason = 	tPosInfo->PositionModeReason();
	theVerAccuracy = 	REFPOS_VERT_ACCURACY;	// For second x3p reference position comparison
	theHorAccuracy = 	REFPOS_HORZ_ACCURACY;	// For second x3p reference position comparison
	theLat = 			REFPOS_LAT;	// For second x3p reference position comparison
	theLong = 			REFPOS_LONG;	// For second x3p reference position comparison
	theAltitude = 		REFPOS_ALT;	// For second x3p reference position comparison


	//	Setup position information using intialised variables
	aPosInfoSecond->SetModuleId(theModuleId);
	aPosInfoSecond->SetUpdateType(EPositionUpdateGeneral);
	aPosInfoSecond->SetPositionMode((TPositionModuleInfo::TTechnologyType)theTechType);
	aPosInfoSecond->SetPositionModeReason((TPositionModeReason)theModeReason);
	TPosition position;
	position.SetCurrentTime();
	position.SetAccuracy(theHorAccuracy,theVerAccuracy);
	position.SetCoordinate(theLat,theLong, theAltitude);
	static_cast<TPositionInfo*>(aPosInfoSecond)->SetPosition(position);
}


TPositionInfo* CT_LbsStep_VerifyLogInfo::CommonPositionInfo()
{
	//All this in TPosInfo Structure
	TUid theModuleId;	// use from VerifyLogInfo array
	TPositionModuleInfo::TTechnologyType theTechType;
	TPositionModeReason theModeReason;
	TReal32 theVerAccuracy;
	TReal32 theHorAccuracy;
	TReal64 theLat;
	TReal64 theLong;
	TReal theAltitude;

	//	All this in TPosInfo Structure use from VerifyLogInfo array
	RPointerArray<TAny>& logInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	
	//	Initialise variables from Verify array
	TPositionInfo* tPosInfo = (reinterpret_cast<TPositionInfo*>(logInfoArr[0]));
	TPosition tPos;
	tPosInfo->GetPosition(tPos);
		
	theModuleId = 		TUid::Uid(271064388); // 271064388;
	theTechType = 		tPosInfo->PositionMode();
	theModeReason = 	tPosInfo->PositionModeReason();
	theVerAccuracy = 	tPos.VerticalAccuracy();
	theHorAccuracy = 	tPos.HorizontalAccuracy();
	theLat = 			tPos.Latitude();
	theLong = 			tPos.Longitude();
	theAltitude = 		tPos.Altitude();


	//	Setup position information using intialised variables
	TPositionInfo* posInfo = new TPositionInfo();
	posInfo->SetModuleId(theModuleId);
	posInfo->SetUpdateType(EPositionUpdateGeneral);
	posInfo->SetPositionMode((TPositionModuleInfo::TTechnologyType)theTechType);
	posInfo->SetPositionModeReason((TPositionModeReason)theModeReason);
	TPosition position;
	position.SetCurrentTime();
	position.SetAccuracy(theHorAccuracy,theVerAccuracy);
	position.SetCoordinate(theLat,theLong, theAltitude);
	static_cast<TPositionInfo*>(posInfo)->SetPosition(position);
	
	return posInfo;
}

/** Compares the timestamps of two log events
 	returns 1 if the lhs is newer, -1 if the lhs is older or 0 if both are the same */
TInt CT_LbsStep_VerifyLogInfo::SelfLocateLogEventTimestampCompare(const CLbsSelfLocateLogEvent& aLog1,
														 const CLbsSelfLocateLogEvent& aLog2)
	{
	TPositionInfo* tPosInfo1 = static_cast<TPositionInfo*>(aLog1.PositionInfo());
	TPositionInfo* tPosInfo2 = static_cast<TPositionInfo*>(aLog2.PositionInfo());
	TPosition tPos1;
	TPosition tPos2;
	tPosInfo1->GetPosition(tPos1);
	tPosInfo2->GetPosition(tPos2);
	TInt64 t1 = tPos1.Time().Int64();
	TInt64 t2 = tPos2.Time().Int64();
	if(t1 < t2)
		{
		// lhs is Newer
		return 1;
		}
	else if(t1 > t2)
		{
		// lhs is Older
		return -1;
		}
	// the same
	return 0;
	}
