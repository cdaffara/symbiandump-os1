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

#ifndef HCICOMMANDQUEUECLIENT_H
#define HCICOMMANDQUEUECLIENT_H

#include <e32def.h>

class THCIEventBase;
class CHCICommandBase;

/**
Mixin for the API to receive notification of HCI events received
by the command queue.
*/
class MHCICommandQueueClient
	{
public:
	/**
	Invoked by the command queue to notify observers that a HCI event has been received, 
	and what command item has caused (or is associated with) the event.  This method will be
	called for all events received even if they convey a HCI error condition.
	
	@param aEvent The event that has been received. 
	@param aRelatedCommand The command that is associated with the event, this
	can be NULL for events that cannot be matched to a command.
	*/
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand) = 0;

	/**
	Invoked by the command queue to notify observers that a command has failed with an error.
	This could be caused by:
		- the command timing out before receiving a response from the controller
		- the QDP rejecting the command
	
	@param aErrorCode The error code with which the command failed.
	@param aRelatedCommand The command that that failed, this can be NULL for
	errors that cannot be associated to a command.
	*/
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand) = 0;
	};

#endif // HCICOMMANDQUEUECLIENT_H
