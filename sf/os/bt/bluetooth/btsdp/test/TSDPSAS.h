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
// Verification Test SDP
// 
//

#include <btsdp.h>
#include "sdpkey.h"
#include "SDPDatabase.h"
#include <btmanclient.h>

#ifndef __SDPSAS_H__
#define __SDPSAS_H__

#ifndef __ES_SOCK_H__
#include <es_sock.h>
#endif

#ifndef __BT_SOCK_H__
#include <bt_sock.h>
#endif


// Constants
const TInt KMaxRequestableUUIDs	= 12;
const TInt KMaxNumRecordHandles	= 50;
const TInt KMaxNumBrowseUUIDs	= 50;
const TInt KNumSDPServiceTests = 6;
const TInt KNumSDPAttributeTests = 23;
const TInt KNumSDPServiceAttributeTests = 24;
const TInt KMaxAttributeListByteCount = 200;

// SDP test base class. Provides untilities for running raw SDP tests over
// L2CAP. 
class CSdpTestBase : public CBase
	{
public:
	enum TUuidType {EUuid16,
					EUuid32,
					EUuid128,
					EUuidInvalid} ;
	CSdpTestBase(CConsoleBase& aConsole);
	~CSdpTestBase();

	// Utility functions
	//
	void StartSDPServerL();
	void SendCloseL();
	void WaitForCloseL();
	void RegisterDatabaseInServerL(CSdpDatabase& aDb);
	void SdpConnectL();
	virtual	CSdpDatabase* BuildDbL() = 0;	// To be implemented according to test types.

protected:
	// SDP database stuff
	//
	RSdp iSdp;
	RSdpDatabase iSdpDatabase;
	CSdpDatabase* iTheDb;
	CConsoleBase& iConsole;
	RSocket iRawSock;	// Socket for sending raw bytes over L2CAP PSM 1 (SDP).

	TUint16 iRawTransactionId;	// Incremented to provide a unique ID for every query.

	};

// Class to represent the SDP header bytes
class TSDPHeader
{
public:
	TSDPHeader();
	TSDPHeader(TUint8 aPduId, TUint16 aTranId, TUint16 aLen);
	void AppendHeader(TDes8& aBuf);

	inline TUint8 GetPduUd(){return iPduId;}
	inline TUint16 GetTransactionId(){return iTransactionId;}
	inline TUint16 GetParamsLen(){return iParamLen;}
private:
	TUint8  iPduId;
	TUint16  iTransactionId;
	TUint16  iParamLen;
};

// Class to represent the SDP service request bytes
class TServiceRequest	// Note this doesn't contain continuation
	{
public:
	TServiceRequest();
	//TServiceRequest(TUint8* aDes, const TUint8* aUuid128, TUint16 aMaxRecordCount);
	TServiceRequest(TUint8* aDes, TUUID aUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxRecordCount);
	TServiceRequest(TUint8* aDes, CSdpSearchPattern& aSP, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxRecordCount);
	TServiceRequest(TUUID aUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxRecordCount);
	TServiceRequest(CSdpSearchPattern& aSP, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxRecordCount);
	void SetupDES();
	void SetUuidType(CSdpTestBase::TUuidType aUuidType);
	void AppendRequest(TDes8& aBuf);

	inline TUUID Uuid(){return iUuid;}
	inline CSdpSearchPattern* SP(){return iSP;}
	inline CSdpTestBase::TUuidType UuidType(){return iUuidType;}
	inline TUint16 MaxRecordCount(){return iMaxRecordCount;}
	inline TUint8* Des(){return iDes;}
private:
	TUint8  iDes[3];
	//TUint32	iUuid32;
	TUUID iUuid;
	CSdpSearchPattern* iSP;//for multiple uuid requests
	TUint16 iMaxRecordCount;
	CSdpTestBase::TUuidType iUuidType; //inticates which length of Uuid used
	TBool iAutoDes;
	};
	
