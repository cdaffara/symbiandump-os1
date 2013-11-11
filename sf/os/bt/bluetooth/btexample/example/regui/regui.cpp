// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example UI for Registry
// 
//

#include "regui.h"


GLDEF_D TSize gSmallConsole(75,15);


// ----------------------------------------------------------------------------
//							Registry UI App
// ----------------------------------------------------------------------------

CRegUIApp::CRegUIApp()
	{
	}

CRegUIApp::~CRegUIApp()
	{
	delete iLocalDeviceUser;
	iUsers.ResetAndDestroy();
	iUsers.Close();
	iRegServ.Close();
	delete iActiveConsole;
	}

CRegUIApp* CRegUIApp::NewL()
	{
	CRegUIApp* thisapp = new (ELeave) CRegUIApp();
	CleanupStack::PushL(thisapp);
	thisapp->ConstructL();
	CleanupStack::Pop();
	return thisapp;
	}

void CRegUIApp::ConstructL()
	{
	iActiveConsole = CActiveConsole::NewL(*this);
	StartRegistryServerL();
	}

void CRegUIApp::StartRegistryServerL()
	{
	User::LeaveIfError(iRegServ.Connect());
	}

void CRegUIApp::StartL()
	{
	// go ....
	TestMenu();
	CActiveScheduler::Start();
	}

void CRegUIApp::Stop()
	{
	CActiveScheduler::Stop();
	}

void CRegUIApp::Error(TInt aError)
	{
	if (aError==KErrServerTerminated)
		{
		// try to restart server
		TRAP(aError, StartRegistryServerL());
		if (aError)
			{
			iActiveConsole->Console().Printf(_L("Failed to restart server. Error: %d\n"), aError);
			iActiveConsole->Console().Printf(_L("Stopping App\n"));			
			Stop();
			}
		}
	}

void CRegUIApp::CreateLocalDeviceL()
	{
	iLocalDeviceUser = CLocalDeviceUser::NewL(*this, iRegServ);
	}

void CRegUIApp::CreateViewL(const TBTRegistrySearch& aSearch)
	{
	CActiveRegistryUser* u = NewRegUserPleaseL();
	u->Search(aSearch);
	}
	
CActiveRegistryUser* CRegUIApp::NewRegUserPleaseL()
	{
	CActiveRegistryUser* u = CActiveRegistryUser::NewL(*this, iRegServ);
	CleanupStack::PushL(u);
	User::LeaveIfError(iUsers.Append(u));
	CleanupStack::Pop(u);
	return u;	
	}
	
void CRegUIApp::KindlyRemoveRegUserPleaseL(CActiveRegistryUser* aUser)
	{
	for (TInt i=0; i<iUsers.Count(); i++)
		{
		if (iUsers[i]==aUser)
			{
			delete aUser;
			iUsers.Remove(i);
			}
		}
	}
	
void CRegUIApp::KindlyRemoveLocalDeviceUserPlease()
	{
	delete iLocalDeviceUser;
	iLocalDeviceUser = NULL;
	}
	
void CRegUIApp::TestMenu()
	{
	CConsoleBase& con = iActiveConsole->Console();
	con.Printf(_L("Choose action...\n\n"));

	con.Printf(_L("a.\t\tList all devices\n"));
	con.Printf(_L("b.\t\tList all bonded devices\n"));
	con.Printf(_L("t.\t\tList all trusted devices\n"));	
	con.Printf(_L("p.\t\tList all Phones\n"));
	con.Printf(_L("c.\t\tList all Computers\n"));
	con.Printf(_L("l.\t\tList all Laptops\n"));
	con.Printf(_L("v.\t\tList all least sig. cookied devices\n"));
	con.Printf(_L("o.\t\tList all added devices\n"));
	con.Printf(_L("s.\t\tFind specific device\n"));
	con.Printf(_L("0.\t\tClear up completed tasks\n"));
	con.Printf(_L("L.\t\tExamine local device info\n"));
	con.Printf(_L("\n"));
	con.Printf(_L("A.\t\tAdd test device\n"));
	con.Printf(_L("Esc.\tStop\n"));
	iActiveConsole->RequestKey();
	}
	

