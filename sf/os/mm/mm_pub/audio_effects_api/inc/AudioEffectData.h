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
* Description:  This file contains definitions of common audio effects data structures.
*
*/



#ifndef AUDIOEFFECTDATA_H
#define AUDIOEFFECTDATA_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  This class defines the common effect data
*
*  @lib EffectBase.lib
*  @since 3.0
*/
class TEfCommon
    {
    public: // Constructor and Destructor

        /**
        * Constructor.
        */
        TEfCommon() :
        	iEnabled(EFalse), iEnforced(EFalse), iHaveUpdateRights(ETrue) {}

    public:	// Data
		// Flag to indicate whether the effect is enabled or not
		TBool iEnabled;
		// Flag to indicate wheter the effect is enforced
		TInt8 iEnforced;
		// Flag to indicate wheter the effect current has update rights
		TBool iHaveUpdateRights;
    };

#endif      // AUDIOEFFECTDATA_H

// End of File
