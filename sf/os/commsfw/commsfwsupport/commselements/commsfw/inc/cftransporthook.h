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

#ifndef __CFTRANSPORTHOOK_H__
#define __CFTRANSPORTHOOK_H__

#include <cftransport.h>
#include <elements/metadata.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemCommsFwcftrn, "ElemCommsFwcftrn");
#endif

#ifdef HOOKS_IMPLEMENTED

namespace CommsFW
{

class TCFDeregisterHookSignal;
//hooks listen for a message of given parameters and given sender/receiver.
//hooks are being walked when a message is beeing posted since sender is registered by
//transport receiver in its own thread
//When the corrent message detected is forwarded to the listener
//Hooks snoop on mesasages BEFORE the message has been dispatched (ok for snooping
//on send not so good for snooping on receive=>snooping on receive should be avoided since it doesn't
//actually say anthing about whoever is about to receive the message
class CCFHookEntry : public CBase
    {
    //friend class TCFDeregisterHookSignal;
    friend class TCFTransportHooks;

public:
    IMPORT_C CCFHookEntry( const Meta::STypeId& aTypeId, const TCookie& aNode, const TCookie& aListener );

    IMPORT_C virtual TBool Match( const TCFSignalBase& aMessage, const TCookie& aRecipient );
    IMPORT_C virtual void Notify( const TCFSignalBase& aMessage ) const;

protected:
    IMPORT_C virtual TBool Match( const TCFDeregisterHookSignal& aReq ) const;

protected:
    Meta::STypeId iTypeId; //of the message to listen for
    TCookie iNode; //of the node listener is interested in or NULL for any node
    TCookie iListener; //of the listener
    };

//TCFHookSignal derived message is sent to a node of interest. E.g. a node the hook wants
//to listen for sent/received messages
class TCFHookSignal : protected TCFClientSignalBase //protected derivation so that conversion
                                                    //to TCFClientSignalBase is not possible
    {
    friend class TCFTransportHooks;
    friend class RCFHookInterfaceBase; //so that it can convert it to TCFClientSignalBase

public:
    IMPORT_C TCFHookSignal(const Meta::STypeId& aTypeId);
	EXPORT_DATA_VTABLE_AND_FN

protected:
    TCFHookSignal(){}
    IMPORT_C virtual TInt VerifyDestination(const TCookie& aDestination, MTransportReceiver& aTransport);
    virtual TInt LocalTag(const MTransportReceiver& aTransport) const = 0;

protected:
    Meta::STypeId iTypeId; //of the message to listen for
    TCookie iNode; //of the message sender if NULL then any sender is a valid one
                     //not serialised but initialised in VerifyDestination
    MTransportReceiver* iTransportReceiver;
    };

//TCFRegisterHookSignal derived messages MUST be sent to a client whose message(s)
//one wshes to monitor
class TCFRegisterHookSignal : public TCFHookSignal
    {
    friend class TCFTransportHooks;
public:
    IMPORT_C TCFRegisterHookSignal(const Meta::STypeId& aTypeId);

protected:
    TCFRegisterHookSignal(){}

	IMPORT_C virtual void DispatchL(TAny* aItf, TPtrC8& aParams);
	IMPORT_C virtual CCFHookEntry* NewHookEntryL() const;
    IMPORT_C virtual TInt LocalTag(const MTransportReceiver& aTransport) const;
    };

//TCFDeregisterHookSignal derived messages MUST be sent to the same client as TCFRegisterHookSignal
class TCFDeregisterHookSignal : public TCFHookSignal
    {
    friend class CCFHookEntry;
public:
    IMPORT_C TCFDeregisterHookSignal(const Meta::STypeId& aTypeId);

protected:
    TCFDeregisterHookSignal(){}

	IMPORT_C virtual void DispatchL(TAny* aItf, TPtrC8& aParams);
	IMPORT_C virtual void Error(TAny* aItf, TInt aError);
    IMPORT_C virtual TInt LocalTag(const MTransportReceiver& aTransport) const;
    };

//class allows for customisable hook walking
class CCFTransportHooks : public CBase
    {
    friend class TCFSignalBase;
    friend class TCFRegisterHookSignal;
    friend class TCFDeregisterHookSignal;
    friend class CCommsTransport;

public:
    IMPORT_C CCFTransportHooks();

    IMPORT_C virtual void WalkHooks( const TCFSignalBase& aMessage, const TCookie& aRecipient );
    IMPORT_C virtual void RegisterL( const TCFRegisterHookSignal& aMessage );
    IMPORT_C virtual void Deregister( const TCFDeregisterHookSignal& aMessage );

protected:
    TCFTransportHooks& TransportHooks(){__ASSERT_DEBUG(iTransportHooks, User::Panic(KSpecAssert_ElemCommsFwcftrn, 1));return *iTransportHooks;}
    IMPORT_C void SetTransportHooks(CCommsTransport& aTransport);

protected:
    TCFTransportHooks* iTransportHooks;
    };

// Base for objects which can offer further interfaces
class RCFHookInterfaceBase : public RCFInterfaceBase
	{
public:
	//Only one explicit constructor by design
	IMPORT_C explicit RCFHookInterfaceBase(const MTransportReceiver& aTransport);
	IMPORT_C void PostHookMessage(const TCFHookSignal& aMessage);

protected:
	const MTransportReceiver& iTransport;
	};

} // namespace CommsFW

#endif //HOOKS_IMPLEMENTED

#endif // __CFTRANSPORTHOOK_H__

