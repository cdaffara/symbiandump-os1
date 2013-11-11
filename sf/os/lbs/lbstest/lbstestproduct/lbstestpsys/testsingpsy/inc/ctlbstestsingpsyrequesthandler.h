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



#ifndef __CT_LBS_TEST_SING_PSY_REQUEST_HANDLER_H__
#define __CT_LBS_TEST_SING_PSY_REQUEST_HANDLER_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS
class MPositionerStatus; 


// CLASS DECLARATION
class CT_LbsTestSingPsyRequestHandler : public CBase
    {
	public:

		static CT_LbsTestSingPsyRequestHandler* InstanceL(MPositionerStatus* aPositionerStatus);

        void Release();
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestSingPsyRequestHandler(MPositionerStatus* aPositionerStatus);    
        
		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestSingPsyRequestHandler();
    
	protected: // Functions from base classes
	
	private:

        // By default, prohibit copy constructor
        CT_LbsTestSingPsyRequestHandler( const CT_LbsTestSingPsyRequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestSingPsyRequestHandler& operator= ( const CT_LbsTestSingPsyRequestHandler& );

	private:    // Data
		TUint				iReferenceCounter;
        MPositionerStatus*	iPositionerStatus;
	};

#endif      // __CT_LBS_TEST_SING_PSY_REQUEST_HANDLER_H__  
            
// End of File
