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

#ifndef HCIDATAEVENTOBSERVER_H
#define HCIDATAEVENTOBSERVER_H

class THCIEventBase;

/**
Mixin for being informed about reception of HCI Events from the HCI that are associated
with sending ACL or Synchronous data to the HCI.
This should be implemented by the Stack/HCI Facade.

This interface is used as a way of passing the following unsolicited data events:
	Number_Of_Completed_Packets Event 
	Flush_Occurred Event
	Data_Buffer_Overflow Event
	QoS_Violation Event
Note that events that seem data related but are in fact the result of a command (e.g. QoS Setup Command)
do not follow the path described above and go to MHCICommandEventObserver instead.
@see MHCICommandEventObserver
*/
class MHCIDataEventObserver
	{
public:
	/**
	Called when an event that is considered a response to ACL or Synchronous data has been received.
	@param aEvent The event received.
	*/
	virtual void MhdeoEventNotification(THCIEventBase& aEvent) = 0;
	};

#endif // HCIDATAEVENTOBSERVER_H
