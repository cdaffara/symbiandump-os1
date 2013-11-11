// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <es_sock.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <bluetooth/logger.h>
#include <e32keys.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, "TextNotifiers");
#endif

void CBTManDeviceScanner::NotifyErrorL(TRefByValue<const TDesC> aErrorNotification)
	{
	LOG_FUNC
	CConsoleBase* console = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selector"), TSize(50,2));
	CleanupStack::PushL(console);
	console->Printf(aErrorNotification);
	console->Printf(_L("\nPress any key to continue"));
	console->Getch();
	CleanupStack::PopAndDestroy();
	}

void CBTManDeviceScanner::ResetDeviceLists()
	{
	LOG_FUNC
	iDevices.ResetAndDestroy();
	iDevices.Close();
	
	iNoNameDevices.ResetAndDestroy();
	iNoNameDevices.Close();
	}


void CBTManDeviceScanner::EnsureConsoleExistsL()
	{
	LOG_FUNC
	if(!iConsole)
		{
		StopWaitMessage();
		iConsole = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selector"), TSize(70, 6+KMaxDeviceRows));
		iConsole->Printf(iWaitMessage);
		}
	}

void CBTManDeviceScanner::StartWaitMessageL()
	{
	LOG_FUNC
	delete iWaitConsole;
	iWaitConsole = 0;
	iWaitConsole = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selector"), TSize(20,1));
	iWaitConsole->Printf(iWaitMessage);
	}

void CBTManDeviceScanner::StopWaitMessage()
	{
	LOG_FUNC
	delete iWaitConsole;
	iWaitConsole = 0;
	}

void CBTManDeviceScanner::AddDiscoveredDeviceL(RBTDevices& aDeviceArray, const TNameEntry& aNameEntry)
	{
	LOG_FUNC
	TInquirySockAddr& sa = TInquirySockAddr::Cast(aNameEntry().iAddr);
	const TBTDevAddr& bdaddr = sa.BTAddr();
	CBTDevice* device = CBTDevice::NewL(bdaddr);
	CleanupStack::PushL(device);
	
	HBufC8* temp = HBufC8::NewLC(aNameEntry().iName.Length());	 // to convert iName to narrow
	TPtr8 ptr=temp->Des();
	ptr.Copy(aNameEntry().iName);

	device->SetDeviceNameL(*temp);
	TBTDeviceClass devClass(sa.MajorServiceClass(), 
							sa.MajorClassOfDevice(), 
							sa.MinorClassOfDevice());
	device->SetDeviceClass(devClass);
	User::LeaveIfError(aDeviceArray.Append(device));

	CleanupStack::PopAndDestroy(1, temp);
	CleanupStack::Pop(device);
	}

void CBTManDeviceScanner::SetupDeviceParams(RHostResolver aHr, TBTNamelessDevice& aDevInfo)
	{
	LOG_FUNC
	SetupDeviceParams(_L(""), aDevInfo);
	//iDevPms.SetDeviceAddress(aDevInfo.Addr());
	//iDevPms.SetDeviceClass(aDevInfo.Class());
	TInquirySockAddr addr;
	addr.SetIAC(KGIAC);
	TRequestStatus stat;
	addr.SetAction(KHostResName);
	addr.SetBTAddr(aDevInfo.Address());
	TNameEntry entry;
	aHr.GetByAddress(addr, entry, stat);
	User::WaitForRequest(stat);
	if (stat == KErrNone)
		{
		iDevPms.SetDeviceName(entry().iName);
		}
	}

void CBTManDeviceScanner::SetupDeviceParams(const THostName& aName, TBTNamelessDevice& aDevInfo)
	{
	LOG_FUNC
	iDevPms.SetDeviceAddress(aDevInfo.Address());
	iDevPms.SetDeviceClass(aDevInfo.DeviceClass());
	iDevPms.SetDeviceName(aName);
	}

void CBTManDeviceScanner::RubOutWaitMessage()
	{
	LOG_FUNC
	TBuf<1> character;
	if(iConsole->WhereX()||iConsole->WhereY())
		{
		TInt len = iWaitMessage.Length();
		for(TInt i=0;i<len;++i)
			{
			character.SetLength(0);
			character.Append(0x08);
			iConsole->Printf(_L("%S"),&character);
			}
		}
	}

