/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains definitions of audio effects data structures for
*                equalizer.
*
*/



#ifndef AUDIOEQUALIZERUTILITYDATA_H
#define AUDIOEQUALIZERUTILITYDATA_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  This class defines the Equalizer Effect Utility data structure.
*
*  @lib AudioEqualizerUtility.lib
*  @since 3.0
*/
class TEfAudioEqualizerUtilityPreset
    {
    public:

        /**
        * Constructor.
        */
        TEfAudioEqualizerUtilityPreset() : iPresetNameKey(0),iPresetDescriptionKey(0),iPresetName(NULL)
        {}

		TUint32 iPresetNameKey;
		TUint32 iPresetDescriptionKey;
		TBuf16<32>  iPresetName;

    };

#endif      // AUDIOEQUALIZERUTILITYDATA_H

// End of File
