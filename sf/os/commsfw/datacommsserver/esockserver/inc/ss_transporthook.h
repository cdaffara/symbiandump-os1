/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
* 
*
*/



/**
 @file ss_transporthook.h
 @internalTechnology
*/

#ifndef SYMBIAN_TRANSPORTHOOK_H
#define SYMBIAN_TRANSPORTHOOK_H

#include <comms-infras/ss_nodemessages.h>

//TODO[PROD] - reimplement hooks?
#ifdef HOOKS_IMPLEMENTED
#include <cftransporthook.h>
#endif

namespace ESock
{

//TODO[PROD] - reimplement hooks?
#ifdef HOOKS_IMPLEMENTED
class TCFRegisterMessageHook : public CommsFW::TCFRegisterHookSignal
    {
public:
    IMPORT_C TCFRegisterMessageHook(const Messages::TNodeId& aSenderId, TUint aActivityId, const Messages::TNodeSignal::TMessageId& aMessageId);

	EXPORT_DATA_VTABLE_AND_FN
	DECLARE_MVIP_CTR(TCFRegisterMessageHook)

protected:
    TCFRegisterMessageHook(){};
	IMPORT_C virtual CommsFW::CCFHookEntry* NewHookEntryL() const;
	IMPORT_C virtual void Error(TAny* aItf, TInt aError);

protected:
    Messages::TNodeSignal::TMessageId iMessageId; //of the message to listen for
	TUint iActivityId; //to address the right activity on error
    };

class TCFDeregisterMessageHook : public CommsFW::TCFDeregisterHookSignal
    {
    friend class TTransportHookError;
public:
    IMPORT_C TCFDeregisterMessageHook(const Messages::TNodeId& aSenderId, const Messages::TNodeSignal::TMessageId& aMessageId);

	EXPORT_DATA_VTABLE_AND_FN
	DECLARE_MVIP_CTR(TCFDeregisterMessageHook)

protected:
    TCFDeregisterMessageHook(){};
    void DeregisterLocalTag(CommsFW::MTransportReceiver& aTransport){LocalTag(aTransport);}


protected:
    Messages::TNodeSignal::TMessageId iMessageId;
    };

//class MTransportSender;
class TTransportHookError : public Messages::TSignatureBase
    {
public:
	EXPORT_DATA_VTABLE_AND_FN
	DECLARE_MVIP_CTR(TTransportHookError)

protected:
    TTransportHookError(){};
    explicit TTransportHookError(const Messages::TNodeSignal::TMessageId& aMessageId, const Messages::TNodeId& aSenderId, TUint aActivityId, const Messages::TNodeSignal::TMessageId& aHookedMessageId, TInt aError):
        Messages::TSignatureBase(aMessageId, aSenderId, aActivityId),
        iError(aError),
        iHookedMessageId(aHookedMessageId)
        {
        }

	virtual TInt VerifyDestination(const CommsFW::TCookie& aDestination, CommsFW::MTransportReceiver& aTransport);

protected:
	TInt iError;
	Messages::TNodeSignal::TMessageId iHookedMessageId;
    };

template<TInt id, TInt32 realm>
struct TCFMessageTransportHookError : public TTransportHookError, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
    enum { iId = id };
	explicit TCFMessageTransportHookError(const Messages::TNodeId& aSenderId, TUint aActivityId, const Messages::TNodeSignal::TMessageId& aMessageId, TInt aError ) :
	    TTransportHookError(Messages::TNodeSignal::TMessageId(id, realm), aSenderId, aActivityId, aMessageId, aError){}
    };

//this class shouldn't be here if we export the file
class CCFMessageHooks : public CommsFW::CCFTransportHooks
    {
public:
    CCFMessageHooks() :
        CCFTransportHooks()
        {
        }

    virtual void WalkHooks( const CommsFW::TCFSignalBase& aMessage, const CommsFW::TCookie& aRecipient );
    virtual void RegisterL( const CommsFW::TCFRegisterHookSignal& aMessage );
    virtual void Deregister( const CommsFW::TCFDeregisterHookSignal& aMessage );
    };

class CCFMessageHookEntry : public CommsFW::CCFHookEntry
    {
public:
    IMPORT_C CCFMessageHookEntry( const Messages::TNodeSignal::TMessageId& aMessageId, TInt aActivityId, const Messages::TNodeId& aNode, const Messages::TNodeId& aListener );
    IMPORT_C virtual TBool Match( const CommsFW::TCFSignalBase& aMessage, const CommsFW::TCookie& aRecipient );
    IMPORT_C virtual void Notify( const CommsFW::TCFSignalBase& aMessage ) const;

protected:
    IMPORT_C virtual TBool Match( const CommsFW::TCFDeregisterHookSignal& aReq ) const;

protected:
    Messages::TNodeSignal::TMessageId iMessageId;
    TInt iActivityId;
    };

#endif

} //namespace ESock

#endif //SYMBIAN_TRANSPORTHOOK_H

