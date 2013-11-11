// BtServicesEng.cpp
// 
// Copyright (c) 2003 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

//#include <qbtselectdlg.h>

#include <e32debug.h>

#include <utf.h>
#include "BtData.h"
#include "btserviceseng.h"
#include <btmanclient.h>
//#include <MQikListBoxData.h>

const TInt KArrayIncrement = 5;
const TInt KMaxServiceNameLength = 256;
const TInt KDefaultLanguage = 0x0100;
const TInt KIndentationFactor = 3;

_LIT(KTextHexPrefix,"0x");

CBtServicesEng::CBtServicesEng()
	{
	iSdpAgent = NULL;
	iSearchPattern = NULL;
	iAttributeArray = NULL;
	
	iView = NULL;
	
	iAttributeMatchList = NULL;
	iAttributeBuilder = NULL;
	
	iUUIDFilter = (KL2CAP);
	}

CBtServicesEng::~CBtServicesEng()
	{
	delete iSdpAgent; // no need to Cancel separately
	delete iSearchPattern;
	if (iAttributeArray)
		{
		iAttributeArray->Reset();
		delete iAttributeArray;
		}
	iRecordArray.ResetAndDestroy();
	delete iAttributeMatchList;
	delete iAttributeBuilder;

	iSocketServ.Close();
	iUUIDs.Close();
	iUUIDsAsText.ResetAndDestroy();
	iUUIDsAsText.Close();

	iErrorCodes.Close();
	iErrorMessages.ResetAndDestroy();
	iErrorMessages.Close();
	}

CBtServicesEng* CBtServicesEng::NewL()
	{
	CBtServicesEng* self = new(ELeave) CBtServicesEng();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CBtServicesEng::ConstructL()
	{
	//iEikon = CEikonEnv::Static();

	iSearchPattern = CSdpSearchPattern::NewL();
	iAttributeArray = new(ELeave) CDesCArrayFlat(KArrayIncrement);
	iAttributeBuilder = CBtAttributeBuilder::NewL(*this);

	//read the available filter UUID and corresponding text
	TInt arrayCount = sizeof(UuidAsArray) / sizeof(TUUidPair);
	RDebug::Printf("CBtServicesEng::ConstructL() arrayCount= %d \r\n", 
			arrayCount);
	
	TInt i;
	for (i = 0; i < arrayCount; i++)
		{
		TUint32 uuid = UuidAsArray[i].uuid;
		iUUIDs.Append(TUUID(uuid));
		
		HBufC* textForm = HBufC::NewL(64);
		RDebug::Printf("i=%d HBufC* textForm = 0x%08x \r\n", 
				i, textForm);

		RDebug::Printf("i=%d HBufC* textForm = 0x%08x \r\n", 
				i, textForm);
	
		TPtrC UuidText = UuidAsArray[i].txt;
		RDebug::Print(_L("UuidText: %S \r\n"), &UuidText);

		*textForm = UuidText;
				
		iUUIDsAsText.Append(textForm);
		}

	//==================================================
	/*
	buf = iEikon->AllocReadResourceAsDes8LC(R_BLUETOOTH_ERROR_MESSAGES);
	res.SetBuffer(buf);
	arrayCount = res.ReadInt16();
	for (i = 0; i < arrayCount; i++)
		{
		TInt errorCode = res.ReadInt32();
		iErrorCodes.Append(errorCode);
		HBufC* message = res.ReadHBufCL();
		iErrorMessages.Append(message);
		}
	CleanupStack::PopAndDestroy(); // buf
	*/

	User::LeaveIfError(iSocketServ.Connect());
	}

void CBtServicesEng::ReportMessage(TInt aVerboseLevel, TRefByValue<const TDesC> aFmt, ...)
	{
	if (iView)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		iTextBuf.FormatList(aFmt, list);
		iView->PrintTextMsg(aVerboseLevel, iTextBuf);
		}
	}

