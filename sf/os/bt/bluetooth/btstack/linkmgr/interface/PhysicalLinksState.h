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
 @publishedPartner
 @released
*/

#ifndef PHYSICALLINKSSTATE_H
#define PHYSICALLINKSSTATE_H

#include <e32cmn.h>
#include <bttypes.h>
#include <bluetooth/hci/hcitypes.h>

class TBTBasebandLinkState;
class TBTFeatures;
class TLinkPolicy;
class TBTDevAddr;
class TBTDeviceClass;

typedef RArray<THCIConnHandle> RHCIConnHandleArray;

class MPhysicalLinksState
/** An implementation of this interface would present a snapshot of
	the current Physical, Asynchronous and Synchronous links
	associated with a Bluetooth device.

	It also provides information appertaining to a remote device at
	the other end of a physical link.

	It should be noted that the returned data are a snapshot of the
	current links associated with a Bluetooth device.  They may become
	stale very quickly.

	Mixin class to be used as an interface to the stack.
*/
	{
public:
	/**Returns a list of 'connected' connection handles for all links
	   of the specified type.  Pending connection handles are not
	   returned	 by this method.

	   The returned data are a snapshot and will grow stale. They
	   should therefore be used immediately (in the same call stack)
	   or not at all. It would be wise to check whether the returned
	   links are still connected.

	   @see TInt BasebandLinkState(TBTBasebandLinkState&
	   aBasebandLinkState, const TBTDevAddr& aBDAddr);

	   @param aConnectionHandles A client supplied dynamic array that,
	   on successful completion of this operation, will be populated
	   with the requested connection handles.

	   @param aLinkType Indicates the type of link handles to be
	   returned.

	   @return Status code.
	*/
	virtual TInt MplsGetConnectionHandles(RHCIConnHandleArray& aConnectionHandles,
										  TLinkType aLinkType) const = 0;

	/**Returns the number of 'pending' connection handles for all
	   links of the specified type.

	   The returned data are a snapshot and will grow stale. They
	   should therefore be used immediately (in the same call stack)
	   or not at all.

	   @param aConnectionHandles A client supplied TInt that, on
	   successful completion of this operation, will be contain the
	   number of pending connection handles.

	   @param aLinkType Indicates the type of link handles to be
	   returned.

	   @return Status code.
	*/
	virtual TInt MplsGetNumPendingHandles(TInt& aConnectionHandles,
										  TLinkType aLinkType) const = 0;

	/**Returns a list of 'connected' connection handles for all links
	   of the specified type and Bluetooth device address.	Pending
	   connection handles are not returned by this method.

	   The returned data are a snapshot and will grow stale. They
	   should therefore be used immediately (in the same call stack)
	   or not at all. It would be wise to check whether the returned
	   links are still connected.

	   @param aConnectionHandles A client supplied dynamic array that,
	   on successful completion of this operation, will be populated
	   with the requested connection handles.

	   @param aLinkType Indicates the type of link handles to be
	   returned.

	   @param aBDAddr The client supplied Bluetooth device address of
	   the remote device for which the list of links is required.

	   @return Status code.
	*/
	virtual TInt MplsGetConnectionHandles(RHCIConnHandleArray& aConnectionHandles,
										  TLinkType aLinkType,
										  const TBTDevAddr& aBDAddr) const = 0;

	/**Returns the number of 'pending' connection handles for all
	   links of the specified type and Bluetooth device
	   address.

	   The returned data are a snapshot and will grow stale. They
	   should therefore be used immediately (in the same call stack)
	   or not at all.

	   @param aConnectionHandles A client supplied TInt that, on
	   successful completion of this operation, will contain the
	   number of pending connection handles..

	   @param aLinkType Indicates the type of link handles to be
	   returned.

	   @param aBDAddr The client supplied Bluetooth device address of
	   the remote device for which the number of pending links is
	   required.

	   @return Status code.
	*/
	virtual TInt MplsGetNumPendingHandles(TInt& aConnectionHandles,
										  TLinkType aLinkType,
										  const TBTDevAddr& aBDAddr) const = 0;

	/**Returns the Bluetooth device address associated with the
	   supplied connection handle.

	   @param aBDAddr A client supplied variable that, on successful
	   completion of this operation, will be populated with the
	   required Bluetooth device address.

	   @param aConnectionHandle A client supplied connection handle
	   for which the device address is required.

	   @return Status code.
	*/
	virtual TInt MplsGetRemoteAddress(TBTDevAddr& aBDAddr,
									  THCIConnHandle aConnectionHandle) const = 0;


	/**Returns the remote device's class for the device specified by
	   the Bluetooth device address.

	   @param aDeviceClass A client supplied variable that, on
	   successful completion of this operation, will be populated with
	   the required Bluetooth device class.

	   @param aBDAddr A client supplied Bluetooth device address for
	   which the device class is required.

	   @return Status code.
	*/
	virtual TInt MplsGetRemoteDeviceClass(TBTDeviceClass& aDeviceClass,
										  const TBTDevAddr& aBDAddr) const = 0;


	/**Returns the Bluetooth capabilities of the remote device
	   specified by the Bluetooth device address.

	   @param aRemoteSupportedFeatures A client supplied variable
	   that, on successful completion of this operation, will be
	   populated with the required Bluetooth remote supported
	   features.

	   @param aBDAddr A client supplied Bluetooth device address for
	   which the device class is required.

	   @return Status code.
	*/
	virtual TInt MplsGetRemoteSupportedFeatures(TBTFeatures& aRemoteSupportedFeatures,
											const TBTDevAddr& aBDAddr) const = 0;


	/**Returns the link policy settings for the physical link with the
	   remote device specified by the Bluetooth device address
	   parameter

	   @param aLinkPolicySettings A client supplied variable that, on
	   successful completion of this operation, will be populated with
	   the required Bluetooth link policy settings

	   @param aBDAddr A client supplied Bluetooth device address for
	   which the device class is required.

	   @return Status code.
	*/
	virtual TInt MplsGetLinkPolicySettings(TLinkPolicy& aLinkPolicySettings,
									   const TBTDevAddr& aBDAddr) const = 0;


	/**Returns the link state for the device specified by the
	   Bluetooth device address.  The state indicates whether the link
	   is up, down or pending. If the link is up, the following
	   information may be determined: the role (master or slave),
	   power mode (link mode, e.g Sniff Mode), max slots (currently 1,
	   3 or 5), whether the link is encrypted, and whether the link is
	   authenticated.

	   @see TBTBasebandLinkState

	   @param aBasebandLinkState A client supplied variable that, on
	   successful completion of this operation, will be populated with
	   the required Bluetooth link state.

	   @param aBDAddr A client supplied Bluetooth device address for
	   which the device class is required.

	   @return Status code.
	*/
	virtual TInt MplsGetBasebandLinkState(TBTBasebandLinkState& aBasebandLinkState,
									  const TBTDevAddr& aBDAddr) const = 0;
	};


#endif //PHYSICALLINKSSTATE_H
