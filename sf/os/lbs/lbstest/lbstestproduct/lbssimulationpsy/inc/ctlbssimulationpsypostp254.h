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



#ifndef __CT_LBS_SIMULATION_PSY_POS_TP254_H__
#define __CT_LBS_SIMULATION_PSY_POS_TP254_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CT_LbsSimulationPSYPosTp254 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsSimulationPSYPosTp254(CT_LbsServer& aParent);
        
        /**
        * Destructor.
        */
        ~CT_LbsSimulationPSYPosTp254();

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

    public: // data
        
        typedef TUint TMissingPositionData;

        enum _TMissingPositionData
            {
            ENone       = 0,               
            ELongitude  = 0x01,         
            ELatitude   = 0x02,
            ETimeStamp  = 0x04,
            EAltitude   = 0x08
            };
            
    private:
    
        TInt Request(TPositionInfoBase& aPositionInfo);

        void CheckTPositionInfoL(const TPositionInfo& aPosInfo, TMissingPositionData aMissingData);
    
        void SetFileInUseL(const TDesC& aFileName, TBool aPartial=ETrue);
		
        void PrintChecksum(const TDesC8& aSentence);
    
        void SetPartialUpdateL(TBool aPartial);
        
        void SetUpdateTimeOutL(const TInt& aInterval);
    private:    // Data
        
        TRequestStatus iStatus;
    };

#endif      // __CT_LBS_SIMULATION_PSY_POS_TP254_H__
            
// End of File
