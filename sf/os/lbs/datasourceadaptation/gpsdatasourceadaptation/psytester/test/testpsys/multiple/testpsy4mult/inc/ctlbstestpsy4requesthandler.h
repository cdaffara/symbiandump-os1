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



#ifndef CT_LbsTestPsy4RequestHandler_H
#define CT_LbsTestPsy4RequestHandler_H

//  INCLUDES
#include "ctlbstestPsyBase.h"
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy4RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		static CT_LbsTestPsy4RequestHandler* InstanceL();

        void Release();
	
	public: // Functions from base class

	    /**
        * Cancels a position info request if it is the active request.
        */
		void CancelNotifyPositionUpdate(
			/* IN/OUT */	TRequestStatus* aStatus);
	
	private: // Constructors and destructor
        
		/**
		* C++ default constructor.
		*/
		CT_LbsTestPsy4RequestHandler();    
        
		/**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy4RequestHandler();
    
	protected: // Functions from base class
	
	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy4RequestHandler( const CT_LbsTestPsy4RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy4RequestHandler& operator=( const CT_LbsTestPsy4RequestHandler& );

	private:    // Data
		TInt					iReferenceCounter;
	
		TUint					iRequests;
	};

#endif      // CT_LbsTestPsy4RequestHandler_H  
            
// End of File
