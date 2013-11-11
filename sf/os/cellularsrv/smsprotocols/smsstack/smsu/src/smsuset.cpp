// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the TSmsSettings class.
// 
//

/**
 @file
*/

#include "smsuset.h"

#include <s32strm.h>


#include "smsstacklog.h"



/**
 *  Constructor.
 *  
 *  Initialises the settings with default values.
 *  
 *  @capability None
 */
EXPORT_C TSmsSettings::TSmsSettings():
    iTimeoutMicroSeconds32(KDefaultTimeoutMicroSeconds32),
    iSendTryLimit(KDefaultSendTryLimit),
    iFlags(ESmsFlagDeletePDUsFromPhoneStores),
    iReassemblyLifetime(KDefaultReassemblyLifetime), //  one day
    iKSegmentationLifetimeMultiplier(KDefaultKSegmentationLifetimeMultiplier),
    iModemInitializationTimeoutMicroSeconds32(KDefaultModemInitializationTimeoutMicroSeconds32),
    iSendTryTimeoutMicroSeconds32(KDefaultSendTryTimeoutMicroSeconds32), // 60 seconds
    iBootTimerTimeout32(KDefaultBootTimerMicroSeconds32)
    {
    }

/**
 *  Internalises the object.
 *  
 *  Sets the SMS sttings from the stream store.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void TSmsSettings::InternalizeL(RReadStream& aStream)
    {
    // Ignore in code coverage - not used within the SMS stack.
    BULLSEYE_OFF
    iTimeoutMicroSeconds32=aStream.ReadInt32L();
    iSendTryLimit=aStream.ReadInt32L();
    iFlags=aStream.ReadUint32L();
    iReassemblyLifetime=aStream.ReadUint32L();
    iKSegmentationLifetimeMultiplier=aStream.ReadUint32L();
    iModemInitializationTimeoutMicroSeconds32=aStream.ReadInt32L();
    BULLSEYE_RESTORE
    }

/**
 *  Externalises the object.
 *  
 *  Gets the SMS sttings from the stream store.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void TSmsSettings::ExternalizeL(RWriteStream& aStream) const
    {
    // Ignore in code coverage - not used within the SMS stack.
    BULLSEYE_OFF
    aStream.WriteInt32L(iTimeoutMicroSeconds32.Int());
    aStream.WriteInt32L(iSendTryLimit);
    aStream.WriteUint32L(iFlags);
    aStream.WriteUint32L(iReassemblyLifetime.Int());
    aStream.WriteUint32L(iKSegmentationLifetimeMultiplier);
    aStream.WriteInt32L(iModemInitializationTimeoutMicroSeconds32.Int());
    BULLSEYE_RESTORE
    }
