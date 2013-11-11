// btservices.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32std.h>
#include <BTExtNotifiers.h>

#include <fshell/common.mmh>
#include <fshell/ioutils.h>
using namespace IoUtils;

#include "BtServicesEng.h"
#include "BtServiceView.h"

class CBTEngSettings;

#if FSHELL_PLATFORM_S60 >= 5
// This is an S60-only API
#define SUPPORT_ONOFF
#endif

#ifdef SUPPORT_ONOFF
#include <btengsettings.h>
#endif

CBtServiceView::CBtServiceView()
	{
	}

CBtServiceView::~CBtServiceView()
	{
	}

void CBtServiceView::PrintTextMsg(TInt /*aVerboseLevel*/, const TDesC& /*aMsg*/)
	{
	}

void CBtServiceView::AsyncCompleted()
	{
	}

class CCmdBtsvc : public CCommandBase, public CBtServiceView 
	{
public:
	static CCommandBase* NewLC();
	~CCmdBtsvc();
private:
	CCmdBtsvc();
	void DoScanDeviceL();
	void DoListServiceL();
	void DoAttributeL();
	void DoLocalL();
	void DoListUuidFilterL();
	void SelectBTDeviceL(TBTDeviceName& aName, TBTDevAddr& aBDAddr, TBTDeviceClass& aDevClass);
	TBool GetBTDevClassFromRegistryL(TBTDevAddr& aBDAddr, TBTDeviceClass& aDevClass);

#ifdef SUPPORT_ONOFF
	void SetEnabledL(TBool aEnable);
	void ShowStatusL();
	TBool GetPowerStateL();
#endif

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
private:
	virtual void PrintTextMsg(TInt aVerboseLevel, const TDesC& aMsg);
	virtual void AsyncCompleted();

private:
	CBtServicesEng* iEngine; 
	CActiveSchedulerWait* iActiveWait;
	RArray<TBool> iVerbose;
	
	HBufC* iOptBTAddr;
	HBufC* iOptSvcHandle;	//service handle
	HBufC* iOptUuidFilter;	//uuid filter
	
	enum TBtSvcCmd
		{
		ECmdDevice,
		ECmdService,
		ECmdAttribute,
		ECmdLocal,
		ECmdUuid,
		ECmdStatus,
		ECmdEnable,
		ECmdDisable,
		};
	
	TBtSvcCmd iCommand;

	CBTEngSettings* iSettings;
	};

CCommandBase* CCmdBtsvc::NewLC()
	{
	CCmdBtsvc* self = new(ELeave) CCmdBtsvc();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdBtsvc::CCmdBtsvc()
	{
	iActiveWait = new (ELeave) CActiveSchedulerWait;
	}


CCmdBtsvc::~CCmdBtsvc()
	{
#ifdef SUPPORT_ONOFF
	delete iSettings;
#endif

	delete iOptBTAddr;
	delete iOptSvcHandle;
	delete iOptUuidFilter;
	delete iActiveWait;
	delete iEngine;
	iVerbose.Close();
	}

void CCmdBtsvc::PrintTextMsg(TInt aVerboseLevel, const TDesC& aMsg)
	{
	TInt VerboseLevel = iVerbose.Count();
	if (aVerboseLevel <= VerboseLevel)
		Printf(aMsg);
	}

void CCmdBtsvc::AsyncCompleted()
	{
	if (iActiveWait->IsStarted())
		{
		iActiveWait->AsyncStop();
		}
	}

const TDesC& CCmdBtsvc::Name() const
	{
	_LIT(KName, "btservices");
	return KName;
	}

void CCmdBtsvc::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgCommand, "command");
	aArguments.AppendEnumL((TInt&)iCommand, KArgCommand);
	}

void CCmdBtsvc::OptionsL(RCommandOptionList& aOptions)
	{		
	aOptions.AppendStringL(iOptBTAddr, _L("address"));
	aOptions.AppendStringL(iOptSvcHandle, _L("record"));
	aOptions.AppendStringL(iOptUuidFilter, _L("uuid"));		
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	}

