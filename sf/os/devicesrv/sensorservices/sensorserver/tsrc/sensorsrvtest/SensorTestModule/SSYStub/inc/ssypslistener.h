/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listener for PS key changes
*
*/


#ifndef SSYPSLISTENER_H
#define SSYPSLISTENER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <e32property.h>

/**
* A callback interface for handling PS notifications
*
* @since S60 5.0
*/
class MSsyPsObserver
    {
    public:

        /** 
        * Called when the subscribed value is set.
        * All changes are not guarateed to be notified but latest value will
        * always be notified.
        *
        * @since S60 5.0
        * @param aCategory The category of ps value that was set
        * @param aKey The key id of ps value that was set
        * @param aValue The value that was set
        */
        virtual void PsValueSet( TUid aCategory, 
                                 TUint aKey,
                                 TInt aValue ) = 0;
    };
    
/**
* Listens for specified PS key value and notifies observer
* when the key is set.
*
* @since S60 5.0
*/
class CSsyPsListener : public CActive
    {
    public:
        
        /**
        * Two-phased constructor.
        *
        * @since S60 5.0
        * @param aObserver Callback object for informing about PS value sets
        * @param aCategory The category of ps value that was set
        * @param aKey The key id of ps value that was set
        */
        static CSsyPsListener* NewL( MSsyPsObserver& aObserver,
                                     TUid aCategory, 
                                     TUint aKey );
        /**
        * Destructor.
        */
        virtual ~CSsyPsListener();

    protected:

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
        
        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
        CSsyPsListener(MSsyPsObserver& aObserver,
                       TUid aCategory, 
                       TUint aKey);

        /**
        * 2nd phase of construction.
        *
        * @since S60 5.0
        */
        void ConstructL();

    private: // Member variables
    
        /**
        * Observer for sets
        * Not own.
        */
        MSsyPsObserver& iObserver;
        
        /**
        * Property used for listening
        */
        RProperty iProperty;
        
        /**
        * Category of the listened key
        */
        TUid iCategory;
        
        /**
        * The listened key identifier
        */
        TUint iKey;
        
        /**
        * The listened key value
        */
        TInt iValue;

    };



#endif // SSYPSLISTENER_H
