// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the Simulator TSY Voice Call functionality.  
// The Call classes process the Call-based requests made by ETel clients 
// and passed down to the TSY by the ETel Server.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimVoiceCallTraces.h"
#endif

#include <testconfigfileparser.h>

#include "CSimVoiceCall.h"
#include "CSimPhone.h"
#include "CSimDtmf.h"
#include "CSimTsyMode.h"

CSimVoiceCall* CSimVoiceCall::NewL(CSimLine* aLine,const TDesC& aName, CSimPhone* aPhone)
/**
* Standard two phase constructor.
*
* @param aLine pointer to the Line object.
* @param aName name of the call to be constructed
* @return CSimVoiceCall  pointer to the voice call object created
* @leave Leaves if no memory or object is not created for any reason
*/
	{
	CSimVoiceCall* voiceCall=new(ELeave) CSimVoiceCall(aLine,aName,aPhone);
	TCleanupItem newCallVoiceClose(CloseCall,voiceCall);
	CleanupStack::PushL(newCallVoiceClose);
	voiceCall->ConstructL();
	CleanupStack::Pop();
	return voiceCall;
	}

CSimVoiceCall::CSimVoiceCall(CSimLine* aLine,const TDesC& aName, CSimPhone* aPhone)
	: CSimCall(aLine,aName,aPhone)
/**
* Trivial constructor. Calls CSimCall to initialise its members
*/
	{
	}

void CSimVoiceCall::ConstructL()
/**
* Second phase of 2-Phase Constructor
* Retrieves all the pausing duration tags from the config file
*
* @param aName name of the voice call to be constructed
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_CONSTRUCTL_1, "Starting to parse Voice Call config parameters...");
	iCaps=Caps();
	iDiallingPause=iLine->CfgFile()->ItemValue(KDiallingPauseDuration,KDefaultDiallingPauseDuration);
	iConnectingPause=iLine->CfgFile()->ItemValue(KConnectingPauseDuration,KDefaultConnectingPauseDuration);
	iDisconnectingPause=iLine->CfgFile()->ItemValue(KDisconnectingPauseDuration,KDefaultDisconnectingPauseDuration);
	iAnswerIncomingPause=iLine->CfgFile()->ItemValue(KAnswerIncomingPauseDuration,KDefaultAnswerIncomingPauseDuration);
	iRemoteHangupPause=iLine->CfgFile()->ItemValue(KRemoteHangupPauseDuration,KDefaultRemoteHangupPauseDuration);

	iTimer=CSimTimer::NewL(iLine->iPhone);
	CSimCall::ConstructL();
	
	//If present read in remote party info tag
	TPtrC8 callingname, remotenumber;
	TInt delay=0;
	const CTestConfigItem* item=NULL;
	item=iLine->CfgFile()->Item(KNotifyRemotePartyInfo);
	TInt ret;
	if (item)
		{
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,delay);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element DELAY returned %d (element no. %d) from tag %s.",ret,0,KNotifyRemotePartyInfo);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,callingname);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CALLINGNAME returned %d (element no. %d) from tag %s.",ret,1,KNotifyRemotePartyInfo);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,remotenumber);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element REMOTENUMBER returned %d (element no. %d) from tag %s.",ret,2,KNotifyRemotePartyInfo);
			}
		
		iNotifyRemotePartyInfoTimer->iDelay = delay;
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iDirection = RMobileCall::EDirectionUnknown;
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iCallingName.Copy(callingname);
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteNumber.iTelNumber.Copy(remotenumber);	
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;					
		}
	else
		{
		iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
		}
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_CONSTRUCTL_5, "...Finished parsing Voice Call config parameters...");
	}

CSimVoiceCall::~CSimVoiceCall()
/**
* Destroy all the objects constructed.
* CSimTimer and CSimSysAgent objects are destroyed here
*/
	{
	delete iTimer;
	if(iAnswerIncomingCall.iNotifyPending)
		iLine->ResetAutoAnswerCallObject(this);
	}

