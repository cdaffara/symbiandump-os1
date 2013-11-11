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



#ifndef CPOSNMEACONTROLLER_H
#define CPOSNMEACONTROLLER_H

// INCLUDES
// #include <e32base.h>
#include <lbspositioninfo.h>
#include "EPos_CPosControllerBase.h"
#include "EPos_CPosSimulationPositioner.h"
#include "EPos_TNmeaSentenceParser.h"
#include "EPos_SimPsyConstants.h"

// CONSTANTS
// A limit to number of reads from a Nmea file
const TInt KMaxReads = 30;

// FORWARD DECLARATIONS
class CPosFileHandler;

/**
 *
 * A controller class that handles nmea requests.
 *
 */
class CPosNmeaController : public CPosControllerBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aNMEAFile The name of the NMEA file to read from.
        * @param aTimeRelativePlayback turns on/off time related playback feature
        */
        static CPosNmeaController* NewL(
        /* IN  */       const TDesC& aNMEAFile,
        /* IN  */       TBool aTimeRelativePlayback
        );

        /**
        * Destructor.
        */
        virtual ~CPosNmeaController();

    public: // From base classes

        /**
        * From CPosControllerBase. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status.
        * @param aSimPos Positioner which called this function
        */
        void NotifyPositionUpdate(
        /* IN/OUT */    TPositionInfoBase& aPosInfo,
        /* OUT    */    TRequestStatus& aStatus,
        /* IN     */	CPosSimulationPositioner& aSimPos
        );

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CPosNmeaController(TBool aTimeRelativePlayback);

        /**
        * EPOC constructor.
        *
        * @param aNMEAFile The name of the NMEA file to read from.
        */
        void ConstructL(
        /* IN  */       const TDesC& aNMEAFile
        );

        // Prohibit copy constructor
        CPosNmeaController( const CPosNmeaController& );
        // Prohibit assigment operator
        CPosNmeaController& operator= ( const CPosNmeaController& );

    private:  // New functions

        /**
        * Completes all requests with the specified error code
        * @param aErrorCode The error code - KErrNone if everything went OK.
        * If error code is something else, the position is invalid.
        */
        void CompleteRequest(TInt aErrorCode);

        /**
        * Reads an Nmea sentence from file.
        */
        void ReadSentence();

        /**
        * Parses an Nmea sentence in parser.
        */
        void ParseSentence();

        /**
        * Checks whether a position is partial or not.
        *
        * @param aPosInfo A position info object.
        * @return ETrue if position is partial, EFalse otherwise.
        */
        TBool IsPartialPosition(
        /* IN/OUT */    TPositionInfoBase& aPosInfo
        );

        /**
        * Checks if Simulation PSY should complete request or if parser
        * and position should be resetted and another try to get a
        * full fix should be started.
        */
        void CompleteOrRetry();
        /**
         * Kicks timer to trigger RunL in CPosControllerBase::iTimeBetweenReads
         */
        void StartTrpDelayTimer();

    private: // Functions from base classes

        /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        void RunL();

        /**
        * From CActive. Implements cancellation of an outstanding
        * request.
        */
        void DoCancel();

    private:    // Data
    	enum ENmeaControllerState
    		{
    		EIdle,
    		EWaiting,
    		EReadingSentence,
    		EParsingSentence,
    		
    		};
        //Buffer for storing last read sentence
        TBuf8<KMaxSentenceLength> iInputBuffer;

        //Pointer to the instance of CPosFileHandler object
        CPosFileHandler* iFileHandler;

        //Instance of TNmeaSentenceParser object
        TNmeaSentenceParser iParser;

        TBool iHasAlreadyReadFirstSentence;
        /**
         * Timer object used to provide time relative playback (TRP) feature.
         * When working in TRP mode it triggers reads from NMEA data file
         * every CPosControllerBase::iTimeBetweenReads
         */
        RTimer	iTimerTrigger;
        /**
         * Defines state of current object
         */
        ENmeaControllerState iState;
        
    };

#endif      // CPOSNMEACONTROLLER_H

// End of File
