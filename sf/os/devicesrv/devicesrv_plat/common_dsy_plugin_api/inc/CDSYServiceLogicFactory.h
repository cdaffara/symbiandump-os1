/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of MCDSYServiceLogicFactory class
*
*/


#ifndef CDSYSERVICELOGICFACTORY_H
#define CDSYSERVICELOGICFACTORY_H

// INCLUDES
#include <e32base.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CCDSYController;



/**
*  MCDSYServiceLogicFactory
*  This class implements the MCDSYServiceLogicFactory, which is the interface class for LDSY factory class.
*/
NONSHARABLE_CLASS( MCDSYServiceLogicFactory )
    {
    public:
        /**
        * Initializes the factory.
        * @param CCDSYController& aCDSYController   Reference to the CDSYController.
        * @return void
        */
        virtual void InitializeL( CCDSYController& aCDSYController ) = 0;

        /**
        * Deinitializes the factory.
        * @return void
        */
        virtual void DeInitialize() = 0;

        /**
        * Destructor.
        */
        virtual ~MCDSYServiceLogicFactory()
            {
            }
    };



#endif //#ifndef CDSYSERVICELOGICFACTORY_H



//  End of File
