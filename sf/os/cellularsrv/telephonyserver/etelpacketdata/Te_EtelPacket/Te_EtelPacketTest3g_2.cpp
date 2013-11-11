// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest3G_2.cpp
// JetStream Packet data tests.
// This file tests the 3G (WCDMA) Packet Context functionality - TestCase 3G.2
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest3g_2.h"
#include "Te_etelpckt_defs.h"

CEtelPacketTest3G_2::CEtelPacketTest3G_2()
/**
 * Constructor
 * Stores the name of this test case.
 */
	{
	SetTestStepName(_L("3GPacketContext"));
	}


CEtelPacketTest3G_2::~CEtelPacketTest3G_2()
/**
 * Destructor
 */
	{}

enum TVerdict CEtelPacketTest3G_2::doTestStepL( void )
/**
 * Test step 3G.2 calls the 3G Packet Context (RPacketContext) methods
 */
{
	RPhone pcktPhone;
	TInt ret=pcktPhone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT(ret,KErrNone,CHP_OPEN_PHONE);

	RPacketService wcdmaService;
	ret=wcdmaService.Open(pcktPhone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.3G1"));

// Open a Primary context
	RPacketContext primaryContext;
	TName contextName;
	
	ret=primaryContext.OpenNewContext(wcdmaService, contextName);
	CHECKPOINT(ret,KErrNone,CHP_CNTXT_CASE("B.3G1"));

// Open a Secondary context. An arbitrary value of 20 is assigned for the buffer size.
// Using a TName object would incur a great load on the stack, which is unnecessary for
// this test code.
	RPacketContext wcdmaContext;
	TBuf<20> secondaryContextName;

	ret=wcdmaContext.OpenNewSecondaryContext(wcdmaService, contextName, secondaryContextName);
	CHECKPOINT(ret,KErrNone,CHP_CNTXT_CASE("B.3G2"));

	TRequestStatus reqStatus;
	
//Creating Traffic Flow Template (TFT)
	wcdmaContext.CreateNewTFT(reqStatus, DPCKTTSY_TFT_NO_FILTERS);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("B.3G9"));
	
//Cancel Creating Traffic Flow Template (TFT)
	wcdmaContext.CreateNewTFT(reqStatus, DPCKTTSY_TFT_NO_FILTERS);
	wcdmaContext.CancelAsyncRequest(EPacketContextCreateNewTFT);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("B.3G9"));
	
//Deleting Traffic Flow Template
	wcdmaContext.DeleteTFT(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("B.3G10"));
	
//Cancel Deleting Traffic Flow Template
	wcdmaContext.DeleteTFT(reqStatus);
	wcdmaContext.CancelAsyncRequest(EPacketContextDeleteTFT);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("B.3G10"));

// Enumerate Packet filters
	TInt filterCount;

	wcdmaContext.EnumeratePacketFilters(reqStatus, filterCount);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G3"));
	CHECKPOINT(filterCount,DPCKTTSY_NUMBER_OF_PACKET_FILTERS,CHP_CNTXT_CASE("B.3G3"));

// Cancel Request
	wcdmaContext.EnumeratePacketFilters(reqStatus, filterCount);
	wcdmaContext.CancelAsyncRequest(EPacketContextEnumeratePacketFilters);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G3"));

// Get Packet filter information
	TInt filterIndex=1;
	RPacketContext::TPacketFilterV2 packetFilterInfo;
	RPacketContext::TPacketFilterV2Pckg packetFilterInfoPckg(packetFilterInfo);

	wcdmaContext.GetPacketFilterInfo(reqStatus, filterIndex, packetFilterInfoPckg);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G4"));
	
	CHECKPOINT(packetFilterInfo.iId,DPCKTTSY_FILTER_ID1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iEvaluationPrecedenceIndex,DPCKTTSY_EVALUATION_PRECEDENCE_INDEX1,CHP_CNTXT_CASE("B.3G4"));

	CHECKPOINT(packetFilterInfo.iSrcAddr[0],DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_0,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcAddr[1],DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcAddr[2],DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_2,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcAddr[3],DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_3,CHP_CNTXT_CASE("B.3G4"));

	CHECKPOINT(packetFilterInfo.iSrcAddrSubnetMask[0],DPCKTTSY_DOMAIN_SUBNET_MASK1_0,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcAddrSubnetMask[1],DPCKTTSY_DOMAIN_SUBNET_MASK1_1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcAddrSubnetMask[2],DPCKTTSY_DOMAIN_SUBNET_MASK1_2,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcAddrSubnetMask[3],DPCKTTSY_DOMAIN_SUBNET_MASK1_3,CHP_CNTXT_CASE("B.3G4"));

	CHECKPOINT(packetFilterInfo.iProtocolNumberOrNextHeader,DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcPortMin,DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iSrcPortMax,DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iDestPortMin,DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iDestPortMax,DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1,CHP_CNTXT_CASE("B.3G4"));
	CHECKPOINT(packetFilterInfo.iTOSorTrafficClass,DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1,CHP_CNTXT_CASE("B.3G4"));
	
