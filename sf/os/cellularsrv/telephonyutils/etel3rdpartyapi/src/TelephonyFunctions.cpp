/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Code for CTelephonyFunctions class, used by CTelephony class.
*
*/


/**
 @file
*/

#include "TelephonyFunctions.h"
#include "TelephonyActPhone.h"
#include "TelephonyActSupplServices.h"
#include "TelephonyActLine.h"
#include "TelephonyActCall.h"
#include "TelephonyActNetwork.h"

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include <commsdat_partner.h>
using namespace CommsDat;

// construct/destruct

CTelephonyFunctions* CTelephonyFunctions::NewLC()
/**
Constructs a CTelephonyFunctions object.

A pointer to this object is left on the CleanupStack.

@leave	Leaves if no memory, or underlying Etel DLL returns error.
@return	Pointer to a newly created CTelephonyFunctions object connected to an RTelServer.
*/
	{
	CTelephonyFunctions* self=new (ELeave) CTelephonyFunctions();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTelephonyFunctions* CTelephonyFunctions::NewL()
/**
Constructs a CTelephonyFunctions object.

@leave	Leaves if no memory, or underlying Etel DLL returns error.
@return	pointer to a newly created CTelephonyFunctions object connected to an RTelServer.
*/
	{
	CTelephonyFunctions* self=NewLC();
	CleanupStack::Pop();
	return self;
	}

CTelephonyFunctions::~CTelephonyFunctions()
/**
Destructor
*/
	{

	CleanUp();

	if (iTelServerIsOpen)
		{
		iTelServer.Close();
		}

	iReqPendingTable.Close();
	iReqStatusTable.Close();
	iCallPool.Close();
	iCallPoolStatus.Close();
	iPhonePowerProperty.Close();
	}

/**
Cleans up any outstanding "Open" objects.
*/
void CTelephonyFunctions::CleanUp()
	{
	for ( TInt i=0;i<iReqPendingTable.Count(); i++)
		{
		if(iReqPendingTable[i]!=EFalse)
			{
			User::RequestComplete(iReqStatusTable[i], KErrCancel);
			}
		}

	iState = ECleaningUp;

	if(iGetPhoneId!=NULL)
		{
		delete iGetPhoneId;
		iGetPhoneId = NULL;
		}
	if(iGetSubscriberId!=NULL)
		{
		delete iGetSubscriberId;
		iGetSubscriberId = NULL;
		}
	if(iGetIndicator!=NULL)
		{
		delete iGetIndicator;
		iGetIndicator = NULL;
		}
	if(iGetBatteryInfo!=NULL)
		{
		delete iGetBatteryInfo;
		iGetBatteryInfo = NULL;
		}
	if(iGetSignalStrength!=NULL)
		{
		delete iGetSignalStrength;
		iGetSignalStrength = NULL;
		}
	if(iGetIccLockInfo!=NULL)
		{
		delete iGetIccLockInfo;
		iGetIccLockInfo = NULL;
		}
	if(iSendDTMFTones!=NULL)
		{
		delete iSendDTMFTones;
		iSendDTMFTones = NULL;
		}
	if(iDialNewCall!=NULL)
		{
		delete iDialNewCall;
		iDialNewCall = NULL;
		}
	if(iHold!=NULL)
		{
		delete iHold;
		iHold = NULL;
		}
	if(iResume!=NULL)
		{
		delete iResume;
		iResume = NULL;
		}
	if(iSwap!=NULL)
		{
		delete iSwap;
		iSwap = NULL;
		}
	if(iHangup!=NULL)
		{
		delete iHangup;
		iHangup = NULL;
		}
	if(iAnswerIncomingCall!=NULL)
		{
		delete iAnswerIncomingCall;
		iAnswerIncomingCall = NULL;
		}
	if(iGetNetworkRegistrationStatus!=NULL)
		{
		delete iGetNetworkRegistrationStatus;
		iGetNetworkRegistrationStatus = NULL;
		}
	if(iGetCurrentNetworkInfo!=NULL)
		{
		delete iGetCurrentNetworkInfo;
		iGetCurrentNetworkInfo = NULL;
		}
	if(iGetCurrentNetworkName!=NULL)
		{
		delete iGetCurrentNetworkName;
		iGetCurrentNetworkName = NULL;
		}
	if(iGetOperatorName!=NULL)
		{
		delete iGetOperatorName;
		iGetOperatorName = NULL;
		}
	if(iGetCFSupplementaryServiceStatus!=NULL)
		{
		delete iGetCFSupplementaryServiceStatus;
		iGetCFSupplementaryServiceStatus = NULL;
		}
	if(iGetCBSupplementaryServiceStatus!=NULL)
		{
		delete iGetCBSupplementaryServiceStatus;
		iGetCBSupplementaryServiceStatus = NULL;
		}
	if(iGetCWSupplementaryServiceStatus!=NULL)
		{
		delete iGetCWSupplementaryServiceStatus;
		iGetCWSupplementaryServiceStatus = NULL;
		}
	if(iGetIdentityServiceStatus!=NULL)
		{
		delete iGetIdentityServiceStatus;
		iGetIdentityServiceStatus = NULL;
		}
	if(iFlightModeChange!=NULL)
		{
		delete iFlightModeChange;
		iFlightModeChange = NULL;
		}
	if(iNotifyIndicator!=NULL)
		{
		delete iNotifyIndicator;
		iNotifyIndicator = NULL;
		}
	if(iNotifyBatteryInfo!=NULL)
		{
		delete iNotifyBatteryInfo;
		iNotifyBatteryInfo = NULL;
		}
	if(iNotifySignalStrength!=NULL)
		{
		delete iNotifySignalStrength;
		iNotifySignalStrength = NULL;
		}
	if(iNotifyIccLockInfo!=NULL)
		{
		delete iNotifyIccLockInfo;
		iNotifyIccLockInfo = NULL;
		}
	if(iNotifyNetworkRegStatus!=NULL)
		{
		delete iNotifyNetworkRegStatus;
		iNotifyNetworkRegStatus = NULL;
		}
	if(iNotifyCurrentNetworkInfo!=NULL)
		{
		delete iNotifyCurrentNetworkInfo;
		iNotifyCurrentNetworkInfo = NULL;
		}
	if(iNotifyOwnedCall1Status!=NULL)
		{
		delete iNotifyOwnedCall1Status;
		iNotifyOwnedCall1Status = NULL;
		}
	if(iNotifyOwnedCall2Status!=NULL)
		{
		delete iNotifyOwnedCall2Status;
		iNotifyOwnedCall2Status = NULL;
		}
	if(iNotifyOwnedCall1RemoteInfo!=NULL)
		{
		delete iNotifyOwnedCall1RemoteInfo;
		iNotifyOwnedCall1RemoteInfo = NULL;
		}
	if(iNotifyOwnedCall2RemoteInfo!=NULL)
		{
		delete iNotifyOwnedCall2RemoteInfo;
		iNotifyOwnedCall2RemoteInfo = NULL;
		}

	// Delete the internal active objects last
	if(iInternalNotifyVoiceLineStatus!=NULL)
		{
		delete iInternalNotifyVoiceLineStatus;
		iInternalNotifyVoiceLineStatus = NULL;
		}
	if (iInternalNotifyIncomingCall!=NULL)
		{
		delete iInternalNotifyIncomingCall;
		iInternalNotifyIncomingCall = NULL;
		}

	if(iCallIsOpen)
		{
		iCallIsOpen = EFalse;
		iCall.Close();
		}

	for(TInt k = 0; k < iCallPool.Count(); k++)
		{
		iCallPool[k].Close();
		}
		
	for(TInt k = 0; k < iCallPoolStatus.Count(); k++)
		{
		iCallPoolStatus[k] = EUnset;
		}

	if(iLineIsDataOpen)
		{
		iLineIsDataOpen = EFalse;
		iLineData.Close();
		}

	if(iLineIsVoiceOpen)
		{
		iLineIsVoiceOpen = EFalse;
		iLineVoice.Close();
		}

	if(iLineIsFaxOpen)
		{
		iLineIsFaxOpen = EFalse;
		iLineFax.Close();
		}

	if(iPhoneIsOpen)
		{
		iPhoneIsOpen = EFalse;
		iPhone.Close();
		}
	iState = EIdle;	// all cleaned up.
	}


CTelephonyFunctions::CTelephonyFunctions()
/**
First-phase constructor

private function which cannot Leave().
*/
: iState(EIdle),
  iReqStatusTable(EMaxNumberOperations),
  iReqPendingTable(EMaxNumberOperations),  
  iCallPool(CTelephony::EISVMaxNumOfCalls),
  iCallPoolStatus(CTelephony::EISVMaxNumOfCalls)
	{
	}


void CTelephonyFunctions::ConstructL()
/**
Second-phase constructor
*/
	{
#ifdef __WINS__
	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
  	// it needs a different CommDB
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);		// Start Comms server if not already active.
	if( err!=KErrNone && err!=KErrAlreadyExists )
		{
		User::Leave(err);
		}

#endif
	User::LeaveIfError(iTelServer.Connect());
	iTelServerIsOpen = ETrue;
	User::LeaveIfError(iPhonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));

	for( TInt i=0; i<EMaxNumberOperations; i++)
		{
		TRequestStatus dummy(KErrNone);
		iReqPendingTable.AppendL(EFalse);
		iReqStatusTable.AppendL(&dummy);
		}
	for( TInt j=0; j<CTelephony::EISVMaxNumOfCalls; j++)
		{
		RMobileCall aDummyCall;
		iCallPool.AppendL(aDummyCall);
		iCallPoolStatus.AppendL(EUnset);
		}

	__ASSERT_DEBUG(iState==EIdle, User::Invariant());

	InitialisePhoneL();
	}


