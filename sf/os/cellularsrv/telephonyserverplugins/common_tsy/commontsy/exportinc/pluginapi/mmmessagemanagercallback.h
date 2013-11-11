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
#ifndef MMMESSAGEMANAGERCALLBACK_H
#define MMMESSAGEMANAGERCALLBACK_H

// INCLUDES
#include <e32base.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

// CLASS DECLARATION

/**
 *  Defines the Message Manager callback interface
 *
 */
class MmMessageManagerCallback
    {
    public:
    
		/**
		 * Completes request to CTSY
		 *
		 * @param aIpc The IPC number of the request
		 * @param aResult The result of the request
		 */ 
		virtual void Complete( TInt aIpc, TInt aResult ) = 0;
		
		/**
		 * Completes request to CTSY
		 *
		 * @param aIpc The IPC number of the request
		 * @param aData Data to pass from Licensee TSY
		 * @param aResult The result of the request
		 */
		virtual void Complete( TInt aIpc, 
		    CMmDataPackage* aData, TInt aResult ) = 0;  

    };

#endif  // MMMESSAGEMANAGERCALLBACK_H

//  End of file
