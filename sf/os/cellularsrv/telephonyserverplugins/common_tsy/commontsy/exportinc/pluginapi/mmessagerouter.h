/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @publishedPartner
 @released
*/

#ifndef MMESSAGEROUTER_H
#define MMESSAGEROUTER_H

// INCLUDES
#include <e32base.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

// CLASS DECLARATION

/**
 *  Defines the Message Router interface
 *
 */
class MMessageRouter
    {
    public:
    
		/**
		 * Directs requests to Licensee TSY
		 * 
		 * @param aIpc The IPC number of the request
		 * @param aDataPackage The data to pass to licensee TSY
		 * @return standard error code
		 */
	    virtual TInt ExtFuncL( 
	        TInt aIpc, 
	        const CMmDataPackage* aDataPackage ) = 0;  

    };

#endif  // MMMESSAGEROUTER_H

//  End of file
