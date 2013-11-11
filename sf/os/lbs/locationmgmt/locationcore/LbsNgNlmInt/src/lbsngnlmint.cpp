/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "lbsngnlmint.h"
#include "lbsnetinternalmsgtypes.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include "lbsngnlmintcommon.h"

//
// Network Location Manager <-> Network Gateway Message Types
//

//
// Base Class
//

EXPORT_C TLbsNetLocMsgBase::TLbsNetLocMsgType TLbsNetLocMsgBase::Type() const
	{
	return iType;
	}
	
EXPORT_C TInt32 TLbsNetLocMsgBase::Size() const
	{
	return iSize;
	}
	
EXPORT_C const TLbsNetSessionIdInt& TLbsNetLocMsgBase::SessionId() const
	{
	return iSessionId;
	}
	
EXPORT_C TLbsNetLocMsgBase& TLbsNetLocMsgBase::operator= (const TLbsNetLocMsgBase& aSrc)
	{
	__ASSERT_ALWAYS(Type() == aSrc.Type(), User::Panic(_L("LbsNetInternal"), 4));
	Mem::Copy(this, &aSrc, aSrc.Size());
	return *this;	
	}

EXPORT_C TLbsNetLocMsgBase::TLbsNetLocMsgBase() :
	iType(ENetLocMsgUnknown),
	iSize(sizeof(TLbsNetLocMsgBase))
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaxLbsNetLocMsgSize, User::Panic(_L("LbsNetInternal"), 3));
	}

EXPORT_C TLbsNetLocMsgBase::TLbsNetLocMsgBase(const TLbsNetLocMsgBase& aSrc)
	{
	*this = aSrc;
	}

TLbsNetLocMsgBase::TLbsNetLocMsgBase(TLbsNetLocMsgType aType, 
									 TInt32 aSize,
									 const TLbsNetSessionIdInt& aSessionId) :
	iType(aType),
	iSize(aSize),
	iSessionId(aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaxLbsNetLocMsgSize, User::Panic(_L("LbsNetInternal"), 3));
	}

//
// Network Location Request Message
//

EXPORT_C TLbsNetLocNetworkLocationRequestMsg::TLbsNetLocNetworkLocationRequestMsg(
		const TLbsNetSessionIdInt& aSessionId,
		TBool aNewClient,
		TLbsNetPosRequestQualityInt aQuality) :
	TLbsNetLocMsgBase(ENetLocMsgNetworkLocationRequest, 
					  sizeof(TLbsNetLocNetworkLocationRequestMsg),
					  aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetLocNetworkLocationRequestMsg) <= KMaximumMessageSizeBytes);

	iNewClient = aNewClient;
	iQuality = aQuality;
	}

EXPORT_C TBool TLbsNetLocNetworkLocationRequestMsg::NewClient() const
	{
	return iNewClient;
	}
	
EXPORT_C const TLbsNetPosRequestQualityInt& TLbsNetLocNetworkLocationRequestMsg::Quality() const
	{
	return iQuality;
	}
	
//
// Network Location Cancel Message
//

EXPORT_C TLbsNetLocNetworkLocationCancelMsg::TLbsNetLocNetworkLocationCancelMsg(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason) :
	TLbsNetLocMsgBase(ENetLocMsgNetworkLocationCancel, 
					  sizeof(TLbsNetLocNetworkLocationCancelMsg),
					  aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetLocNetworkLocationCancelMsg) <= KMaximumMessageSizeBytes);

	iReason = aReason;	
	}

EXPORT_C TInt TLbsNetLocNetworkLocationCancelMsg::Reason() const
	{
	return iReason;
	}
	
//
// Network Location Complete Message
//

EXPORT_C TLbsNetLocNetworkLocationCompleteMsg::TLbsNetLocNetworkLocationCompleteMsg(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason) :
	TLbsNetLocMsgBase(ENetLocMsgNetworkLocationComplete, 
					  sizeof(TLbsNetLocNetworkLocationCompleteMsg),
					  aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetLocNetworkLocationCompleteMsg) <= KMaximumMessageSizeBytes);

	iReason = aReason;
	}

EXPORT_C TInt TLbsNetLocNetworkLocationCompleteMsg::Reason() const
	{
	return iReason;
	}

//
// Network Location Manager -> Network Gateway Interface
//

EXPORT_C void RLbsNetworkLocationRequests::InitializeL()
	{
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
	
	TInt err = RProperty::Define(KNetworkLocationRequestMessageKey, 
								 RProperty::EByteArray, 
								 readPolicy,
								 writePolicy,
								 KMaxLbsNetLocMsgSize);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsNgNlmIntCommon::SetCategoryUidL(KNetworkLocationRequestCategoryKey, 
									   RProcess().SecureId());	
	}
	
EXPORT_C void RLbsNetworkLocationRequests::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KNetworkLocationRequestMessageKey));
	}

EXPORT_C RLbsNetworkLocationRequests::RLbsNetworkLocationRequests()
	{
	}
	
