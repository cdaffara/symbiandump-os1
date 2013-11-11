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



#ifndef TNMEASENTENCEPARSER_H
#define TNMEASENTENCEPARSER_H

// INCLUDES
#include <e32math.h>
#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include <lbserrors.h>
#include "EPos_TDesTokeniser.h"

// CONSTANTS
typedef TInt TNmeaSentence;

const TInt KPosNotParsed = 0x0;
const TInt KPosGGAParsed = 0x1;
const TInt KPosGSAParsed = 0x2;
const TInt KPosRMCParsed = 0x4;
const TInt KPosGLLParsed = 0x8;

const TInt KPosErrorAlreadyParsed = 2000;
const TInt KPosErrorNotValidNMEASentence = KErrNone;
const TInt KPosAllParsed = KPosGGAParsed |
                           KPosGSAParsed |
                           KPosRMCParsed |
                           KPosGLLParsed;

// CLASS DECLARATION

/**
 * Parses NMEA sentences.
 */
class TNmeaSentenceParser
    {
    public:  // Constructors and destructor

        /**
         * Default C++ constructor.
         */
        TNmeaSentenceParser();

    public: // New functions

        /**
         * Parses a sentence and places result in the attached position.
         * If no position is attached a position will be allocated.
         *
         * @param  aSentence The sentence to parse
         * @param  aPosition Position data will be put here
         * @param  aConst When set to ETrue doesn't change the state of the object.
         * 			Allows getting the same information as for previous call to this function
         * 			without changing the state. Is used to feed more then one TPositionInfo data
         * 			in loop with the same data.
         * @return an error code
         */
        TInt ParseSentence(
        /* IN  */       const TDesC8&       aSentence,
        /* IN OUT */    TPositionInfo&      aPosInfo, 
        /* IN  */       TBool aConst=EFalse
        );

        /**
         * Returns the progress made.
         *
         * @return TInt with bits set to represent the sentences that have
         * been parsed.
         */
        TInt Progress();

        /**
         * Resets the internal state of the parser.
         */
        void Reset();

    protected:  // New functions

        /**
         * Returns the type of an NMEA sentence.
         *
         * @param  aSentence The sentence to parse.
         * @return The sentence type.
         */
        TNmeaSentence SentenceType(const TDesC8& aSentence);

        /**
         * Parses a GGA NMEA sentence.
         *
         * @param  aSentence The sentence to parse
         * @param  aPositionInfo An object that can store position data.
         * @return KErrNone if parsed successfully.
         */
        TInt ParseGGA(
        /* IN  */       const TDesC8&   aSentence,
        /* IN OUT */    TPositionInfo&  aPositionInfo
        );

        /**
         * Parses a RMC NMEA sentence.
         *
         * @param  aSentence The sentence to parse.
         * @param  aPositionInfo An object that can store position data.
         * @return KErrNone if parsed successfully.
         */
        TInt ParseRMC(
        /* IN  */       const TDesC8&   aSentence,
        /* IN OUT */    TPositionInfo&  aPositionInfo
        );

        /**
         * Parses a GSA NMEA sentence.
         *
         * @param aSentence The sentence to parse.
         * @param aPositionInfo An object that can store GPS specific
         * position data
         * @return KErrNone if parsed successfully.
         */
        TInt ParseGSA(
        /* IN  */       const TDesC8&   aSentence,
        /* IN OUT */    TPositionInfo&  aPositionInfo
        );

        /**
         * Parses a GSV NMEA sentence.
         *
         * @param aSentence The sentence to parse.
         * @param aPositionInfo An object that can store GPS specific
         * position data
         * @param  aConst When set to ETrue doesn't change the state of the object.
         * 			Allows getting the same information as for previous call to this function
         * 			without changing the state. Is used to feed more then one TPositionInfo data
         * 			in loop with the same data.
         * @return KErrNone if parsed successfully.
         */
        TInt ParseGSV(
        /* IN  */       const TDesC8&   aSentence,
        /* IN OUT */    TPositionInfo&  aPositionInfo,
        /* IN  */       TBool aConst=EFalse
        );

        /**
         * Parses a PGRMM NMEA sentence.
         *
         * @param aSentence The sentence to parse.
         * @param aPositionInfo An object that can store position data.
         * @return KErrNone if parsed successfully.
         */
        TInt ParsePGRMM(
        /* IN  */       const TDesC8&   aSentence,
        /* IN OUT */    TPositionInfo&  aPositionInfo
        );

    private:
    	/**
		 * Adding NMEA sentence to position info object 
		 *
		 * @param aPositionGenericInfo An object that can store position data.
		 * @param aSentence The sentence to parse.
		 * @param  aConst When set to ETrue doesn't change the state of the object.
         * 			Allows getting the same information as for previous call to this function
         * 			without changing the state. Is used to feed more then one TPositionInfo data
         * 			in loop with the same data.
		 * @return KErrNone if parsed successfully.
		 */
        TInt AddNmeaSentence(
            HPositionGenericInfo& aPositionGenericInfo,
            const TDesC8& aSentence, TBool aConst=EFalse);

        void ToCoordinate(
            const TDesC8& aDes,
            const TDesC8& aDir,
            TReal& aCoordinate);

        TInt DesToInt(const TPtrC8& aDes);

        TReal DesToReal(const TPtrC8& aDes);

        TReal32 DesToFloat(const TPtrC8& aDes);

        TBool VerifyChecksum(const TDesC8& aSentence);

        TUint8 CharToNibble(const TUint8 aChar);

        TInt GetSatelliteListIndex(
            const TPositionSatelliteInfo& aSatInfo,
            TInt aPrnNumber,
            TInt& aIndex
        );

        TInt UpdateSatDataInList(
            TPositionSatelliteInfo& aSatInfo,
            TSatelliteData aSatData,
            TInt aIndex
        );

    private:    // Data
        TInt                            iProgress;
        TInt                            iGSVIndex;
        TUint8                          iNumNmeaSentences;
    };

#endif      // TNMEASENTENCEPARSER_H

// End of File
