/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMEXTENDEDLIGHTSTATUSOBSERVER_H
#define HWRMEXTENDEDLIGHTSTATUSOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <hwrm/hwrmextendedlight.h>
#include <e32property.h>
#include "HWRMCmn.h"

// FORWARD DECLARATIONS
// None

/**
* Active object wrapper for Publish and Subscribe one-shot notification handling.
* Listens for changes in light status and notifies the client.
*
*/
NONSHARABLE_CLASS(CHWRMExtendedLightStatusObserver) : public CActive
    {
    public:

        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMExtendedLightStatusObserver.
        * A newly created instance immediately starts listening to light status changes.
        *
        * @param aCallback Pointer to a callback instance. Can be NULL.
        * @param aSupportedTargets Mask of supported targets.
        * @return A pointer to a new instance of the CHWRMExtendedLightStatusObserver class.
        */
        static CHWRMExtendedLightStatusObserver* NewL(MHWRMExtendedLightObserver* aCallback,
                                                      TInt aSupportedTargets);

        /**
        * Destructor.
        */
        virtual ~CHWRMExtendedLightStatusObserver();
        
        /**
        * This method retrieves the current light status. 
        *
        * @return TLightStatus indicating the current light status
        */
        CHWRMExtendedLight::TLightStatus CurrentStatus(TInt aTarget) const;

    protected:

        // From CActive
        void RunL();
        void DoCancel();

    private:

        /**
        * Constructor
        *
        * @param aCallback Pointer to a callback instance.
        * @param aSupportedTargets Mask of supported targets.
        */
        CHWRMExtendedLightStatusObserver(MHWRMExtendedLightObserver* aCallback,
                                         TInt aSupportedTargets);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /*
        * Order notification
        */
        void OrderNotification();

        /**
        * Notifies the callback about value changes.
        *
        * @param aStatusList New status values obtained from P&S
        */
        void NotifyCallback( RLightStatusArray& aStatusList  );

        /**
        * Finds item from iLastStatusValue by target.
        */
        static TInt FindByTarget( const TInt* aTarget, 
                                  const THWRMStatusInfo& aItem );

        /**
        * Initializes RLightStatusArray.
        */
        TInt InitializeStatusArray( RLightStatusArray& aArray ) const;
        
    private:
        
        MHWRMExtendedLightObserver* iCallback; // Not owned by this class. Can be NULL.
        RProperty iProperty;           // P&S property
        TInt iSupportedTargets;        // Mask of supported targets
        RLightStatusArray iLastStatusValue; // Keeps track of last received status value
        RLightStatusArray iStatusUnknownList; // All targets status unknown, RunError() uses.
    };

#endif      // HWRMLIGHTSTATUSOBSERVER_H

// End of File
