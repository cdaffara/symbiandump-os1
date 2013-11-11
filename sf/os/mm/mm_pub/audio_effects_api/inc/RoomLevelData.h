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
*                RoomLevel.
*
*/



#ifndef TEFROOMLEVEL_H
#define TEFROOMLEVEL_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>


// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib RoomLevelEffect.lib
*  @since 3.0
*/
class TEfRoomLevel : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfRoomLevel() {}

    	// Data
        // RoomLevel values of a RoomLevel object

        TUint32 iEnvironmentalReverbId;		
		TInt32 iStreamRoomLevel;
     	TInt32 iStreamMinRoomLevel;
	    TInt32 iStreamMaxRoomLevel;
	};

typedef TPckgBuf<TEfRoomLevel> TEfRoomLevelDataPckg;

#endif      // TEFROOMLEVEL_H

// End of File