//generate a text list of UUID value/string
void CBtServicesEng::MakeTextUUIDList()
	{
	TInt arrayCount = sizeof(UuidAsArray) / sizeof(TUUidPair);
	TInt i;
	ReportMessage(0, _L("========================\r\n"));	
	ReportMessage(0, _L("UUID Value | UUID STRING\r\n"));	
	ReportMessage(0, _L("========================\r\n"));	
	for (i = 0; i < arrayCount; i++)
		{
		TUint32 uuid = UuidAsArray[i].uuid;
		TPtrC string = UuidAsArray[i].txt;
		
		ReportMessage(0, _L("0x%08x | %S\r\n"), uuid, &string);
		}
	}

//This function should only be called after NewDeviceSelectedL() asynchrnously completes; 
//generate a text list of services found (handle and name)
//
void CBtServicesEng::MakeTextServiceList()
	{
	TInt i;
	
	TPtrC UuidStr = UUIDToString(iUUIDFilter);
	ReportMessage(0, _L("UUID filter 0x%08x %S\r\n"), iUUIDFilter, &UuidStr);			
	
	TInt HandlesCnt = iRecordArray.Count();
	
	if (HandlesCnt)
		{
		ReportMessage(0, _L("============================\r\n"));		
		ReportMessage(0, _L("ServiceHandle | Service Name\r\n"));		
		ReportMessage(0, _L("============================\r\n"));		
		}
	
	for (i=0; i<HandlesCnt; i++)
		{
		TSdpServRecordHandle Handle = iRecordArray[i]->iHandle;
		const TDesC& Name = iRecordArray[i]->iServiceName;
		
		ReportMessage(0, _L("0x%08x    | \"%S\"\r\n"), Handle, &Name);
		
		}
		
	}

//
void CBtServicesEng::NewDeviceSelectedL(const TBTDevAddr& aAddress,const TBTDeviceName& aDeviceName)
	{
	iDeviceName = aDeviceName;
	iDeviceAddress = aAddress;

	GetDeviceServicesL();
	}

/**
This starts the SDP agent searching for services.
Firstly, the previous (if any) SDP agent is deleted and a
new one constructed with the address of the new device. The
record filter is set based on the UUID filter settings.
The NextRecordRequestL() function begins the search, with the
results being received via the MSdpAgentNotifier functions.
*/
void CBtServicesEng::GetDeviceServicesL()
	{
	if (iSdpAgent)
		{
		DeleteSdpAgent();
		}
	iSdpAgent = CSdpAgent::NewL(*this, iDeviceAddress); // create new SDP Agent for given device address

	iSearchPattern->Reset();
	iSearchPattern->AddL(iUUIDFilter);
	iSdpAgent->SetRecordFilterL(*iSearchPattern);

	iRecordArray.ResetAndDestroy();
//	MQikListBoxModel& model = *this;
//	model.RemoveAllDataL();
	//iView->ListBox().HandleItemRemovalL();
	//iView->ListBox().DrawNow();

	/*
	TGulAlignment position(EHRightVCenter);
	_LIT(KMsgFindingServices, "Finding services");
	iView->GetEikonEnv()->BusyMsgL(KMsgFindingServices, position);
	iView->DimCancelButton(EFalse);
	iView->DimRefreshButton(ETrue);
	 */
	
	iSdpAgent->NextRecordRequestL();
	}

/**
The Sdp agent initially returns only record handles for each
service - this function then gets the name attribute for
each one.
*/
void CBtServicesEng::GetServiceNamesL()
	{
	if (iHandleCounter < iRecordArray.Count())
		{
		TSdpAttributeID attributeID = KSdpAttrIdOffsetServiceName + KDefaultLanguage; // only support default language for SDP
		iSdpAgent->AttributeRequestL(iRecordArray[iHandleCounter]->iHandle, 
				attributeID);
				
		ReportMessage(1, _L("Query service name for handle 0x%X...\r\n"), 
				iRecordArray[iHandleCounter]->iHandle );
		}
	else
		{
		iCurrentAttributeRequest = ENoOutstandingRequests;		
//		iView->GetEikonEnv()->BusyMsgCancel();
//		iView->DimCancelButton(ETrue);
//		iView->DimRefreshButton(EFalse);
		
		if (iView)
			iView->AsyncCompleted();
		}
	}

