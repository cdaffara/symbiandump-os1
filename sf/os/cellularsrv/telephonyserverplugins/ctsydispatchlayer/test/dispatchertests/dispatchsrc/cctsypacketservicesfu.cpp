// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file The TEFUnit test suite for PacketServicesControl in the Common TSY.
*/

#include "cctsypacketservicesfu.h"
#include <etel.h>
#include <etelmm.h>
#include <in_sock.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>
#include <pcktcs.h>
#include "mockltsyindicatorids.h"
#include <mbmstypes.h>
#include "listretrieverao.h"

CTestSuite* CCTsyPacketServicesFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0001L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0002L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0003L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0004L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0005L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0006L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUseCase0007L);
   
   	//add other unit tests
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0001L);
 	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0002L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0003L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0004L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0005L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0006L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0007L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0008L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit0009L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00010L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00011AL);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00011BL);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00011CL);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00012L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00013L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00014L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00015L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00016L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00017L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00018L);
 	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00019L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00020L);
 	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00021L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00022L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00023L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00024L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00025L);
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFU, TestUnit00026L);

	END_SUITE;
	}


//
// Drivers
//

// Generates the primary and secondary DNS buffer as described in 3GPP specification 3G TS 24.008
void CCTsyPacketServicesFU::ConvertDNSToPCOBuffer
	(const TDes8& aPrimaryDNS, const TDes8& aSecondaryDNS, const TUint8 aRequestID, TDes8&  aFormatPcoBufferAddr)
	{
	// Handle Primary DNS address
	TInetAddr netPrimaryDNS;
	TBuf<KDnsLength> netPrimaryDNSConv;
	netPrimaryDNSConv.Copy(aPrimaryDNS);
	netPrimaryDNS.Input(netPrimaryDNSConv);
	TUint32 primDNSaddr =  netPrimaryDNS.Address();

	// Handle Secondary DNS address
	TInetAddr netSecondaryDNS;
	TBuf<KDnsLength> netSecondaryDNSConv;
	netSecondaryDNSConv.Copy(aSecondaryDNS);
	netSecondaryDNS.Input(netSecondaryDNSConv);
	TUint32 secondaryDNSaddr =  netSecondaryDNS.Address();

	aFormatPcoBufferAddr[0] = KPrimaryAndSecondaryDNSLength;	// Length
	aFormatPcoBufferAddr[1] = KIPCRequestCode; 					// IPC PRequest Code
	aFormatPcoBufferAddr[2] = aRequestID; 						// Request ID (CHAP challenge)
	aFormatPcoBufferAddr[3] = KPacketLengthMSB;					// Packet length MSB (always zero)
	aFormatPcoBufferAddr[4] = KPrimaryAndSecondaryDNSLength;	// Packet length LSB
	aFormatPcoBufferAddr[5] = KIndicatePrimaryAddress; 			// Indicate Primary address
	aFormatPcoBufferAddr[6]	= KDnsAddressLength;				// Length of address (inc header)
	aFormatPcoBufferAddr[7] = (primDNSaddr >> 24) & 0xff; 		// Primary DNS
	aFormatPcoBufferAddr[8] = (primDNSaddr >> 16) & 0xff; 		// Primary DNS
	aFormatPcoBufferAddr[9] = (primDNSaddr >> 8) & 0xff; 		// Primary DNS
	aFormatPcoBufferAddr[10] =  primDNSaddr & 0xff; 			// Primary DNS
	aFormatPcoBufferAddr[11] = KIndicateSecondaryAddress;		// Indicate Secondary address
	aFormatPcoBufferAddr[12] = KDnsAddressLength;				// Length of address (inc header)
	aFormatPcoBufferAddr[13] = (secondaryDNSaddr >> 24) & 0xff; // Secondary DNS
	aFormatPcoBufferAddr[14] = (secondaryDNSaddr >> 16) & 0xff; // Secondary DNS
	aFormatPcoBufferAddr[15] = (secondaryDNSaddr >> 8) & 0xff; 	// Secondary DNS
	aFormatPcoBufferAddr[16] =  secondaryDNSaddr & 0xff; 		// Secondary DNS
	aFormatPcoBufferAddr.SetLength(KPrimaryAndSecondaryDNSLength + 1);
	}

// Generates the username, password and requestID buffer as described in 3GPP specification 3G TS 24.008
void CCTsyPacketServicesFU::ConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, 
		const TDes8& aPassword, const TUint8 aRequestID, TDes8& aFormatedBuffer)
	{
	TInt usernameLen = aUsername.Length();
	TInt passwordLen = aPassword.Length();
	TInt dataLen = usernameLen + passwordLen + KPasswordUsernameHeaderLength;
	TInt i = 0;

	aFormatedBuffer[i++] = dataLen; 				// Length
	aFormatedBuffer[i++] = KIPCRequestCode; 		// IPC PRequest Codeio
	aFormatedBuffer[i++] = aRequestID; 				// Request ID (CHAP challenge)
	aFormatedBuffer[i++] = KPacketLengthMSB; 		// Packet length MSB (always zero)
	aFormatedBuffer[i++] = dataLen; 				// Packet length LSB
	aFormatedBuffer[i++] = usernameLen; 			// Length of username
	aFormatedBuffer.SetLength(i);
	aFormatedBuffer.Append(aUsername); 				// Username
	aFormatedBuffer.SetLength(i + usernameLen + 1);
	aFormatedBuffer[i + usernameLen] = passwordLen; // Length of password
	aFormatedBuffer.Append(aPassword);				// Password
	}

/***
* 											PCO - Description
*
* The purpose of the protocol configuration options (PCO) is to transfer external network protocol options 
* associated with a PDP context activation, and transfer additional (protocol) data 
* (e.g. configuration parameters, error codes or messages/events) associated with an external protocol 
* or an application.
* The protocol configuration options (PCO) is a type 4 information element with a minimum length of 3 
* octets and a maximum length of 253 octets
* In order to generate the PCO buffer a TTlvStruct object is being used. The TTlvStruct wraps the buffers  
* inserted to the PCO and identifies the buffers with given IDs.
*  
* In the following test the PCO buffer is initiated with the username, password, requestID, 
* primary DNS and secondary DNS. In addition the PCO contains a Misc Buffer which has the 
* same structure and the same characteristics as the PCO buffer and can contain 
* Miscellaneous protocol information. The main risk with the Misc Buffer is an overflow 
* (The dispatcher handles the overflow scenario). 
*
* Following is an example of a PCO buffer outline:
*	
*	PcoBuffer[0] == 0x80);  // Config options ext bit
*	PcoBuffer[1] == 0xC0);  // Header 1
*	PcoBuffer[2] == 0x23);  // Header 2
*	PcoBuffer[3] == 0xC);   // Length
*	PcoBuffer[4] == 0x1);   // IPC PRequest Code
*	PcoBuffer[5] == 0x0);   // Request ID (CHAP challenge)
*	PcoBuffer[6] == 0x0);   // Packet length MSB (always zero)
*	PcoBuffer[7] == 0xC);   // Packet length LSB
*	PcoBuffer[8] == 0x3);   // Length of username
*	PcoBuffer[9] == 119);   // Username
*	PcoBuffer[10] == 101);  // Username
*	PcoBuffer[11] == 98);   // Username
*	PcoBuffer[12] == 0x3);  // Length of password
*	PcoBuffer[13] == 119);  // Password
*	PcoBuffer[14] == 101);  // Password
*	PcoBuffer[15] == 98);   // Password
*	PcoBuffer[16] == 0x80); // DNS HeaderId 1
*	PcoBuffer[17] == 0x21); // DNS HeaderId 2
*	PcoBuffer[18] == 0x10); // Length
*	PcoBuffer[19] == 0x1);  // IPC PRequest Code
*	PcoBuffer[20] == 0x1);  // Request ID (CHAP challenge)
*	PcoBuffer[21] == 0x0);  // Packet length MSB (always zero)  
*	PcoBuffer[22] == 0x10); // Packet length LSB
*	PcoBuffer[23] == 0x81); // Indicate Primary address 
*	PcoBuffer[24] == 0x6);  // Length of address (inc header)        
*	PcoBuffer[25] == 0xBE); // Primary DNS
*	PcoBuffer[26] == 0x0);  // Primary DNS
*	PcoBuffer[27] == 0x65); // Primary DNS
*	PcoBuffer[28] == 0xBD); // Primary DNS               
*	PcoBuffer[29] == 0x83); // Indicate Secondary address 
*	PcoBuffer[30] == 0x6);  // Length of address (inc header)     
*	PcoBuffer[31] == 0x19); // Secondary DNS
*	PcoBuffer[32] == 0x5);  // Secondary DNS
*	PcoBuffer[33] == 0x28); // Secondary DNS
*	PcoBuffer[34] == 0xBF); // Secondary DNS
*   
*   The Misc buffer should be added after the secondary DNS value (i.e. location 35 in the
*   example above).      
*/
 
// 
//
//
// Generates a PCO buffer out of a PCO object.
void CCTsyPacketServicesFU::PCOGeneratorL(RPacketContext::TProtocolConfigOptionV2& aPco, RPacketContext::TMiscProtocolBuffer& aPcoBuffer)
	{

	TBuf8<KMaxUserAndPassLength> userAndPasswordBuffer(KMaxUserAndPassLength);

	// Generates the username, password and requestID buffer as described in 3GPP specification 3G TS 24.008
	ConvertUsernameAndPasswordToPCOBuffer(aPco.iAuthInfo.iUsername, aPco.iAuthInfo.iPassword, aPco.iId++, userAndPasswordBuffer);

	TPtr8 userAndPasswordParamDataPtr(const_cast<TUint8*>(userAndPasswordBuffer.Ptr()),
			userAndPasswordBuffer.Length(), userAndPasswordBuffer.Length());

	TBuf8<KMaxPdpAddressLength> primaryAndSecondaryDNSBuffer(KPrimaryAndSecondaryDNSLength + 1);

	// Generates the primary and secondary DNS buffer as described in 3GPP specification 3G TS 24.008
	ConvertDNSToPCOBuffer(aPco.iDnsAddresses.iPrimaryDns, aPco.iDnsAddresses.iSecondaryDns, aPco.iId++, primaryAndSecondaryDNSBuffer);

	// Setting the configurations buffers
	TPtr8 primaryAndSecondaryDNSParamDataPtr(const_cast<TUint8*>(primaryAndSecondaryDNSBuffer.Ptr()),
				primaryAndSecondaryDNSBuffer.Length(), primaryAndSecondaryDNSBuffer.Length());

	TPtr8 pcoPtr (const_cast<TUint8*> (aPcoBuffer.Ptr()), RPacketContext::KMiscProtocolBufferLength);
	TTlvStruct<RPacketContext::TPcoId, RPacketContext::TPcoItemDataLength> pcoTlv(pcoPtr,0);

	RPacketContext::TPcoId usenameAndPasswordID(KUsenamePasswordDNSID);
	RPacketContext::TPcoId primaryAndSecondaryDNSID(KPrimaryAndSecondaryDNSID);

	pcoTlv.AppendItemL(usenameAndPasswordID, userAndPasswordParamDataPtr);
	pcoTlv.AppendItemL(primaryAndSecondaryDNSID, primaryAndSecondaryDNSParamDataPtr);

	aPcoBuffer.SetLength(primaryAndSecondaryDNSParamDataPtr.Length() + userAndPasswordParamDataPtr.Length() + 2 * KIDLength);

	// Append the misc buffer which is alread in TLV structure
	aPcoBuffer.Append(aPco.iMiscBuffer);
	}

void CCTsyPacketServicesFU::DriverDeleteContextL(TInfoName aContextName)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	TMockLtsyData1<TInfoName> contextNameData(aContextName);
	data.Close();
	contextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, data, KErrNone);
	CleanupStack::PopAndDestroy(1, &data); // data
	}

void CCTsyPacketServicesFU::DriverAttachPacketServiceL()
	{
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId, KErrNone);
	}

void CCTsyPacketServicesFU::DriverDetachPacketServiceL()
	{
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId, KErrNone);

	// SetAttachModeL being called as part of the detach completion process
	RBuf8 dataExpect;
    CleanupClosePushL(dataExpect);
    RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
    TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
    dataExpect.Close();
    ltsyData.SerialiseL(dataExpect);

    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId, dataExpect, KErrNone);
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId, KErrNone);

	CleanupStack::PopAndDestroy(1, &dataExpect); // dataExpect
	}

void CCTsyPacketServicesFU::DriverInitialiseContextL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName primaryContext;
	TInfoName secondaryContext;
	primaryContext.Copy(KFirstAllocatedContextName);
	secondaryContext.Copy(KDefaultContextName);

	//-------------------------------------------------------------------------
	// Successful completion request of
	// RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData3(primaryContext, secondaryContext);
	data.Close();
	expLtsyContextNamesData3.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data);

	TDesC* contextNamePtr = &primaryContext;
	TDesC* channelIdPtr = const_cast<TDesC*>(&KDefaultChannelId);
	TMockLtsyData2<TDesC*, TDesC*> expLtsyContextNameAndChannelIdData(contextNamePtr, channelIdPtr);
	data.Close();
	expLtsyContextNameAndChannelIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNone, data);

	CleanupStack::PopAndDestroy(1, &data); // data
	}

void CCTsyPacketServicesFU::DriverInitialiseContextL(const TInfoName& aPrimaryContextId, const TInfoName& aSecondaryContextId)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName& primaryContext = const_cast<TInfoName&>(aPrimaryContextId);
	TInfoName& secondaryContext = const_cast<TInfoName&>(aSecondaryContextId);

	//-------------------------------------------------------------------------
	// Successful completion request of
	// RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData(primaryContext, secondaryContext);
	data.Close();
	expLtsyContextNamesData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data);


	// Check if a secondary context is being initialise and if it is then compelte initialisation for secondary context.
	TDesC* contextNamePtr = (aSecondaryContextId == KDefaultContextName) ? &primaryContext : &secondaryContext;
	
	TDesC* channelIdPtr = const_cast<TDesC*>(&KSecondaryContextChannelId);
	TMockLtsyData2<TDesC*, TDesC*> expLtsyContextNameAndPortData2(contextNamePtr, channelIdPtr);
	data.Close();
	expLtsyContextNameAndPortData2.SerialiseL(data);	
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNone, data);

	CleanupStack::PopAndDestroy(&data); // data
	}


void CCTsyPacketServicesFU::DriverSetConfigContextL(TInfoName aContextId,
		RPacketContext::TGSNAddress aAccessPoint, RPacketContext::TProtocolType aPdpType, 
		RPacketContext::TProtocolAddress aPdpAddress,
		RPacketContext::TMiscProtocolBuffer aPcoBuffer)
	{

	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType, RPacketContext::TProtocolAddress, RPacketContext::TMiscProtocolBuffer>
					packetSetConfigData(aContextId, aAccessPoint, aPdpType, aPdpAddress, aPcoBuffer);
	data.Close();
	packetSetConfigData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, data, KErrNone);

	// Indicate that the context name is due to comeback from the SetConfig callback
	TInfoName callbackContextId = aContextId;
	TMockLtsyData1<TInfoName> callbackContextIdData(callbackContextId);
	data.Close();
	callbackContextIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);

	CleanupStack::PopAndDestroy(1, &data); // data
	}
void CCTsyPacketServicesFU::DriverModifyActiveContextL(TInt8 aRejectionCode, TInt aError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName modifyActiveContextName;
	modifyActiveContextName.Copy(KFirstAllocatedContextName);
	TMockLtsyData1<TInfoName>
		modifyActiveContextNameData(modifyActiveContextName);
	data.Close();
	modifyActiveContextNameData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, data, KErrNone);

	TMockLtsyData2<TInfoName, TInt8 >
		completeModifyActiveContextData(modifyActiveContextName, aRejectionCode); // Context name and rejection code
	data.Close();
	completeModifyActiveContextData.SerialiseL(data);

	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, aError, data);

	CleanupStack::PopAndDestroy(1, &data); // data
	}

void CCTsyPacketServicesFU::OpenInitialiseMbmsContextLC(RPacketService& aPacketService, RPacketMbmsContext& aPacketContext, TDes& aContextName, const TDesC& aChannelId, TInt aCompError)
	{
	aPacketContext.OpenNewContext(aPacketService, aContextName);
	CleanupClosePushL(aPacketContext);

	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC* contextNamePtr = &aContextName;
	TMockLtsyData1<TDesC*> initMbmdContextExpData(contextNamePtr);
	initMbmdContextExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId, data);
	data.Close();
	
	
	TDesC* channelPtr = const_cast<TDesC*>(&aChannelId);
	TMockLtsyData2<TDesC*,TDesC*> initMbmdContextCompData(contextNamePtr, channelPtr);
	initMbmdContextCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId, aCompError, data);
	data.Close();
	
	//initialisation of the context
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPckg(dataChannel);

	TRequestStatus reqStatus;
    aPacketContext.InitialiseContext(reqStatus, dataChannelPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), aCompError);
	ASSERT_TRUE((aCompError != KErrNone) || (dataChannel.iChannelId == aChannelId) || (dataChannel.iPort == KMBMSFirstAllocatedContextName));
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

