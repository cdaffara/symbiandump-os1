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


#ifndef LBSPSYNGMSGS_H
#define LBSPSYNGMSGS_H

#include <e32std.h>
#include "lbscommoninternaldatatypes.h"
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsloccommon.h>
#include "lbsnetinternalmsgtypes.h"

/**
@file
@internalTechnology
@released
*/

/** Location request from the PSY
*/
NONSHARABLE_CLASS(TLbsCellLocationRequestMsg) : public TLbsNetInternalMsgBase
    {
public:
    IMPORT_C TLbsCellLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId, 
                                           TUid aProtocolModule,
                                           const TLbsNetPosRequestOptionsInt& aOptions);

    IMPORT_C TUid ProtocolModule() const;    
    IMPORT_C const TLbsNetPosRequestOptionsInt& Options() const;
    
private:
    TLbsCellLocationRequestMsg();

private:
    TLbsNetPosRequestOptionsInt iOptions;
    TUid iProtocolModule;
    };
  
/** Location request cancel from the PSY
*/
NONSHARABLE_CLASS(TLbsCellLocationCancelMsg) : public TLbsNetInternalMsgBase
    {
public:
    IMPORT_C TLbsCellLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
                                          TInt aReason);
    IMPORT_C TInt Reason() const;

private:
    TLbsCellLocationCancelMsg();
    
private:
    TInt iReason;
    };

/** Response to a location request to PSY
*/
NONSHARABLE_CLASS(TLbsCellLocationResponseMsg) : public TLbsNetInternalMsgBase
    {
public:
    IMPORT_C TLbsCellLocationResponseMsg(const TLbsNetSessionIdInt& aSessionId,
                                        TInt aReason,
                                        const TPositionInfoBase& aPosInfo);
                                        
    
    IMPORT_C TInt Reason() const;
    IMPORT_C const TPositionInfoBase& PositionInfo() const;
        
private:
    TLbsCellLocationResponseMsg();
    
private:
    TInt iReason;
    TPositionExtendedSatelliteInfo iPosInfo;
    };

#endif // LBSPSYNGMSGS_H
