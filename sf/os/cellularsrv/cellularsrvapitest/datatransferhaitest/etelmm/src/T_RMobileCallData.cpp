/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_RMobileCallData.h"
#include "T_RMobileLineData.h"
//System includes
#include <e32property.h>


_LIT( KCallName,     	"Call");
_LIT( KCapabilities, 	"Capabilities" );
_LIT( KPhoneNumber,  	"PhoneNumber" );
_LIT( KDefaultSection, 	"Default" );
_LIT( KMaxPhoneCalls, 	"MaxPhoneCalls");

// Call capabilities RMobileCall::GetCaps())
_LIT( KCapsVoiceCall,   "VoiceCall" );
_LIT( KCapsHold,        "Hold" );
_LIT( KCapsResume,      "Resume" );
_LIT( KCapsSwap,        "Swap" );
_LIT( KCapsDeflect,     "Deflect" );
_LIT( KCapsTransfer,    "Transfer" );
_LIT( KCapsJoin,        "Join" );       // Conference
_LIT( KCapsOneToOne,    "OneToOne" );   // Conference

_LIT(KMobileLineKey, "RMobileLine");
/*@}*/
// ini file Keys


//Voice Call 
_LIT(KCmdAnswerIncomingCall, "AnswerIncomingCall");
_LIT(KCmdAnswerIncomingCallPost, "AnswerIncomingCallPost");
_LIT(KCmdGetCaps, "GetCaps");
_LIT(KCmdGetCallDuration, "GetCallDuration");
_LIT(KCmdHangUp, "HangUp");
_LIT(KCmdClose, "Close");
_LIT(KCmdOpenNewCall, "OpenNewCall");
_LIT(KCmdOpenExistingCall, "OpenExistingCall");
_LIT(KCmdDial, "Dial");
_LIT(KCmdDialCancel, "DialCancel");
_LIT(KCmdGetStatus, "GetStatus");
_LIT(KCmdHold, "Hold");
_LIT(KCmdResume, "Resume");
_LIT(KCmdSwap, "Swap");
_LIT(KCmdTransfer, "Transfer");
_LIT(KCmdCheckCaps, "CheckCaps");
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RMobileCallData* CT_RMobileCallData::NewL()
	{
	CT_RMobileCallData* ret=new (ELeave) CT_RMobileCallData();
	CleanupStack::PushL (ret);
	ret->ConstructL ();
	CleanupStack::Pop (ret);
	return ret;
	}

/**
 * Protected constructor. First phase construction
 */
CT_RMobileCallData::CT_RMobileCallData()
:	iActiveCallback(NULL)
	{
	}

/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
void CT_RMobileCallData::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL (*this);
	}

/**
 * Third phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
void CT_RMobileCallData::InitialiseL()
	{
	CDataWrapperBase::InitialiseL();
	if ( !GetIntFromConfig(KDefaultSection, KMaxPhoneCalls, iMaxPhoneCalls ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaxPhoneCalls);
		SetBlockResult(EFail);
		}

	else
		{
		RMobileCall* mobileCall=  NULL;
		for (TInt i = 0; i < iMaxPhoneCalls; ++i)
			{
			// RMobileCalls
			mobileCall = new (ELeave) RMobileCall(); // We need a bunch of RMobileCall handles
			CleanupStack::PushL (mobileCall);
			iMobileCalls.AppendL (mobileCall);
			CleanupStack::Pop (mobileCall);
			}
		}
	}

/**
 * Public destructor
 */