/**
Begins getting the full attribute list
for a given service. Because an MSdpElementBuilder
pointer is passed in AttributeRequestL(), the SDP agent
calls the builder functions with each result, rather than
AttributeRequestResult(). 
*/
void CBtServicesEng::GetAttributesL(TInt aIndex)
	{
	iCurrentAttributeRequest = EGettingAllAttributes;
	iServiceName.Set(iRecordArray[aIndex]->iServiceName);
	iAttributeArray->Reset();
	iAttributeBuilder->Reset();
	if (iAttributeMatchList)
		{
		delete iAttributeMatchList;
		iAttributeMatchList = NULL;
		}
	iAttributeMatchList = CSdpAttrIdMatchList::NewL();
	iAttributeMatchList->AddL(TAttrRange(0, KMaxTUint16)); // getting all attributes
	iAttributeBuilder->SetHandle(iRecordArray[aIndex]->iHandle);
	iSdpAgent->AttributeRequestL(iAttributeBuilder, iRecordArray[aIndex]->iHandle, *iAttributeMatchList);
	}

/**
this function is supposed to be called 
WITHOUT calling "NewDeviceSelectedL()"

Begins getting the full attribute list
for a given service. Because an MSdpElementBuilder
pointer is passed in AttributeRequestL(), the SDP agent
calls the builder functions with each result, rather than
AttributeRequestResult(). 
*/
void CBtServicesEng::GetDeviceAttributesL(const TBTDevAddr& aAddress,const TBTDeviceName& aDeviceName, TSdpServRecordHandle aHandle)
	{
	iDeviceName = aDeviceName;
	iDeviceAddress = aAddress;
	if (iSdpAgent)
		{
		DeleteSdpAgent();
		}
	iSdpAgent = CSdpAgent::NewL(*this, iDeviceAddress); // create new SDP Agent for given device address
		
	iCurrentAttributeRequest = EGettingAllAttributes;
	//iServiceName.Set((*iTextArray)[aIndex]);
	iAttributeArray->Reset();
	iAttributeBuilder->Reset();
	if (iAttributeMatchList)
		{
		delete iAttributeMatchList;
		iAttributeMatchList = NULL;
		}
	iAttributeMatchList = CSdpAttrIdMatchList::NewL();
	iAttributeMatchList->AddL(TAttrRange(0, KMaxTUint16)); // getting all attributes
	iAttributeBuilder->SetHandle(aHandle);
	iSdpAgent->AttributeRequestL(iAttributeBuilder, aHandle, *iAttributeMatchList);
	}


void CBtServicesEng::CancelSdpAgent()
	{
	if (iSdpAgent)
		{
		iSdpAgent->Cancel();
		}
	}

void CBtServicesEng::DeleteSdpAgent()
	{
	delete iSdpAgent;
	iSdpAgent = NULL;
	}

void CBtServicesEng::SetUUIDFilterL(TUint32 aUUID)
	{
	iUUIDFilter = aUUID;
	}

void CBtServicesEng::AppendUUIDText(TDes& aBuf, TUUID aUUID) const
	{
	if (aUUID == TUUID(0))
		{
		aUUID = TUUID(iUUIDFilter);
		}
	TInt i = 0;
	TInt UuidCount = iUUIDs.Count();
	for (i = 0; i < UuidCount; i++)
		{
		if (iUUIDs[i] == aUUID)
			{
			aBuf.Append(*(iUUIDsAsText[i]));
			return;
			}
		}
	_LIT(KTextUUIDHex,"UUID: 0x");
	aBuf.Append(KTextUUIDHex);
	
	const TPtrC8 ptr = aUUID.ShortestForm();
	TInt ShortestLength = ptr.Length();
	for (i = 0; i<ShortestLength ; i++) // get shortest form
		{
		aBuf.AppendNumFixedWidth(aUUID[i],EHex,2);
		}
	}