void CTelephonyFunctions::GetCommDbTSYnameL(TDes& aTsyName)
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(db);

	CMDBField<TDesC>* globalSettingField = new(ELeave) CMDBField<TDesC>(KCDTIdBearerAvailabilityCheckTSY);
	CleanupStack::PushL(globalSettingField);
	globalSettingField->SetRecordId(1);
	globalSettingField->SetMaxLengthL(KMaxTextLength);
	globalSettingField->LoadL(*db);
	aTsyName = *globalSettingField;
	CleanupStack::PopAndDestroy(globalSettingField);

	CleanupStack::PopAndDestroy(db);
	}


void CTelephonyFunctions::InitialisePhoneL()
/**
Finds the first data-Line and Open()'s the Phone and Line.

@leave	Leaves if underlying ETEL.DLL returns error,
or if we can't find a suitable Phone/Line or can't open it.
*/
	{
	TBuf<KCommsDbSvrMaxFieldLength> tsyName;
	GetCommDbTSYnameL(tsyName);

	User::LeaveIfError(iTelServer.LoadPhoneModule(tsyName));

	TInt numPhones;
	User::LeaveIfError(iTelServer.EnumeratePhones(numPhones));

	TInt phoneIndx;
	TInt ret = KErrHardwareNotAvailable;

	RPhone::TLineInfo lineInfo;
	for(phoneIndx=0; phoneIndx<numPhones; phoneIndx++)
		{
		RTelServer::TPhoneInfo tInfo;
		ret = iTelServer.GetPhoneInfo(phoneIndx, tInfo);
		if(ret != KErrNone)
			{
			continue;
			}
		
		ret = iPhone.Open(iTelServer, tInfo.iName);	
		if(ret != KErrNone)
			{
			continue;
			}

		iPhoneIsOpen = ETrue;

		RPhone::TStatus status;
		User::LeaveIfError(iPhone.GetStatus(status));
		if(status.iModemDetected!=RPhone::EDetectedPresent)
			{
			ret = iPhone.Initialise();
			if(ret != KErrNone)
				{
				iPhone.Close();
				iPhoneIsOpen = EFalse;
				continue;
				}
			}

		TInt lines;
		ret = iPhone.EnumerateLines(lines);
		if(ret != KErrNone)
			{
			iPhone.Close();
			iPhoneIsOpen = EFalse;
			continue;
			}

		if(lines<1)
			{
			ret = KErrEtelWrongModemType;
			iPhone.Close();
			iPhoneIsOpen = EFalse;
			continue;
			}

		TInt lineIndx;
		TBool lineVoiceFound=EFalse, lineFaxFound=EFalse, lineDataFound=EFalse;
		for(lineIndx=0; lineIndx<lines; lineIndx++)
			{
			if(iPhone.GetLineInfo(lineIndx, lineInfo) != KErrNone)
				{
				continue;
				}

			if((lineInfo.iLineCapsFlags & RLine::KCapsVoice)&&(!lineVoiceFound))
				{
				User::LeaveIfError(iLineVoice.Open(iPhone, lineInfo.iName));
				iLineIsVoiceOpen = ETrue;
				lineVoiceFound = ETrue;
				iLineVoiceName.Copy(lineInfo.iName);
				continue;
				}
			else if((lineInfo.iLineCapsFlags & RLine::KCapsData)&&(!lineDataFound))
				{
				User::LeaveIfError(iLineData.Open(iPhone, lineInfo.iName));
				iLineIsDataOpen = ETrue;
				lineDataFound = ETrue;
				continue;
				}
			else if((lineInfo.iLineCapsFlags & RLine::KCapsFax)&&(!lineFaxFound))
				{
				User::LeaveIfError(iLineFax.Open(iPhone, lineInfo.iName));
				iLineIsFaxOpen = ETrue;
				lineFaxFound = ETrue;
				continue;
				}

			}

		if(!iLineIsVoiceOpen)
			{
			ret = KErrEtelWrongModemType;
			iPhone.Close();
			iPhoneIsOpen = EFalse;
			continue;
			}

		//get TSY version
		TBool result;
		iTsyVersion = KETelExtMultimodeV3;
		User::LeaveIfError(iTelServer.IsSupportedByModule(tsyName, iTsyVersion, result));
		if(!result)
			{
			iTsyVersion = KETelExtMultimodeV2;
			User::LeaveIfError(iTelServer.IsSupportedByModule(tsyName, iTsyVersion, result));
			if(!result)
				{
				iTsyVersion = KETelExtMultimodeV1;
				User::LeaveIfError(iTelServer.IsSupportedByModule(tsyName, iTsyVersion, result));
				if(!result)
					{
					iTsyVersion = KNoMultimodeTsy;
					}
				}
			}

		//
		// A CTelephonyFunctions object is always listening for a change
		// in the line status and for an incoming call.
		// The iInternalNotifyVoiceLineStatus and iInternalNotifyIncoming
		// call objects persist throughout this object's lifetime.
		// Create the internal active objects and post the first request.
		//
		iInternalNotifyIncomingCall=CNotifyIncomingCallAct::NewL(this);
		iInternalNotifyIncomingCall->NotifyIncomingCall();
		iInternalNotifyVoiceLineStatus=CNotifyLineStatusAct::NewL(this, CTelephonyFunctions::ENotifyVoiceLineStatus, *iInternalNotifyIncomingCall);
		iInternalNotifyVoiceLineStatus->NotifyLineStatus(NULL);

		//
		// We have found a valid phone to use...
		//
		break;
		}

	//
	// ret will be KErrNone if a valid phone was found...
	//
	User::LeaveIfError(ret);
	}

