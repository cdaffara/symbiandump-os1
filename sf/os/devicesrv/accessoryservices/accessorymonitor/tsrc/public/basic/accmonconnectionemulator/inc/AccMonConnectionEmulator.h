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
* Description:  Used to emulate accessory connections in emulator.
*
*/


#ifndef ACCMONCONNECTIONEMULATOR_H
#define ACCMONCONNECTIONEMULATOR_H

#include <e32cons.h>
#include <e32property.h>
#include <accmonitorinfo.h>
#include <accpolgenericid.h>

NONSHARABLE_CLASS( CConnectionEmulator )
    {
public:
    
    /**
    * Enumerators for connecting an accessory type.
    */
    enum TEnumAccConnectType
        {
        ETestNull = 0,
        EWiredHeadSetCon = 1,
        EWirelessHeadSetCon = 2,
        EWiredCarKitCon = 3,
        EWirelessCarKitCon = 4,
        EMusicStandCon = 5,
        ETVOutCon = 6,
        ELoopset = 7,
        ETTY = 8,
        E3PoleOpenCable = 9
        };
    
    /**
    * Symbian two phased constructor.
    */
    IMPORT_C static CConnectionEmulator* NewL();
    
    /**
    * Symbian two phased constructor. Leaves the instance to cleanup stack.
    */
    IMPORT_C static CConnectionEmulator* NewLC();
    
    /**
    * Connects some accessory with predefined accessory type.
    *
    * @param    aAccDefintion is an definition of some accessory type
    *           that is defiend in AccMonConnectionTypes.h.
    * @param    aSync defines if the connect is done synchronously(ETrue) or
    *           asynchronously(EFalse).
    * @return   KErrNone if query succeeded otherwise standard Symbian error
    *           code.
    */
    IMPORT_C TInt ConnectAccessoryL( TEnumAccConnectType aAccDefintion, TBool aSync );
    
    /**
    * Connects some accessory with capabilities definend in an array.
    *
    * @param    aCapabilityArray is an array of accessory capabilities that
    *           the connected accessory needs to have.
    * @param    aSync defines if the connect is done synchronously(ETrue) or
    *           asynchronously(EFalse).
    * @return   KErrNone if query succeeded otherwise standard Symbian error
    *           code.
    */
    IMPORT_C TInt ConnectAccessoryL( RAccMonCapabilityArray& aCapabilityArray, TBool aSync );
    
    /**
    * Disconnects some accessory with some pre defined accessory type.
    *
    * @param    aAccDefintion is an definition of some accessory type
    *           that is defiend in AccMonConnectionTypes.h.
    * @param    aSync defines if the disconnect is done synchronously(ETrue) or
    *           asynchronously(EFalse).
    * @return   KErrNone if query succeeded otherwise standard Symbian error
    *           code
    */
    IMPORT_C TInt DisconnectAccessoryL( TEnumAccConnectType aAccDefintion, TBool aSync );
    
    /**
    * Disconnects some connected accessory.
    *
    * @param    aAccInfo is an instance of CAccMonitorInfo (information
    *           of some connected accessory). This information can be
    *           received through Accessory Monitoring API.
    * @param    aSync defines if the disconnect is done synchronously(ETrue) or
    *           asynchronously(EFalse).
    * @return   KErrNone if query succeeded otherwise standard Symbian error
    *           code.
    */
    IMPORT_C TInt DisconnectAccessoryL( CAccMonitorInfo* aAccInfo, TBool aSync );
    
    virtual ~CConnectionEmulator();
    
private:
    
    // C++ constructor - not exported;
    CConnectionEmulator();
    
    TInt SetPubSubL();
    
    TRequestStatus iStatus;
    
    RProperty iSubscribe;
    
    };

#endif      // ACCMONCONNECTIONEMULATOR_H
