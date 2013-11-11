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



#ifndef __CT_LBS_TESTPSY18REQUESTHANDLER_H__
#define __CT_LBS_TESTPSY18REQUESTHANDLER_H__

//  INCLUDES
#include "ctlbstestpsybase.h"
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy18RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsTestPsy18RequestHandler* InstanceL();

        void Release();
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestPsy18RequestHandler();    
        
		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy18RequestHandler();
    
	protected: // Functions from base class
	
		
        void SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache);

		void SetAddressGenericInfoL(HPositionGenericInfo& aGenericInfo);
		
		void SetBuildingGenericInfoL(HPositionGenericInfo& aGenericInfo);

		void SetMediaGenericInfoL(HPositionGenericInfo& aGenericInfo);

	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy18RequestHandler( const CT_LbsTestPsy18RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy18RequestHandler& operator=( const CT_LbsTestPsy18RequestHandler& );

	private:    // Data
		TUint				iReferenceCounter;
		TUint				iRequests;
	};

#endif      // __CT_LBS_TESTPSY18REQUESTHANDLER_H__
            
// End of File
