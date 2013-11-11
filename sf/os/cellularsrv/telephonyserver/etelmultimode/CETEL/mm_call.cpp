// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains method implementations for RMobileCall subsession
// 
//

/**
 @file
*/

#include <etelext.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include "mm_hold.h"

/************************************************************************/
//
//  RMobileCall
//
/************************************************************************/

EXPORT_C RMobileCall::RMobileCall()
	: iMmPtrHolder(NULL)
/** Default constructor, and is present only to support virtual 
function table export.
This member is internal and not intended use. */
	{
	}

EXPORT_C void RMobileCall::ConstructL()
	{
	RCall::ConstructL();
	__ASSERT_ALWAYS(iMmPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iMmPtrHolder = CMobileCallPtrHolder::NewL(CMobileCallPtrHolder::EMaxNumberCallPtrSlots,CMobileCallPtrHolder::EMaxNumberCallPtrCSlots);
	}

EXPORT_C void RMobileCall::Destruct()
	{
	RCall::Destruct();
	delete iMmPtrHolder;
	iMmPtrHolder = NULL;
	}

EXPORT_C RMobileCall::TMobileCallParamsV1::TMobileCallParamsV1() 
:	RCall::TCallParams(),
	iIdRestrict(EIdRestrictDefault), 
	iAutoRedial(EFalse)
	{
	iCug.iExplicitInvoke = EFalse;
	iCug.iCugIndex = 0;
	iCug.iSuppressOA = EFalse;
	iCug.iSuppressPrefCug = EFalse;
	iExtensionId = KETelMobileCallParamsV1; //overwrite iExtensionId setup in RCall::TCallParams c'tor
	}

EXPORT_C RMobileCall::TMobileCallParamsV2::TMobileCallParamsV2() 
:	TMobileCallParamsV1(), 
 	iBearerMode(EMulticallNotSupported)
	/** Default constructor. The bearer mode is set to EMulticallNotSupported. */
 	{
 	iExtensionId = KETelMobileCallParamsV2; //overwrite iExtensionId setup in RCall::TCallParams c'tor
 	}

EXPORT_C RMobileCall::TMobileCallParamsV7::TMobileCallParamsV7()
:	TMobileCallParamsV2(),
	iCallParamOrigin(EOriginatorUnknown),
	iBCRepeatIndicator(EBCAlternateMode)
	{
	iExtensionId = KETelMobileCallParamsV7;
	iIconId.iQualifier = EIconQualifierNotSet;
	iIconId.iIdentifier = 0;
	}


/***********************************************************************************/
//
// MobileDataCall Functional Unit
//
/***********************************************************************************/

EXPORT_C RMobileCall::TMobileCallCugV1::TMobileCallCugV1() :
	iExplicitInvoke(EFalse), 
	iCugIndex(0), 
	iSuppressOA(EFalse), 
	iSuppressPrefCug(EFalse)
	/** Default constructor, and is present only to support virtual 
	function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}

EXPORT_C RMobileCall::TMobileCallDataCapsV1::TMobileCallDataCapsV1() : 
	iSpeedCaps(0), 
	iProtocolCaps(0), 
	iServiceCaps(0), 
	iQoSCaps(0), 
	iHscsdSupport(EFalse),
	iMClass(0), 
	iMaxRxTimeSlots(0), 
	iMaxTxTimeSlots(0), 
	iTotalRxTxTimeSlots(0), 
	iCodingCaps(0),
	iAsymmetryCaps(0),
	iUserInitUpgrade(EFalse),
	iRLPVersionCaps(0),
	iV42bisCaps(0)
	/** Default constructor, and is present only to support virtual 
	function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobileCall::GetMobileDataCallCaps(TDes8& aCaps) const
/** Gets the current data call capabilities. The data capabilities 
provide clients with knowledge of which data call parameter values they can 
then set-up or negotiate for this call.

@param aCaps On completion, contains the current snapshot of the call's circuit 
switched data capabilities. The capabilities will be placed in an instance 
of the TMobileCallDataCapsV1Pckg.
@return KErrNone if the function member was successful, KErrNotSupported if 
call does not support circuit switched data, KErrNotFound if this 
call is not a data call. 
@capability None
*/
	{
	return Get(EMobileCallGetMobileDataCallCaps,aCaps);
	}

EXPORT_C void RMobileCall::NotifyMobileDataCallCapsChange(TRequestStatus& aReqStatus, TDes8& aCaps) const
/** Allows clients to be notified of a change in the call's data capabilities.

The data capabilities of a call can change if the phone changes mode before 
the call moves out of the idle state.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyMobileDataCallCapsChange) 
to cancel a previously placed asynchronous NotifyMobileDataCallCapsChange() 
request.

@param aReqStatus KErrNone if successful, otherwise another of the system-wide 
error codes.
@param aCaps On completion, contains the current snapshot of the call's circuit 
switched data capabilities. The new capabilities will be placed in an instance 
of the TMobileCallDataCapsV1 type. This class will have been packaged into 
a TMobileCallDataCapsV1Pckg. 
@capability None
*/
	{
	Get(EMobileCallNotifyMobileDataCallCapsChange,aReqStatus,aCaps);
	}

EXPORT_C RMobileCall::TMobileDataRLPRangesV1::TMobileDataRLPRangesV1() :
	iIWSMax(0),
	iIWSMin(0),
	iMWSMax(0),
	iMWSMin(0),
	iT1Max(0),
	iT1Min(0),
	iN2Max(0),
	iN2Min(0),
	iT4Max(0),
	iT4Min(0)
	/** Default constructor, and is present only to support virtual 
	function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}

EXPORT_C void RMobileCall::GetMobileDataCallRLPRange(TRequestStatus& aReqStatus, TInt aRLPVersion, TDes8& aRLPRange) const
/** Gets the minimum and maximum RLP parameter ranges supported 
by the phone for the specified RLP version.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallGetMobileDataCallRLPRange)with 
to cancel a previously placed asynchronous GetMobileDataCallRLPRange() request.

@param aReqStatus KErrNone if the function member was successful, KErrNotSupported 
if the phone does not support the RLP version interrogated.
@param aRLPVersion The requested RLP version.
@param aRLPRange On completion, a descriptor that will contain the RLP parameter 
ranges. The capabilities will be packaged into a TMobileDataRLPRangesV1Pckg. 
@capability NetworkControl
@capability ReadDeviceData

@see TMobileDataRLPRangesV1Pckg
@see TMobileDataRLPRangesV1
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iRLPVersion = aRLPVersion;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot1GetMobileDataCallRLPRange, iMmPtrHolder->iRLPVersion);

	SetAndGet(EMobileCallGetMobileDataCallRLPRange, aReqStatus, ptr1, aRLPRange);
	}

EXPORT_C RMobileCall::TMobileDataCallParamsV1::TMobileDataCallParamsV1() 
:	TMobileCallParamsV1(),
	iService(EServiceUnspecified),
	iSpeed(ESpeedUnspecified), 
	iProtocol(EProtocolUnspecified), 
	iQoS(EQoSUnspecified),
	iRLPVersion(ERLPNotRequested),
	iModemToMSWindowSize(0),
	iMSToModemWindowSize(0),
	iAckTimer(0),
	iRetransmissionAttempts(0),
	iResequencingPeriod(0),
	iV42bisReq(EV42bisNeitherDirection),
	iV42bisCodewordsNum(0),
	iV42bisMaxStringLength(0),
	iUseEdge(0)
/**
Default constructor, and is present only to support virtual 
function table export. 
*/
	{
	iExtensionId = KETelMobileDataCallParamsV1; //overwrite iExtensionId setup in TMobileCallParamsV1 c'tor
	}

EXPORT_C RMobileCall::TMobileDataCallParamsV2::TMobileDataCallParamsV2() 
:	TMobileDataCallParamsV1(),
	iBearerMode(EMulticallNotSupported)
/** Default constructor. The bearer mode is set to EMulticallNotSupported. */
	{
	iExtensionId = KETelMobileDataCallParamsV2; //overwrite iExtensionId setup in TMobileCallParamsV2 c'tor
	}

EXPORT_C RMobileCall::TMobileDataCallParamsV8::TMobileDataCallParamsV8() 
:	TMobileDataCallParamsV2(),
	iCallParamOrigin(EOriginatorUnknown),
	iBCRepeatIndicator(EBCAlternateMode)
/** Default constructor. The call origin is initialized to EOriginatorUnknown. */
	{
	iExtensionId = KETelMobileDataCallParamsV8; 
	iIconId.iQualifier = EIconQualifierNotSet;
	iIconId.iIdentifier = 0;
	}

EXPORT_C RMobileCall::TMobileHscsdCallParamsV1::TMobileHscsdCallParamsV1() 
:	TMobileDataCallParamsV1(),
	iWantedAiur(EAiurBpsUnspecified),
	iWantedRxTimeSlots(0), 
	iMaxTimeSlots(0), 
	iCodings(0),
	iAsymmetry(EAsymmetryNoPreference),
	iUserInitUpgrade(EFalse)
/** Default constructor, and is present only to support virtual 
function table export. */
	{
	iExtensionId = KETelMobileHscsdCallParamsV1; //overwrite iExtensionId setup in TMobileDataCallParamsV1 c'tor
	}
	

EXPORT_C RMobileCall::TMobileHscsdCallParamsV2::TMobileHscsdCallParamsV2() 
:	TMobileHscsdCallParamsV1(),
	iBearerMode(EMulticallNotSupported)
/** Default constructor. The bearer mode defaults to EMulticallNotSupported. */
	{
	iExtensionId = KETelMobileHscsdCallParamsV2; //overwrite iExtensionId setup in TMobileDataCallParamsV2 c'tor
	}

EXPORT_C RMobileCall::TMobileHscsdCallParamsV7::TMobileHscsdCallParamsV7() 
:	TMobileHscsdCallParamsV2(),
	iCallParamOrigin(EOriginatorUnknown)
/** Default constructor. The call origin is set to EOriginatorUnknown by default. */
	{
	iExtensionId = KETelMobileHscsdCallParamsV7;
	iIconId.iQualifier = EIconQualifierNotSet;
	iIconId.iIdentifier = 0;
	}
EXPORT_C RMobileCall::TMobileHscsdCallParamsV8::TMobileHscsdCallParamsV8() 
:	TMobileHscsdCallParamsV7(),
	iBCRepeatIndicator(EBCAlternateMode)
/** Default constructor. The BC Repeat Indicator is initialized to EBCAlternateMode. */
	{
	iExtensionId = KETelMobileHscsdCallParamsV8; 
	}
EXPORT_C void RMobileCall::SetDynamicHscsdParams(TRequestStatus& aReqStatus, TMobileCallAiur aAiur, TInt aRxTimeslots) const
/** Sets the call's dynamic HSCSD parameters.

This request can be used before call set-up starts or during call connection. 
If the call is not a HSCSD call then KErrNotSupported is returned.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallSetDynamicHscsdParams) 
to cancel a previously placed asynchronous SetDynamicHscsdParams() request.

@param aReqStatus KErrNone if the function member was successful, KErrNotSupported 
if the phone does not support the RLP version interrogated.
@param aAiur Wanted air interface user rate.
@param aRxTimeslots Wanted number of receive (downlink) timeslots. 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iAiur = aAiur;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot1SetDynamicHscsdParams, iMmPtrHolder->iAiur);

	iMmPtrHolder->iRxTimeslots = aRxTimeslots;
	TPtrC8& ptr2=iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot2SetDynamicHscsdParams, iMmPtrHolder->iRxTimeslots);

	Set(EMobileCallSetDynamicHscsdParams,aReqStatus,ptr1,ptr2);
	}

EXPORT_C RMobileCall::TMobileCallHscsdInfoV1::TMobileCallHscsdInfoV1() : 
	iAiur(EAiurBpsUnspecified), 
	iRxTimeSlots(0), 
	iTxTimeSlots(0), 
	iCodings(ETchCodingUnspecified)
/** Default constructor, and is present only to support virtual 
function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}

EXPORT_C RMobileCall::TMobileCallHscsdInfoV7::TMobileCallHscsdInfoV7() : 
	iCallParamOrigin(EOriginatorUnknown)
/** Default constructor, and is present only to support virtual 
function table export. */
	{
	iExtensionId = KEtelExtMultimodeV7;
	}
EXPORT_C RMobileCall::TMobileCallHscsdInfoV8::TMobileCallHscsdInfoV8() 
:	iBCRepeatIndicator(EBCAlternateMode)
/** Default constructor. The BC Repeat Indicator is initialized to EBCAlternateMode. */
	{
	iExtensionId = KEtelExtMultimodeV8; 
	}

EXPORT_C TInt RMobileCall::GetCurrentHscsdInfo(TDes8& aHSCSDInfo) const
/** Gets the current values of the HSCSD parameters of an 
active mobile HSCSD call.

@see TMobileCallHscsdInfoV1
@see TMobileCallHscsdInfoV7

@param aHSCSDInfo A descriptor that will contain the retrieved HSCSD information.

@return KErrNone if HSCSD information successfully retrieved. 
@return KErrEtelCallNotActive if call is not an active HSCSD call
@return KErrNotSupported if call does not support HSCSD

@capability None
*/
	{
	return Get(EMobileCallGetCurrentHscsdInfo,aHSCSDInfo);
	}

EXPORT_C void RMobileCall::NotifyHscsdInfoChange(TRequestStatus& aReqStatus, TDes8& aHSCSDInfo) const
/** This notification completes when the mobile call's HSCSD parameters change.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyHscsdInfoChange)to 
cancel a previously placed asynchronous NotifyHscsdInfoChange() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aHSCSDInfo A descriptor that will contain the new HSCSD information 
@capability None
*/
	{
	Get(EMobileCallNotifyHscsdInfoChange,aReqStatus,aHSCSDInfo);			
	}

/***********************************************************************************/
//
// MobileMultimediaCall functional unit
//
/***********************************************************************************/

EXPORT_C void RMobileCall::NotifyVoiceFallback(TRequestStatus& aReqStatus, TName& aCallName) const
/** Notifies the client if a Multimedia call falls back to 
a voice call due to inability in network or terminating end to comply with 
the Multimedia request.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyVoiceFallback) 
to cancel a previously placed asynchronous NotifyVoiceFallback() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aCallName On Completion, contains the name of a new voice call object 
created by the TSY. 
@capability None
*/
	{
	Get(EMobileCallNotifyVoiceFallback, aReqStatus, aCallName);
	}

/***********************************************************************************/
//
// MobileAlternatingCall functional unit
//
/***********************************************************************************/


EXPORT_C void RMobileCall::SwitchAlternatingCall(TRequestStatus& aReqStatus) const
/** Switches an alternating call to its opposite call mode.

Use RTelSubSessionBase::CancelAsyncRequest(TEMobileCallSwitchAlternatingCall) 
to cancel a previously placed asynchronous SwitchAlternatingCall() request.

@param aReqStatus On return, KErrNone if the function member was successful, 
or KErrEtelNoClientInterestedInThisCall if there are no valid clients who 
have called the associated NotifyAlternatingCallSwitch(). 
@capability NetworkServices
*/
	{
	Blank(EMobileCallSwitchAlternatingCall,aReqStatus);
	}

EXPORT_C void RMobileCall::NotifyAlternatingCallSwitch(TRequestStatus& aReqStatus) const
/** This notification completes when an alternating call successfully switches 
call mode.

Use: RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyAlternatingCallSwitch) 
to cancel a previously placed asynchronous NotifyAlternatingCallSwitch() request.

@param aReqStatus On return, KErrNone is returned after an alternating call 
successfully switched call mode, KErrMMETelAlternatingCallTerminated is returned 
if the TSY detects that the alternating call has ended while a client is still 
waiting for an alternating call mode switch. 
@capability None
*/
	{
	Blank(EMobileCallNotifyAlternatingCallSwitch,aReqStatus);
	}

/***********************************************************************************/
//
// Call control methods for all calls
//
/***********************************************************************************/

EXPORT_C RMobileCall::TMobileCallCapsV1::TMobileCallCapsV1() : 
	iCallControlCaps(0),
	iCallEventCaps(0)
	/** Default constructor, and is present only to support virtual 
	function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobileCall::GetMobileCallCaps(TDes8& aCaps) const
/** Gets the mobile call's call control and event capabilities.

@param aCaps On completion, a TMobileCallCapsV1Pckg that will contain the 
call control and event capabilities.
@return KErrNone 
@capability None

@see TMobileCallCapsV1Pckg
@see TMobileCallCapsV1
*/
	{
	return Get(EMobileCallGetMobileCallCaps, aCaps);
	}

EXPORT_C void RMobileCall::NotifyMobileCallCapsChange(TRequestStatus& aReqStatus, TDes8& aCaps) const
/** This notification completes when the mobile call's call control and event capabilities 
change.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyMobileCallCapsChange) 
to cancel a previously placed asynchronous NotifyMobileCallCapsChange() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aCaps On completion, TMobileCallCapsV1Pckg that will contain the new 
call control and event capabilities. 
@capability None

@see TMobileCallCapsV1Pckg
@see TMobileCallCapsV1
*/
	{
	Get(EMobileCallNotifyMobileCallCapsChange, aReqStatus, aCaps);
	}

EXPORT_C TInt RMobileCall::GetMobileCallStatus(TMobileCallStatus& aStatus) const
/** Gets the current status of a mobile call.

@param aStatus On completion, contains the status of the call.
@return KErrNone 
@capability None
*/
	{
	TPckg<TMobileCallStatus> ptr1(aStatus);
	return Get(EMobileCallGetMobileCallStatus, ptr1);
	}

EXPORT_C void RMobileCall::NotifyMobileCallStatusChange(TRequestStatus& aReqStatus, TMobileCallStatus& aStatus) const
/** Allows a client to be notified when the call changes state.

The request completes when the call changes state, the new state being passed 
in the aStatus parameter.

Use RTelSubSessionBase::CancelAsyncRequest(TEMobileCallNotifyMobileCallStatusChange) 
to cancel a previously placed asynchronous NotifyMobileCallStatusChange() 
request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aStatus On completion, contains the new call state. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobileCallPtrHolder::ESlot1NotifyMobileCallStatusChange, aStatus);

	Get(EMobileCallNotifyMobileCallStatusChange, aReqStatus, ptr1);
	}

EXPORT_C void RMobileCall::Hold(TRequestStatus& aReqStatus) const
/** Puts a call on hold.

It will complete when the network confirms that the call is in the hold state. 
It can be called by any client that has a handle on this call (i.e. not necessarily 
the client that first opened and dialed or answered the call). The action 
is only valid if the call is a voice call and if there are no other calls 
already on hold. If call hold is possible then it will be reflected in that 
call's dynamic capabilities, i.e. the KCapsHold bit of TMobileCallControlCaps 
will be set.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallHold) to cancel a previously 
placed asynchronous Hold() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes. 
@capability NetworkServices
*/
	{
	Blank(EMobileCallHold,aReqStatus);
	}

EXPORT_C void RMobileCall::Resume(TRequestStatus& aReqStatus) const
/** Resumes a call that is on hold.

It will complete when the network confirms that the call is in the active 
state. It can be called by any client that has a handle on this call (i.e. 
not necessarily the client that first opened and dialed or answered the call). 
This action is only valid if there are no other calls already active. If call 
resume is possible then it will be reflected in that call's dynamic capabilities, 
i.e. the KCapsResume bit of TMobileCallControlCaps will be set.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallResume) to cancel a 
previously placed asynchronous Resume() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes. 
@capability NetworkServices
*/
	{
	Blank(EMobileCallResume, aReqStatus);
	}

EXPORT_C void RMobileCall::Swap(TRequestStatus& aReqStatus) const
/** Swaps a connected call to its opposite state, either active 
or on hold. 

This function member will complete either when the call has successfully 
moved to the opposite state or if the action failed due to some reason. It 
can be called by any client that has a handle on this call (i.e. not necessarily 
the client that first opened and dialed or answered the call).

This action is possible under two circumstances:

There is just one active or held single call. This implies that there are 
no other calls already in the state to which the client wishes to move this 
call to. The client will call Swap on this call and if it is active it will 
move to the held state and if it is held it will move to the active state.

There are two single calls, one currently active and one on hold. This function 
member therefore swaps the active call with the held call, simultaneously 
placing the active call on hold and resuming the held call.

In the first circumstance, an active call's dynamic capabilities will contain 
the KCapsHold and KCapsSwap bits set. A held calls' dynamic capabilities will 
contain the KCapsResume and KCapsSwap bits set. In the second circumstance, 
both the active and held call's dynamic capabilities will contain just the 
KCapsSwap bit set. The above function member is applicable to both single 
and conference calls. If there is a single call and a conference call then 
the client can call Swap on either the single call or the conference call. 
The action will still move both the single call and the conference call to 
their opposite states.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallSwap) to cancel a previously 
placed asynchronous Swap() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes. 
@capability NetworkServices
*/
	{
	Blank(EMobileCallSwap, aReqStatus);
	}

EXPORT_C void RMobileCall::Deflect(TRequestStatus& aReqStatus, TMobileCallDeflect aDeflectType, const RMobilePhone::TMobileAddress& aDestination) const
/** Allows a client to deflect an incoming call that is still 
in the EStatusRinging state.

The destination of the deflected call is determined by the aDeflectType parameter. 
If this parameter equals EDeflectSuppliedNumber then the destination address 
is given in the aDestination parameter. If call deflection is possible then 
it will be reflected in that call's dynamic capabilities, i.e. the KCapsDeflect 
bit of TMobileCallControlCaps will be set.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallDeflect) to cancel a 
previously placed asynchronous Deflect() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aDeflectType Type of call deflection wanted.
@param aDestination The telephone number (E164) to deflect the call to. 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));


	iMmPtrHolder->iDeflectType = aDeflectType;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot1Deflect, iMmPtrHolder->iDeflectType);
	TPtrC8& ptr2=iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot2Deflect, aDestination);

	Set(EMobileCallDeflect, aReqStatus, ptr1, ptr2);
	}

EXPORT_C void RMobileCall::Transfer(TRequestStatus& aReqStatus) const
/** Allows a client to transfer a call so that the remote 
party of one call ends up connected to the remote party of another call while 
this user drops out of both calls. For call transfer to be possible, the phone 
must have one call on hold and another call either connected or still ringing 
at the remote end. If call transfer is possible then it will be reflected 
in the affected call's dynamic capabilities, i.e. the KCapsTransfer bit of 
TMobileCallControlCaps will be set.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallTransfer) to cancel 
a previously placed asynchronous Transfer() request. 

@param aReqStatus On return, KErrNone if successful, KErrMMETelWrongMode if 
the transfer is requested for a CDMA call. 
@capability NetworkServices
*/
	{
	Blank(EMobileCallTransfer,aReqStatus);
	}

EXPORT_C void RMobileCall::GoOneToOne(TRequestStatus& aReqStatus) const
/** Requests a private communication to the remote party of 
one call within a conference call. The rest of the conference call will be 
put on hold while the user and the selected remote party go "one-to-one". 
It can be called by any client that has a handle on this call (i.e. not necessarily 
the client that first opened and dialed or answered the call). The action 
is only valid if the MS is in GSM mode, the selected call is a voice call 
and it is a member of an ongoing conference call. If "one-to-one" is possible 
then it will be reflected in that call's dynamic capabilities, i.e. the KCapsOneToOne 
bit of TMobileCallControlCaps will be set.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallGoOneToOne) to cancel 
a previously placed asynchronous GoOneToOne() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes. 
@capability NetworkServices
*/
	{
	Blank(EMobileCallGoOneToOne,aReqStatus);
	}

EXPORT_C void RMobileCall::NotifyCallEvent(TRequestStatus& aReqStatus, TMobileCallEvent& aEvent) const
/** Allows the client to be notified when various call events occur.

These events may be generated locally (i.e. from within the ME) or remotely 
(by the remote connected party). The request completes when an event occurs. 
Upon completion, the aEvent parameter will return the event type.

The call (and line) status will be changed to reflect any events detected 
locally or remotely. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyCallEvent) to 
cancel a previously placed asynchronous NotifyCallEvent() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aEvent On completion, contains the new call event. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobileCallPtrHolder::ESlot1NotifyCallEvent, aEvent);

	Get(EMobileCallNotifyCallEvent, aReqStatus, ptr1);
	}

EXPORT_C void RMobileCall::DialNoFdnCheck(TRequestStatus& aReqStatus,const TDesC& aTelNumber) const
/**
Dials the number specified by aTelNumber. 

The number used for dialling is not checked against those in the Fixed 
Dialling Number list even if the FDN service is enabled.
Null CallParams are applied.

@param aTelNumber Supplies the number to dial.

@capability NetworkServices
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	// Confirm that we DO have a number to dial
 	if(aTelNumber.Length()==0)
 		{
 		TRequestStatus* status=&aReqStatus;
 		User::RequestComplete(status, KErrArgument);
 		return;
 		}

	TPtr8 nullptr(NULL,0);
 	TPtrC8& null=iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot1DialNoFdnCheck, nullptr);
 	Set(EMobileCallDialNoFdnCheck, aReqStatus, null, aTelNumber);
	}

EXPORT_C void RMobileCall::DialNoFdnCheck(TRequestStatus& aReqStatus,const TDesC8& aCallParams,const TDesC& aTelNumber) const
/**
Dials the number specified by aTelNumber. 

The number used for dialling is not checked against those in the 
Fixed Dialling Number list even if the FDN service is enabled.

@param aCallParams Supplies the call parameters.  This should be a packaged RCall::TCallParams derived class.
@param aTelNumber Supplies the number to dial.

@capability NetworkServices
@capability NetworkControl

@see TMobileCallParamsV1
@see TMobileCallParamsV2
@see TMobileCallParamsV7
@see TMobileDataCallParamsV1
@see TMobileDataCallParamsV2
@see TMobileDataCallParamsV8
@see TMobileHscsdCallParamsV1
@see TMobileHscsdCallParamsV2
@see TMobileHscsdCallParamsV7
@see TMobileHscsdCallParamsV8
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	// Confirm that we DO have a number to dial
 	if(aTelNumber.Length()==0)
 		{
 		TRequestStatus* status=&aReqStatus;
 		User::RequestComplete(status, KErrArgument);
 		return;
 		}
	
 	Set(EMobileCallDialNoFdnCheck, aReqStatus, aCallParams, aTelNumber);
	}
	
EXPORT_C void RMobileCall::DialCallControl(TRequestStatus& aStatus, const TDesC8& aCallParams, const TTelNumberC& aTelNumber) const
/**
Dial the number specified in aTelNumber using the call parameters
specified in aCallParams.  This API differs from the standard
RCall::Dial method to allow the TSY to know that the dial request 
is resulting from Call Control.

When a client wishes to dial a call (using one of the 
other dial APIs such as RCall::Dial) and Call Control is being 
used, the dial request is passed to the Call Control which can
then modify the call parameters originally specified by the
client.  The SAT engine then issues a second dial request to 
make the actual call using the new modified parameters.  
When making this second request, this API should be used to 
indicate to the TSY that this call results from Call Control.
Call parameter classes RMobileCall::TMobileCallParamsV7 and 
RMobileCall::TMobileHscsdCallParamsV2 (and in future one of their
derived classes) allow the call originator to be specified.

The API RMobileCall::GetMobileCallInfo with 
RMobileCall::TMobileCallInfoV7 also allows a client to get
information regarding the originator of a call.

Error codes returned by this API from the TSY/Etelmm via 
aStatus are:

	- KErrNone if the dial request was successful.
	- KErrArgument if no number was supplied or an invalid telephone number was passed in aTelNumber.
	- KErrPermissionDenied if the client does not have the required PlatSec capabilities or the SID of the SAT engine was not provided (see below for more details). 
	- Another error code depending on the failure.

This API is policed by both Platform Security capabilities and SID 
and can only be called by the SAT engine.

On hardware, the API can be called by providing the SID of the SAT 
engine in ETel.iby file found in \epoc32\rom\include

A patchable constant KSatEngineSid is used for defining the SID value
of the SAT engine.

For example, add the following line to ETel.iby:
patchdata etel.dll@KSatEngineSid 0x12345678

On emulator, the API can be called by providing the SID of the SAT
engine in epoc.ini file found at \epoc32\data

To do this, add the following line to \epoc32\data\epoc.ini
etel_KSatEngineSid <SID>

For example:
etel_KSatEngineSid 0x12345678

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallDialCallControl) to 
cancel a previously placed asynchronous DialCallControl() request.

@param aCallParams The call parameters. This should be a 
packaged RCall::TCallParams derived class. 

@param aTelNumber The number to dial.

@param aStatus On completion aReqStatus contains KErrNone if successful. 
Otherwise, another error code depending on failure.

@see RMobileCall::TMobileCallParamsV7
@see RMobileCall::TMobileDataCallParamsV1
@see RMobileCall::TMobileHscsdCallParamsV7

@capability NetworkServices
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	// Check that a number to dial has been passed
 	if(aTelNumber.Length()==0)
 		{
 		TRequestStatus* status=&aStatus;
 		User::RequestComplete(status, KErrArgument);
 		return;
 		}
 	
 	Set(EMobileCallDialCallControl, aStatus, aCallParams, aTelNumber); 	
	}

EXPORT_C RMobileCall::TAudioToneV3::TAudioToneV3() :
	iEvent(EAudioStop),
	iTone(ENoTone),
	iPlayedLocally(EFalse)	
/**
Default constructor.
*/
	{
	iExtensionId = KETelExtMultimodeV3;
	}
	
EXPORT_C void RMobileCall::NotifyAudioToneEvent(TRequestStatus& aReqStatus, TDes8& aToneInfo) const
/**
Notification completes when there has been a change of call control audio tone event.   
 
@param aReqStatus returns the result code after the asynchronous call completes.
@param aToneInfo Will contain the information on whether the tone is to start/stop playing, 
what tone is playing/to be played and whether the tone is to be played locally by the ME.
This should be a packaged RMobileCall::TAudioToneV3 or derived class.

@capability None

@see TAudioToneV3Pckg
*/
	{
	Get(EMobileCallNotifyAudioToneEvent, aReqStatus, aToneInfo);	
	}

/***********************************************************************************/
//
// MobilePrivacy functional unit
//
/***********************************************************************************/


EXPORT_C TInt RMobileCall::SetPrivacy(RMobilePhone::TMobilePhonePrivacy aPrivacySetting) const
/** Causes the phone to request a new privacy setting for this call.

This setting will over-ride (for this call only), the phone's default privacy 
setting.

The function member is synchronous for two reasons. Firstly because the phone 
may not be able to request the new setting immediately if the call has not 
started yet. Secondly, the CDMA network only sends confirmation of a successful 
privacy setting and will not respond if it is unable to service the request. 
This means a client needs to request a privacy setting but then not assume 
the setting is active until "notify privacy confirmation" completes.

@leave KErrMMEtelWrongMode if request made during a mode that does not support it
@param aPrivacySetting Wanted privacy setting (either on or off).
@return KErrNone if request processed successfully. KErrMMEtelWrongMode if 
request made during a mode that does not support it. 
@capability NetworkServices
*/
	{
	TPckg<RMobilePhone::TMobilePhonePrivacy> ptr1(aPrivacySetting);
	return Set(EMobileCallSetPrivacy, ptr1);	
	}

EXPORT_C void RMobileCall::NotifyPrivacyConfirmation(TRequestStatus& aReqStatus, RMobilePhone::TMobilePhonePrivacy& aPrivacySetting) const
/** This notification completes when the network confirms whether privacy is on 
or off for this call.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyPrivacyConfirmation) 
to cancel a previously placed asynchronous NotifyPrivacyConfirmation() request.

@param aReqStatus On return, KErrNone if request processed successfully. KErrMMEtelWrongMode 
if request made during a mode that does not support it.
@param aPrivacySetting On completion, the confirmed privacy setting (either 
on or off). 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileCallPtrHolder::ESlot1NotifyPrivacyConfirmation,aPrivacySetting);

	Get(EMobileCallNotifyPrivacyConfirmation,aReqStatus,ptr1);
	}

/***********************************************************************************/
//
// MobileTrafficChannel Functional Unit
//
/***********************************************************************************/



EXPORT_C TInt RMobileCall::SetTrafficChannel(TMobileCallTch aTchRequest) const
/** Sets a new value for the traffic channel setting of a mobile call.

The function member is synchronous for two reasons. Firstly because the phone 
may not be able to request the new setting immediately if the call has not 
started yet. Secondly, the CDMA network only sends confirmation of a successful 
traffic channel allocation and will not respond if it is unable to service 
the request. This means a client needs to request a traffic channel type but 
then not assume the channel is allocated until "notify traffic channel confirmation" 
completes.

@leave KErrMMEtelWrongMode if request made during a mode that does not support it
@param aTchRequest The wanted traffic channel (either analog or digital).
@return KErrNone if request processed successfully, KErrMMEtelWrongMode if 
request made during a mode that does not support it. 
@capability NetworkServices
*/
	{
	TPckg<TMobileCallTch> ptr1(aTchRequest);
	return Set(EMobileCallSetTrafficChannel,ptr1);
	}

EXPORT_C void RMobileCall::NotifyTrafficChannelConfirmation(TRequestStatus& aReqStatus, TMobileCallTch& aTchType) const
/** Allows the client to be notified when the CDMA network 
has confirmed the traffic channel type requested during call set-up or during 
call connection.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyTrafficChannelConfirmation) 
to cancel a previously placed asynchronous NotifyTrafficChannelConfirmation() 
request. 

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aTchType On completion, the traffic channel type (either analog or digital). 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileCallPtrHolder::ESlot1NotifyTrafficChannelConfirmation,aTchType);

	Get(EMobileCallNotifyTrafficChannelConfirmation,aReqStatus,ptr1);
	}

/***********************************************************************************/
//
// Call Information methods  for calls of any type
//
/***********************************************************************************/

EXPORT_C RMobileCall::TMobileCallInfoV1::TMobileCallInfoV1() 
:   iValid(0), 
	iService(RMobilePhone::EServiceUnspecified), 
	iStatus(EStatusUnknown), 
	iCallId(-1), 
	iExitCode(0), 
	iEmergency(EFalse),
	iForwarded(EFalse),
	iPrivacy(RMobilePhone::EPrivacyUnspecified),
	iAlternatingCall(RMobilePhone::EAlternatingModeUnspecified),
	iDuration(0),
	iTch(RMobileCall::ETchUnknown)
	/** Default constructor, and is present only to support virtual 
	function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}
	
EXPORT_C RMobileCall::TMobileCallInfoV3::TMobileCallInfoV3()
	: TMobileCallInfoV1(),
	  iSecurity(RMobilePhone::ECipheringOff)
/**
Default constructor.
*/
	{
	iExtensionId = KETelExtMultimodeV3;
	}

EXPORT_C RMobileCall::TMobileCallInfoV7::TMobileCallInfoV7()
	: 	TMobileCallInfoV3(),
    	iCallParamOrigin(EOriginatorUnknown),
		iParamsCallControlModified(EFalse)
/**
Default constructor.
*/
	{
	iExtensionId = KEtelExtMultimodeV7;
	iIconId.iQualifier = EIconQualifierNotSet;
	iIconId.iIdentifier = 0;
	}

EXPORT_C RMobileCall::TMobileCallInfoV8::TMobileCallInfoV8()
: 	TMobileCallInfoV7(),
    iBCRepeatIndicator(EBCAlternateMode)
/** Default constructor. The BC Repeat Indicator is initialized to EBCAlternateMode. */
	{
	iExtensionId = KEtelExtMultimodeV8;
	}

EXPORT_C TInt RMobileCall::GetMobileCallInfo(TDes8& aCallInfo) const
/** Retrieves the current snapshot of all information related 
to this call as described in the TMobileCallInfoV1 class.
@leave KErrNotFound if call information is not available
@param aCallInfo On completion, a TMobileCallInfoV1Pckg holding 
the call information block.
@return KErrNone, KErrNotFound if call information is not available 
@capability ReadUserData
*/
	{
	return Get(EMobileCallGetMobileCallInfo, aCallInfo);
	}

EXPORT_C RMobileCall::TMobileCallRemotePartyInfoV1::TMobileCallRemotePartyInfoV1() 
	: iRemoteIdStatus(ERemoteIdentityUnknown), iDirection(EDirectionUnknown)
/** Default constructor, and is present only to support virtual 
function table export. */
	{
	iExtensionId = KETelExtMultimodeV1;
	}

EXPORT_C void RMobileCall::NotifyRemotePartyInfoChange(TRequestStatus& aReqStatus, TDes8& aRemotePartyInfo) const
/** Allows the client to be notified of any change in the remote party information.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange) 
to cancel a previously placed asynchronous NotifyRemotePartyInfoChange() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aRemotePartyInfo On completion, the new remote party information in 
a TMobileCallRemotePartyInfoV1Pckg. 
@capability ReadUserData
*/
	{
	Get(EMobileCallNotifyRemotePartyInfoChange, aReqStatus, aRemotePartyInfo);
	}

/***********************************************************************************/
//
// MobileCallEmergency functional unit
// 
/***********************************************************************************/

EXPORT_C void RMobileCall::DialEmergencyCall(TRequestStatus& aReqStatus, const TDesC& aNumber) const
/** Places an emergency call request.

It is advised that the client opens a call object during system initialization 
and reserves it for an emergency call. This will guarantee that it will not 
leave with a KErrOutOfMemory, which could happen if the client had to open 
a new call to make the emergency call.

The TSY should make sure that any needed resource for an emergency call is 
created at initialization and set aside in the event of a DialEmergencyCall() 
request. Upon receiving a DialEmergencyCall() request, the TSY should ask 
the ETel server if the client is a priority client, using the following call 
to the server: CCallBase::CheckPriorityClient().

DialEmergencyCall is designated a "priority client" request, which means that 
if a client that is known to the server as a priority client calls this, the 
server will guarantee that it will not leave with KErrOutOfMemory. However, 
a non-priority client may call it without this guarantee.

This function member is similar to the standard Dial() function member in 
that it will initiate a call set-up. The emergency call object will follow 
the standard call states.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallDialEmergencyCall) to 
cancel a previously placed asynchronous DialEmergencyCall() request.

@param aReqStatus On return, KErrNone if the emergency call successfully reaches 
the connected state. If the call set-up fails, the function member will complete 
with whatever error was returned by the network.
@param aNumber The actual emergency number, which is needed to set up an emergency 
call to a particular emergency centre (e.g. Ambulance, Police, Fire Brigade, 
etc.). The aNumber parameter should be a RMobileENStore::TEmergencyNumber 
buffer. 
@capability NetworkServices
@capability NetworkControl
*/
	{
	TReqPriorityType type=EIsaEmergencyRequest;
	Set(EMobileCallDialEmergencyCall, aReqStatus, aNumber, type);
	}

/***********************************************************************************/
//
// MobileCallCompletion functional unit
// 
/***********************************************************************************/


EXPORT_C void RMobileCall::ActivateCCBS(TRequestStatus& aReqStatus, TInt& aIndex) const
/**
Activates a CCBS request on a call that has failed due to remote user busy
It will complete once the activate request has been confirmed by the network

@param aIndex the index of the CCBS call activated
@capability NetworkServices
*/
	{
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileCallPtrHolder::ESlot1ActivateCcbs, aIndex);
	Get(EMobileCallActivateCCBS,aReqStatus, ptr1);
	}

