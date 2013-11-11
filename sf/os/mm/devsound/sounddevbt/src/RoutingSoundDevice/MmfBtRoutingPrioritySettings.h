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

#ifndef __MMFBTROUTINGPRIORITYSETTINGS_H__
#define __MMFBTROUTINGPRIORITYSETTINGS_H__

// Routing speaker / BT headset
/**
@publishedPartner
@prototype

The values to specify which device the audio should route to.
This is used in the TMdaPrioritySettings iPref
Defaults to speaker output.
Note only the EA2DPBluetoothHeadset, needs to be specified
*/
enum TSoundDeviceRoute
	{
	EA2DPBluetoothHeadset	= 0x10000000,
	};

/**
*/
const TUid KDeviceUidSpeaker= {0x1020E4D0};
const TUid KDeviceUidMic = {0x1020E4D1};
const TUid KDeviceUidA2dpHeadset= {0x10204979}; 


#endif
