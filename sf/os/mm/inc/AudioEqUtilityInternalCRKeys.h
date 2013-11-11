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
* Description:  Definition of central repository keys for audio equalizer utility.
*
*
*/


#ifndef AUDIOEQUTILITYINTERNALCRKEYS_H
#define AUDIOEQUTILITYINTERNALCRKEYS_H

const TUid KCRUidAudioEqUtilityPresets = {0x10203833};

// Maximum Number of Pre-defined Presets
const TUint32 KAudioEqUtilityMaxNumberOfPreDefinedPresets =  	0x00000001;

// Maximum Number of User-defined Presets
const TUint32 KAudioEqUtilityMaxNumberOfUserDefinedPresets =  	0x00000002;

// Current number of Pre-defined Presets
const TUint32 KAudioEqUtilityNumberOfPreDefinedPresets =  		0x00000003;

// Number of currently User-defined Presets
const TUint32 KAudioEqUtilityNumberOfUserDefinedPresets =  		0x00000004;

// Number of currently User-defined Presets
const TUint32 KAudioEqUtilityPresetsCounter =  					0x00000006;

// Pre-defined Preset keys start
const TUint32 KAudioEqUtilityPreDefinedPresetStart =  			0x00000032;
const TUint32 KAudioEqUtilityPreDefinedPresetStartName =  		0x00000033;

// Subsequent keys are added starting from KAudioEqUtilityPreDefinedPresetStart + 2 ending at 0x80000000

// User-defined Preset keys start
const TUint32 KAudioEqUtilityUserDefinedPresetStart =  			0x80000001;
const TUint32 KAudioEqUtilityUserDefinedPresetStartName =  		0x80000002;

// Subsequent keys are added starting from KAudioEqUtilityUserDefinedPresetStart + 2 ending at 0xFFFFFFFF

#endif      // AUDIOEQUTILITYINTERNALCRKEYS_H