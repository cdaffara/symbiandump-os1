// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// lbspositioningstatusprops.h
// A class for the defining of the Lbs Positioning Status Publish and Subscribe properties 
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef POSITIONING_STATUS_PROPS_H
#define POSITIONING_STATUS_PROPS_H


#include <e32base.h>
#include "lbspropertykeydefs.h"


/** The key for the NI Positioning Status property*/
const TUint  KLbsNiPositioningStatusKey = KLbsPositioningStatusAPIBase;
/** The key for the MO Positioning Status property*/
const TUint  KLbsMoPositioningStatusKey = KLbsNiPositioningStatusKey+1;


/**
The LbsPositioningStatusProps class provides the InitializeL function that allows the process-owner of 
the LBS Positioning Status API RProperties to define them.

@internalTechnology
@released

@see CLbsPositioningStatus
*/
NONSHARABLE_CLASS(LbsPositioningStatusProps)
	{
public:
	IMPORT_C static void InitializeNiPropertyL();
	IMPORT_C static void InitializeMoPropertyL();
	};

#endif //POSITIONING_STATUS_PROPS_H