EXPORT_C TInt RMobileCall::RejectCCBS() const
/** Allows a client to specify that it does not wish to set-up 
a CCBS request on a failed call setup.

When the user is presented with the option of activating a CCBS request to 
a remote busy number, if the user chooses not to bother then this API function 
member can be used by the client application to inform the TSY of this user 
decision.

This enables the TSY to inform the network immediately that the CCBS possibility 
is not going to be used and therefore stop all relevant timers. If the client 
application calls neither ActivateCCBS() or RejectCCBS() when the call control 
capability KCapsCCbsActivate is set, the possibility of activating CCBS will 
eventually time-out and the associated call control capability will disappear.

@leave KErrNotReady if call is not expecting a CCBS rejection
@return KErrNone 
@capability NetworkServices
*/
	{
	return Blank(EMobileCallRejectCCBS);
	}


/************************************************************************************/
//
// MobileUserSignalling functional unit
//
/************************************************************************************/

EXPORT_C RMobileCall::TMobileCallUUSRequestV1::TMobileCallUUSRequestV1()
/** Default constructor, and is present only to support virtual 
function table export. */
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobileCall::GetUUSCaps(TUint32& aCaps) const
/** Gets the current instance of the UUS capabilities of the 
call. This function member can be used before making the call to find out 
which UUS the phone supports, or during the call to find out which UUS are 
active for the current call.

@leave KErrNotSupported if UUS functionality is not supported by the phone/TSY
@param aCaps On completion, the sum of TMobileCallUUSCaps constants of the 
current UUS capabilities.
@return KErrNone if the TSY/ME supports any UUS functionality, KErrNotSupported 
if the TSY/ME does not support any UUS functionality. 
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobileCallGetUUSCaps, ptr1);
	}

EXPORT_C void RMobileCall::NotifyUUSCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const
/** Notifies the clients of changes to the UUS capabilities 
of the phone.

The UUS capabilities are most likely to change when the call starts.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallNotifyUUSCapsChange) 
to cancel a previously placed asynchronous NotifyUUSCapsChange() request.

@param aReqStatus On return, KErrNone if successful, otherwise another of 
the system-wide error codes.
@param aCaps On completion, the sum of TMobileCallUUSCaps relating to the new 
UUS capabilities 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileCallPtrHolder::ESlot1NotifyUUSCapsChange, aCaps);

	Get(EMobileCallNotifyUUSCapsChange, aReqStatus, ptr1);
	}

EXPORT_C void RMobileCall::ActivateUUS(TRequestStatus& aReqStatus, const TDesC8& aUUSRequest) const
/** Specifies which User-To-User Signalling service(s) the 
phone should request to activate.

This function member can be used before a call has started or during a call 
(for UUS3 only).

In order to use UUS the calling subscriber must have the UUS supplementary 
service provisioned by the network provider and the network must have the 
capability of supporting the UUS service.

Use of this function member is only applicable to a phone whose UUS caps (TMobileCallUUSCaps) 
are set to support the required UUS service. The KCapsSetupUUS1Implicit flag 
allows the calling user to activate the UUS service implicitly by sending 
the UUI embedded within the call set-up message. The UUS Service 1 can also 
be activated explicitly. The UUS Services 2 and 3 can only be activated by 
means of an explicit request. Service 2 can only be activated when initiating 
an outgoing call, whereas Service 3 can also be activated during the call.

If UUS Service 1 is being requested, then the iUUI field of TMobileCallUUSRequestV1 
will contain any UUI data to send with the call set-up message.

If the call has not started yet, the TSY is expected to save the request information 
until it receives the next Dial request. In this case ActivateUUS should complete 
with KErrNone if the TSY supports the requested UUS or KErrNotSupported if 
the TSY can not fulfill the request for the next Dial. Once the Dial request 
is received, the phone will send the SETUP request to the network and include 
the UUS service activation request. A number of outcomes are possible once 
the phone has sent the SETUP message.

If the network does not support UUS or the caller is not subscribed to UUS 
and the service is mandatory for the call, the network will send a DISCONNECT 
or RELEASE message. The Dial request will complete with the error received 
from the network. (KErrGsmCCResourceNotAvailable or KErrGsmCCRequestedFacilityNotSubscribed).

If the network does not support UUS but the service is not mandatory, the 
network will still proceed with the call but without UUI, and the Dial request 
will complete with KErrNone when the call is connected.

If the network does support UUS then the remote user will be alerted to the 
UUS service activation.

If the remote user accepts the UUS service activation, the network will proceed 
with the call and the Dial request will complete with KErrNone when the call 
is connected.

If the remote user rejects the UUS service activation and the service is mandatory 
for the call, the network will send a DISCONNECT or RELEASE message and the 
Dial request will complete with the error received from the network (KErrGsmCCFacilityRejected).

If the remote user rejects the UUS service activation and the service is not 
mandatory, the network will still proceed with the call and the Dial request 
will complete with KErrNone when the call is connected.

Once the call is connected, clients can check the call's UUS capabilities 
to see which UUS services are active.

If this function member is used to activate the UUS Service 3 during a call 
then the connection with the remote side has already been established. In 
order to use this function member during the call the activating subscriber 
must have the UUS supplementary service 3 provisioned by the network provider 
and the network must have the capability of supporting the UUS service 3. 
Once the ActivateUUS() request is posted, the phone will send the FACILITY 
request to the network and include the UUS service activation request. A number 
of outcomes are possible once the phone has sent the FACILITY message.

If the network does not support UUS the call will remain connected but no 
UUI transmission is possible and the ActivateUUS() request will complete with 
the error returned by the network (KErrGsmCCResourceNotAvailable or 
KErrGsmCCRequestedFacilityNotSubscribed).

If the network does support UUS then the remote user will be alerted to the 
UUS service activation

If the remote user accepts the UUS service activation, the ActivateUUS() request 
will complete with KErrNone and then UUI can be transferred. 

If the remote user rejects the UUS service activation, the ActivateUUS() request 
will complete with the error returned by the network (KErrGsmCCFacilityRejected) 
and no UUI can be transferred.

Use: RTelSubSessionBase::CancelAsyncRequest(EMobileCallActivateUUS) to cancel 
a previously placed asynchronous ActivateUUS() request. 

@param aReqStatus On return, KErrNone if the UUS request was successful, an 
error message when not.
@param aUUSRequest A packaged instance of RMobileCall::TMobileCallUUSRequestV1 
and the iServiceReq field will contain the bit-wise sum of all service activation 
requests. 
@capability WriteDeviceData
@capability NetworkServices
*/
	{
	Set(EMobileCallActivateUUS, aReqStatus, aUUSRequest);
	}