TInt CSimVoiceCall::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
* for the TSY to process
* A request handle, request type and request data are passed to the TSY
*
* @param aTsyReqHandle
* @param aIpc IPc number representing the request
* @param aPackage data for the request
* @return KErrNone
*/
	{
	TAny* dataPtr=aPackage.Ptr1();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch (aIpc)
		{
	//
	// No Flow Control NOR Multiple Completion
	//
	case EMobileCallGetMobileCallCaps:
		return GetMobileCallCaps(aTsyReqHandle,aPackage.Des1n());

	case EMobileCallGetMobileCallStatus:
		return GetMobileCallStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*,dataPtr));

	case EMobileCallGetMobileCallInfo:
		return GetMobileCallInfo(aTsyReqHandle,aPackage.Des1n());

	case EMobileCallDialEmergencyCall:
		return DialEmergencyCall(aTsyReqHandle,aPackage.Des1u());

	case EMobileCallHold:
		return Hold(aTsyReqHandle);

	case EMobileCallResume:
		return Resume(aTsyReqHandle);

	case EMobileCallSwap:
		return Swap(aTsyReqHandle);
	
	case EMobileCallDialISV:
		return DialISV(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u());

	case EMobileCallAnswerISV:
		TInt retVal;
		TRAPD(errVal, retVal=AnswerIncomingCallISVL(aTsyReqHandle, aPackage.Des1n()));
		if (errVal != KErrNone)
			{
			return errVal;
			}
		else
			{
			return retVal;	
			}
		
	//
	// Multiple Completion Services with Immediate Server Repost
	// (Usually Notifications)
	//
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChange(aTsyReqHandle, aPackage.Des1n());
		
	case EMobileCallNotifyRemotePartyInfoChange:
		return NotifyRemotePartyInfoChange(aTsyReqHandle, aPackage.Des1n());		

	default:
		break;
		}
	return KErrNotSupported;
	}

TInt CSimVoiceCall::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc The IPC number of the request that must be cancelled.  Note: this is not the
 *             IPC number of the cancel request itself.
 * @param aTsyReqHandle The TSY Request Handle of the request to be cancelled.
 */
	{
	switch(aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChangeCancel(aTsyReqHandle);

	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChangeCancel(aTsyReqHandle);

	case EMobileCallDialEmergencyCall:
		return DialEmergencyCallCancel(aTsyReqHandle);

	case EMobileCallHold:
		return HoldCancel(aTsyReqHandle);

	case EMobileCallResume:
		return ResumeCancel(aTsyReqHandle);

	case EMobileCallSwap:
		return SwapCancel(aTsyReqHandle);
		
	case EMobileCallNotifyRemotePartyInfoChange:
		return NotifyRemotePartyInfoChangeCancel();
		
	case EMobileCallDialISV:
		return DialISVCancel(aTsyReqHandle);
		
	case EMobileCallAnswerISV:
		return AnswerIncomingCallISVCancel(aTsyReqHandle);       
	default:
		break;
		}

	return CCallBase::CancelService(aIpc,aTsyReqHandle);
	}

TInt CSimVoiceCall::DialEmergencyCall(const TTsyReqHandle aTsyReqHandle,TDesC* aTelNumber)
/**
* Process a client's emergency call request.
*
* @param aTsyReqHandle
* @param aTelNumber The telephone number to dial
* @return KErrNone
*/
	{
	return Dial(aTsyReqHandle,NULL,aTelNumber);
	}

TInt CSimVoiceCall::DialEmergencyCallCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancels an emergency call request.
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	return DialCancel(aTsyReqHandle);
	}