void CBtServicesEng::DisplayDeviceInfoL()
	{
//	iView->DisplayDeviceInfoDialogL(iDeviceName,iDeviceAddress,iDeviceClass);
	}

//display local BT address, device name and class info
//
void CBtServicesEng::DisplayLocalInfoL()
	{
	/*
	RSocket l2CapSock;
	TInt err = l2CapSock.Open(iSocketServ, KBTAddrFamily, KSockSeqPacket, KL2CAP);
	if (err != KErrNone)
		{
		_LIT(KErrorText,"Not supported");
		TGulAlignment position(EHRightVTop);
		iView->GetEikonEnv()->InfoMsgWithAlignmentAndDuration(position,KErrorText,KErrorMsgDuration);
		return;
		}
	CleanupClosePushL(l2CapSock);
	TRequestStatus stat;
	
	TBTDevAddrPckg addrbuf;
	l2CapSock.Ioctl(KHCILocalAddressIoctl, stat, &addrbuf, KSolBtHCI); // takes a TBTDevAddrPckg (TPckgBuf<TBTDevAddr>) as documented in 7.0 SDK
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());

	THCIDeviceClassBuf classbuf;
	l2CapSock.Ioctl(KHCIReadDeviceClassIoctl, stat, &classbuf, KSolBtHCI); // takes a THCIDeviceClassBuf (TPckgBuf<THCIDeviceClassIoctl>) as documented in 7.0 SDK
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	CleanupStack::PopAndDestroy(); // l2CapSock

	RHostResolver hr;
	User::LeaveIfError(hr.Open(iSocketServ, KBTAddrFamily, KBTLinkManager));
	CleanupClosePushL(hr);
	TBTDeviceName deviceName;
	User::LeaveIfError(hr.GetHostName(deviceName));
	CleanupStack::PopAndDestroy();

	TBTDevAddr& deviceAddress = addrbuf();
	TBTDeviceClass deviceClass(classbuf().iMajorServiceClass, classbuf().iMajorDeviceClass,classbuf().iMinorDeviceClass);
	*/

	RBTRegServ serv;
	RBTLocalDevice me;
	User::LeaveIfError(serv.Connect());
	CleanupClosePushL(serv);
	User::LeaveIfError(me.Open(serv));
	CleanupClosePushL(me);
	TBTLocalDevice dev;
	User::LeaveIfError(me.Get(dev));

	TBTDeviceName deviceName;
	deviceName.Copy(dev.DeviceName().Left(deviceName.MaxLength()));
	TBTDeviceClass deviceClass(dev.DeviceClass());
	// RBtLocalDevice doesn't seem to return the address
	//const TBTDevAddr& deviceAddress = dev.Address();
	TBTDevAddr deviceAddress;
	TPtr8 ptr(deviceAddress.Des());
	User::LeaveIfError(RProperty::Get(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, ptr));

	CleanupStack::PopAndDestroy(2, &serv);
	
	TBuf<64> ReadableAddr;
	deviceAddress.GetReadable(ReadableAddr);
	ReportMessage(0, _L("BTAddress:0x%S, Name:\"%S\", DeviceClass=0x%08x\r\n"), 
			&ReadableAddr, &deviceName, deviceClass.DeviceClass());

	/*
#if 0 //test code, not used	
	//query further about the services provided by this device
	RSdp sdp;
	sdp.Connect();
	CleanupClosePushL(sdp);
		
	RSdpDatabase sdpData;
	sdpData.Open(sdp);
	CleanupClosePushL(sdpData);				

	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
#endif	
	*/
	}


void CBtServicesEng::SetView(CBtServiceView* apView)
	{
	iView = apView;
	}
	

/*
CDesCArrayFlat* CBtServicesEng::GetTextArray() const
	{
	return iTextArray;
	}
*/

/**
Checks whether bluetooth is enabled by opening a host
resolver and immediately closing it again.
*/
TInt CBtServicesEng::BluetoothReady()
	{
	RHostResolver hr;
	TInt err = hr.Open(iSocketServ, KBTAddrFamily, KBTLinkManager); // just to check if bluetooth is enabled
	hr.Close();
	return err;
	}