void CRegUIApp::KeyPressedL(TChar aKey)	
	{
	TBTRegistrySearch searchCriteria;
	
	switch (aKey)
		{
	case 'a':
		{	
		iActiveConsole->Console().Printf(_L("Listing all devices...\n"));
		searchCriteria.FindAll();
		CreateViewL(searchCriteria); 
		break;
		}

	case 'b':
		{	
		iActiveConsole->Console().Printf(_L("Listing bonded devices...\n"));
		searchCriteria.FindBonded();
		CreateViewL(searchCriteria); 
		break;
		}
		
	case 't':
		{	
		iActiveConsole->Console().Printf(_L("Listing trusted devices...\n"));
		searchCriteria.FindTrusted();
		CreateViewL(searchCriteria); 
		break;
		}
		
	case 'p':
		{
		iActiveConsole->Console().Printf(_L("Listing phones...\n"));
		TBTDeviceClass phone(0, EMajorDevicePhone, 0);
		searchCriteria.FindCoD(phone.DeviceClass(), EMajorDevice);
		CreateViewL(searchCriteria);
		break;
		}

	case 'c':
		{
		iActiveConsole->Console().Printf(_L("Listing computers...\n"));
		TBTDeviceClass computer(0, EMajorDeviceComputer, 0);
		searchCriteria.FindCoD(computer.DeviceClass(), EMajorDevice);
		CreateViewL(searchCriteria);
		break;
		}
	case 'l':
		{
		iActiveConsole->Console().Printf(_L("Listing laptops...\n"));
		TBTDeviceClass laptop(0, EMajorDeviceComputer, EMinorDeviceComputerLaptop);
		searchCriteria.FindCoD(laptop.DeviceClass(), static_cast<TBTDeviceClassSearch>(EMajorDevice | EMinorDevice));
		CreateViewL(searchCriteria);
		break;
		}
	case 'v':
		{
		iActiveConsole->Console().Printf(_L("Listing least sig. cookied devices...\n"));
		searchCriteria.FindUiCookie(0x00000001, 0x000000001);
		CreateViewL(searchCriteria);
		break;
		}
	case 'o':
		{
		iActiveConsole->Console().Printf(_L("Listing owned devices...\n"));
		searchCriteria.FindBonded();
		searchCriteria.FindCurrentProcessOwned();
		CreateViewL(searchCriteria);
		break;
		}
	case '0':
		{
		for (TInt i = 0; i<iUsers.Count(); i++)
			{
			iUsers[i]->DestroyIfDone();
			}
		break;
		}
		
	case 'L':
		{
		if(!iLocalDeviceUser)
			{
			iActiveConsole->Console().Printf(_L("Creating local device window...\n"));
			CreateLocalDeviceL();
			}
		else
			{
			iActiveConsole->Console().Printf(_L("Already have local device window...\n"));
			}
		break;
		}
		
	case 'A':
		{
		iActiveConsole->Console().Printf(_L("Adding test device...\n"));
		_LIT8(KTestDeviceAddr, "\x00\x01\x02\x03\x04\x05");
		TBTDevAddr addr(KTestDeviceAddr);
		CBTDevice* device = CBTDevice::NewLC(addr);
		device->SetFriendlyNameL(_L("RegUI Test Device"));
		RBTRegistry reg;
		User::LeaveIfError(reg.Open(iRegServ));
		CleanupClosePushL(reg);
		TRequestStatus status;
		reg.AddDeviceL(*device, status);
		User::WaitForRequest(status);
		iActiveConsole->Console().Printf(_L("Result = %d\n"), status.Int());
		User::LeaveIfError(status.Int());
		CleanupStack::PopAndDestroy(2); // reg, device
		break;
		}

	case 's':
		break;

	case EKeyEscape:
		{
		Stop();
		return;
		}


	default:
		iActiveConsole->Console().Printf(_L("Unknown command\r\n"));
		}
		
	TestMenu();
	}

