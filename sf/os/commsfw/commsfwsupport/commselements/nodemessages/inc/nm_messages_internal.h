// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalTechnology
*/


#ifndef __NM_NODEMESSAGES_BASE_INTERNAL_H__
#define __NM_NODEMESSAGES_BASE_INTERNAL_H__

#include <elements/nm_signatures.h>

#include <elements/nm_messages_base.h>

namespace Messages
{


NONSHARABLE_CLASS(TDispatchErrorHandler) : public TNodeSignal
/**
	Handles the propagation of a dispatch error back to the requesting
	client.

	@internalComponent
*/
    {
protected:
    explicit TDispatchErrorHandler(TNodeSignal::TMessageId aMessageId)
    :	TNodeSignal(aMessageId)
        {
        }

    TDispatchErrorHandler()
    	{
    	}

	virtual void Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError)
	    {
	    TEBase::TError err(MessageId(), aError);
    	RClientInterface::OpenPostMessageClose(aRecipient, aSender, err);
    	}
    };

class TSelfDispatcherAndErrorHandler : public TSignalBase
/**
	Adds default error handling for self-dispatching messages. The handling fires a TError back
	to the requesting client.

	@internalComponent
*/
    {
protected:
    TSelfDispatcherAndErrorHandler()
    	{
    	}

	virtual void Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError)
	    {
	    TEBase::TError err(aError);
    	RClientInterface::OpenPostMessageClose(aRecipient, aSender, err);
    	}
    };

} // namespace Messages


#endif // __NM_NODEMESSAGES_BASE_INTERNAL_H__

