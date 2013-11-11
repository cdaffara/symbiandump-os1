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
*                Distance Attenuation.
*
*/



#ifndef DISTANCEATTENUATIONDATA_H
#define DISTANCEATTENUATIONDATA_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>

// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib DistanceAttenuationEffect.lib
*  @since 3.0
*/
class TEfDistanceAttenuation : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfDistanceAttenuation() {}

        /**
        * Constructor.
        */
        TEfDistanceAttenuation( TInt32 aRMin, TInt32 aRMax, TBool aMuteAfterMax,
        						TUint32 aRollOffFactor, TUint32 aRoomRollOffFactor) :
        			 			iRMax(aRMax), iRMin(aRMin), iMuteAfterMax(aMuteAfterMax),
        			 			iRollOffFactor(aRollOffFactor), iRoomRollOffFactor(aRoomRollOffFactor) {}

    	// Data
		TInt32 iRMin;
		TInt32 iRMax;
		TBool iMuteAfterMax;
		TUint32 iRollOffFactor;
		TUint32 iRoomRollOffFactor;
		TUint32 iRollOffFactorMax;
		TUint32 iRoomRollOffFactorMax;

    };

typedef TPckgBuf<TEfDistanceAttenuation> TEfDistanceAttenuationDataPckg;

#endif      // DISTANCEATTENUATIONDATA_H

// End of File
