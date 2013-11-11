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

#ifndef LBSASSISTANCEAQUISITIONASSISTANCELAYOUT_H
#define LBSASSISTANCEAQUISITIONASSISTANCELAYOUT_H

#include <e32def.h>
#include "LbsAssistanceUtranReferenceTimeLayout.h"
#include "LbsAssistanceRrlpReferenceTimeLayout.h"


struct TAzimuthAndElevationLayout
	{
	TUint iAzimuth;
	TUint iElevation;
	};

struct TExtraDopplerInfoLayout
	{
	TInt iDoppler1stOrder;
	TDopplerUncertainty iDopplerUncertainty;
	};

struct TAcquisitionSatInfoLayout
	{
	TUint iSatID;
	TInt iDoppler0thOrder;
	TExtraDopplerInfoLayout iExtraDopplerInfo;
	TUint iCodePhase;
	TUint iIntegerCodePhase;
	TUint iGpsBitNumber;
	TCodePhaseSearchWindow iCodePhaseSearchWindow;
	TAzimuthAndElevationLayout iAzimuthAndElevation;
	};

struct TAcquisitionSatInfoArrayLayout
	{
	TAcquisitionSatInfoLayout iArray[32];
	};

struct TUEPositioningGpsAcquisitionAssistanceLayout
	{
	TUint iGpsReferenceTime;
	TUtranGpsReferenceTimeLayout iUtranGpsReferenceTime;
	TAcquisitionSatInfoArrayLayout iSatelliteInformationArray;
    TRrlpGpsReferenceTimeLayout iRrlpGpsReferenceTime;	
	};



#endif //LBSASSISTANCEAQUISITIONASSISTANCELAYOUT_H

