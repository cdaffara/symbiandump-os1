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



#ifndef __CT_LBS_CLIENT_POS_TP2_H__
#define __CT_LBS_CLIENT_POS_TP2_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS
class TPositionSatelliteInfo;

// CLASS DECLARATION

/**
*  Test procedure 2
*
*/
class CT_LbsClientPosTp2 : public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
    	CT_LbsClientPosTp2(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp2();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

	private: // New functions

        /**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
        void AssertTrueL(TBool aCondition, const TDesC& aErrorMsg);

		void RequestL(TPositionInfoBase& aInfoBase);

		void CheckPositionL(TPosition& aPosition);

		void CheckPositionInfoL(TPositionInfo& aInfo);
		void CheckPositionCourseInfoL(TPositionCourseInfo& aInfo);
		void CheckPositionSatelliteInfoL(TPositionSatelliteInfo& aInfo);
		void CheckGenericInfoL(HPositionGenericInfo& aInfo);

    };

#endif      // __CT_LBS_CLIENT_POS_TP2_H__

// End of File
