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

#include <bluetooth/logger.h>
#include <e32std.h>
#include "RBnepSetupConnectionRequestControl.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

const TInt KRoleUnknown = 0xffff;

/**
   Extract a role UUID.
   @param aRole a TUUID object to update.
   @param aOffset The role extracted is the aOffset the role in the packet.
   valid values are zero and one.
   @return KErrCorrupt Invalid UUID size.
   @internalComponent
*/
TInt RBnepSetupConnectionRequestControl::GetRole (TUUID& aRole, TInt aOffset)
    {
    LOG_FUNC
    aRole = TUUID(KRoleUnknown);
    TBuf8<KSdpUUIDMaxLength> buf;
    buf.SetLength(KUUIDSizeLength);
	if(Length() < KUUIDSizeOffset + KUUIDSizeLength)
		{
		return KErrCorrupt;
		}
    CopyOut(buf, KUUIDSizeOffset);
    TInt uuidSize = buf[0];
    LOG1(_L8("uuidSize = %d"),uuidSize);                                                          	
	TInt offset;
    switch (uuidSize)
        {
        case 2:
        case 4:
        case 16:
            // If the UUID length is valid, extract its value.
            buf.SetLength(uuidSize);
 			//Check the offset is within our buffer boundary
		 	offset = KUUIDOffset + aOffset * uuidSize;
		 	if(Length() < offset + uuidSize)
		 		{
		 		return KErrCorrupt;
		 		}
            CopyOut(buf, offset);
		
            TRAPD(err, aRole.SetL(buf));
            return err;
        default:
            return KErrCorrupt;
        } 
    }

/**
   Allocate the memory required for a connection setup request.
   @internalComponent
*/
void RBnepSetupConnectionRequestControl::InitL ()
    {
    LOG_FUNC
    AllocL(KMaxSizeOfSetupRequest); 
    }

/**
   Initialise the role UUIDs.
   @internalComponent
*/
void RBnepSetupConnectionRequestControl::SetRolesL (TUUID& aLocalRole, TUUID& aRemoteRole)
    {
    LOG_FUNC
    TInt uuidSize = Max(aLocalRole.MinimumSize(), aRemoteRole.MinimumSize());
    TBuf8<2*sizeof(TUint8)> header;
    header.SetMax();
    header[KControlTypeOffset] = EBnepSetupConnectionRequestMessage;
    header[KUUIDSizeOffset] = static_cast<TUint8>(uuidSize);
    CopyIn(header);
    TPtrC8 uuid;
    uuid.Set(aRemoteRole.SpecifiedLengthL(uuidSize));
    CopyIn(uuid, KUUIDOffset);
    uuid.Set(aLocalRole.SpecifiedLengthL(uuidSize));
    CopyIn(uuid, KUUIDOffset + uuidSize);
    TrimEnd(KUUIDOffset + (2 * uuidSize));
    DUMPFRAME(_L8("RBnepSetupConnectionRequestControl"));
    }



