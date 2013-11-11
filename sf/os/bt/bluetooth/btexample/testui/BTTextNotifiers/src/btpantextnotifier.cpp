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

#include "BTTextNotifiers.h"
#include <btdevice.h>
#include <e32cons.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, "TextNotifiers");
#endif

_LIT(KBTPanDeviceSelectionNotifier, "PanDevSlctNotif"); // for panics
_LIT(KBTPanNotifierConsoleName, "PAN device selector");

//
// Notifier
//

CBTPanDeviceSelectionNotifier* CBTPanDeviceSelectionNotifier::NewLC()
	{
	LOG_STATIC_FUNC
	CBTPanDeviceSelectionNotifier* self = new(ELeave) CBTPanDeviceSelectionNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTPanDeviceSelectionNotifier::CBTPanDeviceSelectionNotifier()
	{
	LOG_FUNC
	}

void CBTPanDeviceSelectionNotifier::ConstructL()
	{
	LOG_FUNC
	iEngine = CBTPanDeviceSelectionNotifierEngine::NewL(*this);
	}

CBTPanDeviceSelectionNotifier::~CBTPanDeviceSelectionNotifier()
	{
	LOG_FUNC
	delete iEngine;
	}

void CBTPanDeviceSelectionNotifier::Release()
	{
	LOG_FUNC
	Cancel();
	delete this;
	}
	
CBTPanDeviceSelectionNotifier::TNotifierInfo CBTPanDeviceSelectionNotifier::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid=KBTPanDeviceSelectionNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVLow;
	return iInfo;
	}

CBTPanDeviceSelectionNotifier::TNotifierInfo CBTPanDeviceSelectionNotifier::Info() const
	{
	LOG_FUNC
	return iInfo;
	}

TPtrC8 CBTPanDeviceSelectionNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	User::Panic(KBTPanDeviceSelectionNotifier, KErrNotSupported);
	return KNullDesC8(); // should never be reached
	}
	
void CBTPanDeviceSelectionNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iMessageOutstanding = ETrue;

	TRAPD(err, iEngine->PromptForDevicesL(aBuffer));
	FTRACE(FPrint(_L("RNot\tPAN device selection notifier completed with result %d"), err));
	if (err)
		{
		aMessage.Complete(err);
		iMessageOutstanding = EFalse;
		User::Leave(err);
		}
	}
	
void CBTPanDeviceSelectionNotifier::Cancel()
	{
	LOG_FUNC
	iEngine->Stop();

	if (iMessageOutstanding)
		{
		iMessage.Complete(KErrCancel);
		iMessageOutstanding = EFalse;
		}
	}

TPtrC8 CBTPanDeviceSelectionNotifier::UpdateL(const TDesC8& /*aBuffer*/)
/**

*/
	{
	LOG_FUNC
	return KNullDesC8();
	}

void CBTPanDeviceSelectionNotifier::DeviceSelectionComplete(TBTDeviceList& aDevices)
/**
The user has selected the devices they want to connect to
*/
	{
	LOG_FUNC
	TInt err = iMessage.Write(iReplySlot, aDevices);
	iMessage.Complete(err);
	iMessageOutstanding = EFalse;
	}
	
void CBTPanDeviceSelectionNotifier::DeviceSelectionError(TInt aError)
/**
There was an error during device selection
*/
	{
	LOG_FUNC
	LOG1(_L("\taError = %d"), aError);
	if(iMessageOutstanding)
		{
		iMessage.Complete(aError);
		iMessageOutstanding = KErrNone;
		}
	}
	
//
// Notifier engine
//