TBTDeviceName CRegUIApp::GetName()
	{
	// do it sync for now
	RTest nameConsole(_L("Get name"));
	TBTDeviceName name;
	TChar ch;
	do
		{
		ch = nameConsole.Getch();
		if (ch==0x8)
			{
			name.SetLength(name.Length()-1);
			}
		else if (ch!=0xd) 
			{
			name.Append(ch);
			}
		nameConsole.Console()->SetPos(0);
		nameConsole.Console()->Printf(name);
		} while (ch!=0xd);
	nameConsole.Close();
	return name;
	}

// ----------------------------------------------------------------------------
//							Active RegUser
// ----------------------------------------------------------------------------

CActiveRegistryUser::CActiveRegistryUser(CRegUIApp& aApp, RBTRegServ& aRegServ)
: CActive(EPriorityStandard), iRegistryServer(aRegServ), iApp(aApp)
	{
	CActiveScheduler::Add(this);
	}

CActiveRegistryUser::~CActiveRegistryUser()
	{
	Cancel();
	delete iActiveConsole;
	delete iResponseHandler;
	delete iViewObserver;
	//iChildUser is NOT owned by us
	iView.Close();
	}
	
CActiveRegistryUser* CActiveRegistryUser::NewL(CRegUIApp& aApp, RBTRegServ& aRegServ)
	{
	CActiveRegistryUser* u = new (ELeave) CActiveRegistryUser(aApp, aRegServ);
	CleanupStack::PushL(u);
	u->ConstructL();
	CleanupStack::Pop(u);
	return u;
	}
	
void CActiveRegistryUser::ConstructL()
	{
	iActiveConsole = CActiveConsole::NewL(*this);
	User::LeaveIfError(iView.Open(iRegistryServer));
	}
	
void CActiveRegistryUser::DestroyIfDone()
	{
	if (iState == EDone)
		{
		iApp.KindlyRemoveRegUserPleaseL(this); //Qualified
		}
	}

void CActiveRegistryUser::KeyPressedL(TChar aKey)
	{
	// non supported at moment
	CConsoleBase& con = iActiveConsole->Console();

	// these valid in any state	
	if (PreProcessKey(aKey))
		return;
	
	if (iState == ESelecting)
		{
		if ( (aKey == 'u') || ( aKey == 'U') )
			{
			// the user typed u or U in order to unpair all devices displayed
			UnpairAllDevicesInView();
			}
		else if ( (aKey == 'd') || ( aKey == 'D') )
			{
			// the user typed d or D in order to delete all devices displayed
			Delete();
			}
		else
			{
			// the input was numerical, this is to select one device only from the list	
			// just to make this simple, and to show that synchronous can be BAD (can't cancel)
			TInt n = static_cast<TInt>(aKey)-49;
			
			if (n>=0 && n<=iResponseHandler->Results().Count())
				{
				// create that device as the child
				iChildUser = iApp.NewRegUserPleaseL(); //Qualified
			
				TBTRegistrySearch s;
				s.FindAddress(iResponseHandler->Results()[n]->BDAddr());
				iChildUser->Search(s);
				}
			else
				{
				con.Printf(_L("Invalid device!\n"));
				}
			}
		iActiveConsole->RequestKey();
		}
	else if (iState == EReady)
		{
		switch (aKey)
			{
			case 'a':
				RequireAuthorisation(ETrue);
				break;
			case 'b':
				RequireAuthorisation(EFalse);
				break;			
			case 'u':
				Unpair();
				break;
			case 'd':
				Delete();
				break;
			case 'c':
					{
					TRAPD(err, SetCookieL())
					if(err != KErrNone)
						{
						iActiveConsole->Console().Printf(_L("Error %d\n"),err);
						}
					}
				break;
			case 'n':
				con.Printf(_L("Finished with this device\n"));
				iState = EDone;
				break;
			case 'f':
				TRAPD(err,ChangeFriendlyNameL());
				if (err)
					{
					iActiveConsole->Console().Printf(_L("Error %d\n"),err);
					}
				break;
			
			}
			
		}
	else
		{
		con.Printf(_L("Not ready\n"));
		}
	
	}


