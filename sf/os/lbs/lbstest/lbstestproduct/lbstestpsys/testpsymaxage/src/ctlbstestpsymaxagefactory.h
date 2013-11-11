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



#ifndef __CT_LBS_TEST_PSY11_FACTORY_H__
#define __CT_LBS_TEST_PSY11_FACTORY_H__

//  INCLUDES
#include "EPos_CPosSubSessionFactoryBase.h"
#include "EPos_CPosSubSession.h"
#include "EPos_CPosHandler.h"
#include "EPos_CPositionerSubSession.h"
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The factory class required in all PSYs.
*  Used by the Epos server to create a subsession.
*/
class CT_LbsTestPsy11Factory :public CPosSubSessionFactoryBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CT_LbsTestPsy11Factory();

        /**
        * Destructor.
        */
        virtual ~CT_LbsTestPsy11Factory();

	public: // New functions

        /**
        * Creates a subsession to communicate with an stub positioner
        * @param aType The argument could either be KPosCapsPositioner or
        *              KPosCapsNMEA.
		* @return The subsession.
        */
		CPosSubSession* CreateSubSessionL(
		/* IN  */		TUint aType
        );

       /**
        * Creates a positioner object without subsession to be used by
        * e.g. the PSY switching subsession.
        * @param aType The argument could either be KPosCapsPositioner or
        *              KPosCapsNMEA.
        * @return a positioner object
        */
        CPosHandler* CreateHandlerL(
		/* IN  */		TUint aType
        );

    private:

        // By default, prohibit copy constructor
        CT_LbsTestPsy11Factory( const CT_LbsTestPsy11Factory& );
        // Prohibit assigment operator
        CT_LbsTestPsy11Factory& operator= ( const CT_LbsTestPsy11Factory& );
    };

#endif      // __CT_LBS_TEST_PSY11_FACTORY_H__   
            
// End of File