EXPORT_C void RMobileCall::SendUUI(TRequestStatus& aReqStatus, TBool aMore, const TMobileCallUUI& aUUI) const
/** Sends a UUI message for UUS2 and UUS3 services to the 
remote user either during the call set-up or during the actual call. The client 
must supply the message data within the aUUI parameter.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallSendUUI) to cancel a 
previously placed asynchronous SendUUI() request.

@leave KErrGsmCCAccessInformationDiscarded if congestion forces the network to 
drop the UUI
@param aReqStatus On return, KErrNone if the UUI is successfully transferred 
or with a network generated error if the network for some reason fails to 
deliver the message to the remote user. A possible error code is 
KErrGsmCCAccessInformationDiscarded if congestion forces the network to drop the UUI.
@param aMore Indicator that there is another UUI message to follow containing 
information belonging to the same block.
@param aUUI The UUI element to send. 
@capability NetworkServices
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iMoreUUI = aMore;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobileCallPtrHolder::ESlot2SendUUI, iMmPtrHolder->iMoreUUI);

	Set(EMobileCallSendUUI, aReqStatus, ptr1, aUUI);
	}

EXPORT_C void RMobileCall::ReceiveUUI(TRequestStatus& aReqStatus, TMobileCallUUI& aUUI) const
/** Enables the client to receive the next incoming UUI message 
from the remote user.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallReceiveUUI) to cancel 
a previously placed asynchronous ReceiveUUI() request.

@param aReqStatus KErrNone
@param aUUI On completion, the UUI message data. 
@capability NetworkServices
@capability ReadUserData
*/
	{
	Get(EMobileCallReceiveUUI, aReqStatus, aUUI);
	}

