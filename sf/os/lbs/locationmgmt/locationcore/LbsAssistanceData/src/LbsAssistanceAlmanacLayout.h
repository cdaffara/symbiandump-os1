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

#ifndef LBSASSISTANCEALMANACLAYOUT_H
#define LBSASSISTANCEALMANACLAYOUT_H

#include "LbsAssistanceInternal.h"
#include <e32def.h>

struct TAlmanacSatInfoLayout
	{
	TUint iDataID;
	TUint iSatID;
	TUint iE;
	TUint iToa;
	TUint iDeltaI;
	TUint iOmegaDot;
	TUint iSatHealth;
	TUint iASqrt;
	TUint iOmega0;
	TUint iM0;
	TUint iOmega;
	TUint iAf0;
	TUint iAf1;
	};

struct TAlmanacSatInfoArrayLayout
	{
	TAlmanacSatInfoLayout iArray[32];
	};


struct TUEPositioningGpsAlmanacLayout
	{
	TUint iWnA;
	TAlmanacSatInfoArrayLayout iAlmanacInfoSatArray;
	TDes8PackMarker<46> iSvGlobalHealth;
	};


#endif //LBSASSISTANCEALMANACLAYOUT_H

