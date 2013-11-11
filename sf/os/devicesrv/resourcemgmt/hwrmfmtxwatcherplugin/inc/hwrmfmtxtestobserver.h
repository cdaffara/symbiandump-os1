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
* Description:  Usb observer definition for fmtx watcher plugin.
*
*/


#ifndef HWRMFMTXTESTOBSERVER_H
#define HWRMFMTXTESTOBSERVER_H

#include <e32property.h>
#include "hwrmfmtxwatcherplugin.h"
#include "hwrmfmtxconnobserver.h"

/**
* Active object for obtaining notification of changes in connections.
*
* @since S60 3.2
* @lib hwrmfmtxwatcherplugin.lib
*/
NONSHARABLE_CLASS(CHWRMFmtxTestObserver) : public CActive, 
                                           public MHWRMFmtxConnObserver
    {
public:

    /**
    * This is a two-phase constructor method that is used to create a new
    * instance for listening to the changes.
    *
    * @param aObserver A reference to an observer instance.
    * @return A pointer to a new instance of the CHWRMFmtxUsbObserver class.
    *
    * @leave One of the Symbian OS error codes.
    */
    static CHWRMFmtxTestObserver* NewL( MHWRMFmtxConnObserverCallback& aObserver );
    
    /**
    * Destructor.
    */
    ~CHWRMFmtxTestObserver();
    
    // From MHWRMFmtxConnObserver
    TBool GetStatusL();
    void StartObservingL();
    void StopObserving();

private:

    /**
    * Constructor.
    *
    * @param aObserver A reference to an observer instance.
    */
    CHWRMFmtxTestObserver( MHWRMFmtxConnObserverCallback& aObserver );
    
    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();
    
    /**
    * Orders notification.
    */
    void OrderNotification();

protected:

    // from CActive
    void RunL();
    void DoCancel();

private:

    /**
    * Handle to test PubSub key.
    */    
    RProperty iTestPubSubKey;

    /**
    * Response callback pointer.
    */    
    MHWRMFmtxConnObserverCallback& iCallback;
    
    /**
    * Stores USB connection status, ETrue if connected.
    * Used for filtering out excess USB configuration change notifications.
    */    
    TBool iConnected;
    };


#endif      // HWRMFMTXTESTOBSERVER_H
            
// End of File
