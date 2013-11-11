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
* Description:  Declaration of CCDSYFactory class
*
*/


#ifndef CDSYFACTORY_H
#define CDSYFACTORY_H

// INCLUDES
#include <e32base.h>
#include <dosfactorybase.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CCDSYController;
class CCDSYDOSServerRequestManager;
class CCDSYEventManager;
class MCDSYServiceLogicFactory;

/**
*  CCDSYFactory
*  This class implements the CCDSYFactory, which is the interface class for the DOS server plug-in factory.
*/
NONSHARABLE_CLASS( CCDSYFactory ): public CDosFactoryBase
    {
    public:
        /**
        * Destructor.
        */
        ~CCDSYFactory();

        /**
        * Creates a new instance of CCDSYFactory.
        * @return CCDSYFactory* Pointer to an instance of CCDSYFactory.
        */
        static CCDSYFactory* NewL();

    protected:              // From Dos plug-in API

        /**
        * The function returns an object derived from CDosExtensionBase, 
        * which implements the pure virtual functions defined in MDosExtensionBaseDSY to DOS Server.
        * @return CDosExtensionBase*    Pointer to the plugin.
        */
        CDosExtensionBase* NewExtensionServiceL();


    private:
        /**
        * Constructor.
        */
        CCDSYFactory();

        /**
        * 2nd phase constructor.
        * @return void 
        */
        void ConstructL();

        /**
        * Loads the LDSY module.
        * @return void 
        */
        void LoadLDSYModuleL();

    private:
        CCDSYController* iCDSYController;
        CCDSYDOSServerRequestManager* iCDSYDOSServerRequestManager;
        CCDSYEventManager* iCDSYEventManager;
        MCDSYServiceLogicFactory* iDSYServiceLogicFactory;
        RLibrary iLDSYLibrary;
    };

#endif //#ifndef CDSYFACTORY_H

//  End of File
