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

#ifndef SYMBIAN_NM_MESSAGES_CHILD_H
#define SYMBIAN_NM_MESSAGES_CHILD_H

#include <elements/nm_messages_base.h>

namespace Messages
{

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TEChild : public TEBase
	{
	friend class CGlobals;

private:
  	enum
 	{
	EDestroy					= 1,
	ELeft						= 2
	};

public:
    enum { ERealmId = 0x10285F39 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /*==========================================================================
    [Name]:      TDestroy
    [Semantics]: Request to perform self-destroy. Typically sent by a control provider certain
                 that there are no dangling references to the recipient held by anyone in the system.

    [Type]:      Request for TLeft
    [Fallible]:  No.
    [Cancellable]: No

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigVoid<EDestroy, TEChild::ERealmId> TDestroy;

	/*==========================================================================
    [Name]:      TLeft
    [Semantics]: Notification about the client (sender) signing out from the recipient's clients'
    			 list.

    [Type]:      Notification

    [Structure]:
    ===========================================================================*/
    typedef TMessageSigVoid<ELeft, TEChild::ERealmId> TLeft;

	};

} //namespace Messages

#endif
	//SYMBIAN_NM_MESSAGES_CHILD_H

