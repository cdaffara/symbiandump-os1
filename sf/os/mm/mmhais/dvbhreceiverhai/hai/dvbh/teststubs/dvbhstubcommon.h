// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DVB-H types common to the test stub implementation.
// 
//

/**
 @file
 @internalComponent
 @prototype
*/

#ifndef DVBHSTUBCOMMON_H
#define DVBHSTUBCOMMON_H

#include <e32cmn.h>
#include <in_sock.h>
#include "dvbhtypes.h"

/**
* @internalComponent
* @prototype
*
* Publish & Subscribe variables used for implementation to API communication.
*/	
const TUid KDvbhPropertyCategory = 
	{
	0x101f45ef //TestFramework's UID3
	};
	
const TUint KDvbhPropertyKeyState = 0;      	 // integer (TDvbhState)
const TUint KDvbhPropertyKeyPlatform = 1;   	 // byte array (TDvbhPlatform + TIp6Addr)
const TUint KDvbhPropertyKeyNetworkTime = 2; 	 // byte array (TDvbhNetworkTime)
const TUint KDvbhPropertyKeyPerformanceData = 3; // byte array (TDvbhPerformanceData)
const TUint KDvbhPropertyKeyFrequency = 4; 		 // integer (TDvbhFrequency)
const TUint KDvbhPropertyKeyNetworkId = 5; 		 // integer (TDvbhNetworkId)
const TUint KDvbhPropertyKeySignalQuality = 6; 	 // integer (TDvbhSignalQuality)
const TUint KDvbhPropertyKeyCellId = 7; 		 // integer (TDvbhCellId)
const TUint KDvbhPropertyKeyExtBatteryState = 8; 	 // integer (TDvbhExtBatteryState)
const TUint KDvbhPropertyKeyExtConnectionState = 9; 	 // integer (TDvbhExtConnectionState)

const TUint KMtvAccAttachedKey = 20; // integer (TMtvAccConnectionState)
const TUint KMtvAccAntennaKey = 21; // integer (TMtvAccAntennaConnectionState)
const TUint KMtvAccInfoKey = 22; // byte array (TMtvAccInfo)

struct TDvbhPlatformProperty
	{
	TDvbhPlatform iPlatform;
	TIp6Addr iEsgRoot;
	};
	
#endif // DVBHSTUBCOMMON_H
