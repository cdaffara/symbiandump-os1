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
*                Orientation.
*
*/



#ifndef TEFORIENTATION_H
#define TEFORIENTATION_H

//  INCLUDES
#include <e32std.h>
#include <AudioEffectData.h>
// CLASS DECLARATION

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib OrientationEffect.lib
*  @since 3.0
*/
class TEfOrientation : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfOrientation() {}

        /**
        * Constructor.
        */
        TEfOrientation( TInt32 aHeading, TInt32 aPitch, TInt32 aRoll,
        			 	TInt32 aFrontX, TInt32 aFrontY, TInt32 aFrontZ,
        			 	TInt32 aAboveX, TInt32 aAboveY, TInt32 aAboveZ ) :
        			 	iHeading(aHeading), iPitch(aPitch), iRoll(aRoll),
        			    iFrontX(aFrontX), iFrontY(aFrontY), iFrontZ(aFrontZ),
        			    iAboveX(aAboveX), iAboveY(aAboveY), iAboveZ(aAboveZ) {}

    	// Data
        // Orientation values of a Orientation object
		TInt32 iHeading;
		TInt32 iPitch;
		TInt32 iRoll;
		TInt32 iFrontX;
		TInt32 iFrontY;
		TInt32 iFrontZ;
		TInt32 iAboveX;
		TInt32 iAboveY;
		TInt32 iAboveZ;

    };

typedef TPckgBuf<TEfOrientation> TEfOrientationDataPckg;

#endif      // TEFORIENTATION_H

// End of File