TBool CBtServicesEng::GettingAllAttributes() const
	{
	return (iCurrentAttributeRequest == EGettingAllAttributes);
	}

void CBtServicesEng::AddAttributeLineL(const TDesC& aDes)
	{
	iAttributeArray->AppendL(aDes);
	}

void CBtServicesEng::AttributeRequestComplete(TSdpServRecordHandle  aHandle, TInt aError)
	{
	ReportMessage(1, _L("[AttributeRequestComplete for Handle:0x%X Err:%d]\r\n\r\n"), aHandle, aError);
	
	if(aError == KErrNone)
		{
		TRAP(aError, DoAttributeRequestCompleteL());
		}
	if(aError != KErrNone)
		{
		SdpCompletedWithError(aError);
		}
	}

void CBtServicesEng::DoAttributeRequestCompleteL()
	{
	if (iCurrentAttributeRequest == EGettingNamesOnly)
		{
		//there are a lot of handles that needs the name, so 
		//need to keep issue requests until all handles are processed,
		//so the whole asynchronous process is not finished yet
		iHandleCounter++;
		GetServiceNamesL();
		}
	else if (iCurrentAttributeRequest == EGettingAllAttributes)
		{ // all attributes have been got
		TInt LineCount = iAttributeArray->Count();
		
		for (TInt i=0; i<LineCount; i++)
			{
			const TDesC& LineString = (*iAttributeArray)[i];						
			ReportMessage(0, _L("%S\r\n"), &LineString);
			}
		
//		iView->DisplayAttributeDialogL(iAttributeArray, iDeviceName, iServiceName);
		iCurrentAttributeRequest = ENoOutstandingRequests;
		if (iView)
			iView->AsyncCompleted();
		}
	}

void CBtServicesEng::AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID /*aAttrID*/, CSdpAttrValue* aAttrValue)
	{
	ReportMessage(1, _L("[AttributeRequestResult for Handle:0x%X ]\r\n"), aHandle );

	TRAPD(err,DoAttributeRequestResultL(aAttrValue, aHandle)); // passing ownership
	if (err != KErrNone)
		{
		iSdpAgent->Cancel();
		}
	}
	
void CBtServicesEng::DoAttributeRequestResultL(CSdpAttrValue* aAttrValue, TSdpServRecordHandle aHandle)
	{
	CleanupStack::PushL(aAttrValue); // taken ownership
	if (iCurrentAttributeRequest == EGettingNamesOnly) 
		{		 
		TBuf16<KMaxServiceNameLength> unicode;
		CnvUtfConverter::ConvertToUnicodeFromUtf8(unicode,aAttrValue->Des());
		if (unicode[unicode.Length() - 1] == L'\0')
			{
			unicode.Delete(unicode.Length() - 1, 1); // remove NULL-termination
			}
		//iView->ListBox().HandleItemAdditionL();
		TBuf<64> string;
		string.Copy(unicode);
		
		//extra check to make sure Handle and Name are paired correctly
		if (iRecordArray[iHandleCounter]->iHandle != aHandle)
			User::Panic(_L("Handle not match"), KErrNone);
		
		iRecordArray[iHandleCounter]->iServiceName = string;
		
		ReportMessage(1, _L("%S \r\n"), &string);		
/*		
		MQikListBoxModel& model = *this;
		model.ModelBeginUpdateLC();
		MQikListBoxData* data = model.NewDataL(EDataNormal);
		data->AddTextL(unicode, EQikListBoxSlotText1);
		data->Close();
		model.ModelEndUpdateL();
*/		
		}

	CleanupStack::PopAndDestroy(aAttrValue);  
	}

void CBtServicesEng::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt /* aTotalRecordsCount */)
	{
	if ((aError == KErrNone) || (aError == KErrEof))
		{
		TRAPD(err, DoNextRecordRequestCompleteL(aHandle, aError));
		if (err != KErrNone)
			{
			aError = err;
			}
		}
	if (aError != KErrNone && aError != KErrEof)
		{
//		iView->GetEikonEnv()->BusyMsgCancel();
		SdpCompletedWithError(aError);
		}
	
	//if aError == KErrEof, that means no more records to read
	
	}

