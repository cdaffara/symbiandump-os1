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
// Header file: Constant definitions for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_constants.h
*/

#ifndef TSU_MDF_CODECAPI_CONSTANTS_H
#define TSU_MDF_CODECAPI_CONSTANTS_H

#include <mdf/codecapiuids.hrh>

#define KMaxDataTypeLength			126

#define KUidMdfProcessingUnit 		0x10273789
#define KUidHwDeviceAudioAdapter	0x10273791
#define KUidHwDeviceInvalidAudioAdapter	0x1099ABCD

// the file we are going to play from during tests
_LIT(KTestPlayFile, "c:\\mm\\mmf\\testfiles\\mdf\\depeche2.wav");

// the file we are going to record into during tests
_LIT(KTestRecordedFile, "c:\\Record.wav");
	
_LIT8(KMMFMimeTypeMp4v, "video/x-symbiantest");

// valid FourCC codes for PCM codecs
const TUint32 KMMFFourCCCodeTestPCM8 = 0x38505420;		//( ' ', 'T', 'P', '8' )
const TUint32 KMMFFourCCCodeTestPCMU8 = 0x38555054;		//('T', 'P', 'U', '8')

// PU Uids
const TUid KUidPuLoader = {KUidPuLoaderImplementation};
const TUid KUidPcmPu ={0x102737BB}; 
const TUid KUidSourceSinkPu = {0x102730BB};

// video PU / custom interface Uids
const TUid KUidVideoTestEncoderPu = {0x102737E4};
const TUid KUidVideoTestDecoderPu = {0x102737E6};
const TUid KMmfVideoRecordSettingsCustomInterface = {0x102737C7};
_LIT8(KPuManufacturerSymbian, "Symbian Ltd.");
_LIT8(KPuManufacturerInvalid, "AcmeIndustries");

// invalid customer interface Uid
const TUid KUidHwInvalidInterface = {0x1099ABCE};

// invalid FourCC codes
const TUint32 KMMFFourCCCodeTestGarbage1 = 0x39555520;		//( ' ', 'U', 'U', '9' )
const TUint32 KMMFFourCCCodeTestGarbage2 = 0x39555555;		//('U', 'U', 'U', '9')

// invalid PU Uid
const TUid KUidGarbagePu ={0x9027FFFF}; // doesn't exist

#endif // TSU_MDF_CODECAPI_CONSTANTS_H
