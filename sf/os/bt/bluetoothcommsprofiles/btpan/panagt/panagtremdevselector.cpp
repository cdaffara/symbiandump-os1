// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <networking/panuiinterfaces.h>
#include <comms-infras/dbaccess.h>
#include "panagtlog.h"
#include "panagtremdevselector.h"
using namespace PanAgent;

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

CPanRemoteDeviceSelector::CPanRemoteDeviceSelector(
	MOutgoingConnectionCreator& aOutgoingConnectionCreator, CCommsDbAccess& aDatabase) :
	CActive(KPanAgtAoRemoteDevicePromptPriority), 
	iOutgoingConnectionCreator(aOutgoingConnectionCreator), 
	iDatabase(aDatabase)
/**
Store all the pointers and chuck ourselves on the active scheduler
*/
	{
	CActiveScheduler::Add(this);
	}

CPanRemoteDeviceSelector::~CPanRemoteDeviceSelector()
	{
	iDevAddresses.Close();
	}

void CPanRemoteDeviceSelector::PerformRemoteDeviceSelectionL()
/**
Find out whether there are any remote devices in commdb, or whether we should be prompting 
(or whether we should just do nothing)
*/
	{
	LOG(_L("RemDevSelector: Deciding which device to connect to."));
	TInt err;
	
	TBool prompt;
	iDatabase.GetBoolL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_PROMPT_FOR_REMOTE_DEVICES), prompt);
	
	LOG1(_L("RemDevSelector: prompt mode is %d..."), prompt);
	
	if (prompt)
		{
		LOG(_L("RemDevSelector: ...starting notifier..."));
		
		// Check if we're already prompting for devices.
		if (IsPrompting())
			{
			User::Leave(KErrInUse);
			}
		
		User::LeaveIfError(err = iNotifier.Connect());
		iNotifier.StartNotifierAndGetResponse(iStatus, KBTPanDeviceSelectionNotifierUid, KNullDesC8, iDeviceList);
		iState = EWaitingForNotifier;
		SetActive();
		return;
		}
	
	// If we've got here, we aren't allowed to prompt until we've tried to connect to a fixed set of devices.
	LOG(_L("RemDevSelector: ...reading address(es) from commdb..."));

	TRAP(err, iDatabase.GetBoolL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_PROMPT_IF_MAC_LIST_FAILS), iPromptIfMACFails));
	if (err)
		{
		iPromptIfMACFails = EFalse;
		}
	LOG1(_L("RemDevSelector: prompt fallback set to %d"), iPromptIfMACFails);

	TBuf<KCommsDbSvrMaxFieldLength> remoteDeviceAddressList;
	TRAP(err, iDatabase.GetDesL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_PEER_MAC_ADDRESSES), remoteDeviceAddressList));
	
	// Parse buffer from commdb
	if((remoteDeviceAddressList.Length()) && !err) // firstly make sure there's something in the buffer
		{
		LOG1(_L("RemDevSelector: ...commdb buffer is %d bytes long..."), remoteDeviceAddressList.Length());
		
		ConvertListToArray(remoteDeviceAddressList);
		//Connect only to one of the devices at a time.
		}
	
	if (HasMoreDevices())
		{
		CreateNextConnectionL();
		}
	else
		{
		if(err)
			{
			LOG1(_L("RemDevSelector: ...reading device addressses from commdb failed with %d, and prompt mode disabled.  Giving up - this must be a listen only connection, or someone's screwed up the commdb settings."), err);
			}
		else
			{
			LOG(_L("RemDevSelector: ...no device addresses in commdb, and prompt mode disabled.  Giving up - this must be a listen only connection, or someone's screwed up the commdb settings."));
			}
		}
	}


/**
Called by the PanAgent upon disconnection (before PAN connection is up) to see if 
there is more devices it can try to connect
*/
TBool CPanRemoteDeviceSelector::HasMoreDevices()
	{
	return iPromptIfMACFails || (iDevAddresses.Count() > 0);
	}

/**
Called by the PanAgent upon device active to prevent PanAgent from trying the next 
device upon disconnection of a successful PAN connection.
*/
void CPanRemoteDeviceSelector::Reset()
	{
	iDevAddresses.Reset();
	}
	
