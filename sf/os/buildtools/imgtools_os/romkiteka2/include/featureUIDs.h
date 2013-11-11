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
// Feature UID allocations for all Symbian OS platforms
// This file is managed by the System Design Authority (sda@symbian.com)
// To allocate a new feature UID, please email the SDA
// The following UID ranges are allocated for features 
// that default to reporting "feature present"
// 0x10279806 - 0x10281805
// All other UIDs default to reporting "feature not present"
// 
//

/**
 @publishedAll
 @released
*/
namespace NFeature
	{
	// default present
	const TUid KFax = {0x10279806};
	const TUid KPrint = {0x10279807};
	const TUid KBluetooth = {0x10279808};
	const TUid KInfrared = {0x10279809};
	const TUid KMmc = {0x1027980a};
	const TUid KUsb = {0x1027980b};
	const TUid KObex = {0x1027980c};
	const TUid KRtpRtcp = {0x1027980d};
	const TUid KSip = {0x1027980f};
	const TUid KOmaDataSync	= {0x10279810};
	const TUid KOmaDeviceManagement = {0x10279811};
	const TUid KIPQoS = {0x10279812};
	const TUid KNetworkQoS = {0x10279813};
	const TUid KIPSec = {0x10279814};
	const TUid KDhcp = {0x10279815};
	const TUid KConnectivity = {0x10279816};	// PC Connectivity
	const TUid KMTP = {0x10279817};

	// default not-present
	const TUid KLocation = {0x10281818};
	const TUid KMobileIP = {0x10281819};
	const TUid KOfflineMode = {0x1028181A};
	const TUid KDRM = {0x1028181B};
	const TUid KOmaDsHostServers = {0x10282663};	
	const TUid KVibra = {0x102835AE};
	const TUid KAmbientLightSensor = {0x102835AF};
	const TUid KCoverDisplay = {0x102835B0};
	const TUid KKeypadNoSlider = {0x102835B1};
	const TUid KCsVideoTelephony = {0x10285A22};
	const TUid KEmergencyCallsEnabledInOfflineMode = {0x10285A23};
	const TUid KDriveModeCanRestrictMtCalls = {0x10285A24}; 
	}
