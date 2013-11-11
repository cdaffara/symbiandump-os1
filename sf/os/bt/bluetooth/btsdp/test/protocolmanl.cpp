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
// protocol manual from protocol main
// a test harness for SDP protocol to send manual PDUs into the server
// and onto the wire.
// 
//



#include <e32test.h>
#include <f32file.h>
#include <c32comm.h>
#include "listener.h"
#include "protocolmanl.h"
#include "reqhandler.h"
// #include "builddb.h" we need one of these
#include "debug.h"

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over com2
#endif

CSdpDatabase* BuildDbL();
CSdpDatabase* BuildUpf4DbL();
// in Builddb.cpp

// tsdpdb.cpp
void HexDes(const TDesC8& aDes);


// being called by tsdpdb
// GLDEF_D RTest test(_L("SDP Server injection tests"));

GLDEF_D CSdpDatabase* gSdpTestDatabase;
GLDEF_D TInt sdp_debug_level = 4;

/*
 ** Injector test
 *
 *  load up manual packets to 
		respond synthetically to requests
		send into the server
 */  
static TUint8 transId = 42;

// Inject dummy SDP requests
// inject buffers and return the response
// first parameter is the request ID, and returns the response ID
// so if an error occurs this can be checked...
HBufC8* InjectLC(TUint8& aReqResID, const TPtr8& aReqBuf, TUint aMTU)
{
	TSdpPdu iRequest;
	TSdpPdu iOutput;
               
	iRequest.iTransId = transId++;
	iRequest.iPduId = aReqResID;
	iRequest.iParams.Set(aReqBuf);
	SDP_DEBUG(2, FPrint(_L("InjectLC")));
	SDP_DEBUG(3, FPrint(_L("SDP PDU ID %d, transaction ID %d, parameter length %d, [params]"), iRequest.iPduId, iRequest.iTransId, iRequest.iParams.Length()));
	SDP_DEBUG(4, FHex(iRequest.iParams));

	HBufC8* iWriteHBuf = HBufC8::NewL(aMTU);
	CleanupStack::PushL(iWriteHBuf);

	iOutput.iParams.Set(iWriteHBuf->Des());
	iOutput.iParams.Zero();
	__UHEAP_MARK;
	TRAPD(errorCode, SdpReqHandler::HandleL(*gSdpTestDatabase, iRequest, iOutput));
	__UHEAP_MARKEND;
	if (errorCode != KErrNone)
		{
		errorCode = SdpReqHandler::RunError(errorCode, iRequest, iOutput);
		}
	aReqResID = iOutput.iPduId;
	SDP_DEBUG(3, FPrint(_L("Writing %d bytes of response data [contents]"), iWriteHBuf->Des().Length()));
	SDP_DEBUG(4, FHex(iWriteHBuf->Des()));
	return iWriteHBuf;
}

#define KErrorResponse 0x01
#define KServiceSearchRequest 0x02
#define KServiceSearchResponse 0x03
#define KAttributeRequest 0x04
#define KAttributeResponse 0x05
#define KServiceAttSearchRequest 0x06
#define KServiceAttSearchResponse 0x07

#define DUMMTU 156


// send a request and match a known response, show data only if a mismatch
TBool testSdpReqL(RTest& test, TUint8* aReqData, TUint aReqLen, TUint8 aReqId, TUint8* aRespData, TUint aRespLen, TUint8 aRespId, TUint aRespSize)
{
	TUint8 pduId = aReqId;
	TBool testMatch;
	HBufC8* requestHBuf = HBufC8::New(aReqLen);
	HBufC8* responseHBuf;
	TPtr8 request = requestHBuf->Des();

	request.SetLength(0);
	request.Append(aReqData, aReqLen);
	responseHBuf = InjectLC(pduId, request, aRespSize);
	if (pduId == aRespId)
		{
		TInt respCmp = Mem::Compare(aRespData, aRespLen, &responseHBuf->Des()[0], responseHBuf->Length());
		if (respCmp)
			{
			test.Printf(_L("no match at offset %d:\n"), respCmp);
			HexDes(responseHBuf->Des());
			test.Printf(_L("\n"));
			testMatch = EFalse;
			}
		else
			{
			test.Printf(_L("Response OK\n"));
			testMatch = ETrue;
			}
		}
	else
		{
			test.Printf(_L("Error:"));
			HexDes(responseHBuf->Des());
			test.Printf(_L("\n"));
			testMatch = EFalse;
		}
	CleanupStack::PopAndDestroy(/*responseHBuf*/);
	delete requestHBuf;
	return testMatch;
}

