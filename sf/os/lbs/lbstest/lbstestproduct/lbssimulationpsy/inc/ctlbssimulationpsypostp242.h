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



#ifndef __CT_LBS_SIMULATION_PSY_POS_TP242_H__ 
#define __CT_LBS_SIMULATION_PSY_POS_TP242_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CT_LbsSimulationPSYPosTp242 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
    	CT_LbsSimulationPSYPosTp242(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsSimulationPSYPosTp242();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

		/**
		* Allways called when shut down test 
		*/
		void CloseTest();

    private:
        void RequestL(TPositionInfoBase& aPositionInfo);

        void CheckTPositionInfo(const TPositionInfo& aTPositionInfo);

        void CheckSatelliteGenericInfoL(const HPositionGenericInfo& aGenericInfo, TBool aCal=EFalse);

        void CheckTPositionSatelliteInfoL(const TPositionSatelliteInfo& aSatelliteInfo);
        
        void CheckSpeedGenericInfoL(const HPositionGenericInfo& aGenericInfo);

        void CheckCompassGenericInfoL(const HPositionGenericInfo& aGenericInfo, TBool aWest=ETrue);

        void CheckTPositionCourseInfoL(const TPositionCourseInfo& aCourseInfo);

        void CheckTPositionCourseInfoEmptySpeedL(const TPositionCourseInfo& aCourseInfo);

        void CheckSupportedNMEAFiles();

        void SetSatelliteRequestedFields(HPositionGenericInfo& aGenericInfo);

        void SetSpeedRequestedFields(HPositionGenericInfo& aGenericInfo);
        
        void SetCompassRequestedFields(HPositionGenericInfo& aGenericInfo);
    
        void GetCorrectSatelliteTime(TTime& aTime);

        void SetFileInUseL(const TDesC& aFileName);
		
        void PrintChecksum(const TDesC8& aSentence);
    private:    // Data
        TRequestStatus iStatus;
    };

#endif      // __CT_LBS_SIMULATION_PSY_POS_TP242_H__
            
// End of File
