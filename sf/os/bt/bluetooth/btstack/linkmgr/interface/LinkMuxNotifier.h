// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @released
*/

#ifndef LINKMUXNOTIFIER_H
#define LINKMUXNOTIFIER_H

#include <e32base.h>

const TInt KLinkMuxNotifierUid = 0x102736E7;

/**
Interface for a class that needs to notify an opportunity to 
send an item waiting on an HCI queue
*/
class MLinkMuxNotifier
	{
public:
	/**
	Function call to indicate that the implementor should attempt to send HCI queue
	items from the HCI queues (in an appropriate manner/order).
	*/
	virtual void TryToSend() = 0;	// request to send on certain channels
	};
#endif //LINKMUXNOTIFIER_H
