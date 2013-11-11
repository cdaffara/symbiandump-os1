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


#include <kernel/kern_priv.h>
#include "display_dev.h"
#include "display_chan.h"


DECLARE_STANDARD_LDD()
	{
	return new DDisplayDevice;
	}


DDisplayDevice::DDisplayDevice()
	{
	iVersion = TVersion(KDisplayChMajorVersionNumber,
			KDisplayChMinorVersionNumber, KDisplayChBuildVersionNumber);
	iParseMask=KDeviceAllowUnit;
	}


DDisplayDevice::~DDisplayDevice()
	{
	}


TInt DDisplayDevice::Install()
	{
	return SetName(&RDisplayChannel::Name());
	}


/**
Called by the kernel's device driver framework to create a Logical Channel.
This is called in the context of the user thread (client) which requested the
creation of a Logical Channel (e.g. through a call to RBusLogicalChannel::DoCreate)
The thread is in a critical section.

@param aChannel Set to point to the created Logical Channel

@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DDisplayDevice::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel = new DDisplayChannel();

	return aChannel ? KErrNone : KErrNoMemory;
	}


/**
Return the drivers capabilities.
Called in the response to an RDevice::GetCaps() request.

@param aDes User-side descriptor to write capabilities information into
*/
void DDisplayDevice::GetCaps(TDes8& aDes) const
	{
	// Create a capabilities object
	DDisplayChannel::TCaps caps;
	caps.iVersion = iVersion;
	// Write it back to user memory
	Kern::InfoCopy(aDes,reinterpret_cast<TUint8*>(&caps),sizeof(caps));
	}