// Cancel Request
	wcdmaContext.GetPacketFilterInfo(reqStatus, filterIndex, packetFilterInfoPckg);
	wcdmaContext.CancelAsyncRequest(EPacketContextGetPacketFilterInfo);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G4"));
	
// Add packet filter to a Traffic Flow Template (TFT)
	packetFilterInfo.iId=DPCKTTSY_FILTER_ID2;
	packetFilterInfo.iEvaluationPrecedenceIndex=DPCKTTSY_EVALUATION_PRECEDENCE_INDEX2;

	packetFilterInfo.iSrcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_0;
	packetFilterInfo.iSrcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_1;
	packetFilterInfo.iSrcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_2;
	packetFilterInfo.iSrcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_3;

	packetFilterInfo.iSrcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK2_0;
	packetFilterInfo.iSrcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK2_1;
	packetFilterInfo.iSrcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK2_2;
	packetFilterInfo.iSrcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK2_3;

	packetFilterInfo.iProtocolNumberOrNextHeader=DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2;
	packetFilterInfo.iIPSecSPI=DPCKTTSY_SECURITY_PARAMETER_INDEX2;
	packetFilterInfo.iTOSorTrafficClass=DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2;
	
	RPacketContext::TPacketFilterV2Pckg addFilterPckg(packetFilterInfo);

	wcdmaContext.AddPacketFilter(reqStatus, addFilterPckg);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G5"));

// Cancel Request
	wcdmaContext.AddPacketFilter(reqStatus, addFilterPckg);
	wcdmaContext.CancelAsyncRequest(EPacketContextAddPacketFilter);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G5"));

// Remove packet filter
	TInt removeIndex=2;

	wcdmaContext.RemovePacketFilter(reqStatus, removeIndex);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G6"));

// Cancel Request
	wcdmaContext.RemovePacketFilter(reqStatus, removeIndex);
	wcdmaContext.CancelAsyncRequest(EPacketContextRemovePacketFilter);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G6"));

// Modify Active Context
	wcdmaContext.ModifyActiveContext(reqStatus);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G7"));

// Cancel Request
	wcdmaContext.ModifyActiveContext(reqStatus);
	wcdmaContext.CancelAsyncRequest(EPacketContextModifyActiveContext);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G7"));

// Change Request: NSAE-579FBV. Added a new method, InitialiseContext(), which supersedes
// the LoanCommPort() and RecoverCommPort() methods. This new method enables asynchronous
// resource reservation for a new primary or secondary context at the creation phase.

// Asynchronous Resource reservation - InitialiseContext()
	RPacketContext::TDataChannelV2 dataChannelV2;
	RPacketContext::TDataChannelV2Pckg dataChannelV2Pckg(dataChannelV2);

	wcdmaContext.InitialiseContext(reqStatus, dataChannelV2Pckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G8"));
	CHECKPOINT_COMP_STR(dataChannelV2.iCsy,DPCKTTSY_CSY_ASYNC,CHP_CNTXT_CASE("B.3G8"));
	CHECKPOINT_COMP_STR(dataChannelV2.iPort,DPCKTTSY_PORT_ASYNC,CHP_CNTXT_CASE("B.3G8"));
	CHECKPOINT_COMP_STR(dataChannelV2.iChannelId,DPCKTTSY_CHANNEL_ID,CHP_CNTXT_CASE("B.3G8"));

// Cancel Request
	wcdmaContext.InitialiseContext(reqStatus, dataChannelV2Pckg);
	wcdmaContext.CancelAsyncRequest(EPacketContextInitialiseContext);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G8"));
	
// TPacketFilterV3 Tests
	RPacketContext::TIPAddress srcAddr;
	RPacketContext::TIPAddress srcAddrSubnetMask;
	
	TInt ii;
	for (ii=0; ii<sizeof(RPacketContext::TIPAddress); ii++)
		{
		srcAddr[ii] = 0;	
		}
	srcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_0;
	srcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_1;
	srcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_2;
	srcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_3;
	
	for (ii=0; ii<sizeof(RPacketContext::TIPAddress); ii++)
		{
		srcAddrSubnetMask[ii] = 0;	
		}
	srcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK1_0;
	srcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK1_1;
	srcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK1_2;
	srcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK1_3;
	
	filterIndex=3;
	RPacketContext::TPacketFilterV3 packetFilterV3Info;
	RPacketContext::TPacketFilterV3Pckg packetFilterV3InfoPckg(packetFilterV3Info);

	wcdmaContext.GetPacketFilterInfo(reqStatus, filterIndex, packetFilterV3InfoPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G11"));
	
/* Testing that the correct values were set  - TYPE 1*/
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr));
	TEST(!(Mem::Compare(packetFilterV3Info.SrcAddr(),sizeof(RPacketContext::TIPAddress),srcAddr,sizeof(RPacketContext::TIPAddress))));

	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddrSubnetMask));
	TEST(!(Mem::Compare(packetFilterV3Info.SrcAddrSubnetMask(),sizeof(RPacketContext::TIPAddress),srcAddrSubnetMask,sizeof(RPacketContext::TIPAddress))));
	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMin));
	TEST((packetFilterV3Info.DestPortMin() == DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1));
	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMax));
	TEST((packetFilterV3Info.DestPortMax() == DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1));
	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader));
	TEST((packetFilterV3Info.ProtocolNumberOrNextHeader() == DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1));
	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMin));
	TEST((packetFilterV3Info.SrcPortMin() == DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1));
	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMax));
	TEST((packetFilterV3Info.SrcPortMax() == DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1));
	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass));
	TEST((packetFilterV3Info.TOSorTrafficClass() == DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1));
	
