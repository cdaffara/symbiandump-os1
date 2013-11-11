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



#ifndef __CT_LBS_TESTPSY2REQUESTHANDLER_H__
#define __CT_LBS_TESTPSY2REQUESTHANDLER_H__

//  INCLUDES
#include "ctlbstestpsybase.h"
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy3RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsTestPsy3RequestHandler* NewL(MPositionerStatus* aPositionerStatus);
	
		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy3RequestHandler();

	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestPsy3RequestHandler(MPositionerStatus* aPositionerStatus);    
        
	protected: // Functions from base class
	
			void SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache);

	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy3RequestHandler( const CT_LbsTestPsy3RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy3RequestHandler& operator=( const CT_LbsTestPsy3RequestHandler& );

	private:    // Data
		TUint				iRequests;
	};

#endif      // __CT_Lbs_TestPsy2RequestHandler_H__
            
// End of File
