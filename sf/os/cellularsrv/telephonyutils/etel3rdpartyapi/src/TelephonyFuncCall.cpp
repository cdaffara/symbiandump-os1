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
* Code for TelephonyFuncLine class, used by CTelephony class.
*
*/


/**
 @file
*/

#include <e32def.h>
#include "TelephonyFunctions.h"
#include "TelephonyActCall.h"
#include "TelephonyActLine.h"

TInt CTelephonyFunctions::DialNewCallL(TRequestStatus& aRequestStatus, TDes8& aCallParams, 
		const CTelephony::TTelNumber& aTelNumber, CTelephony::TCallId& aCallId, const CTelephony::TPhoneLine aLine)
/**
Dial a new call. 
*/
	{
	TName myCallName;

	if(aLine != CTelephony::EVoiceLine)
		{
		return KErrAccessDenied;
		}

	if(IsRequestPending(CTelephonyFunctions::EDialNewCall)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EDialNewCall, ETrue);
		}
	
	CTelephony::TCallId myCallId;
	if(GetISVCall(myCallId) != KErrNone)
		{
		return KErrAccessDenied;
		}
		
	if(!iDialNewCall)
		{
		iDialNewCall = CDialNewCallAct::NewL(this);
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
	
	// 1. If call object has not been previously opened then call OpenNewCall.
	// 2. If the call object was previously opened and used for an AnswerIncomingCall we close the object and then call re-open it with OpenNewCall.
	// 3. If call object was previously opened and used for a dial then can re-use it. 
	
	switch(iCallPoolStatus[myCallId])
		{
	case EAnswer:
		CloseAndReset(myCallId);
	case EUnset:
		User::LeaveIfError(iCallPool[myCallId].OpenNewCall(iLineVoice, myCallName));	
		break;
	case EDial:
	default:
		break;	
		}		
	iCallPoolStatus[myCallId]=EDial;

	iReqStatusTable[EDialNewCall] = &aRequestStatus;
		
	iDialNewCall->DialNewCall(aCallParams,aTelNumber,aCallId,myCallId);	
	 
	return KErrNone;
	}

TInt CTelephonyFunctions::HoldL(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId) 
/**
Attempt to place a call on hold.
*/
	{
	if((aCallId != CTelephony::EISVCall1) &&
			(aCallId != CTelephony::EISVCall2))
		{
		return KErrAccessDenied;
		}

	if(IsRequestPending(CTelephonyFunctions::EHold)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EHold, ETrue);
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
			
	RMobileCall::TMobileCallStatus callStatus;
	if(Call(aCallId)!=NULL)
		{
		Call(aCallId)->GetMobileCallStatus(callStatus);
		if(callStatus!=RMobileCall::EStatusConnected)
			return KErrAccessDenied;
		}
	else
		{
		return KErrAccessDenied;
		}
		
	if(!iHold)
		{
		iHold = CHoldAct::NewL(this);
		}
		
	iReqStatusTable[EHold] = &aRequestStatus;
	
	iHold->Hold(aCallId);	
	 
	return KErrNone;
	}

TInt CTelephonyFunctions::ResumeL(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId) 
/**
Attempt to Resume a previously held call
*/
	{
	if((aCallId != CTelephony::EISVCall1) &&
			(aCallId != CTelephony::EISVCall2))
		{
		return KErrAccessDenied;
		}

	if(IsRequestPending(CTelephonyFunctions::EResume)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EResume, ETrue);
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
			
	RMobileCall::TMobileCallStatus callStatus;
	if(Call(aCallId)!=NULL)
		{
		Call(aCallId)->GetMobileCallStatus(callStatus);
		if(callStatus!=RMobileCall::EStatusHold)
			return KErrAccessDenied;
		}
	else
		{
		return KErrAccessDenied;
		}
			
	if(!iResume)
		{
		iResume = CResumeAct::NewL(this);
		}
		
	iReqStatusTable[EResume] = &aRequestStatus;
	
	iResume->Resume(aCallId);	
	 
	return KErrNone;
	}

TInt CTelephonyFunctions::SwapL(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId1, const 
					  CTelephony::TCallId& aCallId2) 
