/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMVIBRASTATUSOBSERVER_H
#define HWRMVIBRASTATUSOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <hwrmvibra.h>
#include <e32property.h>

// FORWARD DECLARATIONS
// None

/**
* Active object wrapper for Publish and Subscribe one-shot notification handling.
* Listens for changes in light status and notifies the client.
*
*/
NONSHARABLE_CLASS(CHWRMVibraStatusObserver) : public CActive
    {
    public:

        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMVibraStatusObserver.
        * A newly created instance immediately starts listening to vibra status changes.
        *
        * @param aCallback Pointer to a callback instance. Can be NULL.
        * @return          A pointer to a new instance of the CHWRMVibraStatusObserver class.
        */
        static CHWRMVibraStatusObserver* NewL(MHWRMVibraObserver* aCallback);

        /**
        * Destructor.
        */
        virtual ~CHWRMVibraStatusObserver();

        /**
        * This method retrieves the current vibra status. 
        *
        * @return TVibraStatus indicating the current vibra status
        */
        CHWRMVibra::TVibraStatus CurrentStatus() const;

    protected:

        // From CActive
        void RunL();
        void DoCancel();

    private:

        /**
        * Constructor
        *
        * @param aCallback Pointer to a callback instance.
        */
        CHWRMVibraStatusObserver(MHWRMVibraObserver* aCallback);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /*
        * Order notification
        */
        void OrderNotification();
       
    private:
        
        MHWRMVibraObserver* iCallback;         // not owned by this class. Can be NULL.
        RProperty iProperty;                   // P&S property
        CHWRMVibra::TVibraStatus iVibraStatus; // stores status
    };

#endif      // HWRMVIBRASTATUSOBSERVER_H

// End of File