// Class to represent the SDP attribute request bytes
class TAttributeRequest	
	{
public:
	TAttributeRequest();
	TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint8* aDes, TUint16 aAttributeId);
	TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint8* aDes, TUint32 aAttributeIdRange);
	TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint16 aAttributeId);
	TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint32 aAttributeIdRange);
	void SetupDES();
	void AppendRequest(TDes8& aBuf);

	inline TUint32 RecordHandle(){return iRecordHandle;}
	inline void SetRecordHandle(TUint16 aRecordHandle){iRecordHandle=aRecordHandle;}
	inline TUint16 MaxByteCount(){return iMaxByteCount;}
	inline TUint16 AttributeId(){return iAttributeId;}
	inline TUint32 AttributeIdRange(){return iAttributeIdRange;}
	inline TUint8* Des(){return iDes;}

private:
	TUint32 iRecordHandle;
	TUint16 iMaxByteCount;
	TUint8  iDes[3];
	TUint16	iAttributeId;
	TUint32	iAttributeIdRange;
	TBool iAutoDes;
	}; 

//
// TServiceAttributeRequest
//
class TServiceAttributeRequest
	{
public:
	TServiceAttributeRequest();

	//TServiceAttributeRequest(TUint8* aServiceDes, TUint16 aServiceUuid, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint16 aAttributeId);
	//TServiceAttributeRequest(TUint8* aServiceDes, TUint32 aServiceUuid, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint16 aAttributeId);
	TServiceAttributeRequest(TUint8* aServiceDes, TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint16 aAttributeId);
	TServiceAttributeRequest(TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint16 aAttributeId);

	//TServiceAttributeRequest(TUint8* aServiceDes, TUint16 aServiceUuid, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint32 aAttributeIdRange);
	//TServiceAttributeRequest(TUint8* aServiceDes, TUint32 aServiceUuid, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint32 aAttributeIdRange);
	TServiceAttributeRequest(TUint8* aServiceDes, TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint32 aAttributeIdRange);
	TServiceAttributeRequest(TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint32 aAttributeIdRange);

	void SetUuidType(CSdpTestBase::TUuidType aUuidType);
	void AppendRequest(TDes8& aBuf);

	inline TUint16 MaxByteCount(){return iAttributeRequest.MaxByteCount();}

	inline TServiceRequest& ServiceParams(){return iServiceRequest;}
	inline TAttributeRequest& AttributeParams(){return iAttributeRequest;}
private:
	//Have this as a combination of service and attribute requests
	//
	TServiceRequest iServiceRequest;
	TAttributeRequest iAttributeRequest;

	};



