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



#ifndef __CT_LBS_CLIENT_POS_TP257_H__
#define __CT_LBS_CLIENT_POS_TP257_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 257
*
*/
class CT_LbsClientPosTp257 :public CT_LbsPortedStepBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsClientPosTp257(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp257();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Deinitializes the test. 
        * Always called even if test leaves.
        */
        void CloseTest();

        /**
        * From CT_LbsPortedStepBase
        */
        void StartL();

	private: // New functions

        /**
        * Compares two TPositionInfoBase objects
        * @return returns true if equal, otherwise false
        */
        TBool ComparePositionL(TPositionInfo aPosInfo1, TPositionInfo aPosInfo2);

        /**
        * Deletes a file from the file system
        * @param aFile the file name
        */
		void DeleteFileL(const TDesC& aFile);

		/**
        * Check if the specified file exists
        * @param aFile the file name
        */
		TBool FileExistsL(const TDesC& aFile);

    };

#endif      // __CT_LBS_CLIENT_POS_TP257_H__

// End of File
