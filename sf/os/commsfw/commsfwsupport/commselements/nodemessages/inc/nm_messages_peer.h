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

#ifndef SYMBIAN_NM_NODEMESSAGES_PEER_H
#define SYMBIAN_NM_NODEMESSAGES_PEER_H

#include <elements/nm_signatures.h>
#include <elements/nm_messages_base.h>

namespace Messages
{

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TEPeer : public TEBase
	{
	friend class CGlobals;

private:
  	enum
 	{
	ELeaveRequest				= 1,
	ELeaveComplete				= 2,
	};

public:
    enum { ERealmId = 0x10285F3C }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /*==========================================================================
    [Name]:      TClientLeavingRequest
    [Semantics]: Request to sign off out from the recipient's clients' list.
    			 The difference between this request and TClientLeaving is the ability to synchronise
    			 the sender's activity with the completion of the sign-out.

    [Type]:      Request for TLeaveComplete.
    [Fallible]:  No.
    [Cancellable]: No

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigVoid<ELeaveRequest, TEPeer::ERealmId> TLeaveRequest;

    /*==========================================================================
    [Name]:      TLeaveComplete
    [Semantics]: Request to sign off out from the recipient's clients' list succeeded.

    [Type]:      Response to TClientLeavingRequest.

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigVoid<ELeaveComplete, TEPeer::ERealmId> TLeaveComplete;

	};


} //namespace Messages

#endif	//SYMBIAN_NM_NODEMESSAGES_PEER_H



