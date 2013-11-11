/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     CHeadsetStatusObserver functionality.
*
*/


#ifndef __HEADSETOBSERVER_H__
#define __HEADSETOBSERVER_H__

#ifdef __ACCESSORY_FW

// INCLUDES
#include <e32base.h>
#include <AccPolAccessoryMode.h>

// FORWARD DECLARATIONS
class MHeadsetStatusObserver;
class RAccessoryMode;

// CLASS DECLARATION

/**
*   Active object for receiving asynchronous requests
*   from accessory mode change events.
*   Only enabled with __ACCESSORY_FW flag set.
*/
NONSHARABLE_CLASS( CHeadsetStatusObserver ): public CActive
    {
    public:     // Constructors & destructors
       
        /**
        * Destructor 
        */
        ~CHeadsetStatusObserver();

        /**
        * Symbian two phased constructor.
        */
        static CHeadsetStatusObserver* NewL( RAccessoryMode& aAccMode );

    public:     // New methods
    
        /**
        * Sets observer through headset status notifications are received.
        * @param    aObserver: Observer
        * @return   None
        */
        void SetObserver( MHeadsetStatusObserver* aObserver );
        
        /**
        * Activates observing through RAccessoryMode.
        * Method will panic if already active.
        * @param    None
        * @return   None
        */
        void StartObserving();
        
    private:  // From base classes
    
        // From CActive
        void RunL();
        
        // From CActive
        void DoCancel();
    
        // From CActive
        TInt RunError( TInt aError );
        
    private:    // New methods
    
        // Notifies observer from headset status change
        void NotifyObserverL();
        
        // Notifies observer from error
        void NotifyObserverError( TInt aError );
                
    private: 
        
        /**
        * Default Constructor 
        */
        CHeadsetStatusObserver( RAccessoryMode& aAccMode );
        
        /**
        * Symbian OS 2nd phase constructor 
        */
        void ConstructL();  
               
    private:
    
        // headset status observer, not owned
        MHeadsetStatusObserver* iObserver;
        
        // reference to RAccessoryMode, not owned
        RAccessoryMode& iAccMode;
        
        // accessory mode
        TAccPolAccessoryMode iAccPolMode;
    };
    
#endif //__ACCESSORY_FW

#endif //__HEADSETCONTAINER_H__

// End of File