// send a request and resend any continuations - use SDP_DEBUG to dump
TUint testSdpContL(RTest& test, TUint8* aReqData, TUint aReqLen, TUint8 aReqId, /*TUint8* aRespData,*/ TUint aMtu)
{
	TUint8 pduId = aReqId;
	HBufC8* requestHBuf = HBufC8::New(aReqLen + 10 /*KSdpContinuationStateLength*/);
	HBufC8* responseHBuf;
	TPtr8 request = requestHBuf->Des();
	TPtr8 buf(0,0);
	TInt continuationLen = 0;
	TInt partial = 0;
	TInt continutations = 0;

	request.SetLength(0);
	request.Append(aReqData, aReqLen);
	do
		{
		pduId = aReqId;
		responseHBuf = InjectLC(pduId, request, aMtu);
		buf.Set(responseHBuf->Des());

		switch(pduId)
			{
			case 0x03:
				test.Printf(_L("Got SDP_ServiceSearchResponse\n"));
				partial = BigEndian::Get16(&buf[2]);
				partial *= 4;
				partial += 4;
				continuationLen = buf[partial];
				break;
			
			case 0x05:
				test.Printf(_L("Got SDP_ServiceAttributeResponse\n"));
				partial = BigEndian::Get16(&buf[0]);
				partial += 2;
				continuationLen = buf[partial];
				break;
			
			case 0x07:
				test.Printf(_L("Got SDP_ServiceSearchAttributeResponse\n"));
				partial = BigEndian::Get16(&buf[0]);
				partial += 2;
				continuationLen = buf[partial];
				break;
			default:

				test.Printf(_L("Got UnknownResponse (0x%x)\n"), buf[0]);
				continuationLen = 0;	// create a dummy non-continuation
				break;
			}
		continutations++;

		request.Zero();
		request.Append(aReqData, aReqLen-1);
		request.Append(&buf[partial], continuationLen+1);  //1 for continuation len
		CleanupStack::PopAndDestroy(/*responseHBuf*/);
		} while (continuationLen != 0);

	
	delete requestHBuf;
	return continutations;
}


