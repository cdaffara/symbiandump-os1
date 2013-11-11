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
//

#include <etelext.h>
#include <et_clsvr.h>
#include <etelmm.h>
#include "mm_hold.h"

/************************************************************************/
//
//  RMobileConferenceCall
//
/************************************************************************/


EXPORT_C RMobileConferenceCall::RMobileConferenceCall()
	: iMmPtrHolder(NULL)
/** Default constructor. */
	{
	}

EXPORT_C void RMobileConferenceCall::ConstructL()
/** Constructs and initializes the RMobileConferenceCall object. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iMmPtrHolder = CMobileConferenceCallPtrHolder::NewL(CMobileConferenceCallPtrHolder::EMaxNumberConferenceCallPtrSlots,
		CMobileConferenceCallPtrHolder::EMaxNumberConferenceCallPtrCSlots);
	}

EXPORT_C void RMobileConferenceCall::Destruct()
/** C++ destructor. 
@capability None
*/
	{
	delete iMmPtrHolder;
	iMmPtrHolder = NULL;
	ResetSessionHandle();
	}

EXPORT_C TInt RMobileConferenceCall::Open(RMobilePhone& aPhone)
/** Opens a RMobileConferenceCall conference call sub-session from RMobilePhone.

	The name of the sub-session opened in the TSY will equal the string defined
	by the KETelConferenceCall LIT definition.
	
	This API currently supports only one conference call object in the TSY - so
	this means only one conference call can be ongoing at any one time. The conference
	call sub-session is opened from the phone (rather than from a line) in order
	not to place any restrictions on the types of call that could be added to
	the conference. It is up to the TSY to inform clients which calls can be added
	to a conference by setting each call's dynamic KCapsJoin capability.
	
	@param aPhone The phone sub-session used to open the conference call.
	@return KErrNone if successful, a system wide error code if not.

@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TPtrC name(KETelConferenceCall);

	SetSessionHandle(*session);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);

	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileConferenceCall::Close()
/** Closes a RMobileConferenceCall sub-session. 
@capability None
*/
 	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C TInt RMobileConferenceCall::GetCaps(TUint32& aCaps) const
/** 
	Gets the current capabilities of the conference call.

	@param aCaps On completion, a descriptor that will contain the conference
	call capabilities; this consists of the sum of the TMobileConferenceCallCaps
	constants.
	@return KErrNone

@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobileConferenceCallGetCaps,ptr1);
	}

EXPORT_C void RMobileConferenceCall::NotifyCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const
/**
	Allows clients to be notified when the conference call capabilities change.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallNotifyCapsChange) 
	to cancel a previously placed asynchronous NotifyCapsChange() request.
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.
	@param aCaps On completion, a descriptor that will contain the new conference 
	call capabilities, this consists of the sum of the TMobileConferenceCallCaps 
	constants.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileConferenceCallPtrHolder::ESlot1NotifyConferenceCallCapsChange,aCaps);

	Get(EMobileConferenceCallNotifyCapsChange,aReqStatus,ptr1);
	}

EXPORT_C void RMobileConferenceCall::CreateConference(TRequestStatus& aReqStatus) const
/**
	Allows clients to create the initial 3-way conference call.
	
	The phone must be in a state where there are two single voice calls, one on 
	hold and the other connected or alerting the remote, called party.
	
	The TSY will reflect the conference creation capability by setting the KCapsJoin 
	flags of the individual calls and the KCapsCreate flag of the conference call 
	sub-session. 
	
	In GSM mode, this function member will complete when the TSY has received 
	confirmation from the network that the conference call service has been successfully 
	invoked.
	
	In CDMA mode, this function member will complete when the TSY has sent the 
	conference create request to the network. There is no response from CDMA networks 
	to ascertain whether the request failed or succeeded.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallCreateConference) 
	to cancel a previously placed asynchronous CreateConference() request. 
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.

@capability NetworkServices
*/
	{
	Blank(EMobileConferenceCallCreateConference,aReqStatus);
	}

EXPORT_C void RMobileConferenceCall::AddCall(TRequestStatus& aReqStatus, const TName& aCallName) const
/**
	Adds the single voice call to an existing conference call. 
	
	Only calls whose call control capabilities include KCapsJoin can be added 
	to a conference call. 
	
	In GSM mode, this function member will complete when the TSY has received 
	confirmation from the network that the conference call service has been successfully 
	invoked and the call added.
	
	In CDMA mode, only 3-way conference calls are currently possible, so after 
	the initial conference creation it is not possible to add further calls. Therefore 
	single calls outside of the conference will never have the KCapsJoin capability 
	set during CDMA mode. 
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallAddCall) to 
	cancel a previously placed asynchronous AddCall() request.
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.
	@param aCallName The call sub-session to add to the conference call.

@capability NetworkServices
*/
	{
	Set(EMobileConferenceCallAddCall,aReqStatus,aCallName);
	}

