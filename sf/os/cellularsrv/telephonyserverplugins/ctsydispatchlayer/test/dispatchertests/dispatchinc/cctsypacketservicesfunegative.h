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
 @file The TEFUnit header file which tests the CallControl
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYPACKETSERVICESFUNEGATIVE_H
#define CCTSYPACKETSERVICESFUNEGATIVE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"
#include "cctsypacketservicesfu.h"

class CCTsyPacketServicesFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	//'negative' unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	void TestUnit0013L();
	void TestUnit0014L();
	void TestUnit0015L();
	void TestUnit0016L();

	void TestUnit0018L();
	void TestUnit0019L();
	void TestUnit0020L();
	void TestUnit0021L();
	void TestUnit0022L();
	void TestUnit0023L();
	void TestUnit0024L();
	void TestUnit0025L();
	
private:
//	void MyDriverSetConfigContextL(TInfoName aContextId,
//			RPacketContext::TGSNAddress aAccessPoint, RPacketContext::TProtocolType aPdpType, 
//			RPacketContext::TProtocolAddress aPdpAddress,
//			RPacketContext::TMiscProtocolBuffer aPcoBuffer);
//	void MyDriverModifyActiveContextL(TInt8 aRejectionCode, TInt aError);
//	void MyPCOGeneratorL(RPacketContext::TProtocolConfigOptionV2& aPco, RPacketContext::TMiscProtocolBuffer& aPcoBuffer);
//	void MyConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, 
//			const TDes8& aPassword, const TUint8 aRequestID, TDes8& aFormatedBuffer);
//	void ConvertDNSToPCOBuffer
//		(const TDes8& aPrimaryDNS, const TDes8& aSecondaryDNS, const TUint8 aRequestID, TDes8&  aFormatPcoBufferAddr);

//	void MyDriverDeleteContextL(TInfoName aContextName);
//	void MyDriverInitialiseContextL();
//	void MyDriverInitialiseContextL(const TInfoName& aPrimaryContextId, const TInfoName& aSecondaryContextId);

	void ConvertDNSToPCOBuffer (const TDes8& aPrimaryDNS, const TDes8& aSecondaryDNS, const TUint8 aRequestID, TDes8&  aFormatPcoBufferAddr);
	void ConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, const TDes8& aPassword, const TUint8 aRequestID, TDes8& aFormatedBuffer);
	void PCOGeneratorL(RPacketContext::TProtocolConfigOptionV2& aPco, RPacketContext::TMiscProtocolBuffer& aPcoBuffer);
	void DriverDeleteContextL(TInfoName aContextName);
	void DriverAttachPacketServiceL();
	void DriverDetachPacketServiceL();
	void DriverInitialiseContextL();
	void DriverInitialiseContextL(const TInfoName& aPrimaryContextId, const TInfoName& aSecondaryContextId);
	void DriverSetConfigContextL(TInfoName aContextId,  RPacketContext::TGSNAddress aAccessPoint,  RPacketContext::TProtocolType aPdpType,  RPacketContext::TProtocolAddress aPdpAddress,RPacketContext::TMiscProtocolBuffer aPcoBuffer);
	void DriverModifyActiveContextL(TInt8 aRejectionCode, TInt aError);
	void OpenInitialiseMbmsContextLC(RPacketService& aPacketService,  RPacketMbmsContext& aPacketContext, TDes& aContextName, const TDesC& aChannelId, TInt aCompError);

	
	}; // class CCTsyPacketServicesFUNegative

#endif // CCTSYPACKETSERVICESFUNEGATIVE_H