void CCmdBtsvc::DoRunL()
	{
	switch (iCommand)
		{
		case ECmdDevice:
			DoScanDeviceL();
			break;
		case ECmdService:
			DoListServiceL();
			break;
		case ECmdAttribute:
			DoAttributeL();
			break;
		case ECmdLocal:
			DoLocalL();
			break;
		case ECmdUuid:
			DoListUuidFilterL();
			break;
#ifdef SUPPORT_ONOFF
		case ECmdStatus:
			ShowStatusL();
			break;
		case ECmdEnable:
			SetEnabledL(ETrue);
			break;
		case ECmdDisable:
			SetEnabledL(EFalse);
			break;
#endif
		default:	
			LeaveIfErr(KErrArgument, _L("Invalid command"));
		}
	}

void CCmdBtsvc::DoAttributeL()
	{
	TInt err;
		
	//if want to query full attribute set of one particular service,
	//call CBtServicesEng::GetAttributesL()
	if (iOptSvcHandle && iOptSvcHandle->Length())
		{
		iEngine = CBtServicesEng::NewL();
		iEngine->SetView(this);
		
		iEngine->CancelSdpAgent();
		iEngine->DeleteSdpAgent();
		
		err = iEngine->BluetoothReady();
		LeaveIfErr(err, _L("Bluetooth not ready"));
				
		TBTDeviceName Name; 
		TBTDevAddr BDAddr;
		TBTDeviceClass DevClass;
			
		//iOptSvcHandle should begin with "0x" (hex)
		if ((iOptSvcHandle->Left(2))==_L("0x"))
			{
			TSdpServRecordHandle Handle;
			TLex lex(*iOptSvcHandle);
			lex.SkipAndMark(2);
			err = lex.Val(Handle, EHex);
			LeaveIfErr(err, _L("Service Handle:\"%S\" is not in valid format"), iOptSvcHandle);
		
			SelectBTDeviceL(Name, BDAddr, DevClass);
			Printf(_L("==============================================\r\n"));
			
			iEngine->GetDeviceAttributesL(BDAddr, Name, Handle);
			
			//wait for results
			iActiveWait->Start();
									
			}
		else
			LeaveIfErr(KErrArgument, _L("Service Handle:\"%S\" is not in valid format"), iOptSvcHandle);		
		}
	else
		LeaveIfErr(KErrArgument, _L("use -r to specify a service record handle"));
	}