TBool CActiveRegistryUser::PreProcessKey(TChar aKey)
	{
	TBool processed = EFalse;
	
	switch (aKey)
		{
		case 'x':
		case EKeyEscape:
			// ok in any state
//			delete this;	// actually should tell app!
			processed = ETrue;
			break;
		default:
			break;
		}
	return processed;
	}
	
void CActiveRegistryUser::Search(const TBTRegistrySearch& aSearch)
	{
	// keep copy as we go async
	iSearch = aSearch;
	iView.CreateView(iSearch, iStatus);
	iState = ECreatingView;
	SetActive();
	}

void CActiveRegistryUser::Unpair()
	{
	// unpair the single device in this view
	if (!iResponseHandler->Results().Count())
		{
		iActiveConsole->Console().Printf(_L("whoops!\n"));
		}
	else
		{
		iView.UnpairDevice(iResponseHandler->Results()[0]->BDAddr(), iStatus);
		iState = EUnpairing;
		SetActive();
		}
	}
void CActiveRegistryUser::UnpairAllDevicesInView()
	{
	// unpair all the devices in this view
	if (!iResponseHandler->Results().Count())
		{
		iActiveConsole->Console().Printf(_L("whoops!\n"));
		}
	else
		{
		iView.UnpairAllInView(iStatus);
		iState = EUnpairing;
		SetActive();
		}
	}	
void CActiveRegistryUser::ChangeFriendlyNameL()
	{
	if (!iResponseHandler->Results().Count())
		{
		iActiveConsole->Console().Printf(_L("whoops!\n"));
		}
	else
		{
		// havent got around to a full text entry box :o)
		TBTDeviceName friendly = iApp.GetName();
		iView.ModifyFriendlyDeviceNameL(iResponseHandler->Results()[0]->BDAddr(), friendly, iStatus);
		iState = EChangingFriendlyName;
		SetActive();
		
		}
	}

void CActiveRegistryUser::SetCookieL()
	{
	if (!iResponseHandler->Results().Count())
		{
		iActiveConsole->Console().Printf(_L("whoops!\n"));
		}
	else
		{
		iDevice = iResponseHandler->Results()[0]->AsNamelessDevice();
		iDevice.SetUiCookie(0x00800001); // hardcoded for the moment :)
		iState = ESettingCookie;
		iView.ModifyDevice(iDevice, iStatus);
		SetActive();
		}
	}
		
void CActiveRegistryUser::RequireAuthorisation(const TBool aAuthorisationRequired)
	{
	if (!iResponseHandler->Results().Count())
		{
		// there is no device listed..
		iActiveConsole->Console().Printf(_L("whoops!\n"));
		}
	else
		{
		// set the authorisation requirement for the device in this view
		TBTNamelessDevice device;
		device = iResponseHandler->Results()[0]->AsNamelessDevice();
		
		TBTDeviceSecurity devSec = device.GlobalSecurity();
		devSec.SetNoAuthorise(!aAuthorisationRequired);
		device.SetGlobalSecurity(devSec);
		
		iView.ModifyDevice(device, iStatus);
		
		iState = EChangingAuthorisation;
		SetActive();
		}
	}
	

void CActiveRegistryUser::Delete()
	{
	// choose device and create view with it in...let's do by address
	// let user choose device from array
	if (!iResponseHandler->Results().Count())
		{
		iActiveConsole->Console().Printf(_L("whoops!\n"));
		}
	else
		{
		iView.DeleteAllInView(iStatus);
		iState = EDeleting;
		SetActive();
		}
	}

