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
// BT Host Resolver test code
// 
//

#include "teir.h"
#include <utf.h>
#include <btsdp.h>
#include <e32math.h>

#define USE_SMALL_CONSOLE 0

typedef TBuf<20> TBTDevAddrBuf;
const TUint8 KEIRDataOffset = 1;
//_LIT(KManufacturerData, "This is a very very very very very very very very very very very very very very very very very very very very very very very very very very long data from phone manufacturer!");
_LIT(KManufacturerData, "This is a very very long data from phone manufacturer!");
_LIT(KManufacturerData1, "");


GLDEF_D RTest test(_L("Extended Inquiry Response tests"));

GLDEF_D TBTDevAddr gDevAddr(MAKE_TINT64(0x0002, 0x5b019a39)); // csr
//GLDEF_D TBTDevAddr gDevAddr(MAKE_TINT64(0x001b, 0xdc0f5cae)); // EIR windows
GLDEF_D TSize gSmallConsole(75,50);

//TInt CActiveHostResolver::iHRNumber = 0;



/** 
 Appends a TDesC string to a TDes string. 
 
 @param aDes String, which aOrg appends to
 @param aDesC String to be appended to aDest
 @return EFalse, if there not enough space in the TDes sting to contain the TDesC string. Otherwise, ETrue.
 */
TBool AppendDes(TDes& aDest, const TDesC& aOrg)
	{
	if((aDest.MaxLength() - aDest.Length()) < aOrg.Length())
		return EFalse;

	aDest.Append(aOrg);

	return ETrue;
	}


// ----------------------------------------------------------------------------
//							EIR App
// ----------------------------------------------------------------------------

CEirApp::CEirApp(CConsoleBase& aConsole)
: iConsole(aConsole)
	{
	}

CEirApp::~CEirApp()
	{
	iResolvers.ResetAndDestroy();
	iResolvers.Close();

	iDevicesFound.Close();

	StopProtocols();

	delete iActiveConsole;
	delete iNameRequester;

	if(iEirPublisherTestManuSpecific)
		{
		delete iManuNotifier;
		delete iEirPublisherTestManuSpecific;
		iManuNotifier = NULL;
		iEirPublisherTestManuSpecific = NULL;
		}
	iSdpDB.Close();
	}

CEirApp* CEirApp::NewL(CConsoleBase& aConsole)
	{
	CEirApp* thisapp = new (ELeave) CEirApp(aConsole);
	CleanupStack::PushL(thisapp);
	thisapp->ConstructL(aConsole);
	CleanupStack::Pop(thisapp);
	return thisapp;
	}

void CEirApp::ConstructL(CConsoleBase& aConsole)
	{
	iConsole.Printf(_L("Using HCI_V2\n"));
	iConsole.Printf(_L("Bluetooth Protocols Loading...\n"));
    InitialiseL();
    iConsole.Printf(_L("Bluetooth Protocols Loaded.\n"));
	ShowAddress();
	iActiveConsole = CActiveConsole::NewL(aConsole, *this);
	iManuNotifier = CManufacturerDataNotifier::NewL(*this);
	iEirPublisherTestManuSpecific = CEirPublisherManufacturerSpecificData::NewL(*iManuNotifier);
	}

void CEirApp::StartL()
	{
	// go ....
	CActiveScheduler::Start();
	}

void CEirApp::Stop()
	{
	CActiveScheduler::Stop();
	}
	
void CEirApp::SetPublishManuData(TUint aAvailableDataSize)
	{
	// TODO: some logic can be added here to cope with the given aAvailableDataSize
	// maybe truncating the intended data 
	HBufC8* buf = HBufC8::NewL(240);// LocalName-granular truncation
	buf->Des().Append(KManufacturerData);
	if(buf->Des().Length() > aAvailableDataSize)
		{
		TPtrC8 ptr(buf->Left(aAvailableDataSize));
		iEirPublisherTestManuSpecific->SetManufacturerSpecificData(ptr);
		}
	else
		{
		iEirPublisherTestManuSpecific->SetManufacturerSpecificData(*buf);
		}
	delete buf;
	}

void CEirApp::NewEirPublisherTestL(TTestEirTag aTag)
	{
	switch(aTag)
		{
	case ETestDeviceName:
		{
		CActiveInquirer* inquirer = CActiveInquirer::NewL(iConsole, *this, 0, EFalse, EFalse, EFalse, EFalse);
		CleanupStack::PushL(inquirer);
		iResolvers.AppendL(inquirer);
		CleanupStack::Pop(inquirer);
		iNameRequester->SetLocalNameL();
		}
		break;

	case ETestUuid16:
	
		Register16BitSDPRecordL();
		break;
	
	case ETestUuid128:
		Register128BitSDPRecordL();
		break;
	
	case ETestSetMSD:		
		if(!iManuNotifier && !iEirPublisherTestManuSpecific)
			{
			iManuNotifier = CManufacturerDataNotifier::NewL(*this);
			iEirPublisherTestManuSpecific = CEirPublisherManufacturerSpecificData::NewL(*iManuNotifier);
			}
		iEirPublisherTestManuSpecific->RequestPublishDataSize(55);
		break;
	case ETestRemoveMSD:
		if(iEirPublisherTestManuSpecific)
			{
			delete iEirPublisherTestManuSpecific;
			}
		break;
	default:
        iConsole.Printf(_L("INVALID EIR TAG!\n"));
		//User::Panic();
		break;	
		}
	}

void CEirApp::Register16BitSDPRecordL()
	{
	CreateSdpRecordL(0x1116);
	CreateSdpRecordL(0x1234, EFalse);
	CreateSdpRecordL(0xAABB);
	CreateSdpRecordL(0xF00D);
	CreateSdpRecordL(0x0E0D);
	}