//
// Use Cases
//


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Attach & RPacketService::Detach
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Attach(), RPacketService::Detach()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPacketServicesFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RBuf8 data;
	CleanupClosePushL(data);

	// client side test
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	//

	DriverAttachPacketServiceL();
	DriverDetachPacketServiceL();

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId);
	RPacketService::TAttachMode returnedAttachMode = RPacketService::EAttachWhenPossible;
	TMockLtsyData1<RPacketService::TAttachMode > completeGetAttachModetData(returnedAttachMode);
	data.Close();
	completeGetAttachModetData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId, KErrNone, data);

	//

	TRequestStatus requestAttachStatus;
	RPacketService::TStatus packetStatus;
	packetService.NotifyStatusChange(requestAttachStatus, packetStatus);

	TRequestStatus attachStatus;
	packetService.Attach(attachStatus);

	// Wait for Attach to complete
	User::WaitForRequest(attachStatus);
	ASSERT_EQUALS(KErrNone, attachStatus.Int());
	// Wait for notification to complete
	User::WaitForRequest(requestAttachStatus);
	ASSERT_EQUALS(RPacketService::EStatusAttached, packetStatus);

	RPacketService::TStatus getStatus;
	ASSERT_EQUALS(KErrNone, packetService.GetStatus(getStatus));
	ASSERT_EQUALS(RPacketService::EStatusAttached, getStatus);

	TRequestStatus requestDetachStatus;
	packetService.NotifyStatusChange(requestDetachStatus, packetStatus);
	// Detach and wait for request
	TRequestStatus detachStatus;
	packetService.Detach(detachStatus);

	// Wait for Detach to complete
	User::WaitForRequest(detachStatus);

	// Wait for notification to complete
	User::WaitForRequest(requestDetachStatus);
	ASSERT_EQUALS(KErrNone, detachStatus.Int());
	ASSERT_EQUALS(RPacketService::EStatusUnattached, packetStatus);
	ASSERT_EQUALS(KErrNone, packetService.GetStatus(getStatus));
	ASSERT_EQUALS(RPacketService::EStatusUnattached, getStatus);

	packetService.NotifyStatusChange(requestAttachStatus, packetStatus);
	
	TRequestStatus attachModeStatus;
	RPacketService::TAttachMode completedAttachMode;
	packetService.GetAttachMode(attachModeStatus, completedAttachMode);
	User::WaitForRequest(attachModeStatus);
	ASSERT_EQUALS(KErrNone, attachModeStatus.Int());
	ASSERT_EQUALS(RPacketService::EAttachWhenPossible, completedAttachMode);

	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, packetService, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for activation and QoS configuration for R97.
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewContext(), RPacketContext::InitialiseContext(), RPacketContext::SetConfig(), RPacketQoS::OpenNewQoS(), RPacketQoS::SetProfileParameters(), RPacketQoS::NotifyProfileChanged(), RPacketContext::Activate(), RPacketContext::NotifyStatusChange() for R97
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPacketServicesFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);

	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS setConfigGPRS;
	setConfigGPRS.iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRS.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRS.iPdpAddress.Copy(KPdpAddGPRS);
	setConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigGPRS.iProtocolConfigOption.iId = KRequestID;
	setConfigGPRS.iProtocolConfigOption.iMiscBuffer.Copy(KDummyMiscBuffer);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRS(setConfigGPRS);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfigGPRS.iProtocolConfigOption, pcoBuffer);
	setConfigGPRS.iProtocolConfigOption.iId = KRequestID;
	
	//
	// GPRS Configuration
	DriverInitialiseContextL();
	
	DriverSetConfigContextL(contextId, setConfigGPRS.iAccessPointName, setConfigGPRS.iPdpType, setConfigGPRS.iPdpAddress, pcoBuffer);
	
	RPacketQoS::TQoSGPRSRequested qosExpectedRequestedGPRSProfile;
	qosExpectedRequestedGPRSProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iReqDelay = RPacketQoS::EDelayClass1;
	qosExpectedRequestedGPRSProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iMinReliability = RPacketQoS::EReliabilityClass1;
	qosExpectedRequestedGPRSProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile.iMinMeanThroughput = RPacketQoS::EMeanThroughput500;

	RBuf8 data;
	CleanupClosePushL(data);
	
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextId,qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextId);
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	// Activate
	TMockLtsyData1<TInfoName> expectedActivateContextNameData(contextId);
	data.Close();
	expectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);

	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
		completedActivateContextNameData(contextId, setConfigGPRS.iAccessPointName, setConfigGPRS.iPdpType);
	data.Close();
	completedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);

	RPacketQoS::TQoSGPRSNegotiated completedProfile;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated> expLtsyQosProfileChangedInd(contextId, completedProfile);
	data.Close();
	expLtsyQosProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS setConfigGPRSActive;
	setConfigGPRSActive.iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRSActive.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRSActive.iPdpCompression = 0;
	setConfigGPRSActive.iPdpAddress.Copy(KPdpAddGPRSActive);
	setConfigGPRSActive.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRSActive.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRSActive.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRSActive.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRSActive.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					expLtsyProfileChangedInd(contextId, setConfigGPRSActive.iAccessPointName,
					setConfigGPRSActive.iPdpAddress, setConfigGPRSActive.iPdpCompression, pcoBuffer);
	data.Close();
	expLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);

	TMockLtsyData1<TInfoName> deactivateContextNameData(contextId);
	data.Close();
	deactivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, KErrNone, data);
	
	DriverDeleteContextL(contextId);

	//

	// client side test
	RPacketContext packetContext;

	// Open a new context
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // now deprecated

	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS);
	
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());

	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	//
    // SetProfileParameters for GPRS Configurations
    //

	RPacketQoS::TQoSGPRSRequested qosGPRSRequestedProfile;
	qosGPRSRequestedProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iReqDelay = RPacketQoS::EDelayClass1;
	qosGPRSRequestedProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iReqReliability =  RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iMinReliability = RPacketQoS::EReliabilityClass1;
	qosGPRSRequestedProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile.iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS(qosGPRSRequestedProfile);
	
	TRequestStatus notifyProfileChangeStatus;
	RPacketQoS::TQoSGPRSNegotiated profile;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosGPRSNegotiatedProfilePk(profile);
	packetQoS.NotifyProfileChanged(notifyProfileChangeStatus, qosGPRSNegotiatedProfilePk);
	
	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	//

	RPacketContext::TContextConfigGPRS setConfigGPRSActiveNotifyConfigChanged;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifyConfigChanged(setConfigGPRSActiveNotifyConfigChanged);

	TRequestStatus contextConfigStatus;
	packetContext.NotifyConfigChanged(contextConfigStatus, pckgConfigGPRSNotifyConfigChanged);

	TRequestStatus contextActivatingRequestStatus;
	RPacketContext::TContextStatus contextStatus;
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);

	TRequestStatus activateStatus;
	packetContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	User::WaitForRequest(contextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, activateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);

	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyProfileChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyProfileChangeStatus.Int());

	// Status change from RPacketContext::EStatusActivating to RPacketContext::EStatusActive
	TRequestStatus contextActivateRequestStatus;
	RPacketContext::TContextStatus contextActiveStatus;
	packetContext.NotifyStatusChange(contextActivateRequestStatus, contextActiveStatus);

	// Complete NotifyConfigChanged	
	User::WaitForRequest(contextActivateRequestStatus);	
	ASSERT_EQUALS(KErrNone, contextActivateRequestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextActiveStatus);
	
	TRequestStatus deactivateContextStatus;
	packetContext.Deactivate(deactivateContextStatus);
	User::WaitForRequest(deactivateContextStatus);
	ASSERT_EQUALS(KErrNone, deactivateContextStatus.Int())

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for activation and QoS configuration for R99.
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewContext(), RPacketContext::InitialiseContext(), RPacketContext::SetConfig(), RPacketQoS::OpenNewQoS(), RPacketQoS::SetProfileParameters(), RPacketQoS::NotifyProfileChanged(), RPacketContext::Activate(), RPacketContext::NotifyStatusChange() for R99
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPacketServicesFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);

	// Prepare context configuration for R4R99
	RPacketContext::TContextConfigR99_R4 setConfig99;
	setConfig99.iAccessPointName.Copy(KPointName99);
	setConfig99.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfig99.iPdpAddress.Copy(KPdpAdd99);
	setConfig99.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfig99.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfig99.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfig99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfig99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfig99.iProtocolConfigOption.iId = KRequestID;
	TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig99(setConfig99);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfig99.iProtocolConfigOption, pcoBuffer);
	setConfig99.iProtocolConfigOption.iId = KRequestID;

	//
	// 99 Configuration
	RPacketQoS::TQoSR99_R4Requested qosExpectedRequested99Profile;

	DriverInitialiseContextL();
	DriverSetConfigContextL(contextId, setConfig99.iAccessPointName, setConfig99.iPdpType, setConfig99.iPdpAddress, pcoBuffer);
	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Requested> packetSetConfig99Data(contextId,qosExpectedRequested99Profile);
	data.Close();
	packetSetConfig99Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdp99ContextNameData(contextId);
	callbackSetPdp99ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);

	//Activate
	TMockLtsyData1<TInfoName> expectedActivateContextNameData(contextId);
	data.Close();
	expectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);

	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
		completedActivateContextNameData(contextId, setConfig99.iAccessPointName, setConfig99.iPdpType);
	data.Close();
	completedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);

	RPacketQoS::TQoSR99_R4Negotiated completedProfile;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Negotiated> expLtsyQosProfileChangedInd(contextId, completedProfile);
	data.Close();
	expLtsyQosProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	// Prepare context configuration for 99
	RPacketContext::TContextConfigR99_R4 setConfig99Active;
	setConfig99Active.iAccessPointName.Copy(KPointName99);
	setConfig99Active.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfig99Active.iPdpAddress.Copy(KAddress);
	TUint pdpCompression = 0;

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					expLtsyProfileChangedInd(contextId, setConfig99Active.iAccessPointName,
					setConfig99Active.iPdpAddress, pdpCompression, pcoBuffer);
	data.Close();
	expLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);

	

	//

	// client side test
	RPacketContext packetContext;

	// Open a new context
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated

	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfig99);
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());

	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	//
    // SetProfileParameters for 99 Configurations
    //

	RPacketQoS::TQoSR99_R4Requested qos99RequestedProfile;
	TPckg< RPacketQoS::TQoSR99_R4Requested > profilePckg99(qos99RequestedProfile);

	
	//

	TRequestStatus notifyProfileChangeStatus;
	RPacketQoS::TQoSR99_R4Negotiated profile;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> qos99NegotiatedProfilePk(profile);
	packetQoS.NotifyProfileChanged(notifyProfileChangeStatus, qos99NegotiatedProfilePk);
	
	TRequestStatus requestStatus99;
	packetQoS.SetProfileParameters(requestStatus99, profilePckg99);
	User::WaitForRequest(requestStatus99);
	ASSERT_EQUALS(KErrNone, requestStatus99.Int());

	RPacketContext::TContextConfigR99_R4 setConfig99ActiveNotifyConfigChanged;
	TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig99NotifyConfigChanged(setConfig99ActiveNotifyConfigChanged);

	TRequestStatus contextConfigStatus;
	packetContext.NotifyConfigChanged(contextConfigStatus, pckgConfig99NotifyConfigChanged);

	TRequestStatus contextActivatingRequestStatus;
	RPacketContext::TContextStatus contextStatus;
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);

	TRequestStatus activateStatus;
	packetContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	User::WaitForRequest(contextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, activateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);

	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyProfileChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyProfileChangeStatus.Int());

	// Status change from RPacketContext::EStatusActivating to RPacketContext::EStatusActive
	TRequestStatus contextActivateRequestStatus;
	RPacketContext::TContextStatus contextActiveStatus;
	packetContext.NotifyStatusChange(contextActivateRequestStatus, contextActiveStatus);

	// Complete NotifyConfigChanged
	//User::WaitForRequest(contextConfigStatus);
	User::WaitForRequest(contextActivateRequestStatus);
	//ASSERT_EQUALS(KErrNone, contextConfigStatus.Int());
	ASSERT_EQUALS(KErrNone, contextActivateRequestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextActiveStatus);

	DriverDeleteContextL(contextId);
	packetQoS.Close();
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for modification of an existing context
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::SetConfig(), RPacketContext::ModifyActiveContext()
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyPacketServicesFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	// Prepare context configuration fo GPRS
	RPacketContext::TContextConfigGPRS setConfigGPRS;
	setConfigGPRS.iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRS.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRS.iPdpAddress.Copy(KPdpAddGPRS);
	setConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigGPRS.iProtocolConfigOption.iId = KRequestID;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRS(setConfigGPRS);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer gprsMiscBuffer;
	gprsMiscBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfigGPRS.iProtocolConfigOption, gprsMiscBuffer);
	setConfigGPRS.iProtocolConfigOption.iId = KRequestID;

	//

	DriverInitialiseContextL();

	// GPRS Configuration
	TInfoName contextName;
	contextName.Copy(KFirstAllocatedContextName);

	DriverSetConfigContextL(contextName, setConfigGPRS.iAccessPointName, setConfigGPRS.iPdpType,
			setConfigGPRS.iPdpAddress, gprsMiscBuffer);

	RPacketQoS::TQoSGPRSRequested qosExpectedRequestedGPRSProfile;

	qosExpectedRequestedGPRSProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;

	RBuf8 data;
	CleanupClosePushL(data);
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextName,
			qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextName);
	data.Close();
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	//Activate
	TMockLtsyData1<TInfoName> expectedActivateContextNameData(contextName);
	data.Close();
	expectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);

	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
	completedActivateContextNameData(contextName, setConfigGPRS.iAccessPointName, setConfigGPRS.iPdpType);
	data.Close();
	completedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);

	RPacketQoS::TQoSGPRSNegotiated completedProfile;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated>
				expLtsyQosProfileChangedInd(contextName, completedProfile);
	data.Close();
	expLtsyQosProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS setConfigGPRSActive;
	setConfigGPRSActive.iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRSActive.iPdpType = RPacketContext::EPdpTypeIPv6;
	setConfigGPRSActive.iPdpAddress.Copy(KPdpAddGPRS);
	setConfigGPRSActive.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRSActive.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRSActive.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRSActive.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRSActive.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					expLtsyProfileChangedInd(contextName, setConfigGPRS.iAccessPointName,
							setConfigGPRS.iPdpAddress, setConfigGPRS.iPdpCompression, gprsMiscBuffer);
	data.Close();
	expLtsyProfileChangedInd.SerialiseL(data);

	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);


	DriverSetConfigContextL(contextName, setConfigGPRSActive.iAccessPointName, setConfigGPRSActive.iPdpType,
			setConfigGPRSActive.iPdpAddress, gprsMiscBuffer);

	DriverModifyActiveContextL(KRejectionCodePass, KErrNone);
	DriverModifyActiveContextL(KRejectionCodeFail, KErrGeneral);

	TMockLtsyData1<TInfoName> deactivateContextNameData(contextName);
	data.Close();
	deactivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, KErrNone, data);

	DriverDeleteContextL(contextName);

	//

	// client side test
	RPacketContext packetContext;

	// Open a new context
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated

	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS);

	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());

	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	//
    // SetProfileParameters for GPRS Configurations
    //

	RPacketQoS::TQoSGPRSRequested qosGPRSRequestedProfile;
	qosGPRSRequestedProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS(qosGPRSRequestedProfile);

	TRequestStatus notifyProfileChangeStatus;
	RPacketQoS::TQoSGPRSNegotiated profile;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosGPRSNegotiatedProfilePk(profile);
	packetQoS.NotifyProfileChanged(notifyProfileChangeStatus, qosGPRSNegotiatedProfilePk);

	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	//

	RPacketContext::TContextConfigGPRS setConfigGPRSActiveNotifyConfigChanged;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifyConfigChanged(setConfigGPRSActiveNotifyConfigChanged);

	TRequestStatus contextConfigStatus;
	packetContext.NotifyConfigChanged(contextConfigStatus, pckgConfigGPRSNotifyConfigChanged);

	TRequestStatus contextActivatingRequestStatus;
	RPacketContext::TContextStatus contextStatus;
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);

	TRequestStatus activateStatus;
	packetContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	User::WaitForRequest(contextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, activateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);

	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyProfileChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyProfileChangeStatus.Int());

	// Status change from RPacketContext::EStatusActivating to RPacketContext::EStatusActive
	TRequestStatus contextActivateRequestStatus;
	RPacketContext::TContextStatus contextActiveStatus;
	packetContext.NotifyStatusChange(contextActivateRequestStatus, contextActiveStatus);

	User::WaitForRequest(contextActivateRequestStatus);
	ASSERT_EQUALS(KErrNone, contextActivateRequestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextActiveStatus);

	// Prepare context configuration for GPRS
	setConfigGPRS.iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
	setConfigGPRS.iPdpAddress.Copy(KPdpAddGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRS2(setConfigGPRS);

	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS2);
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());

	/* Request modification, modification should be successful */
	TRequestStatus modifyActiveContextStatus;
	packetContext.ModifyActiveContext(modifyActiveContextStatus);
	User::WaitForRequest(modifyActiveContextStatus);
	ASSERT_EQUALS(KErrNone, modifyActiveContextStatus.Int());

	/* Request second modification, should fail */
	packetContext.ModifyActiveContext(modifyActiveContextStatus);
	User::WaitForRequest(modifyActiveContextStatus);
	ASSERT_EQUALS(KErrGeneral, modifyActiveContextStatus.Int());

	TRequestStatus deactivateContextStatus;
	packetContext.Deactivate(deactivateContextStatus);
	User::WaitForRequest(deactivateContextStatus);
	ASSERT_EQUALS(KErrNone, deactivateContextStatus.Int())

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();
	
	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for activation of two primary contexts (QoS configuration provided only for the first context).
	@SYMTestPriority High
	@SYMTestActions Invokes RPacketContext::OpenNewContext(), RPacketContext::InitialiseContext(), RPacketContext::SetConfig(), RPacketQoS::OpenNewQoS(), RPacketQoS::SetProfileParameters(), RPacketQoS::NotifyProfileChanged(), RPacketContext::Activate(), RPacketContext::NotifyStatusChange() for R97
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUseCase0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	RPacketContext packetContext;

	// Open the first primary context
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);
	
	// QoS for first context
	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	AssertMockLtsyStatusL();
	
	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* setConfigGPRS = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setConfigGPRS);
	setConfigGPRS->iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRS->iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRS->iPdpAddress.Copy(KPdpAddGPRS);
	setConfigGPRS->iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRS->iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigGPRS->iProtocolConfigOption.iId = KRequestID;
	setConfigGPRS->iProtocolConfigOption.iMiscBuffer.Copy(KDummyMiscBuffer);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRS(*setConfigGPRS);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfigGPRS->iProtocolConfigOption, pcoBuffer);
	setConfigGPRS->iProtocolConfigOption.iId = KRequestID;

	// Inititialise context
	DriverInitialiseContextL();
	
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	
	AssertMockLtsyStatusL();

	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated
		
	DriverSetConfigContextL(contextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType, setConfigGPRS->iPdpAddress, pcoBuffer);
	
	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS);
	User::WaitForRequest(setConfigStatus);
	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());
	
	RPacketQoS::TQoSGPRSRequested* qosExpectedRequestedGPRSProfile = new (ELeave) RPacketQoS::TQoSGPRSRequested();
	CleanupStack::PushL(qosExpectedRequestedGPRSProfile);
    qosExpectedRequestedGPRSProfile->iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile->iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile->iReqDelay = RPacketQoS::EDelayClass1;
	qosExpectedRequestedGPRSProfile->iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile->iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile->iMinReliability = RPacketQoS::EReliabilityClass1;
	qosExpectedRequestedGPRSProfile->iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile->iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile->iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;

	RBuf8 data;
	CleanupClosePushL(data);

	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextId, *qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextId);
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	//
    // SetProfileParameters for GPRS Configurations
    //

	RPacketQoS::TQoSGPRSRequested* qosGPRSRequestedProfile = new (ELeave) RPacketQoS::TQoSGPRSRequested();
	CleanupStack::PushL(qosGPRSRequestedProfile);
	qosGPRSRequestedProfile->iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile->iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile->iReqDelay = RPacketQoS::EDelayClass1;
	qosGPRSRequestedProfile->iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile->iReqReliability =  RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile->iMinReliability = RPacketQoS::EReliabilityClass1;
	qosGPRSRequestedProfile->iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile->iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile->iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS(*qosGPRSRequestedProfile);

	TRequestStatus notifyProfileChangeStatus;
	RPacketQoS::TQoSGPRSNegotiated profile;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosGPRSNegotiatedProfilePk(profile);
	packetQoS.NotifyProfileChanged(notifyProfileChangeStatus, qosGPRSNegotiatedProfilePk);
	
	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);

	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	TRequestStatus contextConfigStatus;
	RPacketContext::TContextConfigGPRS setConfigGPRSActiveNotifyConfigChanged;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifyConfigChanged(setConfigGPRSActiveNotifyConfigChanged);
	packetContext.NotifyConfigChanged(contextConfigStatus, pckgConfigGPRSNotifyConfigChanged);
	
	TRequestStatus contextActivatingRequestStatus;
	RPacketContext::TContextStatus contextStatus;
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);
	
	TMockLtsyData1<TInfoName> expectedActivateContextNameData(contextId);
	data.Close();
	expectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);
				
	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
		completedActivateContextNameData(contextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType);
	data.Close();
	completedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);
	
	RPacketQoS::TQoSGPRSNegotiated completedProfile;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated>
					expLtsyQosProfileChangedInd(contextId, completedProfile);
	data.Close();
	expLtsyQosProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* setConfigGPRSActive = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setConfigGPRSActive);
	setConfigGPRSActive->iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRSActive->iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRSActive->iPdpCompression = 0;
	setConfigGPRSActive->iPdpAddress.Copy(KPdpAddGPRSActive);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					expLtsyProfileChangedInd(contextId, setConfigGPRSActive->iAccessPointName,
					setConfigGPRSActive->iPdpAddress, setConfigGPRSActive->iPdpCompression, pcoBuffer);
	data.Close();
	expLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);
	
	// Activate first primary context
	TRequestStatus activateStatus;
	packetContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	ASSERT_EQUALS(KErrNone, activateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);
	
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);
		
	User::WaitForRequest(contextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, contextConfigStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextStatus);

	AssertMockLtsyStatusL();
	
	//

	// open the second primary context
	RPacketContext secondPacketContext;

	TInfoName secondContextId;
	err = secondPacketContext.OpenNewContext(packetService, secondContextId);
	CleanupClosePushL(secondPacketContext);
	ASSERT_EQUALS(KErrNone, err);
	
	TInfoName emptyContextId;
	emptyContextId.Copy(KDefaultContextName);
	

	DriverInitialiseContextL(secondContextId, emptyContextId);
	
	RPacketContext::TDataChannelV2 secondDataChannel;
	RPacketContext::TDataChannelV2Pckg secondDataChannelPk(secondDataChannel);
	TRequestStatus secondInitialiseContextStatus;

	// Initialize the second primary context
	secondPacketContext.InitialiseContext(secondInitialiseContextStatus, secondDataChannelPk);
	User::WaitForRequest(secondInitialiseContextStatus);
	
	ASSERT_EQUALS(KErrNone, secondInitialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName); 	
	ASSERT_TRUE(secondDataChannel.iChannelId == KSecondaryContextChannelId); // Now deprecated
	
	AssertMockLtsyStatusL();
	
	TRequestStatus secondcontextConfigStatus;
	secondPacketContext.NotifyConfigChanged(secondcontextConfigStatus, pckgConfigGPRSNotifyConfigChanged);
	
	TRequestStatus secondcontextActivatingRequestStatus;
	RPacketContext::TContextStatus secondcontextStatus;
	secondPacketContext.NotifyStatusChange(secondcontextActivatingRequestStatus, secondcontextStatus);

	TMockLtsyData1<TInfoName> secondexpectedActivateContextNameData(secondContextId);
	data.Close();
	secondexpectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);
				
	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
	secondcompletedActivateContextNameData(secondContextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType);
	data.Close();
	secondcompletedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);

	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* secondSetConfigGPRSActive = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(secondSetConfigGPRSActive);
	secondSetConfigGPRSActive->iAccessPointName.Copy(KPointNameGPRS);
	secondSetConfigGPRSActive->iPdpType = RPacketContext::EPdpTypeIPv4;
	secondSetConfigGPRSActive->iPdpCompression = 0;
	secondSetConfigGPRSActive->iPdpAddress.Copy(KPdpAddGPRSActive);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					secondexpLtsyProfileChangedInd(secondContextId, secondSetConfigGPRSActive->iAccessPointName,
					secondSetConfigGPRSActive->iPdpAddress, secondSetConfigGPRSActive->iPdpCompression, pcoBuffer);
	data.Close();
	secondexpLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);

	// Activate second primary context
	TRequestStatus secondactivateStatus;
	secondPacketContext.Activate(secondactivateStatus);
	User::WaitForRequest(secondactivateStatus);
	ASSERT_EQUALS(KErrNone, secondactivateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, secondcontextStatus);
	
	secondPacketContext.NotifyStatusChange(secondcontextActivatingRequestStatus, secondcontextStatus);
		
	User::WaitForRequest(secondcontextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, secondcontextConfigStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, secondcontextStatus);

	DriverDeleteContextL(contextId);
	DriverDeleteContextL(secondContextId);
	
	packetQoS.Close();
	packetContext.Close();
	packetService.Close();
	secondPacketContext.Close();
	
	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(11, this); // secondSetConfigGPRSActive, secondPacketContext, data, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for activation of a primary and secondary contexts. 
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewContext(), RPacketContext::InitialiseContext(), RPacketContext::SetDefaultContextParams(), RPacketContext::OpenNewSecondaryContext(), RPacketContext::AddPacketFilter() for R97
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUseCase0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
		
	RPacketService packetService;
	RPacketContext packetContext;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	// Open the primary context
	TInfoName contextId;
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);
	
	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	AssertMockLtsyStatusL();
	
	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* setConfigGPRS = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setConfigGPRS);
	setConfigGPRS->iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRS->iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRS->iPdpAddress.Copy(KPdpAddGPRS);
	setConfigGPRS->iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRS->iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigGPRS->iProtocolConfigOption.iId = KRequestID;
	setConfigGPRS->iProtocolConfigOption.iMiscBuffer.Copy(KDummyMiscBuffer);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRS(*setConfigGPRS);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfigGPRS->iProtocolConfigOption, pcoBuffer);
	setConfigGPRS->iProtocolConfigOption.iId = KRequestID;

	DriverInitialiseContextL();
	
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	// Initialise primary context
	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated
	
	AssertMockLtsyStatusL();
		
	// Setting default GPRS context parameters. 
	RPacketContext::TContextConfigGPRS expContextConfigGprs;
	expContextConfigGprs.iAccessPointName = KDefaultAccessPoint;
	expContextConfigGprs.iPdpType = KDefaultPdpType;

	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expLtsyGprsData(expContextConfigGprs);
	RBuf8 data;
	CleanupClosePushL(data);
	
	expLtsyGprsData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, KErrNone);
	
	RPacketContext::TContextConfigGPRS defaultGprsContextConfig;
	defaultGprsContextConfig.iAccessPointName = KDefaultAccessPoint;
	defaultGprsContextConfig.iPdpType = KDefaultPdpType;
	TPckg<RPacketContext::TContextConfigGPRS> defaultContextGprsConfigPckg(defaultGprsContextConfig);
	TRequestStatus setDefaultContextParamsStatus;
	packetService.SetDefaultContextParams(setDefaultContextParamsStatus, defaultContextGprsConfigPckg);
	User::WaitForRequest(setDefaultContextParamsStatus);

    ASSERT_EQUALS(KErrNone, setDefaultContextParamsStatus.Int());
    
	AssertMockLtsyStatusL();
	
	DriverSetConfigContextL(contextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType, setConfigGPRS->iPdpAddress, pcoBuffer);
	
	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS);
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());
		
	
	//
	// SetProfileParameters for GPRS Configurations
	//
	
	RPacketQoS::TQoSGPRSRequested* qosExpectedRequestedGPRSProfile = new (ELeave) RPacketQoS::TQoSGPRSRequested();
    qosExpectedRequestedGPRSProfile->iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile->iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile->iReqDelay = RPacketQoS::EDelayClass1;
	qosExpectedRequestedGPRSProfile->iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile->iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile->iMinReliability = RPacketQoS::EReliabilityClass1;
	qosExpectedRequestedGPRSProfile->iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile->iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile->iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;

	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextId, *qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextId);
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	RPacketQoS::TQoSGPRSRequested*  qosGPRSRequestedProfile = new (ELeave) RPacketQoS::TQoSGPRSRequested();
	qosGPRSRequestedProfile->iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile->iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile->iReqDelay = RPacketQoS::EDelayClass1;
	qosGPRSRequestedProfile->iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile->iReqReliability =  RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile->iMinReliability = RPacketQoS::EReliabilityClass1;
	qosGPRSRequestedProfile->iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile->iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput; 
	qosGPRSRequestedProfile->iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS(*qosGPRSRequestedProfile);
	
	TRequestStatus notifyProfileChangeStatus;
	RPacketQoS::TQoSGPRSNegotiated profile;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosGPRSNegotiatedProfilePk(profile);
	packetQoS.NotifyProfileChanged(notifyProfileChangeStatus, qosGPRSNegotiatedProfilePk);

	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
		
	AssertMockLtsyStatusL();
	
	TRequestStatus contextConfigStatus;
	RPacketContext::TContextConfigGPRS* setConfigGPRSActiveNotifyConfigChanged = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setConfigGPRSActiveNotifyConfigChanged);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifyConfigChanged(*setConfigGPRSActiveNotifyConfigChanged);
	packetContext.NotifyConfigChanged(contextConfigStatus, pckgConfigGPRSNotifyConfigChanged);
	
	TRequestStatus contextActivatingRequestStatus;
	RPacketContext::TContextStatus contextStatus;
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);
			
	TMockLtsyData1<TInfoName> expectedActivateContextNameData(contextId);
	data.Close();
	expectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);
				
	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
		completedActivateContextNameData(contextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType);
	data.Close();
	completedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);
	
	RPacketQoS::TQoSGPRSNegotiated completedProfile;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated>
					expLtsyQosProfileChangedInd(contextId, completedProfile);
	data.Close();
	expLtsyQosProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);
	
	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* setConfigGPRSActive  = new (ELeave) RPacketContext::TContextConfigGPRS();
	setConfigGPRSActive->iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRSActive->iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRSActive->iPdpCompression = 0;
	setConfigGPRSActive->iPdpAddress.Copy(KPdpAddGPRSActive);	
	setConfigGPRSActive->iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRSActive->iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRSActive->iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRSActive->iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigGPRSActive->iProtocolConfigOption.iId = KRequestID;

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					expLtsyProfileChangedInd(contextId, setConfigGPRSActive->iAccessPointName,
					setConfigGPRSActive->iPdpAddress, setConfigGPRSActive->iPdpCompression, pcoBuffer);
	data.Close();
	expLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);
	
	// Activate primary context
	TRequestStatus activateStatus;
	packetContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	
	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyProfileChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyProfileChangeStatus.Int());
	
	User::WaitForRequest(contextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, activateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);
	
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);
	
	User::WaitForRequest(contextActivatingRequestStatus);
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextStatus);

	AssertMockLtsyStatusL();
	
	//Open Secondary Packet Context
	TInfoName secondaryContextId;
	RPacketContext secondaryContext;	
	err = secondaryContext.OpenNewSecondaryContext(packetService, contextId, secondaryContextId);
	CleanupClosePushL(secondaryContext);
	ASSERT_EQUALS(KErrNone, err);
	
	AssertMockLtsyStatusL();
				
	DriverInitialiseContextL(contextId, secondaryContextId);

	RPacketContext::TDataChannelV2* secondDataChannel = new (ELeave) RPacketContext::TDataChannelV2();
	RPacketContext::TDataChannelV2Pckg secondDataChannelPk(*secondDataChannel);
	TRequestStatus secondInitialiseContextStatus;
	
	// Initialise secondary context
	secondaryContext.InitialiseContext(secondInitialiseContextStatus, secondDataChannelPk);
	User::WaitForRequest(secondInitialiseContextStatus);	
	ASSERT_EQUALS(KErrNone, secondInitialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName); 	
	ASSERT_TRUE(secondDataChannel->iChannelId == KSecondaryContextChannelId); // Now deprecated
				
	AssertMockLtsyStatusL();
	
	// Open QoS for secondaty context
	RPacketQoS secondaryPacketQoS;
	secondaryPacketQoS.OpenNewQoS(secondaryContext, secondaryContextId);
	CleanupClosePushL(secondaryPacketQoS);
	
	RPacketQoS::TQoSGPRSRequested* qosExpectedRequestedGPRSProfileSecondaryContext = new (ELeave) RPacketQoS::TQoSGPRSRequested();	
	qosExpectedRequestedGPRSProfileSecondaryContext->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSDataSecondaryContext(secondaryContextId, *qosExpectedRequestedGPRSProfileSecondaryContext);
	data.Close();
	packetSetConfigGPRSDataSecondaryContext.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameDataSecondaryContext(secondaryContextId);
	callbackSetPdpGPRSContextNameDataSecondaryContext.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	//Define qos params for secondaty context   
	RPacketQoS::TQoSGPRSRequested* qosGPRSRequestedProfilesecondaryPacket = new (ELeave) RPacketQoS::TQoSGPRSRequested();
	qosGPRSRequestedProfilesecondaryPacket->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGPRSRequestedProfilePk(*qosGPRSRequestedProfilesecondaryPacket);    
	
	//Set QoS Params GPRS for secondaty context
	TRequestStatus setProfileParametersSt;
	secondaryPacketQoS.SetProfileParameters(setProfileParametersSt, qosGPRSRequestedProfilePk);
	User::WaitForRequest(setProfileParametersSt);
	ASSERT_EQUALS(KErrNone, setProfileParametersSt.Int());
	
	AssertMockLtsyStatusL();
		
	// Configure packet filter  secondary context	    	    
    RPacketContext::TPacketFilterV2* packetFilterV2 = new (ELeave) RPacketContext::TPacketFilterV2();
    packetFilterV2->iId = 1;
    packetFilterV2->iEvaluationPrecedenceIndex = 100;
    packetFilterV2->iDestPortMax = 4001;
    packetFilterV2->iDestPortMin = 4000;        
    packetFilterV2->iIPSecSPI = 1;    
    packetFilterV2->iProtocolNumberOrNextHeader = 0;
    packetFilterV2->iSrcAddr[12] = 0;
    memcpy(&(packetFilterV2->iSrcAddr[0]), "200.12.13.19", 12);
    packetFilterV2->iSrcAddrSubnetMask[13] = 0;
    memcpy(&(packetFilterV2->iSrcAddrSubnetMask[0]), "255.255.255.0", 13);    
    packetFilterV2->iSrcPortMin = 5000;
    packetFilterV2->iSrcPortMax = 5001;
    packetFilterV2->iTOSorTrafficClass = 0;    
    RPacketContext::TPacketFilterV2Pckg packetFilterV2Pckg(*packetFilterV2);
    CArrayFixFlat<RPacketContext::TPacketFilterV2>* filterTemporaryArray = new ( ELeave ) CArrayFixFlat<RPacketContext::TPacketFilterV2>( 1 );
    CleanupStack::PushL(filterTemporaryArray);
    filterTemporaryArray->AppendL( *packetFilterV2 );

	TMockLtsyData2<TInfoName, CArrayFixFlat<RPacketContext::TPacketFilterV2>*> expectedAddFilterContextNameData(secondaryContextId, filterTemporaryArray);
	data.Close();
	expectedAddFilterContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId, data, KErrNone);
	
	TMockLtsyData1<TInfoName> callbackContextIdData(secondaryContextId);
	data.Close();
	callbackContextIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);
	
	TRequestStatus addPacketFilterStatus;
	
	// Add filter to secondary context
    secondaryContext.AddPacketFilter(addPacketFilterStatus, packetFilterV2Pckg);	    	    
	User::WaitForRequest(addPacketFilterStatus);
	ASSERT_EQUALS(KErrNone, addPacketFilterStatus.Int());
	
	AssertMockLtsyStatusL();
						
	TMockLtsyData1<TInfoName> secondaryExpectedActivateContextNameData(secondaryContextId);
	data.Close();
	secondaryExpectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);
				
	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
	secondaryCompletedActivateContextNameData(secondaryContextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType);
	data.Close();
	secondaryCompletedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);
		
	RPacketContext::TContextConfigGPRS* setSecondaryConfigGPRSActiveNotifyConfigChanged  = new (ELeave)RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setSecondaryConfigGPRSActiveNotifyConfigChanged);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifySecondaryConfigChanged(*setSecondaryConfigGPRSActiveNotifyConfigChanged);
	
	TRequestStatus secondaryContextConfigStatus;
	secondaryContext.NotifyConfigChanged(secondaryContextConfigStatus, pckgConfigGPRSNotifySecondaryConfigChanged);	
	secondaryContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);

	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* secondarySetConfigGPRSActive = new (ELeave)RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(secondarySetConfigGPRSActive);

	secondarySetConfigGPRSActive->iAccessPointName.Copy(KPointNameGPRS);
	secondarySetConfigGPRSActive->iPdpType = RPacketContext::EPdpTypeIPv4;
	secondarySetConfigGPRSActive->iPdpCompression = 1;
	secondarySetConfigGPRSActive->iPdpAddress.Copy(KPdpAddGPRSActive);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					secondaryexpLtsyProfileChangedInd(secondaryContextId, secondarySetConfigGPRSActive->iAccessPointName,
					secondarySetConfigGPRSActive->iPdpAddress, secondarySetConfigGPRSActive->iPdpCompression, pcoBuffer);
	data.Close();
	secondaryexpLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);

	TRequestStatus secondaryActivateStatus;
	
	// Activate secondary context.	
	secondaryContext.Activate(secondaryActivateStatus);
	User::WaitForRequest(secondaryActivateStatus);
	ASSERT_EQUALS(KErrNone, secondaryActivateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);
	
	AssertMockLtsyStatusL();

	RPacketContext::TContextConfigGPRS* secondarysetConfigGPRSActiveNotifyConfigChanged = new (ELeave)RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(secondarysetConfigGPRSActiveNotifyConfigChanged);
	TPckg<RPacketContext::TContextConfigGPRS> secondarypckgConfigGPRSNotifyConfigChanged(*secondarysetConfigGPRSActiveNotifyConfigChanged);
	
	secondaryContext.NotifyStatusChange(secondaryActivateStatus, contextStatus);
	
	User::WaitForRequest(secondaryActivateStatus);
	ASSERT_EQUALS(KErrNone, secondaryContextConfigStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextStatus);

	AssertMockLtsyStatusL();
		
	TMockLtsyData1<TInfoName> deactivateSecondaryContextNameData(secondaryContextId);
	
	data.Close();
	deactivateSecondaryContextNameData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, KErrNone, data);

	// Deactivate secondary context
	TRequestStatus deactivateContextStatus;
	secondaryContext.Deactivate(deactivateContextStatus);
	User::WaitForRequest(deactivateContextStatus);
	ASSERT_EQUALS(KErrNone, deactivateContextStatus.Int())
	
	AssertMockLtsyStatusL();

	TMockLtsyData1<TInfoName> deactivateContextNameData(contextId);
	data.Close();
	deactivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, KErrNone, data);
					
	// Deactivate primary context
	packetContext.Deactivate(deactivateContextStatus);
	User::WaitForRequest(deactivateContextStatus);
	ASSERT_EQUALS(KErrNone, deactivateContextStatus.Int());
	
	AssertMockLtsyStatusL();
		
	DriverDeleteContextL(secondaryContextId);
	DriverDeleteContextL(contextId);

	secondaryPacketQoS.Close();
	packetQoS.Close();
				
	secondaryContext.Close();
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(13, this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UC0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for activation and modification of a secondary context with two filters and removal of the first filter. 
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewContext(), RPacketContext::InitialiseContext(), RPacketContext::SetDefaultContextParams(), RPacketContext::OpenNewSecondaryContext(), RPacketContext::AddPacketFilter(), RPacketContext::ModifyActiveContext() for R97
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUseCase0007L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
		
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	RPacketContext packetContext;

	// Open the primary context
	TInfoName contextId;
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);
	
	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	AssertMockLtsyStatusL();
	
	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* setConfigGPRS = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setConfigGPRS);
	setConfigGPRS->iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRS->iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRS->iPdpAddress.Copy(KPdpAddGPRS);
	setConfigGPRS->iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	setConfigGPRS->iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigGPRS->iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigGPRS->iProtocolConfigOption.iId = KRequestID;
	setConfigGPRS->iProtocolConfigOption.iMiscBuffer.Copy(KDummyMiscBuffer);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRS(*setConfigGPRS);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfigGPRS->iProtocolConfigOption, pcoBuffer);
	setConfigGPRS->iProtocolConfigOption.iId = KRequestID;
	
	DriverInitialiseContextL();
		
	RPacketContext::TDataChannelV2* dataChannel = new (ELeave) RPacketContext::TDataChannelV2();
	RPacketContext::TDataChannelV2Pckg dataChannelPk(*dataChannel);
	TRequestStatus initialiseContextStatus;

	// Initialise primary context
	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel->iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel->iChannelId == KDefaultChannelId); // Now deprecated
	
	AssertMockLtsyStatusL();
	
	// Setting default GPRS context parameters. 
	RPacketContext::TContextConfigGPRS* expContextConfigGprs = new (ELeave) RPacketContext::TContextConfigGPRS();
	expContextConfigGprs->iAccessPointName = KDefaultAccessPoint;
	expContextConfigGprs->iPdpType = KDefaultPdpType;

	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expLtsyGprsData(*expContextConfigGprs);
	RBuf8 data;
	CleanupClosePushL(data);
	
	expLtsyGprsData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, KErrNone);
	
	RPacketContext::TContextConfigGPRS* defaultGprsContextConfig = new (ELeave) RPacketContext::TContextConfigGPRS();
	defaultGprsContextConfig->iAccessPointName = KDefaultAccessPoint;
	defaultGprsContextConfig->iPdpType = KDefaultPdpType;
	TPckg<RPacketContext::TContextConfigGPRS> defaultContextGprsConfigPckg(*defaultGprsContextConfig);
	TRequestStatus setDefaultContextParamsStatus;
	packetService.SetDefaultContextParams(setDefaultContextParamsStatus, defaultContextGprsConfigPckg);
	User::WaitForRequest(setDefaultContextParamsStatus);

    ASSERT_EQUALS(KErrNone, setDefaultContextParamsStatus.Int());
    
	AssertMockLtsyStatusL();
		
	DriverSetConfigContextL(contextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType, setConfigGPRS->iPdpAddress, pcoBuffer);
	
	// Set configuration for primary context
	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS);
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());
	
	AssertMockLtsyStatusL();		
	
	RPacketQoS::TQoSGPRSRequested* qosExpectedRequestedGPRSProfile = new (ELeave) RPacketQoS::TQoSGPRSRequested();
    qosExpectedRequestedGPRSProfile->iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile->iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile->iReqDelay = RPacketQoS::EDelayClass1;
	qosExpectedRequestedGPRSProfile->iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile->iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile->iMinReliability = RPacketQoS::EReliabilityClass1;
	qosExpectedRequestedGPRSProfile->iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile->iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile->iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;

	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextId, *qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);
	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextId);
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	//
    // SetProfileParameters for GPRS Configurations
    //

	RPacketQoS::TQoSGPRSRequested* qosGPRSRequestedProfile = new (ELeave)RPacketQoS::TQoSGPRSRequested(); 
	qosGPRSRequestedProfile->iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile->iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile->iReqDelay = RPacketQoS::EDelayClass1;
	qosGPRSRequestedProfile->iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile->iReqReliability =  RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile->iMinReliability = RPacketQoS::EReliabilityClass1;
	qosGPRSRequestedProfile->iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile->iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile->iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS(*qosGPRSRequestedProfile);
	
	TRequestStatus notifyProfileChangeStatus;
	RPacketQoS::TQoSGPRSNegotiated profile;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosGPRSNegotiatedProfilePk(profile);
	packetQoS.NotifyProfileChanged(notifyProfileChangeStatus, qosGPRSNegotiatedProfilePk);

	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	TRequestStatus contextConfigStatus;
	RPacketContext::TContextConfigGPRS* setConfigGPRSActiveNotifyConfigChanged = new (ELeave) RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setConfigGPRSActiveNotifyConfigChanged);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifyConfigChanged(*setConfigGPRSActiveNotifyConfigChanged);
	packetContext.NotifyConfigChanged(contextConfigStatus, pckgConfigGPRSNotifyConfigChanged);
		
	TRequestStatus contextActivatingRequestStatus;
	RPacketContext::TContextStatus contextStatus;
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);

	TMockLtsyData1<TInfoName> expectedActivateContextNameData(contextId);
	data.Close();
	expectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);
				
	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
		completedActivateContextNameData(contextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType);
	data.Close();
	completedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);
	
	RPacketQoS::TQoSGPRSNegotiated completedProfile;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated>
					expLtsyQosProfileChangedInd(contextId, completedProfile);
	data.Close();
	expLtsyQosProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
		
	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* setConfigGPRSActive = new (ELeave)RPacketContext::TContextConfigGPRS();
	setConfigGPRSActive->iAccessPointName.Copy(KPointNameGPRS);
	setConfigGPRSActive->iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigGPRSActive->iPdpCompression = 0;
	setConfigGPRSActive->iPdpAddress.Copy(KPdpAddGPRSActive);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					expLtsyProfileChangedInd(contextId, setConfigGPRSActive->iAccessPointName,
					setConfigGPRSActive->iPdpAddress, setConfigGPRSActive->iPdpCompression, pcoBuffer);
	data.Close();
	expLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);

	// Activate primary context
	TRequestStatus activateStatus;
	packetContext.Activate(activateStatus);
	User::WaitForRequest(activateStatus);
	ASSERT_EQUALS(KErrNone, activateStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);
	
	packetContext.NotifyStatusChange(contextActivatingRequestStatus, contextStatus);
		
	User::WaitForRequest(contextActivatingRequestStatus);	
	User::WaitForRequest(contextConfigStatus);	
	ASSERT_EQUALS(KErrNone, contextConfigStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextStatus);
		
	AssertMockLtsyStatusL();
	
	//Open Secondary Packet Context
	TInfoName secondaryContextId;
	RPacketContext secondaryContext;	
	err = secondaryContext.OpenNewSecondaryContext(packetService, contextId, secondaryContextId);
	CleanupClosePushL(secondaryContext);
	ASSERT_EQUALS(KErrNone, err);
	
	AssertMockLtsyStatusL();
				
	DriverInitialiseContextL(contextId, secondaryContextId);

	RPacketContext::TDataChannelV2*  secondDataChannel = new (ELeave) RPacketContext::TDataChannelV2();
	RPacketContext::TDataChannelV2Pckg* secondDataChannelPk = new (ELeave) RPacketContext::TDataChannelV2Pckg (*secondDataChannel);
	TRequestStatus secondInitialiseContextStatus;
	
	// Initialise secondary context
	secondaryContext.InitialiseContext(secondInitialiseContextStatus, *secondDataChannelPk);
	User::WaitForRequest(secondInitialiseContextStatus);	
	ASSERT_EQUALS(KErrNone, secondInitialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel->iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(secondDataChannel->iChannelId == KSecondaryContextChannelId); // Now deprecated
				
	AssertMockLtsyStatusL();
	
	// Open QoS for secondaty context
	RPacketQoS secondaryPacketQoS;
	secondaryPacketQoS.OpenNewQoS(secondaryContext, secondaryContextId);
	CleanupClosePushL(secondaryPacketQoS);
	
	RPacketQoS::TQoSGPRSRequested* qosExpectedRequestedGPRSProfileSecondaryContext = new (ELeave) RPacketQoS::TQoSGPRSRequested();	
	qosExpectedRequestedGPRSProfileSecondaryContext->iMinMeanThroughput = RPacketQoS::EMeanThroughput500;	
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSDataSecondaryContext(secondaryContextId, *qosExpectedRequestedGPRSProfileSecondaryContext);
	data.Close();
	packetSetConfigGPRSDataSecondaryContext.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameDataSecondaryContext(secondaryContextId);
	callbackSetPdpGPRSContextNameDataSecondaryContext.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	// Define QoS params for secondaty context   
	RPacketQoS::TQoSGPRSRequested qosGPRSRequestedProfilesecondaryPacket;
	qosGPRSRequestedProfilesecondaryPacket.iMinMeanThroughput = RPacketQoS::EMeanThroughput500;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGPRSRequestedProfilePk(qosGPRSRequestedProfilesecondaryPacket);    
	
	// Set QoS Params GPRS for secondaty context
	TRequestStatus setProfileParametersSt;
	secondaryPacketQoS.SetProfileParameters(setProfileParametersSt, qosGPRSRequestedProfilePk);
	User::WaitForRequest(setProfileParametersSt);
	ASSERT_EQUALS(KErrNone, setProfileParametersSt.Int());
	
	AssertMockLtsyStatusL();
		
    RPacketContext::TPacketFilterV2* packetFilterV2 = new (ELeave) RPacketContext::TPacketFilterV2();
    CleanupStack::PushL(packetFilterV2);
    packetFilterV2->iId = 1;
    packetFilterV2->iEvaluationPrecedenceIndex = 100;
    packetFilterV2->iDestPortMax = 4001;
    packetFilterV2->iDestPortMin = 4000;    
    RPacketContext::TPacketFilterV2Pckg packetFilterV2Pckg(*packetFilterV2);    
    CArrayFixFlat<RPacketContext::TPacketFilterV2>* filterTemporaryArray = new ( ELeave ) CArrayFixFlat<RPacketContext::TPacketFilterV2>( 1 );
    CleanupStack::PushL(filterTemporaryArray);
    filterTemporaryArray->AppendL( *packetFilterV2 );
    
	TMockLtsyData2<TInfoName, CArrayFixFlat<RPacketContext::TPacketFilterV2>*> expectedAddFilterContextNameData(secondaryContextId, filterTemporaryArray);
	data.Close();
	expectedAddFilterContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId, data, KErrNone);
	
	TMockLtsyData1<TInfoName> callbackContextIdData(secondaryContextId);
	data.Close();
	callbackContextIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);

	// Add filter to secondary filter
	TRequestStatus addPacketFilterStatus;
    secondaryContext.AddPacketFilter(addPacketFilterStatus, packetFilterV2Pckg);	    	    
	User::WaitForRequest(addPacketFilterStatus);
	ASSERT_EQUALS(KErrNone, addPacketFilterStatus.Int());
		
	AssertMockLtsyStatusL();

	// Prepare second filter
	packetFilterV2->iId = 2;
	filterTemporaryArray->AppendL(*packetFilterV2);
	
	TMockLtsyData2<TInfoName, CArrayFixFlat<RPacketContext::TPacketFilterV2>*> expectedAddSecondFilterContextNameData(secondaryContextId, filterTemporaryArray);
	data.Close();
	expectedAddSecondFilterContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId, data, KErrNone);
	
	TMockLtsyData1<TInfoName> callbackSecondContextIdData(secondaryContextId);
	data.Close();
	callbackSecondContextIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);
	
	// Add a second filter to the secondary filter
    secondaryContext.AddPacketFilter(addPacketFilterStatus, packetFilterV2Pckg);	    	    
	User::WaitForRequest(addPacketFilterStatus);
	ASSERT_EQUALS(KErrNone, addPacketFilterStatus.Int());
	
	AssertMockLtsyStatusL();
		
	TMockLtsyData1<TInfoName> secondaryExpectedActivateContextNameData(secondaryContextId);
	data.Close();
	secondaryExpectedActivateContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, data, KErrNone);				
	TMockLtsyData3<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType>
	secondaryCompletedActivateContextNameData(secondaryContextId, setConfigGPRS->iAccessPointName, setConfigGPRS->iPdpType);
	data.Close();
	secondaryCompletedActivateContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, KErrNone, data);

	RPacketContext::TContextConfigGPRS* setSecondaryConfigGPRSActiveNotifyConfigChanged = new (ELeave)RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(setSecondaryConfigGPRSActiveNotifyConfigChanged);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfigGPRSNotifySecondaryConfigChanged(*setSecondaryConfigGPRSActiveNotifyConfigChanged);
	
	TRequestStatus secondaryContextConfigStatus;
	secondaryContext.NotifyConfigChanged(secondaryContextConfigStatus, pckgConfigGPRSNotifySecondaryConfigChanged);
	
	TRequestStatus secondaryContextActivatingRequestStatus;
	RPacketContext::TContextStatus secondaryContextStatus;
	secondaryContext.NotifyStatusChange(secondaryContextActivatingRequestStatus, secondaryContextStatus);
		
	// Prepare context configuration for GPRS
	RPacketContext::TContextConfigGPRS* secondarySetConfigGPRSActive = new (ELeave)RPacketContext::TContextConfigGPRS();
	CleanupStack::PushL(secondarySetConfigGPRSActive);
	secondarySetConfigGPRSActive->iAccessPointName.Copy(KPointNameGPRS);
	secondarySetConfigGPRSActive->iPdpType = RPacketContext::EPdpTypeIPv4;
	secondarySetConfigGPRSActive->iPdpCompression = 1;
	secondarySetConfigGPRSActive->iPdpAddress.Copy(KPdpAddGPRSActive);

	TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer>
					secondaryexpLtsyProfileChangedInd(secondaryContextId, secondarySetConfigGPRSActive->iAccessPointName,
					secondarySetConfigGPRSActive->iPdpAddress, secondarySetConfigGPRSActive->iPdpCompression, pcoBuffer);
	data.Close();
	secondaryexpLtsyProfileChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId, KErrNone, data);
	
	// Activate secondary context.
	TRequestStatus secondaryActivateStatus;
	secondaryContext.Activate(secondaryActivateStatus);
	User::WaitForRequest(secondaryActivateStatus);	
	ASSERT_EQUALS(RPacketContext::EStatusActivating, secondaryContextStatus);
	
	AssertMockLtsyStatusL();
	
	secondaryContext.NotifyStatusChange(secondaryContextActivatingRequestStatus, secondaryContextStatus);	
	User::WaitForRequest(secondaryContextActivatingRequestStatus);
	ASSERT_EQUALS(KErrNone, secondaryContextConfigStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, secondaryContextStatus);
	
	AssertMockLtsyStatusL();

	// ModifyActiveContext pass test
	TInt8 KRejectionCodePass = 0;
	TInt8 KRejectionCodeFail = 1;
	
	TMockLtsyData1<TInfoName>
	modifyActiveContextNameData(secondaryContextId);
	data.Close();
	modifyActiveContextNameData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, data, KErrNone);

	TMockLtsyData2<TInfoName, TInt8> completeModifyActiveContextData(secondaryContextId, KRejectionCodePass); // Context name and rejection code
	data.Close();
	completeModifyActiveContextData.SerialiseL(data);

	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, KErrNone, data);

	// Request modification, modification should be successful 
	TRequestStatus modifyActiveContextStatus;
	secondaryContext.ModifyActiveContext(modifyActiveContextStatus);
	User::WaitForRequest(modifyActiveContextStatus);
	ASSERT_EQUALS(KErrNone, modifyActiveContextStatus.Int());
	
	AssertMockLtsyStatusL();
	
	TMockLtsyData1<TInfoName>
	failedModifyActiveContextNameData(secondaryContextId);
	data.Close();
	failedModifyActiveContextNameData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, data, KErrNone);
	TMockLtsyData2<TInfoName,  TInt8 >
	completeFailedModifyActiveContextData(secondaryContextId, KRejectionCodeFail); // Context name and rejection code
	data.Close();
	completeFailedModifyActiveContextData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, KErrGeneral, data);

	// Request second modification, should fail 
	secondaryContext.ModifyActiveContext(modifyActiveContextStatus);
	User::WaitForRequest(modifyActiveContextStatus);
	ASSERT_EQUALS(KErrGeneral, modifyActiveContextStatus.Int());
	
	AssertMockLtsyStatusL();
	
	// Remove packet filter
	TInt filterId = 2;	
	TMockLtsyData2<TInfoName, TInt > expectedRemoveSecondFilterContextNameData(secondaryContextId, filterId);
	data.Close();
	expectedRemoveSecondFilterContextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId, data, KErrNone);
	
	TMockLtsyData1<TInfoName> callbackRemoveFilterData(secondaryContextId);
	data.Close();
	callbackRemoveFilterData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);
	
	//-------------------------------------------------------------------------
	// TEST Successful completion request of
	// RPacketContext::ModifyActiveContext with removal of first filter 
 	//-------------------------------------------------------------------------		
	TRequestStatus removeFilterRequestStatus;	
	secondaryContext.RemovePacketFilter(removeFilterRequestStatus, filterId);
	User::WaitForRequest(removeFilterRequestStatus);
	ASSERT_EQUALS(KErrNone, removeFilterRequestStatus.Int());	
	AssertMockLtsyStatusL();		
	
	TMockLtsyData1<TInfoName> removeFilterModifyActiveContextNameData(secondaryContextId);
	data.Close();
	removeFilterModifyActiveContextNameData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, data, KErrNone);

	TMockLtsyData2<TInfoName,  TInt8 >	
	completeRemoveFilterModifyActiveContextData(secondaryContextId, KRejectionCodePass); // Context name and rejection code
	data.Close();
	completeRemoveFilterModifyActiveContextData.SerialiseL(data);

	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, KErrNone, data);
	
	secondaryContext.ModifyActiveContext(modifyActiveContextStatus);
	User::WaitForRequest(modifyActiveContextStatus);
	ASSERT_EQUALS(KErrNone, modifyActiveContextStatus.Int());
    	 
	AssertMockLtsyStatusL();
			
	DriverDeleteContextL(contextId);
	DriverDeleteContextL(secondaryContextId);
	
	secondaryContext.Close();
	packetContext.Close();
		
	packetQoS.Close();
	packetService.Close();
	secondaryPacketQoS.Close();
	
	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(13, this); // secondarySetConfigGPRSActive, setSecondaryConfigGPRSActiveNotifyConfigChanged, secondarysetConfigGPRSActiveNotifyConfigChanged, 
											//filterTemporaryArray, packetFilterV2, secondaryPacketQoS, secondaryContext, setConfigGPRSActive, setConfigGPRSActiveNotifyConfigChanged
											//data, setConfigGPRS, packetQoS, packetContext, packetService, this
	}

