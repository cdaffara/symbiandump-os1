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
*                location.
*
*/



#ifndef TEFLOCATION_H
#define TEFLOCATION_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>

// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib LocationEffect.lib
*  @since 3.0
*/



class TEfLocation : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfLocation() {}

        /**
        * Constructor.
        */
        TEfLocation( TInt32 aXCoordinate, TInt32 aYCoordinate, TInt32 aZCoordinate,
        			 TInt32 aAzimuth, TInt32 aElevation, TInt32 aRadius ):
        	iXCoordinate(aXCoordinate), iYCoordinate(aYCoordinate), iZCoordinate(aZCoordinate),
        	iAzimuth(aAzimuth), iElevation(aElevation), iRadius(aRadius) {}

    	// Data
        // Location values of a location object
		TInt32 iXCoordinate;
		TInt32 iYCoordinate;
		TInt32 iZCoordinate;
		TInt32 iAzimuth;
		TInt32 iElevation;
		TInt32 iRadius;

    };

typedef TPckgBuf<TEfLocation> TEfLocationDataPckg;

#endif      // TEFLOCATION_H

// End of File