/**
Attempt to swap the currently held and active calls.
*/
	{

	if(!((aCallId1 == CTelephony::EISVCall1 &&
		  aCallId2 == CTelephony::EISVCall2) ||
			(aCallId1 == CTelephony::EISVCall2 &&
			aCallId2 == CTelephony::EISVCall1)
			))
		{
		return KErrAccessDenied;
		}

	if(IsRequestPending(CTelephonyFunctions::ESwap)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::ESwap, ETrue);
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
			
	RMobileCall::TMobileCallStatus callStatus1, callStatus2;
	if((Call(aCallId1)!=NULL) && (Call(aCallId2)!=NULL))
		{
		Call(aCallId1)->GetMobileCallStatus(callStatus1);
		Call(aCallId2)->GetMobileCallStatus(callStatus2);

		if (!( ((callStatus1==RMobileCall::EStatusConnected) &&
			  (callStatus2==RMobileCall::EStatusHold)) ||
		      ((callStatus1==RMobileCall::EStatusHold) &&
		    	(callStatus2==RMobileCall::EStatusConnected))))
		    {
			return KErrAccessDenied;
			}
		}
	else
		{
		return KErrAccessDenied;
		}
		
	if(!iSwap)
		{
		iSwap = CSwapAct::NewL(this);
		}
	
	iReqStatusTable[ESwap] = &aRequestStatus;
	
	iSwap->Swap(aCallId1);	
	 
	return KErrNone;
	}

TInt CTelephonyFunctions::HangupL(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId) 
/**
Attempt to hangup a call.
*/
	{
	if((aCallId != CTelephony::EISVCall1) &&
			(aCallId != CTelephony::EISVCall2))
		{
		return KErrAccessDenied;
		}

	if(IsRequestPending(CTelephonyFunctions::EHangup)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EHangup, ETrue);
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
			
	RMobileCall::TMobileCallStatus callStatus;
	if(Call(aCallId)!=NULL)
		{
		Call(aCallId)->GetMobileCallStatus(callStatus);
		if(callStatus!=RMobileCall::EStatusHold &&
		   callStatus!=RMobileCall::EStatusConnected && 
		   callStatus!=RMobileCall::EStatusConnecting &&
		   callStatus!=RMobileCall::EStatusRinging &&
		   callStatus!=RMobileCall::EStatusAnswering)
		   {
		   return KErrAccessDenied;
		   }
		}
	else
		{
		return KErrAccessDenied;
		}
		
	if(!iHangup)
		{
		iHangup = CHangupAct::NewL(this);
		}
	
	iReqStatusTable[EHangup] = &aRequestStatus;	
		
	iHangup->Hangup(aCallId);	
	 
	return KErrNone;
	}

/**
Attempt to answer an incoming call.

This can only be done with a voice line.
*/
TInt CTelephonyFunctions::AnswerIncomingCallL(TRequestStatus& aRequestStatus, CTelephony::TCallId& aCallId, 
		const CTelephony::TPhoneLine aLine)
	{
	if(aLine != CTelephony::EVoiceLine)
		{
		return KErrAccessDenied;
		}

	if(IsRequestPending(CTelephonyFunctions::EAnswerIncomingCall)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EAnswerIncomingCall, ETrue);
		}
			
	// Get free call line.  If none available then return.
	CTelephony::TCallId myCallId;
	if(GetISVCall(myCallId) != KErrNone)
		{
		return KErrAccessDenied;
		}
		 
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	RMobileCall::TMobileCallStatus callStatus;
	
	// Check that the line is in the ringing state.  
	// If it is not then there is no call to answer 
	// so return.
	User::LeaveIfError(iLineVoice.GetMobileLineStatus(callStatus));
	if(callStatus!=RMobileCall::EStatusRinging)
		{
		return KErrAccessDenied;
		}	
	
	// Our call pool object (returned from GetISVCall() above) may 
	// have been previously used.
	// This means a OpenExistingCall or OpenNewCall has been called 
	// previously which must be closed before we call 
	// OpenExistingCall again.
	if(iCallPoolStatus[myCallId]==EAnswer || iCallPoolStatus[myCallId]==EDial)
		{
		CloseAndReset(myCallId);
		}
	
	//Create an answer call AO.	
	if(iAnswerIncomingCall == NULL)
		{
		iAnswerIncomingCall = CAnswerIncomingCallAct::NewL(this, *iInternalNotifyIncomingCall, iCallPool, iCallPoolStatus);
		}
	
	iReqStatusTable[EAnswerIncomingCall] = &aRequestStatus;	
	
	// Initiate answer call request.	
	iAnswerIncomingCall->AnswerIncomingCall(aCallId,myCallId);	
	 
	return KErrNone;
	}