void CActiveRegistryUser::RunL()
	{
	switch (iState)
		{
		case ECreatingView:
			{
			// check for error or num devices
			TInt result = iStatus.Int();
			if (result == 0 || result == KErrNotFound)
				{
				iActiveConsole->Console().Printf(_L("No devices found by search"));
				iState = EDone;
				}
			else if(result < 0)
				{
				iActiveConsole->Console().Printf(_L("Error searching for devices (%d)"), result);
				iState = EDone;
				}
			else
				{
				if(result == 1)
					{
					iActiveConsole->Console().Printf(_L("*** 1 device found ***"));
					}
				else
					{
					iActiveConsole->Console().Printf(_L("*** %d devices found ***"), result);
					}
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

			// now display them
			for (TInt i=0; i<array.Count(); i++)
				{
				DoDisplayDevice(i, *array[i]);
				}
			// depending on how many devices we may need to choose
			if (array.Count() > 1)
				{
				ChooseDeviceL();
				iState = ESelecting; // actually this stays here now
				iViewObserver = CRegistryViewObserver::New(*this, iView);
				}
			else
				{
				// go and do stuff with that device
				MenuL();
				iState = EReady;
				iViewObserver = CRegistryViewObserver::New(*this, iView);
				}
			break;
			}			
		case EUnpairing:
			{
			iActiveConsole->Console().Printf(_L("Unpaired\n"));
			iState = EDone;
			break;
			}
		case EDeleting:
			{
			iActiveConsole->Console().Printf(_L("Deleted\n"));
			iState = EDone;
			break;
			}
		case EChangingFriendlyName:
			{
			iActiveConsole->Console().Printf(_L("FriendlyName Changed\n"));
			iState = EDone;			
			break;
			}
		case EChangingAuthorisation:
			{
			iActiveConsole->Console().Printf(_L("Authorisation requirement changed\n"));
			iState = EDone;			
			break;
			}
		case ESettingCookie:
			{
			iActiveConsole->Console().Printf(_L("Set UI Cookie completed\n"));
			iState = EDone;
			break;
			}
			
		}
	}
	
void CActiveRegistryUser::ChooseDeviceL()
	{
	CConsoleBase& con = iActiveConsole->Console();
	RBTDeviceArray& array = iResponseHandler->Results();
	
	con.Printf(_L("Enter U/D to Unpair/Delete all in view or Choose device 1-%d \n"), array.Count());
	iActiveConsole->RequestKey();
	}

void CActiveRegistryUser::MenuL()
	{
	CConsoleBase& con = iActiveConsole->Console();
	con.Printf(_L("\nu.\tUnpair\td.\tDelete\na.\tRequire Authorisation\tb.\tNo Authorisation(=Trust)\nf.\tchange Friendly name\tc.\tSet UI Cookie\nn.\tNothing\n"));
	iActiveConsole->RequestKey();
	}
		
TInt CActiveRegistryUser::RunError(TInt aError)
	{
	iActiveConsole->Console().Printf(_L("**RunL left with %d in state %d**\n"), aError, iState);
	iApp.Error(aError);
	return KErrNone;
	}


void CActiveRegistryUser::DoCancel()
	{
	iView.CancelRequest(iStatus);
	iActiveConsole->Console().Printf(_L("Cancelled.\n"));
	}


void CActiveRegistryUser::DoDisplayDevice(TInt aIndex, const CBTDevice& aDevice)
/**
	Pretty print a device
*/
	{
	// if Friendly name then print it
	// else if Bluetooth name print it
	// else print address
	// to emulate phone display only print 20 chars
	const TInt KMaxLen = 20;
	TBuf<KMaxLen> dispBuf; // could use Ptr (though makes rending BDAddr bit harder)

	CConsoleBase& con = iActiveConsole->Console();
	con.Printf(_L("\n"));

	if (aDevice.IsValidFriendlyName())
		dispBuf.Copy(aDevice.FriendlyName().Left(KMaxLen));
		
	else if (aDevice.IsValidDeviceName())
		dispBuf.Copy(aDevice.DeviceName().Left(KMaxLen));
	else
		aDevice.BDAddr().GetReadable(dispBuf);
	
	con.Printf(_L("%d: %S "), aIndex+1, &dispBuf);
	
	// show if bonded
	if (aDevice.IsValidPaired() && aDevice.IsPaired())
		{
		TBuf<40> linkKeyTypeString;
		switch(aDevice.LinkKeyType())
			{
		case ELinkKeyCombination:
			linkKeyTypeString = _L("Combination");
			break;
		case ELinkKeyUnauthenticatedUpgradable:
			linkKeyTypeString = _L("Unauthenticated");
			break;
		case ELinkKeyUnauthenticatedNonUpgradable:
			linkKeyTypeString = _L("Unauthenticated");
			break;
		case ELinkKeyAuthenticated:
			linkKeyTypeString = _L("Authenticated");
			break;
		case ELinkKeyDebug:
			linkKeyTypeString = _L("!!!DEBUG!!!");
			break;
		default:
			linkKeyTypeString = _L("Unknown Type");
			break;
			}
		con.Printf(_L("[Bonded: %S]"), &linkKeyTypeString);
		}
	
	con.Printf(_L("\n"));
	
	if (aDevice.IsValidPassKey())
		{	
		TBTPinCode passKey = aDevice.PassKey();
		con.Printf(_L(" - PassKey length: %d\n"), passKey().iLength);
		}
		
	if (aDevice.IsValidUsed())
		{
		TBuf<40> dateString;
		_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%:3%");

		TRAPD(err, aDevice.Used().FormatL(dateString, KDateString));
		if(err == KErrNone)
			{
			con.Printf(_L(" - Last used: %S\n"), &dateString);
			}
		}
	
	if (aDevice.IsValidSeen())
		{
		TBuf<40> dateString;
		_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%:3%");

		TRAPD(err, aDevice.Seen().FormatL(dateString, KDateString));
		if(err == KErrNone)
			{
			con.Printf(_L(" - Last used: %S\n"), &dateString);
			}
		}
		
	if (aDevice.IsValidDeviceClass())
		{
		LogCoD(aDevice.DeviceClass().DeviceClass());
		}
	
	if (aDevice.IsValidUiCookie())
		{
		con.Printf(_L(" - UI Cookie: 0x%08x\n"), aDevice.UiCookie());
		}
	}

void CActiveRegistryUser::LogCoD(TUint aCoD)
	{
	class TCoDServiceClass
		{
		public:
		TCoDServiceClass(TInt aBit, const TDesC& aName) {iBit = aBit, iName = aName;};
		TBool Supported(TUint aBitMask)
			{
			TInt mask = 1 << iBit;
			return (aBitMask & mask);
			};
		TInt iBit;
		TBuf <32> iName;
		};

	TBuf<20> majordeviceclassname;
	TBuf<30> minordeviceclassname;

	TBTDeviceClass cod(aCoD);
	
	TUint8 majordeviceclass = cod.MajorDeviceClass();
	TUint8 minordeviceclass = cod.MinorDeviceClass();

	switch(majordeviceclass)
		{
		case 0:
			majordeviceclassname=_L("Miscellaneous");
			break;
		case 1:
			majordeviceclassname=_L("Computer");
			switch(minordeviceclass)
				{
				case 0:
					minordeviceclassname=_L("Unspecified");
					break;
				case 1:
					minordeviceclassname=_L("Desktop workstation");
					break;
				case 2:
					minordeviceclassname=_L("Server");
					break;
				case 3:
					minordeviceclassname=_L("Laptop");
					break;
				case 4:
					minordeviceclassname=_L("HPC/PDA (clamshell)");
					break;
				case 5:
					minordeviceclassname=_L("Palm sized PC/PDA");
					break;
				default:
					minordeviceclassname=_L("UNKNOWN");
					break;
				}
			break;
		case 2:
			majordeviceclassname=_L("Phone");
			switch(minordeviceclass)
				{
				case 0:
					minordeviceclassname=_L("Unclassified");
					break;
				case 1:
					minordeviceclassname=_L("Cellular");
					break;
				case 2:
					minordeviceclassname=_L("Cordless");
					break;
				case 3:
					minordeviceclassname=_L("Smartphone");
					break;
				case 4:
					minordeviceclassname=_L("Wired modem/voice gateway");
					break;
				default:
					minordeviceclassname=_L("UNKNOWN");
					break;
				}
			break;
		case 3:
			majordeviceclassname=_L("LAN Access Point");
			switch(minordeviceclass)
				{
				case 0:
					minordeviceclassname=_L("Fully available");
					break;
				case 1:
					minordeviceclassname=_L("1-17% utilised");
					break;
				case 2:
					minordeviceclassname=_L("17-33% utilised");
					break;
				case 3:
					minordeviceclassname=_L("33-50% utilised");
					break;
				case 4:
					minordeviceclassname=_L("50-67% utilised");
					break;
				case 5:
					minordeviceclassname=_L("67-83% utilised");
					break;
				case 6:
					minordeviceclassname=_L("83-99% utilised");
					break;
				case 7:
					minordeviceclassname=_L("No service available");
					break;

				}
			break;
		case 4:
			majordeviceclassname=_L("Audio");
			break;
		case 5:
			majordeviceclassname=_L("Peripheral");
			break;
		case 32:
			majordeviceclassname=_L("Unspecified");
			break;
		default:
			majordeviceclassname=_L("UNKNOWN");
		};

	TCoDServiceClass serviceclasses[]=
		{
		TCoDServiceClass(13, _L("LIAC Response")),
		TCoDServiceClass(14, _L("-")),
		TCoDServiceClass(15, _L("-")),
		TCoDServiceClass(16, _L("-")),

		TCoDServiceClass(17, _L("Networking")),
		TCoDServiceClass(18, _L("Rendering")),
		TCoDServiceClass(19, _L("Capturing")),
		TCoDServiceClass(20, _L("ObjectTransfer")),
		TCoDServiceClass(21, _L("Audio")),
		TCoDServiceClass(22, _L("Telephony")),
		TCoDServiceClass(23, _L("Information"))
		};


	TInt numserviceclasses = sizeof(serviceclasses)/sizeof(TCoDServiceClass);
	
	TBuf<100> output;

/*	output.Format(_L("\t%S [0x%04x] | %S [0x%04x] (0x%08x)\n"),
							&majordeviceclassname,
							majordeviceclass,
							&minordeviceclassname,
							minordeviceclass,
							aCoD);
*/
	output.Format(_L(" - CoD: %S | %S\n"),
					&majordeviceclassname,
					&minordeviceclassname);

	iActiveConsole->Console().Printf(output);
	iActiveConsole->Console().Printf(_L(" - Services: "));
	
	for (TInt i = 0; i<numserviceclasses; i++)
		{
		if (serviceclasses[i].Supported(aCoD))
			{
			output.Format(_L("%S "), &serviceclasses[i].iName);
			iActiveConsole->Console().Printf(output);
			}
		}
	iActiveConsole->Console().Printf(_L("\n"));
	}

void CActiveRegistryUser::ViewChanged()
	{
	iActiveConsole->Console().Printf(_L("--Our view has changed!\n"));
	}

// ----------------------------------------------------------------------------
//							Local Device User
// ----------------------------------------------------------------------------

CLocalDeviceUser::CLocalDeviceUser(CRegUIApp& aApp, RBTRegServ& aRegServ)
: iRegistryServer(aRegServ), iApp(aApp)
	{
	}

CLocalDeviceUser::~CLocalDeviceUser()
	{
	delete iActiveConsole;
	iLocalDevice.Close();
	}
	
CLocalDeviceUser* CLocalDeviceUser::NewL(CRegUIApp& aApp, RBTRegServ& aRegServ)
	{
	CLocalDeviceUser* u = new (ELeave) CLocalDeviceUser(aApp, aRegServ);
	CleanupStack::PushL(u);
	u->ConstructL();
	CleanupStack::Pop(u);
	return u;
	}
	
void CLocalDeviceUser::ConstructL()
	{
	iActiveConsole = CActiveConsole::NewL(*this);
	User::LeaveIfError(iLocalDevice.Open(iRegistryServer));

	MenuL();
	}
	
void CLocalDeviceUser::KeyPressedL(TChar aKey)
	{
	// non supported at moment
	CConsoleBase& con = iActiveConsole->Console();
	
	switch (aKey)
		{
		case 's':
			{
			TInt err = iLocalDevice.Get(iLocalDeviceSettings);
			if(!err)
				{
				con.Printf(_L("Scan Setting = 0x%.2x\n"), iLocalDeviceSettings.ScanEnable());
				}
			else
				{
				con.Printf(_L("Couldn't get settings\n"));
				}
			
			iActiveConsole->RequestKey();
			break;
			}
		case 'x':
		case EKeyEscape:
			{
			iApp.KindlyRemoveLocalDeviceUserPlease();
			break;
			}
		default:
			{
			con.Printf(_L("Not Supported\n"));
			iActiveConsole->RequestKey();
			break;
			}
		};
	}

void CLocalDeviceUser::MenuL()
	{
	CConsoleBase& con = iActiveConsole->Console();
	con.Printf(_L("\ns.\tGet Scan Setting\td.\tDelete\n"));
	iActiveConsole->RequestKey();
	}

CRegistryViewObserver* CRegistryViewObserver::New(MViewNotify& aNotifiee, RBTRegistry& aView)
	{
	CRegistryViewObserver* self = new CRegistryViewObserver(aNotifiee, aView);
	return self;
	}

CRegistryViewObserver::CRegistryViewObserver(MViewNotify& aNotifiee, RBTRegistry& aView)
	: CActive(EPriorityStandard), iNotifiee(aNotifiee), iView(aView)
	{
	CActiveScheduler::Add(this);
	Queue();
	}

CRegistryViewObserver::~CRegistryViewObserver()
	{
	Cancel();
	}

void CRegistryViewObserver::RunL()
	{
	if (iStatus == KErrNone)
		{
		iNotifiee.ViewChanged();
		// Requeue request (view remains open)
		Queue();
		}
	else if (iStatus == KErrCompletion)
		{
		// The view is closing - count as a change but dont requeue request.
		iNotifiee.ViewChanged();
		}
	}

void CRegistryViewObserver::Queue()
	{
	iView.NotifyViewChange(iStatus);
	SetActive();
	}

void CRegistryViewObserver::DoCancel()
	{
	iView.CancelRequest(iStatus);
	}


// ----------------------------------------------------------------------------
//							Active Console
// ----------------------------------------------------------------------------


CActiveConsole::CActiveConsole(MActiveConsoleNotify& aNotify)
: CActive(EPriorityStandard), iNotify(aNotify)
	{
	CActiveScheduler::Add(this);
	}


CActiveConsole::~CActiveConsole()
	{
	Cancel();
	delete iConsole;
	}

CActiveConsole* CActiveConsole::NewL(MActiveConsoleNotify& aNotify)
	{
	CActiveConsole* console = new (ELeave) CActiveConsole(aNotify);
	CleanupStack::PushL(console);
	console->ConstructL();
	CleanupStack::Pop();
	return console;
	}

CConsoleBase* CActiveConsole::AutoSizeNewL(const TDesC& aTitle, TSize aSize)
	{
	// Try to create a console of our preferred size, otherwise we
	// guess the screen is too small and use its entirety.
	CConsoleBase* console = NULL;
	TRAPD(err, console = Console::NewL(aTitle, aSize));
	if (err != KErrNone)
		{
		// If we leave now it is not because of offscreen drawing.
		console = Console::NewL(aTitle, TSize(KConsFullScreen, KConsFullScreen));
		}
	return console;
	}

void CActiveConsole::ConstructL()
	{
	iConsole = AutoSizeNewL(_L("RegUI Subsession"), gSmallConsole);
	}

void CActiveConsole::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CActiveConsole::RequestKey()
	{
	DrawCursor();
	iConsole->Read(iStatus);
	SetActive();
	}

void CActiveConsole::DrawCursor()
	{
	iConsole->SetPos(0);
	iConsole->Printf(_L(">>"));
	}

void CActiveConsole::RunL()
	{
	// key has been pressed
	TChar ch = iConsole->KeyCode();
	iNotify.KeyPressedL(ch);
	}

TInt CActiveConsole::RunError(TInt aError)
	{
	iConsole->Printf(_L("**Active console RunL left with %d**\n"), aError);
	return KErrNone;
	}


void StartL()
	{
	CRegUIApp* app = CRegUIApp::NewL();
	app->StartL();

	delete app;
	//finished
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();	// Get CleanupStack

	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);

	TRAPD(err, StartL());

	delete activescheduler;
	delete cleanupStack;
	__UHEAP_MARKEND;

	return err;
	}


