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
// functional unit of the Common TSY Dispatcher.
//



/**
 @file The TEFUnit header file which tests the PacketServices
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYPACKETSERVICESFU_H
#define CCTSYPACKETSERVICESFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

/*
GPRS Requested

iMinDelay, iMinMeanThroughput, iMinPeakThroughput, iMinPrecedence,
iMinReliability, iReqDelay, iReqMeanThroughput, iReqPeakThroughput, iReqPrecedence, iReqReliability
*/

const RPacketContext::TGSNAddress KDefaultAccessPoint = _L8("");
const RPacketContext::TProtocolType KDefaultPdpType = RPacketContext::EPdpTypeIPv4;
const TUint KDefaultServiceOption = 1;
const TInt KMaxMiscBufferSize = 40;
const TInt KAppendableBufferSize=3;
const TInt KPrimaryAndSecondaryDNSLength = 16;
const TUint KPasswordUsernameHeaderLength = 7;
const TUint KIPCRequestCode = 1;
const TUint KRequestID = 0;
const TUint KIDLength = 3;
const TUint KMaxUserAndPassLength = 80;
const TUint KMaxPdpAddressLength = 50;
const TUint KPrimaryAndSecondaryDNSID = 0xC023;
const TUint KUsenamePasswordDNSID = 0x8021;
const TUint KIndicatePrimaryAddress = 0x81;
const TUint KIndicateSecondaryAddress = 0x83;
const TUint KPacketLengthMSB = 0x0;
const TUint KIPv4Len = 4;
const TUint KDnsAddressLength = 6;
const TUint KDnsLength = 32;

_LIT(KDefaultContextName, "");
_LIT(KFirstAllocatedContextName, "100Int_0");
_LIT(KSecondAllocatedContextName, "101Int_0");
_LIT(KThirdAllocatedContextName, "102Int_1");
_LIT(KSecondaryContextName, "101Int_1");
_LIT(KMBMSFirstAllocatedContextName, "MBMS_Context_0");
_LIT(KDefaultChannelId, "1");
_LIT(KSecondaryContextChannelId, "2");
_LIT(KPdpAdd99, "0.0.0.0");
_LIT(KPointName99, "internet");
_LIT(KPrimaryDns99, "127.0.0.1");
_LIT(KPrimaryDns99ForUN0006, "1.2.3.44");
_LIT(KSecondaryDns99, "0.0.0.0");
_LIT(KPrimaryDns, "0.0.0.0");
_LIT(KSecondaryDns, "0.0.0.0");
_LIT(KPdpAddGPRS, "0.0.0.0");
_LIT(KPdpAddGPRSActive, "1.1.1.1");
_LIT(KPointNameGPRS, "internet");
_LIT(KUsername, "USER");
_LIT(KLongUsername, "LONGUSER");
_LIT(KDummyMiscBuffer,"DummyPCO");
_LIT(KPassword, "USER");
_LIT8(KAddress, "1.1.1.1");
_LIT8(KAPN, "0.0.0.0");
const TInt8 KRejectionCodePass = 0;
const TInt8 KRejectionCodeFail = 1;

class CCTsyPacketServicesFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:

	void ConvertDNSToPCOBuffer (const TDes8& aPrimaryDNS, const TDes8& aSecondaryDNS, const TUint8 aRequestID, TDes8&  aFormatPcoBufferAddr);
	void ConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, const TDes8& aPassword, const TUint8 aRequestID, TDes8& aFormatedBuffer);
	void CCTsyPacketServicesFU::PCOGeneratorL(RPacketContext::TProtocolConfigOptionV2& aPco, RPacketContext::TMiscProtocolBuffer& aPcoBuffer);
	void DriverDeleteContextL(TInfoName aContextName);
	void DriverAttachPacketServiceL();
	void DriverDetachPacketServiceL();
	void DriverInitialiseContextL();
	void DriverInitialiseContextL(const TInfoName& aPrimaryContextId, const TInfoName& aSecondaryContextId);
	void DriverSetConfigContextL(TInfoName aContextId,  RPacketContext::TGSNAddress aAccessPoint,  RPacketContext::TProtocolType aPdpType,  RPacketContext::TProtocolAddress aPdpAddress,RPacketContext::TMiscProtocolBuffer aPcoBuffer);
	void DriverModifyActiveContextL(TInt8 aRejectionCode, TInt aError);
	void OpenInitialiseMbmsContextLC(RPacketService& aPacketService,  RPacketMbmsContext& aPacketContext, TDes& aContextName, const TDesC& aChannelId, TInt aCompError);

	
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();
	void TestUseCase0003L();
	void TestUseCase0004L();
	void TestUseCase0005L();
	void TestUseCase0006L();
	void TestUseCase0007L();
	

	//other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit00010L();
	void TestUnit00011AL();
	void TestUnit00011BL();
	void TestUnit00011CL();
	void TestUnit00012L();
	void TestUnit00013L();
	void TestUnit00014L();
	void TestUnit00015L();
	void TestUnit00016L();
	void TestUnit00017L();
	void TestUnit00018L();
	void TestUnit00019L();
	void TestUnit00020L();
	void TestUnit00021L();
	void TestUnit00022L();
	void TestUnit00023L();
	void TestUnit00024L();
	void TestUnit00025L();
	void TestUnit00026L();

private:

	}; // class CCTsyPacketServicesFU

#endif // CCTSYPACKETSERVICESFU_H