void CEirApp::Register128BitSDPRecordL()
	{
	// Set Attr 1 (service class list) to list with UUID = 0x0123456789abcdef
	TUUID uuid1280 = TUUID(0x00000000, 0x00000000, 0x00000000, 0x00000000);
	TUUID uuid1281 = TUUID(0x11111111, 0x11111111, 0x11111111, 0x11111111);
	TUUID uuid1282 = TUUID(0x22222222, 0x22222222, 0x22222222, 0x22222222);
	TUUID uuid1283 = TUUID(0x33333333, 0x33333333, 0x33333333, 0x33333333);
	TUUID uuid1284 = TUUID(0x44444444, 0x44444444, 0x44444444, 0x44444444);
	TUUID uuid1285 = TUUID(0x55555555, 0x55555555, 0x55555555, 0x55555555);
	TUUID uuid1286 = TUUID(0x66666666, 0x66666666, 0x66666666, 0x66666666);
	TUUID uuid1287 = TUUID(0x77777777, 0x77777777, 0x77777777, 0x77777777);
	TUUID uuid1288 = TUUID(0x88888888, 0x88888888, 0x88888888, 0x88888888);
	TUUID uuid1289 = TUUID(0x99999999, 0x99999999, 0x99999999, 0x99999999);
	
	CreateSdpRecordL(uuid1280);
	CreateSdpRecordL(uuid1281, EFalse, EFalse);
	CreateSdpRecordL(uuid1282);
	CreateSdpRecordL(uuid1283);
	CreateSdpRecordL(uuid1284);
	CreateSdpRecordL(uuid1285);
	CreateSdpRecordL(uuid1286);
	CreateSdpRecordL(uuid1287);
	CreateSdpRecordL(uuid1288);
	CreateSdpRecordL(uuid1289);
	}

void CEirApp::CreateSdpRecordL(const TUUID& aUUID,
							  TBool aIsPublicBrowseGroup,
							  TBool aIsL2cap)
	{
	TSdpServRecordHandle recHandle;
	CSdpAttrValueDES* attrValDES = NULL;

	iSdpDB.CreateServiceRecordL(aUUID, recHandle);
	if(aIsPublicBrowseGroup)
		{
		// Browse Group List
		attrValDES = CSdpAttrValueDES::NewDESL(NULL);
		CleanupStack::PushL(attrValDES);
		attrValDES
			->StartListL()
			->BuildUUIDL(TUUID(TUint16(KPublicBrowseGroupUUID))) // Public Browse Group
			->EndListL();
		iSdpDB.UpdateAttributeL(recHandle, KSdpAttrIdBrowseGroupList, *attrValDES);
		CleanupStack::PopAndDestroy(attrValDES);
		}
	
	if(aIsL2cap)
		{
		// protocol Descriptor List
		attrValDES = CSdpAttrValueDES::NewDESL(NULL);
		CleanupStack::PushL(attrValDES);		
		attrValDES
			->StartListL()
			->BuildUUIDL(TUUID(KL2CAPUUID))
			->EndListL();
		iSdpDB.UpdateAttributeL(recHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
		CleanupStack::PopAndDestroy(attrValDES);
		}
	}

void CEirApp::NewNameRequesterL(TBool aEirInstead, TBool aIgnoreCache)
	{
	CActiveNameRequester* requester = CActiveNameRequester::NewL(iConsole, *this, aEirInstead, aIgnoreCache);
	CleanupStack::PushL(requester);
	iResolvers.AppendL(requester);
	CleanupStack::Pop(requester);
	}

void CEirApp::NewInquirerL(TUint aIAC, TBool aDoNames, TBool aIgnoreCache, TBool aDoEir, TBool forever)
	{
	CActiveInquirer* inquirer = CActiveInquirer::NewL(iConsole, *this, aIAC, aDoNames, aIgnoreCache, aDoEir, forever);
	CleanupStack::PushL(inquirer);
	iResolvers.AppendL(inquirer);
	CleanupStack::Pop(inquirer);
	}

void CEirApp::CancelInquirer()
	{
	TInt numresolvers = iResolvers.Count();
	if (numresolvers)
		iResolvers[numresolvers-1]->Cancel(); //FIXME! Cancel's the newest host resolver
	else
		iConsole.Printf(_L("No resolver to cancel!\n"));
	}

void CEirApp::NewNameArrayRequesterL()
	{
	CActiveNameArrayRequester* requester = CActiveNameArrayRequester::NewL(iConsole, *this, iDevicesFound);
	CleanupStack::PushL(requester);
	iResolvers.AppendL(requester);
	CleanupStack::Pop(requester);
	}

void CEirApp::DeviceFound(TBTDevAddr aAddress)
	{
#if USE_SMALL_CONSOLE
	iConsole.Printf(_L("App: Found device.\n"));
#endif
	(void)iDevicesFound.Append(aAddress);
	}

void CEirApp::InquiryComplete()
	{
	iConsole.Printf(_L("App: No more devices.\n"));
	iActiveConsole->DrawCursor();
	}

RSocketServ& CEirApp::SocketServ() 
	{
	return iSocketServer;
	}

void CEirApp::InitialiseL()
	{
	_LIT(KLDDName, "ECOMM");

#if defined (__WINS__)
	iConsole.Printf(_L("Initialise using EMULATOR PDD\n"));

	_LIT(KPDDName, "ECDRV");
#else
	iConsole.Printf(_L("Initialise using H2 PDD\n"));

	_LIT(KPDDName, "EUART1");
#endif

	TInt rerr = KErrNone;

	rerr = User::LoadPhysicalDevice(KPDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		iConsole.Printf(_L("Failed Load Physical Device %d!\n") ,rerr);
		User::Leave(rerr);
		}
	else 
		{
		iConsole.Printf(_L("Loaded LDD\n"));
		}

	rerr = User::LoadLogicalDevice(KLDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
	{
		iConsole.Printf(_L("Failed Load Logical Device%d!\n") ,rerr);
		User::Leave(rerr);
		}
	else 
		{
		iConsole.Printf(_L("Loaded PDD\n"));
		}
	User::LeaveIfError(iSocketServer.Connect());

	rerr = iSdp.Connect();
	if(rerr == KErrNone)
		{
		rerr = iSdpDB.Open(iSdp);
		}

	TProtocolDesc iProtocolInfo;
	TRequestStatus status;

	User::LeaveIfError(iSocketServer.FindProtocol(_L("L2CAP"), iProtocolInfo));
	iSocketServer.StartProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);

	User::LeaveIfError(iSocketServer.FindProtocol(_L("RFCOMM"), iProtocolInfo));

	User::LeaveIfError(iSocketServer.FindProtocol(_L("BTLinkManager"), iProtocolInfo));
	}

void CEirApp::StopProtocols()
	{
	TInt err;
	TProtocolDesc iProtocolInfo;
	TRequestStatus status;

	if(!iSocketServer.Handle())
		{
		// if the socket server has not even been opened
		// then there is no point trying to unload protocols
		return;
		}

	err = iSocketServer.FindProtocol(_L("L2CAP"), iProtocolInfo);
	if(err)
		{
		iConsole.Printf(_L("Failed to find L2CAP protocol. ERROR %d\n"), err);
		}

	iSocketServer.StopProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);

	err = iSocketServer.FindProtocol(_L("RFCOMM"), iProtocolInfo);
	if(err)
		{
		iConsole.Printf(_L("Failed to find RFCOMM protocol. ERROR %d\n"), err);
		}

	iSocketServer.StopProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);
	}

