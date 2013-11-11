// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <lbspositioninfo.h>
#include "ctlbsinstallpsytp273.h"
#include "installpsytp273.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsInstallPsyTp273::CT_LbsInstallPsyTp273() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsInstallPsyTp273::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
    }

// Two-phased constructor.
CT_LbsInstallPsyTp273* CT_LbsInstallPsyTp273::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsInstallPsyTp273* self = new (ELeave) CT_LbsInstallPsyTp273;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsInstallPsyTp273* CT_LbsInstallPsyTp273::NewL(TAny* aConstructionParameters)
    {
    CT_LbsInstallPsyTp273* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsInstallPsyTp273::~CT_LbsInstallPsyTp273()
    {
    }

// ---------------------------------------------------------
// CT_LbsInstallPsyTp273::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsInstallPsyTp273::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
    
    TTime tt;
    tt.UniversalTime();
    //Request ID must be unique, use universalime as seed
    // to give a random number
    TInt64 seed = tt.Int64();
    TReal lat = 90 * Math::FRand(seed);
    TReal lon = 90 * Math::FRand(seed);
    TReal32 alt = (TReal32)(90 * Math::FRand(seed));
    TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
    TUid implUid = { KPosImplementationUid };
    position->SetModuleId(implUid);

    TTime now;
    now.UniversalTime();          

    TPosition posse;
    posse.SetCoordinate(lat, lon, alt);
    posse.SetTime(now);
    position->SetPosition(posse);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
    }

// ---------------------------------------------------------
// CT_LbsInstallPsyTp273::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsInstallPsyTp273::CancelNotifyPositionUpdate()
	{
	}

// End of file


