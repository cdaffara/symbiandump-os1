// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "btgpsdevman.h"    
#include "btgpssimstubs.h"

CBTGPSDevMan::~CBTGPSDevMan()
	{
	Cancel();
	if (iResponse)
		{
		delete iResponse;
		}
	iRegistryView.Close();
	iRegServer.Close();
	
	};
    
	
CBTGPSDevMan::CBTGPSDevMan() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	};

/**
 * Get an array of devices that match the search pattern, from the registry.
 * results are returned via a callback function
 * 
 * @param aSearchPattern Search criteria for the devices to be returned
 * @param aDeviceArray Array owned by the client in which the result will be returned. Ownership is not transferred so this pointer must remain valid until the callback is made.
 */
TInt CBTGPSDevMan::GetDevices(TBTRegistrySearch& aSearchPattern, RBTDeviceArray* aDeviceArray)
	{
	iDeviceArray = aDeviceArray;
	Find(aSearchPattern);
	return KErrNone;
	};
    
/**
 * Two Stage constructor
 * @param aObserver Interface used to call back when results are ready to be returned
 */
CBTGPSDevMan* CBTGPSDevMan::NewL(MBTGPSDevManObserver* aObserver)
	{
	CBTGPSDevMan* self = new(ELeave) CBTGPSDevMan();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self);
	return self;
	};

	
void CBTGPSDevMan::ConstructL(MBTGPSDevManObserver* aObserver)
	{
	iObserver = aObserver;
	User::LeaveIfError(iRegServer.Connect());
	User::LeaveIfError(iRegistryView.Open(iRegServer));
	iResponse = CBTRegistryResponse::NewL(iRegistryView);
	iDeviceArray = &(iResponse->Results());
	};
	

void CBTGPSDevMan::DoCancel()
	{
	iRegistryView.CancelRequest(iStatus);
	if (iResponse)
		{
		iResponse->Cancel();
		}
	};
	

void CBTGPSDevMan::RunL()
	{
	if ( iStatus >= KErrNone)
		{
		switch (iRegistryState)
			{
			case EFinding:
				{
				// search has completed, get the resultset
				iResponse->Start(iStatus);
				iRegistryState = EGetting;
				SetActive ();
				}
				break;
			case EGetting:
				{
				Deque();
				iDeviceArray = &(iResponse->Results());
				
				#ifdef LBS_BTGPSPSY_SIM
				TRAP_IGNORE(AddSimulatedPairedDevicesL(iDeviceArray));
				#endif
				
				iObserver->HandleGetDevicesComplete(KErrNone, iDeviceArray);
				}
				break;
			}
		}
	else
		if (iStatus < KErrNone)
			{
			#ifdef LBS_BTGPSPSY_SIM
			// Still need to add simulated paired devices if PSY is being build in simulation mode
			TRAP_IGNORE(AddSimulatedPairedDevicesL(iDeviceArray));
			if (iDeviceArray->Count() > 0)
				{
				iStatus = KErrNone;
				}
			#endif
		
			Deque();
			iObserver->HandleGetDevicesComplete(iStatus.Int(), iDeviceArray);
			}
	};
	
TInt CBTGPSDevMan::RunError(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		iObserver->HandleGetDevicesComplete(KErrNoMemory, iDeviceArray);
		return KErrNone;
		}
	return aError;
	}
	
void CBTGPSDevMan::Find(TBTRegistrySearch aSearchPattern)
	{
	iRegistryView.CreateView(aSearchPattern, iStatus);
	iRegistryState = EFinding;
	SetActive();
	};
	