//do a BT device scan with RHostResolver,
//and make a list of BT addresses, device name, and device class
//
void CCmdBtsvc::DoScanDeviceL()
	{
	TInt err;
	TInt ItemsFound = 0;
	TBTDevAddr BDAddr;
	TBTDeviceName Name;
	TBTDeviceClass DevClass;
	
	Printf(_L("Scanning for BT devices ...\r\n"));
		
	RSocketServ SktSrv;
	CleanupClosePushL(SktSrv);
	
	err = SktSrv.Connect();
	LeaveIfErr(err, _L("Could not connect to socket server"));
	
	RHostResolver Host;
	CleanupClosePushL(Host);
	RHostResolver HostForName;
	CleanupClosePushL(HostForName);
	
	TProtocolDesc ProtocolInfo;
	err = SktSrv.FindProtocol(_L("BTLinkManager"), ProtocolInfo);
	User::LeaveIfError(err);
	
	err = Host.Open(SktSrv, ProtocolInfo.iAddrFamily , ProtocolInfo.iProtocol);
	LeaveIfErr(err, _L("Could not open host resolver"));
	err = HostForName.Open(SktSrv, ProtocolInfo.iAddrFamily , ProtocolInfo.iProtocol);
	LeaveIfErr(err, _L("Could not open host resolver"));
	
	TInquirySockAddr Inq;	
	Inq.SetIAC(KGIAC);
	Inq.SetAction(KHostResInquiry/*|KHostResIgnoreCache*/);
	
	
	TNameEntry Result;
	err = Host.GetByAddress(Inq, Result);
	
	//this is for querying name
	TInquirySockAddr InqName;	
	InqName.SetAction(KHostResName);
	TNameEntry ResultName;
	
	Printf(_L("========================================================\r\n"));
	Printf(_L("BTAddr         | DeviceClass| Name\r\n"));
	Printf(_L("========================================================\r\n"));
	
	while(1)
		{
		if (err == KErrNone)
			{
			ItemsFound++;
			TInquirySockAddr& sa = TInquirySockAddr::Cast(Result().iAddr);
			const TBTDevAddr& bdaddr = sa.BTAddr();
			BDAddr = bdaddr;
			Name = Result().iName;
			
			TUint16 MajorServiceClass = sa.MajorServiceClass(); 
			DevClass = TBTDeviceClass(
					MajorServiceClass, sa.MajorClassOfDevice(), sa.MinorClassOfDevice() );
			
			TBuf<128> BTReadAddress;
			BDAddr.GetReadable(BTReadAddress);
			
			//query its name
			InqName.SetBTAddr(bdaddr);
			err = HostForName.GetByAddress(InqName, ResultName);			
			if (err==KErrNone)
				Name = ResultName().iName;
			else
				Name.Format(_L("Unable to get the name, err=%d"), err);
			
			Printf(_L("0x%S | 0x%08x | \"%S\" \r\n"), 
					&BTReadAddress, DevClass.DeviceClass(), &Name);
								
			err = Host.Next(Result);
			}
		else if (err == KErrEof)
			{
			break;
			}
		else
			LeaveIfErr(err, _L("Resolving address failed"));
		}
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	
	Printf(_L("%d devices found\r\n"),ItemsFound);
	}

//NOT TESTED YET, do not use
//aBDAddr:[in]
//aDevClass:[out]	
TBool CCmdBtsvc::GetBTDevClassFromRegistryL(TBTDevAddr& aBDAddr, TBTDeviceClass& aDevClass)
	{
	TInt err;
	
	TBuf<128> buf;
	aBDAddr.GetReadable(buf);
	Printf(_L("Query DeviceClass for BTAddr:0x%S\r\n"), &buf);
	
	RBTRegServ Srv;
	CleanupClosePushL(Srv);
	err = Srv.Connect();
	User::LeaveIfError(err);
	
	RBTRegistry Reg;
	CleanupClosePushL(Reg);
	err = Reg.Open(Srv);
	User::LeaveIfError(err);

	TBTNamelessDevice Dev;
	Dev.SetAddress(aBDAddr);
	
	TRequestStatus status;
	Reg.GetDevice(Dev, status);
	User::WaitForRequest(status);
	
	err = status.Int();
	User::LeaveIfError(err);
	
	if (err == KErrNone )
		{
		aDevClass = Dev.DeviceClass();		
		}
	
	CleanupStack::PopAndDestroy();	
	CleanupStack::PopAndDestroy();	
	
	return ETrue;
	}