void CBtServicesEng::DoNextRecordRequestCompleteL(TSdpServRecordHandle aHandle, TInt aError)
	{
	if (aError == KErrNone)    
		{
		ReportMessage(1, _L("Record Handle found 0x%X \r\n"), aHandle );
		
		CServiceRecordItem* pItem = new CServiceRecordItem;
		pItem->iHandle = aHandle;		
		iRecordArray.Append(pItem);
		iSdpAgent->NextRecordRequestL();
		}
	else if (aError == KErrEof)
		//no more records, now query service name for each record found 
		{
		iHandleCounter = 0;
		iCurrentAttributeRequest = EGettingNamesOnly;
		
		//report status
		TInt RecordCnt = iRecordArray.Count();		
		ReportMessage(1, _L("%d records found, querying service name...\r\n"), RecordCnt);
		GetServiceNamesL();
		}
	}

void CBtServicesEng::SdpCompletedWithError(const TInt aError)
	{
	
	HBufC* buf = HBufC::NewLC(KMaxDescriptorLength);
	TPtr errorText = buf->Des();
	AppendErrorMessage(aError,errorText);
//	TGulAlignment position(EHRightVTop);
//	iView->GetEikonEnv()->InfoMsgWithAlignmentAndDuration(position,errorText,KErrorMsgDuration);
	ReportMessage(0, *buf);
	
	CleanupStack::PopAndDestroy();
	iSdpAgent->Cancel();
//	iView->DimRefreshButton(EFalse);
//	iView->DimCancelButton(ETrue);
	if (iView)
		iView->AsyncCompleted();
	
	}

void CBtServicesEng::AppendErrorMessage(const TInt aError, TDes& aBuf) const
	{
	for (TInt i = 0; i < iErrorCodes.Count(); i++)
		{
		if (iErrorCodes[i] == aError)
			{
			aBuf.Append(*(iErrorMessages[i]));
			return;
			}
		}
	_LIT(KErrorTextOther, "Error: ");
	aBuf.Append(KErrorTextOther);
	aBuf.AppendNum(aError);
	}

void CBtServicesEng::Panic(TBtPanicCode aPanicCode)
	{
	_LIT(KBtPanicText, "BTSU::Panic ");
	User::Panic(KBtPanicText, aPanicCode);
	}

void CBtServicesEng::ReleaseModel()
	{
	// Do nothing, list box does not own us
	}

//static
//search the Uuid value, and return a text string of its name
TPtrC CBtServicesEng::UUIDToString(TUint32 aUuidValue)
	{
	//read the available filter UUID and corresponding text
	TInt arrayCount = sizeof(UuidAsArray) / sizeof(TUUidPair);
	TInt i;
	for (i = 0; i < arrayCount; i++)
		{
		TUint32 uuid = UuidAsArray[i].uuid;
		if (uuid == aUuidValue)
			return (UuidAsArray[i].txt);
		}
	return _L("");
	}

//static
//search through the value, and return the UUID value if there is one match
//aUuidValue: [out] the found result
TBool CBtServicesEng::StringToUUID(TDesC& aString, TUint32& aUuidValue)
	{
	//read the available filter UUID and corresponding text
	TInt arrayCount = sizeof(UuidAsArray) / sizeof(TUUidPair);
	TInt i;
	for (i = 0; i < arrayCount; i++)
		{
		TUint32 uuid = UuidAsArray[i].uuid;
		if (aString == UuidAsArray[i].txt)
			{
			aUuidValue = uuid;
			return ETrue;
			}			
		}
	return EFalse;
	}


///////////////////////////////////////
// CBtAttributeBuilder functions
///////////////////////////////////////

