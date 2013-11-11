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

#ifndef LBSASSISTANCEREFERENCELOCATIONLAYOUT_H
#define LBSASSISTANCEREFERENCELOCATIONLAYOUT_H

#include <e32def.h>

struct TEllipsoidPointAltitudeEllipsoideLayout
	{
	TInt iLatitudeSign;
	TUint iLatitude;
	TInt iLongitude;
	TInt iAltitudeDirection;
	TUint iAltitude;
	TUint iUncertaintySemiMajor;
	TUint iUncertaintySemiMinor;
	TUint iOrientationMajorAxis;
	TUint iUncertaintyAltitude;
	TUint iConfidence;
	};
	
struct TReferenceLocationLayout
	{
	TEllipsoidPointAltitudeEllipsoideLayout iEllipsoidPointAltitudeEllipsoide;
	};


#endif //LBSASSISTANCEREFERENCELOCATIONLAYOUT_H

