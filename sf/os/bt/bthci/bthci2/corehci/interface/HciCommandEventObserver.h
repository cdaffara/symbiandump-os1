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

#ifndef HCICOMMANDEVENTOBSERVER_H
#define HCICOMMANDEVENTOBSERVER_H

class THCIEventBase;

/**
Mixin for being informed about reception of HCI Command Events from the HCI.
This should be implemented by the Command Queue.
*/
class MHCICommandEventObserver
	{
public:
	/**
	Called when an event that is considered a response to a command has been received.
	@param aEvent The event received.
	*/
	virtual void MhceoEventNotification(const THCIEventBase& aEvent) = 0;
	};

#endif // HCICOMMANDEVENTOBSERVER_H