EXPORT_C void RMobileCall::HangupWithUUI(TRequestStatus& aReqStatus, const TMobileCallUUI& aUUI) const
/**
This overloaded RCall::HangUp() function member enables the client to send 
a UUS1 data at the call release.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallHangupWithUUI) to cancel 
a previously placed asynchronous HangupWithUUI() request.

@param aReqStatus KErrNone
@param aUUI The UUI message to be send with the release. 
@capability NetworkServices
@capability WriteUserData
*/
	{
	Set(EMobileCallHangupWithUUI,aReqStatus,aUUI);
	}

EXPORT_C void RMobileCall::AnswerIncomingCallWithUUI(TRequestStatus& aReqStatus, const TDesC8& aCallParams, const TMobileCallUUI& aUUI) const
/**
Enables the client to answer an incoming call and send 
UUI to the calling party at the same time.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileCallAnswerWithUUI) to cancel 
a previously placed asynchronous AnswerIncomingCallWIthUUI() request.

@param aReqStatus On return, KErrNone if the call successfully connects or 
with an error if the call fails to connect for some reason.
@param aCallParams The data call parameters, can be a packaged instance of 
any of the call parameters classes.
@param aUUI The UUI element to send to the calling party. 
@capability NetworkServices
@capability WriteUserData
*/
	{
	Set(EMobileCallAnswerWithUUI,aReqStatus,aCallParams,aUUI);
	}

