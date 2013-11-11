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
// The file contains implementation of TGsmSmsSlot class.
// 
//

/**
 @file
*/


#include "gsmuSmsSlot.h"

#include "smsstacklog.h"

/**
 *  Default constructor.
 */
EXPORT_C TGsmSmsSlot::TGsmSmsSlot()
	{
	} // TGsmSmsSlot::TGsmSmsSlot


/**
 *  Constructor.
 */
EXPORT_C TGsmSmsSlot::TGsmSmsSlot(const RMobileSmsStore::TMobileGsmSmsEntryV1& aSmsEntry)
	: RMobileSmsStore::TMobileGsmSmsEntryV1( aSmsEntry )
	{
	} // RMobileSmsStore::TMobileGsmSmsEntryV1


/**
 *  Internalize
 *  
 *  @param aStream For internalizing the data
 */
void TGsmSmsSlot::InternalizeL(RReadStream& aStream)
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF
    aStream >> iStore;
    iIndex=aStream.ReadInt32L();
    BULLSEYE_RESTORE
    }

/**
 *  Externalize
 *  
 *  @param aStream For Externalizing the data
 */
void TGsmSmsSlot::ExternalizeL(RWriteStream& aStream) const
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF
    aStream << iStore;
    aStream.WriteInt32L(iIndex);
    BULLSEYE_RESTORE
    }