//
// Unit Tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for SetDefaultContextParams
@SYMTestPriority High
@SYMTestActions Invokes SetDefaultContextParams only for GPRS, R99 and CDMA
@SYMTestExpectedResults Pass
@SYMTestType UT
 */

void CCTsyPacketServicesFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketContext::TContextConfigGPRS expContextConfigGprs;
	expContextConfigGprs.iAccessPointName = KDefaultAccessPoint;
	expContextConfigGprs.iPdpType = KDefaultPdpType;

	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expLtsyGprsData(expContextConfigGprs);
	data.Close();
	expLtsyGprsData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, KErrNone);

	/* Client side test */
	RPacketContext::TContextConfigGPRS defaultGprsContextConfig;
	defaultGprsContextConfig.iAccessPointName = KDefaultAccessPoint;
	defaultGprsContextConfig.iPdpType = KDefaultPdpType;
	TPckg<RPacketContext::TContextConfigGPRS> defaultContextGprsConfigPckg(defaultGprsContextConfig);
	TRequestStatus setDefaultContextParamsStatus;
	packetService.SetDefaultContextParams(setDefaultContextParamsStatus, defaultContextGprsConfigPckg);
	User::WaitForRequest(setDefaultContextParamsStatus);

    ASSERT_EQUALS(KErrNone, setDefaultContextParamsStatus.Int());

	RPacketContext::TContextConfigR99_R4 expContextConfigR99R4;
	expContextConfigGprs.iAccessPointName = KDefaultAccessPoint;
	expContextConfigGprs.iPdpType = KDefaultPdpType;

	TMockLtsyData1<RPacketContext::TContextConfigR99_R4> expLtsyR99R4Data(expContextConfigR99R4);
	data.Close();
	expLtsyR99R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId, KErrNone);

	/* Client side test */
	RPacketContext::TContextConfigR99_R4 defaultR99R4ContextConfig;
	defaultR99R4ContextConfig.iAccessPointName = KDefaultAccessPoint;
	defaultR99R4ContextConfig.iPdpType = KDefaultPdpType;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultContextR99R4ConfigPckg(defaultR99R4ContextConfig);
	packetService.SetDefaultContextParams(setDefaultContextParamsStatus, defaultContextR99R4ConfigPckg);
	User::WaitForRequest(setDefaultContextParamsStatus);

    ASSERT_EQUALS(KErrNone, setDefaultContextParamsStatus.Int());

	RPacketContext::TContextConfigCDMA expContextConfigCDMA;
	expContextConfigCDMA.iServiceOption = KDefaultServiceOption;
	expContextConfigCDMA.iProtocolType = KDefaultPdpType;

	TMockLtsyData1<RPacketContext::TContextConfigCDMA> expLtsyCDMAData(expContextConfigCDMA);
	data.Close();
	expLtsyCDMAData.SerialiseL(data);

	/* Client side test */
	RPacketContext::TContextConfigCDMA defaultCDMAContextConfig;
	defaultCDMAContextConfig.iServiceOption = KDefaultServiceOption;
	defaultCDMAContextConfig.iProtocolType = KDefaultPdpType;
	TPckg<RPacketContext::TContextConfigCDMA> defaultContextCDMAConfigPckg(defaultCDMAContextConfig);
	packetService.SetDefaultContextParams(setDefaultContextParamsStatus, defaultContextCDMAConfigPckg);
	User::WaitForRequest(setDefaultContextParamsStatus);

    ASSERT_EQUALS(KErrArgument, setDefaultContextParamsStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for InitialiseContext with port
@SYMTestPriority High
@SYMTestActions Invokes InitialiseContext only, successfully obtaining the port
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	//Open a New Packet Context
	RPacketContext packetContext;
	CleanupClosePushL(packetContext);
	TName contextName;
	TInt err = packetContext.OpenNewContext(packetService,contextName);

	ASSERT_EQUALS(KErrNone, err);

	TInfoName primaryContext;
	TInfoName secondaryContext;
	primaryContext.Copy(KFirstAllocatedContextName);
	secondaryContext.Copy(KDefaultContextName);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData2(primaryContext, secondaryContext);
	data.Close();
	expLtsyContextNamesData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data, KErrNotSupported);

	//Client side test
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

    ASSERT_EQUALS(KErrNotSupported, initialiseContextStatus.Int());

	AssertMockLtsyStatusL();
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data, KErrNone);

	TDesC* contextNamePtr = &primaryContext;
	TDesC* channelIdPtr = const_cast<TDesC*>(&KDefaultChannelId);
	TMockLtsyData2<TDesC*, TDesC*> expLtsyContextNameAndChannelData(contextNamePtr, channelIdPtr);
	data.Close();
	expLtsyContextNameAndChannelData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNotSupported, data);

	//Client side test
	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

    ASSERT_EQUALS(KErrNotSupported, initialiseContextStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------

	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData3(primaryContext, secondaryContext);
	data.Close();
	expLtsyContextNamesData3.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data);

	TMockLtsyData2<TDesC*, TDesC*> expLtsyContextNameAndChannelData2(contextNamePtr, channelIdPtr);
	data.Close();
	expLtsyContextNameAndChannelData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNone, data);

	//Client side test
	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

    ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName); 
    ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated

	AssertMockLtsyStatusL();
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::InitialiseContext
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus terminatedStatus;
	iMockLTSY.NotifyTerminated(terminatedStatus);

	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNone, data);

	//Client side test
	User::WaitForRequest(terminatedStatus);
    ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());

	DriverDeleteContextL(primaryContext);

	packetContext.Close();
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------

    // Open two contexts and attempt to issue multiple initialise context requests

	//Open a New Packet Context
	CleanupClosePushL(packetContext);
	err = packetContext.OpenNewContext(packetService,contextName);
	ASSERT_EQUALS(KErrNone, err);

    //Open a New Packet Context 2
	RPacketContext packetContext2;
	CleanupClosePushL(packetContext2);
	err = packetContext2.OpenNewContext(packetService,contextName);
	ASSERT_EQUALS(KErrNone, err);

	TInfoName primaryContext2;
	primaryContext.Copy(KSecondAllocatedContextName);
	primaryContext2.Copy(KThirdAllocatedContextName);

	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData4(primaryContext, secondaryContext);
	data.Close();
	expLtsyContextNamesData4.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data);

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);

	TRequestStatus initialiseContextStatus2;

	/* This request is queued by ETEL as being a duplicate and does not reach the C-TSY at this point */
	packetContext2.InitialiseContext(initialiseContextStatus2,dataChannelPk);

	TMockLtsyData2<TDesC*, TDesC*> expLtsyContextNameAndChannelData3(contextNamePtr, channelIdPtr);
	data.Close();
	expLtsyContextNameAndChannelData3.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNone, data);

	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData5(primaryContext2, secondaryContext);
	data.Close();
	expLtsyContextNamesData5.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data);

	User::WaitForRequest(initialiseContextStatus);

	contextNamePtr = &primaryContext2;
	TMockLtsyData2<TDesC*, TDesC*> expLtsyContextNameAndChannelData4(contextNamePtr, channelIdPtr);
	data.Close();
	expLtsyContextNameAndChannelData4.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, KErrNone, data);

	User::WaitForRequest(initialiseContextStatus2);

	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_EQUALS(KErrNone, initialiseContextStatus2.Int());

	DriverDeleteContextL(primaryContext2);
	packetContext2.Close();

	DriverDeleteContextL(primaryContext);
	packetContext.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // packetContext2, packetContext, packetContext, data, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for InitialiseContext with port
