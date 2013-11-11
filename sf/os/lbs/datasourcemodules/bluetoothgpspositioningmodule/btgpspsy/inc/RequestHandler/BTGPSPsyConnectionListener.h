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




#ifndef BTGPSPSYCONNECTIONLISTENER_H
#define BTGPSPSYCONNECTIONLISTENER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface class to get PSY connection information change. 
*  
*
*/
class MBTGPSPsyConnectionListener
    {
    public:  // New functions

        /**
        * Handle PSY connection status chagned. This function is
		* called when PSY connection status changes. PSY connection
		* status include new PSY connected or disconnected, PSY locaiton
		* request status changed, or tracking status changed.
        */
        virtual void HandlePsyConnectionChange() = 0;
    };
#endif
// End of File
