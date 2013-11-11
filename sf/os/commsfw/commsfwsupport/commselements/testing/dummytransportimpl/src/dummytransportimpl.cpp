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

/**
 @file
 @internalTechnology
*/

#include "dummytransportimpl.h"
#include <elements/metadata.h>
#include <elements/h_trbuf.h>
#include <elements/nm_address_internal.h>
#include <elements/nm_signals.h>
#include <elements/nm_log.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemTestingdmytr, "ElemTestingdmytr");
#endif

using namespace Meta;
using namespace Messages;
using namespace DummyTransport;

namespace DummyTransport
{

//
//CDummyTransportEntry
class CDummyTransportEntry : public CBase
	{
public:
	static CDummyTransportEntry* NewL(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage);
	const TRuntimeCtxId& PostedFrom();

	//This dummy transport can only carry messages addressed to a valid node!!!
	const TNodeId& PostedTo();
	TDes8& Message();
	~CDummyTransportEntry();

private:
	RBuf8 iPostedFromBuf;
	RBuf8 iPostedToBuf;

public: //for logging
	RBuf8 iMessageBuf;
	};

CDummyTransportEntry* CDummyTransportEntry::NewL(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage)
	{
	CDummyTransportEntry* self = new (ELeave) CDummyTransportEntry;
	CleanupStack::PushL(self);
	self->iPostedFromBuf.CreateL(__Align8(TRuntimeCtxId::KMaxInlineAddressSize));
	self->iPostedToBuf.CreateL(__Align8(TRuntimeCtxId::KMaxInlineAddressSize));
	self->iMessageBuf.CreateL(__Align8(TSignalBase::KMaxInlineMessageSize));
	User::LeaveIfError(aPostFrom.Store(self->iPostedFromBuf));
	User::LeaveIfError(aPostTo.Store(self->iPostedToBuf));
	User::LeaveIfError(aMessage.Store(self->iMessageBuf));
	CleanupStack::Pop(self);
	return self;
	}

const TRuntimeCtxId& CDummyTransportEntry::PostedFrom()
	{
	return *reinterpret_cast<const TRuntimeCtxId*>(iPostedFromBuf.Ptr());
	}

//This dummy transport can only carry messages addressed to a valid node!!!
const TNodeId& CDummyTransportEntry::PostedTo()
	{
	const TRuntimeCtxId& postedTo = *reinterpret_cast<const TRuntimeCtxId*>(iPostedToBuf.Ptr());
	return address_cast<TNodeId>(postedTo);
	}

TDes8& CDummyTransportEntry::Message()
	{
	return iMessageBuf;
	}

CDummyTransportEntry::~CDummyTransportEntry()
	{
	iPostedFromBuf.Close();
	iPostedToBuf.Close();
	iMessageBuf.Close();
	}

} //namespace DummyTransport

//
//CDummyTransportImpl
EXPORT_C CDummyTransportImpl* CDummyTransportImpl::NewL(const CMetaDataVirtualCtorInPlace& aVirtCtor)
	{
	CDummyTransportImpl* self = new (ELeave) CDummyTransportImpl(aVirtCtor);
	return self;
	}

EXPORT_C CDummyTransportImpl::~CDummyTransportImpl()
	{
	Cancel();
	__ASSERT_DEBUG(iCurrentEntry==NULL, User::Panic(KSpecAssert_ElemTestingdmytr, 1));
	__ASSERT_DEBUG(iEntries.Count()==0, User::Panic(KSpecAssert_ElemTestingdmytr, 2));
	iEntries.Close();
	}