void CEirApp::ShowAddress()
/**
	Shows the example of using a synchronous Get to retrieve local BTAddr

*/
	{
	RProperty property;
	TBuf8<6> addr;
	TRequestStatus status;

	iConsole.Printf(_L("Waiting for Local BDADDR...\n"));
	// Wait for the address to be set
	TInt err = property.Attach(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress);
	
	if(err)
		{
		iConsole.Printf(_L("P&S: ERROR attaching to local address\n"));
		return;
		}

	RTimer addrTimer;
	err = addrTimer.CreateLocal();
	if(err != KErrNone)
		{
		iConsole.Printf(_L("Error creating P&S timer (%d)\n"), err);
		property.Close();
		return;
		}
	TRequestStatus timerStatus;
	
	property.Subscribe(status);
	addrTimer.After(timerStatus, 5000000); // 5 seconds.
	User::WaitForRequest(status, timerStatus);
	
	addrTimer.Close(); // done with timer now.

	err = property.Get(addr);

	if (err)
		{	
		iConsole.Printf(_L("P&S: ERROR getting local address\n"));
		}
	else
		{
		iConsole.Printf(_L("Local Address: %02X:%02X:%02X:%02X:%02X:%02X\n"), addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
		}
	
	// Set inquiry scan
	RProperty discprop;
	err=discprop.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan);
	if (err)
		{	
		iConsole.Printf(_L("P&S: ERROR setting discoverability\n"));
		}
	else
		{
		iConsole.Printf(_L("Device now in discoverable mode\n"));
		}
	property.Close();

	User::WaitForRequest(status, timerStatus);
	}





// ----------------------------------------------------------------------------
//							Active Host Resolver
// ----------------------------------------------------------------------------

CActiveHostResolver::CActiveHostResolver(CConsoleBase& aConsole, CEirApp& aApp)
: CActive(EPriorityStandard), iConsole(aConsole), iApp(aApp)
	{
	//++iHRNumber;
	CActiveScheduler::Add(this);
	}

CActiveHostResolver::~CActiveHostResolver()
	{
//	if (iHostResolver.SubSessionHandle())
//		iHostResolver.Cancel();
	Cancel();
	iHostResolver.Close();
#if USE_SMALL_CONSOLE
	delete iHROutputConsole;
#endif
	}

void CActiveHostResolver::StartServerL()
/** 
  Connect to socket server and get BTLinkManager protocol info
 **/
	{
	iSocketServer = iApp.SocketServ();
	}

void CActiveHostResolver::StartHostResolverL()
/** 
	In future have a bunch of host resolvers
*/
	{
	iConsole.Printf(_L("Opening Host Resolver ..\n"));

        User::LeaveIfError(iSocketServer.FindProtocol(_L("BTLinkManager"), iProtocolInfo));
	User::LeaveIfError(iHostResolver.Open(iSocketServer,
											iProtocolInfo.iAddrFamily,
											iProtocolInfo.iProtocol));
//#if 0
	// show Localname for free
	TBTDeviceName localName;
	iHostResolver.GetHostName(localName);
	iConsole.Printf(_L("Local hostname is %S\n"), &localName);					
//#endif
	}

TInt CActiveHostResolver::RunError(TInt /*aError*/)
	{
	__DEBUGGER();
	return KErrNone;
	}

void CActiveHostResolver::SetLocalNameL()
	{
	iConsole.Printf(_L("Enter local device name (esc to finish)"));

	const TUint8 KMaxLength = 16;
	TBuf<KMaxLength> localName;
	localName.Zero();
	FOREVER
		{
		TChar c = test.Getch();
		if (c == EKeyEscape)
			break;
		else
			{
			TBuf<1> buf;
			buf.Append(c);
			iConsole.Printf(buf);
			localName.Append(c);
			}

		if(localName.Length() >= KMaxLength)
			// User entered too many characters...
			{
			// ...get them to start again
			iConsole.Printf(_L("\nToo many chars!\n"));
			iConsole.Printf(_L("(max %d chars) <ESC> to finish: "), KMaxLength);
			localName.Zero();
			}
		} // END - FOREVER loop

	// If the name is OK, send it to the hardware... 
	if (localName.Length()>0)
		{
		User::LeaveIfError(iHostResolver.SetHostName(localName));
		}
	else
		{
		User::LeaveIfError(KErrGeneral);
		}
	}