CT_RMobileCallData::~CT_RMobileCallData()
	{
	delete iActiveCallback;
	iActiveCallback=NULL;

	// Empty arrays and also delete objects whose pointers are contained within
	iMobileCalls.ResetAndDestroy ();
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RMobileCallData::GetObject()
	{
	return &iMobileCall;
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_RMobileCallData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{

	TBool ret = ETrue;

	if ( aCommand==KCmdGetCaps)
		{
		DoCmdGetCaps (aSection);
		}
	else if	( aCommand==KCmdCheckCaps)
		{
		UtilityCheckCapability(aSection);
		}
	else if ( aCommand==KCmdGetCallDuration)
		{
		DoCmdGetCallDuration (aSection);
		}
	else if ( aCommand==KCmdHangUp)
		{
		DoCmdHangUp (aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdClose)
		{
		DoCmdClose (aSection);
		}
	else if ( aCommand==KCmdOpenNewCall)
		{
		DoCmdOpenNewCall (aSection);
		}
	else if ( aCommand==KCmdOpenExistingCall)
		{
		DoCmdOpenExistingCall (aSection);
		}
	else if ( aCommand==KCmdDial)
		{
		DoCmdDial (aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdDialCancel)
		{
		DoCmdDialCancel (aSection);
		}
	else if ( aCommand==KCmdAnswerIncomingCall)
		{
		DoCmdAnswerIncomingCall (aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdAnswerIncomingCallPost)
		{
		DoCmdAnswerIncomingCallPost ( aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdGetStatus)
		{
		DoCmdGetStatus ();
		}
	else if ( aCommand == KCmdHold)
		{
		DoCmdHold (aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdSwap)
		{
		DoCmdSwap (aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdResume)
		{
		DoCmdResume (aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdTransfer)
		{
		DoCmdTransfer (aSection, aAsyncErrorIndex);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret = EFalse;
		}

	return ret;
	}

void CT_RMobileCallData::DoCmdGetCaps(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdGetCaps"));
	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Getting call capabilities"));
			TRAP( error, iMobileCall->GetCaps(iCallCaps) );
			if ( error != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get mobile call capabilities with error %d"), error);
				SetError(error);
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdGetCaps"));
	}

void CT_RMobileCallData::DoCmdGetCallDuration(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdGetCallDuration"));
	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			TTimeIntervalSeconds duration = 0;
			error = iMobileCall->GetCallDuration(duration);
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to get call duration with error %d"), error);
				SetError(error);
				}
			else
				{
				INFO_PRINTF3(_L("Call %d duration was %d seconds"), callNameParameter, duration.Int());
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdGetCallDuration"));
	}

void CT_RMobileCallData::DoCmdHangUp(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdHangUp"));
	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			iMobileCall->HangUp(iActiveCallback->iStatus);
			iActiveCallback->Activate (aAsyncErrorIndex);
			IncOutstanding ();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdHangUp"));
	}

void CT_RMobileCallData::DoCmdClose(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdCloseCall"));
	TInt callNameParameter;
	if ( !GetIntFromConfig( aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Close handle to RMobileCall"));
			iMobileCall->Close ();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::CloseCallL"));
	}

void CT_RMobileCallData::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::RunL"));
	DecOutstanding (); // One of the async calls has completed 
	TInt err = aActive->iStatus.Int();
	if ( err != KErrNone)
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError ( aIndex, err);
		}
	else
		{
		INFO_PRINTF1(_L("RunL  completed successfully"));
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::RunL"));
	}

void CT_RMobileCallData::DoCmdOpenNewCall(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdOpenNewCall"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		TPtrC mobileLineName;
		if ( !GetStringFromConfig (aSection, KMobileLineKey (), mobileLineName))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMobileLineKey );
			SetBlockResult(EFail);
			}
		else
			{
			CT_RMobileLineData* mobileLineWrapper = static_cast<CT_RMobileLineData*>(GetDataWrapperL(mobileLineName));
			TName* callName;
			TRAPD( error, callName = mobileLineWrapper->GetCallNameL (callNameParameter) );
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("Left while getting call name with error %d"), error);
				SetError(error);
				}
			else
				{
				INFO_PRINTF1(_L("Open handle to RMobileCall which can be used to receive incoming or dial outgoing call."));
				RMobileLine* mobileLineObject = static_cast<RMobileLine*>(GetDataObjectL(mobileLineName));
				INFO_PRINTF1(_L("Getting mobile call"));
				TRAP(error, iMobileCall = GetMobileCallL(callNameParameter));
				if(error != KErrNone)
					{
					ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
					SetBlockResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Opening new call"));
					error = iMobileCall->OpenNewCall(*mobileLineObject, *callName);
					if ( error != KErrNone)
						{
						ERR_PRINTF3(_L("OpenNewCall named: %S, failed with error [%d]"), &callName, error);
						SetError (error);
						}
					else
						{
						INFO_PRINTF1(_L("Open new call succeeded"));
						}
					}
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdOpenNewCall"));
	}

void CT_RMobileCallData::DoCmdOpenExistingCall(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdOpenExistingCall"));
	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		// Open call that already exists at the server (incoming call)
		TPtrC mobileLineName;
		if ( !GetStringFromConfig (aSection, KMobileLineKey (), mobileLineName))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMobileLineKey );
			SetBlockResult(EFail);
			}
		else
			{
			CT_RMobileLineData* mobileLineWrapper = static_cast<CT_RMobileLineData*>(GetDataWrapperL(mobileLineName));
			const TName* callName = NULL;
			TRAPD( error, callName = mobileLineWrapper->GetCallNameL (callNameParameter) );
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("Left while getting call name with error %d"), error);
				SetError(error);
				}
			else
				{
				INFO_PRINTF1(_L("Open handle to RMobileCall which can be used to receive incoming or dial outgoing call."));
				RMobileLine* mobileLineObject = static_cast<RMobileLine*>(GetDataObjectL(mobileLineName));
				INFO_PRINTF1(_L("Getting mobile call"));
				TRAP( error, iMobileCall = GetMobileCallL(callNameParameter) );
				if(error != KErrNone)
					{
					ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
					SetBlockResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Opening existing call"));
					error = iMobileCall->OpenExistingCall(*mobileLineObject, *callName);
					if ( error != KErrNone)
						{
						ERR_PRINTF2(_L("OpenExistingCall failed with error [%d]"), error);
						SetError (error);
						}
					else
						{
						INFO_PRINTF1(_L("OpenExistingCall succeeded"));
						}
					}
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdOpenExistingCall"));
	}

/**
 Dial out to given phone number using previously opened RMobileCall
 @param aSection
 @param KCallName
 @param KPhoneNumber
 @param KCallTimeout
 @return error - Symbian error code. KErrNone if successful
 */
void CT_RMobileCallData::DoCmdDial(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdDial"));

	if ( !GetStringFromConfig( aSection, KPhoneNumber, iPhoneNumber ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPhoneNumber);
		SetBlockResult(EFail);
		}
	else
		{
		TInt callNameParameter;
		if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Getting mobile call"));
			TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("Dialing \"%S\""), &iPhoneNumber);
				iMobileCall->Dial (iActiveCallback->iStatus, iPhoneNumber);
				iActiveCallback->Activate(aAsyncErrorIndex);
				IncOutstanding ();
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdDial"));
	}

void CT_RMobileCallData::DoCmdAnswerIncomingCall(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdAnswerIncomingCallAsync"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		TRequestStatus status;
		// Call status is read to a RCall:TStatus variable.
		RCall::TStatus callStatus;
		TBool callReady = EFalse;
	
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			while (callReady == EFalse)
				{
				INFO_PRINTF1(_L("Getting call status..."));
				error = iMobileCall->GetStatus (callStatus);
				if ( error != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to read call status [%d]"), error);
					SetError(error);
					}
				else
					{
					switch(callStatus)
						{
						case RCall::EStatusRinging:
							// Mobile call is ringing, we can answer it now.
							INFO_PRINTF1(_L("Mobile call is ringing"));
							callReady = ETrue;
						break;
						case RCall::EStatusIdle:
							// Mobile call was terminated before we could answer it.
							INFO_PRINTF1(_L("Mobile call was terminated before it could be answered"));
							SetBlockResult(EFail);
						break;
						default:
							INFO_PRINTF1(_L("Mobile call is still waiting ring"));
							INFO_PRINTF1(_L("Waiting for the phone status change..."));
							TRequestStatus status;
							iMobileCall->NotifyStatusChange(status, callStatus);
							User::WaitForRequest(status);
						break;
						}
					}
				}
			// Answer the incoming phone call now.
			INFO_PRINTF1(_L("Mobile call is ready, answering"));
			iMobileCall->AnswerIncomingCall (iActiveCallback->iStatus);
			iActiveCallback->Activate (aAsyncErrorIndex);
			IncOutstanding ();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdAnswerIncomingCallAsync"));
	}

void CT_RMobileCallData::DoCmdAnswerIncomingCallPost(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdAnswerIncomingCallPost"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Read call status to check if the other party hanged up."));
			RCall::TStatus callStatus;
			error = iMobileCall->GetStatus(callStatus);
			if ( error != KErrNone)
				{
				ERR_PRINTF2(_L("Failed to read mobile call's status [%d]"), error);
				SetError(error);
				}
			else
				{
				// Normal status of the connection is EStatusConnected.
				if ( callStatus == RCall::EStatusConnected)
					{
					INFO_PRINTF1(_L("Mobile call is connected."));
					}
				else
					{
					INFO_PRINTF1(_L("Mobile call was disconnected, hanging up."));
					iMobileCall->HangUp (iActiveCallback->iStatus);
					iActiveCallback->Activate (aAsyncErrorIndex);
					IncOutstanding ();
					}
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdAnswerIncomingCallPost"));
	}


void CT_RMobileCallData::DoCmdGetStatus()
	{
	INFO_PRINTF1(_L( "*START*CT_RMobileCallData::DoCmdCheckStatusWasConnecting" ));

	TRAPD( error, iMobileCall->GetStatus(iTimeoutedMOCallStatus) );
	if( error != KErrNone )
		{
		ERR_PRINTF2(_L("Failed to get call status with error %d"), error);
		SetError(error);
		}
	else
		{
		switch(iTimeoutedMOCallStatus)
			{
			case RCall::EStatusUnknown: 
				INFO_PRINTF1(_L("The recieving device was in unknown status when the call was cancelled."));
			break;
			case RCall::EStatusIdle: 
				INFO_PRINTF1(_L("The recieving device was idle when the call was cancelled."));
			break;
			case RCall::EStatusDialling: 
				INFO_PRINTF1(_L("The recieving device was dialing when the call was cancelled."));
			break;
			case RCall::EStatusRinging: 
				INFO_PRINTF1(_L("The recieving device was ringing when the call was cancelled."));
			break;
			case RCall::EStatusAnswering: 
				INFO_PRINTF1(_L("The recieving device was answering when the call was cancelled."));
			break;
			case RCall::EStatusConnecting: 
				INFO_PRINTF1(_L("The recieving device was connecting when the call was cancelled."));
			break;
			case RCall::EStatusConnected: 
				INFO_PRINTF1(_L("The recieving device was connected when the call was cancelled."));
			break;
			case RCall::EStatusHangingUp: 
				INFO_PRINTF1(_L("The recieving device was hanging up when the call was cancelled."));
			break;
			}
		}
	INFO_PRINTF1(_L( "*END*CT_RMobileCallData::DoCmdCheckStatusWasConnecting" ));
	}

void CT_RMobileCallData::DoCmdDialCancel(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdDialCancel"));
	
	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Cancelling dial..."));
			iMobileCall->DialCancel();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdDialCancel"));
	}

void CT_RMobileCallData::DoCmdHold(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdHold"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Holding call..."));
			iMobileCall->Hold(iActiveCallback->iStatus);
			iActiveCallback->Activate (aAsyncErrorIndex);
			IncOutstanding();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdHold"));
	}

void CT_RMobileCallData::DoCmdResume(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdResume"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Resuming call..."));
			iMobileCall->Resume (iActiveCallback->iStatus);
			iActiveCallback->Activate (aAsyncErrorIndex);
			IncOutstanding ();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdResume"));
	}

void CT_RMobileCallData::DoCmdSwap(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdSwap"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Swapping call..."));
			iMobileCall->Swap(iActiveCallback->iStatus);
			iActiveCallback->Activate(aAsyncErrorIndex);
			IncOutstanding ();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdSwap"));
	}

