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
* Description:  Fmtx watcher plugin definition.
*
*/


#ifndef HWRMFMTXWATCHERPLUGIN_H_
#define HWRMFMTXWATCHERPLUGIN_H_

#include <hwrmpluginservice.h>
#include <babitflags.h>
#include "hwrmfmtxconnobserver.h"
#include "hwrmfmtxplugintimer.h"

/**
* FM Tx watcher plugin implementation.
*
* @since S60 3.2
* @lib hwrmfmtxwatcherplugin.lib
*/
class CHWRMFmtxWatcherPlugin : public CHWRMPluginService,
                               public MHWRMFmtxPluginTimerCallback,
                               public MHWRMFmtxConnObserver,
                               public MHWRMFmtxConnObserverCallback
    {
public:

    /**
    * This is a two-phase constructor method that is used to create a new
    * instance of fmtx watcher plugin.
    *
	* @leave One of the Symbian OS error codes.
    */
    static CHWRMFmtxWatcherPlugin* NewL();

    /**
    * Destructor.
    */
    ~CHWRMFmtxWatcherPlugin();

private:
    // from CHWRMPluginService
    /**
    * Method to invoke a particular command in the plugin
    * @param aCommandId Command ID
    * @param aTransId   Transaction ID
    * @param aData      Data associated with command
    */
    virtual void ProcessCommandL(const TInt aCommandId,
                                 const TUint8 aTransId,
                                 TDesC8& aData);

    /**
    * Method to cancel a particular command
    * @param aTransId   Transaction ID
    * @param aCommandId Command ID to optionally double check with the
    *                   transaction ID
    */
    virtual void CancelCommandL(const TUint8 aTransId,
                                const TInt aCommandId);

private:

    // From MPluginTimerCallback
    void GenericTimerFired(const TInt aCommandId,
                           const TUint8 aTransId,
                           TInt aRetVal);

    // From MHWRMFmtxConnObserver
    TBool GetStatusL();
    void StartObservingL();
    void StopObserving();

    // From MHWRMFmtxConnObserverCallback
    void HandleConnectionChange( TFmtxWatcherObserverTypes aType, TBool aConnected );

private:

    /**
    * C++ default constructor.
    *
    */
    CHWRMFmtxWatcherPlugin();

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();

private:

    /**
    * Connection observers.
    */
	RPointerArray<MHWRMFmtxConnObserver> iObservers;

    /**
    * Timer array for command responses.
    */
    RPointerArray<CHWRMFmtxPluginTimer> iTimers;
    
    /**
    * Connection status.
    */
    TBitFlags iConnected;
    };

#endif // HWRMFMTXWATCHERPLUGIN_H_
