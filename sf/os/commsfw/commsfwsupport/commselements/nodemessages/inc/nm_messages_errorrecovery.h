// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NM_NODEMESSAGES_ERRORRECOVERY_H
#define SYMBIAN_NM_NODEMESSAGES_ERRORRECOVERY_H

#include <elements/nm_common.h>
#include <elements/nm_address.h>
#include <elements/nm_signatures.h>

namespace Messages
{

struct TErrContext
/**
 @publishedPartner
 @released
*/
    {
    TErrContext()
    :	iOriginator(),
    	iActivitySigId(0),
    	iMessageId(),
    	iStateChange(0,0),
		iInfo(NULL)
    	{
    	}

    TErrContext(const TNodeId& aOriginator, const TNodeSignal::TMessageId& aMessageId, TUint aActivitySigId, const Elements::TStateChange& aStateChange, TAny* aInfo = NULL)
    :	iOriginator(aOriginator),
    	iActivitySigId(aActivitySigId),
    	iMessageId(aMessageId),
    	iStateChange(aStateChange),
		iInfo(aInfo)
    	{
    	}

    TNodeId iOriginator;
    TUint iActivitySigId;
    TNodeSignal::TMessageId iMessageId;
    Elements::TStateChange iStateChange;
    TAny* iInfo; //Optional, additional info, if any
    };

struct TErrResponse
/**
 @publishedPartner
 @released
*/
    {
    enum TRespAction
        {
        EIgnore,
        EPropagate,
        ERetry
        };

    TErrResponse()
    :	iAction(EIgnore),
    	iError(KErrNone),
    	iMessageId()
    	{
    	}

    TErrResponse(TRespAction aRespAction, TInt aError, const TNodeSignal::TMessageId& aMessageId)
    :	iAction(aRespAction),
    	iError(aError),
    	iMessageId(aMessageId)
    	{
    	}

    TRespAction iAction;
    TInt iError;                              //goes to TError.iValue (e.g. what to propagate in case iAction == ERespPropagate)
    TNodeSignal::TMessageId iMessageId; //goes to TError.iMsgId (e.g. what to propagate in case iAction == ERespPropagate)
    };


DECLARE_MESSAGE_SIG_1(SigErrContext, TErrContext, ErrContext)
DECLARE_MESSAGE_SIG_1(SigErrResponse, TErrResponse, ErrResponse)

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TEErrorRecovery
	{
	friend class CGlobals;

private:
  	enum
 	{
	EErrorRecoveryRequest         = 1,
	EErrorRecoveryResponse        = 2,
	};

public:
    enum { ERealmId = 0x10285F3A }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /*==========================================================================
    [Name]:      TErrorRecoveryRequest
    [Semantics]: [399TODO] RZ: Michal please finish.

    [Type]:
    [Fallible]: Effectively no (as an error is also a response to this request)
    [Cancellable]: No

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigErrContext<EErrorRecoveryRequest, TEErrorRecovery::ERealmId> TErrorRecoveryRequest;

    /*==========================================================================
    [Name]:      TErrorRecoveryResponse
    [Semantics]: [399TODO] RZ: Michal please finish.

    [Type]:

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigErrResponse<EErrorRecoveryResponse, TEErrorRecovery::ERealmId> TErrorRecoveryResponse;
	};

} //namespace Messages

#endif
	//SYMBIAN_NM_NODEMESSAGES_ERROR_RECOVERY_H

