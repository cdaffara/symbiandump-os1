// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BASEBANDPACKETCONSTS_H
#define BASEBANDPACKETCONSTS_H

#include <e32def.h>

// Common packet types
const TUint16 KDM1Packet = 0x0008; /*!< DM1 Packet */

// ACL only packet types
const TUint16 K2DH1Packet = 0x0002; /*!< 2-DH1 Packet */
const TUint16 K3DH1Packet = 0x0004; /*!< 3-DH1 Packet */
const TUint16 KDH1Packet = 0x0010; /*!< DH1 Packet */
const TUint16 K2DH3Packet = 0x0100; /*!< 2-DH3 Packet */
const TUint16 K3DH3Packet = 0x0200; /*!< 3-DH3 Packet */
const TUint16 KAUX1Packet = 0x0200; /*!< AUX1 Packet */
const TUint16 KDM3Packet = 0x0400; /*!< DM3 Packet */
const TUint16 KDH3Packet = 0x0800; /*!< DH3 Packet */
const TUint16 K2DH5Packet = 0x1000; /*!< 2-DH5 Packet */
const TUint16 K3DH5Packet = 0x2000; /*!< 3-DH5 Packet */
const TUint16 KDM5Packet = 0x4000; /*!< DM5 Packet */
const TUint16 KDH5Packet = 0x8000; /*!< DH5 Packet */

// SCO only packet types
const TUint16 KHV1Packet = 0x0020; /*!< HV1 Packet */
const TUint16 KHV2Packet = 0x0040; /*!< HV2 Packet */
const TUint16 KHV3Packet = 0x0080; /*!< HV3 Packet */
const TUint16 KDVPacket = 0x0100; /*!< DV Packet */

// eSCO only packet types
const TUint16 KEV3Packet = 0x0008; /*!< EV3 Packet */
const TUint16 KEV4Packet = 0x0010; /*!< EV4 Packet */
const TUint16 KEV5Packet = 0x0020; /*!< EV5 Packet */
const TUint16 K2EV3Packet = 0x0040; /*!< 2-EV3 Packet */
const TUint16 K3EV3Packet = 0x0080; /*!< 3-EV3 Packet */
const TUint16 K2EV5Packet = 0x0100; /*!< 2-EV5 Packet */
const TUint16 K3EV5Packet = 0x0200; /*!< 3-EV5 Packet */

#endif // BASEBANDPACKETCONSTS_H 
