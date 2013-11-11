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
//

#include "Te_SimPacketPenultimateContextTest.h"

CSimPacketPenultimateContextTest::CSimPacketPenultimateContextTest()
	{ 
	SetTestStepName(_L("PenultimateContextTest"));
	}

TVerdict CSimPacketPenultimateContextTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketPenultimateContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TInt x=0;
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	// test RPacketContext::OpenNewContext
	TName contextName;
	TName contextNameCompare;
	TESTL(iFirstPrimaryPacketContext.OpenNewContext(iPacketService, contextName)==KErrNone);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Context object"));

	TName anotherContextName;
	TName anotherContextNameCompare;
	TESTL(iSecondPrimaryPacketContext.OpenNewContext(iPacketService, anotherContextName)==KErrNone);
	anotherContextNameCompare.Append(KSimPrimaryPacketContextName);
	anotherContextNameCompare.AppendNum(2);
	TESTL(anotherContextName.Compare(anotherContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened another Primary Context object"));
	
	// test RPacketContext::Activate - before calling InitialiseContext
   	TRequestStatus reqStatus;
   	StartNTRasSimulation();
   	iFirstPrimaryPacketContext.Activate(reqStatus);
   	User::WaitForRequest(reqStatus);
   	TESTL(reqStatus.Int()==KErrNone);
   	INFO_PRINTF1(_L("Activate test passed"));	

	// test RPacketContext::InitialiseContext
	RPacketContext::TDataChannelV2 channelV2;
	RPacketContext::TDataChannelV2Pckg channelV2pckg(channelV2); 	
	iFirstPrimaryPacketContext.InitialiseContext(reqStatus, channelV2pckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.41"));

	// test RPacketContext::OpenNewSecondaryContext
	TName aNewContextName;
	
	iPacketContext.OpenNewSecondaryContext(iPacketService, 
												contextName,
												aNewContextName);

	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.29"));
	INFO_PRINTF1(_L("OpenNewSeondaryContext called successfully"));	

	// test RPacketService::EnumerateNifs
	TInt count;
	iPacketService.EnumerateNifs(reqStatus, count); 
	iPacketService.CancelAsyncRequest(EPacketEnumerateNifs);
	User::WaitForRequest(reqStatus);
	TESTL(count == 2);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.30"));
	INFO_PRINTF1(_L("EnumerateNifs tested successfully"));

	// test RPacketService::GetNifInfo
	TInt index = count-1;
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pckg(nifInfoV2);

	iPacketService.GetNifInfo(reqStatus, index, nifInfoV2Pckg);
	iPacketService.CancelAsyncRequest(EPacketGetNifInfo);
	User::WaitForRequest(reqStatus);
	TESTL(nifInfoV2.iContextName.Compare(anotherContextName)==0);
	TESTL(nifInfoV2.iContextType == RPacketService::EUnspecified);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.31"));
	INFO_PRINTF1(_L("GetNifInfo Test Passed"));

	// test RPacketService::EnumerateContextsInNif
	iPacketService.EnumerateContextsInNif(reqStatus, nifInfoV2.iContextName, count);
	iPacketService.CancelAsyncRequest(EPacketEnumerateContextsInNif);
	User::WaitForRequest(reqStatus);
	TESTL(count == 1);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.32"));
	INFO_PRINTF1(_L("EnumerateContextsInNif Test Passed"));

	// test RPacketService::GetContextNameInNif
	index = count-1;
	iPacketService.GetContextNameInNif(reqStatus, nifInfoV2.iContextName, index, anotherContextNameCompare);
	iPacketService.CancelAsyncRequest(EPacketGetContextNameInNif);
	User::WaitForRequest(reqStatus);
	TESTL(anotherContextNameCompare.Compare(anotherContextName)==0);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.33"));

	INFO_PRINTF1(_L("GetContextNameInNif Test Passed"));	
	
	// test RPacketService::GetCurrentReleaseMode
	RPacketService::TPacketReleaseMode releaseMode;
	iPacketService.GetCurrentReleaseMode(reqStatus, releaseMode);
	iPacketService.CancelAsyncRequest(EPacketGetCurrentReleaseMode);
	User::WaitForRequest(reqStatus);
	TESTL(releaseMode==5);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.34"));
	INFO_PRINTF1(_L("GetCurrentReleaseMode Test Passed"));	

	// test RPacketService::NotifyReleaseModeChange 
	iPacketService.NotifyReleaseModeChange(reqStatus, releaseMode);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.35"));
	INFO_PRINTF1(_L("NotifyReleaseModeChange Test Passed"));	

	TUint8 addrArray[RPacketContext::KIPAddressSize] = {0,0,0,0,
														0,0,0,0,
														0,0,255,255,
														102,23,224,132};
	TUint8 subArray[RPacketContext::KIPAddressSize] =  {0,0,0,0,
														0,0,0,0,
														0,0,255,255,
														255,255,255,255};
	RPacketContext::TPacketFilterV2 packetFilterV2;
	packetFilterV2.iId = 0;
	packetFilterV2.iEvaluationPrecedenceIndex = 1;

	for(x = 0;  x<RPacketContext::KIPAddressSize; x++)
			{
			packetFilterV2.iSrcAddr[x] = addrArray[x];
			packetFilterV2.iSrcAddrSubnetMask[x] = subArray[x];
			}
	packetFilterV2.iProtocolNumberOrNextHeader = 1;
	packetFilterV2.iSrcPortMin = 0;
	packetFilterV2.iSrcPortMax = 1;
	packetFilterV2.iDestPortMin = 2;
	packetFilterV2.iDestPortMax = 0;
	packetFilterV2.iIPSecSPI = 0;
	packetFilterV2.iTOSorTrafficClass = 1;
	packetFilterV2.iFlowLabel = 2;
	//TFTfilter = 0, 1, 10.32.2.1, 10.32.5.5, 1, 0, 1, 2, 0, 0, 1, 2 - as in configuration file
	RPacketContext::TPacketFilterV2Pckg packetFilterV2pckg(packetFilterV2);
	
	// test RPacketContext::AddPacketFilter (V2)
	iFirstPrimaryPacketContext.AddPacketFilter(reqStatus, packetFilterV2pckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.38"));
	INFO_PRINTF1(_L("AddPacketFilter V2 Test Passed"));
	INFO_PRINTF1(_L("AddPacketFilter V2 Test Passed"));


	//Set up Source adress and Source address subnet Mask for V3 Packet Filter test
	RPacketContext::TPacketFilterV3 packetFilterV3(RPacketContext::EPacketFilterTypeOne);

	RPacketContext::TIPAddress srcAddr = {0,0,0,0,
										  0,0,0,0,
										  0,0,255,255,
										  102,23,224,132};

	RPacketContext::TIPAddress srcAddrSubnetMask = {0,0,0,0,
													0,0,0,0,
													0,0,255,255,
													255,255,255,255};


	packetFilterV3.iId=V3_PCKT_FILTER_ID;
	packetFilterV3.iEvaluationPrecedenceIndex=V3_PCKT_FILTER_EVALUATION_PRECEDENCE_INDEX;
	packetFilterV3.SetSrcAddr(srcAddr);
	packetFilterV3.SetSrcAddrSubnetMask(srcAddrSubnetMask);
	packetFilterV3.SetProtocolNumberOrNextHeader(V3_PCKT_FILTER_PROTOCOLNUMBER_OR_NEXTHEADER);
	packetFilterV3.SetSrcPortMin(V3_PCKT_FILTER_MINIMUM_SOURCE_PORT_NUMBER);
	packetFilterV3.SetSrcPortMax(V3_PCKT_FILTER_MAXIMUM_SOURCE_PORT_NUMBER);
	packetFilterV3.SetDestPortMin(V3_PCKT_FILTER_MINIMUM_DESTINATION_PORT_NUMBER);
	packetFilterV3.SetDestPortMax(V3_PCKT_FILTER_MAXIMUM_DESTINATION_PORT_NUMBER);
	packetFilterV3.SetTOSorTrafficClass(V3_PCKT_FILTER_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS);
	
	RPacketContext::TPacketFilterV3Pckg packetFilterV3pckg(packetFilterV3);

	// test RPacketContext::AddPacketFilter (V3)
	iFirstPrimaryPacketContext.AddPacketFilter(reqStatus, packetFilterV3pckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_IMS_CASE("P.12"));
	INFO_PRINTF1(_L("AddPacketFilter V3 Test Passed"));

	// test RPacketContext::EnumeratePacketFilters
	iFirstPrimaryPacketContext.EnumeratePacketFilters(reqStatus, count);
	User::WaitForRequest(reqStatus);
	TESTL(count == 2);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_IMS_CASE("P.13"));
	INFO_PRINTF1(_L("EnumeratePacketFilters Test Passed"));


	// test RPacketContext::GetPacketFilterInfo (V2)
	index = 0;
	iFirstPrimaryPacketContext.GetPacketFilterInfo(reqStatus, index, packetFilterV2pckg);
	User::WaitForRequest(reqStatus);
	TESTL(packetFilterV2.iId == 0);
	TESTL(packetFilterV2.iEvaluationPrecedenceIndex == 1);
		for(x = 0;  x<RPacketContext::KIPAddressSize; x++)
			{
			TESTL(packetFilterV2.iSrcAddr[x] == addrArray[x]);
			TESTL(packetFilterV2.iSrcAddrSubnetMask[x] == subArray[x]);
			}
	TESTL(packetFilterV2.iProtocolNumberOrNextHeader == 1);
	TESTL(packetFilterV2.iSrcPortMin == 0);
	TESTL(packetFilterV2.iSrcPortMax == 1);
	TESTL(packetFilterV2.iDestPortMin == 2);
	TESTL(packetFilterV2.iDestPortMax == 0);
	TESTL(packetFilterV2.iIPSecSPI == 0);
	TESTL(packetFilterV2.iTOSorTrafficClass == 1);
	TESTL(packetFilterV2.iFlowLabel == 2);

	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.37"));

	//test RPacketContext::GetPacketFilterInfo (V3)
	index = 1;
	RPacketContext::TPacketFilterV3 packetFilterV3_2(RPacketContext::EPacketFilterTypeOne);
	RPacketContext::TPacketFilterV3Pckg packetFilterV3_2pckg(packetFilterV3_2);

	iFirstPrimaryPacketContext.GetPacketFilterInfo(reqStatus, index, packetFilterV3_2pckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_IMS_CASE("P.14"));

	TESTL(packetFilterV3_2.iId == V3_PCKT_FILTER_ID);
	TESTL(packetFilterV3_2.iEvaluationPrecedenceIndex == V3_PCKT_FILTER_EVALUATION_PRECEDENCE_INDEX);
	for(x = 0;  x<RPacketContext::KIPAddressSize; x++)
		{
		TESTL(packetFilterV3_2.SrcAddr()[x] == srcAddr[x]);
		TESTL(packetFilterV3_2.SrcAddrSubnetMask()[x] == srcAddrSubnetMask[x]);
		}

	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMin));
	TESTL((packetFilterV3_2.DestPortMin() == V3_PCKT_FILTER_MINIMUM_DESTINATION_PORT_NUMBER));
	
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMax));
	TESTL((packetFilterV3_2.DestPortMax() == V3_PCKT_FILTER_MAXIMUM_DESTINATION_PORT_NUMBER));
	
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader));
	TESTL((packetFilterV3_2.ProtocolNumberOrNextHeader() == V3_PCKT_FILTER_PROTOCOLNUMBER_OR_NEXTHEADER));
	
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMin));
	TESTL((packetFilterV3_2.SrcPortMin() == V3_PCKT_FILTER_MINIMUM_SOURCE_PORT_NUMBER));
	
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMax));
	TESTL((packetFilterV3_2.SrcPortMax() == V3_PCKT_FILTER_MAXIMUM_SOURCE_PORT_NUMBER));
	
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass));
	TESTL((packetFilterV3_2.TOSorTrafficClass() == V3_PCKT_FILTER_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS));
	
/* Because the Filter Type was One SPI will not be set and should be 0 */
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI) == EFalse);	
	TESTL(packetFilterV3_2.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterFlowLabel) == EFalse);

	INFO_PRINTF1(_L("GetPacketFilterInfo Test Passed"));

	// test RPacketContext::ModifyActiveContext
	iFirstPrimaryPacketContext.ModifyActiveContext(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.40"));
	INFO_PRINTF1(_L("ModifyActiveContext Test Passed"));

	index = 0;
	RPacketService::TContextInfo contextInfo;
	iPacketService.GetContextInfo(reqStatus, index, contextInfo);
	iPacketService.CancelAsyncRequest(EPacketGetContextInfo);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.6"));
	INFO_PRINTF1(_L("Get Info Test Passed"));

	// test RPacketContext::RemovePacketFilter
	iFirstPrimaryPacketContext.RemovePacketFilter(reqStatus, index);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.39"));
	INFO_PRINTF1(_L("RemovePacketFilter Test Passed"));
	
	// End of RPacketContext and RPacketService tests
	iPacketContext.Close();
	iFirstPrimaryPacketContext.Close();
	iSecondPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

