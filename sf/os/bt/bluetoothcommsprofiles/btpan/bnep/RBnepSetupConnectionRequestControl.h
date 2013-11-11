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

#ifndef __RBNEPSETUPCONNECTIONREQUESTCONTROL_H
#define __RBNEPSETUPCONNECTIONREQUESTCONTROL_H 

#include <bttypes.h>
#include "RBnepControl.h"

NONSHARABLE_CLASS(RBnepSetupConnectionRequestControl) : public RBnepControl  
    {
    typedef enum
        {
        KUUIDSizeOffset = 1,
        KUUIDSizeLength = 1,
        KUUIDOffset = 2,
        KLocalUUID = 0,
        KRemoteUUID = 1,
        KMaxSizeOfSetupRequest = KSizeOfBnepControlType + 1 + 2 * KSdpUUIDMaxLength	// Type, UUID size, 2x UUIDs
        } SetupRequestConstants;
public:
    void InitL ();
    TInt LocalRole (TUUID& aRole);
    TInt RemoteRole (TUUID& aRole);
    void SetRolesL (TUUID& aLocalRole, TUUID& aRemoteRole);

private:
    TInt GetRole (TUUID& aRole, TInt aOffset);
    };

/**
   Fill in the supplied TUUID with the local role.
   @param aRole Updated to reflect the requested local role.
   In the case where an invalid UUID length has been specified in the connection
   request, aRole is set to EUnknown.
   @return Standard error codes.  KErrCorrupt is returned when UUID length was invalid.
*/ 
inline TInt RBnepSetupConnectionRequestControl::LocalRole (TUUID& aRole)
    {
    return GetRole(aRole, KLocalUUID); 
    }

/**
   Fill in the supplied TUUID with the remote role.
   @param aRole Updated to reflect the requested local role.
   In the case where an invalid UUID length has been specified in the connection
   request, aRole is set to EUnknown.
   @return Standard error codes.  KErrCorrupt is returned when UUID length was invalid.
*/
inline TInt RBnepSetupConnectionRequestControl::RemoteRole (TUUID& aRole)
    {
    return GetRole(aRole, KRemoteUUID); 
    }


#endif
