/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of central repository keys for StereoWidening utility.
*
*
*/


#ifndef STEREOWIDENINGINTERNALCRKEYS_H
#define STEREOWIDENINGINTERNALCRKEYS_H

const TUid KCRUidStereoWideningPresets = {0x10207A84};

// Maximum Number of Pre-defined Presets
const TUint32 KStereoWideningMaxNumberOfPreDefinedPresets =  	0x00000001;

// Maximum Number of User-defined Presets
const TUint32 KStereoWideningMaxNumberOfUserDefinedPresets =  	0x00000002;

// Current number of Pre-defined Presets
const TUint32 KStereoWideningNumberOfPreDefinedPresets =  		0x00000003;

// Number of currently User-defined Presets
const TUint32 KStereoWideningNumberOfUserDefinedPresets =  		0x00000004;

// Number of currently User-defined Presets
const TUint32 KStereoWideningPresetsCounter =  					0x00000006;

// Pre-defined Preset keys start
const TUint32 KStereoWideningPreDefinedPresetStart =  			0x00000032;
const TUint32 KStereoWideningPreDefinedPresetStartName =  		0x00000033;

// Subsequent keys are added starting from KStereoWideningPreDefinedPresetStart + 2 ending at 0x80000000

// User-defined Preset keys start
const TUint32 KStereoWideningUserDefinedPresetStart =  			0x80000001;
const TUint32 KStereoWideningUserDefinedPresetStartName =  		0x80000002;

// Subsequent keys are added starting from KStereoWideningUserDefinedPresetStart + 2 ending at 0xFFFFFFFF

#endif      // STEREOWIDENINGINTERNALCRKEYS_H