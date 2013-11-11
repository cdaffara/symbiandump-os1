// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BNEPUTILS_H
#define __BNEPUTILS_H 

#include <e32std.h>
#include <e32def.h>
#include <bttypes.h>

const TInt KEthernetHWAddrSize = KBTDevAddrSize;
const TInt KEthernetProtocolSize = 2;
const TInt KBnepPacketTypeSize = 1;
const TInt KMaxBnepHeaderSize = 19;
const TInt KEthernetProtocolOffset = KEthernetHWAddrSize * 2;
const TInt KEthernetHeaderSize = KEthernetProtocolOffset + KEthernetProtocolSize;

const TInt KSocketReaderPriority = EPriorityHigh;
//KBnepProceedCallBackPriority is used for async callback from PanAgent after processing
//a part of the frame. The priority has be higher than the one used in socket reader.
//Otherwise the current frame may be overwritten if the socket reader completes request 
//before the async callback.
const TInt KBnepProceedCallBackPriority = KSocketReaderPriority + 1;

namespace Bnep
    {
    enum TBnepPanic
        {
        ENullTlsPointer,
        ENullPointer,
        EUnableToRespond,
        ESocketWriterQueueOverflow,
        ELogResetAttempt,
        EAppendFailed,
        ECallBackAlreadyAdded,
        EProceedingOnInvalidLink,
        EUnexpectedMessage
        };
    } // Namespace Bnep

class BnepUtils 
    {
public:
    static TBool IsMulticast (const TBTDevAddr& aAddr);
    static void Panic (Bnep::TBnepPanic aReason);
    };

#endif
