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



#ifndef __CT_LBS_SIMULATION_PSY_POS_TP115_H__
#define __CT_LBS_SIMULATION_PSY_POS_TP115_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// CLASS DECLARATION

/**
*  Test procedure 115
*/
class CT_LbsSimulationPSYPosTp115 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
    	CT_LbsSimulationPSYPosTp115(CT_LbsServer& aParent, TInt aIndex);
        
        /**
        * Destructor.
        */
        ~CT_LbsSimulationPSYPosTp115();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();

    private: // New Functions

        /**
        * Gets the name of an individual test index
        * @param aIndex Indicates which test index name that is asked for.
        * @param aName The name of the test index.
        */
        void GetIndexName(TInt aIndex, TDes& aName) const;

		void TestSimulationNmeaReaderL(const TDesC& aNMEASource);
		
        void TestPowerUpTimeL();
        void TestTimeToFixL();
        void TestDeterministicFailureL();
        void TestRandomFailureL();
        void TestAltitudeL();
        void TestLongLatitudeL();
        void TestMovementL();
        void TestBadInputFileL();
		void CheckBadInputHelperL(const TDesC& aFrom);
		void TestCorrectFixL();
		void TestCorrectTimingAfterCancelL();
		void CheckPositionL(TPositionInfo& aPosition, TInt aFixNumber);
		TReal ToCoordinate( const TDesC8& aDes, const TDesC8& aDir);
		TInt DesToInt(const TPtrC8& aDes);
		TReal DesToReal(const TPtrC8& aDes);

        void Extended1TestSimulationNmeaReaderL(const TDesC& aNMEASource);
        void Extended2TestSimulationNmeaReaderL(const TDesC& aNMEASource);
        void ReplaceSwitchInTrapL(TInt aIndex);

    private:

        TBuf<100> iMsg;

		// EPos server instance
		RPositionServer	iPosServer;
        
		// Positioner instance
		RPositioner iPositioner;
		TInt 		iIndex;

    };

#endif      // __CT_LBS_SIMULATION_PSY_POS_TP115_H__
            
// End of File