EXPORT_C void RMobileCall::DialISV(TRequestStatus& aStatus, const TDesC8& aCallParams, const TTelNumberC& aTelNumber)
/**
Attempts to establish a connection using the telephone number specified.
This is an asynchronous method and is intended for use by the EtelISV API

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

This method is similar to RCall::Dial(), with two exceptions.  Functionally,
RMobileCall::DialISV() is given lower capabilities than RCall::Dial()'s
NetworkServices + NetworkControl.  The second difference is up to the TSY developer;
a TSY may chose to distinguish between a CTelephony (i.e. ETel3rdParty)
RMobileCall::DialISV() and an RCall::Dial() originated dial request.  Although
it would also be correct for a TSY to treat the two in the same way; the choice
is left to the TSY writer and the use cases they can envisage.

Giving this method a lower PlatSec capability requirement means that it is more
easily accessible to the third party developers.  Also, RCall::Dial() is intended
for use by the Phone Application alone, thus, the higher RCall capability
settings may also provide it with some form of protection.  Thus, this can be
viewed as an attempt to restrict RCall::Dial() to the Phone Application.

The two different forms of dialling should be seen as a way of filtering the more
important calls, requested by the high priority Phone Application/UI, from those
requested by other applications.  The TSY can use this filtering of call requests,
perhaps to use different or dedicated channels for each IPC, prioritise one over
the other (e.g. in the case of limited channels), etc..

@param aStatus A variable that indicates the completion status of the request.
@param aCallParams The call parameters used by the TSY (a TCallParamsPckg object).
@param aTelNumber The telephone number 
@capability NetworkServices
*/
	{
	if(aTelNumber.Length()==0)
		{
		TRequestStatus* status=&aStatus;
		User::RequestComplete(status,KErrArgument);
		return;
		}
	Set(EMobileCallDialISV, aStatus, aCallParams, aTelNumber);
	}


