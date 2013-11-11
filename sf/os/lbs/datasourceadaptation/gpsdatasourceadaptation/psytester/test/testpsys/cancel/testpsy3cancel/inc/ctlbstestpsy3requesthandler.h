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



#ifndef __CT_LBS_TESTPSY3REQUESTHANDLER_H__
#define __CT_LBS_TESTPSY3REQUESTHANDLER_H__

//  INCLUDES
#include <e32base.h>
#include "ctlbstestpsybase.h"

// CONSTANTS

// CLASS DECLARATION

class CT_LbsPsy3RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsPsy3RequestHandler* InstanceL();

        void Release();
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsPsy3RequestHandler();    
        
		/**
		* Destructor.
		*/
		virtual ~CT_LbsPsy3RequestHandler();
    
	public: // Functions from base class
	
		void CancelNotifyPositionUpdate(TRequestStatus* aStatus);
		
	private:

        // By default, prohibit copy constructor
        CT_LbsPsy3RequestHandler( const CT_LbsPsy3RequestHandler& );
        // Prohibit assigment operator
        CT_LbsPsy3RequestHandler& operator=( const CT_LbsPsy3RequestHandler& );

	private:    // Data
		TUint				iReferenceCounter;
		TUint				iRequests;
	};

#endif
            
// End of File