EXPORT_C void RLbsNetworkLocationRequests::OpenL(TUid /*aModuleId*/)
	{
	__ASSERT_DEBUG(iNetLocRequestProperty.Handle() == NULL, User::Invariant());
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsNgNlmIntCommon::GetCategoryUidL(KNetworkLocationRequestCategoryKey, 
									   categoryUid);	
	
	User::LeaveIfError(iNetLocRequestProperty.Attach(categoryUid, KNetworkLocationRequestMessageKey));
	}
	
EXPORT_C void RLbsNetworkLocationRequests::Close()
	{
	CancelNotifyNetworkLocationRequestUpdate();
	iNetLocRequestProperty.Close();
	}

EXPORT_C TInt RLbsNetworkLocationRequests::GetNetworkLocationRequest(TLbsNetLocMsgBase& aMessage)
	{
	__ASSERT_DEBUG(iNetLocRequestProperty.Handle() != NULL, User::Invariant());
	TPtr8 ptr(reinterpret_cast<TUint8*>(&aMessage), aMessage.Size());
	return (iNetLocRequestProperty.Get(ptr));
	}

EXPORT_C TInt RLbsNetworkLocationRequests::SetNetworkLocationRequest(const TLbsNetLocMsgBase& aMessage)
	{
	__ASSERT_DEBUG(iNetLocRequestProperty.Handle() != NULL, User::Invariant());
	TPtrC8 ptr(reinterpret_cast<const TUint8*>(&aMessage), aMessage.Size());
	return (iNetLocRequestProperty.Set(ptr));
	}

EXPORT_C void RLbsNetworkLocationRequests::NotifyNetworkLocationRequestUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iNetLocRequestProperty.Handle() != NULL, User::Invariant());
	iNetLocRequestProperty.Subscribe(aStatus);
	}

EXPORT_C void RLbsNetworkLocationRequests::CancelNotifyNetworkLocationRequestUpdate()
	{
	__ASSERT_DEBUG(iNetLocRequestProperty.Handle() != NULL, User::Invariant());
	iNetLocRequestProperty.Cancel();
	}

//
// Network Gateway -> Network Location Manager Interface
//

EXPORT_C void RLbsNetworkLocationResponses::InitializeL()
	{
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
	
	TInt err = RProperty::Define(KNetworkLocationResponseMessageKey, 
								 RProperty::EByteArray, 
								 readPolicy,
								 writePolicy,
								 KMaxLbsNetLocMsgSize);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsNgNlmIntCommon::SetCategoryUidL(KNetworkLocationResponseCategoryKey, 
									   RProcess().SecureId());	
	}
	
EXPORT_C void RLbsNetworkLocationResponses::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KNetworkLocationResponseMessageKey));	
	}

EXPORT_C RLbsNetworkLocationResponses::RLbsNetworkLocationResponses()
	{
	}
	
EXPORT_C void RLbsNetworkLocationResponses::OpenL(TUid /*aModuleId*/)
	{
	__ASSERT_DEBUG(iNetLocResponseProperty.Handle() == NULL, User::Invariant());
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsNgNlmIntCommon::GetCategoryUidL(KNetworkLocationResponseCategoryKey, 
									   categoryUid);	
	
	User::LeaveIfError(iNetLocResponseProperty.Attach(categoryUid, KNetworkLocationResponseMessageKey));	
	}
	
EXPORT_C void RLbsNetworkLocationResponses::Close()
	{
	CancelNotifyNetworkLocationResponseUpdate();
	iNetLocResponseProperty.Close();
	}
	
EXPORT_C TInt RLbsNetworkLocationResponses::GetNetworkLocationResponse(TLbsNetLocMsgBase& aMessage)
	{
	__ASSERT_DEBUG(iNetLocResponseProperty.Handle() != NULL, User::Invariant());
	TPtr8 ptr(reinterpret_cast<TUint8*>(&aMessage), aMessage.Size());
	return (iNetLocResponseProperty.Get(ptr));	}

EXPORT_C TInt RLbsNetworkLocationResponses::SetNetworkLocationResponse(const TLbsNetLocMsgBase& aMessage)
	{
	__ASSERT_DEBUG(iNetLocResponseProperty.Handle() != NULL, User::Invariant());
	TPtrC8 ptr(reinterpret_cast<const TUint8*>(&aMessage), aMessage.Size());
	return (iNetLocResponseProperty.Set(ptr));
	}
	
EXPORT_C void RLbsNetworkLocationResponses::NotifyNetworkLocationResponseUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iNetLocResponseProperty.Handle() != NULL, User::Invariant());
	iNetLocResponseProperty.Subscribe(aStatus);	
	}

EXPORT_C void RLbsNetworkLocationResponses::CancelNotifyNetworkLocationResponseUpdate()
	{
	__ASSERT_DEBUG(iNetLocResponseProperty.Handle() != NULL, User::Invariant());
	iNetLocResponseProperty.Cancel();	
	}
