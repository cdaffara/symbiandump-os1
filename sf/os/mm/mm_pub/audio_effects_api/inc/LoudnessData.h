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
*                Echo Cancellation Data.
*
*/



#ifndef TEFLOUDNESSDATA_H
#define TEFLOUDNESSDATA_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>

// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib LoudnessEffect.lib
*  @since 3.0
*/
class TEfLoudnessData : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfLoudnessData() {}

		// No specific data
    };

typedef TPckgBuf<TEfLoudnessData> TEfLoudnessDataPckg;

#endif      // TEFLOUDNESSDATA_H

// End of File
