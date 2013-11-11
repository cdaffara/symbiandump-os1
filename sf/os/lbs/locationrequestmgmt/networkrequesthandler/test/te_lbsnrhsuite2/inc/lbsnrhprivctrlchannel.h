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
 @file lbsnrhngchannel.h
*/
#ifndef LBSNRHPRIVCTRLCHANNEL_H_
#define LBSNRHPRIVCTRLCHANNEL_H_

#include <e32base.h>
#include <lbs.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsprivacycontroller.h>


class CTestExecuteLogger;

/* Class that handles sending messages to the privacy controller
   interface.
*/
class CPrivCtrlChannel : public CActive,
				  		 public MLbsPrivacyObserver
	{
public:
	enum TPrivCtrlResponseType
		{
		EDataResponseUnknown = 0,
		EDataResponsePrivReq,
		EDataResponsePosUpdate,
		EDataResponseReqComplete
		};

public:
	static CPrivCtrlChannel* NewL(CTestExecuteLogger& aLogger);
	~CPrivCtrlChannel();
	
	void RunL();
	void DoCancel();
	
	void SendPrivacyResponse(TUint aRequestId,
			CLbsPrivacyController::TRequestVerificationResult aResponse,
			TTimeIntervalSeconds aTimeout);

	void SendCancelPrivacyRequest(TUint aRequestId,
			TTimeIntervalSeconds aTimeout);
	
	void WaitForResponseL(TTimeIntervalSeconds aTimeout);
	TPrivCtrlResponseType GetNextResponseType();
	void GetNextResponseAsPrivacyRequestL(
			TUint& aRequestId,
			MLbsPrivacyObserver::TNotificationType& aNotificationType,
			TLbsExternalRequestInfo& aRequestInfo);
	void GetNextResponseAsPositionUpdateL(
			TUint& aRequestId,
			TPositionInfoBase& aPosInfo);
	void GetNextResponseAsRequestCompleteL(
			TUint& aRequestId,
			TInt& aReason);
	
private:
	CPrivCtrlChannel(CTestExecuteLogger& aLogger);
	void ConstructL();

	// From MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);

private:
	class TPrivCtrlDataBase
		{
	protected:
		TPrivCtrlDataBase() {}
		TPrivCtrlDataBase(
				TPrivCtrlResponseType aType) :
			iType(aType),
			iRequestId(0)
			{}
	public:
		TPrivCtrlResponseType Type() const { return iType; }
		TUint iRequestId;
		
	private:
		TPrivCtrlResponseType iType;
		};

	class TPrivCtrlPrivReq : public TPrivCtrlDataBase
		{
	public:
		TPrivCtrlPrivReq() :
			TPrivCtrlDataBase(EDataResponsePrivReq),
			iNotificationType(MLbsPrivacyObserver::ENotifyUnknown) 
			{}
	public:
		MLbsPrivacyObserver::TNotificationType iNotificationType;
		TLbsExternalRequestInfo2 iRequestInfo;
		};
	
	class TPosUpdate : public TPrivCtrlDataBase
		{
	public:
		TPosUpdate() :
			TPrivCtrlDataBase(EDataResponsePosUpdate)
			{}
	public:
		TPositionExtendedSatelliteInfo iPosInfo;
		};
	
	class TReqComplete : public TPrivCtrlDataBase
		{
	public:
		TReqComplete() :
			TPrivCtrlDataBase(EDataResponseReqComplete),
			iReason(KErrNone) 
			{}
	public:
		TInt iReason;
		};
	
private:
	CTestExecuteLogger& iLogger;
	CActiveSchedulerWait iWait;
	CLbsPrivacyController* iPrivController;
	RPointerArray<TPrivCtrlDataBase> iResponseBuffer;
	};

#endif /*LBSNRHPRIVCTRLCHANNEL_H_*/