void CActiveHostResolver::SetIAC(TUint aIAC)
	{
	iInquiryRequest.SetIAC(aIAC);
	}


void CActiveHostResolver::DoCancel()
	{
	iHROutputConsole->Printf(_L("Cancelled.\n"));
	iHostResolver.Cancel();
	}

void CActiveHostResolver::ConstructL()
	{
#if USE_SMALL_CONSOLE
	iHROutputConsole = Console::NewL(_L("Host Resolver output"),gSmallConsole);
#else
	iHROutputConsole = &iConsole;
#endif

	StartServerL();
	StartHostResolverL();
	}

// ----------------------------------------------------------------------------
//							Active Inquirer
// ----------------------------------------------------------------------------

CActiveInquirer::CActiveInquirer(CConsoleBase& aConsole, CEirApp& aApp, TBool aDoNames, TBool aIgnoreCache, TBool aDoEir, TBool forever)
: CActiveHostResolver(aConsole, aApp), iDoNames(aDoNames), iIgnoreCache(aIgnoreCache), iDoEir(aDoEir), iForever(forever)
	{
	}

CActiveInquirer* CActiveInquirer::NewL(CConsoleBase& aConsole, CEirApp& aApp, TUint aIAC, TBool aDoNames, TBool aIgnoreCache, TBool aDoEir, TBool forever)
	{
	CActiveInquirer* inquirer = new (ELeave) CActiveInquirer(aConsole, aApp, aDoNames, aIgnoreCache, aDoEir, forever);
	CleanupStack::PushL(inquirer);
	inquirer->ConstructL(aIAC);
	CleanupStack::Pop(inquirer);
	return inquirer;
	}

void CActiveInquirer::ConstructL(TUint aIAC)
	{
	CActiveHostResolver::ConstructL();

//	SetLocalNameL();
	TUint action = KHostResInquiry;

	if (iDoNames)
		action |= KHostResName;

	if (iIgnoreCache)
		action |= KHostResIgnoreCache;

	if (iDoEir)
		action |= KHostResEir;

	iHROutputConsole->Printf(_L("Action: 0x%x\n"), action); 
	iInquiryRequest.SetAction(action);
	iInquiryRequest.SetIAC(aIAC);

	Start();
	}

void CActiveInquirer::Start()
/**
	Start an Inquiry
**/
	{
	TUint currentIAC;
	currentIAC = iInquiryRequest.IAC();

	iHROutputConsole->Printf(_L("Inquiring [IAC 0x%06x]\n"), currentIAC);

	iHostResolver.GetByAddress(iInquiryRequest, iResult, iStatus); // async
	SetActive();
	}

