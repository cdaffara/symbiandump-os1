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
 @file The TEFUnit test suite for CallControl in the Common TSY.
*/

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>

#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>
#include "config.h"
#include "mockltsyindicatorids.h"
#include <in_sock.h>

#include "cctsypacketservicesfunegative.h"

void CCTsyPacketServicesFUNegative::ConvertDNSToPCOBuffer
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
void CCTsyPacketServicesFUNegative::ConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, 
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
void CCTsyPacketServicesFUNegative::PCOGeneratorL(RPacketContext::TProtocolConfigOptionV2& aPco, RPacketContext::TMiscProtocolBuffer& aPcoBuffer)
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

void CCTsyPacketServicesFUNegative::DriverDeleteContextL(TInfoName aContextName)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	TMockLtsyData1<TInfoName> contextNameData(aContextName);
	data.Close();
	contextNameData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, data, KErrNone);
	CleanupStack::PopAndDestroy(1, &data); // data
	}

void CCTsyPacketServicesFUNegative::DriverAttachPacketServiceL()
	{
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId, KErrNone);
	}

void CCTsyPacketServicesFUNegative::DriverDetachPacketServiceL()
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

void CCTsyPacketServicesFUNegative::DriverInitialiseContextL()
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

void CCTsyPacketServicesFUNegative::DriverInitialiseContextL(const TInfoName& aPrimaryContextId, const TInfoName& aSecondaryContextId)
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

void CCTsyPacketServicesFUNegative::DriverSetConfigContextL(TInfoName aContextId,
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

void CCTsyPacketServicesFUNegative::DriverModifyActiveContextL(TInt8 aRejectionCode, TInt aError)
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

void CCTsyPacketServicesFUNegative::OpenInitialiseMbmsContextLC(RPacketService& aPacketService, RPacketMbmsContext& aPacketContext, TDes& aContextName, const TDesC& aChannelId, TInt aCompError)
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

	TRequestStatus requestStatus;
    aPacketContext.InitialiseContext(requestStatus, dataChannelPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), aCompError);
	ASSERT_TRUE((aCompError != KErrNone) || (dataChannel.iChannelId == aChannelId));
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&data);
	}

CTestSuite* CCTsyPacketServicesFUNegative::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
  	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0016L);

	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0018L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0019L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0020L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0021L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0022L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0023L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0024L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServicesFUNegative, TestUnit0025L);
	END_SUITE;
	}

//
// 'Negative' unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketAttach is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Attach()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0001L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);

    packetService.Attach(requestStatus);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrGsmMMNetworkFailure, requestStatus.Int());
	
	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketGetAttachMode is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetAttachMode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0002L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
	
	RPacketService::TAttachMode completedAttachMode;
	
	packetService.GetAttachMode(requestStatus, completedAttachMode);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketGetNtwkRegStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetNtwkRegStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0003L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
	
	RPacketService::TRegistrationStatus registrationStatus;

	packetService.GetNtwkRegStatus(requestStatus, registrationStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketDetach is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Detach()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0004L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
	
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    
    TRequestStatus requestStatus;
    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGsmMMNetworkFailure, requestStatus.Int());

	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextSetConfig is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::SetConfig()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0005L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, EFalse);

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
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId);

	TRequestStatus setConfigStatus;

	packetContext.SetConfig(setConfigStatus, pckgConfigGPRS);
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNotSupported, setConfigStatus.Int());

	packetContext.Close();
	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextModifyActiveContext is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::ModifyActiveContext()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0006L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, EFalse);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId);

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
	ASSERT_EQUALS(KErrNotSupported, modifyActiveContextStatus.Int());

	TRequestStatus deactivateContextStatus;
	packetContext.Deactivate(deactivateContextStatus);
	User::WaitForRequest(deactivateContextStatus);
	ASSERT_EQUALS(KErrNone, deactivateContextStatus.Int())

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextInitialiseContext is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::InitialiseContext()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0007L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);

	TInfoName contextName;
	contextName.Copy(KFirstAllocatedContextName);
	
	DriverDeleteContextL(contextName);
	
	//Open a New Packet Context
	RPacketContext packetContext;
	CleanupClosePushL(packetContext);

	TInt err = packetContext.OpenNewContext(packetService,contextName);
	ASSERT_EQUALS(KErrNone, err);

	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataChannel;
	RPacketContext::TDataChannelV2Pckg dataChannelPk(dataChannel);

	packetContext.InitialiseContext(requestStatus,dataChannelPk);
	User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	packetContext.Close();
	AssertMockLtsyStatusL();
	config.Reset();	
	CleanupStack::PopAndDestroy(3, this); // packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextDelete is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Delete()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0008L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId);

	TRequestStatus setConfigStatus;

	packetContext.Delete(setConfigStatus);
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNotSupported, setConfigStatus.Int());

	// Close is not called since it invokes EPacketContextDelete
	//	packetContext.Close();
	//	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketSetAttachMode is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetAttachMode()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0009L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
	
	TRequestStatus requestStatus;
	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
    
	packetService.SetAttachMode(requestStatus, attachMode);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

    AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketNotifyStatusChange - *Notify* NOT NEEDED