@SYMTestPriority High
@SYMTestActions Invokes InitialiseContext only, allows it to timeout
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	//Open a New Packet Context
	RPacketContext packetContext;
	CleanupClosePushL(packetContext);
	TName contextName;
	TInt err = packetContext.OpenNewContext(packetService,contextName);

	ASSERT_EQUALS(KErrNone, err);

	TInfoName primaryContext;
	TInfoName secondaryContext;
	primaryContext.Copy(KFirstAllocatedContextName);
	secondaryContext.Copy(KDefaultContextName);

	//-------------------------------------------------------------------------
	// TEST: timeout on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	TMockLtsyData2<TInfoName, TInfoName> expLtsyContextNamesData2(primaryContext, secondaryContext);
	data.Close();
	expLtsyContextNamesData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, data);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	//Deliberately leave out the Complete in order to induce a timeout

	//Client side test
	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);

    ASSERT_EQUALS(KErrTimedOut, initialiseContextStatus.Int());

    DriverDeleteContextL(primaryContext);
	packetContext.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetContext, data, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for open the packet service when packet service status is set to EStatusUnattached
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Open()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// client side test
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RPacketService::TStatus getStatus;
 	ASSERT_EQUALS(KErrNone, packetService.GetStatus(getStatus));
 	ASSERT_EQUALS(RPacketService::EStatusUnattached, getStatus);

	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for open the packet service when packet service status is set to EStatusAttached.
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Open()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// client side test
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusAttached);
	
	RPacketService::TStatus getStatus;
 	ASSERT_EQUALS(KErrNone, packetService.GetStatus(getStatus));
 	ASSERT_EQUALS(RPacketService::EStatusAttached, getStatus);

	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for configuration settings using R99/R4
