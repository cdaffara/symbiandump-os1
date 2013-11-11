// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file globlephonemanager.cpp
// This contains CGlobalPhonemanager which manage the global phone status and call info manager.
// 

// user include
#include "globalphonemanager.h"
#include "atmanager.h"
#include "commengine.h"
#include "tsyconfg.h"
#include "mslogger.h"
#include "requestbase.h"
#include "ltsycallinformationmanager.h"
#include "athangupcommandrecords.h"

#if defined (__WINS__)
_LIT(KPDDName,"ECDRV");
_LIT(KLDDName,"ECOMM");
#else
_LIT(KPDDName,"EUART1");
#if defined (PDD2_NAME)
_LIT(KPDD2Name,"EUART2");
#endif
_LIT(KLDDName,"ECOMM");
#endif

//define constant value
const TInt KLtsyDefaultSecondsToWaitForCarrier=40;
const TInt KLtsyCommReadPriority = 10;
const TInt KLtsyCommWritePriority = 20;
const TInt KLtsyChatBufferSize = 400;

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CGlobalPhonemanager* CGlobalPhonemanager::NewL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CGlobalPhonemanager::NewL()"));
	
	CGlobalPhonemanager *self = CGlobalPhonemanager::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CGlobalPhonemanager* CGlobalPhonemanager::NewLC()
	{
	LOGTEXT(_L8("[Ltsy] Starting CGlobalPhonemanager::NewLC()"));
	
	CGlobalPhonemanager *self = new (ELeave) CGlobalPhonemanager;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::~CGlobalPhonemanager
// other items were commented in a header
// ---------------------------------------------------------------------------
CGlobalPhonemanager::~CGlobalPhonemanager()
	{
	LOGTEXT(_L8("[Ltsy] Starting CGlobalPhonemanager::~CGlobalPhonemanager()"));

	delete iConfiguration;
	delete iAtManager;
	delete iCommEngine;
	delete iActiveIpcRequest;
	iContextList.ResetAndDestroy();
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::CGlobalPhonemanager
// other items were commented in a header
// ---------------------------------------------------------------------------
CGlobalPhonemanager::CGlobalPhonemanager()
	{
	LOGTEXT(_L8("[Ltsy] Starting CGlobalPhonemanager::CGlobalPhonemanager()"));
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGlobalPhonemanager::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CGlobalPhonemanager::ConstructL()"));
	
	InitL();
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::InitL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGlobalPhonemanager::InitL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CGlobalPhonemanager::InitL()"));
	
	// initialize the status of phone
	iPhoneStatus.iLocalNetworkSel.iMethod = RMobilePhone::ENetworkSelectionUnknown;
	iPhoneStatus.iModemDetected = RPhone::EDetectedUnknown;
	iPhoneStatus.iDataAndFaxFlags = RPhone::KCapsUnknown;
	iPhoneStatus.iWaitForCarrierTime = KLtsyDefaultSecondsToWaitForCarrier;
	iPhoneStatus.iRegistrationStatus = RMobilePhone::ERegistrationUnknown;
	iPhoneStatus.iPortAccess = EPortAccessAllowed;
	iPhoneStatus.iMode = RPhone::EModeUnknown;
	iPhoneStatus.iDataAndFaxFlags = RPhone::KCapsUnknown;
	iPhoneStatus.iInitStatus = EPhoneNotInitialised;
	iPhoneStatus.iGprsMaxNumContexts = 1;
	iPhoneStatus.iMode=RPhone::EModeOnlineCommand;
	
	//Create the call information manager
	iCallInfoManager = CLtsyCallInformationManager::NewL();
	
	// load physical device driver
	TInt r = User::LoadPhysicalDevice(KPDDName);
	if (r != KErrNone && r != KErrAlreadyExists)
		{
		User::Leave(r);
		}
	
	// load logical device driver
	r = User::LoadLogicalDevice(KLDDName);
	if (r != KErrNone && r != KErrAlreadyExists)
		{
		User::Leave(r);
		}
		
	// create the AT Manager
	iAtManager = CAtManager::NewL();
	
	// create the Comm Engine which handle 
	iCommEngine = CCommEngine::NewL(KLtsyChatBufferSize,KLtsyCommReadPriority,KLtsyCommWritePriority,iPhoneStatus.iPortAccess);
	
	// Set Comm Engine observer		
	iCommEngine->SetCommEngineObserver(iAtManager);
	iCommEngine->SetCommReadLineNotify(iAtManager);
	
	// create a CommDB configration helper class
	iConfiguration = CTsyConfig::NewL();
	
	TFileName csy;
	TName port;	
	LOGTEXT(_L8("[Ltsy] Getting CSY from CommDB"));
	User::LeaveIfError(iConfiguration->ConfigModemStringL(TPtrC(KCDTypeNameCsyName),csy));

	LOGTEXT(_L8("[Ltsy] Getting PORT from CommDB"));
	User::LeaveIfError(iConfiguration->ConfigModemStringL(TPtrC(KCDTypeNamePortName),port));
	
	// Open serial port by shared mode
	User::LeaveIfError(iCommEngine->CommOpen(csy, port, ECommShared));
	
	// Configure Port
	TCommConfig tConfigPckg;
	
	// Get the port configuration
	LOGTEXT(_L8("[Ltsy] Setting Port Config"));	
	User::LeaveIfError(iConfiguration->PortConfig(tConfigPckg,EConfigTypeInit));

	LOGTEXT(_L8("[Ltsy] CommEngine Configure Port"));
	User::LeaveIfError(iCommEngine->ConfigurePort(tConfigPckg));
	
	// issue reading from baseband
	iCommEngine->Read();
	
	// Reset the context list
	iContextList.Reset();
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::CheckGlobalPhoneStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CGlobalPhonemanager::CheckGlobalPhoneStatus()
	{
	if(EPhoneInitialised == iPhoneStatus.iInitStatus)
		{
		if (iEventSignalActive)
			{
			return  KErrInUse;
			}
		else
			{
			DeleteLastActiveRequest();
			return KErrNone;
			}
		}
	else
		{
		return iPhoneStatus.iInitStatus;
		}
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::SetActiveRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGlobalPhonemanager::SetActiveRequest(CRequestBase* aCurActiveRequest)
	{
	iActiveIpcRequest = aCurActiveRequest;
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::DeleteLastActiveRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGlobalPhonemanager::DeleteLastActiveRequest()
	{
	delete iActiveIpcRequest;
	iActiveIpcRequest = NULL;
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::GetCallInfoManager
// other items were commented in a header
// ---------------------------------------------------------------------------
CLtsyCallInformationManager& CGlobalPhonemanager::GetCallInfoManager()
	{
	return (*iCallInfoManager);
	}

// ---------------------------------------------------------------------------
// CGlobalPhonemanager::CurrentActiveRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
CRequestBase* CGlobalPhonemanager::CurrentActiveRequest()
    {
    return iActiveIpcRequest;
    }

// End of file
