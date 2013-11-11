// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology
 
*/

#ifndef NET_PM_UTILS_H
#define NET_PM_UTILS_H

//************************************************************************************************************
#include <e32base.h>

// Literals used
_LIT(KNetPMUtils, "NetPMUtils");

/**
Constants used in the encoding of position values.
*/
const TReal KLbsLatitudeConst  = (8388608 / 90);  // 2^23 / 90
const TReal KLbsLongitudeConst = (16777216 / 360);  // 2^24 / 360
const TInt  KLbsMaxAltitude    = 32767;          	// 2^15 - 1

/**
Constants used in velocity conversion
1 meter per second = 3.6 kilometers per hour
*/
const TReal KLbsMpsKmphConstant = 3.6;

class NetPmUtils
    {
	public:
	
	public: // New functions
        IMPORT_C static TInt NetPmUtils::Uncertainty(const TReal32& aDistance);
        IMPORT_C static TInt EncodeAltitude(const TReal32& aAltitude);
        IMPORT_C static TInt UncertaintyAltitude(const TReal32& aDistance);
	private:
    	NetPmUtils( const NetPmUtils& );
    };


#endif //NET_PM_UTILS_H