void CBTManDeviceScanner::PrintDiscoveredDevice()
	{
	LOG_FUNC

	RubOutWaitMessage();
	
	const TBTDevAddr& bdaddr = iDevPms.BDAddr();
	iConsole->Printf(_L("%d.\t0x%02x%02x%02x%02x%02x%02x\t"),
						++iPrintedCounter,bdaddr[0],bdaddr[1],bdaddr[2],bdaddr[3],
						bdaddr[4],bdaddr[5]);
	if(!(iDevPms.DeviceName().Length()))
		iConsole->Printf(_L("(No name found)\t"));
	else
		{
		// need to widen it for displaying! just display 40 chars worth
		TBuf<40> dispBuf;
		dispBuf.Copy(iDevPms.DeviceName().Left(40));
		iConsole->Printf(_L("\"%S\"\t"), &dispBuf);
		}
	iConsole->Printf(_L("0x%04x-"),iDevPms.DeviceClass().MajorServiceClass());
	iConsole->Printf(_L("%02x-"),iDevPms.DeviceClass().MajorDeviceClass());
	iConsole->Printf(_L("%02x\n"),iDevPms.DeviceClass().MinorDeviceClass());
	iConsole->Printf(iWaitMessage);
	}

TBool CBTManDeviceScanner::DeviceNumberFromUserL(TBool allowMore)
	{
	LOG_FUNC
	TBuf<3> numBuf;
	TBuf<63> errStr;
	errStr.SetLength(0);
	numBuf.Zero();
	TKeyCode code;
	TBuf<1> character;

	RubOutWaitMessage();
	
	TBuf<0x100> prompt(_L("\nIf the device you require appears above,\n\
		\rselect the device number you require and THEN press 'Return'.\n\
		\rIf not,  "));
	if(allowMore)
		{
		prompt.SetLength(prompt.Length()-2);
		prompt.Append(_L("\npress 'Esc' to abort or 'Return' to seek more devices.\n"));
		iConsole->Printf(prompt);
		}
	else
		{
		prompt.SetLength(prompt.Length()-2);
		prompt.Append(_L(" - THERE ARE NO MORE DEVICES - so\n\
			\rpress 'Esc' to abort or 'Return' to ***REDO*** device inquiry.\n"));
		iConsole->Printf(prompt);
		}
	FOREVER
		{
		code = iConsole->Getch();
		if(!(numBuf.Length())&&code==EKeyEnter)
			{
			// No device number entered so this must be a redo device inquiry
			iScanState = EScanStateUnfinished;
			break;
			}
		if(!(numBuf.Length())&&code==EKeyBackspace)
			{
			// No device number entered so nothing to delete, just continue looking for input
			continue;
			}
		if(!(code>=0x30&&code<=0x39)&&code!=EKeyEscape&&code!=EKeyEnter&&code!=EKeyBackspace)
			{
			// Not a valid number and not escape, enter or backspace, just continue looking for input
			continue;
			}
		if(code==EKeyEscape)
			{
			// Escape is always abort regardless if a number has been entered
			iScanState = EScanStateCancelled;
			break;
			}
		character.SetLength(0);
		character.Append(code);
	
		if (code == EKeyEnter)
			{
			// We must have a number at this point otherwise the enter key would have forced a
			// redo inquiry above
			TLex lex(numBuf);
			TInt err = lex.Val(iChosenDeviceIndex, EDecimal);
			User::LeaveIfError(err);
			if(iChosenDeviceIndex<=iPrintedCounter&&
				iChosenDeviceIndex>=((iPrintedCounter-1)/KMaxDeviceRows)*KMaxDeviceRows+1)
				{
				iScanState = EScanStateSelected;
				break;
				}
			else
				{
				while((numBuf.Length() != 0))
					{
					character.SetLength(0);
					character.Append(EKeyBackspace);
					iConsole->Printf(_L("%S"),&character);
					numBuf.SetLength((numBuf.Length()-1));
					}
				while((errStr.Length() != 0))
					{
					character.SetLength(0);
					character.Append(EKeyBackspace);
					iConsole->Printf(_L("%S"),&character);
					errStr.SetLength((errStr.Length()-1));
					}
				errStr.Format(_L("Device %d does not exist! Please re-enter...."), iChosenDeviceIndex);
				iConsole->Printf(_L("%S"), &errStr);
				}
			}
		else if (code == EKeyBackspace)
			{
			// We must have a number at this point otherwise we would have asked for another key press
			// above
			iConsole->Printf(_L("%S"),&character);
			numBuf.SetLength((numBuf.Length()-1));
			}
		else
			{
			if (numBuf.Length() < numBuf.MaxLength())
				{
				// Store the number
				iConsole->Printf(_L("%S"),&character);
				numBuf.Append(code);
				}
			}
		}

	delete iConsole;
	iConsole = 0;

	//return FALSE if scan not terminated (by cancelation or selection)
	if(iScanState == EScanStateUnfinished)
		{
		StartWaitMessageL();
		return EFalse;
		}
	return ETrue;
	}