//From MTransportSender
EXPORT_C void CDummyTransportImpl::PostMessage(const Messages::TRuntimeCtxId& aPostFrom, const Messages::TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage)
	{
	//This implementation does not need to handle errors - panic immediatelly on first error!
	CDummyTransportEntry* entry;
	__ASSERT_ALWAYS_NO_LEAVE(entry = CDummyTransportEntry::NewL(aPostFrom, aPostTo, aMessage));
	__ASSERT_ALWAYS_NO_LEAVE(iEntries.AppendL(entry));

#ifdef SYMBIAN_TRACE_ENABLE
    _LIT8(KLogTransportSubTag, "cftransport");
    NM_LOG_START_BLOCK(KLogTransportSubTag, _L8("TransportSender::PostMessage"));
        NM_LOG_ADDRESS(KLogTransportSubTag, aPostFrom);
        NM_LOG_ADDRESS(KLogTransportSubTag, aPostTo);
        NM_LOG_MESSAGE(KLogTransportSubTag, entry->iMessageBuf);
    NM_LOG_END_BLOCK(KLogTransportSubTag, _L8("TransportSender::PostMessage"));
#endif

	if (!IsActive())
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

//From MTransportReceiver
EXPORT_C void CDummyTransportImpl::RegisterAddress(TRuntimeCtxId& aCookie)
	{
	TNodeId& nodeId = address_cast<TNodeId>(aCookie); //This transport only registers TNodeId
	__ASSERT_ALWAYS(nodeId.Ptr(), User::Invariant());
	__ASSERT_ALWAYS(nodeId.IsNull(), User::Invariant());
	++iRegisteredCount;

	// code below relies upon one byte of salt - would be nice to __COMPILE_ASSERT()
	iPointerSalt = ((iPointerSalt + 1) & 0xFF);
	TRuntimeCtxIdOp op(nodeId);
	op.Set(iPointerSalt, 0);
	}

EXPORT_C void CDummyTransportImpl::DeregisterAddress(TRuntimeCtxId& aCookie)
	{
	--iRegisteredCount;
	__ASSERT_ALWAYS(iRegisteredCount >= 0, User::Invariant());
	aCookie.SetNull();
	}

EXPORT_C TInt CDummyTransportImpl::RegisteredCount() const
	{
	return iRegisteredCount;
	}

EXPORT_C TInt CDummyTransportImpl::VerifyDestination(const TRuntimeCtxId& /*aDestination*/)
	{
	return KErrNone;
	}

CDummyTransportImpl::CDummyTransportImpl(const CMetaDataVirtualCtorInPlace& aVirtCtor)
:	CActive(CActive::EPriorityLow),
	iVirtCtor(aVirtCtor)
	{
	CActiveScheduler::Add(this);
	}

// CActive
void CDummyTransportImpl::RunL()
	{
	//This implementation does not need to handle errors - panic immediatelly on first error!
	User::LeaveIfError(iStatus.Int());
	CDummyTransportEntry* entry = iEntries[0];
	__ASSERT_ALWAYS(iEntries.Count() > 0 && entry, User::Invariant());

	TBuf8<TSignalBase::KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead> msgBuf;
	TPtrC8 params(entry->Message());
	TSignalBase* msg = static_cast<TSignalBase*>(iVirtCtor.New(params, msgBuf));	// A panic stemming from here probably means that the concrete T-message class
																					// has too many unserialised members (or that the serialised form is markedly
																					// more compressed. Consider removing members or sending a Blob instead

	__ASSERT_ALWAYS(msg != NULL, User::Invariant());	// No virtual constructor found. Likely causes:
														//	(1) Forgot to register it through the CMetaDataVirtualCtorInPlace used when constructing the transport
														//	(2) T-class being dispatched isn't in the virtual ctor table at all
														//	(3) Ctor table deregistered prematurely

	__ASSERT_ALWAYS(VerifyDestination(entry->PostedTo()) == KErrNone, User::Invariant());

	iCurrentEntry = entry; //From now on only the DispatchL can leave
	iEntries.Remove(0);


#ifdef SYMBIAN_TRACE_ENABLE
            _LIT8(KLogTransportSubTag, "cftransport");
    	    NM_LOG_START_BLOCK(KLogTransportSubTag, _L8("TransportReceiver::DispatchMessage"));
            NM_LOG_ADDRESS(KLogTransportSubTag, iCurrentEntry->PostedFrom());
            NM_LOG_ADDRESS(KLogTransportSubTag, entry->PostedTo());
            NM_LOG_MESSAGE(KLogTransportSubTag, entry->Message());
    	    NM_LOG_END_BLOCK(KLogTransportSubTag, _L8("TransportReceiver::DispatchMessage"));
#endif

	msg->DispatchL(iCurrentEntry->PostedFrom(), entry->PostedTo());

	delete iCurrentEntry;
	iCurrentEntry = NULL;

	if (!IsActive() && iEntries.Count() > 0)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

TInt CDummyTransportImpl::RunError(TInt aError)
	{
	//This implementation does not need to handle errors - panic immediatelly on first error!
	if (iCurrentEntry==NULL)
		{
		delete iCurrentEntry;
		iCurrentEntry = NULL;
		User::Invariant(); //optional here :))
		return aError;
		}

	//DispatchL left
	TBuf8<TSignalBase::KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead> msgBuf;
	TPtrC8 params(iCurrentEntry->Message());
	TSignalBase* msg = static_cast<TSignalBase*>(iVirtCtor.New(params, msgBuf));
	__ASSERT_ALWAYS(msg != NULL, User::Invariant());
	msg->Error(iCurrentEntry->PostedFrom(), iCurrentEntry->PostedTo(), aError);
	delete iCurrentEntry;
	iCurrentEntry = NULL;
	return KErrNone;
	}

void CDummyTransportImpl::DoCancel()
	{
	iEntries.ResetAndDestroy();
	}







