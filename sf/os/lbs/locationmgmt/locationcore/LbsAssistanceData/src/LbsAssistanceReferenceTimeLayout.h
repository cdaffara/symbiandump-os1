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

#ifndef LBSASSISTANCEREFERENCETIMELAYOUT_H
#define LBSASSISTANCEREFERENCETIMELAYOUT_H

#include <e32def.h>
#include "LbsAssistanceUtranReferenceTimeLayout.h"
#include "LbsAssistanceRrlpReferenceTimeLayout.h"
#include <lbs/lbsassistanceenum.h>

struct TGpsTowAssistLayout
	{
	TUint iSatID;
	TUint iTlmMessage;
	TBool iAlert;
	TBool iAntiSpoof;
	TUint iTlmReserved;
	};

struct TGpsTowAssistArrayLayout
	{
	TGpsTowAssistLayout iArray[32];
	};

struct TUEPositioningGpsReferenceTimeLayout
	{
	TUint iGpsWeek;
	TUint iGpsTow1Msec;
	TUtranGpsReferenceTimeLayout iUtranGpsReferenceTime;
	TSfnTowUncertainty iSfnTowUncertainty;
	TUtranGpsDriftRate iUtranGpsDriftRate;
	TGpsTowAssistArrayLayout iGpsTowAssistArray;
	TRrlpGpsReferenceTimeLayout iRrlpGpsReferenceTime;
	};

#endif //LBSASSISTANCEREFERENCETIMELAYOUT_H

