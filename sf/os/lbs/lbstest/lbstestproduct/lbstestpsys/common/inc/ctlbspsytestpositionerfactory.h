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



#ifndef __CT_LBS_PSY_TEST_POSITIONER_FACTORY_H__
#define __CT_LBS_PSY_TEST_POSITIONER_FACTORY_H__

//  INCLUDES
#include "EPos_CPosSubSessionFactoryBase.h"
#include "EPos_CPosSubSession.h"
#include "EPos_CPositionerSubSession.h"
#include "EPos_CPosHandler.h"
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The factory class required in all PSYs.
*  Used by the Epos server to create a subsession.
*/
class CT_LbsPsyTestPositionerFactory :public CPosSubSessionFactoryBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CT_LbsPsyTestPositionerFactory();

        /**
        * Destructor.
        */
        virtual ~CT_LbsPsyTestPositionerFactory();

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

    };

#endif      // __CT_LBS_PSY_TEST_POSITIONER_FACTORY_H__

// End of File
