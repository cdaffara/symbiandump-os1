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

#include <e32def.h>
#include <etel3rdparty.h>
#include "TelephonyFunctions.h"


// Constructors

EXPORT_C CTelephony* CTelephony::NewLC()
/**
Constructs a CTelephony object.

A pointer to this object is left on the CleanupStack.

The returned pointer is typically assigned to a pointer-variable
on the stack.

@return	Pointer to a newly created CTelephony object.
*/
	{
	CTelephony* self=new (ELeave) CTelephony();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CTelephony* CTelephony::NewL()
/**
Constructs a CTelephony object.

No pointer to this object is left on the CleanupStack.

The returned pointer is typically assigned to a pointer-variable
stored in the Heap.

@return	Pointer to a newly created CTelephony object.
*/
	{
	CTelephony* self=NewLC();
	CleanupStack::Pop();
	return self;
	}

// Destructors

CTelephony::~CTelephony() // virtual, so no export
/**
Destructor

Deletes the underlying CTelephonyFunctions object.
*/
	{
	delete iTelephonyFunctions;
	}


/**
This method is no longer supported.

@return KErrNotSupported
@publishedAll
@deprecated
*/
EXPORT_C TInt CTelephony::FindDeviceStatus(TInt& /*aCallStatus*/)
	{
	return KErrNotSupported;
	}


/**
This method is no longer supported. 

It was used to establish a circuit-switched data call, but support was removed
as functionality was no longer required by licensees. Developers should
consider using GPRS via RConnection and RSocket for data functionality instead.

@return KErrNotSupported
@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C TInt CTelephony::EstablishDataCall(TInt& /*aDataChannel*/, const TDesC& /*aDialNumber*/)
	{
	return KErrNotSupported;
	}


/**
This method is no longer supported.

It was used to establish a circuit-switched data call, but support was removed
as functionality was no longer required by licensees. Developers should
consider using GPRS via RConnection and RSocket for data functionality instead.

@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C void CTelephony::EstablishDataCall(TRequestStatus& aRequestStatus, TInt& /*aDataChannel*/,
                                            const TDesC& /*aDialNumber*/)
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;

	User::RequestComplete(reqStatusPtr, KErrNotSupported);
	}


/**
This method is no longer supported.

It was used to terminate a circuit-switched data call, but support was removed
as functionality was no longer required by licensees. Developers should
consider using GPRS via RConnection and RSocket for data functionality instead.

@return KErrNotSupported
@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C TInt CTelephony::TerminateDataCall()
	{
	return KErrNotSupported;
	}


/**
This method is no longer supported.

It was used to receive a circuit-switched data call, but support was removed
as functionality was no longer required by licensees. Developers should
consider using GPRS via RConnection and RSocket for data functionality instead.

@return KErrNotSupported
@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C TInt CTelephony::ReceiveDataCall(TInt& /*aDataChannel*/)
	{
	return KErrNotSupported;
	}


/**
This method is no longer supported.

It was used to receive a circuit-switched data call, but support was removed
as functionality was no longer required by licensees. Developers should
consider using GPRS via RConnection and RSocket for data functionality instead.

@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C void CTelephony::ReceiveDataCall(TRequestStatus& aRequestStatus, TInt& /*aDataChannel*/)
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;

	User::RequestComplete(reqStatusPtr, KErrNotSupported);
	}


/**
This method is no longer supported.

It was used to cancel an outstanding request to establish a circuit-switched
data call, but support was removed as functionality was no longer required by
licensees. Developers should consider using GPRS via RConnection and RSocket
for data functionality instead.

@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C void CTelephony::CancelEstablishDataCall()
	{
	return;
	}


/**
This method is no longer supported.

It was used to cancel an outstanding request to receive a circuit-switched data
call, but support was removed as functionality was no longer required by
licensees. Developers should consider using GPRS via RConnection and RSocket
for data functionality instead.

@publishedAll
@deprecated
@see RConnection
@see RSocket
@see RMobilePhone
*/
EXPORT_C void CTelephony::CancelReceiveDataCall()
	{
	return;
	}


// constructor support
// Not exported, as used only by functions inside this DLL, eg NewLC()