void CT_RMobileCallData::DoCmdTransfer(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RMobileCallData::DoCmdTransfer"));

	TInt callNameParameter;
	if ( !GetIntFromConfig(aSection, KCallName, callNameParameter ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Getting mobile call"));
		TRAPD( error, iMobileCall = GetMobileCallL(callNameParameter) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("GetMobileCallL left when trying to obtain the MobileCall with error %d"), error);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Transferring call..."));
			iMobileCall->Transfer(iActiveCallback->iStatus);
			iActiveCallback->Activate(aAsyncErrorIndex);
			IncOutstanding ();
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobileCallData::DoCmdTransfer"));
	}

RMobileCall* CT_RMobileCallData::GetMobileCallL(TInt aCall)
	{
	INFO_PRINTF1(_L("*START* CT_RMobileCallData::GetMobileCallL"));
	
	INFO_PRINTF2(_L("Get RMobileCall: %d"), aCall);

	// Check that over/under flow does not occur
	if ( aCall < 0 || aCall >= iMobileCalls.Count ())
		{
		INFO_PRINTF2(_L("There is no such call as (%d)"), aCall);
		User::Leave (KErrArgument);
		}
	INFO_PRINTF1(_L("*END* CT_RMobileCallData::GetMobileCallL"));
	return iMobileCalls[aCall];
	}


void CT_RMobileCallData::UtilityCheckCapability(const TTEFFunction& aSection)
    {
    INFO_PRINTF1(_L("*START* CT_RMobileCallData::UtilityCheckCapability"));

    TPtrC capability;
	if ( !GetStringFromConfig( aSection, KCapabilities, capability ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCapabilities);
		SetBlockResult(EFail);
		}
	else
		{
	    if (!(iCallCaps.iFlags))
	        {
	        ERR_PRINTF1(_L("Call capabilities are not supported."));
	        SetBlockResult(EFail);
	        }
	    else
	        {
	        if (capability == KCapsVoiceCall)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no voice call capabilities."));
	            if (!(RMobileCall::KCapsVoice))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsHold)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no hold capabilities."));
	            if (!(RMobileCall::KCapsHold))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsResume)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no resume capabilities."));
	            if (!(RMobileCall::KCapsResume))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsSwap)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no swap capabilities."));
	            if (!(RMobileCall::KCapsSwap))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsDeflect)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no deflect capabilities."));
	            if (!(RMobileCall::KCapsDeflect))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsTransfer)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no transfer capabilities."));
	            if (!(RMobileCall::KCapsTransfer))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsJoin)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no join (conference) capabilities."));
	            if (!(RMobileCall::KCapsJoin))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else if (capability == KCapsOneToOne)
	            {
	            INFO_PRINTF1(_L("Checking if Call has no OneToOne (conference) capabilities."));
	            if (!(RMobileCall::KCapsOneToOne))
	                {
	                SetBlockResult(EFail);
	                }
	            }
	        else
	        	{
	        	ERR_PRINTF1(_L("Unknown capability."));
	        	SetBlockResult(EFail);
	        	}
	        }
        }
	INFO_PRINTF1(_L("*END* CT_RMobileCallData::UtilityCheckCapability"));
    }

