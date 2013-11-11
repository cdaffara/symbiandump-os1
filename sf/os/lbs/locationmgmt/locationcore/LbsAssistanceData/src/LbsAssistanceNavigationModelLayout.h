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


#ifndef LBSASSISTANCENAVIGATIONMODELLAYOUT_H
#define LBSASSISTANCENAVIGATIONMODELLAYOUT_H

#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistanceenum.h>

struct TSubFrame1ReservedArrayLayout
/**
These are reserved bits in the subframe 1 of the GPS navigation message - an array 
of 4 elements (23, 24, 24 and 16 bits in size).
*/
    {
    TUint iArray[4];
    };

struct TEphemerisParameterLayout
	{
	TUint iCodeOnL2;
	TUint iUraIndex;
	TUint iSatHealth;
	TUint iIodc;
	TUint iL2Pflag;
	TUint iTGD;
	TUint iToc;
	TUint iAf2;
	TUint iAf1;
	TUint iAf0;
	TUint iCrs;
	TUint iDeltaN;
	TUint iM0;
	TUint iCuc;
	TUint iE;
	TUint iCus;
	TUint iASqrt;
	TUint iToe;
	TUint iFitInterval;
	TUint iAodo;
	TUint iCic;
	TUint iOmega0;
	TUint iCis;
	TUint iI0;
	TUint iCrc;
	TUint iOmega;
	TUint iOmegaDot;
	TUint iIDot;
	TSubFrame1ReservedArrayLayout iSf1Revd; // These are reserved bits in the subframe 1 of the GPS 
					// navigation message - an array of 4 elements (23, 24, 24 and 16 bits in size).
	};

struct TNavigationModelSatInfoLayout
	{
	TUint iSatId;
	TSatelliteStatus iSatelliteStatus;
	TEphemerisParameterLayout iEphemerisParameter;
	};
	
struct TNavigationModelSatInfoArrayLayout
	{
	TNavigationModelSatInfoLayout iArray[32];
	};

struct TUEPositioningGpsNavigationModelLayout
	{
	TNavigationModelSatInfoArrayLayout iNavigationModelSatelliteInfoArray;
	};


#endif //LBSASSISTANCENAVIGATIONMODELLAYOUT_H