CTelephony::CTelephony()
//
//First-phase constructor
//
//Private function which cannot Leave().
//
	{
	}


// Class member functions

void CTelephony::ConstructL()
//
//Second-phase constructor
//
//Allocates and initialises CTelephony object by constructing a
//CTelephonyFunctions object where the real work is done.
//
	{
	iTelephonyFunctions = CTelephonyFunctions::NewL();
	}


EXPORT_C TInt CTelephony::CancelAsync(TCancellationRequest aCancel) const
/**
Cancels an outstanding asynchronous request.  

The result of the cancellation request will be reflected through the 
request status of the pending request.  A status of KErrCancel denotes 
that the request has been safely cancelled.

@param aCancel Canncellation code of request to be cancelled.
@capability None
*/
	{
	return iTelephonyFunctions->CancelAsync(aCancel);
	}
	
/////////////////////////////////////////////////
//Notifier related exports
/////////////////////////////////////////////////

EXPORT_C void CTelephony::NotifyChange(TRequestStatus& aReqStatus, const TNotificationEvent& aEvent, TDes8& aDes) const
/**
Registers interest in receiving change notifications for events.

Specify the event that you are interested with the aEvent parameter. 

Each event is associated with a packaged class.  Pass an instance of 
this class to this method.  Then, when the event occurs, information 
relating to the event is written into it.  The description
of CTelephony::TNotificationEvent tells you the information class for 
each event.

This is an asynchronous method. Notification can be cancelled using the
cancellation codes specified in CTelephony::TNotificationEvent. Pass the
appropriate code to CTelephony::CancelAsync() to cancel the notification
request.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aEvent Event on which change notification is requested. 
@param aDes Packaged information class.  On completion, information 
relating to the event is written into this class.  Until then, this 
class's data is not valid.
@capability Dependent This function depends on the TNotificationEvent value 
passed to it.
*/
	{
	TRequestStatus* reqStatusPtr = &aReqStatus;

	__ASSERT_ALWAYS(aReqStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aReqStatus = KRequestPending;
	TInt rc=KErrInUse;

	TRAPD(leaveCode, rc=iTelephonyFunctions->NotifyChangeL(aReqStatus, aEvent, aDes););

	if(leaveCode != KErrNone)
		{
		iTelephonyFunctions->ResetPendingNotifyRequest(aEvent);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->ResetPendingNotifyRequest(aEvent);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}

/////////////////////////////////////////////////
//Call related exports
/////////////////////////////////////////////////

EXPORT_C CTelephony::TCallParamsV1::TCallParamsV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::DialNewCall(TRequestStatus& aRequestStatus, TDes8& aCallParams, 
							 const CTelephony::TTelNumber& aTelNumber, CTelephony::TCallId& aCallId, const CTelephony::TPhoneLine aLine) const 
/**
Initiates a new call.

The client specifies the telephone number via the aTelNumber argument 
and the desired call parameters via the aCallParams argument. 

This is an asynchronous method. After successful completion of the dial request 
a call identifier is returned back to the client via the aCallId argument.  
The call identifier is needed to hold, resume, swap, and terminate the call.  
It is also needed to monitor the status of the call.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes.  Pass CTelephony::CancelAsync() CTelephony::EDialNewCallCancel to cancel.

@param aCallParams The call parameters used by the TSY.  Pass a CTelephony::TCallParamsV1Pckg, a 
packaged CTelephony::TCallParamsV1.
@param aTelNumber  Telephone number to dial.
@param aCallId Upon completion, contains the call identifier. 
@param aLine Line on which call can be made. Currently, only voice calls are supported.

@see AnswerIncomingCall()
@see Hold()
@see Resume()
@see Swap()
@see Hangup()
@see GetCallStatus()
@see GetCallDynamicCaps()
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->DialNewCallL(aRequestStatus, aCallParams, aTelNumber,aCallId,aLine));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EDialNewCall, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EDialNewCall, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::Hold(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId) const
/**
Places a call on hold. This is only for calls you dialled or answered with CTelephony. 

The call's dynamic capabilities must allow the Hold operation for 
this call to have successful completion of this request; see CTelephony::GetCallDynamicCaps().

This is an asynchronous method. Upon successful completion of Hold, line and call status might be affected by this operation. 
Hold is only available for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EHoldCancel to cancel.

@param aCallId Call ID, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().

@see AnswerIncomingCall()
@see DialNewCall()
@see Resume()
@see Swap()
@see Hangup()
@see GetCallStatus()
@see GetCallDynamicCaps()
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->HoldL(aRequestStatus, aCallId));

	if(leaveCode != KErrNone)
		{
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EHold, EFalse);	
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EHold, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::Resume(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId) const
/**
Resumes a held call. This is only for calls you dialled or answered with CTelephony.  
The client specifies the call ID via the aCallId argument. 

The call's dynamic capabilities must allow the Resume operation for this 
call to have successful completion of this request; see CTelephony::GetCallDynamicCaps().

This is an asynchronous method. Upon successful completion of Resume, line and 
call status might be affected by this operation. 
Resume is only available for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes.  Pass CTelephony::CancelAsync() CTelephony::EResumeCancel to cancel.
@param aCallId Call ID, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().

@see AnswerIncomingCall()
@see DialNewCall()
@see Hold()
@see Swap()
@see Hangup()
@see GetCallStatus()
@see GetCallDynamicCaps()
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->ResumeL(aRequestStatus, aCallId));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EResume, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EResume, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::Swap(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId1, const 
					  CTelephony::TCallId& aCallId2) const
/**
Swaps calls. This is only for calls you dialled or answered with CTelephony. 

Swap is possible only if both calls are owned by this ISV application. 
The client specifies the call ids via the aCallId1 and aCallId2 arguments. 

This is an asynchronous method.

The calls' dynamic capabilities must allow the Swap operation for this call to 
have successful completion of this request; see CTelephony::GetCallDynamicCaps().

Swap is only available for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::ESwapCancel to cancel.
@param aCallId1 ID of a call, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().
@param aCallId2 ID of another call, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().

@see AnswerIncomingCall()
@see DialNewCall()
@see Hold()
@see Resume()
@see Hangup()
@see GetCallStatus()
@see GetCallDynamicCaps()
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->SwapL(aRequestStatus, aCallId1, aCallId2));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::ESwap, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::ESwap, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::Hangup(TRequestStatus& aRequestStatus, const CTelephony::TCallId& aCallId) const
/**
Hangs up a call.  This is only for calls you dialled or answered with CTelephony. 

The client specifies the call ID via the aCallId argument. 

This is an asynchronous method. Upon successful completion of Hangup, the line 
and call status might be affected.
Hangup is available only for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EHangupCancel to cancel.
@param aCallId Call ID, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().

@see AnswerIncomingCall()
@see DialNewCall()
@see Hold()
@see Resume()
@see Swap()
@see GetCallStatus()
@see GetCallDynamicCaps()
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->HangupL(aRequestStatus, aCallId));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EHangup, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EHangup, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::AnswerIncomingCall(TRequestStatus& aRequestStatus, CTelephony::TCallId& aCallId, 
		const CTelephony::TPhoneLine aLine) const 
/**
Answers an incoming new voice call.  Fax and data calls cannot be answered.

Answering an incoming call is possible only when the voice line status 
is ringing (CTelephony::EStatusRinging) and RLine::NotifyIncomingCall
has completed.

This is an asynchronous method. Upon successful completion of 
AnswerIncomingCall() a call ID is returned to the client via the aCallId 
argument. The call identifier is needed to hold, resume, swap, and terminate 
the call.  It is also needed to monitor the status of the call.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EAnswerIncomingCallCancel to cancel 
@param aCallId On completion, contains the call ID.
@param aLine Line from which the answer request will attempted.  Currently, only the voice line is supported.

@see DialNewCall()
@see Hold()
@see Resume()
@see Swap()
@see Hangup()
@see GetCallStatus()
@see GetCallDynamicCaps()
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->AnswerIncomingCallL(aRequestStatus,aCallId,aLine));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EAnswerIncomingCall, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EAnswerIncomingCall, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C TInt CTelephony::GetCallStatus(const TCallId& aCallId, TDes8& aStatus) const
/**
Retrieves the status of the selected call specified by the aCallId argument. 

The current status of the selected call is returned to the client via the 
aStatus argument. 
GetCallStatus is available only for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aCallId Call ID, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().
@param aStatus On completion contains a CTelephony::TCallStatusV1Pckg, 
a packaged CTelephony::TCallStatusV1 containing the call status.
@return KErrNone if successful, otherwise another of the system-wide error codes.

@see AnswerIncomingCall()
@see DialNewCall()
@see Hold()
@see Resume()
@see Swap()
@see Hangup()
@see GetCallDynamicCaps()
@capability None
*/
	{
	TInt rc=KErrAccessDenied;	
	
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCallStatusL(aCallId, aStatus));

	if(leaveCode != KErrNone)
		{	
		return leaveCode;
		}
	else if (rc != KErrNone)
		{
		return rc;
		}
	return KErrNone;
	}	

EXPORT_C CTelephony::TCallCapsV1::TCallCapsV1()
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C TInt CTelephony::GetCallDynamicCaps(const TCallId& aCallId, TDes8& aCaps) const
/**
Retrieves the dynamic call capabilities for calls you dialled or answered with CTelephony. 

The client specifies the call ID via the aCallId argument. 
Upon successful retrieval the call control capabilities (Hold, Resume 
and Swap) are returned via the aCaps argument.

This functionality is not available when the phone is in "flight" mode.

@param aCallId Call ID, as returned by CTelephony::DialNewCall() or CTelephony::AnswerIncomingCall().
@param aCaps On completion contains a CTelephony::TCallCapsV1Pckg, 
a packaged CTelephony::TCallCapsV1 containing the call's dynamic capabilities.
@return KErrNone if successful, otherwise another of the system-wide error codes.

@capability None
*/
	{
	TInt rc=KErrAccessDenied;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCallDynamicCaps(aCallId, aCaps));

	if(leaveCode != KErrNone)
		{	
		return leaveCode;
		}
	else if (rc != KErrNone)
		{
		return KErrAccessDenied;
		}
	return KErrNone;
	}
	
