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
#include <e32base.h> 
#include <lbs/epos_cpositioner.h>
#include "ctlbstestpsy11.h"
#include "testpsy11.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy11::CT_LbsTestPsy11() 
    {
		iEveryOther = ETrue;
		iLongitud = 10;
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy11::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
    }

// Two-phased constructor.
CT_LbsTestPsy11* CT_LbsTestPsy11::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy11* self = new (ELeave) CT_LbsTestPsy11;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
	return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy11* CT_LbsTestPsy11::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy11* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy11::~CT_LbsTestPsy11()
    {
    }

// ---------------------------------------------------------
// CT_LbsTestPsy11::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy11::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
{
    TInt err = 0;
    
	TPositionInfo* iPosition = static_cast<TPositionInfo*> (&aPosInfo);
	TUid implUid = { KPosImplementationUid };
	iPosition->SetModuleId(implUid);
	
	// Every other return nothing (KErrEPosNoPositionFix)
	if (iEveryOther)
		{
		iEveryOther = EFalse;

	    TPosition position;
		position.SetCoordinate(iLongitud, iLongitud, (TReal32)iLongitud);
		
		TTime now;
        now.UniversalTime();
        position.SetTime(now);
		//aPosInfo->SetPhoneTime(now);
		iPosition->SetPosition(position);
		iLongitud+=10;
        
		if (iLongitud >= 90)
			{
            iLongitud = 5;
			}
		}
	else
		{
		iEveryOther = ETrue;
		err = KPositionPartialUpdate;
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
}

// ---------------------------------------------------------
// CT_LbsTestPsy11::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy11::CancelNotifyPositionUpdate()
	{
	}

//  End of File  
