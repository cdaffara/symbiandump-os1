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
namespace NFeature {
	// default present
	const TUid KBluetooth = {0x10279808};
	const TUid KConnectivity = {0x10279816};
	const TUid KDhcp = {0x10279815};
	const TUid KFax = {0x10279806};
	const TUid KInfrared = {0x10279809};
	const TUid KIPQoS = {0x10279812};
	const TUid KIPSec = {0x10279814};
	const TUid KLocationManagement = {0x10279818};
	const TUid KMmc = {0x1027980A};
	const TUid KMTP = {0x10279817};
	const TUid KNetworkQoS = {0x10279813};
	const TUid KObex = {0x1027980C};
	const TUid KOmaDataSync = {0x10279810};
	const TUid KOmaDeviceManagement = {0x10279811};
	const TUid KPrint = {0x10279807};
	const TUid KRtpRtcp = {0x1027980D};
	const TUid KSip = {0x1027980F};
	const TUid KUsb = {0x1027980B};

	// default not present
	const TUid KAGPSPositioningPlugin = {0x10286AA2};
	const TUid KAmbientLightSensor = {0x102835AF};
	const TUid KBluetoothGPSPositioningPlugin = {0x10285D6A};
	const TUid KCoverDisplay = {0x102835B0};
	const TUid KCsVideoTelephony = {0x10285A22};
	const TUid KDefaultPositioningPlugin = {0x10285D6B};
	const TUid KDriveModeCanRestrictMtCalls = {0x10285A24};
	const TUid KDRM = {0x1028181B};
	const TUid KEmergencyCallsEnabledInOfflineMode = {0x10285A23};
	const TUid KFmTx = {0x10285DD6};
	const TUid KGPSPositioningPlugin = {0x10286AA1};
	const TUid KKeypadNoSlider = {0x102835B1};
	const TUid KLocation = {0x10281818};
	const TUid KLocationAdvancedDialog = {0x102859F3};
	const TUid KLocationAPIVariant2 = {0x10285D69};
	const TUid KLocationPrivacyRequestAPIs = {0x102859F2};
	const TUid KMobileIP = {0x10281819};
	const TUid KNetworkPositioningPlugin = {0x10286AA3};
	const TUid KOfflineMode = {0x1028181A};
	const TUid KOmaDsHostServers = {0x10282663};
	const TUid KSimulationPositioningPlugin = {0x10285D6C};
	const TUid KVibra = {0x102835AE};
}
