// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalAll 
*/

#ifndef MDISPATCHERCALLBACK_H_
#define MDISPATCHERCALLBACK_H_

#include "requestqueueoneshot.h"


class MDispatcherCallback
	{
public:

	/**
	 * This function forms part of the handling of IPCs which are completed 
	 * synchronously by the Licensee LTSY but needs completion asynchronously
	 * up to the Common TSY.  It is used by the CRequestQueueOneShot object
	 * to forward the request back to the correct dispatcher along with the
	 * completion data so that the request data can then be unpacked and cleaned
	 * up properly on return to the Common TSY. 
	 * 
	 * @param aIpcDataPackage Encapsulates the request to complete.
	 */
	virtual void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage) = 0;

	}; // class MDispatcherCallback 

#endif /*MDISPATCHERCALLBACK_H_*/
