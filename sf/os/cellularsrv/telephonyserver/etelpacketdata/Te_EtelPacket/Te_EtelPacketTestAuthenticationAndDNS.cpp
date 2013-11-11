// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTestAuthenticationAndDNS.cpp
// This is the cpp file for Packet Data Context test Authentication and DNS Server names
// Change Request: PHAR-5F6JUL .
// File created 19-02-03.
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTestAuthenticationAndDNS.h"
#include "Te_etelpckt_defs.h"

// constructor
CEtelPacketTestAuthenticationAndDNS::CEtelPacketTestAuthenticationAndDNS()
{
	// store the name of this test case
	SetTestStepName(_L("Authentication_And_DNS_Server_Names"));
}

// destructor
CEtelPacketTestAuthenticationAndDNS::~CEtelPacketTestAuthenticationAndDNS()
{
}


enum TVerdict CEtelPacketTestAuthenticationAndDNS::doTestStepL( void )
/**
 * Test step Extended_Service tests new DNS server and protocol configuration
 * related data setting
 */
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);

	RPacketService packetService;
	ret=packetService.Open(phone);

	RPacketContext r99Context;
	TName contextName;
	ret=r99Context.OpenNewContext(packetService, contextName);

	TRequestStatus reqStatus; 

	//Set configuration data, including DNS server names and protocol configuration information
	RPacketContext::TContextConfigR99_R4 rel99ContextConfig;
	rel99ContextConfig.ExtensionId(),TPacketDataConfigBase::KConfigRel99Rel4;
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckgRel99(rel99ContextConfig);

	rel99ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel99ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel99ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel99ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
	rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns=DPCKTTSY_DNS_SERVER1;
	rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns=DPCKTTSY_DNS_SERVER2;
	rel99ContextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
	rel99ContextConfig.iPFI = DPCKTTSY_PFICONTEXT_SMS;

	packetService.SetDefaultContextParams(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	packetService.GetDefaultContextParams(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(rel99ContextConfig.iAccessPointName==DPCKTTSY_ACCESS_POINT2);
	TEST(rel99ContextConfig.iPdpAddress==DPCKTTSY_PDP_ADDRESS2);
	TEST(rel99ContextConfig.iPdpType==DPCKTTSY_PDP_TYPE2);
	TEST(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername==DPCKTTSY_USER_NAME1);
	TEST(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword==DPCKTTSY_PASSWORD1);
	TEST(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol==DPCKTTSY_PROTOCOL_TYPE1_0);

	// now test the getting of DNS server names
	RPacketContext::TDnsInfoV2 dnsInfoV2;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoV2pckg(dnsInfoV2);

	r99Context.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	User::WaitForRequest(reqStatus); 
	
	TEST(dnsInfoV2.iPrimaryDns==DPCKTTSY_DNS_SERVER1);
	TEST(dnsInfoV2.iSecondaryDns==DPCKTTSY_DNS_SERVER2);

	TEST(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfo Test Passed"));

	// now test cancel of retrieval of DNS server names
	r99Context.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	r99Context.CancelAsyncRequest(EPacketContextGetDNSInfo);
	User::WaitForRequest(reqStatus); 
	TEST(reqStatus.Int()==KErrCancel); 
	INFO_PRINTF1(_L("GetDnsInfoCancel Test Passed"));

	RPacketContext::TContextConfigGPRS contextConfig;

	RPacketContext gprsContext;
	TName secondContextName;
	ret=gprsContext.OpenNewContext(packetService, secondContextName);

	contextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	contextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	contextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	contextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	contextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	contextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	contextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
	contextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
	contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
	contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns=DPCKTTSY_DNS_SERVER1;
	contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns=DPCKTTSY_DNS_SERVER2;

	RPacketContext::TDnsInfoV2 gprsDnsInfoV2;
	RPacketContext::TTDnsInfoV2Pckg gprsDnsInfoV2pckg(gprsDnsInfoV2);

	gprsContext.GetDnsInfo(reqStatus, gprsDnsInfoV2pckg);	
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfo Test Passed"));
	
	// Start sanity test for
	// REQ3880 Enable SBLP Control Procedures

	RPacketContext r5Context;
	TName r5contextName;
	ret=r5Context.OpenNewContext(packetService, r5contextName);
	TRequestStatus aStatus;
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthV3 = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuthV3);
	RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier iFlowIdentifier;
	((*mediaAuthV3).iAuthorizationToken).Copy(DPCKTTSY_AUTH_TOKEN);
	iFlowIdentifier.iMediaComponentNumber = DPCKTTSY_COMP_NUMBER1;
	iFlowIdentifier.iIPFlowNumber = DPCKTTSY_IPFLOW_NUMBER1;
	(*mediaAuthV3).iFlowIds.Append(iFlowIdentifier);
	iFlowIdentifier.iMediaComponentNumber = DPCKTTSY_COMP_NUMBER2;
	iFlowIdentifier.iIPFlowNumber = DPCKTTSY_IPFLOW_NUMBER2;
	(*mediaAuthV3).iFlowIds.Append(iFlowIdentifier);
	
	// Testing Externalize and Internalize
	HBufC8* aTestBuffer;
	aTestBuffer = NULL;
	mediaAuthV3->ExternalizeL(aTestBuffer);
	TPtr8 testBuf = aTestBuffer->Des();
	(*mediaAuthV3).iAuthorizationToken = _L8("");
	mediaAuthV3->iFlowIds.Reset();
	mediaAuthV3->InternalizeL(testBuf);
	TEST(!(mediaAuthV3->iAuthorizationToken).Compare(DPCKTTSY_AUTH_TOKEN));
	TEST(mediaAuthV3->iFlowIds[0].iMediaComponentNumber == DPCKTTSY_COMP_NUMBER1);
	TEST(mediaAuthV3->iFlowIds[0].iIPFlowNumber == DPCKTTSY_IPFLOW_NUMBER1);
	TEST(mediaAuthV3->iFlowIds[1].iMediaComponentNumber == DPCKTTSY_COMP_NUMBER2);
	TEST(mediaAuthV3->iFlowIds[1].iIPFlowNumber == DPCKTTSY_IPFLOW_NUMBER2);
	
	//OOM test for internalize
	TInt err=KErrNone;
	TInt len=0;
	do
		{
		testBuf.SetLength(len);
		TRAP(err, mediaAuthV3->InternalizeL(testBuf));
		if(err == KErrOverflow)
			{
			len++;	
			}
		else
			{
			TEST(err == KErrNone);
			break;
			}		
		}
	while(len < 100); //just in case, set a limit to finish the loop.
	TEST(len<100);
	
	
	// Testing Internalize with empty buffer
//	testBuf.SetLength(0);
//	mediaAuthV3->InternalizeL(testBuf);
	
	r5Context.AddMediaAuthorizationL(aStatus, *mediaAuthV3);
	User::WaitForRequest(aStatus);
	TEST(aStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("AddMediaAuthorization Test Passed"));
	
	r5Context.RemoveMediaAuthorization(aStatus, (*mediaAuthV3).iAuthorizationToken);
	User::WaitForRequest(aStatus);
	TEST(aStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("RemoveMediaAuthorization Test Passed"));
	
	r5Context.AddMediaAuthorizationL(aStatus, *mediaAuthV3);
	r5Context.CancelAsyncRequest(EPacketAddMediaAuthorization);
	User::WaitForRequest(aStatus); 
	TEST(aStatus.Int()==KErrCancel); 
	INFO_PRINTF1(_L("AddMediaAuthorizationCancel Test Passed"));
	
	r5Context.RemoveMediaAuthorization(aStatus, (*mediaAuthV3).iAuthorizationToken);
	r5Context.CancelAsyncRequest(EPacketRemoveMediaAuthorization);
	User::WaitForRequest(aStatus); 
	TEST(aStatus.Int()==KErrCancel); 
	INFO_PRINTF1(_L("RemoveMediaAuthorizationCancel Test Passed"));
	
	CleanupStack::PopAndDestroy(mediaAuthV3);
	delete aTestBuffer;
	aTestBuffer = NULL;
	r5Context.Close();
	
	// End sanity test for
	// REQ3880 Enable SBLP Control Procedures

	gprsContext.Close();
	r99Context.Close();
	packetService.Close();
	phone.Close();

	return TestStepResult();
}

