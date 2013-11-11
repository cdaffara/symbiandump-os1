/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_SIMULATION_PSY_POS_NMEA_FILEREADER_H 
#define __CT_LBS_SIMULATION_PSY_POS_NMEA_FILEREADER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// CONSTANTS
const TInt KLineReaderBufferSize = 256;
const TInt KLineReaderMaxLabelLength = 256;

// CLASS DECLARATION

/**
*  Reading from a text file
*
*/
class CT_LbsSimulationPSYPosNmeaFileReader : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsSimulationPSYPosNmeaFileReader* NewLC(
        /* IN  */    const TDesC& aFileName
        );
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsSimulationPSYPosNmeaFileReader* NewL(
        /* IN  */    const TDesC& aFileName
        );
        
        /**
        * Destructor.
        */
        ~CT_LbsSimulationPSYPosNmeaFileReader();

    public:

        TInt ReadFirstNmeaSentence(
        /* OUT */    TDes8& aInputBuffer
        );

        TInt ReadNmeaBurst(
        /* OUT */    TDes8& aBuffer,
        /* IN  */    const TDesC8& aDelimiter
        );

        // FrBo Changed access from public from private
        // Does this affect something?
        TInt ReadNmeaSentence(
        /* OUT */    TDes8& aInputBuffer
        );

    private:

        /**
        * C++ default constructor.
        */
    	CT_LbsSimulationPSYPosNmeaFileReader();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
        /* IN  */    const TDesC& aFileName
        );
	
    private:

        RFs                             iFileSession;
        RFile                           iFile;
        TBuf8<KLineReaderBufferSize>    iReadBuffer;
        TInt                            iReadBufferCursor;
        TBuf8<KLineReaderBufferSize>    iLastLine;
    };

#endif      // __CT_LBS_SIMULATION_PSY_POS_NMEA_FILEREADER_H
            
// End of File
