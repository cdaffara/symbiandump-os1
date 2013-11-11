// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32debug.h>
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "lbsmessageenums.h"
#include "messageutils.h"
#include "lbsdevloggermacros.h"

/** 
constructor, call the CSecureSessionBase base class constructor

@internalTechnology
@released
 */	
CSecureSessionSubSessionBase::CSecureSessionSubSessionBase() : CSecureSessionBase()
	{
	LBSLOG(ELogP3, "CSecureSessionSubSessionBase::CSecureSessionSubSessionBase()");
	}

/** 
destructor, release the subsession handler container

@internalTechnology
@released
 */	
CSecureSessionSubSessionBase::~CSecureSessionSubSessionBase()
	{
	LBSLOG(ELogP3, "->CSecureSessionSubSessionBase::~CSecureSessionSubSessionBase()");

	if (iObjectIx)
		{
		TInt count = iObjectIx->Count();
		for(TInt i=0; i<count; i++)
			{
			CObject* theObj = (*iObjectIx)[i];
			if (theObj)
				{
				TInt handle = iObjectIx->At(theObj);
				// if we reach here somethingbad has happened. The server class
				// (i.e. the session object) is being closed by the client, BEFORE
				// the client has closes all of the open subsessions.
				// So the only reasonable thingto do here is:
				// delete the subsession - without closing it first. 'Cos this is
				// _exactly_ what the client side has asked for (for some reason).
				// The may or may not be legal, or even a good idea depending on the type of server
				// that is being developeds. But the server framework should NOT
				// prevent this from happening. Generating a panic is a 
				// server developers choice, not a framework developers choice.
				// We delete by calling CObjectIdx::Remove(). This will call CObject::Close.
				// This in turn calls MSubSessionImpl::MRelease::VirtualRelease
				// which will destroy the sub session for us.
				// CloseSubSession should not be called directly.
				iObjectIx->Remove(handle);
				
				//
				// If the array length has changed, then update count and move back one space,
				// so that the same cell will be re-inspected during the next iteration of the
				// loop  This is because we have removed the current cell and therefore the
				// next cell is actually in iObjectIx[i] (i.e. the current one).
				TInt newCount = iObjectIx->Count();
				
				if (newCount != count)
					{
					count = newCount;
					i--;
					}

				}
			}
		delete iObjectIx;
		}
	if (iContainerIx)
		{
		iContainerIx->Remove(iObjectCon);
		delete iContainerIx;
		}
	// session decrementing handled in the parent class
	LBSLOG(ELogP3, "<-CSecureSessionSubSessionBase::~CSecureSessionSubSessionBase()");
	}
	
/** 
called as part of the CSession2 construction. 
Create subsession handler container and call the session CreateL function to increase the session count

@internalTechnology
@released
 */	
void CSecureSessionSubSessionBase::CreateL()
	{
	LBSLOG(ELogP3, "->CSecureSessionSubSessionBase::CreateL");
	iObjectIx = CObjectIx::NewL();
	iContainerIx = CObjectConIx::NewL();
	iObjectCon = iContainerIx->CreateL();
	CSecureSessionBase::CreateL(); // call the parent class to inc the session count
	iBaseCreateCalled = ETrue;
	LBSLOG(ELogP3, "<-CSecureSessionSubSessionBase::CreateL");
	}
	
/** 
Do the real thing for subsession construction.
Check whether the session constructL function is called or not first. If not, panic the server,
Create a subsession and push it to stack, get the implementation of subsession and invoke the 
real subsession creation implementation.

Add the subsession handler to container and write the handler to client address space through message object
Complete the creating message.

@param aMessage A RMessage2 reference.
@internalTechnology
@released
 */	
void CSecureSessionSubSessionBase::CreateSubSessionL(const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iBaseCreateCalled, Server()->PanicServer(CSecureServerBase::EBaseCreateNotCalled));
	CSubSessionBase* subSession = new (ELeave) CSubSessionBase();
	// Note: Use the CObject::Close() function instead of usual destructor
	// because it is needed to correctly decrement the reference count.
	// Close() will destruct subSession anyway if the ref count == 0.
	CleanupClosePushL(*subSession);

	MSubSessionImpl* impl = subSession->GetImplL(aMessage);
	CleanupVirtualReleasePushL(*impl); // call Release() on destruction
	//CleanupVirtualRelease::PushL(*impl);
	subSession->SetImpl(impl); // will take ownership(!)
	CleanupStack::Pop(impl);

	iObjectCon->AddL(subSession);
	TInt handle = iObjectIx->AddL(subSession);
	TPckgC<TInt> pH(handle);
	MessageUtils::Write(aMessage, 3, pH);

	// We need to pass a handle to the controllign server down to the subsession object.
	// Since the subsession impl object is a CBase object (NOT a CSession2 object)
	// we haev to do this outherwise the subsession will not be able to referene the
	// server - which seems like a useful thing to do.
	const CSecureServerBase* server = reinterpret_cast<const CSecureServerBase*>(Server()); 	
	impl->CreateSubSessionL(aMessage, server);
	
	aMessage.Complete(KErrNone);
	CleanupStack::Pop(subSession);
	}
	
/** 
Do the real thing for subsession destruction.
Check whether the session constructL function is called or not first. If not, panic the server,
get a subsession handler from container and complete the message if subsession is already close, 
if not, close subsession and remove it from container then complete the message.

@param aMessage A RMessage2 reference.
@internalTechnology
@released
 */	
