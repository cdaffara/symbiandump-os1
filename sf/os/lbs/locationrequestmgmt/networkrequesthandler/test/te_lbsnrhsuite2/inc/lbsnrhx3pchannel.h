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
 @file lbsnrhx3pchannel.h
*/
#ifndef LBSNRHX3PCHANNEL_H_
#define LBSNRHX3PCHANNEL_H_

#include <e32base.h>
#include <lbs.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>


class CTestExecuteLogger;

/* 
*/
class MX3pRequestObserver
	{
public:
	virtual void OnX3pReferencePositionUpdate(
		TUint aRequestId, 
		const TPositionInfoBase& aRefPosInfo) = 0;
		
	virtual void OnX3pCompletion(
		TUint aRequestId, 
		TInt aReason,
		const TPositionInfoBase& aPosInfo) = 0;
	};

/* Class that handles a single asynchronous X3P request
*/
class CX3pRequest : public CActive
	{
public:
	static CX3pRequest* NewL(MX3pRequestObserver& aObserver, 
							 RLbsTransmitPositionServer& aServer,
							 TUint aRequestId);
	~CX3pRequest();

public:
	void DoTransmitPosition(
		TUint aRequestId,
		const TDesC& aDestinationID,
		TUint aTransmitPriority,
		TBool aGetReferencePosUpdate);
	
	void DoCancelTransmitPosition();
		
	TUint ClientId() const { return iRequestId; }
		
public:
	void OnReferencePositionUpdate();

protected:
	CX3pRequest(MX3pRequestObserver& aObserver, TUint aRequestId);
	void ConstructL(RLbsTransmitPositionServer& aServer);

	void RunL();
	void DoCancel();
	TInt RunError(TInt& aError);

protected:
	/* Simple class only needed to wait for the reference position.
	*/
	class CX3pRefPosMonitor : public CActive
		{
	public:
		static CX3pRefPosMonitor* NewL(CX3pRequest& aX3pRequest);
		~CX3pRefPosMonitor();
		
		void StartMonitor();
		
	protected:
		CX3pRefPosMonitor(CX3pRequest& aX3pRequest);
		
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	
	private:
		CX3pRequest& iX3pRequest;		
		};

private:
	MX3pRequestObserver& iObserver;
	RLbsTransmitPosition iTransmitPosition;
	TUint iRequestId;
	CX3pRefPosMonitor* iRefPosMonitor;
	TPositionInfo iRefPosInfo;
	TPositionExtendedSatelliteInfo iFinalPosInfo;
	};

/* Class that handles sending messages to the privacy controller
   interface.
*/
class CX3pChannel : public CBase,
					public MX3pRequestObserver
	{
public:
	enum TX3pResponseType
		{
		EX3pResponseUnknown = 0,
		EX3pResponseRefPosUpdate,
		EX3pResponseComplete
		};

public:
	static CX3pChannel* NewL(CTestExecuteLogger& aLogger);
	~CX3pChannel();
	
	void ConnectClientL(TUint aRequestId);
	void DisconnectClient(TUint aRequestId);
	
	void SendTransmitLocateRequestL(
		TUint aRequestId,
		const TDesC& aDestinationID,
		TUint aTransmitPriority,
		TBool aGetReferencePosUpdate);
	
	void SendCancelTransmitLocateRequestL(
		TUint aRequestId);
	
	void WaitForResponseL(TTimeIntervalSeconds aTimeout);
	TX3pResponseType GetNextResponseType();
	void GetNextResponseAsReferencePositionUpdateL(
			TUint& aRequestId,
			TPositionInfoBase& aRefPosInfo);
	void GetNextResponseAsRequestCompleteL(
			TUint& aRequestId,
			TInt& aReason,
			TPositionInfoBase& aPosInfo);
	
	const TDesC& X3pResponseTypeToDesC(TX3pResponseType aType);

protected:
	void OnX3pReferencePositionUpdate(
		TUint aRequestId, 
		const TPositionInfoBase& aRefPosInfo);
		
	void OnX3pCompletion(
		TUint aRequestId, 
		TInt aReason,
		const TPositionInfoBase& aPosInfo);
			
private:
	CX3pChannel(CTestExecuteLogger& aLogger);
	void ConstructL();
	
	CTestExecuteLogger& Logger() { return iLogger; }
	
	static TBool IsClientIdEqual(const TUint* aClientId, 
								 const CX3pRequest& aRequest);

private:
	class TX3pDataBase
		{
	protected:
		TX3pDataBase() {}
		TX3pDataBase(
				TX3pResponseType aType) :
			iType(aType),
			iRequestId(0)
			{}
	public:
		TX3pResponseType Type() const { return iType; }
		TUint iRequestId;
		
	private:
		TX3pResponseType iType;
		};
	
	class TX3pRefPosUpdate : public TX3pDataBase
		{
	public:
		TX3pRefPosUpdate() :
			TX3pDataBase(EX3pResponseRefPosUpdate)
			{}
	public:
		TPositionExtendedSatelliteInfo iRefPosInfo;
		};
	
	class TX3pReqComplete : public TX3pDataBase
		{
	public:
		TX3pReqComplete() :
			TX3pDataBase(EX3pResponseComplete),
			iReason(KErrNone) 
			{}
	public:
		TInt iReason;
		TPositionExtendedSatelliteInfo iPosInfo;
		};
	
private:
	CTestExecuteLogger& iLogger;
	CActiveSchedulerWait iWait;
	TBool iConnected;
	RLbsTransmitPositionServer iTransmitPositionServer;
	RPointerArray<TX3pDataBase> iResponseBuffer;
	RPointerArray<CX3pRequest> iRequestBuffer;
	};

#endif /*LBSNRHX3PCHANNEL_H_*/
