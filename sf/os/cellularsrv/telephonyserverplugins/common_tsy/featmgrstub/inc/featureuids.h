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

namespace NFeature {
	// default present
	const TUid KConnectivity = {0x10279816};
	const TUid KFax = {0x10279806};
	const TUid KIPQoS = {0x10279812};
	const TUid KLocationManagement = {0x10279818};
	const TUid KNetworkQoS = {0x10279813};
	const TUid KObex = {0x1027980C};
	const TUid KRtpRtcp = {0x1027980D};
	const TUid KSip = {0x1027980F};

	// default not present
	const TUid KAmbientLightSensor = {0x5F9};
	const TUid KBluetooth = {0xC};
	const TUid KBluetoothGPSPositioningPlugin = {0x7A};
	const TUid KCoverDisplay = {0x3};
	const TUid KCsVideoTelephony = {0x59};
	const TUid KDefaultPositioningPlugin = {0x10285D6B};
	const TUid KDhcp = {0x6B};
	const TUid KDriveModeCanRestrictMtCalls = {0x584};
	const TUid KDRM = {0x5B};
	const TUid KEmergencyCallsEnabledInOfflineMode = {0x126};
	const TUid KFmTx = {0x6A9};
	const TUid KInfrared = {0xB};
	const TUid KIPSec = {0x66};
	const TUid KKeypadNoSlider = {0x3F5};
	const TUid KLibxml2 = {0x10286747};
	const TUid KLibxml2DOMXPathAPI = {0x10286727};
	const TUid KLibxml2SAXParser = {0x10286707};
	const TUid KLocation = {0x72};
	const TUid KLocationAdvancedDialog = {0x102859F3};
	const TUid KLocationAPIVariant2 = {0x10285D69};
	const TUid KLocationPrivacyRequestAPIs = {0x102859F2};
	const TUid KMmc = {0x1};
	const TUid KMobileIP = {0x10281819};
	const TUid KMTP = {0x1F8};
	const TUid KOfflineMode = {0x7};
	const TUid KPrint = {0x5FF};
	const TUid KSimulationPositioningPlugin = {0x10285D6C};
	const TUid KUsb = {0xD};
	const TUid KVibra = {0x19B};
}
