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



#ifndef __CT_LBS_TESTPSY1REQUESTHANDLER_H__
#define __CT_LBS_TESTPSY1REQUESTHANDLER_H__

//  INCLUDES
#include "ctlbstestpsybase.h"
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy1RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsTestPsy1RequestHandler* InstanceL();

        void Release();
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestPsy1RequestHandler();    
        
		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy1RequestHandler();
    
	protected: // Functions from base classes
	
	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy1RequestHandler( const CT_LbsTestPsy1RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy1RequestHandler& operator= ( const CT_LbsTestPsy1RequestHandler& );

	private:    // Data
		TInt					iReferenceCounter;
	};

#endif      // __CT_LBS_TESTPSY1REQUESTHANDLER_H__  
            
// End of File