/**
Retrieve the current call status.
*/
TInt CTelephonyFunctions::GetCallStatusL(const CTelephony::TCallId& aCallId, TDes8& aStatus)
	{
	if((aCallId != CTelephony::EISVCall1) &&
			(aCallId != CTelephony::EISVCall2))
		{
		return KErrAccessDenied;
		}
	
	CTelephony::TCallStatusV1& CallStatusV1 = 
			reinterpret_cast<CTelephony::TCallStatusV1&> ( const_cast<TUint8&> ( *aStatus.Ptr() ) );

	RMobileCall::TMobileCallStatus callStatus=RMobileCall::EStatusUnknown;

	if(Call(aCallId) == NULL)
		{
		return KErrAccessDenied;
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
	
	User::LeaveIfError(Call(aCallId)->GetMobileCallStatus(callStatus));
	
	GetCallStatus(callStatus, CallStatusV1.iStatus);

	return KErrNone;
	}	
	
void CTelephonyFunctions::GetCallStatus(const RMobileCall::TMobileCallStatus aMMCallStatus, CTelephony::TCallStatus& aTelCallStatus)
/**
Map RMobileCall::TMobileCallStatus to CTelephony::TCallStatus
*/
	{
	switch(aMMCallStatus)
		{
	case RMobileCall::EStatusUnknown:
		aTelCallStatus=CTelephony::EStatusUnknown;
		break;
	case RMobileCall::EStatusIdle:
		aTelCallStatus=CTelephony::EStatusIdle;
		break;
	case RMobileCall::EStatusDialling:
		aTelCallStatus=CTelephony::EStatusDialling;
		break;
	case RMobileCall::EStatusRinging:
		aTelCallStatus=CTelephony::EStatusRinging;
		break;
	case RMobileCall::EStatusAnswering:
		aTelCallStatus=CTelephony::EStatusAnswering;
		break;
	case RMobileCall::EStatusConnecting:
		aTelCallStatus=CTelephony::EStatusConnecting;
		break;
	case RMobileCall::EStatusConnected:
		aTelCallStatus=CTelephony::EStatusConnected;
		break;
	case RMobileCall::EStatusReconnectPending:
		aTelCallStatus=CTelephony::EStatusReconnectPending;
		break;
	case RMobileCall::EStatusDisconnecting:
	case RMobileCall::EStatusDisconnectingWithInband:
		aTelCallStatus=CTelephony::EStatusDisconnecting;
		break;
	case RMobileCall::EStatusHold:
		aTelCallStatus=CTelephony::EStatusHold;
		break;
	case RMobileCall::EStatusTransferring:
		aTelCallStatus=CTelephony::EStatusTransferring;
		break;
	case RMobileCall::EStatusTransferAlerting:
		aTelCallStatus=CTelephony::EStatusTransferAlerting;
		break;
	default:
		aTelCallStatus=CTelephony::EStatusUnknown;
		break;
		}
	}

TInt CTelephonyFunctions::GetCallDynamicCaps(const CTelephony::TCallId& aCallId, TDes8& aCaps)
/**
Retrieve the calls dynamic capabilities.
*/
	{
	if((aCallId != CTelephony::EISVCall1) &&
			(aCallId != CTelephony::EISVCall2))
		{
		return KErrAccessDenied;
		}
				
	CTelephony::TCallCapsV1& CallCapsV1 = *( reinterpret_cast<CTelephony::TCallCapsV1*> 
												( const_cast<TUint8*> ( aCaps.Ptr() ) ) );
	RMobileCall::TMobileCallCapsV1 callCaps;
	RMobileCall::TMobileCallCapsV1Pckg callCapsPckg(callCaps);

	if(Call(aCallId)==NULL)
		{
		return KErrAccessDenied;
		}
		
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
	
	TInt ret = Call(aCallId)->GetMobileCallCaps(callCapsPckg);	 
	 
	// Set the call control capability
	CallCapsV1.iControlCaps = 0;
	if 	(callCaps.iCallControlCaps & RMobileCall::KCapsHold)
	  CallCapsV1.iControlCaps |= CTelephony::KCapsHold;
	if 	(callCaps.iCallControlCaps & RMobileCall::KCapsResume)
	  CallCapsV1.iControlCaps |= CTelephony::KCapsResume;
	if 	(callCaps.iCallControlCaps & RMobileCall::KCapsSwap)
	  CallCapsV1.iControlCaps |= CTelephony::KCapsSwap;		
		
	return ret;
	}	
	

