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
*                StereoWidening.
*
*/



#ifndef STEREOWIDENINGUTILITYDATA_H
#define STEREOWIDENINGUTILITYDATA_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  This class defines the StereoWidening Effect Utility data structure.
*
*  @lib StereoWideningUtility.lib
*  @since 3.0
*/
class TEfStereoWideningUtilityPreset
    {
    public:

        /**
        * Constructor.
        */
        TEfStereoWideningUtilityPreset() : iPresetNameKey(0),iPresetDescriptionKey(0),iPresetName(NULL)
        {}

		TUint32 iPresetNameKey;
		TUint32 iPresetDescriptionKey;
		TBuf16<32>  iPresetName;

    };

#endif      // STEREOWIDENINGUTILITYDATA_H

// End of File
