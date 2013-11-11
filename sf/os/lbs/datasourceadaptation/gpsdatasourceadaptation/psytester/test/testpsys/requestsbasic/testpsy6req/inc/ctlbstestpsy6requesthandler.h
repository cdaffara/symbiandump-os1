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



#ifndef __CT_LBS_TESTPSY6REQUESTHANDLER_H__
#define __CT_LBS_TESTPSY6REQUESTHANDLER_H__

//  INCLUDES
#include "ctlbstestpsybase.h"
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy6RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsTestPsy6RequestHandler* InstanceL();

        void Release();
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestPsy6RequestHandler();    
        
		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy6RequestHandler();
    
	protected: // Functions from base classes
	
		void SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache);

		void SetTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo, const TBool& aCache);

		void SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo);
	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy6RequestHandler( const CT_LbsTestPsy6RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy6RequestHandler& operator=( const CT_LbsTestPsy6RequestHandler& );

	private:    // Data
		TUint					iReferenceCounter;
		TUint					iRequests;					
	};

#endif      // __CT_LBS_TESTPSY6REQUESTHANDLER_H__  
            
// End of File
