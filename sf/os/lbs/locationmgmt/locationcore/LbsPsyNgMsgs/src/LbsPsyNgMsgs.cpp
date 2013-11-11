/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#include "LbsPsyNgMsgs.h"
#include <lbsclasstypes.h>

//
// Location Request Message from PSY
//
EXPORT_C TLbsCellLocationRequestMsg::TLbsCellLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId,
                                                                TUid aProtocolModule,
                                                                const TLbsNetPosRequestOptionsInt& aOptions) :
        TLbsNetInternalMsgBase(ECellLocationRequest, 
                      sizeof(TLbsCellLocationRequestMsg),
                      aSessionId)
    {
    // Must make sure that the data to be stored is not bigger than the buffer.
    __ASSERT_COMPILE(sizeof(TLbsCellLocationRequestMsg) <= KMaximumMessageSizeBytes);

    iProtocolModule = aProtocolModule;
    iOptions = aOptions;
    }

EXPORT_C const TLbsNetPosRequestOptionsInt& TLbsCellLocationRequestMsg::Options() const
    {
    return iOptions;
    }
    
EXPORT_C TUid TLbsCellLocationRequestMsg::ProtocolModule() const
    {
    return iProtocolModule;
    }

//
// Location Cancel Message from PSY
//
EXPORT_C TLbsCellLocationCancelMsg::TLbsCellLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
                                                                    TInt aReason):
    TLbsNetInternalMsgBase(ECellLocationCancel, 
                           sizeof(TLbsCellLocationCancelMsg),
                           aSessionId)
    {
    // Must make sure that the data to be stored is not bigger than the buffer.
    __ASSERT_COMPILE(sizeof(TLbsCellLocationCancelMsg) <= KMaximumMessageSizeBytes);
    
    iReason = aReason;
    }
                                       
EXPORT_C TInt TLbsCellLocationCancelMsg::Reason() const
    {
    return iReason;
    }

//
// Location Response Message to PSY
//
EXPORT_C TLbsCellLocationResponseMsg::TLbsCellLocationResponseMsg(const TLbsNetSessionIdInt& aSessionId,
                                                                TInt aReason,
                                                                const TPositionInfoBase& aPosInfo): 
         TLbsNetInternalMsgBase(ECellLocationResponse, 
                           sizeof(TLbsCellLocationResponseMsg), 
                           aSessionId)
    {
    // Must make sure that the data to be stored is not bigger than the buffer.
    __ASSERT_COMPILE(sizeof(TLbsCellLocationResponseMsg) <= KMaximumMessageSizeBytes);

    iReason = aReason;
    // Check that the type of aPosInfo is valid
    TUint32 type = aPosInfo.PositionClassType();
    if (((type & EPositionInfoUnknownClass) != EPositionInfoUnknownClass)
        && ((type & EPositionInfoClass) != EPositionInfoClass)
        && ((type & EPositionCourseInfoClass) != EPositionCourseInfoClass)
        && ((type & EPositionSatelliteInfoClass) != EPositionSatelliteInfoClass)
        && ((type & EPositionExtendedSatelliteInfoClass) != EPositionExtendedSatelliteInfoClass))
        {
        User::Panic(_L("LbsNetInternal"), 1);
        }
    
    // The TPositionInfoBase is just the base class, so we need to copy over 
    // the data from the actual concrete class type.
    __ASSERT_DEBUG(aPosInfo.PositionClassSize() <= sizeof(TPositionExtendedSatelliteInfo), User::Invariant());
    Mem::Copy(&iPosInfo, &aPosInfo, aPosInfo.PositionClassSize());
    }

EXPORT_C TInt TLbsCellLocationResponseMsg::Reason() const
    {
    return iReason;
    }

EXPORT_C const TPositionInfoBase& TLbsCellLocationResponseMsg::PositionInfo() const
    {
    return iPosInfo;
    }