@SYMTestPriority High
@SYMTestActions SetConfig for R99R4
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	// client side test
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	// Prepare context configuration
	RPacketContext::TContextConfigR99_R4 setConfig99;
	setConfig99.iAccessPointName.Copy(KPointName99);
	setConfig99.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfig99.iPdpAddress.Copy(KPdpAdd99);
	// PCO Configurations
	setConfig99.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns99ForUN0006);
	setConfig99.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns99);
	setConfig99.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfig99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KLongUsername);
	setConfig99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfig99.iProtocolConfigOption.iId = KRequestID;
	RPacketContext::TMiscProtocolBuffer miscBuffer;
	miscBuffer.SetLength(KMaxMiscBufferSize);
	miscBuffer.FillZ();
	TPtr8 miscPtr(miscBuffer.LeftTPtr(miscBuffer.MaxLength()));
	miscPtr.SetLength(0);

	// TLV struct to store the Misc buffer:
	TTlvStruct<RPacketContext::TPcoId, RPacketContext::TPcoItemDataLength> tlv(miscPtr,0);

    const TUint16 XId=1;
    const TUint16 YId=10;

    RPacketContext::TPcoId pcoIdX(XId);
    RPacketContext::TPcoId pcoIdY(YId);

    //  BufferX
    TBuf8<KAppendableBufferSize> appendableBufferX(_L8("xxx"));
    appendableBufferX.SetLength(KAppendableBufferSize);
    TPtr8 appendablePtrX(const_cast<TUint8*>(appendableBufferX.Ptr()),appendableBufferX.Length(),appendableBufferX.Length());
    //  BufferY
    TBuf8<KAppendableBufferSize> appendableBufferY(_L8("yyy"));
	appendableBufferY.SetLength(KAppendableBufferSize);
	TPtr8 appendablePtrY(const_cast<TUint8*>(appendableBufferY.Ptr()),appendableBufferY.Length(),appendableBufferY.Length());

	//  Append 3 Items to the misc buffer -  X + Y + X
	tlv.AppendItemL(pcoIdX,appendablePtrX);
	tlv.AppendItemL(pcoIdY,appendablePtrY);
	tlv.AppendItemL(pcoIdX,appendablePtrX);

	setConfig99.iProtocolConfigOption.iMiscBuffer.Copy(miscBuffer);
	setConfig99.iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());

	TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig(setConfig99);
	//

	DriverInitialiseContextL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);
	RPacketContext::TGSNAddress accessPointName;
	accessPointName.Copy(KPointName99);
	RPacketContext::TProtocolType pdpType = RPacketContext::EPdpTypeIPv4;
	RPacketContext::TProtocolAddress pdpAddress;
	pdpAddress.Copy(KPdpAdd99);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer out of the packet service configuration
	PCOGeneratorL(setConfig99.iProtocolConfigOption, pcoBuffer);
	setConfig99.iProtocolConfigOption.iId = KRequestID;

	TMockLtsyData5<TInfoName,RPacketContext::TGSNAddress, RPacketContext::TProtocolType, RPacketContext::TProtocolAddress, RPacketContext::TMiscProtocolBuffer>
					packetSetConfigData(contextId, accessPointName, pdpType, pdpAddress, pcoBuffer);
	data.Close();
	packetSetConfigData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, data, KErrNone);

	// Indicate that the context name is due to comeback from the SetConfig callback
	TMockLtsyData1<TInfoName> callbackContextIdData(contextId);
	data.Close();
	callbackContextIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);

	DriverDeleteContextL(contextId);

	//

	RPacketContext packetContext;

	// Open a new context
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);

	//Client side test
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated

	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfig);

	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());

	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // data, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for configuration settings using R5