void CPanRemoteDeviceSelector::RunL()
/**
Prompt has completed. We assume the user want to connect to all of them if multiple 
devices are selected.
*/
	{
	switch(iState)
		{
		case EWaitingForNotifier:
			{
			LOG1(_L("RemDevSelector: ...notifier completed with %d"), iStatus.Int());
			TInt err = iNotifier.CancelNotifier(KBTPanDeviceSelectionNotifierUid);
			LOG1(_L("RemDevSelector: ...CancelNotifier completed with %d"), err);

			if(iStatus==KErrNone)
				{
				TBTDevAddr devAddr;
				err = iDeviceList.GetDevice(devAddr);
				while(err==KErrNone)
					{
					TRAP(err, iOutgoingConnectionCreator.CreateNewConnectionForOutgoingL(devAddr));
					if(err) // might be that device is connected already, or a more general error
						{
						err = iOutgoingConnectionCreator.DeviceSelectionError(err); // replace one err with anoth-err
						if(err==KErrDisconnected)	// we've decided to disconnect
							{
							return;
							}
						}
					err = iDeviceList.GetDevice(devAddr);
					}
				iDeviceList.Reset();
				}
			else
				{
				if (iStatus==KErrCancel)
					{
					// The user has rejected the remote device(s) by cancelling the selector, 
					// but in order that CNifSession::DecideWhetherToRetry() will respond to 
					// this action by going to the next IAP in its connection preference list, 
					// we need to change the reported error...
					iStatus = KErrCouldNotConnect;
					}
					
				iOutgoingConnectionCreator.DeviceSelectionError(iStatus.Int());
				return;
				}
			break;
			}
		case ENotWaiting:
		// fall through
		default:
			{
			PanAgentPanic(EAsyncOperationOnOutgoingConnectionStarterWhenItsNotPerformingAnAsyncRequest);
			}
		}
	}

TBool CPanRemoteDeviceSelector::IsPrompting()
/**
Is there currently a prompt on screen?
*/
	{
	return((iState==EWaitingForNotifier)&&IsActive());
	}
	
void CPanRemoteDeviceSelector::DoCancel()
/**

*/
	{
	switch(iState)
		{
		case EWaitingForNotifier:
			{
			iNotifier.CancelNotifier(KBTPanDeviceSelectionNotifierUid);
			break;
			}
		case ENotWaiting:
		// fall through
		default:
			{
			PanAgentPanic(EAsyncOperationOnOutgoingConnectionStarterWhenItsNotPerformingAnAsyncRequest);
			break;
			}
		}
	}

void CPanRemoteDeviceSelector::ConvertListToArray(TDesC& aDeviceAddrList)
/**
Put the comma-separated list of readable BT device addresses into an array
*/
	{
	TBTDevAddr devAddr;
	TLex lex(aDeviceAddrList);

	TChar charComma(',');	// this weird construct is needed to prevent us needing static data
	while(lex.Peek()!=charComma && !lex.Eos())
	{
		lex.Mark();
		while((lex.Get()!=charComma) && !lex.Eos())
			{
			
			}
		devAddr.SetReadable(lex.MarkedToken());
		// RArrays default to a granularity of 8 and we only connect to one device
		// at a time so it's reasonable to ignore this error, as a) in common use
		// bluetooth piconet size can't go beyond 8 anyway and b) if we use up
		// all the 8/16/... addresses in this array the user will be prompted for
		// more (although they probably won't have enough memory to make the
		// connection anyway).
		(void)iDevAddresses.Append(devAddr);
		}
	}

/**
Connect to the first device in the address list and remove it from the list
*/
void CPanRemoteDeviceSelector::CreateNextConnectionL()
	{
	//Assertion here is just for flusing out internal problems e.g. The notifier completes the
	//Request with KErrNone while no devices are selected
	__ASSERT_DEBUG(HasMoreDevices(), PanAgentPanic(EPanAgtRemDevSelectorNoDevicesToConnect));
	
	// If we've run out of addresses in our list, CommsDat may be configured to allow us to ask
	// the user for another device.
	// In fact, if we're here and we have no devices, then we *MUST* be allowed to ask, as this
	// will have already been checked.  So we can assume the prompt flag without having to check.
	if (!iDevAddresses.Count())
		{
		iPromptIfMACFails = EFalse;		// To maintain symmetry with the consumption of the remote list, clear this flag.
		
		LOG(_L("RemDevSelector: ...starting notifier (empty devices list or no more remaining)..."));
		
		// Check if we're already prompting for devices.
		if (IsPrompting())
			{
			User::Leave(KErrInUse);
			}
		
		User::LeaveIfError(iNotifier.Connect());
		iNotifier.StartNotifierAndGetResponse(iStatus, KBTPanDeviceSelectionNotifierUid, KNullDesC8, iDeviceList);
		iState = EWaitingForNotifier;
		SetActive();
		return;
		}


	TBTDevAddr remDevAddr = iDevAddresses[0];

#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> tempDevAddrBuf;
	remDevAddr.GetReadable(tempDevAddrBuf, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG1(_L("RemDevSelector: Creating connection to new device, address: %S"), &tempDevAddrBuf);
#endif

	
	iDevAddresses.Remove(0);
	iOutgoingConnectionCreator.CreateNewConnectionForOutgoingL(remDevAddr);
	}
