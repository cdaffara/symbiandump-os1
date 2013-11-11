// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEVVIDEOHWDEVICETUNNELCUSTOMINTERFACE_H
#define DEVVIDEOHWDEVICETUNNELCUSTOMINTERFACE_H

#include <mmf/devvideo/devvideobase.h>

/**
KUidMmfVideoHwDeviceTunnel interface UID.

@publishedPartner
@released
*/
const TUid KUidMmfVideoHwDeviceTunnel = { 0x1028668C };

/**
HwDevice Tunneling custom interface.
Controllers or other DevVideo clients can use this interface in order to setup
a tunnel from one HwDevice to another. For instance this would allow a video
decoder to be tunneled to a video encoder for transcoding use cases.

Clients must query the HwDevice at both ends of the tunnel for the custom
interface in order to ensure that both devices support tunneled operation.

@publishedPartner
@released
*/
class MMmfVideoHwDeviceTunnel
	{
public:
	/**
	Get the instance of the HwDevice at this end of the tunnel.

	@return The HwDevice instance
	*/
	virtual CMMFVideoHwDevice* MmvhdtHwDevice() = 0; 

	/**
	Setup a tunnel to another HwDevice

	@param aHwDevice The HwDevice to setup the tunnel to
	*/
	virtual void MmvhdtSetupTunnelL(CMMFVideoHwDevice* aHwDevice) = 0;
	};

#endif // DEVVIDEOHWDEVICETUNNELCUSTOMINTERFACE_H
