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

#ifndef HWRMPSCHANGEOBSERVER_H
#define HWRMPSCHANGEOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
// None

/**
*  An interface the developer implements for handling notification events. 
*
*/
class MHWRMPSChangeObserver
    {    
    public:
        
        /** 
        * Called when the PS value changes.
        * Note that if the value changes
        * very rapidly, some state transitions might be missed.
        * It is however guaranteed that latest state is always obtained.
        *
        * @param aUid Uid of the changed property
        * @param aKey Property identifier
        * @param aValue Indicates latest value
		*/
        virtual void PSValueChanged(const TUid& aUid, TUint32 aKey, TInt aValue) = 0;
	};

/**
* Active object wrapper for Publish and Subscribe one-shot notification handling.
* Listens for changes in light status and notifies the client.
*
*/
class CHWRMPSChangeObserver : public CActive
    {
    public:

        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMPSChangeObserver.
        * A newly created instance immediately starts listening to PS value changes.
        *
        * @param aCallback Reference to a callback instance.
        * @param aUid Uid of the changed property
        * @param aKey Property identifier
        * @return          A pointer to a new instance of the CHWRMPSChangeObserver class.
        */
        static CHWRMPSChangeObserver* NewL(MHWRMPSChangeObserver& aCallback,
                                      const TUid& aUid,
                                      TUint32 aKey);

        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMPSChangeObserver.
        * A newly created instance immediately starts listening to PS value changes.
        * Leaves the constructed instance to cleanup stack. 
        *
        * @param aCallback Reference to a callback instance.
        * @param aUid Uid of the changed property
        * @param aKey Property identifier
        * @return          A pointer to a new instance of the CHWRMPSChangeObserver class.
        */
        static CHWRMPSChangeObserver* NewLC(MHWRMPSChangeObserver& aCallback,
                                      const TUid& aUid,
                                      TUint32 aKey);

        /**
        * Destructor.
        */
        virtual ~CHWRMPSChangeObserver();

    protected:

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private:

        /**
        * Constructor
        *
        * @param aCallback Reference to a callback instance.
        * @param aUid Uid of the changed property
        * @param aKey Property identifier
        */
        CHWRMPSChangeObserver(MHWRMPSChangeObserver& aCallback,
                              const TUid& aUid,
                              TUint32 aKey);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /*
        * Order notification
        */
        void OrderNotification();
       
    private:
        
        MHWRMPSChangeObserver& iCallback;       // not owned by this class
        RProperty              iProperty;       // P&S property
        TUid                   iUid;            // Uid of the property
        TUint32                iKey;            // Property identifier

    };

#endif      // HWRMVIBRASTATUSOBSERVER_H

// End of File