TInt CSimVoiceCall::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* /*aTelNumber*/)
/**
* Process a client's dial request.
*
* @param aTsyReqHandle
* @param aCallParams  the call parameters
* @param aTelNumber The telephone number to dial
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIAL_1, ">>CSimVoiceCall::Dial");
	// Note: The telephone number and call parameters should be validated against config file
	//       values here.
	
	//see where the dial request has orignated from
	if(aCallParams)
		{
		RCall::TCallParamsPckg* callparamsPckg=reinterpret_cast<RCall::TCallParamsPckg*>(const_cast<TDesC8*>(aCallParams));
		RCall::TCallParams& callparams=(*callparamsPckg)();
		
		if(callparams.ExtensionId() == RMobileCall::KETel3rdPartyCallParamsV1)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIAL_2, "<<CSimVoiceCall::Dial request from Etel 3rd Party client.");
			}
		}

	iDialRequestHandle=aTsyReqHandle;
	TInt ret;
	ret = ActionEvent(ECallEventDial,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIAL_3, "<<CSimVoiceCall::Dial");
	return ret;
	}
	
TInt CSimVoiceCall::DialCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a dial Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALCANCEL_1, ">>CSimVoiceCall::DialCancel");
	switch(iState)
		{
	case RMobileCall::EStatusIdle:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	case RMobileCall::EStatusDialling:
	case RMobileCall::EStatusConnecting:
		iTimer->DoCancel();
		// Can ignore for cancel
		TInt err;
		err = KErrNone;
		TRAP(err, (void)ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
		if (err != KErrNone)
			{
			ReqCompleted(iDialRequestHandle, err);
			}
		else
			{
			ReqCompleted(iDialRequestHandle, KErrCancel);
			}
		break;

	case RMobileCall::EStatusConnected:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	default:
		break;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALCANCEL_2, "<<CSimVoiceCall::DialCancel");
	return KErrNone;
	}

TInt CSimVoiceCall::DialISV(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* /*aTelNumber*/)
/**
* Process an EtelISV client's dial request.
*
* @param aTsyReqHandle
* @param aCallParams  the call parameters
* @param aTelNumber The telephone number to dial
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALISV_1, ">>CSimVoiceCall::DialISV");
	// Note: The telephone number and call parameters should be validated against config file
	//       values here.
	
	//see where the dial request has orignated from
	if(aCallParams)
		{
		RCall::TCallParamsPckg* callparamsPckg=reinterpret_cast<RCall::TCallParamsPckg*>(const_cast<TDesC8*>(aCallParams));
		RCall::TCallParams& callparams=(*callparamsPckg)();
		
		if(callparams.ExtensionId() == RMobileCall::KETel3rdPartyCallParamsV1)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALISV_2, "<<CSimVoiceCall::Dial request from Etel 3rd Party client.");
			}
		}

	iDialRequestHandle=aTsyReqHandle;
	TInt ret;
	ret = ActionEvent(ECallEventDial,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALISV_3, "<<CSimVoiceCall::DialISV");
	return ret;
	}

TInt CSimVoiceCall::DialISVCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels an EtelISV originated dial Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALISVCANCEL_1, ">>CSimVoiceCall::DialISVCancel");
	switch(iState)
		{
	case RMobileCall::EStatusIdle:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	case RMobileCall::EStatusDialling:
	case RMobileCall::EStatusConnecting:
		iTimer->DoCancel();
		// Can ignore for cancel
		TInt err;
		err = KErrNone;
		TRAP(err, (void)ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
		if (err != KErrNone)
			{
			ReqCompleted(iDialRequestHandle, err);
			}
		else
			{
			ReqCompleted(iDialRequestHandle, KErrCancel);
			}
		break;

	case RMobileCall::EStatusConnected:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	default:
		break;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_DIALISVCANCEL_2, "<<CSimVoiceCall::DialISVCancel");
	return KErrNone;
	}

TInt CSimVoiceCall::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams)
/**
* Register a client's interest in answering the next incoming call.
* First register interest in incoming calls with the line, then, if a call
* is already ringing, start the answer procedure.
*
* @param aTsyReqHandle
* @param aCallParams  the call parameters
* @return KErrNone
*/
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALL_1, ">>CSimVoiceCall::AnswerIncomingCall  0x%08x, state %d entry ",(TUint)this,iState);
	
	//see where the answer request has orignated from
	if(aCallParams)
		{
		RCall::TCallParamsPckg* callparamsPckg=reinterpret_cast<RCall::TCallParamsPckg*>(const_cast<TDesC8*>(aCallParams));
		RCall::TCallParams& callparams=(*callparamsPckg)();
		
		if(callparams.ExtensionId() == RMobileCall::KETel3rdPartyCallParamsV1)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALL_2, "<<CSimVoiceCall::AnswerIncomingCall request from Etel 3rd Party client.");
			}
		}
	
	TInt ret=iLine->SetAutoAnswerCallObject(this);
	if(ret!=KErrNone)
		{
		ReqCompleted(aTsyReqHandle,ret);
		return KErrNone;
		}

	iAnswerIncomingCall.iNotifyPending=ETrue;
	iAnswerIncomingCall.iNotifyHandle=aTsyReqHandle;

	if(iState==RMobileCall::EStatusRinging)
			ret = ActionEvent(ECallEventAnswerIncoming,KErrNone);	
	else if(iLine->iState==RMobileCall::EStatusRinging)
		{		
		TRAPD(errVal, ret=ChangeStateL(RMobileCall::EStatusRinging,EFalse,EFalse));
		if (errVal != KErrNone)
			{
			ret = errVal;
			}

		if(ret==KErrNone)
			ret = ActionEvent(ECallEventAnswerIncoming,KErrNone);		
		}
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALL_3, "<<CSimVoiceCall::AnswerIncomingCall  0x%08x, state %d exit",(TUint)this,iState);
	return ret;
	}

