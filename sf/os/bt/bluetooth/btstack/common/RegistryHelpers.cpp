// RegistryHelpers.CPP

// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Small classes to assist stack in using Registry
// 
//

#include "RegistryHelpers.h"
#include "hostresolver.h"
#include "linkmgr.h"
#include "linkutil.h"
#include "debug.h"
#include <comms-infras/commsdebugutility.h>

__FLOG_STMT(_LIT8(KSubsys, "btstack");)
__FLOG_STMT(_LIT8(KComponent, "btregistryhelpers");)

//
// Classes apertaining to MBTRegistryTaskNotifier //
//

CBTRegistryHelperBase::CBTRegistryHelperBase(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess)
	: CActive(EPriorityHigh)
	, iRegSess(aRegSess)
	, iParent(&aParent)
	{
	// high for servicing the protocol quickly
	CActiveScheduler::Add(this);
	}

CBTRegistryHelperBase::~CBTRegistryHelperBase()
	{
	__FLOG_STATIC(KSubsys, KComponent, _L8("sec\tRegistryHelper destructing"));
	Cancel();
	iRegistry.Close();
	iRegSess.Close();
	}

void CBTRegistryHelperBase::ConstructL()
	{
	iRegSess.Open(); // Open the resource (as we will close on delete).
	User::LeaveIfError(iRegistry.Open(iRegSess.RegServ()));
	}

void CBTRegistryHelperBase::DoCancel()
	{
	iRegistry.CancelRequest(iStatus);
	}

void CBTRegistryHelperBase::RunL()
	{
	// Store the value from iStatus in case another active request is started. 
	TInt rValue = iStatus.Int();
	
	// Check that the registry access is complete.
	if(IsRegistryTaskCompleteL())
		{
		// Is the parent of this object still available to be notified
		// of completion.
		if(iParent)
			{
			Notify(rValue);
			}
		else
			{
			delete this;
			}
		}
	}

TInt CBTRegistryHelperBase::RunError(TInt aError)
	{
	if(iParent)
		{
		Notify(aError);
		}
	else
		{
		delete this;
		}
	return KErrNone;
	}

TBool CBTRegistryHelperBase::IsRegistryTaskCompleteL()
	{
	// Default implementation.
	return ETrue;
	}
	
void CBTRegistryHelperBase::Notify(TInt aError)
	{
	// Default implementation.
	__FLOG_STATIC2(KSubsys, KComponent, _L8("CBTRegistryHelperBase::Notify(%d), this 0x%08x"), aError, this);
	iParent->RegistryTaskComplete(this, aError);
	}

void CBTRegistryHelperBase::DetachParent()
	{
	__FLOG_STATIC3(KSubsys, KComponent, _L8("CBTRegistryHelperBase::DetachParent(), this 0x%08x, Current Parent 0x%08x, IsActive() %d")
			       ,this, iParent, IsActive());
	
	// If the helper is not currently active then this object must be deleted.
	if(!IsActive())
		{
		delete this;
		}
	else
		{
		iParent = NULL;
		}
	}
	
CBTDeviceGetter::CBTDeviceGetter(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess)
: CBTRegistryHelperBase(aParent, aRegSess)
	{
	}

