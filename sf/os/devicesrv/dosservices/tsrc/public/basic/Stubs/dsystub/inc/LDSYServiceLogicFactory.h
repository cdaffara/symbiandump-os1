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
* Description:  Declaration of CLDSYServiceLogicFactory class
*
*/


#ifndef LDSYSERVICELOGICFACTORY_H
#define LDSYSERVICELOGICFACTORY_H

// INCLUDES
#include <e32base.h>
#include <CDSYServiceLogicFactory.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CLDSYStubServiceLogic;
class CLDSYNotSupportedServiceLogic;


/**
*  CLDSYServiceLogicFactory
*  This class implements the CLDSYServiceLogicFactory
*/
class CLDSYServiceLogicFactory : public CBase, public MCDSYServiceLogicFactory
    {
    public:
        /**
        * Constructor.
        */
        CLDSYServiceLogicFactory();

        /**
        * Destructor.
        */
        ~CLDSYServiceLogicFactory();

        /**
        * Creates a new instance of CLDSYServiceLogicFactory.
        * @return CLDSYServiceLogicFactory*         New instance of CLDSYServiceLogicFactory.
        */
        static CLDSYServiceLogicFactory* NewL();

        /**
        * Initializes service logic factory.
        * @param CCDSYController& aCDSYController   Reference to CCDSYController.
        * @return void 
        */
        virtual void InitializeL( CCDSYController& aCDSYController );

        /**
        * Deinitializes service logic factory.
        * @return void 
        */
        virtual void DeInitialize();

    private:
        CLDSYStubServiceLogic* iLDSYStubServiceLogic;                   // Pointer to stub service logic
        CLDSYNotSupportedServiceLogic* iLDSYNotSupportedServiceLogic;   // Pointer to not supported service logic
        
    };



#endif //#ifndef LDSYSERVICELOGICFACTORY_H



//  End of File