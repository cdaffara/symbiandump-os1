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



#ifndef __CT_LBS_STUB_NMEAREADER_H__
#define __CT_LBS_STUB_NMEAREADER_H__

//  INCLUDES

#include <EPos_CPosNmeaReaderSubSession.h>
#include <EPos_CPosNmeaReader.h>

// FORWARD DECLARATIONS
class CT_LbsStubNmeaCommunicator;

// CLASS DECLARATION
/**
*  Used to get position info from a file containing nmea data
*/
class CT_LbsStubNmeaReader : public CPosNmeaReader
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsStubNmeaReader* NewL();

        /**
        * Destructor.
        */
        ~CT_LbsStubNmeaReader();

    public:  // Functions from base classes

        /**
        * From CPosNmeaReader. Requests Nmea data asynchronously.
        *
        * @param aStatus status of the request
        * @param aNmeaData text buffer containing raw Nmea data
        * @param aPositionInfo reference to a position pointer. This
        *                      pointer must be in scope until the request
        *                      has completed.
        */
        void GetNmeaData(
            /* IN OUT */   TRequestStatus& aStatus,
            /* IN  */   TInt            aQueryId,
            /* OUT */   HBufC8*&        aNmeaData,
            /* OUT */   CPositionInfo*& aPositionInfo
            ) ;

        /**
        * From CPosNmeaReader. Cancels request of Nmea data.
        */
        void CancelGetNmeaData();

    private:

        /**
        * C++ default constructor.
        */
        CT_LbsStubNmeaReader();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CT_LbsStubNmeaReader( const CT_LbsStubNmeaReader& );

        // Prohibit assigment operator
        CT_LbsStubNmeaReader& operator= (const CT_LbsStubNmeaReader&);
                    
    private:    // Data
        TRequestStatus*         iStatusPtr;
        CT_LbsStubNmeaCommunicator*  iCommunicator;
    };

#endif      // __CT_LBS_STUB_NMEAREADER_H__

// End of File
