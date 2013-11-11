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



#ifndef CT_LbsTestPsy6RequestHandler_H
#define CT_LbsTestPsy6RequestHandler_H

//  INCLUDES
#include "ctlbstestpsybase.h"
#include <e32base.h>

// CONSTANTS


// CLASS DECLARATION


class CT_LbsTestPsy6RequestHandler : public CT_LbsTestPsyBase
    {
	public:

		/**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy6RequestHandler* NewL();

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy6RequestHandler* NewLC();
        
        /**
		* Destructor.
		*/
		virtual ~CT_LbsTestPsy6RequestHandler(); 

	    void SetPartialSupport(const TBool& aPartialSupport);

	private: // Constructors and destructor
        
        /**
		* C++ default constructor.
		*/
		CT_LbsTestPsy6RequestHandler();
       
	protected: // Functions from base classes
	    
        void SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache);
     
	private:
		
		/**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CT_LbsTestPsy6RequestHandler( const CT_LbsTestPsy6RequestHandler& );
        // Prohibit assigment operator
        CT_LbsTestPsy6RequestHandler& operator=( const CT_LbsTestPsy6RequestHandler& );

	private:    // Data
        TUint       iPartialRequests;
        TUint       iRequests;
		TBool       iPartialSupport;
	};

#endif      // CT_LbsTestPsy6RequestHandler_H  
            
// End of File
