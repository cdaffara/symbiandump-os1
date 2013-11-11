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

#ifndef HCIDATAFRAMER_H
#define HCIDATAFRAMER_H

#include <bttypes.h>
#include <bluetooth/hci/aclpacketconsts.h>

class CHctlAclDataFrame;
class CHctlSynchronousDataFrame;

/** 
The UID of the HCI Data Framer API. If the HCI Data Framer API ever has to change, a 
new UID and associated M- class will be created. New implementations of Data Framer 
Plugin may implement the new API. Old (non-updated) HCI Data Framer Plugins will
still work as long as the Stack supports the old API.
*/
const TInt KHCIDataFramerInterfaceUid = 0x102736EA;

/**
Mixin for the HCI Data Framer API.
This interface is to be used via CCoreHCIPlugin.
@see CCoreHCIPlugin::Interface(TUid)
*/
class MHCIDataFramer
	{
public:
	/**
	Creates a new ACL data frame object of specified size.

	@param aSize The size of the frame.
	@return A pointer to a newly constructed instance representing an ACL data frame.
	*/
	virtual CHctlAclDataFrame* MhdfNewAclDataFrameL(TUint16 aSize) = 0;
	
	/**
	Creates a new Synchronous data frame object of specified size.

	@param aSize the size of the frame.
	@return A pointer to a newly constructed instance representing a 
    Synchronous data frame.
	*/
	virtual CHctlSynchronousDataFrame* MhdfNewSynchronousDataFrameL(TUint8 aSize) = 0;
	
	/** 
	Formats ACL data into a HCI Data Frame (which includes the connection handle
	and a broadcast flag as well as the ACL data.)

	@param aFrame The ACL data frame object that is to be formatted.
	@param aConnH The connection handle that the frame is to be associated with.
	@param aFlag The ACL flag fields to be set in the frame.
	@param aData The data that is to be sent in the frame.
	*/
	virtual void MhdfFormatAclData(CHctlAclDataFrame& aFrame, THCIConnHandle aConnH, TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag, const TDesC8& aData) = 0;
	
	/**
	Formats Synchronous data into a HCI Data Frame (which includes the connection handle
	as well as the Synchronous data.)

	@param aFrame The Synchronous data frame object that is to be formatted.
	@param aConnH The connection handle that the frame is to be associated with.
	@param aData The data that is to be sent in the frame.	
	*/
	virtual void MhdfFormatSynchronousData(CHctlSynchronousDataFrame& aFrame, THCIConnHandle aConnH, const TDesC8& aData) = 0;
	};

#endif // HCIDATAFRAMER_H
