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
* Description:  Declaration of CCDSYEventManager class
*
*/


#ifndef CDSYEVENTMANAGER_H
#define CDSYEVENTMANAGER_H

// INCLUDES
#include <e32base.h>
#include "CDSYServiceLogic.h"



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CCDSYFactory;
class CCDSYController;



/**
*  CCDSYEventManager
*  This class implements the CCDSYEventManager, which raises events using CDosEventManager.
*/
NONSHARABLE_CLASS( CCDSYEventManager ): public CBase, public MCDSYServiceLogic
    {
    public:
        /**
        * Destructor.
        */
        ~CCDSYEventManager();

        /**
        * Creates a new instance of CCDSYEventManager.
        * @param CCDSYController& aCDSYController       Reference to CCDSYController.
        * @param aCDSYFactory& aCDSYFactory             Reference to an instance of aCDSYFactory.
        * @return CCDSYEventManager*                    Pointer to an instance of CCDSYEventManager.
        */
        static CCDSYEventManager* NewL( CCDSYController& aCDSYController, CCDSYFactory& aCDSYFactory );

        /**
        * Processes the incoming TCDSYMessage. The function raises an event to using CDosEventManager.
        * @param TCDSYMessage& aCDSYMessage             Event message.
        * @return void 
        */
        void ProcessMessageL( TCDSYMessage& aCDSYMessage );

        /**
        * Expires the message.
        * @param TCDSYMessage& aCDSYMessage             Message to be expired.
        * @return void 
        */
        inline void ExpireMessageL( TCDSYMessage& /*aCDSYMessage*/ )
            {
            }

    private:
        /**
        * Constructor.
        * @param CCDSYController& aCDSYController       Reference to CCDSYController.
        * @param CCDSYFactory& aCDSYFactory             Reference to an instance of CCDSYFactory.
        */
        CCDSYEventManager( CCDSYController& aCDSYController, CCDSYFactory& aCDSYFactory );

        /**
        * 2nd phase constructor.
        * @return void
        */
        void ConstructL();

    private:
        CCDSYController& iCDSYController;
        CCDSYFactory& iCDSYFactory;
    };



#endif //#ifndef CDSYEVENTMANAGER_H



//  End of File
