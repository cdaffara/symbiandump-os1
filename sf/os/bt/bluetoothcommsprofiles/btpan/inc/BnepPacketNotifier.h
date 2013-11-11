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

#ifndef __BNEPPACKETNOTIFIER_H
#define __BNEPPACKETNOTIFIER_H

#include <bttypes.h>

const TUid KUidBnepPacketNotifierV1 = { 0x10282665 };
const TUid KUidPanLinkControlV1 = { 0x10282666 };

/**
Record to store information about amount of data transferred by BNEP
*/
class TBnepBytesTransferred
	{
public:
	/**
	Number of Bytes Received on this link since the last call of MbpnBnepDataTransferred
	*/
	TUint iBytesReceived;

	/**
	Number of Bytes Sent on this link since the last call of MbpnBnepDataTransferred
	*/
	TUint iBytesSent;

	/**
	Bluetooth address of this link
	*/
	TBTDevAddr iBTDevAddr;
	};

class MPanLinkControlBase
	{
public:
	/**
	Called by the policy DLL to get a pointer to the pan link control interface
	implemented by PAN with UID aInterface. This is a mechanism for allowing future 
	change to the PAN LPM API without breaking BC in existing (non-updated) 
	LPM policy plugins.
	@param aInterface UID of the interface to return
    @return Pointer to the interface
*/
	virtual TAny* MplcbGetInterface(const TUid & aInterface) = 0;
	};

/**
Interface implemented by the PAN profile to control low power mode
*/
class MPanLinkControl: public MPanLinkControlBase
	{
public:
	/**
	Called to Request Sniff mode
	@param aAddr Bluetooth Device address of the link to be put into sniff mode
	@return Error code
	*/
	virtual TInt MplcRequestSniff(const TBTDevAddr& aAddr) = 0;

	/**
	Called to Request Active mode
	@param aAddr Bluetooth Device address of the link to be put into active mode
	@return Error code
	*/
	virtual TInt MplcRequestActive(const TBTDevAddr& aAddr) = 0;

	};

class MBnepPacketNotifierBase
	{
public:
	/**
	Called by BNEP bridge to get a pointer to an object which implements the 
	LPM policy interface with UID aInterace. This is a mechanism for allowing future 
	change to the BNEP LPM API without breaking BC in existing (non-updated) 
	policies.
	@param aInterface UID of interface requested
	@return Pointer to the LPM policy interface
	*/
	virtual TAny* MbpnbGetInterface(const TUid & aInterface) = 0;
	
	/**
	Called by BNEP bridge to signal to the LPM policy that it can be destroyed
	*/
	
	virtual void MbpnbRelease() = 0;
	
	};
/**
Interface to be implemented by low power mode policy plugins for PAN
*/
class MBnepPacketNotifier: public MBnepPacketNotifierBase
	{
public:
	/**
	Called by BNEP every 2 seconds with information of how much data has been transferred in that time.
	If no data is transferred for 20 seconds, this will stop being called.
	@param aBnepBytesTransferred Array of TBnepBytesTransferred, with information of number of bytes sent and received to each physical link
	*/
	virtual void MbpnBnepDataTransferred(const TArray<TBnepBytesTransferred> & aBnepBytesTransferred) = 0;
	
	/**
	Called by BNEP to set the MPanLinkControlBase interface, which can be called to control sniff mode
	@param aPanLinkControl Reference to MPanLinkControl which will be implemented by the PAN profile
	*/
	virtual void MbpnSetLinkControl(MPanLinkControlBase& aPanLinkControl) = 0;
	
	/**
	Called by BNEP when a new device is added
	@param aAddr Bluetooth device address for the new device
	*/
	virtual void MbpnDeviceAdded(const TBTDevAddr& aAddr) = 0;
	
	/**
	Called by BNEP when a device is removed
	@param aAddr Bluetooth device address of the removed device
	*/
	virtual void MbpnDeviceRemoved(const TBTDevAddr& aAddr) = 0;
	};



#endif
