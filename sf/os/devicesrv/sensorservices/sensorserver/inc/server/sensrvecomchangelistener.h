/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listener for new ECom plugins
*
*/


#ifndef SENSRVECOMCHANGELISTENER_H
#define SENSRVECOMCHANGELISTENER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "sensrvecomchangelistenertimer.h"

/**
* A callback interface for handling ecom notification and 
* timer expiration events.
*
* @since S60 5.0
*/
class MSensrvEcomChangeObserver
    {
    public:

        /** 
        * Called when the ecom has been scanned
        *
        * @since S60 5.0
        * @param aImplementationInfos Found SSY implementation infos.
        *        The observer is responsible for cleanup of this array,
        *        if change was not blocked (return ETrue).
        * @return EFalse if changing is currently blocked. Try again later.
        */
        virtual TBool EcomChanged( RImplInfoPtrArray* aImplementationInfos ) = 0;
    };
    
/**
* Listens for changes in ECom registration files and rescans 
* for SSYs if changes are detected and notifies proxymanager.
*
* @since S60 5.0
*/
class CSensrvEcomChangeListener : public CActive
    {
    public:
        
        /**
        * Two-phased constructor.
        *
        * @since S60 5.0
        * @param aObserver Callback object for informing about ecom changes
        * @param aTimerId Timer identifier used in timer fired callback.
        */
        static CSensrvEcomChangeListener* NewL( MSensrvEcomChangeObserver& aObserver );
        /**
        * Destructor.
        */
        virtual ~CSensrvEcomChangeListener();

        void TimerCallBack();
    protected:

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
        void CheckEcomChanges();
        
        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
        CSensrvEcomChangeListener(MSensrvEcomChangeObserver& aObserver);

        /**
        * 2nd phase of construction.
        *
        * @since S60 5.0
        */
        void ConstructL();

    private: // Member variables
    
        /**
        * Observer for changes
        * Not own.
        */
        MSensrvEcomChangeObserver& iObserver;
        
        /**
        * ECom session pointer
        * Not own.
        */
        REComSession* iEcom;
        
        /**
        * Scan timer used to delay the scan
        */
        CSensrvEcomChangeListenerTimer* iTimer;
        
    };



#endif // SENSRVECOMCHANGELISTENER_H
