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
*                stereo widening.
*
*/



#ifndef TEFSTEREOWIDENINGDATA_H
#define TEFSTEREOWIDENINGDATA_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>
// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib StereoWideningEffect.lib
*  @since 3.0
*/
class TEfStereoWidening : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfStereoWidening() {}

        /**
        * Constructor.
        */
        TEfStereoWidening( TUint8 aLevel, TBool aContinuousLevelSupported ) :
        	iLevel(aLevel), iContinuousLevelSupported(aContinuousLevelSupported) {}

    	// Data
        // Stereo widening level
		TUint8 iLevel;
        TBool iContinuousLevelSupported;
    };

typedef TPckgBuf<TEfStereoWidening> TEfStereoWideningDataPckg;

#endif      // TEFSTEREOWIDENINGDATA_H

// End of File
