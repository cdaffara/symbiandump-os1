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

#ifndef HCIDATAOBSERVER_H
#define HCIDATAOBSERVER_H

#include <bttypes.h>
#include <bluetooth/hci/aclpacketconsts.h>

/**
Mixin to receive data packets (ACL or Synchronous) from the HCI.
This should be implemented by the Stack.
*/
class MHCIDataObserver
	{
public:

	/**
	Called by the HCI Layer when it has received an ACL data packet.
	
	@param aConnH The connection handle for the connection over which the data was received.
	@param aFlag The flags (i.e. PB & BC) associated with the ACL data packet received.
	@param aData The payload of the packet that has been received.
	*/
	virtual void MhdoProcessAclData(THCIConnHandle aConnH, TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag, const TDesC8& aData) = 0;
	
	/**
	Called by the HCI Layer when it has received a synchronous data packet.
	
	@param aConnH The connection handle for the connection over which the data was received.
	@param aReserved The bits held in the reserved section of the packet, any value will be ignored.
	@param aData The payload of the packet that has been received.
	*/
	virtual void MhdoProcessSynchronousData(THCIConnHandle aConnH, TUint8 aReserved, const TDesC8& aData) = 0;
	};

#endif // HCIDATAOBSERVER_H
