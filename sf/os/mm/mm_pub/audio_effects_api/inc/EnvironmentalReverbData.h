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
*                Reverb.
*
*/



#ifndef TEFENVIRONMENTALREVERB_H
#define TEFENVIRONMENTALREVERB_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>

// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib EnvironmentalReverbEffect.lib
*  @since 3.0
*/
class TEfEnvironmentalReverb : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfEnvironmentalReverb() {}

    	// Data
        // Reverb values of a Reverb object

		TUint32 iDecayHFRatio;
		TUint32 iDecayHFRatioMin;
		TUint32 iDecayHFRatioMax;
		TUint32 iDecayTime;
		TUint32 iDecayTimeMin;
		TUint32 iDecayTimeMax;
		TUint32 iDensity;
		TUint32 iDiffusion;
		TInt32 iReflectionsLevel;
		TInt32 iReflectionLevelMin;
		TInt32 iReflectionLevelMax;
		TUint32 iReflectionsDelay;
		TUint32 iReflectionsDelayMax;
		TUint32 iReverbDelay;
		TUint32 iReverbDelayMax;
		TInt32 iReverbLevel;
		TInt32 iReverbLevelMin;
		TInt32 iReverbLevelMax;
		TInt32 iRoomHFLevel;
		TInt32 iRoomHFLevelMin;
		TInt32 iRoomHFLevelMax;
		TInt32 iRoomLevel;
		TInt32 iRoomLevelMin;
		TInt32 iRoomLevelMax;
		TUint32 iDelayMax;
		TUint32 iEnvironmentalReverbId;

	};

typedef TPckgBuf<TEfEnvironmentalReverb> TEfEnvReverbDataPckg;

#endif      // TEFENVIRONMENTALREVERB_H

// End of File