@SYMTestPriority High
@SYMTestActions SetConfig for R5
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	// client side test
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);


	// Prepare context configuration
	RPacketContext::TContextConfig_R5 setConfigR5;
	setConfigR5.iAccessPointName.Copy(KPointName99);
	setConfigR5.iPdpType = RPacketContext::EPdpTypeIPv4;
	setConfigR5.iPdpAddress.Copy(KPdpAdd99);
	// PCO Configurations
	setConfigR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns99);
	setConfigR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns99);
	setConfigR5.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	setConfigR5.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	setConfigR5.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	setConfigR5.iProtocolConfigOption.iId = KRequestID;
	RPacketContext::TMiscProtocolBuffer miscBuffer;
	miscBuffer.SetLength(KMaxMiscBufferSize);
	miscBuffer.FillZ();
	TPtr8 miscPtr(miscBuffer.LeftTPtr(miscBuffer.MaxLength()));
	miscPtr.SetLength(0);

	// TLV struct to store the Misc buffer:
	TTlvStruct<RPacketContext::TPcoId, RPacketContext::TPcoItemDataLength> tlv(miscPtr,0);

    const TUint16 XId=1;
    const TUint16 YId=10;

    RPacketContext::TPcoId pcoIdX(XId);
    RPacketContext::TPcoId pcoIdY(YId);

    //  BufferX
    TBuf8<KAppendableBufferSize> appendableBufferX(_L8("xxx"));
    appendableBufferX.SetLength(KAppendableBufferSize);
    TPtr8 appendablePtrX(const_cast<TUint8*>(appendableBufferX.Ptr()),appendableBufferX.Length(),appendableBufferX.Length());
    //  BufferY
    TBuf8<KAppendableBufferSize> appendableBufferY(_L8("yyy"));
	appendableBufferY.SetLength(KAppendableBufferSize);
	TPtr8 appendablePtrY(const_cast<TUint8*>(appendableBufferY.Ptr()),appendableBufferY.Length(),appendableBufferY.Length());

	//  Append 3 Items to the misc buffer -  X + Y + X
	tlv.AppendItemL(pcoIdX,appendablePtrX);
	tlv.AppendItemL(pcoIdY,appendablePtrY);
	tlv.AppendItemL(pcoIdX,appendablePtrX);

	setConfigR5.iProtocolConfigOption.iMiscBuffer.Copy(miscBuffer);
	setConfigR5.iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());

	TPckg<RPacketContext::TContextConfig_R5> pckgConfig(setConfigR5);

	//

	DriverInitialiseContextL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);
	RPacketContext::TGSNAddress accessPointName;
	accessPointName.Copy(KPointName99);
	RPacketContext::TProtocolType pdpType = RPacketContext::EPdpTypeIPv4;
	RPacketContext::TProtocolAddress pdpAddress;
	pdpAddress.Copy(KPdpAdd99);

	// Create the PCO buffer
	RPacketContext::TMiscProtocolBuffer pcoBuffer;
	pcoBuffer.Zero();

	// Generate a PCO buffer
	PCOGeneratorL(setConfigR5.iProtocolConfigOption, pcoBuffer);
	setConfigR5.iProtocolConfigOption.iId = KRequestID;

	TMockLtsyData5<TInfoName,RPacketContext::TGSNAddress, RPacketContext::TProtocolType, RPacketContext::TProtocolAddress, RPacketContext::TMiscProtocolBuffer>
					packetSetConfigData(contextId, accessPointName, pdpType, pdpAddress, pcoBuffer);
	data.Close();
	packetSetConfigData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, data, KErrNone);

	// Indicate that the context name is due to comeback from the SetConfig callback
	TMockLtsyData1<TInfoName> callbackContextIdData(contextId);
	data.Close();
	callbackContextIdData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, KErrNone, data);

	DriverDeleteContextL(contextId);

	//

	RPacketContext packetContext;

	// Open a new context
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);

	//Client side test
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);	
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now deprecated

	TRequestStatus setConfigStatus;
	packetContext.SetConfig(setConfigStatus, pckgConfig);

	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());

	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // data, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for QoS profile parameters
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::SetProfileParameters() only for GPRS, R99 and R5
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);

	//

	DriverInitialiseContextL();

	// GPRS Configuration
	RPacketQoS::TQoSGPRSRequested qosExpectedRequestedGPRSProfile;
	qosExpectedRequestedGPRSProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextId,
																				qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextId);
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	RPacketQoS::TQoSGPRSNegotiated completedProfileGPRS;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated> expLtsyQosProfileGPRSChangedInd(contextId, completedProfileGPRS);
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);
	
	// R99_R4 Configuration
	RPacketQoS::TQoSR99_R4Requested expectedPacketR99_R4Profile;
	expectedPacketR99_R4Profile.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	expectedPacketR99_R4Profile.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	expectedPacketR99_R4Profile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	expectedPacketR99_R4Profile.iMaxBER = RPacketQoS::EBERUnspecified;
	expectedPacketR99_R4Profile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	expectedPacketR99_R4Profile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	expectedPacketR99_R4Profile.iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	expectedPacketR99_R4Profile.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	expectedPacketR99_R4Profile.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	expectedPacketR99_R4Profile.iReqBER = RPacketQoS::EBERUnspecified;
	expectedPacketR99_R4Profile.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	expectedPacketR99_R4Profile.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	expectedPacketR99_R4Profile.iMaxTransferDelay = 0;
	expectedPacketR99_R4Profile.iMinAcceptableMaxRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iMinAcceptableMaxRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iMinAcceptableMaxSDUSize = 0;
	expectedPacketR99_R4Profile.iMinGuaranteedRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iMinGuaranteedRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iReqGuaranteedRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iReqGuaranteedRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iReqMaxRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iReqMaxRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iReqMaxSDUSize = 0;
	expectedPacketR99_R4Profile.iReqTransferDelay = 0;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Requested> packetSetConfigR99_R4Data(contextId,
																					expectedPacketR99_R4Profile);
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpR99_R4ContextNameData(contextId);
	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	RPacketQoS::TQoSR99_R4Negotiated completedProfileR99;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Negotiated> expLtsyQosProfileR99ChangedInd(contextId, completedProfileR99);
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	// R5 Configuration
	RPacketQoS::TQoSR5Requested expectedPacketR5Profile;
	expectedPacketR5Profile.iSignallingIndication = EFalse;
	expectedPacketR5Profile.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
	expectedPacketR5Profile.iMaxBER = RPacketQoS::EBERUnspecified;
	expectedPacketR5Profile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	expectedPacketR5Profile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	expectedPacketR5Profile.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	expectedPacketR5Profile.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	expectedPacketR5Profile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	expectedPacketR5Profile.iReqBER = RPacketQoS::EBERUnspecified;
	expectedPacketR5Profile.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	expectedPacketR5Profile.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	expectedPacketR5Profile.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	expectedPacketR5Profile.iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	expectedPacketR5Profile.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;

	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR5Requested> packetSetConfigR5Data(contextId, expectedPacketR5Profile);
	data.Close();
	packetSetConfigR5Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpR5ContextNameData(contextId);
	callbackSetPdpR5ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId, KErrNone, data);
	
	RPacketQoS::TQoSR5Negotiated completedProfileR5;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR5Negotiated> expLtsyQosProfileR5ChangedInd(contextId, completedProfileR5);
	data.Close();
	expLtsyQosProfileR5ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	DriverDeleteContextL(contextId);

	//

	// client side test
	RPacketContext packetContext;

	// Open a new context
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now depreciated

	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	//
    // SetProfileParameters for GPRS Configurations
    //

	RPacketQoS::TQoSGPRSRequested qosGPRSRequestedProfile;
	qosGPRSRequestedProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
    TPckg<RPacketQoS::TQoSGPRSRequested> profilePckgGPRS(qosGPRSRequestedProfile);

    TRequestStatus notifyGPRSChangeStatus;
	RPacketQoS::TQoSGPRSNegotiated profileGPRS;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosGPRSNegotiatedProfilePk(profileGPRS);
	packetQoS.NotifyProfileChanged(notifyGPRSChangeStatus, qosGPRSNegotiatedProfilePk);
    
	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyGPRSChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyGPRSChangeStatus.Int());

	//
    // SetProfileParameters for R99R4 Configurations
    //

	RPacketQoS::TQoSR99_R4Requested packetR99_R4;
	packetR99_R4.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	packetR99_R4.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	packetR99_R4.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	packetR99_R4.iMaxBER = RPacketQoS::EBERUnspecified;
	packetR99_R4.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	packetR99_R4.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	packetR99_R4.iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	packetR99_R4.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	packetR99_R4.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	packetR99_R4.iReqBER = RPacketQoS::EBERUnspecified;
	packetR99_R4.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	packetR99_R4.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	packetR99_R4.iMaxTransferDelay = 0;
	packetR99_R4.iMinAcceptableMaxRate.iDownlinkRate = 0;
	packetR99_R4.iMinAcceptableMaxRate.iUplinkRate = 0;
	packetR99_R4.iMinAcceptableMaxSDUSize = 0;
	packetR99_R4.iMinGuaranteedRate.iDownlinkRate = 0;
	packetR99_R4.iMinGuaranteedRate.iUplinkRate = 0;
	packetR99_R4.iReqGuaranteedRate.iDownlinkRate = 0;
	packetR99_R4.iReqGuaranteedRate.iUplinkRate = 0;
	packetR99_R4.iReqMaxRate.iDownlinkRate = 0;
	packetR99_R4.iReqMaxRate.iUplinkRate = 0;
	packetR99_R4.iReqMaxSDUSize = 0;
	packetR99_R4.iReqTransferDelay = 0;
	TPckg< RPacketQoS::TQoSR99_R4Requested > profilePckgR99_R4(packetR99_R4);
	
	TRequestStatus notifyR99ChangeStatus;
	RPacketQoS::TQoSR99_R4Negotiated profileR99;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> qos99NegotiatedProfilePk(profileR99);
	packetQoS.NotifyProfileChanged(notifyR99ChangeStatus, qos99NegotiatedProfilePk);
		
	TRequestStatus requestStatusR99_R4;
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());
	
	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyR99ChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyR99ChangeStatus.Int());

	//
    // SetProfileParameters for R5 Configurations
    //

	RPacketQoS::TQoSR5Requested packetR5;
	packetR5.iSignallingIndication = EFalse;
	packetR5.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
	packetR5.iMaxBER = RPacketQoS::EBERUnspecified;
	packetR5.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	packetR5.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	packetR5.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	packetR5.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	packetR5.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	packetR5.iReqBER = RPacketQoS::EBERUnspecified;
	packetR5.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	packetR5.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	packetR5.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	packetR5.iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	packetR5.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	TPckg< RPacketQoS::TQoSR5Requested > profilePckgR5(packetR5);

	TRequestStatus notifyR5ChangeStatus;
	RPacketQoS::TQoSR5Negotiated profileR5;
	TPckg<RPacketQoS::TQoSR5Negotiated> qosR5NegotiatedProfilePk(profileR5);
	packetQoS.NotifyProfileChanged(notifyR5ChangeStatus, qosR5NegotiatedProfilePk);
	
	TRequestStatus requestStatusR5;
	packetQoS.SetProfileParameters(requestStatusR5, profilePckgR5);
	User::WaitForRequest(requestStatusR5);
	ASSERT_EQUALS(KErrNone, requestStatusR5.Int());
	
	// Complete NotifyProfileChanged
	User::WaitForRequest(notifyR5ChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyR5ChangeStatus.Int());

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // data, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN0009 
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::RejectActivationRequest
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::RejectActivationRequest()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId, KErrNone);

	TRequestStatus requestStatus;
	packetService.RejectActivationRequest(requestStatus);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	packetService.Close();
	
	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notification of transfer capability.
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyDynamicCapsChange()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);


	TDynamicTransferCapsFlags transferCaps = KCapsHSDPA;
  	TMockLtsyData1< TDynamicTransferCapsFlags > ltsyData(transferCaps);
  	ltsyData.SerialiseL(data);


	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);

	//
	// client side test
	
	//send request
	TRequestStatus requestStatus;
	RPacketService::TDynamicCapsFlags dynamicCapsFlags;

	// Required as changes to transfer capability changes the dynamic capabilities
	packetService.NotifyDynamicCapsChange(requestStatus, dynamicCapsFlags);
	// Issue the Complete...
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId, KErrNone, data);
	
	User::WaitForRequest(requestStatus);

	//expected dynamic caps according to transferCaps being KCapsHSDPA
	RPacketService::TDynamicCapsFlags dynamicCaps;
	dynamicCaps = 0;
      dynamicCaps |= RPacketService::KCapsHSDPA;

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(dynamicCapsFlags, dynamicCaps);
	
	packetService.Close();
	
	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00011A
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyContextActivationRequested GPRS
@SYMTestPriority High
@SYMTestActions Invokes NotifyContextActivationRequested
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00011AL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

  	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RPacketService::NotifyContextActivationRequested
 	//-------------------------------------------------------------------------
	
	// GPRS test
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	contextConfigGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
	contextConfigGPRS.iPdpAddress.Copy(KAddress);
	contextConfigGPRS.iAccessPointName.Copy(KAPN);
	
	TMockLtsyData1 <RPacketContext::TContextConfigGPRS> ltsyData(contextConfigGPRS);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId, KErrNone, data);

	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS getContextParam;
	TPckg<RPacketContext::TContextConfigGPRS> pckgParam(getContextParam);
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(getContextParam.iPdpType, contextConfigGPRS.iPdpType);
	ASSERT_EQUALS(getContextParam.iPdpAddress, contextConfigGPRS.iPdpAddress);
	ASSERT_EQUALS(getContextParam.iAccessPointName, contextConfigGPRS.iAccessPointName);

	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00011B
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyContextActivationRequested R99_R4
@SYMTestPriority High
@SYMTestActions Invokes NotifyContextActivationRequested
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00011BL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

  	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RPacketService::NotifyContextActivationRequested
 	//-------------------------------------------------------------------------
	
	// R99_R4 test 
	RPacketContext::TContextConfigR99_R4 contextConfigR99;
	contextConfigR99.iAccessPointName.Copy(KPointName99);
	contextConfigR99.iPdpType = RPacketContext::EPdpTypeIPv4;
	contextConfigR99.iPdpAddress.Copy(KPdpAdd99);
	contextConfigR99.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	contextConfigR99.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);
	contextConfigR99.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	contextConfigR99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	contextConfigR99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	contextConfigR99.iProtocolConfigOption.iId = KRequestID;
	
	TMockLtsyData1 <RPacketContext::TContextConfigR99_R4> ltsyData(contextConfigR99);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId, KErrNone, data);

	TRequestStatus requestStatus;
	RPacketContext::TContextConfigR99_R4 getContextParam;
	TPckg<RPacketContext::TContextConfigR99_R4> pckgParam(getContextParam);
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(getContextParam.iPdpType, contextConfigR99.iPdpType);
	ASSERT_EQUALS(getContextParam.iPdpAddress, contextConfigR99.iPdpAddress);
	ASSERT_EQUALS(getContextParam.iAccessPointName, contextConfigR99.iAccessPointName);

	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, contextConfigR99.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);	
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, contextConfigR99.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iAuthInfo.iProtocol, contextConfigR99.iProtocolConfigOption.iAuthInfo.iProtocol);
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iAuthInfo.iUsername, contextConfigR99.iProtocolConfigOption.iAuthInfo.iUsername);	
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iAuthInfo.iPassword, contextConfigR99.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iId, contextConfigR99.iProtocolConfigOption.iId);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00011C
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyContextActivationRequested R5
@SYMTestPriority High
@SYMTestActions Invokes NotifyContextActivationRequested
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00011CL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

  	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RPacketService::NotifyContextActivationRequested
 	//-------------------------------------------------------------------------
	
	// R5 test 
	
	RPacketContext::TContextConfig_R5 contextConfigR5;
	contextConfigR5.iAccessPointName.Copy(KPointName99);
	contextConfigR5.iPdpType = RPacketContext::EPdpTypeIPv4;
	contextConfigR5.iPdpAddress.Copy(KPdpAdd99);
	// PCO Configurations
	contextConfigR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns99);
	contextConfigR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns99);
	contextConfigR5.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	contextConfigR5.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	contextConfigR5.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	contextConfigR5.iProtocolConfigOption.iId = KRequestID;
	RPacketContext::TMiscProtocolBuffer miscBuffer;
	miscBuffer.SetLength(KMaxMiscBufferSize);
	miscBuffer.FillZ();
	TPtr8 miscPtr(miscBuffer.LeftTPtr(miscBuffer.MaxLength()));
	miscPtr.SetLength(0);

	// TLV struct to store the Misc buffer:
	TTlvStruct<RPacketContext::TPcoId, RPacketContext::TPcoItemDataLength> tlv(miscPtr,0);

    const TUint16 XId=1;
    const TUint16 YId=10;

    RPacketContext::TPcoId pcoIdX(XId);
    RPacketContext::TPcoId pcoIdY(YId);

    //  BufferX
    TBuf8<KAppendableBufferSize> appendableBufferX(_L8("xxx"));
    appendableBufferX.SetLength(KAppendableBufferSize);
    TPtr8 appendablePtrX(const_cast<TUint8*>(appendableBufferX.Ptr()),appendableBufferX.Length(),appendableBufferX.Length());
    //  BufferY
    TBuf8<KAppendableBufferSize> appendableBufferY(_L8("yyy"));
	appendableBufferY.SetLength(KAppendableBufferSize);
	TPtr8 appendablePtrY(const_cast<TUint8*>(appendableBufferY.Ptr()),appendableBufferY.Length(),appendableBufferY.Length());

	//  Append 3 Items to the misc buffer -  X + Y + X
	tlv.AppendItemL(pcoIdX,appendablePtrX);
	tlv.AppendItemL(pcoIdY,appendablePtrY);
	tlv.AppendItemL(pcoIdX,appendablePtrX);

	contextConfigR5.iProtocolConfigOption.iMiscBuffer.Copy(miscBuffer);
	contextConfigR5.iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());
	
	TMockLtsyData1 <RPacketContext::TContextConfig_R5> ltsyData(contextConfigR5);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId, KErrNone, data);

	TRequestStatus requestStatus;
	RPacketContext::TContextConfig_R5 getContextParam;
	TPckg<RPacketContext::TContextConfig_R5> pckgParam(getContextParam);
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(getContextParam.iPdpType, contextConfigR5.iPdpType);
	ASSERT_EQUALS(getContextParam.iPdpAddress, contextConfigR5.iPdpAddress);
	ASSERT_EQUALS(getContextParam.iAccessPointName, contextConfigR5.iAccessPointName);

	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, contextConfigR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);	
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, contextConfigR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iAuthInfo.iProtocol, contextConfigR5.iProtocolConfigOption.iAuthInfo.iProtocol);
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iAuthInfo.iUsername, contextConfigR5.iProtocolConfigOption.iAuthInfo.iUsername);	
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iAuthInfo.iPassword, contextConfigR5.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iId, contextConfigR5.iProtocolConfigOption.iId);
	ASSERT_EQUALS(getContextParam.iProtocolConfigOption.iMiscBuffer, contextConfigR5.iProtocolConfigOption.iMiscBuffer);
		
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, packetService, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyMSClassChange
@SYMTestPriority High
@SYMTestActions Invokes NotifyMSClassChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	RBuf8 data;
	CleanupClosePushL(data);

  	//pack context param for completion into data
	RPacketService::TMSClass tMSclassReturn = RPacketService::EMSClassDualMode;
	TMockLtsyData1<RPacketService::TMSClass> expLtsyMSClassChangeInd(tMSclassReturn);
	data.Close();
	expLtsyMSClassChangeInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPacketMsClassChangeIndId, KErrNone, data);

	//-------------------------------------------------------------------------
	// TEST: Notify MS Class Change
 	//-------------------------------------------------------------------------
	TRequestStatus notifyMSClassChangeStatus;
	RPacketService::TMSClass tMSClass;
	packetService.NotifyMSClassChange(notifyMSClassChangeStatus, tMSClass);
	User::WaitForRequest(notifyMSClassChangeStatus);

	ASSERT_EQUALS(tMSclassReturn, tMSClass);
	ASSERT_EQUALS(KErrNone, notifyMSClassChangeStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyDynamicCapsChange
@SYMTestPriority High
@SYMTestActions Invokes NotifyDynamicCapsChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

  	//pack context param for completion into data
	TBool gprsIsSupportedOnCell;
	TBool csServicesIsSupportedOnCell;
	TMockLtsyData2<TBool, TBool> expLtsyDynamicCapsFlags(gprsIsSupportedOnCell, csServicesIsSupportedOnCell);
	data.Close();
	expLtsyDynamicCapsFlags.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId, KErrNone, data);

	//-------------------------------------------------------------------------
	// TEST: Notify Dynamic Caps Change
 	//-------------------------------------------------------------------------

	TRequestStatus notifyDynamicCapsChangeStatus;
	RPacketService::TDynamicCapsFlags dynamicCapsFlags;
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynamicCapsFlags);
	User::WaitForRequest(notifyDynamicCapsChangeStatus);

	//expected dynamic caps according to the set gprsIsSupportedOnCell and csServicesIsSupportedOnCell
	//and the initial state of the CTSY
	RPacketService::TDynamicCapsFlags dynamicCaps;
	dynamicCaps = 0;
	dynamicCaps |= RPacketService::KCapsManualAttach;
	dynamicCaps &= ~( RPacketService::KCapsSMSTransfer |
						RPacketService::KCapsManualDetach |
						RPacketService::KCapsRxContextActivationReq );
	dynamicCaps |= RPacketService::KCapsRxCSCall;

	ASSERT_EQUALS(dynamicCapsFlags, dynamicCaps);
	ASSERT_EQUALS(KErrNone, notifyDynamicCapsChangeStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyChangeOfNtwkRegStatus
@SYMTestPriority High
@SYMTestActions Invokes NotifyChangeOfNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService::TRegistrationStatus registrationStatusReturn;
	registrationStatusReturn = RPacketService::ENotRegisteredAndNotAvailable;
	TMockLtsyData1<RPacketService::TRegistrationStatus> expLtsyRegistrationStatus(registrationStatusReturn);
	data.Close();
	expLtsyRegistrationStatus.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId, KErrNone, data);

	//-------------------------------------------------------------------------
	// TEST: notify change of network registration status
 	//-------------------------------------------------------------------------
	TRequestStatus notifyChangeOfNtwkRegStatusStatus;
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.NotifyChangeOfNtwkRegStatus(notifyChangeOfNtwkRegStatusStatus, registrationStatus);

	User::WaitForRequest(notifyChangeOfNtwkRegStatusStatus);

	ASSERT_EQUALS(registrationStatus, registrationStatusReturn);
	ASSERT_EQUALS(KErrNone, notifyChangeOfNtwkRegStatusStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyContextAdded
@SYMTestPriority High
@SYMTestActions Invokes NotifyContextAdded
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00015L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// TEST: open a new context and notify a new context was added
 	//-------------------------------------------------------------------------

	RPacketContext packetContext;
	TRequestStatus notifyContextAddedStatus;
	TName contextNameNotify;
	packetService.NotifyContextAdded(notifyContextAddedStatus, contextNameNotify);

	//open a context and get a context name
	TName contextName;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);

	//complete the context added notification
	TMockLtsyData1<TName> expLtsyContextNameData2(contextName);
	data.Close();
	expLtsyContextNameData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextAddedIndId, KErrNone, data);
	User::WaitForRequest(notifyContextAddedStatus);

	ASSERT_EQUALS(contextName, contextNameNotify);
	ASSERT_EQUALS(KErrNone, notifyContextAddedStatus.Int());

	DriverDeleteContextL(contextName);
	packetContext.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00016L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	//open a context and get a context name
	TInfoName contextName;

	RPacketContext packetContext;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);

	//--------------------------------------------------------------------------------
	// TEST: open a new context and notify context status changed without reject cause
	//--------------------------------------------------------------------------------

	TRequestStatus notifyContextStatusChangeStatus;
	RPacketContext::TContextStatus contextStatusNotify;
	packetContext.NotifyStatusChange(notifyContextStatusChangeStatus, contextStatusNotify);

	//complete the context status change notification
	TContextMisc contextMiscReturn;
	contextMiscReturn.iStatus = RPacketContext::EStatusActive;

	TMockLtsyData2<TInfoName, TContextMisc> expLtsyContextNameData(contextName, contextMiscReturn);
	data.Close();
	expLtsyContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId, KErrNone, data);
	User::WaitForRequest(notifyContextStatusChangeStatus);

	ASSERT_EQUALS(contextStatusNotify, contextMiscReturn.iStatus);
	ASSERT_EQUALS(KErrNone, notifyContextStatusChangeStatus.Int());

	//--------------------------------------------------------------------------------
	// TEST: notify context status changed with reject cause
	//--------------------------------------------------------------------------------

	TRequestStatus notifyContextStatusChangeStatus2;
	RPacketContext::TContextStatus contextStatusNotify2;
	packetContext.NotifyStatusChange(notifyContextStatusChangeStatus2, contextStatusNotify2);

	//complete the context status change notification
	_LIT8(KRejectCode, "1");
	TContextMisc contextMiscReturn2;
	contextMiscReturn2.iStatus = RPacketContext::EStatusInactive;
	contextMiscReturn2.iRejectionCause.Copy(KRejectCode);

	TMockLtsyData2<TInfoName, TContextMisc> expLtsyContextNameData2(contextName, contextMiscReturn2);
	data.Close();
	expLtsyContextNameData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId, KErrNone, data);
	User::WaitForRequest(notifyContextStatusChangeStatus2);

	ASSERT_EQUALS(contextStatusNotify2, contextMiscReturn2.iStatus);
	ASSERT_EQUALS(KErrNone, notifyContextStatusChangeStatus2.Int());


	//close context
	DriverDeleteContextL(contextName);
	packetContext.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this

	}