/* Because the Filter Type was One SPI will not be set and should be 0 */
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI) == EFalse);	
	TEST(packetFilterV3Info.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterFlowLabel) == EFalse);

	
// Cancel Request
	wcdmaContext.GetPacketFilterInfo(reqStatus, filterIndex, packetFilterInfoPckg);
	wcdmaContext.CancelAsyncRequest(EPacketContextGetPacketFilterInfo);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G11"));
	
	filterIndex=4;
	RPacketContext::TPacketFilterV3 packetFilterV3Info2;
	RPacketContext::TPacketFilterV3Pckg packetFilterV3InfoPckg2(packetFilterV3Info2);

	wcdmaContext.GetPacketFilterInfo(reqStatus, filterIndex, packetFilterV3InfoPckg2);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G11"));
	
/* Testing that the correct values were set */
	TEST(packetFilterV3Info2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader));
	TEST((packetFilterV3Info2.ProtocolNumberOrNextHeader() == DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2));
	
	TEST(packetFilterV3Info2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI));	
	TEST((packetFilterV3Info2.SPI() == DPCKTTSY_SECURITY_PARAMETER_INDEX2));
	
	TEST(packetFilterV3Info2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass));
	TEST((packetFilterV3Info2.TOSorTrafficClass() == DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2));
/* Because the Filter Type was Two SrcPortMin will not be set and should be 0 */
	TEST((packetFilterV3Info2.SrcPortMin() == 0));
	
// Cancel Request
	wcdmaContext.GetPacketFilterInfo(reqStatus, filterIndex, packetFilterInfoPckg);
	wcdmaContext.CancelAsyncRequest(EPacketContextGetPacketFilterInfo);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G11"));
	
// Add packet filter for Type One

	RPacketContext::TPacketFilterV3 addPacketFilterV3a(RPacketContext::EPacketFilterTypeOne);
	
	addPacketFilterV3a.iId=DPCKTTSY_FILTER_ID1;
	addPacketFilterV3a.iEvaluationPrecedenceIndex=DPCKTTSY_EVALUATION_PRECEDENCE_INDEX1;

	addPacketFilterV3a.SetSrcAddr(srcAddr);
	addPacketFilterV3a.SetSrcAddrSubnetMask(srcAddrSubnetMask);
	addPacketFilterV3a.SetProtocolNumberOrNextHeader(DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1);
	addPacketFilterV3a.SetSrcPortMin(DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1);
	addPacketFilterV3a.SetSrcPortMax(DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1);
	addPacketFilterV3a.SetDestPortMin(DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1);
	addPacketFilterV3a.SetDestPortMax(DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1);
	addPacketFilterV3a.SetTOSorTrafficClass(DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1);
	
	RPacketContext::TPacketFilterV3Pckg addFilterV3aPckg(addPacketFilterV3a);

	wcdmaContext.AddPacketFilter(reqStatus, addFilterV3aPckg);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G12"));
	
