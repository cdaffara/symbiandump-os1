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

#ifndef LBSASSISTANCEUTRANREFERENCETIMELAYOUT_H
#define LBSASSISTANCEUTRANREFERENCETIMELAYOUT_H

#include <e32def.h>
#include <lbs/lbsassistanceenum.h>

struct TPrimaryCpichInfoLayout
	{
	TUint iPrimaryScramblingCode;
	};

struct TUtranGpsReferenceTimeLayout
	{
	TUint iUtranGPSTimingOfCell_MsPart;
	TUint iUtranGPSTimingOfCell_LsPart;
	TPrimaryCpichInfoLayout iModeSpecificInfo_Fdd_ReferenceIdentity;
	TUint iModeSpecificInfo_Tdd_ReferenceIdentity;
	TUint iSfn;
	};


#endif //LBSASSISTANCEUTRANREFERENCETIMELAYOUT_H