TInt CSimVoiceCall::AnswerIncomingCallCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a AnswerIncomingCall Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLCANCEL_1, ">>CSimVoiceCall::AnswerIncomingCallCancel");
	if(iAnswerIncomingCall.iNotifyPending)
		{
		iAnswerIncomingCall.iNotifyPending=EFalse;
		iLine->ResetAutoAnswerCallObject(this);
		ReqCompleted(iAnswerIncomingCall.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLCANCEL_2, "<<CSimVoiceCall::AnswerIncomingCallCancel");
	return KErrNone;
	}

TInt CSimVoiceCall::AnswerIncomingCallISVL(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams)
/**
* Register an EtelISV client's interest in answering the next incoming call.
* First register interest in incoming calls with the line, then, if a call
* is already ringing, start the answer procedure.
*
* @param aTsyReqHandle
* @param aCallParams  the call parameters
* @return KErrNone
*/
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLISVL_1, ">>CSimVoiceCall::AnswerIncomingCallISV  0x%08x, state %d entry ",(TUint)this,iState);
	
	//see where the answer request has orignated from
	if(aCallParams)
		{
		RCall::TCallParamsPckg* callparamsPckg=reinterpret_cast<RCall::TCallParamsPckg*>(const_cast<TDesC8*>(aCallParams));
		RCall::TCallParams& callparams=(*callparamsPckg)();
		
		if(callparams.ExtensionId() == RMobileCall::KETel3rdPartyCallParamsV1)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLISVL_2, "<<CSimVoiceCall::AnswerIncomingCallISV request from Etel 3rd Party client.");
			}
		}
	
	TInt ret=iLine->SetAutoAnswerCallObject(this);
	if(ret!=KErrNone)
		{
		ReqCompleted(aTsyReqHandle,ret);
		return KErrNone;
		}

	iAnswerIncomingCall.iNotifyPending=ETrue;
	iAnswerIncomingCall.iNotifyHandle=aTsyReqHandle;

	if(iState==RMobileCall::EStatusRinging)
			ret = ActionEvent(ECallEventAnswerIncoming,KErrNone);	
	else if(iLine->iState==RMobileCall::EStatusRinging)
			{
			TRAPD(leaveValue, ret=ChangeStateL(RMobileCall::EStatusRinging,EFalse,EFalse));
			if (leaveValue != KErrNone)
				{
				return leaveValue;
				}

			if (ret == KErrNone)
                {
				ret = ActionEvent(ECallEventAnswerIncoming,KErrNone);		
	 		    }		
		}
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLISVL_3, "<<CSimVoiceCall::AnswerIncomingCallISV  0x%08x, state %d exit",(TUint)this,iState);
	return ret;
	}

TInt CSimVoiceCall::AnswerIncomingCallISVCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels an EtelISV originated AnswerIncomingCall Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLISVCANCEL_1, ">>CSimVoiceCall::AnswerIncomingCallISVCancel");
	if(iAnswerIncomingCall.iNotifyPending)
		{
		iAnswerIncomingCall.iNotifyPending=EFalse;
		iLine->ResetAutoAnswerCallObject(this);
		ReqCompleted(iAnswerIncomingCall.iNotifyHandle,KErrCancel);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ANSWERINCOMINGCALLISVCANCEL_2, "<<CSimVoiceCall::AnswerIncomingCallISVCancel");
	return KErrNone;
	}

TInt CSimVoiceCall::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* /*aCallParams*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimVoiceCall::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimVoiceCall::HangUp(const TTsyReqHandle aTsyReqHandle)
/**
* Process a client's HangUp request.
*
* @param aTsyReqHandle
* @return KErrNone
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_HANGUP_1, ">>CSimVoiceCall::HangUp");
	iHangUpRequestHandle=aTsyReqHandle;
	TInt ret=ActionEvent(ECallEventHangUp,KErrNone);
	if(ret!=KErrNone)
		ReqCompleted(aTsyReqHandle,ret);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_HANGUP_2, "<<CSimVoiceCall::HangUp");
	return KErrNone;
	}

TInt CSimVoiceCall::HangUpCancel(const TTsyReqHandle /*aTsyReqHandle*/)
/**
* Cancels a HangUp Request
*
* @param aTsyReqHandle
* @return KErrNone if successfully cancelled
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_HANGUPCANCEL_1, ">>CSimVoiceCall::HangUpCancel");
	switch(iState)
		{
	case RMobileCall::EStatusIdle:
		SimPanic(EIllegalCancelRequest);		// A DialCancel should never reach the TSY in this state.
		break;

	case RMobileCall::EStatusDisconnecting:
		iTimer->DoCancel();
		// Can ignore for cancel
		TInt err;
		err = KErrNone;
		TRAP(err, (void)ChangeStateL(RMobileCall::EStatusConnected,EFalse,EFalse));
		if (err != KErrNone)
			{
			ReqCompleted(iHangUpRequestHandle, err);
			}
		else
			{
			ReqCompleted(iHangUpRequestHandle, KErrCancel);
			}
		break;

	default:
		break;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_HANGUPCANCEL_2, "<<CSimVoiceCall::HangUpCancel");
	return KErrNone;
	}

TInt CSimVoiceCall::RelinquishOwnership()
	{
	return KErrNotSupported;
	}

TInt CSimVoiceCall::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* /*aBearerService*/)
/**
* Retrieves the bearer service info
* Not supported for voice calls
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimVoiceCall::GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* /*aParams*/)
/**
* Retrives the call parameters
* Not supported for voice calls
*/
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CSimVoiceCall::LoanDataPort(const TTsyReqHandle,RCall::TCommPort*)
/**
* Loans the comm port.
* This is a data call specific request so not supported here.
*/
	{
	return KErrNotSupported;
	}
