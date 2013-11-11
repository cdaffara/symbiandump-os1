// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#include <ssm/starterdomaincrkeys.h>
#include <centralrepository.h>
#include <e32property.h>
#include <ssm/startupdomainpskeys.h>

#include "cmdpublishstartupmode.h"
#include "ssmuiproviderdll.h"
#include "ssmdebug.h"


/**
 * Constructs and returns a new custom command, leaving on errors.
 * 
 * @internalComponent
 */
CCustomCmdPublishStartupMode* CCustomCmdPublishStartupMode::NewL()
	{
	CCustomCmdPublishStartupMode* self = new (ELeave) CCustomCmdPublishStartupMode();
	return self;
	}

CCustomCmdPublishStartupMode::CCustomCmdPublishStartupMode()
:	CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);
	}

CCustomCmdPublishStartupMode::~CCustomCmdPublishStartupMode()
	{
	Deque();
	//Cancel the request if there is any
	iSsmMiscAdaptation.Cancel();
	
	//Close RSsmMiscAdaptation
	iSsmMiscAdaptation.Close();
	
	delete iRepository;
	}

/**
 * Initializes this custom command. This consists of connecting to the required servers.
 * 
 * @internalComponent
 */
TInt CCustomCmdPublishStartupMode::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	iHiddenResetPckg = EFalse;
	iState = EInitial;
	// Connect to the central repository
	TRAPD(err, iRepository = CRepository::NewL(KCRUidStartup));
	if(KErrNone != err && KErrNotFound != err)
			{
			return err;
			}
	
	//Connect to RSsmMiscAdaptation
	err = iSsmMiscAdaptation.Connect();
	return err;
	}

/**
 * Destory this object and any resources allocated to it.
 * 
 * @internalComponent
 */
void CCustomCmdPublishStartupMode::Close()
	{
	}

void CCustomCmdPublishStartupMode::Release()
	{
	delete this;
	}

/**
 * Issues the required requests to publish the startup mode. Completes aStatus upon completion.
 * 
 * @internalComponent
 */
void CCustomCmdPublishStartupMode::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	// Remember our completion value
	iReturnStatus = &aStatus;
	aStatus = KRequestPending;
    iState = ERequestingStartupMode;

	// Issue request
	iSsmMiscAdaptation.GetGlobalStartupMode(iStartupModePckg, iStatus);
	SetActive();
	}

void CCustomCmdPublishStartupMode::PublishStartupModeL()
    {
    if(iStatus != KErrNone)
         {
         DEBUGPRINT2A("Request GetGlobalStartupMode returned error: %d", iStatus.Int());
         User::Leave(iStatus.Int());
         }

     // Publish startup mode
     TInt err = RProperty::Set(CSsmUiSpecific::StartupPSUid(), KPSGlobalStartupMode, iStartupModePckg());
     if(KErrNone != err)
         {
         DEBUGPRINT2A("Publishing startup mode returned error: %d", err);
         User::Leave(err);
         }

     iState = ERequestingHiddenReset;
     iSsmMiscAdaptation.GetHiddenReset(iHiddenResetPckg, iStatus);
     SetActive();
    }

void CCustomCmdPublishStartupMode::PublishHiddenResetL()
    {
    DEBUGPRINT2A("Request GetHiddenReset completed with errorcode: %d", iStatus.Int());
    // Only if we've been able to open the repository it is published from.
    if(iRepository != NULL)
        {
        // Retrieve startup reason
        TInt startupReason = 0;
        TInt err = iRepository->Get(KStartupReason, startupReason);

        if (KErrNone == err)
            {
            if ( startupReason == ENormalStartup &&
                 iStatus.Int() == KErrNone && // Hidden reset query was successful
                 iHiddenResetPckg())
                {
                DEBUGPRINT1A("Hidden reset detected");
                startupReason = EUnknownReset;
                }
			else
				{
				err = iRepository->Set(KStartupDevLockStatus, EStartupDevLockNotSucess);
				DEBUGPRINT1A("Normal Device Startup");
				DEBUGPRINT2A("Setting KStartupDevLockStatus to EStartupDevLockNotSucess completed with error %d", err);
				}
            
            // Publish startup reason
            err = RProperty::Set(CSsmUiSpecific::StartupPSUid(), KPSStartupReason, startupReason);
            if(KErrNone != err)
                {
                DEBUGPRINT2A("Publishing startup reason returned error: %d", err);
                User::Leave(err);
                }
            DEBUGPRINT2A( "Startup reason set to %d", startupReason);
            }
        }

    // Command complete, signal back
    User::RequestComplete(iReturnStatus, KErrNone);
    // Reset iReturnStatus so it is not completed again in future
    iReturnStatus = NULL;       
    }

/**
 * From CActive::RunL()
 * 
 * @internalComponent 
 */
void CCustomCmdPublishStartupMode::RunL()
	{
	switch(iState)
	    {
	    case ERequestingStartupMode:
	        {
	        PublishStartupModeL();
	        break;
	        }
	    case ERequestingHiddenReset:
	        {
	        PublishHiddenResetL();
	        break;
	        }
	    default:
	        {
	        User::Leave(KErrArgument);
	        }
	    }
	}

/**
 * From CActive::DoCancel()
 * 
 * @internalComponent 
 */
void CCustomCmdPublishStartupMode::DoCancel()
	{
	//Cancel the request if there is any
	iSsmMiscAdaptation.Cancel();
	}

/**
 * From CActive::RunError()
 * 
 * @internalComponent 
 */
TInt CCustomCmdPublishStartupMode::RunError(TInt aError)
	{
	DEBUGPRINT2A("CCustomCmdPublishStartupMode::RunError received error: %d", aError);
	User::RequestComplete(iReturnStatus, aError);
	// We've returned the error via the status so return KErrNone here.
	return KErrNone;
	}

void CCustomCmdPublishStartupMode::ExecuteCancel()
	{
	Cancel();
	if(iReturnStatus != NULL)
		{
		User::RequestComplete(iReturnStatus, KErrCancel);
		iReturnStatus = NULL;
		}
	}