// Add packet filter for Type Two

	RPacketContext::TPacketFilterV3 addPacketFilterV3b(RPacketContext::EPacketFilterTypeTwo);
	
	addPacketFilterV3b.iId=DPCKTTSY_FILTER_ID1;
	addPacketFilterV3b.iEvaluationPrecedenceIndex=DPCKTTSY_EVALUATION_PRECEDENCE_INDEX1;

	addPacketFilterV3b.SetSrcAddr(srcAddr);
	addPacketFilterV3b.SetSrcAddrSubnetMask(srcAddrSubnetMask);
	addPacketFilterV3b.SetProtocolNumberOrNextHeader(DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1);
	addPacketFilterV3b.SetSPI(DPCKTTSY_SECURITY_PARAMETER_INDEX2);
	addPacketFilterV3b.SetTOSorTrafficClass(DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1);
	
	RPacketContext::TPacketFilterV3Pckg addFilterV3bPckg(addPacketFilterV3b);

	wcdmaContext.AddPacketFilter(reqStatus, addFilterV3bPckg);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G12"));
	
// Add packet filter for Type Three

	RPacketContext::TPacketFilterV3 addPacketFilterV3c(RPacketContext::EPacketFilterTypeThree);
	
	addPacketFilterV3c.iId=DPCKTTSY_FILTER_ID1;
	addPacketFilterV3c.iEvaluationPrecedenceIndex=DPCKTTSY_EVALUATION_PRECEDENCE_INDEX1;

	addPacketFilterV3c.SetSrcAddr(srcAddr);
	addPacketFilterV3c.SetSrcAddrSubnetMask(srcAddrSubnetMask);
	addPacketFilterV3c.SetTOSorTrafficClass(DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1);
	addPacketFilterV3c.SetFlowLabel(DPCKTTSY_FLOW_LABEL);
	
	RPacketContext::TPacketFilterV3Pckg addFilterV3cPckg(addPacketFilterV3c);

	wcdmaContext.AddPacketFilter(reqStatus, addFilterV3cPckg);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.3G12"));

// Cancel Request
	wcdmaContext.AddPacketFilter(reqStatus, addFilterV3cPckg);
	wcdmaContext.CancelAsyncRequest(EPacketContextAddPacketFilter);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.3G12"));
	
// Testing FilterTypeOne
	INFO_PRINTF1(_L("Testing Filter Type One"));
	TestPacketFilterV3Type(RPacketContext::EPacketFilterTypeOne);
	
// Testing FilterTypeTwo
	INFO_PRINTF1(_L("Testing Filter Type Two"));
	TestPacketFilterV3Type(RPacketContext::EPacketFilterTypeTwo);
		
// Testing FilterTypeThree
	INFO_PRINTF1(_L("Testing Filter Type Three"));
	TestPacketFilterV3Type(RPacketContext::EPacketFilterTypeThree);

	
	primaryContext.Close();
	wcdmaContext.Close();	
	wcdmaService.Close();
	pcktPhone.Close();

	return TestStepResult();
}