/**
* Cancels the LoanDataPort request.
* This is a data call specific request so not supported here.
*/

TInt CSimVoiceCall::LoanDataPortCancel(const TTsyReqHandle)
	{
	return KErrNotSupported;
	}

TInt CSimVoiceCall::RecoverDataPort(const TTsyReqHandle)
/**
* Recovers the comm port.
* This is a data call specific request so not supported here.
*/
	{
	return KErrNotSupported;
	}

TInt CSimVoiceCall::Hold(const TTsyReqHandle aReqHandle)
/**
 * Put the call on hold.
 * @param aReqHandle	The request handle associated with this call.
 * @return TInt			Standard error value.
 */
	{
	if(iState!=RMobileCall::EStatusConnected)
		{
		ReqCompleted(aReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}
	iHoldResumeRequestHandle=aReqHandle;
	TInt ret=ActionEvent(ECallEventHold,KErrNone);
	return ret;
	}

TInt CSimVoiceCall::HoldCancel(const TTsyReqHandle)
/**
 * Cancel a Hold request.  Since hold is completed synchronously, this function is empty.
 */
	{
	return KErrNone;
	}
	
/**
 * Cancel a NotifyConnectConfirmation request. It is possible to cancel
 * a pending notification if the client did not issue a Dial request.
 * @param aReqHandle	The request handle associated with this call.
 */
TInt CSimVoiceCall::NotifyConnectConfirmationCancel(const TTsyReqHandle aReqHandle)
	{
	if (iNotifyConnectConfirmReqHandle==aReqHandle)
		{
		iNotifyConnectConfirmReqHandle = NULL;
		}
	ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}

/**
 * Resume a pending MO Connection.
 * @param aReqHandle	The request handle associated with this call.
 * @param aConnectContinue	Boolean to indicate whether to continue/terminate 
 *							the call connection.
 * @return TInt			Standard error value.
 */
TInt CSimVoiceCall::ResumeConnect(const TTsyReqHandle aReqHandle, const TBool * aConnectContinue)
	{
	TInt ret=KErrNone;
	
	if(iWaitForConnectConfirm)
		{
		// synchronous operation for either case
		if (*aConnectContinue)
			{
			// okay to continue, initiate dialing, this will
			// change the state
			ret=ActionEvent(ECallEventConnectContinue,KErrNone);
			}
		else
			{
			// terminate the call...
	
			// there must be a dial request previously issue,
			// but we never send the dial request down to call-stack,
			// and we did not invoke the dial timer, so it will
			// never expire, so we complete the dial request here
			ReqCompleted(iDialRequestHandle,KErrNone);
			}
			
		// complete the resume request
		ReqCompleted(aReqHandle,ret);
		}
	else
		{
		// client did not issue dial request previously, this request
		// is received in the wrong state, reject this request
		ReqCompleted(aReqHandle,KErrAccessDenied);
		}
	return ret;
	}
	
/**
 * Cancel a ResumeConnect request.  Since resume is completed synchronously, this function is empty.
 */
TInt CSimVoiceCall::ResumeConnectCancel(const TTsyReqHandle)
	{
	return KErrNone;
	}


TInt CSimVoiceCall::Resume(const TTsyReqHandle aReqHandle)
/**
 * Put the call on resume.
 * @param aReqHandle	The request handle associated with this call.
 * @return TInt			Standard error value.
 */
	{
	if(iState!=RMobileCall::EStatusHold)
		{
		ReqCompleted(aReqHandle,KErrAccessDenied);
		return KErrNone;
		}
	iHoldResumeRequestHandle=aReqHandle;
	TInt ret=ActionEvent(ECallEventResume,KErrNone);
	return ret;
	}


TInt CSimVoiceCall::ResumeCancel(const TTsyReqHandle)
/**
 * Cancel a Resume request.  Since resume is completed synchronously, this function is empty.
 */
	{
	return KErrNone;
	}

TInt CSimVoiceCall::Swap(const TTsyReqHandle aReqHandle)
/**
 * If the call is active, put it on hold.  If its on hold, resume it.
 * @param aReqHandle	The request handle associated with this call.
 * @return TInt			Standard error value.
 */
	{
	TCallEvent event;
	if(iState==RMobileCall::EStatusConnected)
		{
		event=ECallEventHold;		
		}
	else if(iState==RMobileCall::EStatusHold)
		{	
		event=ECallEventResume;
		}
	else
		{
		ReqCompleted(aReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}

	iHoldResumeRequestHandle=aReqHandle;
	TInt ret=ActionEvent(event,ECallEventSwap);	
	return ret;
	}

TInt CSimVoiceCall::SwapCancel(const TTsyReqHandle)
/**
 * Cancel a Swap request.  Since swap is completed synchronously, this function is empty.
 */
	{
	return KErrNone;
	}

TInt CSimVoiceCall::ActionEvent(TCallEvent aEvent,TInt aOtherArgument)
/**
* Entry point when an event has occured that may advance the state machine.
* The aEvent parameter describes the event.
*
* This function contains the main state machine for the voice call.  The outer layer
* switches on the event type.  Where appropriate, there are inner layer switches
* or conditional statements to handle the different responses that may be required to
* the same event occurring in different states.
*
* @param aEvent The Call event to handle
* @return value represents the error state caused by the attempted state machine jump.
*/
	{
	TInt ret=KErrNone;
	__ASSERT_ALWAYS(iState!=RMobileCall::EStatusUnknown,SimPanic(ECallStatusUnknownIllegal));
	__ASSERT_ALWAYS(aEvent!=ECallEventNtRasConnected,SimPanic(ECallEventIllegal));
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_1, ">>CSimVoiceCall::ActionEvent 0x%08x %d",(TUint)this,iState);

	switch(aEvent)
		{
	case ECallEventDial:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_2, ">>CSimVoiceCall::ActionEvent = [ECallEventDial]");
		if(iState==RMobileCall::EStatusIdle)
			{			
			TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusDialling,EFalse,EFalse));
			if(ret==KErrNone)
				iTimer->Start(iDiallingPause,this);
			}
		else
			return KErrEtelCallAlreadyActive;
		break;

	case ECallEventHangUp:
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_3, ">>CSimVoiceCall::ActionEvent = [ECallEventHangUp]");
		switch(iState)
			{
		case RMobileCall::EStatusDialling:
		case RMobileCall::EStatusRinging:
		case RMobileCall::EStatusAnswering:
		case RMobileCall::EStatusConnecting:
		case RMobileCall::EStatusConnected:
		case RMobileCall::EStatusHold:
			TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusDisconnecting,EFalse,EFalse));
			if(ret==KErrNone)
				iTimer->Start(iDisconnectingPause,this);
			if(iSimDtmf)
				{
				iSimDtmf->CallClosureCallback();
				iSimDtmf=NULL;
				}
			break;
		default:
			return KErrEtelCallNotActive;
			}
		}
		break;

	case ECallEventIncomingCall:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_4, ">>CSimVoiceCall::ActionEvent = [ECallEventIncomingCall]");
		if(iState==RMobileCall::EStatusIdle)
			{
			if(iAnswerIncomingCall.iNotifyPending)
				{
				TRAP(ret,	 ret=ProcessAnswerIncomingCallL());
				}
			else
				{
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusRinging,EFalse,EFalse));
				if(ret!=KErrNone)
					return ret;
				}
			}
		else
			return KErrEtelCallAlreadyActive;
		break;

	case ECallEventAnswerIncoming:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_5, ">>CSimVoiceCall::ActionEvent = [ECallEventAnswerIncoming]");
		if(iState==RMobileCall::EStatusRinging)
			{
			TRAP(ret, ret=ProcessAnswerIncomingCallL());
			if(ret!=KErrNone)
				{
				return ret;
				}
			}
		else
			SimPanic(EIllegalStateInconsistancy);		// This is checked before calling ActionEvent
		break;

	case ECallEventRemoteHangup:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_6, ">>CSimVoiceCall::ActionEvent = [ECallEventRemoteHangup]");
		if(iState==RMobileCall::EStatusConnected)
			{
			TRAP(ret, ret=ProcessRemoteHangupL());
			if(ret!=KErrNone)
				{
				return ret;
				}
			}
		else
			SimPanic(EIllegalStateInconsistancy);		// This is checked before calling ActionEvent
		break;

	case ECallEventTimeOut:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_7, ">>CSimVoiceCall::ActionEvent = [ECallEventTimeOut]");
			switch(iState)
				{
			case RMobileCall::EStatusDialling:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_8, ">>CSimVoiceCall::State = [EStatusDialling]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnecting,EFalse,EFalse));
				if(ret==KErrNone)
					iTimer->Start(iConnectingPause,this);
				return ret;
			
			case RMobileCall::EStatusConnecting:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_9, ">>CSimVoiceCall::State = [EStatusConnecting]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnected,EFalse,EFalse));
				UpdateRemotePartyInfoDirection(RMobileCall::EStatusConnecting);
				ReqCompleted(iDialRequestHandle,ret);
				return ret;
				
			case RMobileCall::EStatusDisconnecting:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_10, ">>CSimVoiceCall::State = [EStatusDisconnecting]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusIdle,EFalse,EFalse));
				ReqCompleted(iHangUpRequestHandle,ret);
				return ret;
				
			case RMobileCall::EStatusAnswering:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_11, ">>CSimVoiceCall::State = [EStatusAnswering]");
				TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnected,EFalse,EFalse));
				UpdateRemotePartyInfoDirection(RMobileCall::EStatusAnswering);
				ReqCompleted(iAnswerIncomingCall.iNotifyHandle,ret);
				return ret;

			default:
				break;
				}
			}
		break;
	case ECallEventHold:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_12, ">>CSimVoiceCall::ActionEvent = [ECallEventHold]");
		__ASSERT_ALWAYS(iState==RMobileCall::EStatusConnected,SimPanic(EIllegalStateInconsistancy));
		TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusHold,aOtherArgument == ECallEventSwap,EFalse));
		if (ret == KErrNone)
			ReqCompleted(iHoldResumeRequestHandle,ret);
		break;

	case ECallEventResume:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_ACTIONEVENT_13, ">>CSimVoiceCall::ActionEvent = [ECallEventResume]");
		__ASSERT_ALWAYS(iState==RMobileCall::EStatusHold,SimPanic(EIllegalStateInconsistancy));
		TRAP(ret, ret=ChangeStateL(RMobileCall::EStatusConnected,aOtherArgument == ECallEventSwap,EFalse));
		if (ret == KErrNone)
			ReqCompleted(iHoldResumeRequestHandle,ret);
		break;

	case ECallEventConnectContinue:
		SimPanic(EIllegalVoiceCallEvent);	// All other events not legal for voice call.
		break;
	default:
		SimPanic(EIllegalVoiceCallEvent);	// All other events not legal for voice call.
		break;
		}

	return ret;
	}


