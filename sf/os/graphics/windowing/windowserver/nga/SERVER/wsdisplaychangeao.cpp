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
*/

#include "wsdisplaychangeao.h"
#include "ScrDev.H"
#include "server.h"

CWsDisplayChangeNotifier::CWsDisplayChangeNotifier(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner)
:CActive(EEventPriority), iNextLevelInterface(aNextLevelInterface), iOwner(aOwner)
	{
	CActiveScheduler::Add(this);
	}

CWsDisplayChangeNotifier* CWsDisplayChangeNotifier::NewL(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner)
	{
	CWsDisplayChangeNotifier* self = new(ELeave) CWsDisplayChangeNotifier(aNextLevelInterface, aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CWsDisplayChangeNotifier::ConstructL()
	{
	iRetry = CEventQueueRetry::NewL();
	}

void CWsDisplayChangeNotifier::IssueNotificationRequest()
	{
	//before submitting request, save the last resolution list
	iLastErr = iNextLevelInterface->GetResolutions(iLastResList);
	if(iLastErr != KErrNone)
		{
		//probabally KErrDisconnect
		iLastResList.Reset();
		}
	iNextLevelInterface->NotifyOnDisplayChange(iStatus);
	SetActive();
	}

void CWsDisplayChangeNotifier::CancelNotificationRequest()
	{
	Cancel();
	}

void CWsDisplayChangeNotifier::RunL()
	{
	if(iStatus == KErrNone)
		{
		//IssueNotificationRequest() will overwrite iLastResList and iLastErr, save a copy first
		TInt lastErr = iLastErr;
		RArray<MWsDisplayControl::TResolution> lastResList;
		CleanupClosePushL(lastResList);
		lastResList.ReserveL(iLastResList.Count());
		for(TInt i = 0; i < iLastResList.Count(); i++)
			{
			lastResList.Append(iLastResList[i]);
			}
		//submit request again, this should be called as early as possible in RunL(). Otherwise display change occurs
		//before submitting the request will be missed.
		IssueNotificationRequest();
		
		RArray<MWsDisplayControl::TResolution> currentResList;
		CleanupClosePushL(currentResList);
		TInt err = iNextLevelInterface->GetResolutions(currentResList);
		if(err != KErrNone)
			{
			//probabally KErrDisconnect
			currentResList.Reset();
			}
		//return code not equal also counts as an event
		if(!IsResListEqual(currentResList, lastResList) || lastErr != err)
			{
			//first thing is to cancel the retry AO if it's running for the last event
			iRetry->CancelRetry();
			
			iOwner->IncreaseDisplaySpinner();
			//put display change event on queue
			RPointerArray<CWsClient> clientArray;
			CleanupClosePushL(clientArray);
			User::LeaveIfError(iOwner->GetNotificationClients(clientArray));
			TBool eventOnAllQueues = ETrue;
			for(TInt i = 0; i < clientArray.Count(); i++)
				{
				if(!TWindowServerEvent::SendDisplayChangedEvents(clientArray[i], iOwner->ScreenNumber(),
						iOwner->ConfigSpinner(), iOwner->DisplaySpinner()))
					{
					eventOnAllQueues = EFalse;
					clientArray[i]->SetRetryFlag(EEventDisplayChanged);
					}
				}
			CleanupStack::PopAndDestroy(&clientArray);
			
			//some event queues are full, kick off retry AO
			if(!eventOnAllQueues)
				{
				iRetry->Init(iOwner);
				iRetry->Retry(KRetryInitialDelay);
				}
						
			//codes below are dealing with detach/attach
			MWsDisplayPolicy* policy = iOwner->DisplayPolicy();
			if((err == KErrDisconnected && lastErr != KErrDisconnected)
					||(currentResList.Count() == 0 && lastResList.Count() != 0))
				{
				//The display is disconnected
				if(policy)
					{
					TInt appMode = policy->SuitableAppMode(MWsDisplayPolicy::EDetach);
					//Last app mode can be resumed when display is connected again from disconnection
					iOwner->DisplayPolicy()->SetLastAppMode(iOwner->ScreenSizeMode());
					//This gonna set the screen mode to smallest app mode
					if(appMode >= 0)
						{
						iOwner->doSetScreenMode(appMode);
						}
					}
				else
					{
					//if policy is not available, do a SetConfiguration set to the same config before disconnect
					//this will update parameters for MDisplayMapping stored in CDisplayPolicy
					TDisplayConfiguration config;
					iNextLevelInterface->GetConfiguration(config);
					iNextLevelInterface->SetConfiguration(config);
					}
				//stop DSA drawing as the above set screen mode is under disconnection and it won't
				//go the normal routine to stop DSA drawing
				iOwner->AbortAllDirectDrawing(RDirectScreenAccess::ETerminateScreenMode);
				}
			
			if(currentResList.Count() > 0 && lastResList.Count() == 0)
				{
				//The display is connected
				if(policy)
					{
					TInt appMode = policy->SuitableAppMode(MWsDisplayPolicy::EAttach);
					//This gonna resume the screen mode to the one before disconnection
					if(appMode >= 0)
						{
						iOwner->doSetScreenMode(appMode);
						}
					}
				else
					{
					//if policy is not available, force a SetConfiguration to trigger a config change notification
					//as the twips size in config is changed on attaching display though resolution may remain 0x0 
					TDisplayConfiguration config;
					config.SetResolution(currentResList[0].iPixelSize);
					iOwner->SetConfiguration(config);
					}
				iOwner->RecalculateModeTwips();
				}
			}
		CleanupStack::PopAndDestroy(2, &lastResList);
		}
	else if(iStatus != KErrCancel && iStatus != KErrNotSupported)
		{
		IssueNotificationRequest();	//This should be ok, not deadlock
		}
		
	}

TBool CWsDisplayChangeNotifier::IsResListEqual(RArray<MWsDisplayControl::TResolution>& aResListA, RArray<MWsDisplayControl::TResolution>& aResListB)
	{
	if (aResListA.Count() != aResListB.Count())
		return EFalse;
	
	for(TInt i = 0; i < aResListA.Count(); i++)
		{
		if(aResListA[i].iPixelSize != aResListB[i].iPixelSize)
			{
			return EFalse;
			}
		if(aResListA[i].iTwipsSize != aResListB[i].iTwipsSize)
			{
			return EFalse;
			}
		if(!(aResListA[i].iFlags == aResListB[i].iFlags))
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}

void CWsDisplayChangeNotifier::DoCancel()
	{}

CWsDisplayChangeNotifier::~CWsDisplayChangeNotifier()
	{
	Cancel();
	iLastResList.Close();
	delete iRetry;
	}

CWsConfigChangeNotifier::CWsConfigChangeNotifier(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner)
:CActive(EEventPriority), iNextLevelInterface(aNextLevelInterface), iOwner(aOwner)
	{
	CActiveScheduler::Add(this);
	}

CWsConfigChangeNotifier* CWsConfigChangeNotifier::NewL(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner)
	{
	CWsConfigChangeNotifier* self = new(ELeave) CWsConfigChangeNotifier(aNextLevelInterface, aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CWsConfigChangeNotifier::ConstructL()
	{
	iRetry = CEventQueueRetry::NewL();
	}

void CWsConfigChangeNotifier::IssueNotificationRequest()
	{
	//Before submitting the request, save last configuration
	iNextLevelInterface->GetConfiguration(iLastConfig);
	iNextLevelInterface->NotifyOnConfigChange(iStatus);
	SetActive();
	}

void CWsConfigChangeNotifier::CancelNotificationRequest()
	{
	Cancel();
	}

void CWsConfigChangeNotifier::UpdateLastSetConfiguration(TDisplayConfiguration& aNewConfig)
    {
    TSize resolution(0,0);
    if (aNewConfig.GetResolution(resolution))
        {
        iLastSetConfig.SetResolution(resolution);
        }
    
    TSize resolutionTwips(0,0);
    if (aNewConfig.GetResolutionTwips(resolutionTwips))
        {
        iLastSetConfig.SetResolutionTwips(resolutionTwips);   
        }
    
    TDisplayConfiguration1::TRotation rotation;
    if (aNewConfig.GetRotation(rotation))
        {
        iLastSetConfig.SetRotation(rotation);
        }
    }

void CWsConfigChangeNotifier::RunL()
	{
	if(iStatus == KErrNone)
		{
		//IssueNotificationRequest() will overwrite iLastConfig, save a copy first
		TDisplayConfiguration lastConfig(iLastConfig);
		IssueNotificationRequest();
		
		TDisplayConfiguration currentConfig;
		iNextLevelInterface->GetConfiguration(currentConfig);
		if(!(currentConfig == lastConfig))
			{
			//first thing is to cancel the retry AO if it's running for the last event
			iRetry->CancelRetry();
			
			iOwner->IncreaseConfigSpinner();
			
			//if the config change comes from a render stage then ensure screen device size 
			//is also updated
			TSize currentRes;
			currentConfig.GetResolution(currentRes);
			TBool disconnected = (currentRes.iHeight == 0 || currentRes.iWidth == 0) ? ETrue : EFalse;
            
			//if the config change is due to CScreen::SetConfiguration() being called then we
			//don't want to update it again. Only update if the configs are different and the
			//display is connected...
			TDisplayConfiguration lastSetConfig(iLastSetConfig);
			if (!((currentConfig == lastSetConfig) || (disconnected)))
			    {
                TDisplayConfiguration1::TRotation rotation;
                if (lastSetConfig.GetRotation(rotation))
                    {
                    //use the latest rotation value to ensure we don't get any
                    //inconsistencies with the layer extents
                    currentConfig.SetRotation(rotation);
                    }
                iOwner->UpdateConfiguration(currentConfig);
			    }		
			
			//put config change event on queue
			RPointerArray<CWsClient> clientArray;
			CleanupClosePushL(clientArray);
			User::LeaveIfError(iOwner->GetNotificationClients(clientArray));
			TBool eventOnAllQueues = ETrue;
			for(TInt i = 0; i < clientArray.Count(); i++)
				{
				if(!TWindowServerEvent::SendDisplayChangedEvents(clientArray[i], iOwner->ScreenNumber(),
						iOwner->ConfigSpinner(), iOwner->DisplaySpinner()))
					{
					eventOnAllQueues = EFalse;
					clientArray[i]->SetRetryFlag(EEventDisplayChanged);					
					}
				}
			
			CleanupStack::PopAndDestroy(&clientArray);
			//some event queues are full, kick off retry AO
			if(!eventOnAllQueues)
				{
				iRetry->Init(iOwner);
				iRetry->Retry(KRetryInitialDelay);
				}
			}
		iNextLevelInterface->GetConfiguration(iLastSetConfig);
		}
	else if(iStatus != KErrCancel && iStatus != KErrNotSupported)
		{
		IssueNotificationRequest();
		}
	
	}


void CWsConfigChangeNotifier::DoCancel()
	{}

CWsConfigChangeNotifier::~CWsConfigChangeNotifier()
	{
	Cancel();
	delete iRetry;
	}


