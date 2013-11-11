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



#ifndef TEFBASSBOOSTDATA_H
#define TEFBASSBOOSTDATA_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>

// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib BassBoostDataUtility.lib
*  @since 3.0
*/
class TEfBassBoostData : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfBassBoostData() {}

		// No effect specifc data. Maybe we don't need one for this effect.
    };

typedef TPckgBuf<TEfBassBoostData> TEfBassBoostDataPckg;

#endif      // TEFBASSBOOSTDATA_H

// End of File
