// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSASSISTANCERRLPREFERENCETIMELAYOUT_H
#define LBSASSISTANCERRLPREFERENCETIMELAYOUT_H

/**
 @file
 @internalComponent
*/

#include <e32def.h>
#include <lbs/lbsassistanceenum.h>

struct TRrlpGpsReferenceTimeLayout
	{
    /** This uses 10 bits */
    TUint iBcchCarrier;
    /** This uses 6 bits */
    TUint iBsic;
    /** This uses 21 bits and represents the number of frames */
    TUint iFrameNumber;
    /** This uses 3 bits and represents the number of timeslots */
    TUint iTimeslotsNumber;
    /** This uses 8 bits and represents the number of bits */
    TUint iBitsNumber;
	};


#endif //LBSASSISTANCERRLPREFERENCETIMELAYOUT_H