// Class for the SDP verification test group.
class CSdpVerificationTests : public CSdpTestBase , public MSdpElementBuilder
	{
public:
	static void RunTestL(CConsoleBase& aConsole);
	CSdpVerificationTests(CConsoleBase& aConsole);
	~CSdpVerificationTests();

	// Master 
	void RunMasterTestL();
	CSdpDatabase* BuildDbL();
	void PrintDb(CSdpDatabase& aDb, CConsoleBase& aConsole);
private:

	// Test functions
	void DoTestsL();
	void SDPQueryL();
	void RunSDPSearchL();
	void SDPServiceSearchL();
	void SDPAttributeSearchL();
	void SDPServiceAttributeSearch();
	void SDPBrowseTestL(TUuidType aUuidType);
	TBool AnotherQuery();
	TBool Continue();
	TInt GetUuidSize();
	void UpdateTestString();
	void FlogTestId();
	TInt MaxCountFoundAfterServiceSearchPatternOrWhatYouWill(TDesC8& aString);
	void FlogParsedPDUHeader(TDesC8& aString);
	void FlogString(const TText* aLineStart, TDesC8& aString);
	void FlogReadString(TDesC8& aString);
	void FlogContinuationString(TDesC8&aString);
	void FlogWriteString(TDesC8& aString);
	void WaitForStartSignalL();
	void SendStartSignalL();
	void PromptedGetch();

	// Browse tests
	//
	void BuildBrowseDbL(CConsoleBase&  aConsole);

	// Initialisation
	void SetUpServiceTestParams();
	void SetUpAttributeTestParams();
	void SetUpServiceAttributeTestParams();

	// Process results
	TInt ProcessSearchResults(TDesC8& aResult1, TDesC8& aResult2);
	TInt ProcessServiceSearchResults(TDesC8& aResult1, TDesC8& aResult2);
	TInt ProcessAttributeSearchResults(TDesC8& aResult1, TDesC8& aResult2);
	TInt ProcessServiceAttributeSearchResults(TDesC8& aResult1, TDesC8& aResult2);

	// MSdpElementBuilder implementations
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	MSdpElementBuilder* BuildDESL();  // Must not return NULL
	MSdpElementBuilder* BuildURLL(const TDesC8& aURL);
	MSdpElementBuilder* StartListL();
	MSdpElementBuilder* EndListL();
	MSdpElementBuilder* BuildNilL();


	// Private data
	//
	TServiceRequest iServiceTests[KNumSDPServiceTests + KNumSDPAttributeTests];	// Holds the service test params.
																				// Attribute tests involve a service query as well.
	TAttributeRequest iAttributeTests[KNumSDPAttributeTests];	//Holds the attribute test params
	TServiceAttributeRequest iServiceAttributeTests[KNumSDPServiceAttributeTests];
	
	CSdpSearchPattern* iSP;

	
	TUint8 iContinuationBuffer[KSdpContinuationStateMaxLength];	// Buffer for continuation state
	TUint8 iContLength;	// Continuation state length
	TBuf8<14000>	iContinuationStore;	// Holds the growing buffer while the requests 
									// are returning one by one
	TUint32 iAttributeTestServiceRecordHandle;
	TUint32 iBrowseTestServiceRecordHandle[KMaxNumRecordHandles];
	TUint32 iBrowseTestNumRecordHandlesStored;
	TUUID iBrowseUUID[KMaxNumBrowseUUIDs];
	TInt iNumBrowseUUIDs;

	enum TTestType {EServiceRequest,
					EServiceAttributeRequest,
					EServiceSearchAttributeRequest} ;

	TTestType iType;	// The type of test currently being run
	TInt8 iQueryIndex;	// For the tests that have 2 queries.
	TTestType	iQueryState;
	TUint iTestNum;		// THe number of test running.
	TBool iNeedAttributeId;
	TInt iBuildingList;
	TInt iBrowseIter;
	TInt iIndent;
	TInt iRequestMaxCount;
	TInt iRequestMaxCount1;
	TInt iRequestMaxCount2;
	TInt iRequestTransId;
	TInt iRequestTransId1;
	TInt iRequestTransId2;
	TBool iNeedMoreRecordHandles;
	CSdpTestBase::TUuidType iUuidType; //indicates which length of Uuid to be used
	TBuf<77> iTestName;
	};

class CBrowseResponseBuilder : public CBase, public MSdpElementBuilder
	{
public:
		CBrowseResponseBuilder(TDes& aTestName, TInt aBrowseIter, CSdpSearchPattern& aBrowseSP);
		~CBrowseResponseBuilder();
		MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
		MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
 		MSdpElementBuilder* BuildDESL();  // Must not return NULL
		MSdpElementBuilder* StartListL();
		MSdpElementBuilder* EndListL();
private:
	TDes& iTestName;
	TInt iBrowseIter;
	TBool iNeedAttributeId;
	TInt iBuildingList;
	CSdpSearchPattern& iBrowseSP;
	};


class CPrintBuilder : public CBase, public MSdpElementBuilder
	{
public:
	CPrintBuilder(CConsoleBase& aConsole);
	~CPrintBuilder();

	void HexDumpVT(const TDesC8& aDes);
	MSdpElementBuilder* BuildUnknown(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildIntL(const TDesC8& aUint);
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* BuildBooleanL(TBool aBool);
	MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	MSdpElementBuilder* BuildDESL();  // Must not return NULL
	MSdpElementBuilder* BuildDEAL();  // Must not return NULL
	MSdpElementBuilder* BuildURLL(const TDesC8& aURL);
	MSdpElementBuilder* StartListL();
	MSdpElementBuilder* EndListL();
	MSdpElementBuilder* BuildNilL();

private:
	TInt iIndent;
	CConsoleBase& iConsole;
	};
#endif