TBool CBTManDeviceScanner::GetByScanL(TBTDevAddr& aBDAddr, 
									  TBTDeviceName& aName, 
									  TBTDeviceClass& aClass)
	{
 	LOG_FUNC
	StartWaitMessageL();
    RSocketServ ss;
	TInt ret=ss.Connect();
	if(ret)
		{
		NotifyErrorL(_L("Internal error: unable to connect to outside socket."));
		return EFalse;
		}

	RHostResolver hr;

 	TProtocolDesc pInfo;
	// List out all the protocols
 	ret=ss.FindProtocol(_L("BTLinkManager"),pInfo);
	if(ret)
		{
		NotifyErrorL(_L("Internal error: unable to find Bluetooth."));
		ss.Close();
		return EFalse;
		}

	ret = hr.Open(ss,pInfo.iAddrFamily,pInfo.iProtocol);
	//ret = hr.Open(ss,KBTAddrFamily,KBTLinkManager);
	if(ret)
		{
		NotifyErrorL(_L("Internal error: unable to find Bluetooth."));
		ss.Close();
		return EFalse;
		}

	iScanState = EScanStateUnfinished;
	while(iScanState==EScanStateUnfinished)
		{
		iResultCountNoName = 0;
		iResultCountWithName = 0;
		iChosenDeviceIndex = 0;
		iPrintedCounter = 0;
		ResetDeviceLists();	

		TInquirySockAddr addr;
		addr.SetIAC(KGIAC);
		TRequestStatus stat;
		addr.SetAction(KHostResInquiry + KHostResName);	// Default: Both
		TNameEntry entry;
	
		hr.GetByAddress(addr, entry, stat);
		User::WaitForRequest(stat);

		//Collect surrounding devices in a device array
		iPrintedCounter = 0;
		while (stat == KErrNone)
			{
			if(entry().iName.Length())
				{
				EnsureConsoleExistsL();
				AddDiscoveredDeviceL(iDevices, entry);
				SetupDeviceParams(entry().iName, (*iDevices[iResultCountWithName]).AsNamelessDevice());
				PrintDiscoveredDevice();
				iResultCountWithName++;
				if(!(iPrintedCounter%KMaxDeviceRows))
					if(CBTManDeviceScanner::DeviceNumberFromUserL(ETrue))
						break;
				}
			else
				{
				// Don't add to the Devices list yet, add to a separate list for those
				// who's name needs to be host resolve-d.
				TNameEntry* copied = new(ELeave) TNameEntry();
				CleanupStack::PushL(copied);
				
				(*copied)().iName = entry().iName; // should be 0 length
				(*copied)().iAddr = entry().iAddr;
				(*copied)().iFlags = entry().iFlags;
				
				User::LeaveIfError(iNoNameDevices.Append(copied));
				CleanupStack::Pop(copied);
				iResultCountNoName++;
				}
			if(iResultCountNoName+iResultCountWithName==999)
				{
				EnsureConsoleExistsL();
				if(CBTManDeviceScanner::DeviceNumberFromUserL(EFalse))
					//=>User has requested to see more devices
					{
					iScanState = EScanStateCancelled;//ensure do not search on unnamed devices
					DisplayNoDeviceMessageL(ETrue);
					}
				break;
				}
			hr.Next(entry, stat);
			User::WaitForRequest(stat);
			}

		addr.SetAction( KHostResName );
		while(iResultCountNoName && iScanState==EScanStateUnfinished && 
			  iPrintedCounter<(iResultCountWithName+iResultCountNoName))
			{
			EnsureConsoleExistsL();
			//Should never happen....
			if(iPrintedCounter<iResultCountWithName)
				break;
			
			TNameEntry* noNameEntry = iNoNameDevices[iPrintedCounter-iResultCountWithName];
			AddDiscoveredDeviceL(iDevices, *noNameEntry);
			SetupDeviceParams(hr, (*iDevices[iPrintedCounter]).AsNamelessDevice());
			PrintDiscoveredDevice();
			if(!(iPrintedCounter%KMaxDeviceRows))
				if(CBTManDeviceScanner::DeviceNumberFromUserL(ETrue))
					break;
			}

		if(iConsole)
			CBTManDeviceScanner::DeviceNumberFromUserL(EFalse);
				/*NB because of parameter EFalse above will never change 
					 state to EScanStateUnfinished so will never start wait dialog.*/
		if(!iPrintedCounter)
			{
			DisplayNoDeviceMessageL(EFalse);
			iScanState = EScanStateCancelled;
			}
		CheckUnfinishedL();
		}

	//now extract the selected device number from the string...
	TBTNamelessDevice devInfo;
	if(iScanState == EScanStateSelected)
		{
		if(iChosenDeviceIndex<=iResultCountWithName)
			devInfo = (*iDevices[iChosenDeviceIndex-1]).AsNamelessDevice();
		else if(iChosenDeviceIndex<=(iResultCountWithName+iResultCountNoName))
			devInfo = (*iDevices[iChosenDeviceIndex-iResultCountWithName-1]).AsNamelessDevice();
		else
			{
			iScanState = EScanStateCancelled;
			NotifyErrorL(_L("Error taking in device selection."));
			}
		}

	if(iScanState == EScanStateSelected)
		{
		StartWaitMessageL();
		SetupDeviceParams(hr, devInfo);
		aBDAddr = iDevPms.BDAddr();
		aName.Copy(iDevPms.DeviceName());
		aClass = iDevPms.DeviceClass();
		StopWaitMessage();
		}

	//Shouldn't ever be true
	if(iScanState == EScanStateUnfinished)
		{
		DisplayNoDeviceMessageL(iPrintedCounter?ETrue:EFalse);
		}

	StopWaitMessage();
	hr.Close();
	ss.Close();

	return (iScanState == EScanStateSelected)?ETrue:EFalse;
	}