CBtAttributeBuilder* CBtAttributeBuilder::NewL(CBtServicesEng& aEngine)
	{
	CBtAttributeBuilder* self = new(ELeave) CBtAttributeBuilder(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtAttributeBuilder::CBtAttributeBuilder(CBtServicesEng& aEngine)
: iEngine(aEngine)
	{
	}

void CBtAttributeBuilder::ConstructL()
	{
/*	
	HBufC8* buf = CEikonEnv::Static()->AllocReadResourceAsDes8LC(R_ATTR_ID_MNEMONICS);

	TResourceReader res;
	res.SetBuffer(buf);

	TInt arrayCount = res.ReadInt16();
	for (TInt i = 0; i < arrayCount; i++)
		{
		TUint attrID = res.ReadUint32();
		iAttrIDs.Append(attrID);
		HBufC* mnemonic = res.ReadHBufCL();
		iAttrMnemonics.Append(mnemonic);
		}
	CleanupStack::PopAndDestroy(); // buf
*/	
	}

CBtAttributeBuilder::~CBtAttributeBuilder()
	{
	iAttrIDs.Close();
	iAttrMnemonics.ResetAndDestroy();
	iAttrMnemonics.Close();
	}

void CBtAttributeBuilder::Reset()
	{
	iIndentation = 0;
	iCurrentLine.Zero();
	}

void CBtAttributeBuilder::SetHandle(TSdpServRecordHandle aHandle)
	{
	iHandle = aHandle;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildUnknownL(TUint8 aType, TUint8 /* aSizeDesc */, const TDesC8& /* aData */)
	{
	if (iStatus == EExpectingAttrID)
		{
		_LIT(KErrorText,"**Error: Unexpected UNKNOWN");
		iCurrentLine.Append(KErrorText);
		}
	else
		{
		_LIT(KTextUnknown,"UNKNOWN");
		iCurrentLine.Append(KTextUnknown);
		iCurrentLine.AppendNum((TInt) aType);
		}		
	LineFinishedL();
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildNilL()
	{
	if (iStatus == EExpectingAttrID)
		{
		_LIT(KErrorText,"**Error: Unexpected NIL");
		iCurrentLine.Append(KErrorText);
		}
	else
		{
		_LIT(KTextNULL,"NULL");
		iCurrentLine.Append(KTextNULL);
		}
	LineFinishedL();
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildUintL(const TDesC8& aUint)
	{

	if (iStatus == EExpectingAttrValue)
		{
		iCurrentLine.Append(KTextHexPrefix);
		for (TInt i = 0; i < aUint.Length(); i++)
			{
			iCurrentLine.AppendNumFixedWidth(aUint[i], EHex,2);
			}
		LineFinishedL();
		}
	else if (iStatus == EExpectingAttrID)
		{
		TUint value = 0;
		for (TInt i = 0; i < aUint.Length(); i++)
			{
			value <<= 8;
			value += aUint[i];
			}
		AppendAttrMnemonic(iCurrentLine,value);
		iStatus = EExpectingAttrValue;
		}

	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildIntL(const TDesC8& aInt)
	{
	if (iStatus == EExpectingAttrID)
		{
		_LIT(KErrorText,"**Error: Unexpected Int");
		iCurrentLine.Append(KErrorText);
		}
	else
		{
		iCurrentLine.Append(KTextHexPrefix);
		for (TInt i = 0; i < aInt.Length(); i++)
			{
			iCurrentLine.AppendNumFixedWidth(aInt[i], EHex,2);
			}
		}
	LineFinishedL();
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildUUIDL(const TUUID& aUUID)
	{
	if (iStatus == EExpectingAttrID)
		{
		_LIT(KErrorText,"**Error: Unexpected UUID");
		iCurrentLine.Append(KErrorText);
		}
	else
		{
		iEngine.AppendUUIDText(iCurrentLine,aUUID);
		}
	LineFinishedL();
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildBooleanL(TBool aBool)
	{
	if (iStatus == EExpectingAttrID)
		{
		_LIT(KErrorText,"**Error: Unexpected Boolean");
		iCurrentLine.Append(KErrorText);
		}
	else
		{
		if (aBool)
			{
			_LIT(KTextTrue,"TRUE");
			iCurrentLine.Append(KTextTrue);
			}
		else
			{
			_LIT(KTextFalse,"FALSE");
			iCurrentLine.Append(KTextFalse);
			}
		}
	LineFinishedL();
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildStringL(const TDesC8& aString)
	{
	if (iStatus == EExpectingAttrID)
		{
		_LIT(KErrorText,"**Error: Unexpected String");
		iCurrentLine.Append(KErrorText);
		}
	else
		{
		TBuf16<KMaxDescriptorLength> unicode;
		CnvUtfConverter::ConvertToUnicodeFromUtf8(unicode,aString);
		if (unicode[unicode.Length() - 1] == NULL)
			{
			unicode.Delete(unicode.Length() - 1, 1); // remove NULL-termination
			}
		iCurrentLine.Append(unicode);
		}
	LineFinishedL();
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildDESL()
	{
	if (iIndentation == 1)
		{
		iEngine.AddAttributeLineL(iCurrentLine);
		}
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildDEAL()
	{
	_LIT(KTextDEAL,"Data Element Alternate List");
	iCurrentLine.Append(KTextDEAL);
	iEngine.AddAttributeLineL(iCurrentLine);
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::StartListL()
	{
	iIndentation++;
	iCurrentLine.Zero();
	__ASSERT_ALWAYS(iIndentation > 0, CBtServicesEng::Panic(EBtIndentationZeroInStartList));
	iCurrentLine.AppendFill(' ', (iIndentation - 1) * KIndentationFactor);
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::EndListL()
	{
	_LIT(KTextEndListDashes,"--------");
	iCurrentLine.Append(KTextEndListDashes);
	iEngine.AddAttributeLineL(iCurrentLine);
	iIndentation--;
	iCurrentLine.Zero();
	if (iIndentation)
		{
		iCurrentLine.AppendFill(' ', (iIndentation - 1) * KIndentationFactor); // in this case indentation has already been checked to be greater than zero
		}
	if (iIndentation == 1)
		{
		iStatus = EExpectingAttrID;
		}
	return this;
	}

MSdpElementBuilder* CBtAttributeBuilder::BuildURLL(const TDesC8& aURL)
	{
	TBuf16<KMaxDescriptorLength> unicode;
	CnvUtfConverter::ConvertToUnicodeFromUtf8(unicode,aURL);
	iCurrentLine.Append(unicode);
	LineFinishedL();
	return this;
	}

void CBtAttributeBuilder::AppendAttrMnemonic(TDes& aBuf, TUint aAttrID) const
	{
	for (TInt i = 0; i < iAttrIDs.Count(); i++)
		{
		if (iAttrIDs[i] == aAttrID)
			{
			aBuf.Append(*(iAttrMnemonics[i]));
			return;
			}
		}
	if (aAttrID == 0x0200 && iHandle == 0)
		{
		_LIT(KVersionNoPrefix,"VersionNo.: ");
		aBuf.Append(KVersionNoPrefix);
		}
	else if (aAttrID == 0x0201 && iHandle == 0)
		{
		_LIT(KDatabaseStatePrefix,"DatabaseState: ");
		aBuf.Append(KDatabaseStatePrefix);
		}
	else
		{
		_LIT(KTextAttrIDHex,"AttrID 0x");
		aBuf.Append(KTextAttrIDHex);
		aBuf.AppendNum(aAttrID,EHex);
		_LIT(KTextColonSpace,": ");
		aBuf.Append(KTextColonSpace);
		}
	}

void CBtAttributeBuilder::LineFinishedL()
	{
	iEngine.AddAttributeLineL(iCurrentLine);
	iCurrentLine.Zero();
	__ASSERT_ALWAYS(iIndentation > 0, CBtServicesEng::Panic(EBtIndentationZeroInLineFinished));
	iCurrentLine.AppendFill(' ', (iIndentation - 1) * KIndentationFactor);
	if (iIndentation == 1)
		{
		iStatus = EExpectingAttrID;
		}
	}
