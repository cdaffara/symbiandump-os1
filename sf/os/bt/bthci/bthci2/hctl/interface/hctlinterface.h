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

#ifndef HCTLINTERFACE_H
#define HCTLINTERFACE_H

#include <e32def.h>

class MHCTLChannelObserver;
class MHCTLDataObserver;
class MHCTLEventObserver;
class MControllerStateObserver;
class MQdpPluginInterfaceFinder;

class TDesC8;

/**
The UID of the hctl API. If the API ever has to change, a new UID and
associated M class will be created. New implementations of CHCTLBase
may implement the new API. Old (non-updated) implementations will still
work as long as the old API is supported.
*/
const TInt KHCTLInterfaceUid = 0x102736E9;

/**
Mixin for the HCTL API.
This interface is to be used via the HCTL Plugin.
@see CHCTLBase::GetInterface(TUid)
*/
class MHCTLInterface
	{
public:
	/**
	Writes data out as a command frame.
	The data should represent a single complete command frame.

	@param aData Byte representation of a command frame.
	@return KErrNone if success, otherwise one of the Symbian global error codes.
	*/
	virtual TInt MhiWriteCommand(const TDesC8& aData) = 0;
	
	/**
	Writes data out as an ACL data frame.
	The data should represent a single complete ACL data frame.
	
	@param aData Byte representation of an ACL data frame.
	@return KErrNone if success, otherwise one of the Symbian global error codes.
	*/
	virtual TInt MhiWriteAclData(const TDesC8& aData) = 0;
	
	/**
	Writes data out as a Synchronous data frame.
	The data should represent a single complete Synchronous data frame.

	@param aData Byte representation of a Synchronous data frame.
	@return KErrNone if success, otherwise one of the Symbian global error codes.
	*/
	virtual TInt MhiWriteSynchronousData(const TDesC8& aData) = 0;

	/**
	Get the HCTL layer ACL data header and trailer size
	@param aHeaderSize Outgoing parameter for retrieving ACL data header
	size
	@param aTrailerSize Outgoing parameter for retrieving ACL data trailer 
	size
	*/
	virtual void MhiGetAclDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const = 0;

	/**
	Get the HCTL layer Synchronous data header and trailer size
	@param aHeaderSize Outgoing parameter for retrieving Synchronous data header
	size
	@param aTrailerSize Outgoing parameter for retrieving Synchronous data trailer
	size
	*/
	virtual void MhiGetSynchronousDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const = 0;

	/**
	Get the HCTL layer command header and trailer size
	@param aHeaderSize Outgoing parameter for retrieving command header
	size
	@param aTrailerSize Outgoing parameter for retrieving command trailer
	size
	*/
	virtual void MhiGetCommandTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const = 0;

	/**
	Setter for Data Observer
	@param aDataObserver The data observer gets notification of incoming data from HCTL
	*/
	virtual void MhiSetDataObserver(MHCTLDataObserver& aDataObserver) = 0;

	/**
	Setter for Event Observer
	@param aEventObserver The event observer gets notification of incoming events from HCTL
	*/
	virtual void MhiSetEventObserver(MHCTLEventObserver& aEventObserver) = 0;

	/**
	Setter for the channel observer
	@param aChannelObserver The channel observer gets notification of channel open and close from HCTL
	*/
	virtual void MhiSetChannelObserver(MHCTLChannelObserver& aChannelObserver) = 0;

	/**
	Setter for the controller state observer
	@param aControllerStateObserver The controller state observer gets notification of changes in the controller state such as power on and off
	*/
	virtual void MhiSetControllerStateObserver(MControllerStateObserver& aControllerStateObserver) = 0;

	/**
	Setter for the qdp plugin interface finder
	@param aControllerStateObserver The qdp plugin interface finder may be used to access extended qdp functionality.
	*/
	virtual void MhiSetQdpPluginInterfaceFinder(MQdpPluginInterfaceFinder& aQDPPluginInterfaceFinder) = 0;
	};

#endif // HCTLINTERFACE_H