/////////////////////////////////////////////////
//Phone related exports
/////////////////////////////////////////////////
	
EXPORT_C TInt CTelephony::TEtelISVType::VersionId() const
/**
Default constructor.
*/
	{
	return iVersionId;
	}

EXPORT_C CTelephony::TPhoneIdV1::TPhoneIdV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetPhoneId(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves the model information and unique identification of the mobile device.

This is an asynchronous method. Upon successful completion of the request the 
manufacturer name, model and serial number (IMEI or ESN) are returned back to 
the client via the aDes argument. 

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetPhoneIdCancel to cancel.
@param aDes Contains a CTelephony::TPhoneIdV1Pckg, a packaged version of the 
CTelephony::TPhoneIdV1 class which is filled on completion.
@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrAccessDenied;	
	
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetPhoneIdL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetPhoneId, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetPhoneId, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TSubscriberIdV1::TSubscriberIdV1()
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetSubscriberId(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves information about the mobile device's current subscriber. 
This will normally identify the SIM card on GSM and WCDMA networks or 
the R-UIM card on CDMA networks.  It is independent of the phone.

This is an asynchronous method. Upon successful completion of the request
the serial number of the SIM or R-UIM card is returned back to the client
via the aDes argument. 

@param aRequestStatus Returns the result code after the asynchronous 
call completes.  Pass CTelephony::CancelAsync() CTelephony::EGetSubscriberIdCancel to cancel.
@param aDes Contains a CTelephony::TSubscriberIdV1Pckg, a packaged version of the 
CTelephony::TSubscriberIdV1 class which is filled on completion.	
@capability ReadDeviceData
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	

	TRAPD(leaveCode, rc=iTelephonyFunctions->GetSubscriberIdL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetSubscriberId, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetSubscriberId, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TIndicatorV1::TIndicatorV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetIndicator(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves the battery charging indicator, the network availability 
indicator and call-in-progress indicator. 

This is an asynchronous method. The method returns two flags for each
indicator: the first is a capability flag; it says whether the indicator
is valid.  The second flag is the indicator itself.

For instance, two flags are returned for the battery charging 
indicator.  The capability flag is only set if the phone can detect 
when a charger is connected.  The indicator flag is only set when a 
charger is actually connected.  The indicator flag is only valid when 
the capability flag is set.

Six flags, two for each indicator, are returned by this method in the 
packaged aIndicator parameter.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetBatteryInfoCancel to cancel.

@param aDes Contains a CTelephony::TIndicatorV1Pckg, a packaged version of the 
CTelephony::TIndicatorV1 class which is filled on completion.	

@see TPhoneIndicators
@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetIndicatorL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetIndicator, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetIndicator, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::GetBatteryInfo(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves the status and charge level of the phone battery. 

This is an asynchronous method. Upon successful completion of the request
the battery information is returned via the packaged aDes argument. 

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetBatteryInfoCancel to cancel.
@param aDes Contains a CTelephony::TBatteryInfoV1Pckg, a packaged version of the 
CTelephony::TBatteryInfoV1 class which is filled on completion.	
@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetBatteryInfoL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetBatteryInfo, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetBatteryInfo, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TSignalStrengthV1::TSignalStrengthV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetSignalStrength(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves the phone's current signal strength via the aDes argument. 

This is an asynchronous method.

If signal strength information is not supported by the phone the method will 
return KErrNotSupported. If no signal strength information is currently 
available the method will return KErrNotFound.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetSignalStrengthCancel to cancel. 

@param aDes Contains a CTelephony::TSignalStrengthV1Pckg, a packaged version of 
the CTelephony::TSignalStrengthV1 class which is filled on completion.

@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetSignalStrengthL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetSignalStrength, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetSignalStrength, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TIccLockInfoV1::TIccLockInfoV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetLockInfo(TRequestStatus& aRequestStatus, const TIccLock& aLock, TDes8& aDes) const
/**
Some phones allow users to lock them to prevent unauthorised use. 
Typically, a code must be entered to unlock the phone. 

This method retrieves the state and status of ICC locks PIN1/2. 
The client selects the lock that it wishes to retrieve the status and 
state of. 

This is an asynchronous method. Upon successful completion of the
request the PIN1/2 information is returned via the aDes argument. 

Note that this member function does not concern the keypad locks.  
Nor does it concern the lock that the networks use to prevent a phone from 
being used on other networks.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetLockInfoCancel to cancel.

@param aLock Lock to interrogate. Either Pin1 or Pin2.

@param aDes Contains a CTelephony::TIccLockInfoV1Pckg, a packaged version of the 
CTelephony::TIccLockInfoV1 class which is filled on completion.

@capability ReadDeviceData
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetLockInfoL(aRequestStatus, aLock, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetIccLockInfo, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetIccLockInfo, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C void CTelephony::SendDTMFTones(TRequestStatus& aRequestStatus,  const TDesC& aTones) const
/**
Transmits DTMF tones across all the current active voice calls. 

This is an asynchronous method. The request completes when the tones
have been sent.
The string of DTMF characters is passed via the aTones parameter. 
The string of DTMF characters can contain one or more DTMF tone(s).

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. use CTelephony::ESendDTMFTonesCancel to cancel.

@param aTones String containing tones to transmit. It can contain one or more
 occurance of the numbers 0 to 9, * and #.
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->SendDTMFTonesL(aRequestStatus, aTones));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::ESendDTMFTones, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::ESendDTMFTones, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TFlightModeV1::TFlightModeV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetFlightMode(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves the current flight mode status. The flight mode is returned 
via the aMode argument. 

This is an asynchronous method.

If the flight mode status is set to "off" then all network-related 
requests are disabled.

If the phone’s software does not support this feature then this method 
returns KErrNotSupported.

@param aRequestStatus Returns the result code after the asynchronous 
call completes.  Pass CTelephony::CancelAsync() CTelephony::EGetFlightModeCancel to cancel.

@param aDes Contains a CTelephony::TFlightModeV1Pckg, a packaged version of the 
CTelephony::TFlightModeV1 class which is filled on completion.

@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetFlightMode(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetFlightMode, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetFlightMode, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TBatteryInfoV1::TBatteryInfoV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}
	
EXPORT_C CTelephony::TCallStatusV1::TCallStatusV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}
	
/////////////////////////////////////////////////
//Line related exports
/////////////////////////////////////////////////

EXPORT_C TInt CTelephony::GetLineStatus(const CTelephony::TPhoneLine& aLine, TDes8& aStatus) const
/**
Retrieves the status of the line selected by the aLine argument. The current 
status of the selected line is returned to the client via the aStatus argument. 

When only one call is in progress, the line status is identical to the 
status of the call.

When two calls are in progress (one active and one on hold) then the 
status of the most "active" (connected and active or in progress) call 
is returned.

This functionality is not available when the phone is in "flight" mode.

@param aLine Identifier to line from which status is to be retrieved.
@param aStatus On completion contains the current line status in a 
CTelephony::TCallStatusV1Pckg, a packaged version of CTelephony::TCallStatusV1.
@return KErrNone if successful, otherwise another of the system-wide error codes.

@capability None
*/
{
	TInt rc=KErrAccessDenied;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetLineStatus(aLine, aStatus));

	if(leaveCode != KErrNone)
		{	
		return leaveCode;
		}
	else if (rc != KErrNone)
		{
		return rc;
		}
	return KErrNone;
	}
	
EXPORT_C CTelephony::TCallInfoV1::TCallInfoV1()
/**
Default constructor.
*/
	: iCallId(-1)
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C CTelephony::TTelAddress::TTelAddress()
/**
Default constructor.
*/
	:	iTypeOfNumber(EUnknownNumber),
		iNumberPlan(EUnknownNumberingPlan),
		iTelNumber(0)
	{
	}
	
EXPORT_C CTelephony::TRemotePartyInfoV1::TRemotePartyInfoV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}
	
EXPORT_C CTelephony::TCallSelectionV1::TCallSelectionV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C TInt CTelephony::GetCallInfo(TDes8& aCallSelect, TDes8& aCallInfo, TDes8& aRemoteInfo) const
/**
Retrieves information about the call selected by the aCallSelect argument. 

If a call with the appropriate status is not available, then 
KErrNotFound is returned. Upon successful completion the call 
information will be returned via the aCallInfo and aRemoteCallInfo 
arguments.

This functionality should be used after a notification of an incoming 
call, notification of line status change to dialling, or notification 
of remote party info change. Notification is requested with 
CTelephony::NotifyChange(). After completion of the notification the 
client should immediately request information about the call.

@param aCallSelect Identifies the call for which information is retrieved.
@param aCallInfo Contains CTelephony::TCallInfoV1Pckg, the packaged CTelephony::TCallInfoV1 
parameter class which is filled on completion.
@param aRemoteInfo Contains CTelephony::TRemotePartyInfoV1Pckg, the packaged 
CTelephony::TRemotePartyInfoV1 parameter class which is filled on completion.
@return KErrNone if successful, otherwise another of the system-wide error codes.

@capability ReadUserData
*/
	{
	TInt rc=KErrInUse;	

	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCallInfoL(aCallSelect, aCallInfo, aRemoteInfo));

	if(leaveCode != KErrNone)
		{	
		return leaveCode;
		}
	else if (rc != KErrNone)
		{
		return rc;
		}
	return KErrNone;
	}
	
/////////////////////////////////////////////////
//Network related exports
/////////////////////////////////////////////////
	
EXPORT_C CTelephony::TNetworkRegistrationV1::TNetworkRegistrationV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetNetworkRegistrationStatus(TRequestStatus& aRequestStatus, TDes8& aDes) const
/**
Retrieves the current network registration status. 

This is an asynchronous method. Upon successful completion of this request
the registration status is returned via the aDes argument. 

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetNetworkRegistrationStatusCancel 
to cancel.

@param aDes Contains a CTelephony::TNetworkRegistrationV1Pckg, a packaged version 
of the CTelephony::TNetworkRegistrationV1 class which is filled on completion.

@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetNetworkRegistrationStatusL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetNetworkRegistrationStatus, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetNetworkRegistrationStatus, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TNetworkInfoV1::TNetworkInfoV1()
/**
Default constructor. 
*/
	{
	iVersionId=KEtelISVV1;
	}

/**
Default constructor. 
*/
EXPORT_C CTelephony::TNetworkInfoV2::TNetworkInfoV2()
	{
	iVersionId=KEtelISVV2;
	}

/**
Retrieves over-the-air network information about the currently 
registered mobile network. 

This is an asynchronous method. Upon successful completion the network 
information is returned via the aDes argument.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous 
call completes. Pass CTelephony::CancelAsync() CTelephony::EGetCurrentNetworkInfoCancel to 
cancel.  KErrAccessDenied is given to this request status if the device
is in flight mode.

@param aDes Contains a CTelephony::TNetworkInfoV1Pckg, a packaged version of the 
CTelephony::TNetworkInfoV1 class which is filled on completion.	

@capability ReadDeviceData because Cell ID is retrieved from the platform.
*/
EXPORT_C void CTelephony::GetCurrentNetworkInfo(TRequestStatus& aRequestStatus, TDes8& aDes) const
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;

	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCurrentNetworkInfoL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCurrentNetworkInfo, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCurrentNetworkInfo, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
EXPORT_C CTelephony::TNetworkNameV1::TNetworkNameV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

/**
Default constructor.
*/
EXPORT_C CTelephony::TNetworkNameV2::TNetworkNameV2()
	{
	iVersionId=KEtelISVV2;
	}

/**
Retrieves ICC-stored information about the (preferred) name of the
currently registered mobile network.

This is an asynchronous method. Upon successful completion the retrieved
network name(s) is returned via the aDes argument.

This functionality is not available when the phone is in "flight" mode.

Cancellation of the request can be achieved by passing
CTelephony::EGetCurrentNetworkNameCancel to the function CTelephony::CancelAsync().

Note: This information is only available on GSM/WCDMA networks.

@param aRequestStatus Returns the result code after the asynchronous call completes.
                      KErrAccessDenied will be returned if the phone is in flight mode.

@param aDes Contains a CTelephony::TNetworkNameV1Pckg, a packaged version of the
            CTelephony::TNetworkNameV1 class which is filled on completion.

@capability None
*/
EXPORT_C void CTelephony::GetCurrentNetworkName(TRequestStatus& aRequestStatus, TDes8& aDes) const
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCurrentNetworkNameL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCurrentNetworkName, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCurrentNetworkName, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
/////////////////////////////////////////////////
//Supplementary Services related exports
/////////////////////////////////////////////////	
	
EXPORT_C CTelephony::TCallForwardingSupplServicesV1::TCallForwardingSupplServicesV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}
	
EXPORT_C CTelephony::TCallBarringSupplServicesV1::TCallBarringSupplServicesV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}
	
EXPORT_C CTelephony::TCallWaitingSupplServicesV1::TCallWaitingSupplServicesV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}
	
EXPORT_C void CTelephony::GetCallForwardingStatus(TRequestStatus& aRequestStatus, const TCallForwardingCondition aCondition, TDes8& aDes, const TServiceGroup aServiceGroup) const
/**
Interrogates the current status of the call forwarding services. 

Specify the condition for enquiry via the aCondition argument. 

This is an asynchronous method. Upon successful completion the status
information is returned via the aSSInfo argument. Support is available
only for the voice line.

This functionality is not available when the phone is in "flight" mode.

Note: This information is only available on GSM/WCDMA networks.

@param aRequestStatus Returns the result code after the asynchronous call completes.  
Pass CTelephony::CancelAsync() CTelephony::EGetCallForwardingStatusCancel to 
cancel.
@param aCondition Call forwarding condition to be interrogated.

@param aDes  Contains a CTelephony::TCallForwardingSupplServicesV1Pckg, a packaged 
version of the CTelephony::TCallForwardingSupplServicesV1 class which is filled on 
completion.

@param aServiceGroup Service group to be interrogated.

@capability ReadDeviceData
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCFSupplementaryServiceStatusL(aRequestStatus, aCondition, aDes, aServiceGroup));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCFSupplServicesStatus, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCFSupplServicesStatus, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
