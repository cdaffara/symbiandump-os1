// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements patchable constants for rawip
//

/**
 @file
*/

#include <e32def.h>
#include <Constants.h>
#include <rawip_const.h>

#ifdef __EABI__
namespace RawIPConst {
    // Patch data is used and KMtuIPv4 and KRMtuIPv4 can be modified to a different value in RawIpNif.iby file
    extern const TInt KMtuIPv4 = KDefaultMtu;
    extern const TInt KRMtuIPv4 = KDefaultMtu;
    
    // Patch data is used and KMtuIPv6 and KRMtuIPv6 can be modified to a different value in RawIpNif.iby file
    extern const TInt KMtuIPv6 = KDefaultMtu;
    extern const TInt KRMtuIPv6 = KDefaultMtu;
    
    extern const TUint KDefaultSendQueueSize=5;
    
    // Patch data is used and KMaxTxIPPacketSize and KMaxRxIPPacketSize can be modified to a different value in RawIpNif.iby file
    extern const TInt KMaxSendQueueLen = KDefaultSendQueueSize;
    extern const TInt KMaxTxIPPacketSize = KMaxIPPacket + KIPTagHeaderLength;
    extern const TInt KMaxRxIPPacketSize = KMaxIPPacket + KIPTagHeaderLength;
};

#endif