/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00017
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for getting the network registration status
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00017L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	//-------------------------------------------------------------------------
	// TEST: get the current network status
 	//-------------------------------------------------------------------------

	RPacketService::TRegistrationStatus registrationStatusReturn;
	registrationStatusReturn = RPacketService::ERegisteredRoaming;
	TMockLtsyData1<RPacketService::TRegistrationStatus> completeRegistrationStatus(registrationStatusReturn);
	data.Close();
	completeRegistrationStatus.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId, KErrNone, data);

	TRequestStatus getNetworkStatus;
	RPacketService::TRegistrationStatus registrationStatus;

	packetService.GetNtwkRegStatus(getNetworkStatus, registrationStatus);
	User::WaitForRequest(getNetworkStatus);

	ASSERT_EQUALS(registrationStatusReturn, registrationStatus);
	ASSERT_EQUALS(KErrNone, getNetworkStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00018
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for NotifyConnectionSpeedChange
@SYMTestPriority High
@SYMTestActions Invokes NotifyConnectionSpeedChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00018L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketContext packetContext;
	CleanupClosePushL(packetContext);

	//open a context and get a context name
	TInfoName contextName;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);

	//---------------------------------------------------------------------
	// TEST: open a new context and notify context connection speed change
	//---------------------------------------------------------------------

	TRequestStatus notifyContextConnectionSpeedChangeStatus;
	TUint contextConnectionSpeedNotify;
	
	packetContext.NotifyConnectionSpeedChange(notifyContextConnectionSpeedChangeStatus, contextConnectionSpeedNotify);
	
	//complete the context connection speed change notification
	TUint contextConnectionSpeedReturn = 1234569;
	TMockLtsyData2<TInfoName, TUint> expLtsyContextNameData(contextName, contextConnectionSpeedReturn);
	data.Close();
	expLtsyContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPdpContextConnectionSpeedChangeIndId, KErrNone, data);
	
	User::WaitForRequest(notifyContextConnectionSpeedChangeStatus);

	ASSERT_EQUALS(contextConnectionSpeedNotify, contextConnectionSpeedReturn);
	ASSERT_EQUALS(KErrNone, notifyContextConnectionSpeedChangeStatus.Int());

	//close context
	DriverDeleteContextL(contextName);
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetService, packetContext, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00019
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::InitialiseContext, RPacketMbmsContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes InitialiseContext, NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00019L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	// Open new Mbms context - complete with KErrNone
	TInfoName contextName1;
	RPacketMbmsContext packetContext1;
	_LIT(KCHANNEL1,"CHANNEL1");
	OpenInitialiseMbmsContextLC(packetService,packetContext1,contextName1,KCHANNEL1,KErrNone);
	
	// Open new Mbms context - complete with KErrNone (demonstrates each new context causes the initialise IPCs)
	TInfoName contextName2;
	RPacketMbmsContext packetContext2;
	_LIT(KCHANNEL2,"CHANNEL2");
	OpenInitialiseMbmsContextLC(packetService,packetContext2,contextName2,KCHANNEL2,KErrNone);
	
	// Open new Mbms context - complete with KErrGeneral 
	TInfoName contextName3;
	RPacketMbmsContext packetContext3;
	_LIT(KCHANNEL3,"CHANNEL3");
	OpenInitialiseMbmsContextLC(packetService,packetContext3,contextName3,KCHANNEL3,KErrGeneral);

	//test a NotifyConfigChanged - KErrNone case
	TDesC* contextNamePtr = &contextName1;
	RPacketMbmsContext::TContextConfigMbmsV1 compContextConfig;
	compContextConfig.iTmgi.SetMCC(1);
	compContextConfig.iTmgi.SetMNC(2);
	compContextConfig.iTmgi.SetServiceId(3);
	compContextConfig.iMbmsAccessBearer = E2GBearer;
	compContextConfig.iMbmsServicePriority = EMbmsMediumServicePriority;
	compContextConfig.iMbmsServiceMode = KBroadcast;
	compContextConfig.iMbmsSessionFlag = EFalse;
	TMockLtsyData2<TDesC*, RPacketMbmsContext::TContextConfigMbmsV1> contextConfigCompData(contextNamePtr,compContextConfig);
	contextConfigCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId, KErrNone, data);
	data.Close();
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	RPacketMbmsContext::TContextConfigMbmsV1Pckg contextConfigPckg(contextConfig);
	packetContext1.NotifyConfigChanged(reqStatus,contextConfigPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(compContextConfig.iTmgi.GetMCC(),contextConfig.iTmgi.GetMCC());
	ASSERT_EQUALS(compContextConfig.iTmgi.GetMNC(),contextConfig.iTmgi.GetMNC());
	ASSERT_EQUALS(compContextConfig.iTmgi.GetServiceId(),contextConfig.iTmgi.GetServiceId());
	ASSERT_EQUALS(compContextConfig.iMbmsAccessBearer,contextConfig.iMbmsAccessBearer);
	ASSERT_EQUALS(compContextConfig.iMbmsServicePriority,contextConfig.iMbmsServicePriority);
	ASSERT_EQUALS(compContextConfig.iMbmsServiceMode,contextConfig.iMbmsServiceMode);
	ASSERT_EQUALS(compContextConfig.iMbmsSessionFlag,contextConfig.iMbmsSessionFlag);

	AssertMockLtsyStatusL();
	
	//test a NotifyConfigChanged - KErrGeneral case
	contextConfigCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId, KErrGeneral, data);
	data.Close();
	
	packetContext1.NotifyConfigChanged(reqStatus,contextConfigPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&packetContext3);
	CleanupStack::PopAndDestroy(&packetContext2);
	CleanupStack::PopAndDestroy(&packetContext1);
	CleanupStack::PopAndDestroy(&packetService);
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00020
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetMbmsNetworkServiceStatus RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes GetMbmsNetworkServiceStatus, NotifyMbmsNetworkServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00020L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	TBool attemptAttach = ETrue; //in
	
	//test the get with a KErrNone comp
	TMockLtsyData1<TBool> getMbmsNetworkServiceStatusExpData(attemptAttach);
	getMbmsNetworkServiceStatusExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId,data);
	data.Close();
	
	TMbmsNetworkServiceStatus mbmsNetworkServiceStatusReturn = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> getMbmsNetworkServiceStatusCompData(mbmsNetworkServiceStatusReturn);
	getMbmsNetworkServiceStatusCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId, KErrNone, data);
	data.Close();

	TMbmsNetworkServiceStatus mbmsNetworkServiceStatus = EMbmsSupportUnknown;; //out
	packetService.GetMbmsNetworkServiceStatus(reqStatus,attemptAttach,mbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(mbmsNetworkServiceStatus, mbmsNetworkServiceStatusReturn);
	AssertMockLtsyStatusL();
	
	//result is now cached (a call will not cause an LTSY call)
	mbmsNetworkServiceStatus = EMbmsSupportUnknown;
	packetService.GetMbmsNetworkServiceStatus(reqStatus,attemptAttach,mbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(mbmsNetworkServiceStatus, mbmsNetworkServiceStatusReturn);
	AssertMockLtsyStatusL();

	//update the cache via notifier - KErrNone case
	mbmsNetworkServiceStatusReturn = EMbmsNotSupported;
	getMbmsNetworkServiceStatusCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId, KErrNone, data);
	data.Close();
	mbmsNetworkServiceStatus = EMbmsSupportUnknown;
	packetService.NotifyMbmsNetworkServiceStatusChange(reqStatus,mbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(mbmsNetworkServiceStatusReturn, mbmsNetworkServiceStatus);
	AssertMockLtsyStatusL();

	//a cached get (should return notified status)
	mbmsNetworkServiceStatus = EMbmsSupportUnknown;
	packetService.GetMbmsNetworkServiceStatus(reqStatus,attemptAttach,mbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(mbmsNetworkServiceStatus, mbmsNetworkServiceStatusReturn);
	AssertMockLtsyStatusL();

	
	//update the cache via notifier - KErrGeneral case
	mbmsNetworkServiceStatusReturn = EMbmsSupported;
	getMbmsNetworkServiceStatusCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId, KErrGeneral, data);
	data.Close();
	mbmsNetworkServiceStatus = EMbmsSupportUnknown;
	packetService.NotifyMbmsNetworkServiceStatusChange(reqStatus,mbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);
	ASSERT_EQUALS(EMbmsSupportUnknown, mbmsNetworkServiceStatus);
	AssertMockLtsyStatusL();
	
	
			
	// we need to close packetservice and re-open so we can test the error cases as to null the cache
	CleanupStack::PopAndDestroy(&packetService);
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	//test the get with a KErrGeneral comp
	getMbmsNetworkServiceStatusExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId,data);
	data.Close();
	
	getMbmsNetworkServiceStatusCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId, KErrGeneral, data);
	data.Close();
	
	mbmsNetworkServiceStatus = EMbmsSupportUnknown;
	packetService.GetMbmsNetworkServiceStatus(reqStatus,attemptAttach,mbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);
	ASSERT_EQUALS(mbmsNetworkServiceStatus, EMbmsSupportUnknown);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&packetService);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00021
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::UpdateMbmsMonitorServiceListL, RPacketService::NotifyMbmsServiceAvailabilityChange
@SYMTestPriority High
@SYMTestActions Invokes UpdateMbmsMonitorServiceListL, NotifyMbmsServiceAvailabilityChange, CRetrievePcktMbmsMonitoredServices
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00021L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	TRequestStatus notifyReqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
	
	CGetMbmsMonitorServiceListAO* getMbmsMonitorServiceListAO = CGetMbmsMonitorServiceListAO::NewLC(packetService);
	
	RPacketService::TMbmsServiceAvailabilityV1 entry1; 
	entry1.iTmgi.SetMCC(1);
	entry1.iTmgi.SetMNC(1);
	entry1.iTmgi.SetServiceId(1);
	entry1.iMbmsAccessBearer = E2GBearer;
	entry1.iMbmsAvailabilityStatus = EMbmsUnavailable;
	entry1.iMbmsServiceMode = KBroadcast;
	
	RPacketService::TMbmsServiceAvailabilityV1 entry2; 
	entry2.iTmgi.SetMCC(2);
	entry2.iTmgi.SetMNC(2);
	entry2.iTmgi.SetServiceId(2);
	entry2.iMbmsAccessBearer = E3GBearer;
	entry2.iMbmsAvailabilityStatus = EMbmsAvailable;
	entry2.iMbmsServiceMode = KSelectedBroadcast;
	
	CPcktMbmsMonitoredServiceList* nullList = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(nullList);
	
	CPcktMbmsMonitoredServiceList* list = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(list);

	list->AddEntryL(entry1);
	list->AddEntryL(entry2);
	
	RArray<TUint> serviceIds;
	CleanupClosePushL(serviceIds);
	
	//add entries (KErrNone complete case)
	TMbmsAction action = EAddEntries;
	CPcktMbmsMonitoredServiceList* serviceList = nullList;
	CPcktMbmsMonitoredServiceList* changeList = list;
	
	TMockLtsyData3<TMbmsAction, CPcktMbmsMonitoredServiceList*, CPcktMbmsMonitoredServiceList*> updateMbmsMonitorServiceListExpData(action,serviceList,changeList);
	updateMbmsMonitorServiceListExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId,data);
	data.Close();

	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId, KErrNone);
	
	packetService.UpdateMbmsMonitorServiceListL(reqStatus,action,list);
	packetService.NotifyMbmsServiceAvailabilityChange(notifyReqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	User::WaitForRequest(notifyReqStatus);
	ASSERT_EQUALS(notifyReqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();

	//check the results
	getMbmsMonitorServiceListAO->TestRetrieveL();
	ASSERT_EQUALS(getMbmsMonitorServiceListAO->CheckResultsL(*list), static_cast<TBool>(ETrue));
		
	
	//remove entries (KErrNone complete case)
	action = ERemoveEntries;
	serviceList = list;
	changeList = list;

	updateMbmsMonitorServiceListExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId,data);
	data.Close();

	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId, KErrNone);

	packetService.UpdateMbmsMonitorServiceListL(reqStatus,action,list);
	packetService.NotifyMbmsServiceAvailabilityChange(notifyReqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	User::WaitForRequest(notifyReqStatus);
	ASSERT_EQUALS(notifyReqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();

	//check the results
	getMbmsMonitorServiceListAO->TestRetrieveL();
	ASSERT_EQUALS(getMbmsMonitorServiceListAO->CheckResultsL(*nullList), static_cast<TBool>(ETrue));

	
	//add entries (KErrMbmsImpreciseServiceEntries complete case)
	action = EAddEntries;
	serviceList = nullList;
	changeList = list;
	
	updateMbmsMonitorServiceListExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId,data);
	data.Close();

	serviceIds.AppendL(1);
	TMockLtsyData1<RArray<TUint> > updateMbmsMonitorServiceListCompData(serviceIds);
	updateMbmsMonitorServiceListCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId, KErrMbmsImpreciseServiceEntries, data);
	data.Close();
	
	packetService.UpdateMbmsMonitorServiceListL(reqStatus,action,list);
	//can't do a NotifyMbmsServiceAvailabilityChange as not completed in the error case
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrMbmsImpreciseServiceEntries);
	AssertMockLtsyStatusL();

	
	//check the results
	getMbmsMonitorServiceListAO->TestRetrieveL();
	list->DeleteEntryL(0); //remove entry in serviceId error list
	ASSERT_EQUALS(getMbmsMonitorServiceListAO->CheckResultsL(*list), static_cast<TBool>(ETrue));
	
	
	//test the service change to unavailable
	packetService.NotifyMbmsServiceAvailabilityChange(reqStatus);

	//all services are unavailable
	serviceIds.Reset();
	TMockLtsyData1<RArray<TUint> > notifyMbmsServiceAvailabilityChangeCompData(serviceIds);
	notifyMbmsServiceAvailabilityChangeCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId, KErrNone, data);
	data.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();
		
	//check the results
	getMbmsMonitorServiceListAO->TestRetrieveL();
	//update the test data
	entry2.iMbmsAvailabilityStatus = EMbmsUnavailable;
	list->ChangeEntryL(0,entry2);
	ASSERT_EQUALS(getMbmsMonitorServiceListAO->CheckResultsL(*list), static_cast<TBool>(ETrue));
	
	
	//test the service change to available
	packetService.NotifyMbmsServiceAvailabilityChange(reqStatus);

	//services 1 is available
	serviceIds.Reset();
	serviceIds.AppendL(entry2.iTmgi.GetServiceId());
	notifyMbmsServiceAvailabilityChangeCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId, KErrNone, data);
	data.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();
		
	//check the results
	getMbmsMonitorServiceListAO->TestRetrieveL();
	//update the test data
	entry2.iMbmsAvailabilityStatus = EMbmsAvailable;
	list->ChangeEntryL(0,entry2);
	ASSERT_EQUALS(getMbmsMonitorServiceListAO->CheckResultsL(*list), static_cast<TBool>(ETrue));

	
	//test the service change to unavailable (error case)
	packetService.NotifyMbmsServiceAvailabilityChange(reqStatus);

	//services 1 is unavailable
	serviceIds.Reset();
	notifyMbmsServiceAvailabilityChangeCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId, KErrGeneral, data);
	data.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);
	AssertMockLtsyStatusL();
		
	//check the results (no change)
	getMbmsMonitorServiceListAO->TestRetrieveL();
	ASSERT_EQUALS(getMbmsMonitorServiceListAO->CheckResultsL(*list), static_cast<TBool>(ETrue));

	
	//test cancel for code coverage (CTSY test)
	getMbmsMonitorServiceListAO->TestCancel();	
	
	//check #ifdef defect fix of PopAndDestroy when iMBMSMonitoredList is null
	packetService.Close();
	RPacketService packetService2;
	OpenPacketServiceLC(packetService2,RPacketService::EStatusUnattached);
	
	packetService2.NotifyMbmsServiceAvailabilityChange(reqStatus);

	//services 1 is available
	serviceIds.Reset();
	notifyMbmsServiceAvailabilityChangeCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId, KErrNone, data);
	data.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();


	CleanupStack::PopAndDestroy(&packetService2);
	CleanupStack::PopAndDestroy(&serviceIds);
	CleanupStack::PopAndDestroy(list);
	CleanupStack::PopAndDestroy(nullList);
	CleanupStack::PopAndDestroy(getMbmsMonitorServiceListAO);
	CleanupStack::PopAndDestroy(&packetService);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00022
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestPriority High
@SYMTestActions Invokes UpdateMbmsSessionList
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00022L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	// Open new Mbms context
	TInfoName contextName;
	RPacketMbmsContext packetMbmsContext;
	_LIT(KCHANNEL1,"CHANNEL1");
	OpenInitialiseMbmsContextLC(packetService,packetMbmsContext,contextName,KCHANNEL1,KErrNone);

	// retriever for the session list (to test results)
	CGetMbmsSessionListAO* getMbmsSessionListAO = CGetMbmsSessionListAO::NewLC(packetMbmsContext);
	
	RPacketMbmsContext::CMbmsSession* sessionIdList = RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(sessionIdList);

	//test adding in KErrNone complete case
	TDesC* contextNamePtr = &contextName;
	TMbmsAction action;
	TMbmsSessionId session;
	TMockLtsyData4<TDesC*,TMbmsAction,TMbmsSessionId,TMbmsSessionIdList> updateMbmsSessionListExpData(contextNamePtr,action,session,sessionIdList->iSessionIdList);

	TMockLtsyData1<TDesC*> updateMbmsSessionListCompData(contextNamePtr);

	for (session = 0; session < 10; ++session)
		{
		action = EAddEntries;
		
		updateMbmsSessionListExpData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, data);
		data.Close();
		
		updateMbmsSessionListCompData.SerialiseL(data);
		iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, KErrNone, data);
		data.Close();

		packetMbmsContext.UpdateMbmsSessionList(reqStatus, action, session);    
		User::WaitForRequest(reqStatus);
		ASSERT_EQUALS(reqStatus.Int(), KErrNone);
		AssertMockLtsyStatusL();
		
		//check the results
		sessionIdList->iSessionIdList.AppendL(session);
		getMbmsSessionListAO->TestRetrieveL();
		getMbmsSessionListAO->CheckResultsL(*sessionIdList);		
		}
	
	//test adding in KErrGeneral complete case
	action = EAddEntries;
	session += 1;
	updateMbmsSessionListExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, data);
	data.Close();
	
	updateMbmsSessionListCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, KErrGeneral, data);
	data.Close();

	packetMbmsContext.UpdateMbmsSessionList(reqStatus, action, session);    
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);
	AssertMockLtsyStatusL();
	
	//check the results
	getMbmsSessionListAO->TestRetrieveL();
	getMbmsSessionListAO->CheckResultsL(*sessionIdList);		

	//test remove in KErrNone complete case
	action = ERemoveEntries;
	session = 0;
	updateMbmsSessionListExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, data);
	data.Close();
	
	updateMbmsSessionListCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, KErrNone, data);
	data.Close();

	packetMbmsContext.UpdateMbmsSessionList(reqStatus, action, session);    
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();
	
	//check the results
	getMbmsSessionListAO->TestRetrieveL();
	sessionIdList->iSessionIdList.Remove(session);
	getMbmsSessionListAO->CheckResultsL(*sessionIdList);		


	//test removeall in KErrNone complete case
	action = ERemoveAllEntries;
	session = 0;
	updateMbmsSessionListExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, data);
	data.Close();
	
	updateMbmsSessionListCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, KErrNone, data);
	data.Close();

	packetMbmsContext.UpdateMbmsSessionList(reqStatus, action, session);    
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	AssertMockLtsyStatusL();
	
	//check the results
	getMbmsSessionListAO->TestRetrieveL();
	sessionIdList->iSessionIdList.Reset();
	getMbmsSessionListAO->CheckResultsL(*sessionIdList);		
	
	//for coverage
	getMbmsSessionListAO->TestCancel();
	
	CleanupStack::PopAndDestroy(sessionIdList);
	CleanupStack::PopAndDestroy(getMbmsSessionListAO);
	CleanupStack::PopAndDestroy(&packetMbmsContext);
	CleanupStack::PopAndDestroy(&packetService);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00023
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyConnectionInfoChange
@SYMTestPriority High
@SYMTestActions Invokes NotifyConnectionInfoChange, GetConnectionInfo
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00023L()
	{				
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
		
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);
		
	TInfoName contextName;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextName);
	CleanupClosePushL(packetContext);
	
	TRequestStatus notifyRequestStatus;	
	TRequestStatus mockLtsyStatus;	
	TRequestStatus requestStatus;
    RPacketContext::TConnectionInfoV1 notifyConnectionInfoV1;		
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;

    //Initialize connectionInfoV1
    notifyConnectionInfoV1.iValid = 0;
    notifyConnectionInfoV1.iHSDPACategory = 0;
    notifyConnectionInfoV1.iHSUPACategory = 0;	
    
    TPckg<RPacketContext::TConnectionInfoV1> pckgNotifyConnectionInfoV1(notifyConnectionInfoV1);     
	packetContext.NotifyConnectionInfoChange(notifyRequestStatus, pckgNotifyConnectionInfoV1);
    
	//-------------------------------------------------------------------------
	// TEST A1: Successful completion of RPacketContext::GetConnectionInfo
	// request.
	//-------------------------------------------------------------------------
	 	
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1(reqConnectionInfoV1); 
		    	
	//Get connection info
	packetContext.GetConnectionInfo(requestStatus, pckgReqConnectionInfoV1);	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
		
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1, reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS((TUint32)0, reqConnectionInfoV1.iValid);

	//-------------------------------------------------------------------------
	// TEST A2: Successful completion of RPacketContext::GetConnectionInfo
	// request where RPacketContext::TConnectionInfoV1 parameter contains 
	// valid data.
	//-------------------------------------------------------------------------
		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TDesC* contextNamePtr = &contextName;
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory;
	respConnectionInfoV1.iHSDPACategory = 5;
	respConnectionInfoV1.iValid |= RPacketContext::KHSUPACategory;
	respConnectionInfoV1.iHSUPACategory = 6;	    	
	TMockLtsyData2< TDesC*, RPacketContext::TConnectionInfoV1 > ltsyData(contextNamePtr, respConnectionInfoV1);
	ltsyData.SerialiseL(data);
	
	// Change connection information
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyConnectionInfoChangeIndId, KErrNone, data);
	data.Close();
	
	User::WaitForRequest(notifyRequestStatus);
	ASSERT_EQUALS(KErrNone, notifyRequestStatus.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   notifyConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          notifyConnectionInfoV1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  notifyConnectionInfoV1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  notifyConnectionInfoV1.iHSUPACategory);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//Get connection info
	packetContext.GetConnectionInfo(requestStatus, pckgReqConnectionInfoV1);
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
		
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1.iHSUPACategory);

	//close context
	DriverDeleteContextL(contextName);
	packetContext.Close();
	packetService.Close();	
	
	CleanupStack::PopAndDestroy(&packetContext);
	CleanupStack::PopAndDestroy(&packetService);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
