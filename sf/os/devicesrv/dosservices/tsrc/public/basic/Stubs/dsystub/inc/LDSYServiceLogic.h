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
* Description:  Declaration of CLDSYServiceLogic class
*
*/


#ifndef TLSYSERVICELOGIC_H
#define TLSYSERVICELOGIC_H

// INCLUDES
#include <e32base.h>
#include <CDSYServiceLogic.h>



// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class TCDSYMessage;
class CCDSYController;


/**
*  TLDSYServiceLogic
*  This class implements the CLDSYServiceLogic, which is 
*/
class TLDSYServiceLogic : public MCDSYServiceLogic
    {
    public:
        /**
        * Processes the incoming message.
        * @param TCDSYMessage& aCDSYMessage Reference to the message.
        * @return void
        */
        virtual void ProcessMessageL( TCDSYMessage& aCDSYMessage );

    protected:
        /**
        * Processes the incoming Common DSY message.
        * @param TCDSYMessage& aCDSYMessage Reference to the message.
        * @return void 
        */
        virtual void ProcessCDSYMessageL( TCDSYMessage& aCDSYMessage );

    public:
        /**
        * Expires the message.
        * @param TCDSYMessage& aCDSYMessage Reference to the message.
        * @return void 
        */
        virtual void ExpireMessageL( TCDSYMessage& aCDSYMessage );

    protected:
        /**
        * Constructor.
        * @param CCDSYController& aCDSYController   CCDSYController reference.
        */
        TLDSYServiceLogic( CCDSYController& aCDSYController );

        /**
        * Destructor
        */
        virtual ~TLDSYServiceLogic();

    protected:
        CCDSYController& iCDSYController;   // Reference to Common DSY controller

    };



#endif //#ifndef TDSYSERVICELOGIC_H



//  End of File