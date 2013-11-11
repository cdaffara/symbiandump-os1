/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_STUB_NMEA_COMMUNICATOR_H
#define __CT_LBS_STUB_NMEA_COMMUNICATOR_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CPositionInfo;

// CLASS DECLARATION

/** 
 * A controller class that handles nmea requests. 
 *
 */
class CT_LbsStubNmeaCommunicator : public CActive
    {
	public:  // Destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsStubNmeaCommunicator* NewL();        

        /**
        * Two-phased constructor.
        */

        static CT_LbsStubNmeaCommunicator* NewLC();        
        /**
        * Used instead of destructor.
        */

        /**
        * Destructor.
        */
        virtual ~CT_LbsStubNmeaCommunicator();		

    public: // New functions                

        /**
        * Requests Nmea data asynchronously.
        *
        * @param aStatus status of the request
        * @param aNmeaData text buffer containing raw Nmea data
        * @param aPositionInfo reference to a position pointer. This
        *                      pointer must be in scope until the request
        *                      has completed.
        */
        void GetNmeaData(
            /* IN  */   TRequestStatus* aStatus,
            /* OUT */   HBufC8*&        aNmeaData,
            /* OUT */   CPositionInfo*& aPosition,
            /* IN  */   TUid            aUid
            ) ;

        /**
        * Cancels request of Nmea data.
        * @param aStatus The status of the request.
        */
        void CancelGetNmeaData(
            /* IN  */   TRequestStatus* aStatus
            );

    private:  // Constructors and destructor        

        /**
        * C++ default constructor.
        */
        CT_LbsStubNmeaCommunicator();

        /**
        * EPOC constructor.
        */
        void ConstructL();

        // Prohibit copy constructor
        CT_LbsStubNmeaCommunicator( const CT_LbsStubNmeaCommunicator& );
        // Prohibit assigment operator
        CT_LbsStubNmeaCommunicator& operator= ( const CT_LbsStubNmeaCommunicator& );

    private:  // New functions        

    private: // Functions from base classes

        /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        void RunL();

        /**
        * From CActive. Implements cancellation of an outstanding
        * request
        */
        void DoCancel();

    private:    // Data		
        RTimer          		  iTimer;
        TInt                      iReferenceCounter;                
        TInt iTimeout;
        TInt iRequestCounter;
                        
        TRequestStatus*           iRequestStatus;
        CPositionInfo**           iPositionRef;
        HBufC8**                  iNmeaDataRef;
                
        HBufC8*                   iNmeaData;                                     

    };

#endif      // __CT_LBS_STUB_NMEA_COMMUNICATOR_H

// End of File




