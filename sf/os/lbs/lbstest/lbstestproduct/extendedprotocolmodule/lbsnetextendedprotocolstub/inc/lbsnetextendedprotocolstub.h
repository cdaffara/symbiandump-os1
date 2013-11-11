/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbsnetextendedprotocolstub.h
*/
#if (!defined LBSNETEXTENDEDPROTOCOLSTUB_H)
#define LBSNETEXTENDEDPROTOCOLSTUB_H

#include <ecom/implementationproxy.h>

#include <lbs/test/lbsnetextendedprotocolproxydefs.h>
#include <lbs/lbsnetprotocolbase.h>

//-------------------------------------------------------------------------------------------------

/*
Performs actions on behalf of proxy as indicated by messages sent on the Action Channel

@internalComponent
@test
*/
class TActionPerformer : public MReadPropPipeObserver
	{
public:	
	TActionPerformer(MLbsNetworkProtocolObserver& aObserver);
	
	void OnReadyToReadL(CReadPropPipe& aPipe);
	
private:
	void doNetMsgProcessPrivacyRequestL  (CReadPropPipe& aPipe,CMemoryManager*aMemManager);
 	void doNetMsgProcessLocationRequestL (CReadPropPipe& aPipe,CMemoryManager*aMemManager);
 	void doNetMsgProcessSessionCompleteL (CReadPropPipe& aPipe,CMemoryManager*aMemManager);
 	void doNetMsgProcessAssistanceDataL  (CReadPropPipe& aPipe,CMemoryManager*aMemManager);
 	void doNetMsgProcessLocationUpdateL  (CReadPropPipe& aPipe,CMemoryManager*aMemManager);
 	void doNetMsgGetCurrentCapabilitiesRequestL(CReadPropPipe& aPipe,CMemoryManager*aMemManager);

	void doNetMsgProcessStatusUpdateL    (CReadPropPipe& aPipe);
	
private:
	MLbsNetworkProtocolObserver& iObserver;
	};


//-------------------------------------------------------------------------------------------------

/**
Implementation of Network Protocol Stub.

@internalComponent
@test
*/
class CNetExtendedProtocolStub : public CLbsNetworkProtocolBase2
	{
public:
	static CNetExtendedProtocolStub* NewL(TAny* aParams);
	~CNetExtendedProtocolStub();
	
	// CLbsNetworkProtocolBase methods
	
	void RespondPrivacyRequest(
								const TLbsNetSessionId& aSessionId,
								const TLbsPrivacyResponse& aResponse,
								TInt aReason);
	
	void RespondLocationRequest(
                                const TLbsNetSessionId& aSessionId,
                                TInt  aReason,
                                const TPositionInfoBase& aPosInfo);

	void RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
								const TDesC& aDestination,
								TInt aPriority,
								const TLbsNetPosRequestOptionsBase& aOptions);
											
	void CancelTransmitLocation(
                                const TLbsNetSessionId& aSessionId,
                                TInt  aReason);
	
	void CancelExternalLocation(
	                                const TLbsNetSessionId& aSessionId,
	                                TInt  aReason);

	void RequestAssistanceData(
								TLbsAsistanceDataGroup aDataRequestMask,
								const TLbsNetSessionIdArray& aSessionIdArray);
						
	void RequestNetworkLocation(
                                const TLbsNetSessionId& aSessionId,
                                const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelNetworkLocation(
                                const TLbsNetSessionId& aSessionId,
                                TInt  aReason);

	void RequestSelfLocation(
                                const TLbsNetSessionId& aSessionId,
                                const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelSelfLocation(
                                const TLbsNetSessionId& aSessionId,
                                TInt  aReason);
									
	void AdviceSystemStatus(
                                TLbsSystemStatus aStatus);

private:
	CNetExtendedProtocolStub(MLbsNetworkProtocolObserver& aObserver);
	void ConstructL();

private:
	/** Reference to this protocol module's observer.
	This is used to make callbacks into LBS from the protocol module.
	*/
	MLbsNetworkProtocolObserver& iObserver;

	CWritePropPipe* iResponseWriter;
	
	TActionPerformer iActionPerformer;
	
	CNotifyReadPropPipe* iActionChannelWatcher;
	};


#endif // LBSNETEXTENDEDPROTOCOLSTUB_H
