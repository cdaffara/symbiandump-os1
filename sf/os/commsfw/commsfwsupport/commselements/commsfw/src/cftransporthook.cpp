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
 @file
 @internalComponent
*/

#include "../inc/cfhookinternals.h"
#include "../inc/cftransportmsg.h"


#ifdef HOOKS_IMPLEMENTED

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemComsFwCfTrnH, "ElemComsFwCfTrnH");
#endif

using namespace CommsFW;
using namespace Meta;
//**********************************************************************************
//CCFHookEntry
EXPORT_C CCFHookEntry::CCFHookEntry( const Meta::STypeId& aTypeId, const TCookie& aNode, const TCookie& aListener ) :
    iTypeId(aTypeId),
    iNode(aNode),
    iListener(aListener)
    {
    __ASSERT_DEBUG(iListener.iType == TCookie::EDispatchItfPtr, User::Panic(KSpecAssert_ElemComsFwCfTrnH, 1));
    iListener.iType = TCookie::EDispatchItfPtrListener;
    }

EXPORT_C TBool CCFHookEntry::Match( const TCFSignalBase& aMessage, const TCookie& aRecipient )
    {
    return aMessage.IsTypeOf(iTypeId) &&
        TCookieAccessor(iNode).IsNull() || aRecipient == iNode;
    }

EXPORT_C void CCFHookEntry::Notify( const TCFSignalBase& aMessage ) const
    {
    __ASSERT_DEBUG(iListener.iType == TCookie::EDispatchItfPtrListener, User::Panic(KSpecAssert_ElemComsFwCfTrnH, 2));
    RCFInterfaceBase::PostMessage(iListener, aMessage);
    }

EXPORT_C TBool CCFHookEntry::Match( const TCFDeregisterHookSignal& aReq ) const
    {
    TCookie temp = iListener;
    temp.iType = TCookie::EDispatchItfPtr;
    return iNode == aReq.iNode && iTypeId == aReq.iTypeId && temp == aReq.ClientId();
    }

//**********************************************************************************
//TCFHookSignal & co
#pragma message("@TODO-PS get uid sorted out")
EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TCFHookSignal, 0, 0 )
	REGISTER_ATTRIBUTE( TCFHookSignal, iTypeId, TMeta<STypeId> )
END_ATTRIBUTE_TABLE_BASE(TCFClientSignalBase,0)

EXPORT_C TCFHookSignal::TCFHookSignal(const STypeId& aTypeId) :
    iTypeId(aTypeId),
    iNode(),
    iTransportReceiver(NULL)
    {
    }

EXPORT_C TInt TCFHookSignal::VerifyDestination(const TCookie& aDestination, MTransportReceiver& aTransport)
    {
    iNode = aDestination;
    iTransportReceiver = &aTransport;
    return TCFClientSignalBase::VerifyDestination(aDestination, aTransport);
    }

EXPORT_C void TCFRegisterHookSignal::DispatchL(TAny* /*aItf*/, TPtrC8& /*aParams*/)
    {
    __ASSERT_DEBUG(iTransportReceiver, User::Panic(KSpecAssert_ElemComsFwCfTrnH, 3));
    CCFTransportHooks& transportHooks = *iTransportReceiver->TransportHooksWalker();
    transportHooks.RegisterL(*this);
    }

EXPORT_C TCFRegisterHookSignal::TCFRegisterHookSignal(const Meta::STypeId& aTypeId) :
    TCFHookSignal(aTypeId)
    {
    }

EXPORT_C CCFHookEntry* TCFRegisterHookSignal::NewHookEntryL() const
    {
    return new(ELeave)CCFHookEntry(iTypeId,iNode,ClientId());
    }

EXPORT_C TInt TCFRegisterHookSignal::LocalTag(const MTransportReceiver& aTransport) const
    {
    return aTransport.TransportHooksWalker()->TransportHooks().RegisterSpy(ClientId());
    }

EXPORT_C TCFDeregisterHookSignal::TCFDeregisterHookSignal(const Meta::STypeId& aTypeId) :
    TCFHookSignal(aTypeId)
    {
    }

