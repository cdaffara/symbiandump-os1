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




#ifndef BTGPSNMEABUFFER_H
#define BTGPSNMEABUFFER_H

//  INCLUDES

#include <e32base.h>

// CONSTANTS
// Constants for NMEA buffer index, mean NMEA message
// has not been received by the client;
const TInt KBTGPSNmeaIndexNotSet = -1;


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class is used to send messages to connected BT device. 
*  It doesn't garentee the message can be received from connected
*  BT device. The message are buffered if necessary to prevent
*  jams on BT connection.
*  
*/
class CBTGPSNmeaBuffer: public CBase
    {
    public: 

        /**
        * Two-phase construction.
        * @param aSize The size of NMEA buffer
        */
        static CBTGPSNmeaBuffer * NewL(TInt aSize);

        /**
        * Destructor
        */
        virtual ~CBTGPSNmeaBuffer();

        /**
        * Add NMEA sentences to buffer
        */
        void AddSentences(const TDesC8& aNmea);

        /**
        * Read NMEA sentences from buffer. 
        * After the sentence is rturned, aBeginning
        * is updated to the new position. When
        * the reading reaches the end, aBeginning
        * will be set as KBTGPSNmeaIndexNotSet.
        * @return KErrNone if nmea sentences are successfully
        * retrieved. Otherwise, the end of the buffer
        * is met.
        */
        TInt ReadSentences(
            TDes8& aDest, 
            TInt& aBeginning) const;
            
        /**
        * Return current index
        */
        TInt CurrentIndex() const;
        
        /**
        * Reset the buffer
        */
        void ResetBuffer();
        
    private:

        /**
        * Second phase of the construction
        */
        void ConstructL(TInt aSize);

        /**
        * Private constructor
        */
        CBTGPSNmeaBuffer();
        
    private:
        //NMEA buffer
        TUint8* iBuffer;
        
        //Size of the buffer
        TInt iSize;

        //End of the FIFO buffer
        TInt iBottom;
    };
#endif
// End of File