void TestBL(RTest& test)
	{
	__UHEAP_MARK;
	test.Start(_L("Test B -- UPF back to back tests"));

	gSdpTestDatabase = BuildUpf4DbL();
	gSdpTestDatabase->EncodeDbL();

	test.Printf(_L("SS for RFCOMM "));
	TUint8 requestdata00[] = {
		0x35, 0x03, 0x19, 0x00, 0x03,	// UUID list
		0x00, 0x03,						// max handles
		0x00							// continuation
		};
	TUint8 responsedata00[] = {
		0x00, 0x03,						// total handle count
		0x00, 0x03,						// this handle count
		0x00, 0x01, 0x00, 0x00,			// handle
		0x00, 0x01, 0x00, 0x01,			// handle
		0x00, 0x01, 0x00, 0x02,			// handle
		0x00							// continuation
		};
	testSdpReqL(test, &requestdata00[0], sizeof(requestdata00), KServiceSearchRequest, &responsedata00[0], sizeof(responsedata00), KServiceSearchResponse, DUMMTU);


	test.Printf(_L("AR for Server Description "));
	TUint8 requestdata01[] = {
		0x00, 0x00, 0x00, 0x00,			// handle
		0x01, 0x00,						// max bytes
		0x35, 0x03,						// DES header attrib list
		0x09, 0x01, 0x01,				// attribute ID 0101
		0x00							// continuation
		};
	TUint8 responsedata01[] = {
		0x00, 0x1c,						// this byte count
		0x35, 0x1a,						// DES of attribute list
		0x09, 0x01, 0x01,				// attribute ID 0101
		0x25, 0x15,						// string of 15h (21) bytes
		0x45, 0x50, 0x4f, 0x43, 0x20, 0x53, 0x44, 0x50, 0x20, 0x73, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x55, 0x50, 0x46, 0x2d, 0x34,
		0x00							// continuation
		};
	testSdpReqL(test, &requestdata01[0], sizeof(requestdata01), KAttributeRequest, &responsedata01[0], sizeof(responsedata01), KAttributeResponse, 0x100);

	test.Printf(_L("UPF test 01 SS "));
	TUint8 requestdataU01a[] = {
		0x35, 0x03, 0x19, 0x11, 0x03,	// UUID list
		0x00, 0x03,						// max handles
		0x00							// continuation
		};
	TUint8 responsedataU01a[] = {
		0x00, 0x01,						// total handle count
		0x00, 0x01,						// this handle count
		0x00, 0x01, 0x00, 0x00,			// handle
		0x00							// continuation
		};
	testSdpReqL(test, &requestdataU01a[0], sizeof(requestdataU01a), KServiceSearchRequest, &responsedataU01a[0], sizeof(responsedataU01a), KServiceSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 01 AR "));
	TUint8 requestdataU01b[] = {
		0x00, 0x01, 0x00, 0x00,			// handle
		0x01, 0x00,						// max bytes
		0x35, 0x03,						// DES header attrib list
		0x09, 0x00, 0x04,				// attribute ID 0004
		0x00							// continuation
		};
	TUint8 responsedataU01b[] = {
		0x00, 0x13,						// this byte count
		0x35, 0x11,						// DES of attribute list
		0x09, 0x00, 0x04,				// attribute ID 0101
		0x35, 0x0c,						// protocol DES
		0x35, 0x03,						// DES for L2Cap
		0x19, 0x01, 0x00,				// UUID L2CAP
		0x35, 0x05,						// DES for RFCOMM
		0x19, 0x00, 0x03,				// UUID for RFCOMM
		0x08, 0x01,						// RFCOMM port number
// 0x0a, 0x00, 0x00, 0x00, 0x01,			// RFCOMM port number
		0x00							// continuation
		};
	testSdpReqL(test, &requestdataU01b[0], sizeof(requestdataU01b), KAttributeRequest, &responsedataU01b[0], sizeof(responsedataU01b), KAttributeResponse, 0x100);

	test.Printf(_L("UPF test 02 "));
	TUint8 requestdataU02a[] = {
		0x35, 0x03, 0x19, 0x10, 0x02,	// UUID list
		0x01, 0x00,						// max byte count
		0x35, 0x03,						// DES header attrib list
		0x09, 0x01, 0x00,				// attribute ID 0100
		0x00							// continuation
		};
	TUint8 responsedataU02a[] = {
		0x00, 0x4d,						// this byte count
		0x35, 0x4b,						// DES of lists of attribute lists
		0x35, 0x0f,						// DES of first attribute list
		0x09, 0x01, 0x00,				// attribute ID 0100
		0x25, 0x0a,						// string of 0ah (10) bytes
		0x53, 0x44, 0x50, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72,
		0x35, 0x17,						// DES of second attribute list
		0x09, 0x01, 0x00,				// attribute ID 0100
		0x25, 0x12,						// string of 12h (18) bytes
		0x44, 0x69, 0x61, 0x6c, 0x2d, 0x75, 0x70, 0x20, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x69, 0x6e, 0x67,
		0x35, 0x08,						// DES of third attribute list
		0x09, 0x01, 0x00,				// attribute ID 0100
		0x25, 0x03,						// string of 03h bytes
		0x46, 0x61, 0x78,
		0x35, 0x15,						// DES of fourth attribute list
		0x09, 0x01, 0x00,				// attribute ID 0100
		0x25, 0x10,						// string of 10h (16) bytes
		0x4f, 0x42, 0x45, 0x58, 0x20, 0x4f, 0x62, 0x6a, 0x65, 0x78, 0x74, 0x20, 0x50, 0x75, 0x73, 0x68,
		0x00							// continuation
		};
	testSdpReqL(test, requestdataU02a, sizeof(requestdataU02a), KServiceAttSearchRequest, responsedataU02a, sizeof(responsedataU02a), KServiceAttSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 03 part 1"));
	TUint8 requestdataU03a[] = {
		0x35, 0x03, 0x19, 0x11, 0x05,	// UUID list
		0x00, 0x45,						// max byte count
		0x35, 0x05, 0x0a, 0x00, 0x00, 0xff, 0xff, // atttribute range
		0x00							// continuation
		};
	TUint8 responsedataU03a[] = {
		0x00, 0x45,						// this byte count
		0x35, 0x6b,						// outer DES header
		0x35, 0x69,						// first record DES
		0x09, 0x00, 0x00,				// attribute 0
		0x0a, 0x00, 0x01, 0x00, 0x02,	// record 10002
		0x09, 0x00, 0x01,				// attribute 1
		0x35, 0x03, 0x19, 0x11, 0x05,	// service class ID
		0x09, 0x00, 0x04,				// attribute 4
		0x35, 0x14,						// protocol list
		0x35, 0x03, 0x19, 0x01, 0x00,
		0x35, 0x08, 0x19, 0x00, 0x03, 0x0a, 0x00, 0x00, 0x00, 0x03,
		0x35, 0x03, 0x19, 0x00, 0x08,
		0x09, 0x00, 0x05,
		0x35, 0x03, 0x19, 0x10, 0x02,
		0x09, 0x00, 0x06,				// attribute 6
		0x35, 0x0f,
		0x0a, 0x00, 0x00, 0x65, 0x6e,
		0x0a, 0x00, 0x00, 0x00, 0x6a,
		0x0a,							// start of next part
		0x0a, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00 // continuation
		};
	testSdpReqL(test, requestdataU03a, sizeof(requestdataU03a), KServiceAttSearchRequest, responsedataU03a, sizeof(responsedataU03a), KServiceAttSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 03 cont. "));
	TUint8 requestdataU03b[] = {
		0x35, 0x03, 0x19, 0x11, 0x05,	// UUID list
		0x00, 0x45,						// max byte count
		0x35, 0x05, 0x0a, 0x00, 0x00, 0xff, 0xff, // atttribute range
		0x0a, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00 // continuation
		};
	TUint8 responsedataU03b[] = {
		0x00, 0x28,						// this byte count
		0x00, 0x00, 0x01, 0x00,			// the rest of a continuation
		0x09, 0x00, 0x09, 
		0x35, 0x0a, 
		0x35, 0x08, 
		0x19, 0x11, 0x05, 
		0x0a, 0x00, 0x00, 0x01, 0x00,
		0x09, 0x01, 0x00,
		0x25, 0x10, 0x4f, 0x42, 0x45, 0x58, 0x20, 0x4f, 0x62, 0x6a, 0x65, 0x78, 0x74, 0x20, 0x50, 0x75, 0x73, 0x68,
		0x00
		};
	testSdpReqL(test, requestdataU03b, sizeof(requestdataU03b), KServiceAttSearchRequest, responsedataU03b, sizeof(responsedataU03b), KServiceAttSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 04 "));
	TUint8 requestdataU04[] = {
		0x35, 0x03, 0x19, 0x12, 0x34,	// UUID list
		0x00, 0x03,						// max handles
		0x00							// continuation
		};
	TUint8 responsedataU04[] = {
		0x00, 0x00,						// total handle count
		0x00, 0x00,						// this handle count
										// no handle
		0x00							// continuation
		};
	testSdpReqL(test, requestdataU04, sizeof(requestdataU04), KServiceSearchRequest, responsedataU04, sizeof(responsedataU04), KServiceSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 05 "));
	TUint8 requestdataU05[] = {
		0x12, 0x34, 0x56, 0x78,			// handle
		0x01, 0x00,						// max bytes
		0x35, 0x03,						// DES header attrib list
		0x09, 0x00, 0x06,				// attribute ID 0006
		0x00							// continuation
		};
	TUint8 responsedataU05[] = {
		0x00, 0x02						// error response
		};
	testSdpReqL(test, &requestdataU05[0], sizeof(requestdataU05), KAttributeRequest, &responsedataU05[0], sizeof(responsedataU05), KErrorResponse, 0x100);

	test.Printf(_L("UPF test 06 "));
	TUint8 requestdataU06[] = {
		0x35, 0x03, 0x19, 0x11, 0x11,	// UUID list
		0x00, 0x45,						// max byte count
		0x35, 0x03, 0x09, 0xab, 0xcd,	// non existent attrib
		0x00
		};
	TUint8 responsedataU06[] = {
		0x00, 0x02,						// this byte count
		0x35, 0x00, 
		0x00
		};
	testSdpReqL(test, requestdataU06, sizeof(requestdataU06), KServiceAttSearchRequest, responsedataU06, sizeof(responsedataU06), KServiceAttSearchResponse, DUMMTU);

// can't do this test - length is handled in ParseNextPacket - fails with wrong code
	test.Printf(_L("UPF test 07 (can't test) "));
	TUint8 requestdataU07[] = {
		0x00, 0x01, 0x00, 0x02,			// handle for OBEX
		0x01, 0x00,						// max bytes
		0x35, 0x03,						// DES header attrib list
		0x09, 0x00, 0x09,				// attribute ID 0009
		0x00							// continuation
		};
	TUint8 responsedataU07[] = {
		0x00, 0x02						// error response
		};
	testSdpReqL(test, &requestdataU07[0], 50, KAttributeRequest, &responsedataU07[0], sizeof(responsedataU07), KErrorResponse, DUMMTU);


	test.Printf(_L("UPF test 08 "));
	TUint8 requestdataU08[] = {
		0x35, 0x03, 0x09, 0x11, 0x11,	// UINT list instead of UUID list
		0x00, 0x45,						// max byte count
		0x35, 0x03, 0x09, 0x00, 0x09,	// non existent attrib
		0x00
		};
	TUint8 responsedataU08[] = {
		0x00, 0x03						// invalid request syntax
		};
	testSdpReqL(test, requestdataU08, sizeof(requestdataU08), KServiceAttSearchRequest, responsedataU08, sizeof(responsedataU08), KErrorResponse, DUMMTU);

	test.Printf(_L("UPF test 09 "));
	TUint8 requestdataU09[] = {
		0x35, 0x09,						// UUID list
		0x19, 0x11, 0x05,				// OBEX
		0x19, 0x00, 0x03,				// RFCOMM
		0x19, 0x01, 0x00,				// L2CAP
		0x00, 0x45,						// max byte count
		0x35, 0x03, 0x09, 0x00, 0x04,	// attrib 04
		0x00
		};
	TUint8 responsedataU09[] = {
		0x00, 0x1d,
		0x35, 0x1b,
		0x35, 0x19,
		0x09, 0x00, 0x04,
		0x35, 0x14,
		0x35, 0x03, 0x19, 0x01, 0x00,
		0x35, 0x08, 0x19, 0x00, 0x03, 0x0a, 0x00, 0x00, 0x00, 0x03,
		0x35, 0x03, 0x19, 0x00, 0x08,
		0x00
		};
	testSdpReqL(test, requestdataU09, sizeof(requestdataU09), KServiceAttSearchRequest, responsedataU09, sizeof(responsedataU09), KServiceAttSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 10 "));
	TUint8 requestdataU10[] = {
		0x35, 0x03, 0x19, 0x00, 0x03,	// UUID list
		0x00, 0x02,						// max handles
		0x00							// continuation
		};
	TUint8 responsedataU10[] = {
		0x00, 0x02,						// total handle count
		0x00, 0x02,						// this handle count
		0x00, 0x01, 0x00, 0x00,			// handle
		0x00, 0x01, 0x00, 0x01,			// handle
		0x00							// continuation
		};
	testSdpReqL(test, &requestdataU10[0], sizeof(requestdataU10), KServiceSearchRequest, &responsedataU10[0], sizeof(responsedataU10), KServiceSearchResponse, DUMMTU);

	test.Printf(_L("UPF test 11 "));
	TUint8 requestdataU11[] = {
		0x00, 0x01, 0x00, 0x00,			// handle for system
		0x01, 0x00,						// max bytes
		0x35, 0x09,						// DES header attrib list
		0x09, 0x00, 0x06,				// attribute exists
		0x09, 0x00, 0x11,				// attribute doesn't exist
		0x09, 0x01, 0x00,				// attribute exists
		0x00							// continuation
		};
	TUint8 responsedataU11[] = {
		0x00, 0x2d,						//  response
		0x35, 0x2b,
		0x09, 0x00, 0x06,
		0x35, 0x0f, 0x0a, 0x00, 0x00, 0x65, 0x6e, 0x0a, 0x00, 0x00, 0x00, 0x6a, 0x0a, 0x00, 0x00, 0x01, 0x00,
		0x09, 0x01, 0x00,
		0x25, 0x12, 0x44, 0x69, 0x61, 0x6c, 0x2d, 0x75, 0x70, 0x20, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0x69, 0x6e, 0x67,
		0x00
		};
	testSdpReqL(test, &requestdataU11[0], sizeof(requestdataU11), KAttributeRequest, &responsedataU11[0], sizeof(responsedataU11), KAttributeResponse, DUMMTU);

	test.Printf(_L("UPF test 12 (can't test)"));
	
//	CleanupStack::PopAndDestroy(/*CSdpReqHandler*/);
	delete gSdpTestDatabase;
	gSdpTestDatabase = 0;
	__UHEAP_MARKEND;
	test.End();
	}


void TestCL(RTest& test, CSdpDatabase *aDb)
	{
	test.Start(_L("Test C -- continuation stress test"));

	gSdpTestDatabase = aDb;
	gSdpTestDatabase->EncodeDbL();

	sdp_debug_level = 1;  // wind the noise down - this works
	test.Printf(_L("SS for KSdpContTestUUID\n"));
	TUint8 requestdata00a[] = {
		0x35, 0x03, 0x19, 0x01, 0x00,	// UUID list
		0xff, 0xff,						// max handles
		0x00							// continuation
		};

	TUint tCount = testSdpContL(test, requestdata00a, sizeof(requestdata00a), KServiceSearchRequest, /*TUint8* aRespData,*/ DUMMTU);
	test.Printf(_L("SS for KSdpContTestUUID %d continuations\n"), tCount);
	test.Getch();

	test.Printf(_L("AR for all attributes\n"));
	TUint8 requestdata01[] = {
		0x00, 0x00, 0x00, 0x00,			// handle
		0x01, 0x00,						// max bytes
		0x35, 0x05,						// DES header attrib list
		0x0a, 0x00, 0x00, 0xff, 0xff,	// attribute ID range 0000 to ffff
		0x00							// continuation
		};
	tCount = testSdpContL(test, requestdata01, sizeof(requestdata01), KAttributeRequest, /*TUint8* aRespData,*/ DUMMTU);
	test.Printf(_L("AR for all attributes, handle 00000000 %d continuations\n"), tCount);
	test.Getch();

	test.Printf(_L("AR for all attributes\n"));
	TUint8 requestdata02[] = {
		0x00, 0x01, 0x00, 0x00,			// handle
		0x01, 0x00,						// max bytes
		0x35, 0x05,						// DES header attrib list
		0x0a, 0x00, 0x00, 0xff, 0xff,	// attribute ID range 0000 to ffff
		0x00							// continuation
		};
	tCount = testSdpContL(test, requestdata02, sizeof(requestdata02), KAttributeRequest, /*TUint8* aRespData,*/ DUMMTU);
	test.Printf(_L("AR for all attributes, handle 00010000 %d continuations\n"), tCount);
	test.Getch();

	test.Printf(_L("AR for attribute 0004\n"));
	TUint8 requestdata03[] = {
		0x00, 0x01, 0x00, 0x00,			// handle
		0x01, 0x00,						// max bytes
		0x35, 0x03,						// DES header attrib list
		0x09, 0x00, 0x04,				// attribute ID 0004
		0x00							// continuation
		};
	tCount = testSdpContL(test, requestdata03, sizeof(requestdata03), KAttributeRequest, /*TUint8* aRespData,*/ DUMMTU);
	test.Printf(_L("AR for attribute 0004, handle 00010000 %d continuations\n"), tCount);
	test.Getch();

	test.Printf(_L("AR for all attributes\n"));
	TUint8 requestdata04[] = {
		0x00, 0x01, 0x00, 0x03e,		// handle
		0x01, 0x00,						// max bytes
		0x35, 0x05,						// DES header attrib list
		0x0a, 0x00, 0x00, 0xff, 0xff,	// attribute ID range 0000 to ffff
		0x00							// continuation
		};
	tCount = testSdpContL(test, requestdata04, sizeof(requestdata04), KAttributeRequest, /*TUint8* aRespData,*/ DUMMTU);
	test.Printf(_L("AR for all attributes, handle 0001003e %d continuations\n"), tCount);
	test.Getch();

	sdp_debug_level = 5;

	test.Printf(_L("SAS for all attributes\n"));
	TUint8 requestdata05[] = {
		0x35, 0x03, 0x19, 0x01, 0x00,	// UUID list
		0x01, 0x00,						// max byte count
		0x35, 0x05,						// DES header attrib list
		0x0a, 0x00, 0x00, 0xff, 0xff,	// attribute ID range 0000 to ffff
		0x00							// continuation
		};
	tCount = testSdpContL(test, requestdata05, sizeof(requestdata05), KServiceAttSearchRequest, /*TUint8* aRespData,*/ DUMMTU);
//	tCount = testSdpCont(test, requestdata05, sizeof(requestdata05), KServiceAttSearchRequest, /*TUint8* aRespData,*/ 102);
	test.Printf(_L("SAS for all attributes, %d continuations\n"), tCount);
	test.Getch();
	
//	CleanupStack::PopAndDestroy(/*CSdpReqHandler*/);
	delete gSdpTestDatabase;
	gSdpTestDatabase = 0;
	test.End();
	}