void CEtelPacketTest3G_2::TestPacketFilterV3Type(RPacketContext::TPacketFilterType aType)
{
	RPacketContext::TIPAddress srcAddr;
	RPacketContext::TIPAddress srcAddrSubnetMask;
	
	srcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_0;
	srcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_1;
	srcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_2;
	srcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_3;

	srcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK2_0;
	srcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK2_1;
	srcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK2_2;
	srcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK2_3;

	
	RPacketContext::TPacketFilterV3 packetFilterV3(aType);
	packetFilterV3.Reset();
	TInt err;
	
	err = packetFilterV3.SetSrcAddr(srcAddr);
	if(err)
		{
		if(!(Mem::Compare(packetFilterV3.SrcAddr(),sizeof(RPacketContext::TIPAddress),srcAddr,sizeof(RPacketContext::TIPAddress))))
			{
			INFO_PRINTF1(_L("Correct Source Address Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr));
		packetFilterV3.Reset(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr);
		TEST(!(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr)));
		}
	else
		{
		TEST(EFalse);	// The Source Address should be set for all the types.
		}
	
	packetFilterV3.Reset();	
	err = packetFilterV3.SetSrcAddrSubnetMask(srcAddrSubnetMask);
		if(err)
		{
		if(!(Mem::Compare(packetFilterV3.SrcAddrSubnetMask(),sizeof(RPacketContext::TIPAddress),srcAddrSubnetMask,sizeof(RPacketContext::TIPAddress))))
			{
			INFO_PRINTF1(_L("Correct Source Address Subnet Mask Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddrSubnetMask));
		}
	else
		{
		TEST(EFalse);	// The Source Address Subnet Mask should be set for all the types.
		}
	
	packetFilterV3.Reset();	
	err = packetFilterV3.SetProtocolNumberOrNextHeader(DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2);
	if(err)
		{
		if(packetFilterV3.ProtocolNumberOrNextHeader() == DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2)
			{
			INFO_PRINTF1(_L("Correct Protocol Number Returned"));		
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader));
		}
	else
		{
		if (aType == RPacketContext::EPacketFilterTypeThree)
			{
			INFO_PRINTF1(_L("Not Allowed to Set Protocol Numbers"));	
			}
		else
			{
			TEST(EFalse);
			}
		}
		
	packetFilterV3.Reset();
	err = packetFilterV3.SetSrcPortMin(DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1);
	if(err)
		{
		if(packetFilterV3.SrcPortMin() == DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1)
			{
			INFO_PRINTF1(_L("Correct Minimum Source Port Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMin));
		}
	else
		{
		if ((aType == RPacketContext::EPacketFilterTypeTwo) || (aType == RPacketContext::EPacketFilterTypeThree))
			{
			INFO_PRINTF1(_L("Not Allowed to Set Minimum Source Port"));
			}
		else
			{
			TEST(EFalse);
			}
		}
		
	packetFilterV3.Reset();
	err = packetFilterV3.SetSrcPortMax(DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1);
	if(err)
		{
		if(packetFilterV3.SrcPortMax() == DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1)
			{
			INFO_PRINTF1(_L("Correct Maximum Source Port Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMax));
		}
	else
		{
		if ((aType == RPacketContext::EPacketFilterTypeTwo) || (aType == RPacketContext::EPacketFilterTypeThree))
			{
			INFO_PRINTF1(_L("Not Allowed to Set Maximum Source Port"));	
			}
		else
			{
			TEST(EFalse);
			}
		}
		
	packetFilterV3.Reset();
	err = packetFilterV3.SetDestPortMin(DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1);
	if(err)
		{
		if(packetFilterV3.DestPortMin() == DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1)
			{
			INFO_PRINTF1(_L("Correct Minimum Destination Port Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMin));
		}
	else
		{
		if ((aType == RPacketContext::EPacketFilterTypeTwo) || (aType == RPacketContext::EPacketFilterTypeThree))
			{
			INFO_PRINTF1(_L("Not Allowed to Set Minimum Destination Port"));	
			}
		else
			{
			TEST(EFalse);
			}
		}
	
	packetFilterV3.Reset();	
	err = packetFilterV3.SetDestPortMax(DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1);
	if(err)
		{
		if(packetFilterV3.DestPortMax() == DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1)
			{
			INFO_PRINTF1(_L("Correct Maximum Destination Port Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMax));
		}
	else
		{
		if ((aType == RPacketContext::EPacketFilterTypeTwo) || (aType == RPacketContext::EPacketFilterTypeThree))
			{
			INFO_PRINTF1(_L("Not Allowed to Set Maximum Destination Port"));
			}
		else
			{
			TEST(EFalse);
			}
		}
		
	packetFilterV3.Reset();
	err = packetFilterV3.SetSPI(DPCKTTSY_SECURITY_PARAMETER_INDEX2);
	if(err)
		{
		if(packetFilterV3.SPI() == DPCKTTSY_SECURITY_PARAMETER_INDEX2)
			{
			INFO_PRINTF1(_L("Correct SPI returend"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI));
		}
	else
		{
		if ((aType == RPacketContext::EPacketFilterTypeOne) || (aType == RPacketContext::EPacketFilterTypeThree))
			{
			INFO_PRINTF1(_L("Not Allowed to Set SPI"));	
			}
		else
			{
			TEST(EFalse);
			}
		}
		
	packetFilterV3.Reset();
	err = packetFilterV3.SetTOSorTrafficClass(DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2);
	if(err)
		{
		if(packetFilterV3.TOSorTrafficClass() == DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2)
			{
			INFO_PRINTF1(_L("Correct TOS or Traffic Class Returned"));
			}
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass));
		}
	else
		{
		TEST(EFalse);	// The TOS or Traffic Class should be set for all the types.
		}
		
	packetFilterV3.Reset();
	err = packetFilterV3.SetFlowLabel(DPCKTTSY_FLOW_LABEL);
	if(err)
		{
		if(packetFilterV3.FlowLabel() == DPCKTTSY_FLOW_LABEL)
			INFO_PRINTF1(_L("Correct Flow Label Returned"));
		TEST(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterFlowLabel));
		}
	else
		{
		if ((aType == RPacketContext::EPacketFilterTypeOne) || (aType == RPacketContext::EPacketFilterTypeTwo))
			{
			INFO_PRINTF1(_L("Not Allowed to Set Flow Label"));	
			}
		else
			{
			TEST(EFalse);
			}
		}

}
