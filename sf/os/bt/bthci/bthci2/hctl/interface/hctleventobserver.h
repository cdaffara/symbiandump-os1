// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef HCTLEVENTOBSERVER_H
#define HCTLEVENTOBSERVER_H

#include <e32def.h>

class TDesC8;

const TInt KHCTLEventObserverUid = 0x102736E6;

/**
Interface for receiving HCI events from the HCTL.

Licensee is free to replace this interface because it is used only between
licensee components
*/
class MHCTLEventObserver
	  {
public:
	/**
	Called when an event has been received from over the HCTL.
	
	@param aEvent The data representing the event.
	*/
	virtual void MheoProcessEvent(const TDesC8& aEvent) =0;
	};

#endif // HCTLEVENTOBSERVER_H