void CActiveInquirer::RunL()
	{
	TReal passLoss = 0;
	TReal estimatedDistance = 0;
	if (iStatus.Int() == KErrNone)
		{
		TInquirySockAddr& sa = TInquirySockAddr::Cast(iResult().iAddr);
		const TBTDevAddr& bdaddr = sa.BTAddr();
		
		iApp.DeviceFound(bdaddr); // notify app

		TBTDevAddrBuf bdaddrbuf;
		bdaddr.GetReadable(bdaddrbuf);

		iHROutputConsole->Printf(_L("Result: BdAddr = 0x%S"),&bdaddrbuf);

		if(sa.ResultFlags() & TInquirySockAddr::ERssiValid) 
			{
			iHROutputConsole->Printf(_L(" RSSI: %d ["), sa.Rssi());
			// store the rssi value in here
			passLoss = sa.Rssi();
#define KRssiBarNum 8
#define KRssiRange (127 + 20 + 1) 
			TUint rssiBars = ((((TInt) sa.Rssi()) + 127 + 1) * KRssiBarNum) / KRssiRange; 
			TUint i;
		
			for(i=1; i <= KRssiBarNum ; i++)
				{
				if(rssiBars >= i)
					{
					iHROutputConsole->Printf(_L("*")); 
					}
				else
					{
					iHROutputConsole->Printf(_L(" ")); 
					}
				}
			iHROutputConsole->Printf(_L("]\n")); 
			}
		else
			{
			iHROutputConsole->Printf(_L("\n")); 
			}

		if(iDoEir)
			{
			TBluetoothNameRecordWrapper eir(iResult());
			//iHROutputConsole->Printf(_L("EIR Length: %d\n"), eir.Length()); 
			TBool isNameComplete;
			TInt error = KErrNone;
			TInt length = 0;
			
			
			// Go and get name
			// This length could be used to create the TBuf to be passed into GetDeviceName()
			length = eir.GetDeviceNameLength();
			TBuf<255> name;
			if(length >= 0)
				{
				// name will contain a Unicode encoded 16-bit string
				error = eir.GetDeviceName(name, isNameComplete);
				}
			else
				{
				error = length;
				}
			if(error == KErrNone)
				// Now we have name here
				{
				if(isNameComplete == EFalse)
					{
					iHROutputConsole->Printf(_L("%d Bytes [Partial] Name: "), length);
					}
				else
					{
					iHROutputConsole->Printf(_L("%d Bytes [Complete] Name: "), length);
					}
				iHROutputConsole->Printf(_L("%S \n"),&name);
				}
			
			// Get TxPowerLevel
			TInt8 txPowerLevel;
			error = eir.GetTxPowerLevel(txPowerLevel);
			if(error == KErrNone)
				// TxPowerLevel present
				{
				iHROutputConsole->Printf(_L("TxPowerLevel: %ddBm\n"), txPowerLevel);
				// NICE one, we can calculate the pass loss here then :)
				if(passLoss != 0)
					{
					passLoss = txPowerLevel - passLoss;
					iHROutputConsole->Printf(_L("Pass Loss: %fdBm\n"), passLoss);
					// Here comes the magic equation:
					TReal power = (passLoss / 20.0) - 2.00221;
					TReal src = 10.0;
					Math::Pow(estimatedDistance, src, power);
					iHROutputConsole->Printf(_L("Estimated distance: %fm\n"), estimatedDistance);
					}
				}
			// Get UUIDs
			RExtendedInquiryResponseUUIDContainer uuidContainer;
			error = eir.GetServiceClassUuids(uuidContainer);
			if(error >= KErrNone)
				{
				RArray<TUUID> uuids;
				TInt uuidCount = uuidContainer.UUIDs().Count();
				if(uuidCount > 0)
					{
					iHROutputConsole->Printf(_L("*** UUID Count: %d\n"), uuidCount);
					TInt i;
					for(i=0;i<uuidCount;i++)
						{
						//iHROutputConsole->Printf(_L("*** UUID #%d minsz: %d value: 0x"), i, uuidContainer.UUIDs()[i].MinimumSize());
						TInt j;
						TPtrC8 uuid(uuidContainer.UUIDs()[i].ShortestForm());
						// Treat it as a big endian
						for(j=0;j<uuid.Length();j++)
							{
							iHROutputConsole->Printf(_L("%02X"), uuid[j]);
							}
						
						iHROutputConsole->Printf(_L(" \n"));
						}
					}
				}
			
			// Get Manufacturer Specific Data
			length = eir.GetVendorSpecificDataLength();
			TBuf8<255> msd;
			if(length > 0)
				{
				error = eir.GetVendorSpecificData(msd);
				}
			else
				{
				error = length;
				}
			if(error == KErrNone)
				// we have Manufacturer Specific Data here
				{
				// This conversion is for display reason, in a real world this may not be necessary as 
				// Manufacturer specific data can be just raw 8-bit data, however GetDeviceName() is
				// different as it always return Unicode encoded 16-bit string
				error = CnvUtfConverter::ConvertToUnicodeFromUtf8(name, msd);
				if(error >= KErrNone && length > 0)
					{
					iHROutputConsole->Printf(_L("%d Bytes Manufacturer Specific Data: %S\n"), length, &name);
					}
				}
			}

		if (iDoNames)
			{
			if(!iDoEir)
				{
				TBuf<255> namebuf;
				namebuf.Copy(iResult().iName);
				iHROutputConsole->Printf(_L("Remote name \"%S\""), &namebuf);
				if(iResult().iFlags & TNameRecord::EPartial)
						{
						iHROutputConsole->Printf(_L(" [PARTIAL]\n"));
						}
					else
						{
						iHROutputConsole->Printf(_L(" [COMPLETE]\n"));
						}
				}
			else
				{
				TBluetoothNameRecordWrapper eir(iResult());
				TInt length = eir.GetDeviceNameLength();
				TInt error = KErrNone;
				TBuf<255> name;
				TBool isNameComplete;

				if(length >= 0)
					{
					// name will contain a Unicode encoded 16-bit string
					error = eir.GetDeviceName(name, isNameComplete);
					}
				else
					{
					error = length;
					}
				if(error == KErrNone)
					// Now we have name here
					{
					if(isNameComplete == EFalse)
						{
						iHROutputConsole->Printf(_L("%d Bytes [Partial] Name (via EIR wrapper): "), length);
						}
					else
						{
						iHROutputConsole->Printf(_L("%d Bytes [Complete] Name (via EIR wrapper): "), length);
						}
					iHROutputConsole->Printf(_L("%S \n"),&name);
					}
				}
			}

		iHostResolver.Next(iResult, iStatus); // asynch
		SetActive();
		}
	else if (iStatus.Int() == KErrEof)
		{
		iHROutputConsole->Printf(_L("Finished inquiry.\n"));

		if(iForever)
			{
			iHROutputConsole->Printf(_L("Restarting inquiry.\n"));
			Start();
			}
		else
			{
			iApp.InquiryComplete();
			}
		}
	else
		{
		iHROutputConsole->Printf(_L("Inquiry request failed with result %d.\n"), iStatus.Int());
		}
	}


// ----------------------------------------------------------------------------
//							Active Name Requester
// ----------------------------------------------------------------------------

void CActiveNameRequester::ConstructL()
	{
	CActiveHostResolver::ConstructL();

	TUint action = iEirInstead ? KHostResEir : KHostResName;
	if (iIgnoreCache)
		action |= KHostResIgnoreCache;
	iInquiryRequest.SetAction(action);
	Start();
	}

CActiveNameRequester::CActiveNameRequester(CConsoleBase& aConsole, CEirApp& aApp, TBool aEirInstead, TBool aIgnoreCache)
: CActiveHostResolver(aConsole, aApp), iEirInstead(aEirInstead), iIgnoreCache(aIgnoreCache)
	{
	}

CActiveNameRequester* CActiveNameRequester::NewL(CConsoleBase& aConsole, CEirApp& aApp, TBool aEirInstead, TBool aIgnoreCache)
	{
	CActiveNameRequester* namerequester = new (ELeave) CActiveNameRequester(aConsole, aApp, aEirInstead, aIgnoreCache);
	CleanupStack::PushL(namerequester);
	namerequester->ConstructL();
	CleanupStack::Pop();
	return namerequester;
	}


void CActiveNameRequester::Start()
/**
	Get just the name
**/
	{
	iInquiryRequest.SetBTAddr(gDevAddr); // this object does the global addr for the moment

	TBTDevAddrBuf bdaddrbuf;
	gDevAddr.GetReadable(bdaddrbuf);

	iHROutputConsole->Printf(_L("Looking up address %S...\r\n"),&bdaddrbuf);

	iHostResolver.GetByAddress(iInquiryRequest, iResult, iStatus); // async
	SetActive();
	}


