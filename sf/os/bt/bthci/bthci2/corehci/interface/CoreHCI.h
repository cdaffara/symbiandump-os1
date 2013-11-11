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

#ifndef COREHCI_H
#define COREHCI_H

#include <e32base.h>

class MHCICommandQueue;
class MPhysicalLinksState;
class MHardResetInitiator;
class MHCIDataEventObserver;
class MHCIDataObserver;
class MHCICommandEventObserver;
class MHCTLChannelObserver;
class MControllerStateObserver;

/** 
The UID of the Core HCI API. If this API ever has to change, a new UID and associated 
M- class will be created. New implementations of the Core HCI may implement the new API. 
Old (non-updated) Core HCI classes will still work as long as the Stack supports the old API.
*/
const TInt KCoreHciInterfaceUid = 0x10274076;

/**
All non-static Bluetooth APIs needed by the HCI components are funnelled 
through this class to be handed out as the Core HCI sees fit.
*/
class MCoreHci
	{
public:
	// HCI General exports
	/** Sets option.

	@param aName option to set
	@param aData option value
	@return System wide error code
	*/
    virtual TInt MchSetOption(TUint aName, const TDesC8& aData) = 0;

    /** Gets option.

	@param aName option to set
	@param aData descriptor to be filled with option data
	@return System wide error code
    */
    virtual TInt MchGetOption(TUint aName, TDes8& aData) = 0;

    /** HCI level Ioctl

	@param aLevel			The Ioctl level
	@param aName			The Ioctl function number
	@param aOption			Data associated with this Ioctl
	@param aStackSAP		A pointer to the SAP, used to track Ioctls
    */
	virtual void MchIoctl(TUint aLevel,TUint aName, TDes8* aOption, TAny* aStackSAP) = 0;

    /** Cancel HCI level Ioctl

	@param aLevel			The Ioctl level
	@param aName			The Ioctl function number
	@param aStackSAP		A pointer to the SAP, used to track Ioctls
    */
	virtual void MchCancelIoctl(TUint aLevel, TUint aName, TAny* aStackSAP) = 0;
	
	/**
    HCI uses this function to be given a reference to MHCICommandQueue which 
	allows it to manipulate the command queue when necessary.
    @param aHCICommandQueue provides interface to command queue
    */
    virtual void MchSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue) = 0;

    /**
	HCI uses this function to be given a reference to MPhysicalLinksState
	@param aPhysicalLinksState provides information about physical link
	*/
    virtual void MchSetPhysicalLinksState(MPhysicalLinksState& aPhysicalLinksState) = 0;

    /**
    HCI uses this function to be given a reference to MHardResetInitiator 
	which allows it to initiate a hard reset of the controller.
    @param aHardResetInitiator provides interface to initiate a hard reset
    */
    virtual void MchSetHardResetInitiator(MHardResetInitiator& aHardResetInitiator) = 0;

    /**
    HCI uses this function to be given a reference to MHCIDataEventObserver 
	which is interested in data related events.
    @param aHCIDataEventObserver provides interface to the observer
    */
	virtual void MchSetDataEventObserver(MHCIDataEventObserver& aHCIDataEventObserver) = 0;
    
    /**
    HCI uses this function to be given a reference to MHCIDataObserver which 
	is interested in incoming data.
    @param aHCIDataObserver provides interface to the observer
    */
    virtual void MchSetDataObserver(MHCIDataObserver& aHCIDataObserver) = 0;

    /**
    HCI uses this function to be given a reference to MHCICommandEventObserver 
	which is interested in command related events.
    @param aHCICommandEventObserver provides interface to the observer
    */
    virtual void MchSetCommandEventObserver(MHCICommandEventObserver& aHCICommandEventObserver) = 0;
    
    /**
    HCI uses this function to be given a reference to MHCTLChannelObserver 
	which is interested in the transport sub-channels.
    @param aHCIChannelObserver provides interface to the observer
    */
    virtual void MchSetChannelObserver(MHCTLChannelObserver& aHCIChannelObserver) = 0;
    
    /**
    HCI uses this function to be given a reference to MControllerStateObserver 
	which is interested in the state of the power to the Bluetooth chip.
    @param aHCIPowerStateObserver provides interface to the observer
    */
    virtual void MchSetControllerStateObserver(MControllerStateObserver& aHCIPowerStateObserver) = 0;

	/**
	HCI uses this function to return a pointer to an Interface provided by
	the HCTL plugin.
	@param aUid UID of the API to be retrieved from the HCTL.
	@return a pointer to an implementation of the interface given by aUid. This 
		does not transfer ownership of the implementation.
	*/
	virtual TAny* MchHctlPluginInterface(const TUid aUid) = 0;
	};

#endif // COREHCI_H
