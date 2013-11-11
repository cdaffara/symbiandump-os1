/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

 
#ifndef TCSTATETILTLISTENDATA_H
#define TCSTATETILTLISTENDATA_H

// INCLUDES
#include <tiltcompensation.h>
#include <e32property.h>

#include "tcstatetilt.h"
#include "tcsendeventtimer.h"
#include "tccenrepobserver.h"

// FORWARD DECLARATIONS
class CTCSendEventTimer;

// CLASS DECLARATION
class TTiltXYZValue
   {
public:
   inline TTiltXYZValue() {};
   inline TTiltXYZValue( TInt16 aX, TInt16 aY, TInt16 aZ) :  iX(aX), iY(aY), iZ(aZ) {};
   TInt16 iX;
   TInt16 iY;
   TInt16 iZ;
   };

/**
*  Listen data state for tilt channel.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCStateTiltListenData ): public CTCStateTilt, 
                                             public MSendDataCallBack,
                                             public MTCCenrepObserver
    {
    
    //friend class CTCSendEventTimer;
    
    private: // Data types
    
        // Bit flags to indicate data compensation state
        enum TTiltDataBit
            {
            ENone               = 0x0,
            EAccelerometerData  = 0x1,
            EMagnetometerData   = 0x2,
            EForceBufferFull    = 0x4
            };
    
    public:
    
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltListenData
        */
        static CTCStateTiltListenData* NewL( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltListenData
        */
        static CTCStateTiltListenData* NewLC( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
        ~CTCStateTiltListenData();
        
        // @see MSendDataCallBack
        void SendDataAfterTimer();
        
    public:
        
        // From MTCCenrepObserver
        void CenrepValueChanged( TUint32 aKey, CRepository& aRepository );
        
    private: // From base classes
    
        // @see TTCStateBase
        void HandleEventL( TTCEventId aId, TTCEvent* aEvent );
    
        // @see TTCStateBase
        void HandleTransactionCompletedL( TTCTransactionId aTransactionId, TInt aError );
        
        // @see TTCStateBase
        void HandleStateEntryL();

        // @see TTCStateBase
        void HandleStateExitL();

    private:
    
        /**
        * C++ constructor
        *
        * @param aStatePool
        * @return aTransactionHandler
        */
        CTCStateTiltListenData( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
        /**
        * 2nd phase of construction
        */
        void ConstructL();
    
    private: // New methods
    
        // Compensate tilt angle
        TBool CalculateTilt();
        
        // Flush buffer and change state
        void CompleteEventL();
        
        // Reset internal data
        void ResetState( TBool aWriteCountIncluded );
        
        // Get updated cenrep value
        void GetCenrepValue( CRepository& aCenRep, 
            TUint32 aKey, TInt aIndex );

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
        
        /** Tilt input */
        TTiltCompensationInput iInput;
        
        /** Write count */
        TInt iWriteCount;
        
        /** Current sampling time */
        TTime iTime;
        
        // The data rate that is used
        TInt iHzSamplingRate;
        
        // Pointer to the timer
        CTCSendEventTimer* iTimerToSend;
        
        // The requested amount of data
        TInt iRequestedDataCount;
        
        TInt16 iPreviousTheta;
        // Tilt compensation parameters
        RParamsArray iParamsArray;
        // Handle to the compensation library
        RLibrary iTiltLib;
        // A function pointer to compensation function
        // from the compensation library.
        CompensateFuncWithParams iCompensationFunc;

        // Observer for compensation parameter changes. Owned.
        CTCCenrepObserver* iCenrepObserver;       
        
        // Arrays for calculating average values for accelerometer and magnetometer time windows
        RArray<TTiltXYZValue> iArrayMag;
        RArray<TTiltXYZValue> iArrayAcc;
        
        // Sizes of accelerometer and magnetometer time windows
        TInt iSizeOfAccArray;
        TInt iSizeOfMagArray;
                
    };

#endif // TCSTATETILTLISTENDATA_H

// End of File
