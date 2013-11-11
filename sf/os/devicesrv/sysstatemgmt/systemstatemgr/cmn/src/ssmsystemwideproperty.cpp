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

#include <ssm/ssmsystemwideproperty.h>
#include "ssmpropertymonitor.h"
#include "ssmdebug.h"
#include "cmnpanic.h"

const TUid KSsmServerSid = {0x2000D75B}; //UID3 of sysstatemgr.exe

//
//---------------- class RSsmSystemWideProperty ------------------
//

/**
Connect to the specified System-Wide-Property.
If the specified property does not yet exist, then this operation will still succeed.
@param aSwpKey the key(a TUint value) of the System-Wide-Property to connect
@return KErrNone if successful, otherwise one of the system wide error codes.
*/
EXPORT_C TInt RSsmSystemWideProperty::Connect(TUint aSwpKey)
	{
	return RSsmSystemWideProperty::Connect(aSwpKey, KSsmServerSid);
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
 @internalComponent
 */
TInt RSsmSystemWideProperty::Connect(TUint aSwpKey, TUid aProcessSid)
	{
	iKey = aSwpKey;
	return iProperty.Attach(aProcessSid, iKey, EOwnerThread);
	} //lint !e1746 Suppress parameter 'aProcessSid' could be made const reference

/**
 Close this handle
 */
EXPORT_C void RSsmSystemWideProperty::Close()
	{
	if(KNullHandle != iProperty.Handle())
		{
		iProperty.Cancel();
		iProperty.Close();
		}
	}

/**
 Reads the value of the connected System-Wide-Property.
 @param aValue A reference to the variable to store the read value into.
 @return KErrNone if successful, KErrBadHandle if not connected, otherwise one of the system wide error codes.
 */
EXPORT_C TInt RSsmSystemWideProperty::GetValue(TInt& aValue) const
	{
	//This function is implemented as "return by reference" because it can't handle
	//any error returned from RProperty in a sensible way.
	TInt err = KErrBadHandle;
	if(iProperty.Handle())
		{
		err = const_cast<RProperty&>(iProperty).Get(aValue);
#ifdef _DEBUG
		if(KErrNone != err)
			{
			DEBUGPRINT3A("RSsmSystemWideProperty::Value read property %d returned error %d", iKey, err);
			}
#endif
		}
	return err;
	}

/**
 Use to get a notification when the value of the connected System-Wide-Property changes
 @param aStatus The TRequestStatus to be completed when the value gets changed. Will complete
 				immediately with KErrBadHandle if no System-Wide-Property is connected.
 */
EXPORT_C void RSsmSystemWideProperty::Subscribe(TRequestStatus& aStatus)
	{
	if(iProperty.Handle())
		{
		iProperty.Subscribe(aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrBadHandle);
		}
	}

/**
 Cancel an outstanding @c Subscribe operation.
 */
EXPORT_C void RSsmSystemWideProperty::SubscribeCancel()
	{
	if(KNullHandle != iProperty.Handle())
		{
		iProperty.Cancel();
		}
	}

//
//---------------- class CSsmSystemWideProperty ------------------
//

/**
Factory method that returns an instance of this object setup to subscribe
on changes of the specifed System-Wide-Property. If the specified property
does not yet exist, then this operation will still succeed.

@param aSwpKey the key(a TUint value) of the System-Wide-Property to subscribe to
@return KErrNone if successful, otherwise one of the system wide error codes.
*/
EXPORT_C CSsmSystemWideProperty* CSsmSystemWideProperty::NewLC(TUint aSwpKey)
	{
	CSsmSystemWideProperty* self = new (ELeave) CSsmSystemWideProperty;
	CleanupStack::PushL(self);
	self->ConstructL(aSwpKey, KSsmServerSid);
	return self;
	} //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
Factory method that returns an instance of this object setup to subscribe
on changes of the specifed System-Wide-Property. If the specified property
does not yet exist, then this operation will still succeed.

@param aSwpKey the key(a TUint value) of the System-Wide-Property to subscribe to
@return KErrNone if successful, otherwise one of the system wide error codes.
*/
 EXPORT_C CSsmSystemWideProperty* CSsmSystemWideProperty::NewL(TUint aSwpKey)
 	{
 	CSsmSystemWideProperty* self = CSsmSystemWideProperty::NewLC(aSwpKey);
 	CleanupStack::Pop(self);
 	return self;
 	} //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
 @internalComponent
 */
CSsmSystemWideProperty* CSsmSystemWideProperty::NewLC(TUint aSwpKey, TUid aProcessSid)
	{
	CSsmSystemWideProperty* self = new (ELeave) CSsmSystemWideProperty;
	CleanupStack::PushL(self);
	self->ConstructL(aSwpKey, aProcessSid);
	return self;
	} //lint !e1746 Suppress parameter 'aProcessSid' could be made const reference

/**
 @internalComponent
 */
void CSsmSystemWideProperty::ConstructL(TUint aSwpKey, TUid aProcessSid)
	{
	iMonitor = CSsmPropertyMonitor::NewL(*this, aSwpKey, aProcessSid);
	} //lint !e1746 Suppress parameter 'aProcessSid' could be made const reference

/**
 @internalComponent
 */
CSsmSystemWideProperty::CSsmSystemWideProperty()
	{
	}

/**
 Destructor
 */
EXPORT_C CSsmSystemWideProperty::~CSsmSystemWideProperty()
	{
	iSubscribers.Close();
	delete 	iMonitor;
	}

/**
 Reads the value held by the connected System-Wide-Property.
 @param aSwp A reference to the variable where the read data will be written.
 @panic ECmnSwpSession1 if an error occurs in RunL
 @return KErrNone if successful, otherwise one of the system wide error codes.
 */
EXPORT_C TInt CSsmSystemWideProperty::GetValue(TSsmSwp& aSwp) const
	{
	__ASSERT_DEBUG(iMonitor, User::Panic(KPanicSsmCmn, ECmnSwpSession1));
	if(iMonitor)
		{
		return iMonitor->GetValue(aSwp);
		}
	else
		{
		return KErrGeneral;
		}
	}

/**
 Register for a callback when the monitored System-Wide-Property changes.
 @param aSubscriber The object to receive the callback
 */
EXPORT_C void CSsmSystemWideProperty::AddSubscriberL(MSwpChangeNotificationSubscriber& aSubscriber)
	{
	iSubscribers.AppendL(&aSubscriber);
	}

/**
 Cancel callback subscription from the monitored System-Wide-Property.
 @param aSubscriber The object for which to cancel subsription.
 */
EXPORT_C void CSsmSystemWideProperty::RemoveSubscriber(const MSwpChangeNotificationSubscriber& aSubscriber)
	{
	const TInt index = iSubscribers.Find(&aSubscriber);
	if(index > KErrNotFound)
		{
		iSubscribers.Remove(index);

		const TInt KDefaultPtrArrayGranularity = 8; // from /cedar/generic/base/e32/common/array.cpp
		const TInt count = iSubscribers.Count();
		if((count % KDefaultPtrArrayGranularity) == 0)
			{
			iSubscribers.Compress();
			}
		}
	}

/**
 Called from CSsmPropertyMonitor when the subsribed System-Wide-Property have changed.
 @internalComponent
 */
void CSsmSystemWideProperty::NotifySubscribers(TSsmSwp aSwp)
	{
	TInt count = iSubscribers.Count();
	while (count--)
		{
#ifdef _DEBUG
		TRAPD(err, iSubscribers[count]->SwpChanged(aSwp));
		if(KErrNone != err)
			{
			DEBUGPRINT2A("Illegal leave (leavecode: %d) detected. Will be ignored", err);
			}
#else
		TRAP_IGNORE(iSubscribers[count]->SwpChanged(aSwp));
#endif
		}
	} //lint !e1746 Suppress parameter 'aSwp' could be made const reference