void CSimVoiceCall::TimerCallBack(TInt /*aId*/)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_TIMERCALLBACK_1, ">>CSimVoiceCall::TimerCallBack");
	TInt ret=ActionEvent(ECallEventTimeOut,KErrNone);
	__ASSERT_ALWAYS(ret==KErrNone,SimPanic(ETimeOutEventActionFailed));
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_TIMERCALLBACK_2, "<<CSimVoiceCall::TimerCallBack");
	}

TInt CSimVoiceCall::ProcessAnswerIncomingCallL()
/**
* Answers an Incoming Call.
* First the call state must be changed to "answering", then the flag indicating
* that an answer incoming call request is no longer pending.  Finally, a new
* call object must be assigned to receive the details of the next incoming call.
*/
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_PROCESSANSWERINCOMINGCALLL_1, ">>CSimVoiceCall::ProcessAnswerIncomingCall %d , 0x%08x",iState,(TUint)this);
	TInt ret=ChangeStateL(RMobileCall::EStatusAnswering,EFalse,EFalse);
	if(ret!=KErrNone)
		return ret;
	iTimer->Start(iAnswerIncomingPause,this);
	iAnswerIncomingCall.iNotifyPending=EFalse;
	iLine->ResetAutoAnswerCallObject(this);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_PROCESSANSWERINCOMINGCALLL_2, "<<CSimVoiceCall::ProcessAnswerIncomingCall");
	return ret;
	}

