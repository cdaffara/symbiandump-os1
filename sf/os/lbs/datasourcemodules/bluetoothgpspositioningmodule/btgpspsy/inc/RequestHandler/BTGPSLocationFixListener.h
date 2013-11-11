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




#ifndef BTGPSLOCATIONFIXLISTENER_H
#define BTGPSLOCATIONFIXLISTENER_H

//  INCLUDES

#include <e32base.h> 

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSFix;

// CLASS DECLARATION

/**
*  Interface class to inform location fix information updates.
*
*/
class MBTGPSLocationFixListener
    {
    public: 

        /**
        * This function is called when location fix information is updated. 
		* The listner object should check from aFix if the there is a valid
		* fix or not.
		*
        * @param aFix Reference to fix information.
        */
        virtual void HandleLocationFixUpdate(const CBTGPSFix& aFix) = 0;
    };
#endif
// End of File

