/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class handles connect and disconnect to AccFw.
*
*/

#ifndef C_AUTOAUDIOASYSERVICE_H__
#define C_AUTOAUDIOASYSERVICE_H__

#include <accessoryserver.h>
#include <accessorycontrol.h>
#include <accpolgenericid.h>
#include <accpolgenericidarray.h>

#include "mautoaudioasypropertylistener.h"

class CAutoAudioAsyPropertyObserver;


/**
 * This class will notification for enabling/disabling RTP streaming channel
   and handles connect and disconnect to AccFW.
 */
NONSHARABLE_CLASS(CAutoAudioAsyService): public CActive,
										 public MAutoAudioAsyPropertyListener
    {
public:
    /**
     * Two-phased constructor
     */
    static CAutoAudioAsyService* NewL ();

    virtual ~CAutoAudioAsyService ();    

	/**
    * Starts ASY service
    */
	void StartL ();
	
    /**
     * Free resources on disconnect or on error
     */
    void FreeResources ();

    /**
     * @param aPS Handle to the property
     * @param aUid The Uid of the property
     * @param aKey The property key
     * @param aStatus The property change status
     * @return void
     */
    void PropertyChange ( RProperty& aPS, TUid aUid, TUint aKey, TInt aStatus );

protected:

    // CActive
    void DoCancel ();
    void RunL ();
    TInt RunError ( TInt aError );

private: //methods

    CAutoAudioAsyService ();

    void ConstructL ();
    void ConnectAccessoryL ();
    void AccessoryDisconnected ();

private: //data
    /**
     * State of the service class.
     */
    enum TServiceState
        {
        EServiceStateIdle,
        EServiceStateConnecting,
        EServiceStateConnected
        };

    /**
     * Handle to accessory server
     * Own.
     */
    RAccessoryServer  iAccessoryServer;

    /**
     * Handle to accessory control
     * Own.
     */
    RAccessoryControl iAccessoryControl;

    /**
     * Generic id header
     */
    TAccPolGIDHeader  iGenericIDHeader;

    /**
     * State of the service class
     */
    TServiceState iState;

    /**
     * Generic id
     * Own.
     */
    CAccConGenericID* iGenericId;

    /**
     * Observer that receives notifications about RTP Channel
     * Own.
     */
    CAutoAudioAsyPropertyObserver* iPropertyObserver;
    };

#endif // C_AUTOAUDIOASYSERVICE_H__