//select a BT device either from RNotifier, 
//or using BT address string if it is specified
//aName: [out]
//aBDAddr: [out]
//aDevClass: [out]
void CCmdBtsvc::SelectBTDeviceL(TBTDeviceName& aName, TBTDevAddr& aBDAddr, TBTDeviceClass& aDevClass)
	{
	TInt err;
	if (iOptBTAddr && iOptBTAddr->Length()>0)
		//use RHostResolver to get name and device class
		{
		Printf(_L("Querying name for \"%S\"...\r\n"), iOptBTAddr);
			
		RSocketServ SktSrv;
		CleanupClosePushL(SktSrv);
		
		err = SktSrv.Connect();
		LeaveIfErr(err, _L("Could not connect to socket server"));
		
		RHostResolver Host;
		CleanupClosePushL(Host);
		
		TProtocolDesc ProtocolInfo;
		err = SktSrv.FindProtocol(_L("BTLinkManager"), ProtocolInfo);
		User::LeaveIfError(err);
		
		err = Host.Open(SktSrv, ProtocolInfo.iAddrFamily , ProtocolInfo.iProtocol);
		LeaveIfErr(err, _L("Could not open host resolver"));
		
		TBTDevAddr Addr;
		err = Addr.SetReadable(*iOptBTAddr);
		LeaveIfErr(err, _L("User-input BT address invalid"));
				
		TInquirySockAddr Inq;	
		Inq.SetAction(KHostResName);
		Inq.SetBTAddr(Addr);	
		
		TNameEntry Result;
		err = Host.GetByAddress(Inq, Result);
		if (err == KErrNone)
			{
			TInquirySockAddr& sa = TInquirySockAddr::Cast(Result().iAddr);
			const TBTDevAddr& bdaddr = sa.BTAddr();
			aBDAddr = bdaddr;
			aName = Result().iName;
						
			TUint16 MajorServiceClass = sa.MajorServiceClass(); 
			aDevClass = TBTDeviceClass(
					MajorServiceClass, sa.MajorClassOfDevice(), sa.MinorClassOfDevice() );			
			}
		else
			LeaveIfErr(err, _L("Resolving address failed"));		
		
		CleanupStack::PopAndDestroy();
		CleanupStack::PopAndDestroy();
		
		TBuf<128> AddressStr;
		aBDAddr.GetReadable(AddressStr);
		Printf(_L("Device \"%S\" BTAddr:0x%S\r\n"), 
				&aName, &AddressStr);		
		}
	else
		//use RNotifier to select a BT device
		{
		//about the search device, prompting user about curret status
		Printf(_L("Searching Bluetooth device through RNotifier,\r\nplease go to main UI screen to select a device...\r\n"));
		
		////////////////////////////////////////////
		RNotifier noti;
		User::LeaveIfError(noti.Connect());

		// 2. Start the device selection plug-in
		TBTDeviceSelectionParams selectionFilter;
		//TUUID targetServiceClass(0x2345);
		//selectionFilter.SetUUID(targetServiceClass);
		TBTDeviceSelectionParamsPckg pckg(selectionFilter);
		TBTDeviceResponseParams result;
		TBTDeviceResponseParamsPckg resultPckg(result);
		TRequestStatus status;
		noti.StartNotifierAndGetResponse(status, KDeviceSelectionNotifierUid, pckg, resultPckg);
		User::After(2000000);

		// 3. Extract device name if it was returned
		User::WaitForRequest(status);

		if (status.Int() == KErrNone)
		    {
		    if (resultPckg().IsValidDeviceName() &&
		    	resultPckg().IsValidBDAddr() &&
		    	resultPckg().IsValidDeviceClass()
		    	)
		        {
		        aName = (resultPckg().DeviceName());	        
		        aBDAddr = resultPckg().BDAddr();
		        aDevClass = resultPckg().DeviceClass();
		        
		        }
		    }
		    
		    
		// 4. Clean up
		noti.CancelNotifier(KDeviceSelectionNotifierUid);
		noti.Close();		
		
		TBuf<128> AddressStr;
		aBDAddr.GetReadable(AddressStr);
		Printf(_L("Device \"%S\" BTAddr:0x%S DeviceClass:0x%08X\r\n"), 
				&aName, &AddressStr, aDevClass.DeviceClass());		
		
		}	
	}


