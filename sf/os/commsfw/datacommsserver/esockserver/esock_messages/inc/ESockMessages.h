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

#if !defined(GUARD_ESOCKMESSAGES_H)
#define GUARD_ESOCKMESSAGES_H

#include <es_prot.h>
#include <connpref.h>

namespace ESock
{

/** Types of an ESockMessage interface represented by CMessage.
@released since 9.2
*/
enum TESockMsgImplementations
    {
/** Any ESock message type */
    EESockMessageAny = Meta::KNetMetaTypeAny,
/** Message type for a connection preferences to be used for starting a connection*/
    EESockMessageCommsDes8,
    EESockMessageCommsNifBuffer
    };

const TInt KESockMessagesImplementationUid = 0x1027366f;
/** Message type for a connection preferences to be used for starting a connection
@released since 9.2
@internalTechnology
*/


} //namespace ESock

#endif   //GUARD_ESOCKMESSAGES_H