void CBTManDeviceScanner::DisplayNoDeviceMessageL(TBool isMore)
	{
	LOG_FUNC
	CConsoleBase* console = 0;
	TRAPD(err, console = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selection"), TSize(45,4)));
	if(err)
		return;
	CleanupStack::PushL(console);

	TBuf<0x40> prompt(_L("Sorry! No"));
	if(isMore)
		prompt.Append(_L(" more"));
	prompt.Append(_L(" devices could be found.\n\nPress any key."));
	console->Printf(prompt);
	console->Getch();
	CleanupStack::PopAndDestroy();//console
	}

	
void CBTManDeviceScanner::CheckUnfinishedL()
	{
	LOG_FUNC
	if(iScanState!=EScanStateUnfinished||
		(iResultCountWithName+iResultCountNoName)%KMaxDeviceRows)
		return;

	CConsoleBase* console = 0;
	TRAPD(err, console = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selection"), TSize(70,3)));
	if(err)
		return;
	CleanupStack::PushL(console);

	TBuf<0x100> prompt(_L("Sorry!"));
	prompt.Append(_L(" - THERE ARE NO MORE DEVICES - so\n\
		\rpress 'Esc' to abort or 'Return' to ***REDO*** device inquiry.\n"));
	console->Printf(prompt);
	TKeyCode code = console->Getch();
	while(code!=0x1b&&code!=0x0d)
		code = console->Getch();
	iScanState = code==0x0d?EScanStateUnfinished:EScanStateCancelled;
	CleanupStack::PopAndDestroy();//console
	}



CBTManDeviceScanner* CBTManDeviceScanner::NewL()
	{
	LOG_STATIC_FUNC
	CBTManDeviceScanner* self=new (ELeave) CBTManDeviceScanner();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CBTManDeviceScanner::~CBTManDeviceScanner()
	{
	LOG_FUNC
	delete iConsole;
	ResetDeviceLists();
	iDevices.Close();
	iNoNameDevices.Close();
	}

CBTManDeviceScanner::CBTManDeviceScanner()
	: iWaitMessage(_L("Please wait..."))
	{
	LOG_FUNC
	}

void CBTManDeviceScanner::ConstructL()
	{
	LOG_FUNC
	}




