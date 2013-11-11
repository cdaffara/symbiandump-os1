/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSMESSAGELISTENER_H
#define BTGPSMESSAGELISTENER_H

//  INCLUDES

#include <e32base.h> 


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class TBTGPSNmeaParser;


// CLASS DECLARATION

/**
*  Interface to get messages from connected BT device.
*
*/
class MBTGPSMessageListener
    {
    public: 
        /**
        * Handle received message.
		* @param aMsg Reference to received message. Observer shall
		*        store the message if the message will be used after
		*        this function call.
        */
        virtual void HandleMessage(const TBTGPSNmeaParser& aMsg) = 0;

    };
#endif
// End of File

