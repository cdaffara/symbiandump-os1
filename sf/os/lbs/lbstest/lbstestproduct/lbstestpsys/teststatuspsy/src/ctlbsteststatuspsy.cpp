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
#include "ctlbsteststatuspsy.h"
#include "teststatuspsy.hrh"
#include "epos_mpositionerstatus.h"

// CONSTANTS

#ifdef __WINS__
const TInt KTimeout = 100000; // Microseconds              
#else
const TInt KTimeout = 1000000; // Microseconds
#endif


const TInt KInvalidStatusArgument = 32;



// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestStatusPsy::CT_LbsTestStatusPsy() : iPositioner(NULL), iPeriodicTimer(NULL), iReportStatusNr(0)
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestStatusPsy::ConstructL(TAny* aConstructionParameters)
	{
	BaseConstructL(aConstructionParameters);
	iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	TCallBack callback(SetDeviceStatus, this);
	iPositioner = PositionerStatus();
	iPeriodicTimer->Start(KTimeout, KTimeout, callback);

	//Create dummy file that is deleted in destructor to indicate if psy is unloaded correctly!
	User::LeaveIfError(ifileserver.Connect());
	TInt err = idummyfile.Open(ifileserver,KDummyFileName, EFileWrite);
	ifileMan = CFileMan::NewL(ifileserver);
        if (err == KErrNotFound) 
            {
            User::LeaveIfError(idummyfile.Create(ifileserver, KDummyFileName, EFileWrite));
            }
    }

// Two-phased constructor.
CT_LbsTestStatusPsy* CT_LbsTestStatusPsy::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestStatusPsy* self = new (ELeave) CT_LbsTestStatusPsy;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestStatusPsy* CT_LbsTestStatusPsy::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestStatusPsy* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestStatusPsy::~CT_LbsTestStatusPsy()
    {
	idummyfile.Close();
	ifileMan->Delete(KDummyFileName, CFileMan::EOverWrite);
	delete ifileMan;
	ifileMan = NULL;
	idummyfile.Close();
    ifileserver.Close();
    iPeriodicTimer->Cancel();
	ReportStatus(TPositionModuleStatus::EDeviceInactive, TPositionModuleStatus::EDataQualityUnknown);
	delete iPeriodicTimer;
	}

// ---------------------------------------------------------
// CT_LbsTestStatusPsy::SetDeviceStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsTestStatusPsy::SetDeviceStatus(TAny* aThis)
	{
	CT_LbsTestStatusPsy* self = static_cast<CT_LbsTestStatusPsy*> (aThis);
    self->ChangeDeviceStatus();
	return KErrNone;
    }

// ---------------------------------------------------------
// CT_LbsTestStatusPsy::ChangeDeviceStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestStatusPsy::ChangeDeviceStatus()
	{	
	switch(iReportStatusNr)
		{
		case EReportReadyFirst:
			{
			ReportStatus(TPositionModuleStatus::EDeviceReady, TPositionModuleStatus::EDataQualityNormal);	
			break;
			}	
        case EReportDisabled:
            {
            ReportStatus(TPositionModuleStatus::EDeviceDisabled, TPositionModuleStatus::EDataQualityUnknown);
            break;
            }
		case EReportErroneous:
			{
			ReportStatus(KInvalidStatusArgument, KInvalidStatusArgument);		
			break;
			}
		case EReportInitialising:
			{
			ReportStatus(TPositionModuleStatus::EDeviceInitialising, TPositionModuleStatus::EDataQualityPartial);		
			break;
			}	
		case EReportStandby:
			{
			ReportStatus(TPositionModuleStatus::EDeviceStandBy, TPositionModuleStatus::EDataQualityLoss);
			break;
			}
		case EReportActive:
			{
			ReportStatus(TPositionModuleStatus::EDeviceActive, TPositionModuleStatus::EDataQualityUnknown);
			break;
			}
		case EReportError:
			{
			ReportStatus(TPositionModuleStatus::EDeviceError, TPositionModuleStatus::EDataQualityUnknown);
			break;
			}
		case EReportUnknown:
			{
			ReportStatus(TPositionModuleStatus::EDeviceUnknown, TPositionModuleStatus::EDataQualityNormal);
			break;
			}
		case EReportInactive:
			{
			ReportStatus(TPositionModuleStatus::EDeviceInactive, TPositionModuleStatus::EDataQualityUnknown);
			break;
			}
		case EReportReadyLast:
			{
			ReportStatus(TPositionModuleStatus::EDeviceReady, TPositionModuleStatus::EDataQualityNormal);
			break;
			}
		default:
			{
			iPeriodicTimer->Cancel();
			break;
			}
		}
		iReportStatusNr++;
	}


// ---------------------------------------------------------
// CT_LbsTestStatusPsy::ReportStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestStatusPsy::ReportStatus(
	const TInt aDeviceStatus, 
	const TInt aDataQuality)
	{
	TPositionModuleStatus moduleStatus;
	moduleStatus.SetDeviceStatus(aDeviceStatus);
	moduleStatus.SetDataQualityStatus(aDataQuality);
	iPositioner->ReportStatus(moduleStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestStatusPsy::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestStatusPsy::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
	{
    TPositionInfo* position = NULL;
    TRequestStatus* status = NULL;
    position = static_cast<TPositionInfo*> (&aPosInfo);
    status = &aStatus;

  	TUid implUid = { KPosImplementationUid };
	position->SetModuleId(implUid);

	// Set a dummy position
	TCoordinate coor(20.0, 20.0, 20.0);
	TLocality loc (coor, 1.0, 1.0);
	TPosition pos (loc, TTime(0));
	position -> SetPosition(pos);

	User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------
// CT_LbsTestStatusPsy::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestStatusPsy::CancelNotifyPositionUpdate()
	{
	}

// End of file
