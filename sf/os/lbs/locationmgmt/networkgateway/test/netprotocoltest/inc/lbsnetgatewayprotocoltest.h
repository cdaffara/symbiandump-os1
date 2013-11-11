// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for the 'stub' network protocol module used in
// the Network Gateway unit tests.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSNETGATEWAYPROTOCOLTEST_H_
#define LBSNETGATEWAYPROTOCOLTEST_H_

#include <e32base.h>
#include "lbsnetgatewayprotocoltestapi.h"


/** The test/skeleton implementation of CLbsNetworkProtocol

The uid for this implementation is 0x1111E193.
*/
class CTestLbsNetworkProtocol : public CLbsNetworkProtocolBase2,
								public MNetGatewayProtocolTestObserver
	{
	
public:
	static CTestLbsNetworkProtocol* New0L(TAny* aData);
	static CTestLbsNetworkProtocol* New1L(TAny* aData);
	static CTestLbsNetworkProtocol* New2L(TAny* aData);
	static CTestLbsNetworkProtocol* New3L(TAny* aData);
	static CTestLbsNetworkProtocol* New4L(TAny* aData);
	static CTestLbsNetworkProtocol* New5L(TAny* aData);
	static CTestLbsNetworkProtocol* New6L(TAny* aData);
	static CTestLbsNetworkProtocol* New7L(TAny* aData);
	static CTestLbsNetworkProtocol* New8L(TAny* aData);
	static CTestLbsNetworkProtocol* New9L(TAny* aData);
	// 10 entry points for roaming PM
	static CTestLbsNetworkProtocol* New10L(TAny* aData);
	static CTestLbsNetworkProtocol* New11L(TAny* aData);
	static CTestLbsNetworkProtocol* New12L(TAny* aData);
	static CTestLbsNetworkProtocol* New13L(TAny* aData);
	static CTestLbsNetworkProtocol* New14L(TAny* aData);
	static CTestLbsNetworkProtocol* New15L(TAny* aData);
	static CTestLbsNetworkProtocol* New16L(TAny* aData);
	static CTestLbsNetworkProtocol* New17L(TAny* aData);
	static CTestLbsNetworkProtocol* New18L(TAny* aData);
	static CTestLbsNetworkProtocol* New19L(TAny* aData);

	
private:
	static CTestLbsNetworkProtocol* NewL(TUint aModuleIndex, TAny* aData);
	
	~CTestLbsNetworkProtocol();
	
public:
	// Implemented functions from CLbsNetworkProtocolBase2
	void RespondPrivacyRequest(const TLbsNetSessionId& aSessionId,
							   const TLbsPrivacyResponse& aResponse,
							   TInt aReason);
	
	void RespondLocationRequest(const TLbsNetSessionId& aSessionId,
								TInt  aReason,
								const TPositionInfoBase& aPosInfo);

	void RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
								 const TDesC& aDestination,
								 TInt aPriority,
								 const TLbsNetPosRequestOptionsBase& aOptions);
									
	void CancelTransmitLocation(const TLbsNetSessionId& aSessionId,
								TInt  aReason);
	
	void RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);
	
	void RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
								const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
								TInt  aReason);
	
	void AdviceSystemStatus(TLbsSystemStatus aStatus);
	
	// Implemented functions from CLbsNetworkProtocolBase2
	void CancelExternalLocation(const TLbsNetSessionId& aSessionId,
									TInt aReason);
	// Implemented functions from CLbsNetworkProtocolBase2
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask,
							   const TLbsNetSessionIdArray& aSessionIdArray);
	
	// Implemented functions from MNetGatewayProtocolTestObserver
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);

private:

	CTestLbsNetworkProtocol(TUint aModuleIndex, TLbsNetProtocolModuleParams& aParams);
	void ConstructL();
	
private:
	MLbsNetworkProtocolObserver& iObserver;
	RNetGatewayProtocolTestChannel iTestChannel;
	TUint iModuleIndex;

	};

#endif // LBSNETGATEWAYPROTOCOLTEST_H_