void CActiveNameRequester::PrintResults()
	{
	TInquirySockAddr& sa = TInquirySockAddr::Cast(iResult().iAddr);
	const TBTDevAddr& bdaddr = sa.BTAddr();

	TBTDevAddrBuf bdaddrbuf;
	bdaddr.GetReadable(bdaddrbuf);

	iHROutputConsole->Printf(_L("Result: BdAddr = 0x%S"),&bdaddrbuf);

	if(sa.ResultFlags() & TInquirySockAddr::ERssiValid) 
		{
		iHROutputConsole->Printf(_L(" RSSI: %d"), sa.Rssi()); 
		}

	iHROutputConsole->Printf(_L("\n")); 

	TBuf<255> buf;
	TBuf8<255> buf8;
	TBool isNameComplete = EFalse;
	TInt error = KErrNone;

	if(iEirInstead)
		{
		TBluetoothNameRecordWrapper eir(iResult());

		iHROutputConsole->Printf(_L("*** EIR START ***\n"));
		
		error = eir.GetDeviceName(buf, isNameComplete);
		if(error == KErrNone)
			{
			if(isNameComplete == EFalse)
				{
				iHROutputConsole->Printf(_L("Partial Name: %S"), &buf);
				}
			else
				{
				iHROutputConsole->Printf(_L("Partial Name: %S"), &buf);
				}
			}
			
		if(eir.GetVendorSpecificDataLength() > 0)
			{
			eir.GetVendorSpecificData(buf8);
			if(buf.Size() > 0)
				{
				iHROutputConsole->Printf(_L("Manufacturer Specific Data: %S"), &buf8);
				}
			}
			
		TInt8 txPowerLevel;
		TInt err = txPowerLevel = eir.GetTxPowerLevel(txPowerLevel);
		if(err == KErrNone)
			{
			iHROutputConsole->Printf(_L("Tx Power Level: %d"), &txPowerLevel);
			}

		iHROutputConsole->Printf(_L("*** EIR END ***\n"));
		// Test EIR methods
		RArray<TUUID> uuids;
		
		TInt error = eir.GetDeviceName(buf, isNameComplete);
		iHROutputConsole->Printf(_L("*** GetLocalName returned: %d\n"), error);
		if(error >= KErrNone)
			{
			iHROutputConsole->Printf(_L("*** Retrieved Local Name: %S\n"),&buf);
			}
		
		RExtendedInquiryResponseUUIDContainer uuidContainer;
		error = eir.GetServiceClassUuids(uuidContainer);
		iHROutputConsole->Printf(_L("*** GetUUIDs returned: %d\n"), error);
		if(error >= KErrNone)
			{
			iHROutputConsole->Printf(_L("*** UUID Count: %d\n"), uuids.Count());
			TInt i;
			for(i=0;i<uuids.Count();i++)
				{
				iHROutputConsole->Printf(_L("*** UUID #%d minsz: %d value: 0x"), i, uuids[i].MinimumSize());
				TInt j;
				TPtrC8 uuid(uuids[i].ShortestForm());
				for(j=0;j<uuid.Length();j++)
					{
					iHROutputConsole->Printf(_L("%02X"), uuid[j]);
					}
				
				iHROutputConsole->Printf(_L(" \n"));
				}
			}

		}
	else
		{
		if (iResult().iName != KNullDesC)
			buf.Copy(iResult().iName);
		else
			buf = _L("NOT AVAILABLE");

		iHROutputConsole->Printf(_L("Result: Remote Name = \"%S\""),&buf);
		if(iResult().iFlags & TNameRecord::EPartial)
				{
				iHROutputConsole->Printf(_L(" [PARTIAL]\n"));
				}
			else
				{
				iHROutputConsole->Printf(_L(" [COMPLETE]\n"));
				}

		}

	TInt majorCoD = sa.MajorClassOfDevice();
	if (majorCoD & (1 << 13))
		iHROutputConsole->Printf(_L("Device is Limited Discoverable\n"));
	}

void CActiveNameRequester::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		PrintResults();
		}
	else
		{
		iHROutputConsole->Printf(_L("GetByAddress failed with code %d\r\n"), iStatus.Int());
		}
	}

// ----------------------------------------------------------------------------
//							Active Name Array Requester
// ----------------------------------------------------------------------------


CActiveNameArrayRequester* CActiveNameArrayRequester::NewL(CConsoleBase& aConsole, CEirApp& aApp, RArray<TBTDevAddr>& aNameArray)
	{
	CActiveNameArrayRequester* namesreq = new (ELeave) CActiveNameArrayRequester(aConsole, aApp, aNameArray);
	CleanupStack::PushL(namesreq);
	namesreq->ConstructL();
	CleanupStack::Pop();
	return namesreq;
	}

CActiveNameArrayRequester::CActiveNameArrayRequester(CConsoleBase& aConsole, CEirApp& aApp, RArray<TBTDevAddr>& aNameArray)
: CActiveNameRequester(aConsole, aApp, EFalse, EFalse), iNameArray(aNameArray)
	{
	}

void CActiveNameArrayRequester::RunL()
	{
	if (iStatus.Int() == KErrNone || iStatus.Int() == KHCIErrorBase-EPageTimedOut)
		{
		PrintResults();
		// do next one if present
		if (iCurrentNameIndex < iNameArray.Count()-1)
			{
			++iCurrentNameIndex;
			DoSingleNameRequest();
			}
		}
	else
		{
		iHROutputConsole->Printf(_L("GetByAddress failed with code %d\n"), iStatus.Int());
		}
	}

void CActiveNameArrayRequester::ConstructL()
	{
	CActiveHostResolver::ConstructL();

	iInquiryRequest.SetAction(KHostResName);			// NAME ONLY
	iCurrentNameIndex = 0;
	Start();
	}

