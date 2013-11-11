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
#include <ctlbstestPsyBase.h>
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy2RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsTestPsy2RequestHandler* NewL(MPositionerStatus* aPositionerStatus);

		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy2RequestHandler();
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestPsy2RequestHandler(MPositionerStatus* aPositionerStatus);    
        
	protected: // Functions from base class
	
			void SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache);

	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy2RequestHandler( const CT_LbsTestPsy2RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy2RequestHandler& operator=( const CT_LbsTestPsy2RequestHandler& );

	private:    // Data
		TUint				iReferenceCounter;
		TUint				iRequests;
	};

#endif      // __CT_LBS_TESTPSY2REQUESTHANDLER_H__  
            
// End of File