_LIT(KNewline, "\n");
_LIT(KBTPanDSNSelectedDevice, "-> ");
_LIT(KBTPanDSNNotSelectedDevice, "   ");
_LIT(KBTPanDSNSelected, " * ");
_LIT(KBTPanDSNNotSelected, "   ");
_LIT(KBTPanDSNDevAddrOpen, "[");
_LIT(KBTPanDSNDevAddrClose, "]");
_LIT(KBTPanDeviceSelectionNotifierInstructions, "Use [up] and [down] arrow keys to move around; press [space] to select a device, press [return] to finish, [esc] to cancel:\nUp to 8 devices can be selected.\n");
_LIT(KBTPanDSNNoMoreDevices, " --- Device discovery complete ---");
const TUint KBTPanDSNDevNameMaxChars = 20;
const TUint KBTPanDSNMaxReadableBufLength = 12;
const TUint KBTPanDSNDeviceDisplayBufLength = (sizeof(KBTPanDSNSelected)/2)+KBTPanDSNDevNameMaxChars+(sizeof(KBTPanDSNDevAddrOpen)/2)+KBTPanDSNMaxReadableBufLength+(sizeof(KBTPanDSNDevAddrClose)/2);
const TUint KBTPanDSNInitialDeviceCount = KMaxDeviceRows; // start out by assuming we're going to discover this many devices - will realloc later if necessary
const TUint KBTPanDSNInitialBufSize = ((sizeof(KBTPanDeviceSelectionNotifierInstructions)/2)+(KBTPanDSNInitialDeviceCount*KBTPanDSNDeviceDisplayBufLength));