void CActiveNameArrayRequester::DoSingleNameRequest()
// FIXME make a member of NameRequester and sort out address ownsership
	{
	TBTDevAddr& currentAddr = iNameArray[iCurrentNameIndex];
	iInquiryRequest.SetBTAddr(currentAddr); // this object does the global addr for the moment

	TBTDevAddrBuf bdaddrbuf;
	currentAddr.GetReadable(bdaddrbuf);

	iHROutputConsole->Printf(_L("Looking up address %S...\n"),&bdaddrbuf);

	iHostResolver.GetByAddress(iInquiryRequest, iResult, iStatus); // async
	SetActive();
	}

void CActiveNameArrayRequester::PrintResults()
	{
	TInquirySockAddr& sa = TInquirySockAddr::Cast(iResult().iAddr);
	const TBTDevAddr& bdaddr = sa.BTAddr();

	TBTDevAddrBuf bdaddrbuf;
	bdaddr.GetReadable(bdaddrbuf);

	TBuf<255> buf;

	if (iResult().iName != KNullDesC)
		buf.Copy(iResult().iName);
	else
		buf = _L("NOT AVAILABLE");

	iHROutputConsole->Printf(_L("Result [HostResolver 0x%08x, Bdaddr %S]: Name: %S \n"),
							&iHostResolver, &bdaddrbuf, &buf);

	}

void CActiveNameArrayRequester::Start()
	{
	DoSingleNameRequest();
	}

// ----------------------------------------------------------------------------
//							Active Console
// ----------------------------------------------------------------------------


CActiveConsole::CActiveConsole(CConsoleBase& aConsole, CEirApp& aApp)
: CActive(EPriorityStandard), iConsole(aConsole), iApp(aApp)
	{
	}


CActiveConsole::~CActiveConsole()
	{
	// must cancel
	Cancel();
	}

CActiveConsole* CActiveConsole::NewL(CConsoleBase& aConsole, CEirApp& aApp)
	{
	CActiveConsole* console = new (ELeave) CActiveConsole(aConsole, aApp);
	CleanupStack::PushL(console);
	console->ConstructL();
	CleanupStack::Pop();
	return console;
	}

void CActiveConsole::ConstructL()
	{
	CActiveScheduler::Add(this);
	TestMenu();
	RequestKey();
	}

void CActiveConsole::DoCancel()
	{
	iConsole.ReadCancel();
	}

void CActiveConsole::RequestKey()
	{
	DrawCursor();
	iConsole.Read(iStatus);
	SetActive();
	}

void CActiveConsole::DrawCursor()
	{
	iConsole.Printf(KNullDesC());
	}

void CActiveConsole::TestMenu()
	{
	iConsole.Printf(_L("Choose action...\n\n"));
	iConsole.Printf(_L("n.\t\tSingle Name Request\n"));
	iConsole.Printf(_L("N.\t\tSingle Name Request; ignore Cache\n"));

	iConsole.Printf(_L("e.\t\tSingle EIR Request\n"));
	iConsole.Printf(_L("E.\t\tSingle EIR Request; ignore Cache\n"));

	iConsole.Printf(_L("i.\t\tGeneral Inquiry no Names\n"));
	iConsole.Printf(_L("I.\t\tGeneral Inquiry no Names; ignore Cache\n"));

	iConsole.Printf(_L("w.\t\tGeneral Inquiry with Names\n"));
	iConsole.Printf(_L("W.\t\tGeneral Inquiry with Names; ignore Cache\n"));

	iConsole.Printf(_L("x.\t\tGeneral Inquiry with EIR\n"));
	iConsole.Printf(_L("X.\t\tGeneral Inquiry with EIR; ignore Cache\n"));

	iConsole.Printf(_L("c.\t\tCancel Inquiry\n"));

	iConsole.Printf(_L("f.\t\tForever: general Inquiry with EIR; ignore Cache\n"));

	iConsole.Printf(_L("------------------------------------\n"));

	iConsole.Printf(_L("1.\t\tAdd EIR Publisher: Local Name\n"));
	iConsole.Printf(_L("2.\t\tAdd new EIR Publisher: UUID 16\n"));
	iConsole.Printf(_L("3.\t\tAdd new EIR Publisher: UUID 128\n"));
	iConsole.Printf(_L("4.\t\tAdd EIR Publisher: Manufacturer Specific\n"));

	iConsole.Printf(_L("Esc.\tStop\n"));
	}

void CActiveConsole::RunL()
	{
	// key has been pressed
	TChar ch = iConsole.KeyCode();
	TBool requestNextKey = ProcessKey(ch);
	if ( requestNextKey )
		{
		RequestKey();
		}
	}
	
