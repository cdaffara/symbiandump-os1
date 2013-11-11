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
* Description:  Declaration of MCDSYServiceLogic class
*
*/


#ifndef CDSYSERVICELOGIC_H
#define CDSYSERVICELOGIC_H

// INCLUDES
#include <e32base.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class TCDSYMessage;



/**
*  MCDSYServiceLogic
*  This class implements the MCDSYServiceLogic, which is interface class for service logics.
*/
NONSHARABLE_CLASS( MCDSYServiceLogic )
    {
    public:
        /**
        * Processes the incoming message.
        * @param TCDSYMessage& aCDSYMessage Reference to the message.
        * @return void
        */
        virtual void ProcessMessageL( TCDSYMessage& aCDSYMessage ) = 0;

        /**
        * Expires the message.
        * @param TCDSYMessage& aCDSYMessage Reference to the message.
        * @return void 
        */
        virtual void ExpireMessageL( TCDSYMessage& aCDSYMessage ) = 0;
    };



#endif //#ifndef CDSYSERVICELOGIC_H



//  End of File