void CSecureSessionSubSessionBase::DestroySubSession(const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iBaseCreateCalled, Server()->PanicServer(CSecureServerBase::EBaseCreateNotCalled));
	CSubSessionBase* subSession = reinterpret_cast<CSubSessionBase*>(iObjectIx->At(aMessage.Int3()));
	
	if (subSession == NULL)
		{
		aMessage.Complete(KErrBadHandle);
		}
	else
		{
		// we have been asked to close a subsession by the client side so
		// do just that :). c.f. CSecureSessionSubSessionBase::~CSecureSessionSubSessionBase() above.
		// In this case we do need to call CloseSubSession before deleting
		// the subsession object.
		// We delete by calling CObjectIdx::Remove().This will call CObject::Close.
		// This in turn calls MSubSessionImpl::MRelease::VirtualRelease
		// which willdestroy the sub session for us.
		CSubSessionBase* subSession = static_cast<CSubSessionBase*>(iObjectIx->At(aMessage.Int3()));
		iObjectIx->Remove(aMessage.Int3());
		aMessage.Complete(KErrNone);
		}
	}

/** 
Check whether the session constructL function is called or not first. If not, panic the server,

Dispatch the message at session level, only two types of message are handled here:
SubSession Open and Close.
The other messages are fwd to subsession via DispatchMessage.

@param aMessage A RMessage2 reference.
@internalTechnology
@released
 */	
void CSecureSessionSubSessionBase::DispatchL(const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iBaseCreateCalled, Server()->PanicServer(CSecureServerBase::EBaseCreateNotCalled));
	switch(aMessage.Function())
		{
		case ESecureSubSessionBaseOpen:
			CreateSubSessionL(aMessage);
			break;
		case ESecureSubSessionBaseClose:
			DestroySubSession(aMessage);
			break;
		default:
			// the message could be EITHER a session operation - which we have to handle here, or an operation
			// handled by the subsession which we have to forward
			DispatchMessageL(aMessage);
			break;
		}
		
	}

/** 
Check whether the session constructL function is called or not first. If not, panic the server,

Check whether the message are for session or subsession
Dispatch the message at subsession level
get the subsession handler from container and dispatch the message to subsession implementation!

@param aMessage A RMessage2 reference.
@internalTechnology
@released
 */	
void CSecureSessionSubSessionBase::DispatchMessageL(const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iBaseCreateCalled, Server()->PanicServer(CSecureServerBase::EBaseCreateNotCalled));
	// NB the test is intened to ignore a function enum of ELastSessionMessageId - this is not a valid command
	if(aMessage.Function()<ELastSessionMessageId)
		{
		// the message is for the session
		ServiceMessageL(aMessage); // implmented in the DERIVED CSessionBase class.
		}
	else if(aMessage.Function()>ELastSessionMessageId)
		{
		// MUST be a message for the subsystem object - delegate
		CSubSessionBase* subSession = static_cast<CSubSessionBase*>(iObjectIx->At(aMessage.Int3()));
		subSession->Impl()->DispatchL(aMessage);
		}
	}

/** 
Return the server

@return Pointer to server base
@internalTechnology
@released
 */	
const CSecureServerBase* CSecureSessionSubSessionBase::Server() const
	{
	return reinterpret_cast<const CSecureServerBase*>(CSession2::Server());
	}

/** 
Check whether the session constructL function is called or not first. If not, panic the server,

Dispatch the message at session level, only two types of message are handled here:
SubSession Open and Close.
If error happened when open subsession, destroy the subsession.

otherwise, pop the error messages to session or fwd to subsession via DispatchError.

@param aMessage A RMessage2 reference.
@param aError Error code.
@internalTechnology
@released
 */	
void CSecureSessionSubSessionBase::DispatchError(const RMessage2& aMessage, TInt aError)
	{
	__ASSERT_ALWAYS(iBaseCreateCalled, Server()->PanicServer(CSecureServerBase::EBaseCreateNotCalled));
	CSubSessionBase* subSession = NULL;
	switch(aMessage.Function())
		{
		case ESecureSubSessionBaseOpen:
			if(aError == KErrArgument)
				{
				// panic the client we have NO idea what sort
				// of sub session class the user was tryign to create. 
				// This is a client side failure!, so shoot the client
				MessageUtils::PanicClient(aMessage, KServerFrameworkPanic, ESecureSubSessionBaseUnknownSubSessionType);
				// NB there is NOTHING to clean up here, everything is deleted via
				// the cleanup stack, and the subsession object was never added to the
				// obejct containers.... See CreateSubSessionL above
				}
			else
				{
				// CreateSubSessionL() has caused a Leave, so there is 
				// no sub session object to clear up - just complete the
				// message with an error
				aMessage.Complete(aError);
				}
			break;
		case ESecureSubSessionBaseClose:
			// Panic? Non-leaving function that just left?
			break;
		default:
			// DisplatchMessageL above has left - this could be becuase ServiceMessageL left or the subsession DisplatchL
			// NB the test is intened to ignore a function enum of ELastSessionMessageId - this is not a valid command
			if(aMessage.Function()<ELastSessionMessageId)
				{
				// the message is for the session
				ServiceMessageError(aMessage, aError); // implmented in the DERIVED CSessionBase class.
				}
			else if(aMessage.Function()>ELastSessionMessageId)
				{
				// MUST be a message for the subsystem object - delegate
				subSession = static_cast<CSubSessionBase*>(iObjectIx->At(aMessage.Int3()));
				subSession->Impl()->DispatchError(aMessage, aError);
				}
			break;
		}
	}
