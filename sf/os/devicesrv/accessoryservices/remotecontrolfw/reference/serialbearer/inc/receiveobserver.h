// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef RECEIVEOBSERVER_H
#define RECEIVEOBSERVER_H

#include <e32base.h>

/**
Observer for receive completions.
*/
class MReceiveObserver
	{
public:
	/**
	Called when a receive has completed.
	@param aError The completion error of the receive.
	*/
	virtual void MroReceiveComplete(TInt aError) = 0;
	};

#endif // RECEIVEOBSERVER_H