/**
Cancel the outstanding async request.
*/
TInt CTelephonyFunctions::CancelAsync(CTelephony::TCancellationRequest aCancel)
	{
	switch(aCancel)
		{
		case CTelephony::EGetPhoneIdCancel:
			if((iGetPhoneId==NULL)||
				(IsRequestPending(EGetPhoneId)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetPhoneId;
				iGetPhoneId=NULL;
				}
			break;
		case CTelephony::EGetSubscriberIdCancel:
			if((iGetSubscriberId==NULL)||
				(IsRequestPending(EGetSubscriberId)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetSubscriberId;
				iGetSubscriberId=NULL;
				}
			break;
		case CTelephony::EGetIndicatorCancel:
			if((iGetIndicator==NULL)||
				(IsRequestPending(EGetIndicator)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetIndicator;
				iGetIndicator=NULL;
				}
			break;
		case CTelephony::EGetBatteryInfoCancel:
			if((iGetBatteryInfo==NULL)||
				(IsRequestPending(EGetBatteryInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetBatteryInfo;
				iGetBatteryInfo=NULL;
				}
			break;
		case CTelephony::EGetSignalStrengthCancel:
			if((iGetSignalStrength==NULL)||
				(IsRequestPending(EGetSignalStrength)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetSignalStrength;
				iGetSignalStrength=NULL;
				}
			break;
		case CTelephony::EGetLockInfoCancel:
			if((iGetIccLockInfo==NULL)||
				(IsRequestPending(EGetIccLockInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetIccLockInfo;
				iGetIccLockInfo=NULL;
				}
			break;
		case CTelephony::ESendDTMFTonesCancel:
			if((iSendDTMFTones==NULL)||
				(IsRequestPending(ESendDTMFTones)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iSendDTMFTones;
				iSendDTMFTones=NULL;
				}
			break;
		case CTelephony::EDialNewCallCancel:
			if((iDialNewCall==NULL)||
				(IsRequestPending(EDialNewCall)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iDialNewCall;
				iDialNewCall=NULL;
				}
			break;
		case CTelephony::EHoldCancel:
			if((iHold==NULL)||
				(IsRequestPending(EHold)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iHold;
				iHold=NULL;
				}
			break;
		case CTelephony::EResumeCancel:
			if((iResume==NULL)||
				(IsRequestPending(EResume)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iResume;
				iResume=NULL;
				}
			break;
		case CTelephony::ESwapCancel:
			if((iSwap==NULL)||
				(IsRequestPending(ESwap)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iSwap;
				iSwap=NULL;
				}
			break;
		case CTelephony::EHangupCancel:
			if((iHangup==NULL)||
				(IsRequestPending(EHangup)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iHangup;
				iHangup=NULL;
				}
			break;
		case CTelephony::EAnswerIncomingCallCancel:
			if((iAnswerIncomingCall==NULL)||
				(IsRequestPending(EAnswerIncomingCall)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iAnswerIncomingCall;
				iAnswerIncomingCall=NULL;
				}
			break;
		case CTelephony::EGetNetworkRegistrationStatusCancel:
			if((iGetNetworkRegistrationStatus==NULL)||
				(IsRequestPending(EGetNetworkRegistrationStatus)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetNetworkRegistrationStatus;
				iGetNetworkRegistrationStatus=NULL;
				}
			break;
		case CTelephony::EGetCurrentNetworkInfoCancel:
			if((iGetCurrentNetworkInfo==NULL)||
				(IsRequestPending(EGetCurrentNetworkInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetCurrentNetworkInfo;
				iGetCurrentNetworkInfo=NULL;
				}
			break;
		case CTelephony::EGetCurrentNetworkNameCancel:
			if((iGetCurrentNetworkName==NULL)||
				(IsRequestPending(EGetCurrentNetworkName)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetCurrentNetworkName;
				iGetCurrentNetworkName=NULL;
				}
			break;
		case CTelephony::EGetOperatorNameCancel:
			if((iGetOperatorName==NULL)||
				(IsRequestPending(EGetOperatorName)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetOperatorName;
				iGetOperatorName=NULL;
				}
			break;
		case CTelephony::EGetCallForwardingStatusCancel:
			if((iGetCFSupplementaryServiceStatus==NULL)||
				(IsRequestPending(EGetCFSupplServicesStatus)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetCFSupplementaryServiceStatus;
				iGetCFSupplementaryServiceStatus=NULL;
				}
			break;
		case CTelephony::EGetCallBarringStatusCancel:
			if((iGetCBSupplementaryServiceStatus==NULL)||
				(IsRequestPending(EGetCBSupplServicesStatus)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetCBSupplementaryServiceStatus;
				iGetCBSupplementaryServiceStatus=NULL;
				}
			break;
		case CTelephony::EGetCallWaitingStatusCancel:
			if((iGetCWSupplementaryServiceStatus==NULL)||
				(IsRequestPending(EGetCWSupplServicesStatus)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetCWSupplementaryServiceStatus;
				iGetCWSupplementaryServiceStatus=NULL;
				}
			break;
		case CTelephony::EGetIdentityServiceStatusCancel:
			if((iGetIdentityServiceStatus==NULL)||
				(IsRequestPending(EGetIdentityServiceStatus)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iGetIdentityServiceStatus;
				iGetIdentityServiceStatus=NULL;
				}
			break;
		case CTelephony::EFlightModeChangeCancel:
			if((iFlightModeChange==NULL)||
				(IsRequestPending(EFlightModeChange)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iFlightModeChange;
				iFlightModeChange=NULL;
				}
			break;
		case CTelephony::EIndicatorChangeCancel:
			if((iNotifyIndicator==NULL)||
				(IsRequestPending(ENotifyIndicator)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyIndicator;
				iNotifyIndicator=NULL;
				}
			break;
		case CTelephony::EBatteryInfoChangeCancel:
			if((iNotifyBatteryInfo==NULL)||
				(IsRequestPending(ENotifyBatteryInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyBatteryInfo;
				iNotifyBatteryInfo=NULL;
				}
			break;
		case CTelephony::ESignalStrengthChangeCancel:
			if((iNotifySignalStrength==NULL)||
				(IsRequestPending(ENotifySignalStrength)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifySignalStrength;
				iNotifySignalStrength=NULL;
				}
			break;
		case CTelephony::EPin1LockInfoChangeCancel:
			if((iNotifyIccLockInfo==NULL)||
				(IsRequestPending(ENotifyPin1LockInfo)==EFalse))
				{
				return KErrTelephonyOutOfSequence;
				}
			else if (IsRequestPending(ENotifyPin2LockInfo))
				{
				CompleteRequest(ENotifyPin1LockInfo,KErrCancel);
				}
			else
				{
				delete iNotifyIccLockInfo;
				iNotifyIccLockInfo=NULL;
				}

			break;
		case CTelephony::EPin2LockInfoChangeCancel:
			if((iNotifyIccLockInfo==NULL)||
				(IsRequestPending(ENotifyPin2LockInfo)==EFalse))
				{
				return KErrTelephonyOutOfSequence;
				}
			else if (IsRequestPending(ENotifyPin1LockInfo))
				{
				CompleteRequest(ENotifyPin2LockInfo,KErrCancel);
				}
			else
				{
				delete iNotifyIccLockInfo;
				iNotifyIccLockInfo=NULL;
				}
			break;
		case CTelephony::ENetworkRegistrationStatusChangeCancel:
			if((iNotifyNetworkRegStatus==NULL)||
				(IsRequestPending(ENotifyNetworkRegistrationStatus)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyNetworkRegStatus;
				iNotifyNetworkRegStatus=NULL;
				}
			break;
		case CTelephony::ECurrentNetworkInfoChangeCancel:
			if((iNotifyCurrentNetworkInfo==NULL)||
				(IsRequestPending(ENotifyCurrentNetworkInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyCurrentNetworkInfo;
				iNotifyCurrentNetworkInfo=NULL;
				}
			break;
		case CTelephony::EOwnedCall1StatusChangeCancel:
			if((iNotifyOwnedCall1Status==NULL)||
				(IsRequestPending(ENotifyOwnedCall1Status)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyOwnedCall1Status;
				iNotifyOwnedCall1Status=NULL;
				}
			break;
		case CTelephony::EOwnedCall2StatusChangeCancel:
			if((iNotifyOwnedCall2Status==NULL)||
				(IsRequestPending(ENotifyOwnedCall2Status)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyOwnedCall2Status;
				iNotifyOwnedCall2Status=NULL;
				}
			break;
		case CTelephony::EVoiceLineStatusChangeCancel:
			if (IsRequestPending(ENotifyVoiceLineStatus)==EFalse)
				{
				return KErrTelephonyOutOfSequence;
				}
			else
				{
				// Notifies the iInternalNotifyVoiceLineStatus active
				// object that the client is no longer interested
				// in being notified of line status changes.
				// The AO will then complete the request back to the
				// client with a status of KErrCancel.
				// The AO will still continue to monitor for line
				// status changes but now the result will not be
				// returned to the client.
				if (iInternalNotifyVoiceLineStatus)
					iInternalNotifyVoiceLineStatus->CancelFromClient();
				}
			break;
		case CTelephony::EFaxLineStatusChangeCancel:
			return KErrNotSupported;
		case CTelephony::EDataLineStatusChangeCancel:
			return KErrNotSupported;
		case CTelephony::EOwnedCall1RemotePartyInfoChangeCancel:
			if((iNotifyOwnedCall1RemoteInfo==NULL)||
				(IsRequestPending(ENotifyOwnedCall1RemoteInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyOwnedCall1RemoteInfo;
				iNotifyOwnedCall1RemoteInfo=NULL;
				}
			break;
		case CTelephony::EOwnedCall2RemotePartyInfoChangeCancel:
			if((iNotifyOwnedCall2RemoteInfo==NULL)||
				(IsRequestPending(ENotifyOwnedCall2RemoteInfo)==EFalse))
				return KErrTelephonyOutOfSequence;
			else
				{
				delete iNotifyOwnedCall2RemoteInfo;
				iNotifyOwnedCall2RemoteInfo=NULL;
				}
			break;
		default:
			return KErrNotSupported;
		}

	return KErrNone;
	}

/**
Send a notification request.

@param aRequestStatus The request status to be returned when an event
                      occurs for client notification.
@param aEvent The notification event the caller is requesting.
@param aDes Packaged data returned by the TSY when an event for notification occurs.
*/
TInt CTelephonyFunctions::NotifyChangeL(TRequestStatus& aRequestStatus,
             const CTelephony::TNotificationEvent& aEvent, TDes8& aDes)
	{
	TInt err;
	switch(aEvent)
		{
		case CTelephony::EFlightModeChange:
			err = NotifyFlightModeL(aRequestStatus, aDes);
			break;

		case CTelephony::EOwnedCall1RemotePartyInfoChange:
			err = NotifyOwnedCall1RemotePartyInfoL(aRequestStatus, aDes);
			break;

		case CTelephony::EOwnedCall2RemotePartyInfoChange:
			err = NotifyOwnedCall2RemotePartyInfoL(aRequestStatus, aDes);
			break;

		case CTelephony::EVoiceLineStatusChange:
			err = NotifyVoiceLineStatusL(aRequestStatus, aDes);
			break;

		case CTelephony::EFaxLineStatusChange:
			err = KErrNotSupported;
			break;

		case CTelephony::EDataLineStatusChange:
			err = KErrNotSupported;
			break;

		case CTelephony::EOwnedCall1StatusChange:
			err = NotifyOwnedCall1StatusL(aRequestStatus, aDes);
			break;

		case CTelephony::EOwnedCall2StatusChange:
			err = NotifyOwnedCall2StatusL(aRequestStatus, aDes);
			break;

		case CTelephony::EIndicatorChange:
			err = NotifyIndicatorL(aRequestStatus, aDes);
			break;

		case CTelephony::EPin1LockInfoChange:
		case CTelephony::EPin2LockInfoChange:
			err = NotifyPinLockInfoL(aRequestStatus, aEvent, aDes);
			break;

		case CTelephony::EBatteryInfoChange:
			err = NotifyBatteryInfoL(aRequestStatus, aDes);
			break;

		case CTelephony::ESignalStrengthChange:
			err = NotifySignalStrengthL(aRequestStatus, aDes);
			break;

		case CTelephony::ENetworkRegistrationStatusChange:
			err = NotifyNetworkRegistrationStatusL(aRequestStatus, aDes);
			break;

		case CTelephony::ECurrentNetworkInfoChange:
			err = NotifyCurrentNetworkInfoL(aRequestStatus, aDes);
			break;
		default:
			err = KErrArgument;
		}
	return err;
	}

TInt CTelephonyFunctions::NotifyFlightModeL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a Flight mode notification request
*/
	{
	if (IsRequestPending(EFlightModeChange)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EFlightModeChange, ETrue);
		}

	if(iFlightModeChange==NULL)
		{
		iFlightModeChange=CFlightModeChangeAct::NewL(this);
		}
	iReqStatusTable[EFlightModeChange] = &aRequestStatus;

	iFlightModeChange->FlightModeChangeL(aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyOwnedCall1RemotePartyInfoL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a remote party info call 1 notification request
*/
	{
	if (IsRequestPending(ENotifyOwnedCall1RemoteInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall1RemoteInfo, ETrue);
		}

	if(iCallPoolStatus[CTelephony::EISVCall1]==EUnset)
		{
		return KErrAccessDenied;
		}

	if(iNotifyOwnedCall1RemoteInfo==NULL)
		{
		iNotifyOwnedCall1RemoteInfo=CNotifyRemotePartyInfoAct::NewL(this, CTelephonyFunctions::ENotifyOwnedCall1RemoteInfo);
		}

	iReqStatusTable[ENotifyOwnedCall1RemoteInfo] = &aRequestStatus;

	iNotifyOwnedCall1RemoteInfo->NotifyRemotePartyInfo(aDes, CTelephony::EISVCall1);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyOwnedCall2RemotePartyInfoL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post aremote party info call 2 notification request
*/
	{
	if (IsRequestPending(ENotifyOwnedCall2RemoteInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall2RemoteInfo, ETrue);
		}

	if(iCallPoolStatus[CTelephony::EISVCall2]==EUnset)
		{
		return KErrAccessDenied;
		}

	if(iNotifyOwnedCall2RemoteInfo==NULL)
		{
		iNotifyOwnedCall2RemoteInfo=CNotifyRemotePartyInfoAct::NewL(this, CTelephonyFunctions::ENotifyOwnedCall2RemoteInfo);
		}

	iReqStatusTable[ENotifyOwnedCall2RemoteInfo] = &aRequestStatus;

	iNotifyOwnedCall2RemoteInfo->NotifyRemotePartyInfo(aDes, CTelephony::EISVCall2);
	return KErrNone;
	}

/**
Post a voice line status notification request.

@param aRequestStatus The request status.
@param aDes Package containing TCallStatus which will store the resulting line status.
*/
TInt CTelephonyFunctions::NotifyVoiceLineStatusL(TRequestStatus& aRequestStatus, TDes8& aDes)
	{
	if (IsRequestPending(ENotifyVoiceLineStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyVoiceLineStatus, ETrue);
		}
	iReqStatusTable[ENotifyVoiceLineStatus] = &aRequestStatus;

	iInternalNotifyVoiceLineStatus->NotifyLineStatus(&aDes);

	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyOwnedCall1StatusL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a call 1 status change notification request
*/
	{
	if (IsRequestPending(ENotifyOwnedCall1Status)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall1Status, ETrue);
		}
	if(iCallPoolStatus[CTelephony::EISVCall1]==EUnset)
		{
		return KErrAccessDenied;
		}

	if(iNotifyOwnedCall1Status==NULL)
		{
		iNotifyOwnedCall1Status=CNotifyCallStatusAct::NewL(this, CTelephonyFunctions::ENotifyOwnedCall1Status);
		}

	iReqStatusTable[ENotifyOwnedCall1Status] = &aRequestStatus;

	iNotifyOwnedCall1Status->NotifyCallStatus(aDes, CTelephony::EISVCall1);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyOwnedCall2StatusL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a call 2 status change notification request
*/
	{
	if (IsRequestPending(ENotifyOwnedCall2Status)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall2Status, ETrue);
		}
	if(iCallPoolStatus[CTelephony::EISVCall2]==EUnset)
		{
		return KErrAccessDenied;
		}

	if(iNotifyOwnedCall2Status==NULL)
		{
		iNotifyOwnedCall2Status=CNotifyCallStatusAct::NewL(this, CTelephonyFunctions::ENotifyOwnedCall2Status);
		}

	iReqStatusTable[ENotifyOwnedCall2Status] = &aRequestStatus;

	iNotifyOwnedCall2Status->NotifyCallStatus(aDes, CTelephony::EISVCall2);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyIndicatorL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a indicator notification request
*/
	{
	if (IsRequestPending(ENotifyIndicator)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyIndicator, ETrue);
		}

	if(iNotifyIndicator==NULL)
		{
		iNotifyIndicator=CNotifyIndicatorAct::NewL(this);
		}

	iReqStatusTable[ENotifyIndicator] = &aRequestStatus;

	iNotifyIndicator->NotifyIndicator(aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyPinLockInfoL(TRequestStatus& aRequestStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes)
/**
Post a pin lock change  notification request
*/
	{
	CTelephony::TIccLock lock;
	if(aEvent == CTelephony::EPin1LockInfoChange)
		{
		lock = CTelephony::ELockPin1;
		if (IsRequestPending(ENotifyPin1LockInfo))
			{
			return KErrInUse;
			}
		}
	else if(aEvent == CTelephony::EPin2LockInfoChange)
		{
		lock = CTelephony::ELockPin2;
		if (IsRequestPending(ENotifyPin2LockInfo))
			{
			return KErrInUse;
			}
		}
	else
		{
		return KErrNotSupported;
		}

	if(iNotifyIccLockInfo==NULL)
		{
		iNotifyIccLockInfo=CNotifyIccLockInfoAct::NewL(this);
		}

	if (lock == CTelephony::ELockPin1)
		{
		SetRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo, ETrue);
		iReqStatusTable[ENotifyPin1LockInfo] = &aRequestStatus;
		}
	else if (lock == CTelephony::ELockPin2)
		{
		SetRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo, ETrue);
		iReqStatusTable[ENotifyPin2LockInfo] = &aRequestStatus;
		}

	iNotifyIccLockInfo->NotifyIccLockInfo(lock, aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyBatteryInfoL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a battery info change notification request
*/
	{
	if (IsRequestPending(ENotifyBatteryInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyBatteryInfo, ETrue);
		}

	if(iNotifyBatteryInfo==NULL)
		{
		iNotifyBatteryInfo=CNotifyBatteryInfoAct::NewL(this);
		}
	iReqStatusTable[ENotifyBatteryInfo] = &aRequestStatus;

	iNotifyBatteryInfo->NotifyBatteryInfo(aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifySignalStrengthL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a signal strength change notification request
*/
	{
	if (IsRequestPending(ENotifySignalStrength)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifySignalStrength, ETrue);
		}

	if(iNotifySignalStrength==NULL)
		{
		iNotifySignalStrength=CNotifySignalStrengthAct::NewL(this);
		}

	iReqStatusTable[ENotifySignalStrength] = &aRequestStatus;

	iNotifySignalStrength->NotifySignalStrength(aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyNetworkRegistrationStatusL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a network registration change notification request
*/
	{
	if (IsRequestPending(ENotifyNetworkRegistrationStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyNetworkRegistrationStatus, ETrue);
		}

	if(iNotifyNetworkRegStatus==NULL)
		{
		iNotifyNetworkRegStatus=CNotifyNetworkRegistrationStatusAct::NewL(this);
		}
	iReqStatusTable[ENotifyNetworkRegistrationStatus] = &aRequestStatus;

	iNotifyNetworkRegStatus->NotifyNetworkRegistrationStatus(aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::NotifyCurrentNetworkInfoL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Post a current network info change notification request
*/
	{
	if (IsRequestPending(ENotifyCurrentNetworkInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ENotifyCurrentNetworkInfo, ETrue);
		}

	if(iNotifyCurrentNetworkInfo==NULL)
		{
		iNotifyCurrentNetworkInfo=CNotifyCurrentNetworkInfoAct::NewL(this);
		}
	iReqStatusTable[CTelephonyFunctions::ENotifyCurrentNetworkInfo] = &aRequestStatus;

	iNotifyCurrentNetworkInfo->NotifyCurrentNetworkInfo(aDes);
	return KErrNone;
	}

TInt CTelephonyFunctions::GetISVCall(CTelephony::TCallId& aCallId)
/**
Return an available call ID.
Also, continue looping though other call objects for opportunity to close them if they have become idle.
*/
	{
	RMobileCall::TMobileCallStatus callStatus;
	//get first free call object
	TInt err;
	TInt ret = KErrNotFound;
	TInt foundACallId = EFalse;
	for(int k=0;k<CTelephony::EISVMaxNumOfCalls;k++)
		{
		if(iCallPoolStatus[k]!=EUnset)
			{
			err = Call((CTelephony::TCallId)k)->GetMobileCallStatus(callStatus);
			if(err==KErrNone && callStatus==RMobileCall::EStatusIdle)
				{
				if(!foundACallId)
					{
					aCallId=(CTelephony::TCallId)k;
					foundACallId = ETrue;
					ret = KErrNone;
					}
				else
					{
					CloseAndReset((CTelephony::TCallId)k);
					}
				}
			}
		else
			{
			if(!foundACallId)
				{
				aCallId=(CTelephony::TCallId)k;
				foundACallId = ETrue;
				ret = KErrNone;
				}
			}
		}
	return ret;
	}

TBool CTelephonyFunctions::IsFlightModeOn()
/**
Get Flight mode from SA
*/
	{
	TInt newPhonePowerState;
	if (iPhonePowerProperty.Get(newPhonePowerState) != KErrNone)
		newPhonePowerState = ESAPhoneOff;
	if(newPhonePowerState==ESAPhoneOn)
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

void CTelephonyFunctions::CloseAndReset(const CTelephony::TCallId aCallId)
/**
Closes the open call object and resets its pool status back to Unset
*/
	{
	//must close all outstanding notifiers invoked from this call session prior to closing the session.
	if( aCallId ==  CTelephony::EISVCall1)
		{
		if(IsRequestPending(ENotifyOwnedCall1RemoteInfo))
			{
			CancelAsync(CTelephony::EOwnedCall1RemotePartyInfoChangeCancel);
			}

		if(IsRequestPending(ENotifyOwnedCall1Status))
			{
			CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);
			}
		}
	else if(aCallId ==  CTelephony::EISVCall2)
		{
		if(IsRequestPending(ENotifyOwnedCall2RemoteInfo))
			{
			CancelAsync(CTelephony::EOwnedCall2RemotePartyInfoChangeCancel);
			}

		if(IsRequestPending(ENotifyOwnedCall2Status))
			{
			CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);
			}
		}

	//close the call session
	Call(aCallId)->Close();
	iCallPoolStatus[aCallId] = EUnset;
	}

void CTelephonyFunctions::ResetPendingNotifyRequest(CTelephony::TNotificationEvent aEvent)
/**
Reset the pending notify request flag
*/
	{
	switch(aEvent)
		{
		case CTelephony::EFlightModeChange:
			SetRequestPending(CTelephonyFunctions::EFlightModeChange, EFalse);
			break;
		case CTelephony::EOwnedCall1RemotePartyInfoChange:
			SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall1RemoteInfo, EFalse);
			break;
		case CTelephony::EOwnedCall2RemotePartyInfoChange:
			SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall2RemoteInfo, EFalse);
			break;
		case CTelephony::EVoiceLineStatusChange:
			SetRequestPending(CTelephonyFunctions::ENotifyVoiceLineStatus, EFalse);
			break;
		case CTelephony::EOwnedCall1StatusChange:
			SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall1Status, EFalse);
			break;
		case CTelephony::EOwnedCall2StatusChange:
			SetRequestPending(CTelephonyFunctions::ENotifyOwnedCall2Status, EFalse);
			break;
		case CTelephony::EIndicatorChange:
			SetRequestPending(CTelephonyFunctions::ENotifyIndicator, EFalse);
			break;
		case CTelephony::EPin1LockInfoChange:
			SetRequestPending(CTelephonyFunctions::ENotifyPin1LockInfo, EFalse);
			break;
		case CTelephony::EPin2LockInfoChange:
			SetRequestPending(CTelephonyFunctions::ENotifyPin2LockInfo, EFalse);
			break;
		case CTelephony::EBatteryInfoChange:
			SetRequestPending(CTelephonyFunctions::ENotifyBatteryInfo, EFalse);
			break;
		case CTelephony::ESignalStrengthChange:
			SetRequestPending(CTelephonyFunctions::ENotifySignalStrength, EFalse);
			break;
		case CTelephony::ENetworkRegistrationStatusChange:
			SetRequestPending(CTelephonyFunctions::ENotifyNetworkRegistrationStatus, EFalse);
			break;
		case CTelephony::ECurrentNetworkInfoChange:
			SetRequestPending(CTelephonyFunctions::ENotifyCurrentNetworkInfo, EFalse);
			break;
		default:
			break;
		}
	}

/**
Completes the request back to the client that requested it.

@param aOperation The operation that has been completed.
@param aCompleteCode The error code to be returned to the client.
*/
void CTelephonyFunctions::CompleteRequest(TOperations aOperation, TInt aCompleteCode)
	{
	if(IsRequestPending(aOperation))
		{
		User::RequestComplete(*GetRequestStatus(aOperation), aCompleteCode);
		SetRequestPending(aOperation, EFalse);
		}

	}

CAsyncRequestBaseAct::CAsyncRequestBaseAct()
	: CActive(CActive::EPriorityStandard)
	{} //NOP

CAsyncRequestBaseAct::~CAsyncRequestBaseAct()
	{} //NOP

void CAsyncRequestBaseAct::RunL()
/**
Service Completed request.

@leave	Leaves if System error.
*/
	{
	Complete();
	}
