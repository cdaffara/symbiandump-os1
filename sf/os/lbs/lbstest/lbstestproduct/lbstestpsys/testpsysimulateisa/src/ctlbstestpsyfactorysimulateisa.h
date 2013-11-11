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



#ifndef __CT_LBS_TEST_PSY_FACTORY_SIMULATEISA_H__
#define __CT_LBS_TEST_PSY_FACTORY_SIMULATEISA_H__

//  INCLUDES
#include "EPos_CPosSubSessionFactoryBase.h"
#include "EPos_CPosSubSession.h"
#include "EPos_CPosHandler.h"
// CLASS DECLARATION

/**
*  PSY for simulate ISA position server
*/
class CT_LbsTestPsyFactorySimulateIsa :public CPosSubSessionFactoryBase
    {
    public:     // Functions from base classes

        /**
        * From CPosSubSessionFactoryBase
        *
        * @param aType The argument could either be KPosCapsPositioner or
        *              KPosCapsNMEA.
        * @return A subsession
        */
        CPosSubSession* CreateSubSessionL(
		/* IN  */		TUint aType
        );

        /**
        * From CPosSubSessionFactoryBase
        *
        * @param aType The argument could either be KPosCapsPositioner or
        *              KPosCapsNMEA.
        * @return a positioner or NMEA reader object
        */
        CPosHandler* CreateHandlerL(
		/* IN  */		TUint aType
        );

    };

#endif      // __CT_LBS_TEST_PSY_FACTORY_SIMULATEISA_H__

// End of File