EXPORT_C void RMobileCall::AnswerIncomingCallISV(TRequestStatus& aStatus,const TDesC8& aCallParams) 
/**
Waits for an incoming call and answers it when it arrives - asynchronous. 
The function also answers calls which are ringing when it is invoked.

This method is intended to be used by the EtelISV API

@param aStatus A variable that indicates the completion status of the request.
@param aCallParams The call parameters used by the TSY (a TCallParamsPckg object). 
@capability NetworkServices
*/
	{
	Set(EMobileCallAnswerISV, aStatus, aCallParams);
	}	


/************************************************************************************/
//
// Multimedia Calls functional unit
//
/************************************************************************************/


EXPORT_C void RMobileCall::AnswerMultimediaCallAsVoice(TRequestStatus& aReqStatus, const TDesC8& aCallParams, TName& aCallName) const
/** Rejects an incoming multimedia call, but offers fallback 
to voice. The aCallParams is now the alternative TMobileCallParamsV1, because 
the call is no longer data. The aCallName is now the name of the new voice 
call created to handle the fallback to voice.

This name is used then by the client to open the voice call by name. The TSY 
returns the name of the new voice call object in aName parameter for the Client 
to open.

@param aReqStatus Upon completion this is result code of the asynchronous call
@param aCallParams Supplies the voice bearer call parameters.
@param aCallName Supplies the voice call name. 
@capability NetworkServices
*/
	{
	SetAndGet(EMobileCallAnswerMultimediaCallAsVoice,aReqStatus,aCallParams,aCallName);
	}
	
EXPORT_C RMobileCall::TEtel3rdPartyMobileCallParamsV1::TEtel3rdPartyMobileCallParamsV1()
/** Default constructor */
: 	RCall::TCallParams(),
	iIdRestrict(EIdRestrictDefault), 
	iAutoRedial(EFalse)
	{
	iExtensionId=KETel3rdPartyCallParamsV1;
	}


