// BtServicesEng.h
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

#ifndef BTSERVICESENG_H_
#define BTSERVICESENG_H_

#include <btsdp.h>
#include <es_sock.h>
#include <bt_sock.h>
#include <barsread.h>

#include "btserviceview.h"
//#include <QikListBoxModel.h>

enum TBtPanicCode
	{
	EBtEngineNotCreated = 1,
	EBtEngineAlreadyExists = 2,
	EBtIndentationZeroInStartList = 3,
	EBtIndentationZeroInLineFinished = 4
	};

class CBtAttributeBuilder;

class CServiceRecordItem
	{
public:
	CServiceRecordItem()
	{
	iHandle = 0;
	iServiceName = _L("");
	}
	
	~CServiceRecordItem(){;}
	
	TSdpServRecordHandle iHandle;
	TBuf<64> iServiceName;
	
	};

/**
This provides the main application functionality, specifically
setting up the SDP agent and implementing the notifier interface
to build the results into an array, displayed by the main View
(CBtServicesListView) in the case of services, or by the attribute
dialog in the case of attributes for a selected service.
*/
class CBtServicesEng : public CBase, public MSdpAgentNotifier  
	{
public:
	static CBtServicesEng* NewL();
	~CBtServicesEng();
	void SetView(CBtServiceView* apView);
	//CDesCArrayFlat* GetTextArray() const;

	// defined in MSdpAgentNotifier
	void AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError);
	void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
	void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);

	/////////////////////////////////////////////////////////
	void ReportMessage(TInt aVerboseLevel, TRefByValue<const TDesC> aFmt, ...);
	
	void MakeTextServiceList();
	void MakeTextUUIDList();
	
	TInt BluetoothReady();
	void NewDeviceSelectedL(const TBTDevAddr& aAddress,const TBTDeviceName& aDeviceName);
	void GetDeviceServicesL();

	void GetAttributesL(TInt aIndex);
	void GetDeviceAttributesL(const TBTDevAddr& aAddress,const TBTDeviceName& aDeviceName, TSdpServRecordHandle aHandle);
	
	TBool GettingAllAttributes() const;
	void AddAttributeLineL(const TDesC& aDes);
	void SetUUIDFilterL(TUint32 aUUID);
	void DisplayDeviceInfoL();
	void DisplayLocalInfoL();
	void CancelSdpAgent();
	void DeleteSdpAgent();
	void AppendUUIDText(TDes& aBuf, TUUID aUUID) const;
	
	static void Panic(TBtPanicCode aPanicCode);
	static TPtrC UUIDToString(TUint32 aUuidValue);
	static TBool StringToUUID(TDesC& aString, TUint32& aUuidValue);	
private:
	CBtServicesEng();
	void ConstructL();
	void GetServiceNamesL();
	void SdpCompletedWithError(const TInt aError);
	void AppendErrorMessage(const TInt aError, TDes& aBuf) const;
	void DoAttributeRequestCompleteL();
	void DoAttributeRequestResultL(CSdpAttrValue* aAttrValue, TSdpServRecordHandle aHandle);
	void DoNextRecordRequestCompleteL(TSdpServRecordHandle aHandle, TInt aError);
	void ReleaseModel();

private:
	// Values of iCurrentAttributeRequest
	enum TAttributeRequests
		{
		ENoOutstandingRequests,
		EGettingNamesOnly,
		EGettingAllAttributes
		};

	CSdpAgent* iSdpAgent;
	CSdpSearchPattern* iSearchPattern;
	//CDesCArrayFlat* iTextArray;
	CDesCArrayFlat* iAttributeArray;
	RPointerArray <CServiceRecordItem> iRecordArray;
	CSdpAttrIdMatchList* iAttributeMatchList;
	CBtAttributeBuilder* iAttributeBuilder;

	CBtServiceView* iView; // iView is not "owned" by this
	//CEikonEnv* iEikon;
	
	TInt iHandleCounter;
	TAttributeRequests iCurrentAttributeRequest;
	TBTDeviceName iDeviceName;	//this is taken from the caller and only be used to display
	TBTDevAddr iDeviceAddress;	//critical for SDP
	
	//TBTDeviceClass iDeviceClass;//this is taken from the caller and only be used to display
	
	TPtrC iServiceName;
	TUint32 iUUIDFilter;

	RArray<TUUID> iUUIDs;
	RPointerArray<HBufC> iUUIDsAsText;
	RArray<TInt> iErrorCodes;
	RPointerArray<HBufC> iErrorMessages;

	RSocketServ iSocketServ;
	
	TBuf<512> iTextBuf;
	};

/**
Receives and parses the results from an SDP Attribute Request, and builds
them into a table (array for listbox) for displaying in
the attributes dialog.
*/
class CBtAttributeBuilder : public CBase, public MSdpElementBuilder
	{
public:
	static CBtAttributeBuilder* NewL(CBtServicesEng& aEngine);
	void SetHandle(TSdpServRecordHandle aHandle);
	void Reset();
	~CBtAttributeBuilder();
private:
	// defined in MSdpElementBuilder
	MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	MSdpElementBuilder* BuildNilL();
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* BuildBooleanL(TBool aBool);
	MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	MSdpElementBuilder* BuildDESL();  // Must not return NULL
	MSdpElementBuilder* BuildDEAL();  // ditto
	MSdpElementBuilder* StartListL(); // ditto
	MSdpElementBuilder* EndListL();
	MSdpElementBuilder* BuildURLL(const TDesC8& aURL);

private:
	CBtAttributeBuilder(CBtServicesEng& aEngine);
	void AppendAttrMnemonic(TDes& aBuf, TUint aAttrID) const;
	void LineFinishedL();
	void ConstructL();
private:
	enum TCurrentStatus {
		EExpectingAttrID,
		EExpectingAttrValue
		};

	RArray<TUint> iAttrIDs;
	RPointerArray<HBufC> iAttrMnemonics;

	CBtServicesEng& iEngine;
	TBuf<KMaxDescriptorLength> iCurrentLine;
	TInt iIndentation;
	TCurrentStatus iStatus;
	TSdpServRecordHandle iHandle;
	};


#endif
