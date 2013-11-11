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

#ifndef SYMBIAN_NM_NODEMESSAGES_BASE_H
#define SYMBIAN_NM_NODEMESSAGES_BASE_H

#include <elements/nm_signatures.h>

namespace Messages
{

template<TInt id, TInt32 realm>
class TErrorSig : public Messages::TSigMessageIdNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
	TErrorSig(const TNodeSignal::TMessageId& aMessageId, TInt aErrorCode)
	:	TSigMessageIdNumber(TNodeSignal::TMessageId(id, realm), aMessageId, aErrorCode)
		{}
	TErrorSig(TInt aErrorCode)
	:	TSigMessageIdNumber(TNodeSignal::TMessageId(id, realm), TNodeSignal::TMessageId(), aErrorCode)
		{}
    };

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TEBase
	{
	friend class CGlobals;

private:
  	enum
 	{
	ENull                         = 1,
	ECancel                       = 2,
	EError                        = 3,
	};

	static void RegisterL(CGlobals& aGlobals);
	static void DeRegister(CGlobals& aGlobals);

public:
    enum { ERealmId = 0x10285F38 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms


    /*==========================================================================
    <<Keys:>>
    [Name]:      <messagename>
    [Semantics]: Message semantics

    [Type]:      Request for <responsemsg>|Response to <requestmsg>|Notification/[Peerless]
                 Where:
                 Request      - request to perform an action. Sender should expect <responsemsg>
                                to ackowledge the completion of the action.
                 Response     - acknowledgement that the previous request is now completed.
                 Notification - a one-off message flagging an event.
                 Peerless     - the message is peerless, i.e.: the sender doesn't have to
                                be the recipient's client.
    [Fallible]:  Yes|No
                 Only applicable to requests and denotes whether the request can fail or not,
                 or, in other words, whether the request can be completed to with TError.

    [Cancellable]: Yes|No
                 Only applicable to requests and denotes whether the request can be cancelled or not.

    [Structure]:
    Every message defined in this realm has at least:
    param        iSender      - the message sender
    ===========================================================================*/
    typedef TMessageSigVoid<ENull, TEBase::ERealmId> TNull;

    /*==========================================================================
    [Name]:      TCancel
    [Semantics]: Request to cancel the previous request identified by the activityid
    		     carried by this message. Only fallible requests can be cancelled.
    		     Completion of TCancel is acknowledged by TError.

    		     WARNING! The request being cancelled may have already been completed
    		     and the response to it may already be in the pipeline at the time TCancel
    		     is being sent. If this is the case, the recipient of TCancel will not
    		     have the activity to match the TCancel with and will hence ignore
    		     TCancel. The requestor (and sender of TCancel) must be prepared to
    		     receive both the normal (success) response to the request being cancelled,
    		     as well as the TError(KErrCancel) response.

    		     Some requests cannot be cancelled. This is true for all requests which
    		     are irreversible in nature or cancelling of which can fail.
    		     As an example, we say that cancelling of the destruction request is not
    		     possible because destruction is irreversible.
    		     Another example could be the stop request. It is not possible to cancel
    		     it because it would almost mean "start again" which is of course fallible.

    [Type]:      Request for TError(KErrCancel)
    [Fallible]:  No
    [Cancellable] No

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigVoid<ECancel, TEBase::ERealmId> TCancel;

	/*==========================================================================
	[Name]:      TError
	[Semantics]: An indication that the previous request has failed. TError is always
                 a response to a previous request (there are no unsolicited TErrors).
	[Type]:      Response to any faillible request.

	[Structure]:
	param: iMsgId - the request that has failed
		   iValue - error code.
	===========================================================================*/
    typedef TErrorSig<EError, TEBase::ERealmId> TError;
	};


} //namespace Messages


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <elements/nm_messages_internal.h>
#endif


#endif
	//SYMBIAN_NM_NODEMESSAGES_BASE_H