TInt CSimVoiceCall::ProcessRemoteHangupL()
/**
* Hangs up a call remotely.
* First the call state must be changed to "disconnecting", then the flag indicating
* that a remote hangup request is no longer pending.  Finally, a new
* call object must be assigned to receive the next remote hangup request.
*/
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_PROCESSREMOTEHANGUPL_1, ">>CSimVoiceCall::ProcessRemoteHangupL %d, 0x%08x",iState,(TUint)this);
	TInt ret=ChangeStateL(RMobileCall::EStatusDisconnecting,EFalse,EFalse);
	if(ret!=KErrNone)
		return ret;
	iTimer->Start(iRemoteHangupPause,this);
	iLine->ResetRemoteHangupCallObject(this);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICECALL_PROCESSREMOTEHANGUPL_2, "<<CSimVoiceCall::ProcessRemoteHangupL");
	return ret;
	}

void CSimVoiceCall::SetDtmfSession(CSimDtmf* aSimDtmf)
/**
 * Set the DTMF session pointer.
 */
	{
	__ASSERT_ALWAYS(iState==RMobileCall::EStatusConnected,SimPanic(EIllegalDtmfReq));
	iSimDtmf=aSimDtmf;
	}

CSimDtmf* CSimVoiceCall::GetDtmfSession()
/**
 * Retrieve the DTMF session pointer.
 */
	{
	return iSimDtmf;
	}


