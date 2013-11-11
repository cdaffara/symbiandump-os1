/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Supported features for the current environment.
*
*/


#ifndef FEATUREINFO_H
#define FEATUREINFO_H

/**
	Device supports Multimedia Card (MMC).
*/
	const TInt KFeatureIdMmc = 1;

/**
	Device has Camera Application.
*/
	const TInt KFeatureIdCamera = 2;

/**
	Device has also separate cover display in addition to the main display.
*/
	const TInt KFeatureIdCoverDisplay = 3;

/**
	Device has Video Recorder (CamCorder) application available.
*/
	const TInt KFeatureIdVideoRecorder = 208;

/**
	Indicates whether a device has qwerty keyboard or not.
*/
	const TInt KFeatureIdQwertyInput = 409;

/**
	Device suppors flight mode. Device can be used off-line. RF and Bluetooth
	transmitters are inactive and emergency call is not possible.
*/
	const TInt KFeatureIdFlightMode = 6;

/**
	Device suppors off-line mode. Device can be used off-line and Bluetooth
	transmitter can be enabled. RF transmitter can be activated only for emergency
	calls.
*/
	const TInt KFeatureIdOfflineMode = 7;

/**
	Device supports OpenGL ES and EGL versions 1.0. This feature enables 3D
	graphics rendering
*/
	const TInt KFeatureIdOpenGLES3DApi = 10;

/**
	Device supports Infrared.
*/
	const TInt KFeatureIdIrda = 11;

/**
	Device supports Bluetooth.
*/
	const TInt KFeatureIdBt = 12;

/**
	Device supports Universal Serial Bus (USB).
*/
	const TInt KFeatureIdUsb = 13;

/**
	Device supports Bluetooth fax profile.
*/
	const TInt KFeatureIdBtFaxProfile = 17;

/**
	Device supports bluetooth imaging profile.
*/
	const TInt KFeatureIdBtImagingProfile = 18;

/**
	Device supports Instant Messaging feature of the Wireless Village initiative
*/
	const TInt KFeatureIdIm = 33;

/**
	Device supports Presence feature of the Wireless Village initiative.
*/
	const TInt KFeatureIdPresence = 36;

/**
	Device supports SyncML device management standard. This feature enables Over
	The Air (OTA) administration of the device settings.
*/
	const TInt KFeatureIdSyncMlDm = 41;

/**
	Device supports SyncML data synchronization standard. This feature enables
	Over The Air (OTA) synchronisation of device data.
*/
	const TInt KFeatureIdSyncMlDs = 42;

/**
	Device supports Scalable Vector Graphics - Tiny (SVGT) file format. The
	feature enables viewing 2D vector graphics animations specified by the W3C.
*/
	const TInt KFeatureIdSvgt = 77;

/**
	Device supports utility APIs and framework for Series 60 Audio applications to
	enable predefined audio effects with audio play/record utilities. Features
	like Volume, Graphical Equalizer, Reverb are possible.
*/
	const TInt KFeatureIdAudioEffectsApi = 497;

/**
	Device supports Audio Resource Indications Utility. Applications can use the
	Audio Resource Indications Utility to request feedack when they can submit a
	play request.
*/
	const TInt KFeatureIdAudioResourceIndications = 499;

/**
	Device supports equalizer.
*/
	const TInt KFeatureIdEqualizer = 1502;

/**
	Device supports GSM cellular stack.
*/
	const TInt KFeatureIdProtocolGsm = 81;

/**
	Device supports WCDMA cellular stack
*/
	const TInt KFeatureIdProtocolWcdma = 82;

/**
	Device supports CDMA cellular stack.
*/
	const TInt KFeatureIdProtocolCdma = 83;

/**
	Device supports circuit switched video telephony
*/
	const TInt KFeatureIdCsVideoTelephony = 89;

/**
	Device supports Quality of Service (QOS). The QoS is capability of a network
	to provide better service to selected network traffic.
*/
	const TInt KFeatureIdQos = 101;

/**
	Device supports IP Security (IPSec). The IPSec is a set of protocols
	developed by the IETF to support secure exchange of packets at the IP layer.
	IPsec has been deployed widely to implement Virtual Private Networks (VPNs). 
*/
	const TInt KFeatureIdIpsec = 102;

/**
	Devices supports Mobile IPv6.
*/
	const TInt KFeatureIdMIPv6 = 106;

/**
	Device supports WLAN protocol.
*/
	const TInt KFeatureIdProtocolWlan = 109;

/**
	Device supports SIP protocol.
*/
	const TInt KFeatureIdSipStack = 111;

/**
	Devices supports Mobile IPv4
*/
	const TInt KFeatureIdMIPv4 = 312;

/**
	Defines whether location landmarks API is included in device.
*/
	const TInt KFeatureIdLandmarks = 112;

/**
	Device supports location basic services.
*/
	const TInt KFeatureIdLocationFrameworkCore = 114;

/**
	Camera application supports burst mode. It enables taking several snapshots
	with one command.
*/
	const TInt KFeatureIdCameraBurstMode = 132;

/**
	Device application UIs use Europe Look And Feel (LAF).
*/
	const TInt KFeatureIdAvkonELaf = 1010;

/**
	Device application UIs use Asia Pacific Look And Feel (LAF).
*/
	const TInt KFeatureIdAvkonApac = 1015;

/**
	Device supports Musical Instrument Digital Interface (MIDI) sounds.
*/
	const TInt KFeatureIdMidi = 1011;

/**
	Device supports MMC hotswap. A MMC card can be inserted and removed from
	device without removing battery or rebooting the device
*/
	const TInt KFeatureIdMmcHotswap = 1076;

/**
	Device uses Japanese language variant.
*/
	const TInt KFeatureIdJapanese = 1080;

/**
	Device uses Thai language variant.
*/
	const TInt KFeatureIdThai = 1081;

/**
	Device supports Unstructured Suppelementary Service Data (USSB).
*/
	const TInt KFeatureIdUSSD = 1091;

/**
	Device uses Chinese language variant.
*/
	const TInt KFeatureIdChinese = 1096;

#endif