//search and make a list of results
void CCmdBtsvc::DoListServiceL()
	{
	TInt VerboseLevel = iVerbose.Count();
	
	if (VerboseLevel >= 2)
		Printf(_L("calling CBtServicesEng::NewL() ...\r\n"));
	
	iEngine = CBtServicesEng::NewL();
	
	
	if (VerboseLevel >= 2)
		Printf(_L("calling CBtServicesEng::SetView()... iEngine=0x%08x\r\n"), 
				iEngine);
	
	iEngine->SetView(this);
	
	if (VerboseLevel >= 2)
		Printf(_L("calling CBtServicesEng::CancelSdpAgent()...\r\n"));
	iEngine->CancelSdpAgent();

	if (VerboseLevel >= 2)
		Printf(_L("calling CBtServicesEng::DeleteSdpAgent()...\r\n"));	
	iEngine->DeleteSdpAgent();
	
	TInt err = iEngine->BluetoothReady();
	LeaveIfErr(err, _L("Bluetooth not ready"));

	TBTDeviceName Name; 
	TBTDevAddr BDAddr;
	TBTDeviceClass DevClass;
	
	SelectBTDeviceL(Name, BDAddr, DevClass);
	
	//see if user has specified a UUID filter
	if (iOptUuidFilter && iOptUuidFilter->Length())
		{
		TUint32 uuid;
		if ((iOptUuidFilter->Left(2))==_L("0x"))
			{
			TLex lex(*iOptUuidFilter);
			lex.SkipAndMark(2);
			err = lex.Val(uuid, EHex);
			if (err==KErrNone)
				{
				iEngine->SetUUIDFilterL(uuid);
				goto QueryService;
				}
			}
		
			//the UUID could be in string form.
			TBool Match = iEngine->StringToUUID(*iOptUuidFilter, uuid);
			if (Match==EFalse)
				{
				Printf(_L("UUID filter not recognised. Ignored\r\n"));
				}
		}
		
QueryService:	
	iEngine->NewDeviceSelectedL(BDAddr, Name);		
	iActiveWait->Start();
	
	iEngine->MakeTextServiceList();	
	
	//test code: just to test if attributes query is OK	
	//iEngine->GetAttributesL(0);
	//iActiveWait->Start();
	
	}

void CCmdBtsvc::DoLocalL()
	{
	TInt err;
	iEngine = CBtServicesEng::NewL();
	iEngine->SetView(this);
	
	iEngine->CancelSdpAgent();
	iEngine->DeleteSdpAgent();
	
	err = iEngine->BluetoothReady();
	LeaveIfErr(err, _L("Bluetooth not ready"));
	
	iEngine->DisplayLocalInfoL();
	
	}


//make a list of UUID filters, values and strings  
void CCmdBtsvc::DoListUuidFilterL()
	{
	TInt err;
	iEngine = CBtServicesEng::NewL();
	iEngine->SetView(this);
	
	iEngine->CancelSdpAgent();
	iEngine->DeleteSdpAgent();
	
	err = iEngine->BluetoothReady();
	LeaveIfErr(err, _L("Bluetooth not ready"));
	
	
	iEngine->MakeTextUUIDList();
	}

	
EXE_BOILER_PLATE(CCmdBtsvc)

#ifdef SUPPORT_ONOFF

TBool CCmdBtsvc::GetPowerStateL()
	{
	if (!iSettings)
		{
		TRAPL(iSettings = CBTEngSettings::NewL(NULL), _L("Couldn't construct CBTEngSettings"));
		}
	TBTPowerStateValue state;
	LeaveIfErr(iSettings->GetPowerState(state), _L("Couldn't read BT power state"));
	return state == EBTPowerOn;
	}

void CCmdBtsvc::ShowStatusL()
	{
	if (GetPowerStateL())
		{
		Printf(_L("Bluetooth is enabled.\r\n"));
		}
	else
		{
		Printf(_L("Bluetooth is disabled.\r\n"));
		}
	}

void CCmdBtsvc::SetEnabledL(TBool aEnable)
	{
	TBool on = GetPowerStateL();
	if (on && !aEnable)
		{
		LeaveIfErr(iSettings->SetPowerState(EBTPowerOff), _L("Couldn't set power state"));
		}
	else if (!on && aEnable)
		{
		LeaveIfErr(iSettings->SetPowerState(EBTPowerOn), _L("Couldn't set power state"));
		}
	}

#endif