CBTDeviceGetter* CBTDeviceGetter::NewL(CRegistrySession& aRegSess,
									  MBTRegistryTaskNotifier& aParent)
	{
	CBTDeviceGetter* self = new(ELeave) CBTDeviceGetter(aParent, aRegSess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CBTDeviceGetter::Start(const TBTDevAddr& aAddr)
	{
	__FLOG_STATIC1(KSubsys, KComponent, _L8("Registry helper (Getter 0x%08x) started..."), this);
	iDevice.SetAddress(aAddr);
	iRegistry.GetDevice(iDevice, iStatus);
	SetActive();
	}

void CBTDeviceGetter::Notify(TInt aError)
	{
	iParent->RegistryTaskComplete(this, iDevice, aError);
	}

CBTDeviceModifier* CBTDeviceModifier::NewL(CRegistrySession& aRegSess, MBTRegistryTaskNotifier& aParent, CBTInquiryMgr& aInquiryMgr)
	{
	CBTDeviceModifier* self = new(ELeave) CBTDeviceModifier(aParent, aInquiryMgr, aRegSess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTDeviceModifier::CBTDeviceModifier(MBTRegistryTaskNotifier& aParent, CBTInquiryMgr& aInquiryMgr, CRegistrySession& aRegSess)
: CBTRegistryHelperBase(aParent, aRegSess), iInquiryMgr(aInquiryMgr), iRegistryUpdateStatus(ENoChange)
	{
	}

void CBTDeviceModifier::Start(const TBTNamelessDevice& aDevice, TBool aCanAddDevice)
	{
	__FLOG_STATIC1(KSubsys, KComponent, _L8("Registry helper (Modifier 0x%08x) started..."), this);

	// Store the arguments and check if the device specified by the
	// aDevice argument is currently present in the registry.
	iCanAddDevice = aCanAddDevice;
	iDevice = aDevice;

	iRegistrySearch.Reset();
	iRegistrySearch.FindAddress(iDevice.Address());
	
	// Check the registry for this address.  If a record exists the iStatus
	// in the RunL will be 1.
	iRegistry.CreateView(iRegistrySearch, iStatus);
	iState = EFindingDevice;
	SetActive();
	}

CBTDeviceModifier::~CBTDeviceModifier()
	{
	Cancel();
	delete iDeviceWithName;
	}

TBool CBTDeviceModifier::IsRegistryTaskCompleteL()
	{
	switch(iState)
		{
		case EFindingDevice:
			{
			// A view has been created based on the address of the new device.
			// If the iStatus value is 1 then the device has been found.
			// Note.  The value can never be greater than 1.  The server will
			// PANIC if this condition were to occur therefore the code here does
			// not need to check this condition.
			if(iStatus.Int() > 0)
				{
				// The device is in the registry, so it can be modified
				iRegistry.ModifyDevice(iDevice, iStatus);
				iState = EModifyingDevice;
				iCanAddDevice = EFalse;		// reinforce this, as its needed in the subsequent state
				SetActive();
				}
			else if (iStatus.Int()==KErrNotFound)
				{
				// The device could not be found.  Check if its valid to
				// add the device.
				if(iCanAddDevice)
					{
					// The device should be added into the registry
					delete iDeviceWithName;
					iDeviceWithName = NULL;
					iDeviceWithName = CBTDevice::NewL(iDevice); 
					iRegistry.AddDeviceL(*iDeviceWithName, iStatus); 
					iState = EAddingDevice;
					SetActive();
					}
				// ELSE Nothing more to be done - the modification will be lost.
				}
			}
			break;				

		case EAddingDevice:
		case EModifyingDevice:
			{
  			if (iStatus.Int() == KErrNone)
	  			{
				if (iCanAddDevice)
					{
					// This informs the Phy that an a reg action has occurred
					iRegistryUpdateStatus = EAddition;
					}
				else
					{
					iRegistryUpdateStatus = EModification;
					}
	  			}
  			else
	  			{
	  			__FLOG_STATIC1(KSubsys, KComponent, _L8("CBTDeviceModifier::IsRegistryTaskCompleteL() - registry update problem (TRequestStatus %d)"), iStatus.Int());
	  			}
	
 			// now try to put the device name in if we have one	
 			const TDesC8* name = iInquiryMgr.DeviceNameFromCache(iDevice.Address());
 			if(name && (name->Length()>0))
				{
 				iRegistry.ModifyBluetoothDeviceNameL(iDevice.Address(), *name, iStatus); 
 				iState = EModifyingName;
 				SetActive();
				}
			}
			break;

		case EModifyingName:
		default:
			break;
		};
		
	// If more modifications are required to the registry this AO
	// will be active.  i.e., if this AO is active then the registry
	// task is NOT complete.
	return (!IsActive());
	}

void CBTDeviceModifier::Notify(TInt aError)
	{
	// Inform the Phy whether the registry has been modified
	iParent->RegistryTaskComplete(this, iRegistryUpdateStatus, aError);
	}

CBTDeviceNameChanger::CBTDeviceNameChanger(MBTRegistryTaskNotifier& aParent,CRegistrySession& aRegSess)
: CBTRegistryHelperBase(aParent, aRegSess)
	{
	}

CBTDeviceNameChanger* CBTDeviceNameChanger::NewL(CRegistrySession& aRegSess,
									  MBTRegistryTaskNotifier& aParent)
	{
	CBTDeviceNameChanger* self = new(ELeave) CBTDeviceNameChanger(aParent, aRegSess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CBTDeviceNameChanger::Start(const TBTDevAddr& aAddr, const TBTDeviceName8& aName)
	{
	__FLOG_STATIC1(KSubsys, KComponent, _L8("Registry helper (NameChanger 0x%08x) started..."), this);
	iAddr = aAddr;
	iName = aName;
	TRAPD(err, iRegistry.ModifyBluetoothDeviceNameL(iAddr, iName, iStatus));
	SetActive();
	if(err != KErrNone)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, err);
		}
	}

CBTLinkKeyDeleter::CBTLinkKeyDeleter(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess)
: CBTRegistryHelperBase(aParent, aRegSess)
	{
	}

void CBTLinkKeyDeleter::Start(const TBTDevAddr& aDeviceAddr)
	{
	__FLOG_STATIC1(KSubsys, KComponent, _L8("Registry helper (Linkkey deleter 0x%08x) started..."), this);
	iRegistry.UnpairDevice(aDeviceAddr, iStatus);
	SetActive();
	}

CBTLinkKeyDeleter* CBTLinkKeyDeleter::NewL(CRegistrySession& aRegSess,
										  MBTRegistryTaskNotifier& aParent)
	{
	CBTLinkKeyDeleter* self = new(ELeave) CBTLinkKeyDeleter(aParent, aRegSess);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}




//
// Classes apertaining to MBtPairedDeviceNotifier //
//


// ----------------------------------------------------------
//	Active Paired Device Getter (from Registry)
// ----------------------------------------------------------

CBTPairedBDAddrGetter::CBTPairedBDAddrGetter(MBTPairedDeviceNotifier& aNotifier, RBTRegServ& aRegServ)
	: CActive(EPriorityStandard)
	, iNotifier(aNotifier)
	, iRegistryServer(aRegServ)
	{
	CActiveScheduler::Add(this);
	}

CBTPairedBDAddrGetter::~CBTPairedBDAddrGetter()
	{
	Cancel();
	delete iResponseHandler;
	iView.Close();
	}
	
CBTPairedBDAddrGetter* CBTPairedBDAddrGetter::NewL(MBTPairedDeviceNotifier& aNotifier, RBTRegServ& aRegServ)
	{
	CBTPairedBDAddrGetter* self = new (ELeave) CBTPairedBDAddrGetter(aNotifier, aRegServ);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CBTPairedBDAddrGetter::ConstructL()
	{
	User::LeaveIfError(iView.Open(iRegistryServer));
	}
	
void CBTPairedBDAddrGetter::Retrieve()
	{
	if(!IsActive())
		{
		// keep copy as we go async
		iRegistrySearch.Reset();
		iRegistrySearch.FindBonded();
		iView.CreateView(iRegistrySearch, iStatus);
		iState = ECreatingView;
		SetActive();
		}
	}

void CBTPairedBDAddrGetter::RunL()
	{
	// check for error
	TInt result = iStatus.Int();
	if (result < 0)
		{
		User::Leave(result);
		}
		
	switch (iState)
		{
		case ECreatingView:
			{
			if (result == 0)
				{
				User::Leave(KErrNotFound); // let RunError handle this
				}
			else
				{
				delete iResponseHandler; //just in case not freed
				iResponseHandler = NULL;
				iResponseHandler = CBTRegistryResponse::NewL(iView);
				iResponseHandler->Start(iStatus);
				SetActive();
				iState = EUsingResponse;
				}
			break;
			}
			
		case EUsingResponse:
			{
			RBTDeviceArray& array(iResponseHandler->Results());
			iNotifier.MbpdnAddPairedDevices(array);
			iState = EReady;
			break;
			}

		case EReady:
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EPairedBDAddrGetterUnexpectedState));
			break;
			}
		}
	}
	
	
TInt CBTPairedBDAddrGetter::RunError(TInt aError)
	{
	iNotifier.MbpdnErrorInGettingPairedDevices(aError);
	iState = EReady;
	return KErrNone;
	}
	

void CBTPairedBDAddrGetter::DoCancel()
	{
	// Cancelling registry search.
	iView.CancelRequest(iStatus);
	// Cancelling response retrieval.
	delete iResponseHandler;
	iResponseHandler = NULL;
	// Resetting state
	iState = EReady;
	}