EXPORT_C void TCFDeregisterHookSignal::DispatchL(TAny* /*aItf*/, TPtrC8& /*aParams*/)
    {
    __ASSERT_DEBUG(iTransportReceiver, User::Panic(KSpecAssert_ElemComsFwCfTrnH, 4));
    CCFTransportHooks& transportHooks = *iTransportReceiver->TransportHooksWalker();
    transportHooks.Deregister(*this);
    }

EXPORT_C void TCFDeregisterHookSignal::Error(TAny* /*aItf*/, TInt /*aError*/)
    {
    __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ElemComsFwCfTrnH, 5)); //should never come here
    }

EXPORT_C TInt TCFDeregisterHookSignal::LocalTag(const MTransportReceiver& aTransport) const
    {
    aTransport.TransportHooksWalker()->TransportHooks().DeregisterSpy(ClientId());
    return KErrNone;
    }


//**********************************************************************************
//TCFTransportHooks
void TCFTransportHooks::RegisterHookL( const TCFRegisterHookSignal& aReq )
    {
    if (!iHookEntries)
        {
        iHookEntries = new(ELeave)RPointerArray<CCFHookEntry>();
        }
    iHookEntries->AppendL(aReq.NewHookEntryL());
    }

void TCFTransportHooks::WalkHooks( const TCFSignalBase& aMessage, const TCookie& aRecipient)
    {
    if (iHookEntries)
        {
        for (TInt n = iHookEntries->Count() - 1; n>=0; n--)
            {
            if ((*iHookEntries)[n]->Match(aMessage,aRecipient))
                {
                (*iHookEntries)[n]->Notify(aMessage);
                }
            }
        }
    }

void TCFTransportHooks::DeregisterHook( const TCFDeregisterHookSignal& aReq )
    {
    if (iHookEntries)
        {
        for (TInt i = iHookEntries->Count() - 1; i>=0; i--)
            {
            if ((*iHookEntries)[i]->Match(aReq))
                {
                CCFHookEntry* entry = (*iHookEntries)[i];
                delete entry;
                iHookEntries->Remove(i);
                }
            }
        }
    }

TInt TCFTransportHooks::RegisterSpy( const TCookie& aSpy )
    {
    TCookieReg spy(aSpy);
    TInt n = KErrNone;
    TInt i;
    if ((i = iSpies.Find(spy)) == KErrNotFound)
        {
        n = iSpies.Append(spy);
        }
    else
        {
        iSpies[i].AddRef();
        }
    return n;
    }

void TCFTransportHooks::DeregisterSpy( const TCookie& aSpy )
    {
    TInt n;
    if ((n = iSpies.Find(TCookieReg(aSpy))) != KErrNotFound)
        {
        if (iSpies[n].ReleaseRef()==0)
            {
            iSpies.Remove(n);
            }
        }
    }

TCFTransportHooks::~TCFTransportHooks()
    {
    if (iHookEntries)
        {
        iHookEntries->ResetAndDestroy();
        }
    delete iHookEntries; //should this call ResetAndDestroy()?
    iSpies.Close();
    }

EXPORT_C CCFTransportHooks::CCFTransportHooks() :
    iTransportHooks(NULL)
    {
    }

EXPORT_C void CCFTransportHooks::WalkHooks( const TCFSignalBase& aMessage, const TCookie& aRecipient )
    {
    iTransportHooks->WalkHooks(aMessage,aRecipient);
    }

EXPORT_C void CCFTransportHooks::RegisterL( const TCFRegisterHookSignal& aMessage )
    {
    iTransportHooks->RegisterHookL(aMessage);
    }

EXPORT_C void CCFTransportHooks::Deregister( const TCFDeregisterHookSignal& aMessage )
    {
    iTransportHooks->DeregisterHook(aMessage);
    }


EXPORT_C void RCFHookInterfaceBase::PostHookMessage(const TCFHookSignal& aMessage)
    {
    TInt n;
    if ((n = aMessage.LocalTag(iTransport)) != KErrNone)
        {
        const_cast<TCFHookSignal&>(aMessage).Error(NULL,n);
        }
    else
        {
        PostMessage(aMessage);
        }
    }

EXPORT_C RCFHookInterfaceBase::RCFHookInterfaceBase(const MTransportReceiver& aTransport)
:	iTransport(aTransport)
	{
	}

#endif //HOOKS_IMPLEMENTED