EXPORT_C void RMobileConferenceCall::Swap(TRequestStatus& aReqStatus) const
/**
	Allows a client to switch a conference call between "Active" and "Hold" states. 
	
	This function member will complete either when the conference call has successfully 
	moved to the opposite state or if the action failed due to some reason. Can 
	be called by any client that has a handle on the conference call (i.e. not 
	necessarily the client that first created the conference).
	
	This action is possible under two circumstances:
	
	There is just a conference call. This implies that there are no single calls 
	already in the state to which the client wishes to toggle the conference call. 
	The client will call Swap on the conference call and if it is active it will 
	move to the held state and if it is held it will move to the active state.
	
	There is a conference call and a single call, one currently active and one 
	on hold. This function member therefore swaps the conference call with the 
	single call, simultaneously placing the active one on hold and resuming the 
	held one.
	
	In either circumstance, the conference call capabilities will include the 
	KCapsSwap flag. In GSM mode, this function member will complete when the TSY 
	has received confirmation from the network that the conference call has been 
	successfully moved to its opposite state. In CDMA mode, this function member 
	will complete when the TSY has sent the swap request to the network. There 
	is no response from CDMA networks to ascertain whether the request failed 
	or succeeded.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallSwap) to cancel 
	a previously placed asynchronous Swap() request.
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.

@capability NetworkServices
*/
	{
	Blank(EMobileConferenceCallSwap,aReqStatus);
	}

EXPORT_C void RMobileConferenceCall::HangUp(TRequestStatus& aReqStatus) const
/**
	Terminates the whole conference.
	
	All calls that are currently part of the conference will also be terminated. 
	Can be called by any client that has a handle on the conference call (i.e. 
	not necessarily the client that first created the conference). 
	
	This action is possible if the conference call capabilities include the KCapsHangUp 
	flag. In GSM mode, this function member will complete when the TSY has received 
	confirmation from the network that the conference call has been fully released. 
	
	
	In CDMA mode, this function member will complete when the TSY has sent the 
	hang-up request to the network. There is no response from CDMA networks to 
	ascertain whether the request failed or succeeded.
	
	After termination, the conference call will consist of zero calls and will 
	return to the EConferenceIdle state.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallHangUp) to 
	cancel a previously placed asynchronous HangUp() request.
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.

@capability NetworkServices
*/
	{
	Blank(EMobileConferenceCallHangUp,aReqStatus);
	}

EXPORT_C TInt RMobileConferenceCall::EnumerateCalls(TInt& aCount) const
/**
	Gets the number of calls (i.e. call objects) that are 
	currently part of the conference call.
	
	This number may be zero if a conference call is not active or equal to two 
	or more if a conference call exists. 
	
	@param aCount On return, the number of calls - will be equal to 0 or >=2.
	@return KErrNone

@capability None
*/
	{
	TPckg<TInt> ptr1(aCount);
	return Get(EMobileConferenceCallEnumerateCalls,ptr1);
	}

EXPORT_C TInt RMobileConferenceCall::GetMobileCallInfo(TInt aIndex, TDes8& aCallInfo) const
/**
	Gets a current snapshot of the call information associated 
	with the call specified by the aIndex parameter.
	
	@param aIndex The index of the call = 0 to aCount-1 where aCount is the returned 
	value from EnumerateCalls()
	@param aCallInfo On completion, a RMobileCall::TMobileCallInfoV1Pckg containing 
	the large block of call information.
	@return KErrNone, if successful, KErrNotFound if call information is not available.

@capability ReadDeviceData
*/
	{
	TPckg<TUint> ptr1(aIndex);
	return Get(EMobileConferenceCallGetMobileCallInfo,ptr1,aCallInfo);	
	}

EXPORT_C TInt RMobileConferenceCall::GetConferenceStatus(TMobileConferenceStatus& aStatus) const
/** 
	Allows clients to retrieve the current status of the conference call.
		
	@param aStatus On completion, the new conference status.
	@return KErrNone if successful, a system wide error code if not.

@capability None
*/
	{
	TPckg<TMobileConferenceStatus> ptr1(aStatus);
	return Get(EMobileConferenceCallGetConferenceStatus,ptr1);
	}

EXPORT_C void RMobileConferenceCall::NotifyConferenceStatusChange(TRequestStatus& aReqStatus, TMobileConferenceStatus& aStatus) const
/**
	Allows clients to be notified of a change in the status 
	of a conference call.
	
	The notification completes when the TSY recognizes that a conference call 
	has changed state.
	
	Conference calls may change state either as a result of a direct client action 
	on the conference call sub-session (e.g. client puts the conference on hold 
	or terminates the whole conference) or as a result of a client action on an 
	individual call (e.g. a private communication with one call in a conference 
	will put the rest of the conference on hold).
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallNotifyConferenceStatusChange) 
	to cancel a previously placed asynchronous NotifyCapsChange() request.
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.
	@param aStatus On return, the conference status.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));	

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileConferenceCallPtrHolder::ESlot1NotifyConferenceStatusChange,aStatus);

	Get(EMobileConferenceCallNotifyConferenceStatusChange,aReqStatus,ptr1);
	}

EXPORT_C void RMobileConferenceCall::NotifyConferenceEvent(TRequestStatus& aReqStatus, TMobileConferenceEvent& aEvent, TName& aCallName) const
/**
	This notification will complete when a conference call event occurs.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileConferenceCallNotifyConferenceEvent) 
	to cancel a previously placed asynchronous NotifyConferenceEvent() request.
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.
	@param aEvent On completion, contains the conference event.
	@param aCallName On completion, contains the name of the individual call, if 
	the	event is a call added or dropped from a conference.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));	

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileConferenceCallPtrHolder::ESlot1NotifyConferenceEvent,aEvent);

	Get(EMobileConferenceCallNotifyConferenceEvent,aReqStatus,ptr1,aCallName);	
	}

