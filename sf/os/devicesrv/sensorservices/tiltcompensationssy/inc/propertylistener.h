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
* Description:  TTCStateTiltListenData class declaration.
*
*/



#ifndef PROPERTYLISTENER_H
#define PROPERTYLISTENER_H

// INCLUDES
#include <e32property.h>

// CLASS DECLARATION

/**
*  Listen data state for tilt channel.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CProprertyListener ): public CActive
    {
    
    public:
    
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltListenData
        */
        static CProprertyListener* NewL( TInt& aX, TInt& aY, TInt& aZ, TInt& aAccX, TInt& aAccY, TInt& aAccZ, TInt& aFilterAngle, TInt& aFilterAttenation, TInt& aAccAverageSize, TInt& aMagAverageSize, TInt& aFilterDivisor );
        
        CProprertyListener( TInt& aX, TInt& aY, TInt& aZ, TInt& aAccX, TInt& aAccY, TInt& aAccZ, TInt& aFilterAngle, TInt& aFilterAttenation, TInt& aAccAverageSize, TInt& aMagAverageSize, TInt& aFilterDivisor );
        
        ~CProprertyListener();
        
    protected:
    
        // From CActive
        void DoCancel();
        
        // From CActive
        void RunL();
        
        // From CActive
        TInt RunError( TInt aError );
        
    private: // Data
        
        // Property for Pub/Sub
        RProperty iSubscribe;
        
        TInt& iDataXAxis;
        
        TInt& iDataYAxis;
        
        TInt& iDataZAxis;
        
        TInt& iDataXAccAxis;
        
        TInt& iDataYAccAxis;
        
        TInt& iDataZAccAxis;
        
        TInt& iFilterAngle;
        
        TInt& iFilterAttenation;
        
        TInt& iAccAverageSize;
        
        TInt& iMagAverageSize;
        
        TInt& iFilterDivisor;
    };

#endif // PROPERTYLISTENER_H

// End of File
