/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef ACCMONCONNECTIONEMULATORCONSTS_H
#define ACCMONCONNECTIONEMULATORCONSTS_H

const TUint64 KBTAddrCarkit            = 0x001122334455;
const TUint64 KBTAddrHeadsetHSPAndHFP  = 0x112233445566;
const TUint64 KBTAddrHeadsetHSP        = 0x223344556677;
const TUint64 KBTAddrHeadsetHFP        = 0x334455667788;
const TUint64 KBTAddrHeadsetEmptyPR    = 0x445566778899;
const TUint64 KBTAddrHeadsetNokiaSPId  = 0x556677889900;
const TUint64 KBTAddrNoResponse        = 0x667788990011;
const TUint64 KBTAddrErrCode           = 0x778899001122;
const TUint64 KBTAllBitsOn             = 0x889900112233;
const TUint64 KBTAddrHeadsetAVRCP      = 0x990011223344;
const TUint64 KBTAddrHeadseA2DP        = 0x111111111111;

const TUid    KTFConnectionEmulatorTestProperty   = {0x099150D7};
const TUint32 KTFConnectionEmulatorMethod         =  0x00001002;

enum TTFASYReference
    {
    ETFAsyNone = 0,
    ETFAsyConn,
    ETFAsyUpdateTVOut,
    ETFAsyUpdateHeadset,
    ETFAsyConnectECIAccessory,
    ETFAsyConnectHeadset,
    ETFAsyConnOpenCable,
    ETFAsyConnectOpenCable3Poles,
    ETFAsyConnectOpenCable4Poles,
    ETFAsyConnectECITVOut,
    ETFAsyConnectECIMusicStand,
    ETFAsyConnectECICarKit,
    ETFBTConnectCarKit,
    ETFBTConnectHeadset,
    ETFAsyConnectArrayOfCapabilities,
    ETFAsyConnectLoopset,
    ETFAsyConnectTTY,
    ETFAsyDisc,
    ETFBTALinkClosedNo,
    ETFBTALinkOpenedNo,
    ETFAsySetBoolValue
    };

#endif      // ACCMONCONNECTIONEMULATORCONSTS_H