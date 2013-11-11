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
 @internalTechnology
*/

#ifndef DUMMYMESSAGES_H
#define DUMMYMESSAGES_H

#include <elements/nm_signatures.h>

namespace Dummy
{

class TBindProtocol
	{
  private:
  	enum
 	{
 	EFindOrCreateFactoryObject = 1,
 	EFactoryObjectCreated      = 2,
 	EJoiningRequest            = 3,
 	EJoinComplete              = 4
	};

public:
    enum { ERealmId = 0x10285A57 };

    /*==========================================================================
    [Name]:       TFindOrCreateFactoryObject
    [Semantics]:  Sent to a factory (container) to request a factory object.
                  The factory object is to be preexisting or newly created
                  depending on the request argument and factory logic.

    [Type]:       Request for TFactoryObjectCreated.
    [Fallible]:   Yes
    [Cancellable] Yes

    [Structure]:
    param  iUid - Uid of the object being asked for.
    ===========================================================================*/
	typedef Messages::TMessageSigUid<EFindOrCreateFactoryObject, ERealmId> TFindOrCreateFactoryObject;

    /*==========================================================================
    [Name]:      TFactoryObjectCreated
    [Semantics]: Response to TFindOrCreateFactoryObject / etc.
                 Carries the actual object's address + sometimes pointer.

    [Type]:      Response

    [Structure]:
    param        iCommsId - address of the new node
    param        iAny - pointer, if any
    ===========================================================================*/
	typedef Messages::TMessageSigNodeIdAny<EFactoryObjectCreated, ERealmId> TFactoryObjectCreated;

    /*==========================================================================
    [Name]:      TClientJoiningRequest
    [Semantics]: A request to add a new client to the recipient's clients' list.

    [Type]:      Request for TJoinComplete/Peerless
    [Fallible]:  Yes.
    [Cancellable] Yes
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<EJoiningRequest, ERealmId> TClientJoiningRequest;

    /*==========================================================================
    [Name]:      TJoinComplete
    [Semantics]: Adding new client to the recipient's clients' list succeeded.

    [Type]:      Response to TClientJoiningRequest.
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<EJoinComplete, ERealmId> TJoinComplete;
	};

class TStartProtocol
	{
  private:
  	enum
 	{
	EStart   = 1,
	EStarted = 2,
	};

public:
    enum { ERealmId = 0x10285A58 };

    /*==========================================================================
    [Name]:      TStart
    [Semantics]: Request to start the recipient. The recipient should create and start
                 whatever it needs to start itself.

    [Type]:      Request for TStarted.
    [Fallible]:  Yes.
    [Cancellable] Yes.

    [Structure]:
    param: iValue - dummy start parameter
    ===========================================================================*/
    typedef Messages::TMessageSigNumber<EStart, ERealmId> TStart;

    /*==========================================================================
    [Name]:      TStarted
    [Semantics]: The recipient has now started.

    [Type]:      Response to TStart.

    [Structure]:
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<EStarted, ERealmId> TStarted;
	};

class TStopProtocol
	{
  private:
  	enum
 	{
	EStop    = 1,
	EStopped = 2,
	};

public:
    enum { ERealmId = 0x10285A59 };

    /*==========================================================================
    [Name]:      TStop
    [Semantics]: Request to stop the recipient.
                 The recipient should stop and leave whatever it does no longer
                 require.

    [Type]:      Request for TStopped.
    [Fallible]:  No.
    [Cancellable] No.

    [Structure]:
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<EStop, ERealmId> TStop;

    /*==========================================================================
    [Name]:      TStopped
    [Semantics]: The recipient has stopped.

    [Type]:      Response to TStop.

    [Structure]:
    ===========================================================================*/
    typedef Messages::TMessageSigVoid<EStopped, ERealmId> TStopped;
	};

} //namespace Dummy

#endif
//DUMMYMESSAGES_H