CBTPanDeviceSelectionNotifierEngine* CBTPanDeviceSelectionNotifierEngine::NewL(MBTPanDeviceSelectionNotify& aNotify)
/**

*/
	{
	LOG_STATIC_FUNC
	CBTPanDeviceSelectionNotifierEngine* self = new(ELeave) CBTPanDeviceSelectionNotifierEngine(aNotify);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTPanDeviceSelectionNotifierEngine::CBTPanDeviceSelectionNotifierEngine(MBTPanDeviceSelectionNotify& aNotify) :
	CActive(EPriorityStandard), iDevices(KMaxDeviceRows), iDisplayPtr(NULL, 0), iNotify(aNotify)
/**

*/
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTPanDeviceSelectionNotifierEngine::ConstructL()
/**

*/
	{
	LOG_FUNC
	iDisplayBuf = HBufC::NewL(KBTPanDSNInitialBufSize);
	iDisplayPtr.Set(iDisplayBuf->Des());
	}

CBTPanDeviceSelectionNotifierEngine::~CBTPanDeviceSelectionNotifierEngine()
/**

*/
	{
	LOG_FUNC
	delete iDeviceDiscoverer;
	iDeviceDiscoverer = NULL;
	
	delete iConsole;
	iConsole = NULL;
	
	delete iDisplayBuf;
	iDisplayBuf = NULL;
	
	iDevices.Close();
	}

void CBTPanDeviceSelectionNotifierEngine::PromptForDevicesL(const TDesC8& /*aBuffer*/)
/**
Start a device discovery
TODO: Reset state each time this is called
*/
	{
	LOG_FUNC
	iDeviceDiscoverer = CBTPanDeviceSelectionNotifierDeviceDiscoverer::NewL(*this);
	iConsole = Console::NewL(KBTPanNotifierConsoleName, TSize(KConsFullScreen,KConsFullScreen));
	Redraw();
	iDeviceDiscoverer->StartDeviceDiscovery();
	iConsole->Read(iStatus);
	SetActive();
	}

void CBTPanDeviceSelectionNotifierEngine::Stop()
/**
The notifier wants us to abort - stop everything
*/
	{
	LOG_FUNC
	AbortEverything();
	}

void CBTPanDeviceSelectionNotifierEngine::RunL()
/**
Got input from the user
*/
	{
	LOG_FUNC
	TChar key = iConsole->KeyCode();
	switch(key)
		{
		case EKeyUpArrow:
			{
			if(iCurrentlySelectedDevice>0)
				{
				--iCurrentlySelectedDevice;
				}
			if(iCurrentlySelectedDevice<iPositionOfTopDeviceInList)
				{
				--iPositionOfTopDeviceInList;
				}
			break;
			}
		case EKeyDownArrow:
			{
			if(iCurrentlySelectedDevice < (iDevices.Count()-1))
				{
				++iCurrentlySelectedDevice;
				}
			if(iCurrentlySelectedDevice>=(iPositionOfTopDeviceInList+KMaxDeviceRows))
				{
				++iPositionOfTopDeviceInList;
				}
			break;
			}
		case EKeySpace:
			{
			if(iDevices.Count()) // make sure there's something in the array
				{
				if(iDevices[iCurrentlySelectedDevice]->iSelected)
					{
					iDevices[iCurrentlySelectedDevice]->iSelected = EFalse;
					}
				else
					{
					if(NumberOfDevicesSelected()<KMaxPanDevicesForSimultaneousSelection)
						{
						iDevices[iCurrentlySelectedDevice]->iSelected = ETrue;
						}
					}
				}
			break;
			}
		case EKeyEnter:
			{
			PackageSelectedDevicesAndCompleteNotifier();
			return;
			}
		default:
			// fall through
		case EKeyEscape:
			{
			CompleteNotifierWithError(KErrCancel);
			return;
			}
		}
		
	Redraw();
	iConsole->Read(iStatus);	// post another read
	SetActive();
	}

void CBTPanDeviceSelectionNotifierEngine::DoCancel()
/**

*/
	{
	LOG_FUNC
	iConsole->ReadCancel();
	}

void CBTPanDeviceSelectionNotifierEngine::NewDeviceFoundL(TNameRecord& aNewDevice)
/**
Add the new device to the end of the list
*/
	{
	LOG_FUNC
	LOG1(_L("\tDevice Name = %S"), &(aNewDevice.iName));
	TBTDeviceSelectionInfo* devInfo = new(ELeave) TBTDeviceSelectionInfo();
	devInfo->iNameRecord = aNewDevice;
	devInfo->iSelected = EFalse;
	User::LeaveIfError(iDevices.Append(devInfo));

	// check if we need to allocate bigger buffer
	TUint instructions = sizeof(KBTPanDeviceSelectionNotifierInstructions)/2;
	TUint devices = iDevices.Count();
	TUint devDisplay = KBTPanDSNDeviceDisplayBufLength;
	TUint minRequiredBufSize = instructions+(devices*devDisplay);
	if(iDisplayPtr.MaxLength() < minRequiredBufSize)
		{
		delete iDisplayBuf;
		iDisplayBuf = NULL;
		TUint size = (sizeof(KBTPanDeviceSelectionNotifierInstructions)/2)+2*iDevices.Count()*KBTPanDSNDeviceDisplayBufLength;
		iDisplayBuf = HBufC::NewL( size );
		iDisplayPtr.Set(iDisplayBuf->Des());
		}		
	Redraw();
	}

void CBTPanDeviceSelectionNotifierEngine::DeviceSearchComplete(TInt aError)
/**
No more devices can be found
*/
	{
	LOG_FUNC
	LOG1(_L("\taError = %d"), aError);
	if(aError!=KErrEof)	// there's a problem, so complete with an error
		{
		CompleteNotifierWithError(aError);
		return;
		}
	else
		{
		iDeviceSearchComplete = ETrue;		
		Redraw();
		}
	}

TInt CBTPanDeviceSelectionNotifierEngine::DisplayDevice(TUint aDeviceNumber, TPtr& aDisplayBuf)
/**

*/
	{
	LOG_FUNC
	//ASSERT(aDisplayBuf.Length()==KBTPanDSNDeviceDisplayBufLength);
	if(aDeviceNumber>=iDevices.Count())
		{
		return KErrNotFound;
		}
	else
		{
		if(iDevices[aDeviceNumber]->iSelected)
			{
			aDisplayBuf.Append(KBTPanDSNSelected);
			}
		else
			{
			aDisplayBuf.Append(KBTPanDSNNotSelected);
			}
			
		aDisplayBuf.Append(iDevices[aDeviceNumber]->iNameRecord.iName.Left(KBTPanDSNDevNameMaxChars));	// append device name
		aDisplayBuf.Append(KBTPanDSNDevAddrOpen);
		TBuf<KBTPanDSNMaxReadableBufLength> readableDevAddrBuf;
		static_cast<TInquirySockAddr>(iDevices[aDeviceNumber]->iNameRecord.iAddr).BTAddr().GetReadable(readableDevAddrBuf);	// append device address
		aDisplayBuf.Append(readableDevAddrBuf);
		aDisplayBuf.Append(KBTPanDSNDevAddrClose);
		return KErrNone;
		}
	}
	
void CBTPanDeviceSelectionNotifierEngine::DisplayInstructions(TPtr& aDisplayBuf)
/**
~130 chars
*/
	{
	LOG_FUNC
	aDisplayBuf.Append(KBTPanDeviceSelectionNotifierInstructions);
	}

void CBTPanDeviceSelectionNotifierEngine::Redraw()
/**
Redraw the console
*/
	{
	LOG_FUNC
	iDisplayPtr.Zero();
	iConsole->ClearScreen();
	
	TInt err;
	DisplayInstructions(iDisplayPtr);
	for(TInt i = 0; i < KMaxDeviceRows; ++i)
		{
		if(iPositionOfTopDeviceInList+i==iCurrentlySelectedDevice)
			{
			iDisplayPtr.Append(KBTPanDSNSelectedDevice);
			}
		else
			{
			iDisplayPtr.Append(KBTPanDSNNotSelectedDevice);
			}
			
		err = DisplayDevice(iPositionOfTopDeviceInList+i, iDisplayPtr);
		if(err==KErrNotFound)	// no more devices left
			{
			break;
			}
		iDisplayPtr.Append(KNewline);
		}
		
	if(iDeviceSearchComplete)
		{
		iDisplayPtr.Append(KBTPanDSNNoMoreDevices);
		}
		
	iConsole->Write(*iDisplayBuf);
	}
	
void CBTPanDeviceSelectionNotifierEngine::PackageSelectedDevicesAndCompleteNotifier()
/**
Package the selected device addresses into the device list and pass back to the notifier
*/
	{
	LOG_FUNC
	AbortEverything();	// stop inquiry and remove console
	
	TBTDeviceList deviceList;
	for(TInt i = 0; i < iDevices.Count(); ++i)
		{
		if(iDevices[i]->iSelected)
			{
			TBTDevAddr devAddr = static_cast<TInquirySockAddr>(iDevices[i]->iNameRecord.iAddr).BTAddr();
			deviceList.AddDevice(devAddr);
			}
		}
	
	iNotify.DeviceSelectionComplete(deviceList);
	}

void CBTPanDeviceSelectionNotifierEngine::CompleteNotifierWithError(TInt aError)
/**
An error occurred during device selection - cancel everything and complete the notifier with an error
*/
	{
	LOG_FUNC
	if(iAborting)
		{
		// do nothing, we're already aborting everything
		}
	else
		{
		iAborting = ETrue; // make a note that we're aborting
		AbortEverything();
		iNotify.DeviceSelectionError(aError);
		iAborting = EFalse; // reset flag
		}
	}

void CBTPanDeviceSelectionNotifierEngine::AbortEverything()
/**
Common method to abort everything - called from both the notifier error path and our own
*/
	{
	LOG_FUNC
	if(iDeviceDiscoverer)
		{
		iDeviceDiscoverer->Cancel();	// cancel the device discovery, and delete the device discoverer
		delete iDeviceDiscoverer;
		iDeviceDiscoverer = NULL;
		}
		
	Cancel();						// cancel the input from the console
	if(iConsole)
		{
		delete iConsole;				// remove the console from screen
		iConsole = NULL;
		}
	}	
	
TInt CBTPanDeviceSelectionNotifierEngine::NumberOfDevicesSelected()
/**
Return the number of devices marked as selected in the array
*/
	{
	LOG_FUNC
	TInt count = 0;
	for(TInt i=0; i < iDevices.Count(); ++i)
		{
		if(iDevices[i]->iSelected)
			{
			++count;
			}
		}
	return count;
	}

