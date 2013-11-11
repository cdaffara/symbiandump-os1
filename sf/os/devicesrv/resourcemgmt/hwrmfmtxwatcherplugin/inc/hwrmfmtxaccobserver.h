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
* Description:  Accessory observer implementation for fmtx watcher plugin.
*
*/


#ifndef HWRMFMTXACCOBSERVER_H
#define HWRMFMTXACCOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "hwrmfmtxconnobserver.h"

// CLASS DECLARATIONS
/**
* Active object for obtaining notification of changes in Accessory connections.
*
* @since S60 3.2
* @lib fmtxwatcherplugin.lib
*/
NONSHARABLE_CLASS(CHWRMFmtxAccObserver) : public CActive, public MHWRMFmtxConnObserver
	{
public: // From MFmtxConnObserver

    /**
    * This is a two-phase constructor method that is used to create a new
    * instance for listening to the changes in Accessory connections.
    *
    * @param aObserver A reference to an observer instance.
    * @return A pointer to a new instance of the CAccObserver class.
    *
    * @leave One of the Symbian OS error codes.
    */
    static CHWRMFmtxAccObserver* NewL( MHWRMFmtxConnObserverCallback& aObserver );
    
    /**
    * Destructor.
    */
    ~CHWRMFmtxAccObserver();
    
    
    // form MHWRMFmtxConnObserver
    TBool GetStatusL();
    void StartObservingL();
    void StopObserving();
	
private:

    /**
    * C++ default constructor.
    *
    * @param aObserver A reference to an observer instance.
    */
    CHWRMFmtxAccObserver( MHWRMFmtxConnObserverCallback& aObserver );
    
    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();
    
    /**
    * Orders notification for wired accessory connection status change.
    */
    void OrderAccessoryNotification();

protected:
    
    // from CActive
    void RunL();
    void DoCancel();

private:

    /**
    * Publish&Subscribe key session.     
    */
    RProperty iProperty;
    
    /**
    * Response callback pointer.
    */    
    MHWRMFmtxConnObserverCallback& iCallback;
    
    /**
    * Connection status.
    * Used for filtering out audio routing change notifications.
    */ 
    TBool iConnected;
	};


#endif // HWRMFMTXACCOBSERVER_H
            
// End of File
