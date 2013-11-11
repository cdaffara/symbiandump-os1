// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ss_transporthook.CPP
*/

#include "ss_transporthook.h"
#include <comms-infras/metatype.h>
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_nodeinterfaces.h>


using namespace ESock;
using namespace Meta;
using namespace CommsFW;
using namespace Messages;


//TODO[PROD] - reimplement hooks?
#if 0

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockstrnsp, "ESockSSockstrnsp");
#endif

//class TCFRegisterMessageHook : public TCFRegisterHookSignal
EXPORT_C TCFRegisterMessageHook::TCFRegisterMessageHook(const TNodeId& aSenderId, TUint aActivityId, const TNodeSignal::TMessageId& aMessageId):
    TCFRegisterHookSignal( Messages::TSignatureBase::TypeId() ),
    iMessageId(aMessageId),
    iActivityId(aActivityId)
    {
    SetClientId(aSenderId());
    }

EXPORT_C CCFHookEntry* TCFRegisterMessageHook::NewHookEntryL() const
    {
    return new(ELeave)CCFMessageHookEntry(iMessageId,iActivityId,TNodeId(iNode),TNodeId(ClientId()));
    }

EXPORT_C void TCFRegisterMessageHook::Error(TAny* aItf, TInt aError)
    {
    Messages::ANode* itf = static_cast<Messages::ANode*>(aItf);
    TNodeId itfid = itf ? itf->NodeId() : TNodeId::NullCommsId();
    TCFMessage::TRegisterHookError(itfid,iActivityId,iMessageId,aError).PostTo(TNodeId(ClientId()));
    }

//class TCFDeregisterMessageHook : public TCFDeregisterHookSignal
EXPORT_C TCFDeregisterMessageHook::TCFDeregisterMessageHook(const TNodeId& aSenderId, const TNodeSignal::TMessageId& aMessageId):
    TCFDeregisterHookSignal(Messages::TSignatureBase::TypeId()),
    iMessageId(aMessageId)
    {
    SetClientId(aSenderId());
    }

//class TTransportHookError : public Messages::TSignatureBase
TInt TTransportHookError::VerifyDestination(const CommsFW::TCookie& aDestination, CommsFW::MTransportReceiver& aTransport)
    {
    //deregister local tag if any
    TCFDeregisterMessageHook dereg(static_cast<const TNodeId&>(TNodeId(aDestination)),iHookedMessageId);
    dereg.DeregisterLocalTag(aTransport);
    return Messages::TSignatureBase::VerifyDestination(aDestination, aTransport);
    }

//class CCFMessageHooks : public CCFTransportHooks
void CCFMessageHooks::WalkHooks( const TCFSignalBase& aMessage, const TCookie& aRecipient )
    {//place to optimise wlak before calling the base class
    CCFTransportHooks::WalkHooks( aMessage, aRecipient );
    }

void CCFMessageHooks::RegisterL( const CommsFW::TCFRegisterHookSignal& aMessage )
    {//place to do some extra stuff for look-up optimisation
    CCFTransportHooks::RegisterL( aMessage );
    }

void CCFMessageHooks::Deregister( const CommsFW::TCFDeregisterHookSignal& aMessage )
    {//place to undo some extra stuff done for look-up optimisation
    CCFTransportHooks::Deregister( aMessage );
    }

//class CCFMessageHookEntry : public CCFHookEntry
EXPORT_C CCFMessageHookEntry::CCFMessageHookEntry( const TNodeSignal::TMessageId& aMessageId, TInt aActivityId, const TNodeId& aNode, const TNodeId& aListener ) :
    CCFHookEntry(Messages::TSignatureBase::TypeId(),aNode(),aListener()),
    iMessageId(aMessageId),
    iActivityId(aActivityId)
    {
	LOG(ESockLog::Printf(KESockMessageHook, _L("CCFMessageHookEntry [this=%08x]:\t Registered [aMessagedId=%d] [aActivityId=%d] [aNode=%08x] [aListener=%08x]"),
	    this, aMessageId.MessageId(), aActivityId, aNode.Printable(), aListener.Printable()));
    }

EXPORT_C TBool CCFMessageHookEntry::Match(const CommsFW::TCFSignalBase& aMessage, const TCookie& aRecipient)
    {
    return CCFHookEntry::Match( aMessage, aRecipient ) && static_cast<const Messages::TSignatureBase&>(aMessage).MessageId() == iMessageId;
    }

EXPORT_C void CCFMessageHookEntry::Notify(const CommsFW::TCFSignalBase& aMessage) const
    {
    __ASSERT_DEBUG(aMessage.IsTypeOf(TNodeSignal::TypeId()), User::Panic(KSpecAssert_ESockSSockstrnsp, 1));
	TNodeSignal& sig = static_cast<TNodeSignal&>(const_cast<CommsFW::TCFSignalBase&>(aMessage));
	TInt activityId = sig.ActivityId();
	sig.SetActivity(iActivityId);
	CCFHookEntry::Notify(sig);
	sig.SetActivity(activityId);
    }

EXPORT_C TBool CCFMessageHookEntry::Match( const TCFDeregisterHookSignal& aReq ) const
    {
    return CCFHookEntry::Match( aReq );
    }

#endif