@SYMTestPriority High
@SYMTestActions Invokes 
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0010L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(this); // , this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketSetDefaultContextParams is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetDefaultContextParams()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0011L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, EFalse);
	// or MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceLC(packetService,RPacketService::EStatusUnattached);

	RPacketContext::TContextConfigGPRS defaultGprsContextConfig;
	defaultGprsContextConfig.iAccessPointName = KDefaultAccessPoint;
	defaultGprsContextConfig.iPdpType = KDefaultPdpType;
	TPckg<RPacketContext::TContextConfigGPRS> defaultContextGprsConfigPckg(defaultGprsContextConfig);
	
	TRequestStatus requestStatus;
	packetService.SetDefaultContextParams(requestStatus, defaultContextGprsConfigPckg);
	User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextActivate is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Activate()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0012L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId);

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

	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNone, requestStatusGPRS.Int());

	TRequestStatus requestStatus;
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketQoSSetProfileParams is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::SetProfileParameters()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0013L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, EFalse);
//	or	MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId
//	or	MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
//	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, data, KErrNone);

	TMockLtsyData1<TInfoName> callbackSetPdpGPRSContextNameData(contextName);
	data.Close();
	callbackSetPdpGPRSContextNameData.SerialiseL(data);
//	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, KErrNone, data);

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
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId);

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

	// in CMmPacketQoSGsmWcdmaExt::SetProfileParametersL()
	// resullt of iMmPacketService->MessageManager()->HandleRequestL()
	// is never checked
	TRequestStatus requestStatusGPRS;
	packetQoS.SetProfileParameters(requestStatusGPRS, profilePckgGPRS);
	User::WaitForRequest(requestStatusGPRS);
	ASSERT_EQUALS(KErrNotSupported, requestStatusGPRS.Int());

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketRejectActivationRequest is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::RejectActivationRequest()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0014L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);

	TRequestStatus requestStatus;
	
	packetService.RejectActivationRequest(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, data
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextDeactivate is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Deactivate()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0015L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
	ASSERT_TRUE(dataChannel.iChannelId == KDefaultChannelId);

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

	TRequestStatus requestStatus;
	packetContext.Deactivate(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	packetQoS.Close();
	packetContext.Close();
	packetService.Close();
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(5, this); // data, packetQoS, packetService, packetContext, this
	}
	
/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextAddPacketFilter is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::AddPacketFilter()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0016L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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
	
	TRequestStatus addPacketFilterStatus;

	// Add filter to secondary context.
    secondaryContext.AddPacketFilter(addPacketFilterStatus, packetFilterV2Pckg);	    	    
	User::WaitForRequest(addPacketFilterStatus);
	ASSERT_EQUALS(KErrNotSupported, addPacketFilterStatus.Int());

	TRequestStatus requestStatus;	
	iMockLTSY.NotifyTerminated(requestStatus);
	
	DriverDeleteContextL(secondaryContextId);
	DriverDeleteContextL(contextId);

	secondaryPacketQoS.Close();
	packetQoS.Close();
				
	secondaryContext.Close();
	packetContext.Close();
	packetService.Close();

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNone);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(10, this);   // setSecondaryConfigGPRSActiveNotifyConfigChanged, secondarySetConfigGPRSActive, secondarysetConfigGPRSActiveNotifyConfigChanged, 
											//filterTemporaryArray, secondPacketContext, setConfigGPRSActiveNotifyConfigChanged, data, setConfigGPRS, packetQoS, packetService, packetContext, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0018
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketInitProxiesIPC is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Open() 
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0018L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
    RPacketService packetService;  
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId);	

	TUint staticCapabilities        
		= RPacketService::KCapsSuspendSupported
		| RPacketService::KCapsNetworkAvailabilitySupported
		| RPacketService::KCapsSetDefaultContextSupported
		| RPacketService::KCapsChangeAttachModeSupported
		| RPacketService::KCapsGetDataTransferredSupported
		| RPacketService::KCapsPreferredBearerSupported
		| RPacketService::KCapsPdpDataCompSupported
		| RPacketService::KCapsPdpHeaderCompSupported
		| RPacketService::KCapsMSClassSupported
		| RPacketService::KCapsNotifyMSClassSupported;
	
	TMockLtsyData1<TUint> packetServiceStaticCapabilities(staticCapabilities);
	packetServiceStaticCapabilities.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId, KErrNone, data);
	data.Close();
	
  	TInt maxNoMonitoredServiceLists = 32;
	TMockLtsyData1<TInt> maxNoMonitoredServiceListsData(maxNoMonitoredServiceLists);
	maxNoMonitoredServiceListsData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId, KErrNone, data);
	data.Close();
	
	TInt maxNoActiveServices = 64;
	TMockLtsyData1<TInt> maxNoActiveServicesData(maxNoActiveServices);
	maxNoActiveServicesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId, KErrNone, data);

	CleanupClosePushL(packetService);

	ASSERT_EQUALS(packetService.Open(iPhone), KErrNone);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNone);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0019
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketGetStaticCaps is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetStaticCaps()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0019L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

    RPacketService packetService;       
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId);	

	TBool resumed = EFalse; // Flag indicating whether packet service has been resumed from suspended state.
	RPacketService::TStatus packetServiceStatus = RPacketService::EStatusUnattached;
	TMockLtsyData2<RPacketService::TStatus, TBool> packetServiceStatusData(packetServiceStatus, resumed);
	packetServiceStatusData.SerialiseL(data);
  	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId, KErrNone, data);
  	data.Close();
  	
  	TInt maxNoMonitoredServiceLists = 32;
	TMockLtsyData1<TInt> maxNoMonitoredServiceListsData(maxNoMonitoredServiceLists);
	maxNoMonitoredServiceListsData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId, KErrNone, data);
	data.Close();
	
	TInt maxNoActiveServices = 64;
	TMockLtsyData1<TInt> maxNoActiveServicesData(maxNoActiveServices);
	maxNoActiveServicesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId, KErrNone, data);

	CleanupClosePushL(packetService);
	ASSERT_EQUALS(packetService.Open(iPhone),KErrNone);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	AssertMockLtsyStatusL();
	
	//check the capabilities
	TUint caps = 0;
	TInt ret = packetService.GetStaticCaps(caps,RPacketContext::EPdpTypeIPv4); // still returns KErrNone 
	ASSERT_EQUALS(ret, KErrNone); 
	
	AssertMockLtsyStatusL();	
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0020
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketEnumerateMbmsMonitorServiceList is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateMbmsMonitorServiceList()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0020L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

    RPacketService packetService;       
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId);	

	TUint staticCapabilities        
		= RPacketService::KCapsSuspendSupported
		| RPacketService::KCapsNetworkAvailabilitySupported
		| RPacketService::KCapsSetDefaultContextSupported
		| RPacketService::KCapsChangeAttachModeSupported
		| RPacketService::KCapsGetDataTransferredSupported
		| RPacketService::KCapsPreferredBearerSupported
		| RPacketService::KCapsPdpDataCompSupported
		| RPacketService::KCapsPdpHeaderCompSupported
		| RPacketService::KCapsMSClassSupported
		| RPacketService::KCapsNotifyMSClassSupported;
	
	TMockLtsyData1<TUint> packetServiceStaticCapabilities(staticCapabilities);
	packetServiceStaticCapabilities.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId, KErrNone, data);
	data.Close();
	
	TBool resumed = EFalse; // Flag indicating whether packet service has been resumed from suspended state.
	RPacketService::TStatus packetServiceStatus = RPacketService::EStatusUnattached;
	TMockLtsyData2<RPacketService::TStatus, TBool> packetServiceStatusData(packetServiceStatus, resumed);
	packetServiceStatusData.SerialiseL(data);
  	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId, KErrNone, data);
  	data.Close();
	
	TInt maxNoActiveServices = 64;
	TMockLtsyData1<TInt> maxNoActiveServicesData(maxNoActiveServices);
	maxNoActiveServicesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId, KErrNone, data);

	CleanupClosePushL(packetService);
	ASSERT_EQUALS(packetService.Open(iPhone),KErrNone);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	AssertMockLtsyStatusL();
	
	//check the max no monitored service lists
	TInt count = 0;
	TInt maxAllowed = 0;
	packetService.EnumerateMbmsMonitorServiceList(requestStatus,count,maxAllowed);
	User::WaitForRequest(requestStatus); // still KErrNone
	ASSERT_EQUALS(requestStatus.Int(), KErrNone);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0021
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketEnumerateMbmsActiveServiceList is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateMbmsActiveServiceList()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0021L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

    RPacketService packetService;      
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId);
	iMockLTSY.ExpectL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId);

	TUint staticCapabilities        
		= RPacketService::KCapsSuspendSupported
		| RPacketService::KCapsNetworkAvailabilitySupported
		| RPacketService::KCapsSetDefaultContextSupported
		| RPacketService::KCapsChangeAttachModeSupported
		| RPacketService::KCapsGetDataTransferredSupported
		| RPacketService::KCapsPreferredBearerSupported
		| RPacketService::KCapsPdpDataCompSupported
		| RPacketService::KCapsPdpHeaderCompSupported
		| RPacketService::KCapsMSClassSupported
		| RPacketService::KCapsNotifyMSClassSupported;
	
	TMockLtsyData1<TUint> packetServiceStaticCapabilities(staticCapabilities);
	packetServiceStaticCapabilities.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId, KErrNone, data);
	data.Close();
	
	TBool resumed = EFalse; // Flag indicating whether packet service has been resumed from suspended state.
	RPacketService::TStatus packetServiceStatus = RPacketService::EStatusUnattached;
	TMockLtsyData2<RPacketService::TStatus, TBool> packetServiceStatusData(packetServiceStatus, resumed);
	packetServiceStatusData.SerialiseL(data);
  	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId, KErrNone, data);
  	data.Close();
  	
  	TInt maxNoMonitoredServiceLists = 32;
	TMockLtsyData1<TInt> maxNoMonitoredServiceListsData(maxNoMonitoredServiceLists);
	maxNoMonitoredServiceListsData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId, KErrNone, data);
	data.Close();

	CleanupClosePushL(packetService);
	ASSERT_EQUALS(packetService.Open(iPhone),KErrNone);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	AssertMockLtsyStatusL();

	//check the max no monitored service lists
	TInt count = 0;
	TInt maxAllowed = 0;

	packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 
	User::WaitForRequest(requestStatus); // still KErrNone
	ASSERT_EQUALS(requestStatus.Int(),KErrNone);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0022
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if ECtsyPacketMbmsInitialiseContextReq is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::InitialiseContext()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0022L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
	
	TInfoName contextName;
	RPacketMbmsContext packetContext;
	
	packetContext.OpenNewContext(packetService, contextName);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataChannel1;
	RPacketContext::TDataChannelV2Pckg dataChannelPckg1(dataChannel1);

	packetContext.InitialiseContext(requestStatus, dataChannelPckg1);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);

	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0023
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketGetMbmsNetworkServiceStatus is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetMbmsNetworkServiceStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0023L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
    
	TRequestStatus requestStatus;
    TMbmsNetworkServiceStatus mbmsNetworkServiceStatus;
    
	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0024
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketUpdateMbmsMonitorServiceList is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::UpdateMbmsMonitorServiceListL()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0024L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
    
	TRequestStatus requestStatus;
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	RPacketService::TMbmsServiceAvailabilityV1 entry1, entry2;
	TInt MBMS_MONITORSERVICECOUNTMULTIPLE = 3;
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		entry1.iTmgi.SetServiceId(i); 
		entry1.iTmgi.SetMCC(i);
		entry1.iTmgi.SetMNC(i);
		entry1.iMbmsServiceMode = KBroadcast;
		entry1.iMbmsAccessBearer = E2GBearer;
		entry1.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( entry1 );
		}
	
	packetService.UpdateMbmsMonitorServiceListL(requestStatus, EAddEntries, multipleEntries);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // multipleEntries, packetService, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-NEGATIVE-UN0025
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test returned value if EPacketContextUpdateMbmsSessionList is not supported by LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::UpdateMbmsSessionList()
@SYMTestExpectedResults Pass
@SYMTestType UT
*/
void CCTsyPacketServicesFUNegative::TestUnit0025L()
	{
	TConfig config;
	config.SetSupportedValue(MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId, EFalse);
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
	
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;	
	mbmsContext.OpenNewContext(packetService, contextId);	 
	CleanupClosePushL( mbmsContext );	
    
	TRequestStatus requestStatus;
    TMbmsSessionId session = 0;  
    
    mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(requestStatus.Int(), KErrNotSupported);
	
	AssertMockLtsyStatusL();
	config.Reset();
	CleanupStack::PopAndDestroy(3, this); // mbmsContext, packetService, this	
	}
