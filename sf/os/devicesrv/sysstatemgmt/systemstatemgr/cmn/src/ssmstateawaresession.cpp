// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <domainmember.h>
#include <ssm/ssmstateawaresession.h>

#include "cmnpanic.h"
#include "ssmdebug.h"
#include "ssmstatemonitor.h"

/**
 @internalComponent
 @released 
 */
static void Panic(TInt aReason)
	{
	User::Panic(KPanicSsmCmn, aReason);
	}

/**
 @internalComponent
 @released 
 */
class RSsmStateAwareSession::RPrivateImpl : public RDmDomain
	{
	};

//
//---------------- class RSsmStateAwareSession ------------------
//

/**
 Default constructor
 */
EXPORT_C RSsmStateAwareSession::RSsmStateAwareSession()
	: iPimpl(NULL)
	{
	}

/**
 Connects to the domain identified by the specified domain Id.
 
 @param aId The identifier of the domain to be connected to.
 @return KErrNone, if successful; otherwise one of the other system-wide error codes. 
 */
EXPORT_C TInt RSsmStateAwareSession::Connect(TDmDomainId aId)
	{
	iPimpl = new RPrivateImpl;
	if(!iPimpl)
		{
		return KErrNoMemory;
		}
	TInt err = iPimpl->Connect(KDmHierarchyIdStartup, aId);
	if (KErrNone != err)
		{
		Close();
		}
	return err;
	}

/**
 Disconnect from the domain.  
 */
EXPORT_C void RSsmStateAwareSession::Close()
	{
	if(iPimpl)
		{
		iPimpl->Close();
		}
	delete iPimpl;
	iPimpl = NULL;
	}

/**
 Reads the System Wide State.
 @return The System Wide State
 @panic ECmnErrState if not connected to a domain
 */
EXPORT_C TSsmState RSsmStateAwareSession::State() const
	{
	__ASSERT_ALWAYS(iPimpl, Panic(ECmnErrState));
	
	TUint32 ds = 0;
	//returned TDmDomainState is currently only 8 bits
	ds = iPimpl->GetState(); //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	TSsmState state;
	state.SetFromInt(ds);
	return state;
	}

/**
 Use to get a notification when the System State changes.
 @param aStatus The TRequestStatus to be completed when the System State gets changed.
 @panic ECmnErrRqstStateNotif if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::RequestStateNotification(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iPimpl, Panic(ECmnErrRqstStateNotif));
	iPimpl->RequestTransitionNotification(aStatus);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	}

/**
 Cancel an outstanding @c RequestStateNotification operation.
 @panic ECmnErrRqstStateNotifCancel if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::RequestStateNotificationCancel()
	{
	__ASSERT_ALWAYS(iPimpl, Panic(ECmnErrRqstStateNotifCancel));
	iPimpl->CancelTransitionNotification();  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
		
	}

/**
 Acknowledges the state change.
 An application must acknowledge that it has performed all actions required by the last known state of the domain. 
 
 @panic ECmnErrAcknldgStateNotif if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::AcknowledgeStateNotification(TInt aError)
	{
	__ASSERT_ALWAYS(iPimpl, Panic(ECmnErrAcknldgStateNotif));
	iPimpl->AcknowledgeLastState(aError);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	}

/**
 Acknowledges the state change.
 An application must acknowledge that it has performed all actions required by the last known state of the domain. 
 This function appears like an atomic function and minimize the risk for missing a state notification.
 @panic ECmnErrAcknldgRqstStateNotif if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::AcknowledgeAndRequestStateNotification(TInt aError, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iPimpl, Panic(ECmnErrAcknldgRqstStateNotif));
	//Typical pattern of using P&S is to subscribe first then get current state
	iPimpl->RequestTransitionNotification(aStatus);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	//Tell domain manager that we have processed the last state change.
	iPimpl->AcknowledgeLastState(aError);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	}

//
//---------------- class CSsmStateAwareSession ------------------
//

/**
 Factory method that returns an instance of this object, connected to the domain 
 identified by the specified domain Id and setup to subscribe on System State changes.
 @param aId The identifier of the domain to be connected to.
 @return A new instance of this class, connected to the domain @c aId.   
 */
EXPORT_C CSsmStateAwareSession* CSsmStateAwareSession::NewL(TDmDomainId aId)
	{
	CSsmStateAwareSession* self = CSsmStateAwareSession::NewLC(aId);
 	CleanupStack::Pop(self);
 	return self;
	}

/**
 Factory method that returns an instance of this object, connected to the domain 
 identified by the specified domain Id and setup to subscribe on System State changes.
 @param aId The identifier of the domain to be connected to.
 @return A new instance of this class, connected to the domain @c aId.   
 */
EXPORT_C CSsmStateAwareSession* CSsmStateAwareSession::NewLC(TDmDomainId aId)
	{
	CSsmStateAwareSession* self = new (ELeave) CSsmStateAwareSession;
	CleanupStack::PushL(self);
	self->ConstructL(aId);
	return self;
	}

/**
 @internalComponent 
 */
void CSsmStateAwareSession::ConstructL(TDmDomainId aId)
	{
	iMonitor = CSsmStateMonitor::NewL(*this, aId);
	}

/**
 Used to coordinate array granularity with Compress() operations.
 @internalComponent 
 */
const TInt KDefaultPtrArrayGranularity = 8;

/**
 @internalComponent 
 */
CSsmStateAwareSession::CSsmStateAwareSession() : iSubscribers(KDefaultPtrArrayGranularity)
	{
	}

/**
 Destructor
 */
EXPORT_C CSsmStateAwareSession::~CSsmStateAwareSession()
	{
	iSubscribers.Close();
	delete 	iMonitor;
	}

/**
 Reads the System State
 @panic ECmnErrStateMon if not connected to a domain
 @return The System State 
 */
EXPORT_C TSsmState CSsmStateAwareSession::State() const
	{
	__ASSERT_ALWAYS(iMonitor, Panic(ECmnErrStateMon));
	return iMonitor->State();
	}

/**
 Register for a callback when the System State changes.
 @param aSubscriber The object to receive the callback
 */
EXPORT_C void CSsmStateAwareSession::AddSubscriberL(MStateChangeNotificationSubscriber& aSubscriber)
	{
	iSubscribers.AppendL(&aSubscriber);
	}

/**
 Cancel callback subscription for System State Changes
 @param aSubscriber The object for which to cancel subsription.
 */
EXPORT_C void CSsmStateAwareSession::RemoveSubscriber(const MStateChangeNotificationSubscriber& aSubscriber)
	{
	const TInt index = iSubscribers.Find(&aSubscriber);
	if(index > KErrNotFound)
		{
		iSubscribers.Remove(index);
		
		const TInt count = iSubscribers.Count();
		if((count % KDefaultPtrArrayGranularity) == 0)
			{
			iSubscribers.Compress();
			}
		}
	}

/**
 Called from CSsmStateMonitor when the System State have changed.
 @internalComponent
 */
void CSsmStateAwareSession::NotifySubscribers(TSsmState aSsmState)
	{
	TInt count = iSubscribers.Count();
	while (count--)
		{
#ifdef _DEBUG
			TRAPD(err, iSubscribers[count]->StateChanged(aSsmState));
			if(KErrNone != err)
				{
				DEBUGPRINT2A("Illegal leave (leavecode: %d) detected. Will be ignored", err);
				}
#else
			TRAP_IGNORE(iSubscribers[count]->StateChanged(aSsmState));
#endif
		}
	} //lint !e1746 Suppress parameter 'aSsmState' could be made const reference