TUint CSimVoiceCall::Caps()
/**
 * Return the current capabilities of this call.
 * @return TUint	Current call capabilities.
 */
	{
	TUint caps=RCall::KCapsVoice;
	if(iState!=RMobileCall::EStatusIdle)
		{
		if(iState==RMobileCall::EStatusConnected)
			{
			caps |= RCall::KCapsHangUp;
			// If there is only a single call and its state is connected then the hold capability 
			// should be set.
			// If there is more than one call then only the swap capability should be set as hold 
			// and resume apply only to single calls.
		
			TInt i=0;
			TBool singleCall = ETrue;
			TInt count=	iLine->iCalls->Count();
		
			while(singleCall && i<count)
				{
 				if(iLine->iCalls->At(i) != this && (iLine->iCalls->At(i)->iState == RMobileCall::EStatusConnected || iLine->iCalls->At(i)->iState == RMobileCall::EStatusHold))
					singleCall = EFalse;
				i++;
				}	
			
			if (singleCall)
				caps |= RMobileCall::KCapsHold;
			// Swap capability is applicable to single and multiple call
			// scenarios
			caps |= RMobileCall::KCapsSwap;
			}
		else if(iState==RMobileCall::EStatusHold)
			{
			caps |= RCall::KCapsHangUp;

			TBool otherConnected = EFalse;
			TInt count=	iLine->iCalls->Count();
			TInt i=0;
			while (!otherConnected && i<count)
				{
 				if(iLine->iCalls->At(i) != this && (iLine->iCalls->At(i)->iState == RMobileCall::EStatusConnected || iLine->iCalls->At(i)->iState == RMobileCall::EStatusHold))
					otherConnected = ETrue;
				i++;
				}	
			if(otherConnected)
				{
				caps |= RMobileCall::KCapsSwap;
				}
			else
				{
				caps |= RMobileCall::KCapsResume;
				caps |= RMobileCall::KCapsSwap;
				}					
			}
		else if (iState==RMobileCall::EStatusRinging)
			{
			if((iLine->IsAnswerCallObjectSpare()))
				caps |= RCall::KCapsAnswer;	
			}
		}
	else
		{
		if(iLine->iState==RMobileCall::EStatusIdle || iLine->iState==RMobileCall::EStatusHold)
			{
			caps |= RCall::KCapsDial;
			if((iLine->IsAnswerCallObjectSpare()))
				caps |= RCall::KCapsAnswer;
			}
		else if (iLine->iState==RMobileCall::EStatusRinging)
			{
			if((iLine->IsAnswerCallObjectSpare()))
				caps |= RCall::KCapsAnswer;			
			}
		}
	return caps;
	}

void CSimVoiceCall::UpdateRemotePartyInfoDirection(RMobileCall::TMobileCallStatus aPreviousStatus)
/**
Update the direction of the call for the remote party info member
*/
	{
	if(iNotifyRemotePartyInfoTimer && iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iRemoteIdStatus != RMobileCall::ERemoteIdentityUnknown)	
		{
		if(aPreviousStatus == RMobileCall::EStatusAnswering)
			iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iDirection = RMobileCall::EMobileTerminated;
		else
			iNotifyRemotePartyInfoTimer->iRemotePartyInfoV1.iDirection = RMobileCall::EMobileOriginated;
		}			
	}
