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
* Description:  Definition of central repository keys for audio EnvironmentalReverb utility.
*
*
*/


#ifndef ENVIRONMENTALREVERBUTILITYINTERNALCRKEYS_H
#define ENVIRONMENTALREVERBUTILITYINTERNALCRKEYS_H

const TUid KCRUidEnvironmentalReverbUtilityPresets = {0x10207A7F};

// Maximum Number of Pre-defined Presets
const TUint32 KEnvironmentalReverbUtilityMaxNumberOfPreDefinedPresets =  	0x00000001;

// Maximum Number of User-defined Presets
const TUint32 KEnvironmentalReverbUtilityMaxNumberOfUserDefinedPresets =  	0x00000002;

// Current number of Pre-defined Presets
const TUint32 KEnvironmentalReverbUtilityNumberOfPreDefinedPresets =  		0x00000003;

// Number of currently User-defined Presets
const TUint32 KEnvironmentalReverbUtilityNumberOfUserDefinedPresets =  		0x00000004;

// Number of currently User-defined Presets
const TUint32 KEnvironmentalReverbUtilityPresetsCounter =  					0x00000006;

// Pre-defined Preset keys start
const TUint32 KEnvironmentalReverbUtilityPreDefinedPresetStart =  			0x00000032;
const TUint32 KEnvironmentalReverbUtilityPreDefinedPresetStartName =  		0x00000033;

// Subsequent keys are added starting from KEnvironmentalReverbUtilityPreDefinedPresetStart + 2 ending at 0x80000000

// User-defined Preset keys start
const TUint32 KEnvironmentalReverbUtilityUserDefinedPresetStart =  			0x80000001;
const TUint32 KEnvironmentalReverbUtilityUserDefinedPresetStartName =  		0x80000002;

// Subsequent keys are added starting from KEnvironmentalReverbUtilityUserDefinedPresetStart + 2 ending at 0xFFFFFFFF

#endif      // ENVIRONMENTALREVERBUTILITYINTERNALCRKEYS_H