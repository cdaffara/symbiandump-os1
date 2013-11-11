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
 @InternalComponent
*/

#include <e32base.h>
#include <lbs/epos_mpositionerstatus.h>
#include <lbssatellite.h>

#include "cgpspositioner.h"
#include "gpspsy.hrh"
#include "agpspsypanic.h"
#include "cpositionerq.h"
#include "crequesthandler.h"
#include "cagpsenvironment.h"
#include "utilfunctions.h"
#include "tagpsrequestparameters.h"
#include "agpsrequestconstants.h"
#include "psylogging.h"

/*
* 
*/
CGpsPositioner::CGpsPositioner()
    {
	iState = ERequestStateInitial;
	iTerminalAssisted = EFalse;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGpsPositioner::~CGpsPositioner()
    {
	TRACESTRING("CGpsPositioner::~CGpsPositioner")

	iEnvironment->UnregisterPSY(this);
	iNetRegStatus.Close();
    }

/**
 * Creates a new instance of a CPositioner implementation.
 *
 * @param aConstructionParameters Construction parameters for CPositioner.
 * @return The positioner implementation.
 */
CGpsPositioner* CGpsPositioner::NewL(TAny* aConstructionParameters)
    {
    CGpsPositioner* self = new( ELeave ) CGpsPositioner;

    CleanupStack::PushL( self );
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

/**
 * 2nd phase constructor
 *
 * @param aConstructionParameters Construction parameters for CPositioner.
 */
void CGpsPositioner::ConstructL(TAny* aConstructionParameters)
	{
	TRACESTRING("CGpsPositioner::ConstructL")

	// Must call BaseConstructL first thing during construction.
	BaseConstructL(aConstructionParameters);

	iEnvironment = CAgpsEnvironment::NewL();
	iEnvironment->RegisterPSYL(this);
	iEnvironment->GetEnvCriteriaL(iCriteria, TUid::Uid(KPosGPSPSYImplUid));
	iInactivityTimeout = iEnvironment->GetInactivityTimeoutL(TUid::Uid(KPosGPSPSYImplUid));
	iRequestHandler = iEnvironment->GetRequestHandler();

	// Get the request timeout from the LS:
	if(iEnvironment->ProfileInUse())	// default timeout was taken from profile, so let LS know
		{
		TPositionQuality quality;
		iCriteria.GetRequiredQuality(quality);
		iTimeOutInterval = quality.TimeToFirstFix();	
		SetPsyDefaultUpdateTimeOut(iTimeOutInterval);
		}	    

	iNetRegStatus.OpenL();
	}


void CGpsPositioner::SetModuleId(TPositionInfoBase* aPosInfo)
	{
	if(aPosInfo)
		{
		aPosInfo->SetModuleId(TUid::Uid(KPosGPSPSYImplUid)); // Use the PSY module Id rather than the AGPS managers
		}
	}

void CGpsPositioner::SetAGPSMode()
	{
    TRACESTRING("CPositionerSubSession::GpsMode()")
	
	iAGPSMode = KAutonomousMode;
	}

//  End of File