#endif	
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00024
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test conversions function CPacketServicesDispatcher::EtelPcktR99R4NegToEtelPcktR97NegQoS
@SYMTestPriority High
@SYMTestActions Invokes EtelPcktR99R4NegToEtelPcktR97NegQoS
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00024L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);

	DriverInitialiseContextL();	
	
	// R99_R4 Configuration
	RPacketQoS::TQoSR99_R4Requested expectedPacketR99_R4Profile;
	expectedPacketR99_R4Profile.iMinTrafficClass = RPacketQoS::ETrafficClassConversational;
	expectedPacketR99_R4Profile.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	expectedPacketR99_R4Profile.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	expectedPacketR99_R4Profile.iMaxBER = RPacketQoS::EBERUnspecified;
	expectedPacketR99_R4Profile.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	expectedPacketR99_R4Profile.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	expectedPacketR99_R4Profile.iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	expectedPacketR99_R4Profile.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	expectedPacketR99_R4Profile.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	expectedPacketR99_R4Profile.iReqBER = RPacketQoS::EBERUnspecified;
	expectedPacketR99_R4Profile.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	expectedPacketR99_R4Profile.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	expectedPacketR99_R4Profile.iMaxTransferDelay = 0;
	expectedPacketR99_R4Profile.iMinAcceptableMaxRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iMinAcceptableMaxRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iMinAcceptableMaxSDUSize = 0;
	expectedPacketR99_R4Profile.iMinGuaranteedRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iMinGuaranteedRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iReqGuaranteedRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iReqGuaranteedRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iReqMaxRate.iDownlinkRate = 0;
	expectedPacketR99_R4Profile.iReqMaxRate.iUplinkRate = 0;
	expectedPacketR99_R4Profile.iReqMaxSDUSize = 0;
	expectedPacketR99_R4Profile.iReqTransferDelay = 0;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Requested> packetSetConfigR99_R4Data(contextId,
																					expectedPacketR99_R4Profile);
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpR99_R4ContextNameData(contextId);
	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	RPacketQoS::TQoSR99_R4Negotiated completedProfileR99;
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassConversational;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Negotiated> expLtsyQosProfileR99ChangedInd(contextId, completedProfileR99);
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassStreaming;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassInteractive;
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassInteractive;
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassInteractive;
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassBackground;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundredThousand;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
	completedProfileR99.iBER = RPacketQoS::EBEROnePerHundredThousand;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iBER = RPacketQoS::EBEROnePerThousand;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 10;
	completedProfileR99.iMaxRate.iUplinkRate = 10;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 11;
	completedProfileR99.iMaxRate.iUplinkRate = 11;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 16;
	completedProfileR99.iMaxRate.iUplinkRate = 16;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 17;
	completedProfileR99.iMaxRate.iUplinkRate = 17;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 32;
	completedProfileR99.iMaxRate.iUplinkRate = 32;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);			
		
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 34;
	completedProfileR99.iMaxRate.iUplinkRate = 34;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 64;
	completedProfileR99.iMaxRate.iUplinkRate = 64;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 127;
	completedProfileR99.iMaxRate.iUplinkRate = 127;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 128;
	completedProfileR99.iMaxRate.iUplinkRate = 128;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 255;
	completedProfileR99.iMaxRate.iUplinkRate = 255;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 256;
	completedProfileR99.iMaxRate.iUplinkRate = 256;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 511;
	completedProfileR99.iMaxRate.iUplinkRate = 511;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 512;
	completedProfileR99.iMaxRate.iUplinkRate = 512;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 1023;
	completedProfileR99.iMaxRate.iUplinkRate = 1023;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 1024;
	completedProfileR99.iMaxRate.iUplinkRate = 1024;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 2047;
	completedProfileR99.iMaxRate.iUplinkRate = 2047;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iMaxRate.iDownlinkRate = 2048;
	completedProfileR99.iMaxRate.iUplinkRate = 2048;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);		
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	data.Close();
	packetSetConfigR99_R4Data.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, data, KErrNone);

	callbackSetPdpR99_R4ContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, KErrNone, data);
	
	completedProfileR99.iTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	data.Close();
	expLtsyQosProfileR99ChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	DriverDeleteContextL(contextId);

	//

	// client side test
	RPacketContext packetContext;

	// Open a new context
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now depreciated

	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);	
		
	RPacketQoS::TQoSR99_R4Requested packetR99_R4;
	packetR99_R4.iMinTrafficClass = RPacketQoS::ETrafficClassConversational;
	packetR99_R4.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	packetR99_R4.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	packetR99_R4.iMaxBER = RPacketQoS::EBERUnspecified;
	packetR99_R4.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	packetR99_R4.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	packetR99_R4.iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	packetR99_R4.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	packetR99_R4.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	packetR99_R4.iReqBER = RPacketQoS::EBERUnspecified;
	packetR99_R4.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	packetR99_R4.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	packetR99_R4.iMaxTransferDelay = 0;
	packetR99_R4.iMinAcceptableMaxRate.iDownlinkRate = 0;
	packetR99_R4.iMinAcceptableMaxRate.iUplinkRate = 0;
	packetR99_R4.iMinAcceptableMaxSDUSize = 0;
	packetR99_R4.iMinGuaranteedRate.iDownlinkRate = 0;
	packetR99_R4.iMinGuaranteedRate.iUplinkRate = 0;
	packetR99_R4.iReqGuaranteedRate.iDownlinkRate = 0;
	packetR99_R4.iReqGuaranteedRate.iUplinkRate = 0;
	packetR99_R4.iReqMaxRate.iDownlinkRate = 0;
	packetR99_R4.iReqMaxRate.iUplinkRate = 0;
	packetR99_R4.iReqMaxSDUSize = 0;
	packetR99_R4.iReqTransferDelay = 0;
	TPckg< RPacketQoS::TQoSR99_R4Requested > profilePckgR99_R4(packetR99_R4);
		
	TRequestStatus requestStatusR99_R4;
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	TRequestStatus requestStatusGPRS;
	RPacketQoS::TQoSGPRSNegotiated qosGPRSNegotiatedProfile;
    TPckg<RPacketQoS::TQoSGPRSNegotiated> profilePckgGPRS(qosGPRSNegotiatedProfile);	
	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EDelayClass1, qosGPRSNegotiatedProfile.iDelay);
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());
	
	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EDelayClass1, qosGPRSNegotiatedProfile.iDelay);	
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());		

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EDelayClass1, qosGPRSNegotiatedProfile.iDelay);	
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EDelayClass2, qosGPRSNegotiatedProfile.iDelay);
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EDelayClass3, qosGPRSNegotiatedProfile.iDelay);	
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EDelayClass4, qosGPRSNegotiatedProfile.iDelay);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EUnspecifiedDelayClass, qosGPRSNegotiatedProfile.iDelay);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EReliabilityClass2, qosGPRSNegotiatedProfile.iReliability);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EReliabilityClass3, qosGPRSNegotiatedProfile.iReliability);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EReliabilityClass4, qosGPRSNegotiatedProfile.iReliability);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EReliabilityClass5, qosGPRSNegotiatedProfile.iReliability);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EUnspecifiedReliabilityClass, qosGPRSNegotiatedProfile.iReliability);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput1000, qosGPRSNegotiatedProfile.iPeakThroughput);	
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput1000, qosGPRSNegotiatedProfile.iPeakThroughput);		
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());
	
	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput2000, qosGPRSNegotiatedProfile.iPeakThroughput);		
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput2000, qosGPRSNegotiatedProfile.iPeakThroughput);		
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput4000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput4000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput8000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput8000, qosGPRSNegotiatedProfile.iPeakThroughput);		
			
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput16000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput16000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput32000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput32000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());		

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput64000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput64000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput128000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput128000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPeakThroughput256000, qosGPRSNegotiatedProfile.iPeakThroughput);		
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPriorityHighPrecedence, qosGPRSNegotiatedProfile.iPrecedence);		
	
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPriorityMediumPrecedence, qosGPRSNegotiatedProfile.iPrecedence);		
			
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());

	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EPriorityLowPrecedence, qosGPRSNegotiatedProfile.iPrecedence);	
		
	packetQoS.SetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(KErrNone, requestStatusR99_R4.Int());	
	
	packetQoS.GetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);	
	ASSERT_EQUALS(RPacketQoS::EUnspecifiedPrecedence, qosGPRSNegotiatedProfile.iPrecedence);	

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // data, data,  packetService, packetContext, this	
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00025
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test conversions function CPacketServicesDispatcher::EtelPcktR97NegToEtelPcktR99R4NegQoS
@SYMTestPriority High
@SYMTestActions Invokes EtelPcktR97NegToEtelPcktR99R4NegQoS
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00025L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RBuf8 data;
	CleanupClosePushL(data);

	TInfoName contextId;
	contextId.Copy(KFirstAllocatedContextName);

	//

	DriverInitialiseContextL();

	// GPRS Configuration
	RPacketQoS::TQoSGPRSRequested qosExpectedRequestedGPRSProfile;
	qosExpectedRequestedGPRSProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosExpectedRequestedGPRSProfile.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosExpectedRequestedGPRSProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosExpectedRequestedGPRSProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosExpectedRequestedGPRSProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosExpectedRequestedGPRSProfile.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> packetSetConfigGPRSData(contextId,
																				qosExpectedRequestedGPRSProfile);
	data.Close();
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextId);
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	RPacketQoS::TQoSGPRSNegotiated completedProfileGPRS;
	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass1;	
	TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSNegotiated> expLtsyQosProfileGPRSChangedInd(contextId, completedProfileGPRS);
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);

	data.Close();	
	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass2;	
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
		
	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass3;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
		
	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass4;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
		
	completedProfileGPRS.iDelay = RPacketQoS::EUnspecifiedDelayClass;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
		
	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass1;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass2;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iDelay = RPacketQoS::EDelayClass3;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iDelay = RPacketQoS::EUnspecifiedDelayClass;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass1;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass2;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass3;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass4;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass5;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass1;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);
	
	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass2;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass3;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass4;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass5;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass1;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass2;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass3;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass4;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EReliabilityClass5;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput1000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput2000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput4000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput8000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput16000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput32000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput64000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput128000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput256000;
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	

	data.Close();	
	packetSetConfigGPRSData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	callbackSetPdpGPRSContextNameData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

	completedProfileGPRS.iPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;	
	data.Close();
	expLtsyQosProfileGPRSChangedInd.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId, KErrNone, data);	
	
	DriverDeleteContextL(contextId);

	//	
	
	// client side test
	RPacketContext packetContext;

	// Open a new context
	TInt err = packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	ASSERT_EQUALS(KErrNone, err);

	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);
	TRequestStatus initialiseContextStatus;

	packetContext.InitialiseContext(initialiseContextStatus,dataChannelPk);
	User::WaitForRequest(initialiseContextStatus);
	ASSERT_EQUALS(KErrNone, initialiseContextStatus.Int());
	ASSERT_TRUE(dataChannel.iPort == KFirstAllocatedContextName);
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId); // Now depreciated

	TName qosName;
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);

	//
    // SetProfileParameters for GPRS Configurations
    //

	RPacketQoS::TQoSGPRSRequested qosGPRSRequestedProfile;
	qosGPRSRequestedProfile.iReqPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
	qosGPRSRequestedProfile.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
	qosGPRSRequestedProfile.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
	qosGPRSRequestedProfile.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
	qosGPRSRequestedProfile.iReqMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
	qosGPRSRequestedProfile.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
    TPckg<RPacketQoS::TQoSGPRSRequested> profilePckgGPRS(qosGPRSRequestedProfile);
    
	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	TRequestStatus requestStatusR99_R4;
	RPacketQoS::TQoSR99_R4Negotiated qosR99_R4NegotiatedProfile;
    TPckg<RPacketQoS::TQoSR99_R4Negotiated> profilePckgR99_R4(qosR99_R4NegotiatedProfile);	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);
	ASSERT_EQUALS(RPacketQoS::ETrafficClassInteractive, qosR99_R4NegotiatedProfile.iTrafficClass);	

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficClassInteractive, qosR99_R4NegotiatedProfile.iTrafficClass);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficClassInteractive, qosR99_R4NegotiatedProfile.iTrafficClass);	

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficClassBackground, qosR99_R4NegotiatedProfile.iTrafficClass);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficClassUnspecified, qosR99_R4NegotiatedProfile.iTrafficClass);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficPriority1, qosR99_R4NegotiatedProfile.iTrafficHandlingPriority);
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficPriority2, qosR99_R4NegotiatedProfile.iTrafficHandlingPriority);
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficPriority3, qosR99_R4NegotiatedProfile.iTrafficHandlingPriority);	
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ETrafficPriorityUnspecified, qosR99_R4NegotiatedProfile.iTrafficHandlingPriority);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ESDUErrorRatioOnePerHundredThousand, qosR99_R4NegotiatedProfile.iSDUErrorRatio);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ESDUErrorRatioOnePerHundredThousand, qosR99_R4NegotiatedProfile.iSDUErrorRatio);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ESDUErrorRatioOnePerTenThousand, qosR99_R4NegotiatedProfile.iSDUErrorRatio);	
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ESDUErrorRatioOnePerThousand, qosR99_R4NegotiatedProfile.iSDUErrorRatio);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ESDUErrorRatioOnePerThousand, qosR99_R4NegotiatedProfile.iSDUErrorRatio);	
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::ESDUErrorRatioUnspecified, qosR99_R4NegotiatedProfile.iSDUErrorRatio);	
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EBEROnePerHundredThousand, qosR99_R4NegotiatedProfile.iBER);	
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EBEROnePerHundredThousand, qosR99_R4NegotiatedProfile.iBER);		
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EBEROnePerHundredThousand, qosR99_R4NegotiatedProfile.iBER);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EBEROnePerHundredThousand, qosR99_R4NegotiatedProfile.iBER);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EBERFourPerThousand, qosR99_R4NegotiatedProfile.iBER);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EBERUnspecified, qosR99_R4NegotiatedProfile.iBER);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EErroneousSDUDeliveryNotRequired, qosR99_R4NegotiatedProfile.iDeliverErroneousSDU);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EErroneousSDUDeliveryNotRequired, qosR99_R4NegotiatedProfile.iDeliverErroneousSDU);		
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EErroneousSDUDeliveryNotRequired, qosR99_R4NegotiatedProfile.iDeliverErroneousSDU);		
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EErroneousSDUDeliveryNotRequired, qosR99_R4NegotiatedProfile.iDeliverErroneousSDU);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EErroneousSDUDeliveryRequired, qosR99_R4NegotiatedProfile.iDeliverErroneousSDU);		

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(RPacketQoS::EErroneousSDUDeliveryUnspecified, qosR99_R4NegotiatedProfile.iDeliverErroneousSDU);		

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(8, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(16, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(32, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(64, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		
		
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(128, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());
	
	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(256, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		
	
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(512, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(1024, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(2048, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		

	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	packetQoS.GetProfileParameters(requestStatusR99_R4, profilePckgR99_R4);
	User::WaitForRequest(requestStatusR99_R4);	
	ASSERT_EQUALS(0, qosR99_R4NegotiatedProfile.iMaxRate.iDownlinkRate);		

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();

	// Let the mock-LTSY finish up before asserting.
	User::After(1000000);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // data, data, packetService, packetContext, this		
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00026
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetPreferredBearer
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetPreferredBearer
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsyPacketServicesFU::TestUnit00026L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RPacketService packetService;
    OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
    
    //-------------------------------------------------------------------------
    // TEST D: RPacketService::GetPreferredBearer again, this time CTSY
    // will get result from the cache.
    //-------------------------------------------------------------------------         
    TRequestStatus  requestStatus;
    RPacketService::TPreferredBearer bearer;
    packetService.GetPreferredBearer(requestStatus, bearer);
    User::WaitForRequest(requestStatus);
    
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RPacketService::EBearerPacketSwitched, bearer);
    
    // --- change PreferredBearer -----------------------------------------------
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    // data for setting
    RPacketService::TPreferredBearer newBearer = RPacketService::EBearerCircuitSwitched;
    TMockLtsyData1 <RPacketService::TPreferredBearer> ltsyData(newBearer);
    ltsyData.SerialiseL(data);      
    
    iMockLTSY.CompleteL(KMockLtsyDispatchPacketServicesNotifyPacketAttachModeChangeIndId, KErrNone, data); 
    User::WaitForRequest(mockLtsyStatus);   
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  
        
    //-------------------------------------------------------------------------
    // TEST D2: RPacketService::GetPreferredBearer again, this time CTSY
    // will get result from the cache.
    //-------------------------------------------------------------------------  
    packetService.GetPreferredBearer(requestStatus, bearer);
    User::WaitForRequest(requestStatus);
    
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(newBearer, bearer);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(1); // packetService
    CleanupStack::PopAndDestroy(2, this); // data, this
    }