TBool CActiveConsole::ProcessKey(TChar aCh)
	{
	TBool requestNextKey = ETrue;
	TInt err;
	switch (aCh)
		{
	case 'n':
		iConsole.Printf(_L("Starting name request...\n"));
		TRAP(err, iApp.NewNameRequesterL(EFalse, EFalse));
		if(err!=KErrNone)
			iConsole.Printf(_L("name request failed - %d"), err);
		break;

	case 'N':
		iConsole.Printf(_L("Starting name request (ignoring cache)...\n"));
		TRAP(err, iApp.NewNameRequesterL(EFalse, ETrue));
		if(err!=KErrNone)
			iConsole.Printf(_L("name request (ignoring cache) failed - %d"), err);
		break;

	case 'e':
		iConsole.Printf(_L("Starting EIR request...\n"));
		TRAP(err, iApp.NewNameRequesterL(ETrue, EFalse));
		if(err!=KErrNone)
			iConsole.Printf(_L("EIR request failed - %d"), err);
		break;

	case 'E':
		iConsole.Printf(_L("Starting EIR request (ignoring cache)...\n"));
		TRAP(err, iApp.NewNameRequesterL(ETrue, ETrue));
		if(err!=KErrNone)
			iConsole.Printf(_L("EIR request (ignoring cache) failed - %d"), err);
		break;

	case 'i':
		iConsole.Printf(_L("Starting General Inquiry no names...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, EFalse, EFalse, EFalse));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry failed - %d"), err);
		break;

	case 'I':
		iConsole.Printf(_L("Starting General Inquiry, no names, ignoring cache...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, EFalse, ETrue, EFalse));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry, no names, ignoring cache failed - %d"), err);
		break;


	case 'w':
		iConsole.Printf(_L("Starting General Inquiry with Names...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, ETrue, EFalse, EFalse));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry with Names failed - %d"), err);
		break;

	case 'W':
		iConsole.Printf(_L("Starting General Inquiry with names and no cache...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, ETrue, ETrue, EFalse));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry with names, ignoring cache failed - %d"), err);
		break;

	case 'x':
		iConsole.Printf(_L("Starting General Inquiry With EIR...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, EFalse, EFalse, ETrue));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry with EIR failed - %d"), err);
		break;

	case 'X':
		iConsole.Printf(_L("Starting General Inquiry With EIR, ignoring cache...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, EFalse, ETrue, ETrue));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry with EIR, ignoring cache failed - %d"), err);
		break;

	case 'a':
		iConsole.Printf(_L("Starting General Inquiry With both Name request and EIR...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, ETrue, EFalse, ETrue));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry with EIR failed - %d"), err);
		break;

	case 'A':
		iConsole.Printf(_L("Starting General Inquiry With both Name request and EIR, ignoring cache...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, ETrue, ETrue, ETrue));
		if(err!=KErrNone)
			iConsole.Printf(_L("General Inquiry with EIR, ignoring cache failed - %d"), err);
		break;

	case 'c':
		// fixme - would be nice to enumerate the one to cancel
		// and see what happens to the others...
		iConsole.Printf(_L("Cancelling inquiry...\n"));
		iApp.CancelInquirer();
		break;

	case 'f':
		iConsole.Printf(_L("Neverending inquiry...\n"));
		TRAP(err, iApp.NewInquirerL(KGIAC, EFalse, ETrue, ETrue, ETrue));
		break;

	case '1':
/*		iConsole.Printf(_L("Change Local Name...\n"));
		TRAP(err, iApp.NewEirPublisherTestL(ETestDeviceName));
		if(err!=KErrNone)
			iConsole.Printf(_L("failed - %d"), err);
*/
		iConsole.Printf(_L("to be supported in teir"));
		break;

	case '2':
		iConsole.Printf(_L("Creating an EIR UUID16 publisher...\n"));
		TRAP(err, iApp.NewEirPublisherTestL(ETestUuid16));
		if(err!=KErrNone)
			iConsole.Printf(_L("failed - %d"), err);
		break;

	case '3':
		iConsole.Printf(_L("Creating an EIR UUID128 publisher...\n"));
		TRAP(err, iApp.NewEirPublisherTestL(ETestUuid128));
		if(err!=KErrNone)
			iConsole.Printf(_L("failed - %d"), err);
		break;

	case '4':
		iConsole.Printf(_L("Creating an EIR Manufacturer Specific publisher...\n"));
		TRAP(err, iApp.NewEirPublisherTestL(ETestSetMSD));
		if(err!=KErrNone)
			iConsole.Printf(_L("failed - %d"), err);
		break;

	case '?':
		TestMenu();
		break;

	case EKeyEscape:
		iApp.Stop(); // don't want another key press
		requestNextKey = EFalse;
		break;

	default:
		iConsole.Printf(_L("Unknown command\r\n"));
		}
	
	return requestNextKey;
	}

TInt CActiveConsole::SetHostName()
	{
	test.Printf(_L("Set Host Name to: "));
	TKeyCode key;
	TBuf16<255> buf;
	
	while((key = test.Getch())!=EKeyEnter && buf.Length()<buf.MaxLength())
		{
		if(key == EKeyBackspace&&buf.Length()!=0)
			{
			buf.SetLength(buf.Length()-1);
			}
		else 
			{
			buf.Append(key);
			}
		
		test.Printf(_L("%c"), key);
		}
		
//	iHostResolver.SetHostName(buf);
	test.Printf(_L("\n"));
	return KErrNone;
	}

CManufacturerDataNotifier* CManufacturerDataNotifier::NewL(CEirApp& aEirApp)
	{
	CManufacturerDataNotifier* self = new (ELeave) CManufacturerDataNotifier(aEirApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CManufacturerDataNotifier::CManufacturerDataNotifier(CEirApp& aEirApp)
: iEirApp(aEirApp)
	{
	}
	
CManufacturerDataNotifier::~CManufacturerDataNotifier()
	{
	}
	
void CManufacturerDataNotifier::ConstructL()
	{
	}
	
// From MManufacturerSpecificDataPublisherNotifier
void CManufacturerDataNotifier::MmsdpnNotifyAvailableDataSize(TUint aAvailableDataSize)
	{
	iEirApp.SetPublishManuData(aAvailableDataSize);
	}

void CManufacturerDataNotifier::MmsdpnNotifyError(TInt /*aResult*/)
	{
	// Check aResult == KErrNone
	}


void StartL()
	{
	CEirApp* app = CEirApp::NewL(*test.Console());
	CleanupStack::PushL(app);
	app->StartL();
	CleanupStack::PopAndDestroy(app);
	}

TInt E32Main()
	{
	TInt ret;
	test.SetLogged(EFalse);
	test.Title();

// For some reason, you have to do the following to
// ensure that the file server behaves properly.
	RFs fs;
	ret=fs.Connect();
	fs.Close();
// Run through tests

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
