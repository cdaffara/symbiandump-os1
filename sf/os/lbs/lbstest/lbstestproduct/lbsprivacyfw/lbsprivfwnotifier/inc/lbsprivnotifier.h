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
//

#ifndef __LBSPRIVNOTIFIER_H_
#define __LBSPRIVNOTIFIER_H_

#include <e32std.h>
#include <e32base.h>
#include <eiknotapi.h>
#include <e32property.h>

#include <lbs/epos_cposprivacynotifier.h>
#include <lbs/lbsloccommon.h>

#include <lbs/test/tlbstestrequestchannel.h>
#include <lbs/test/lbsprivfwdefs.h>


class CLbsPrivacyNotifierBase : public CActive, public MEikSrvNotifierBase2, public MLbsTestChannelRequestObserver
	{
public:
	~CLbsPrivacyNotifierBase();
protected:
	//	From MNotifierBase2
	virtual void Release();
	virtual TNotifierInfo RegisterL() = 0;
	virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage) = 0;
	virtual TNotifierInfo Info() const = 0;
	virtual TPtrC8 StartL(const TDesC8& aBuffer);
	virtual void Cancel();
	virtual TPtrC8 UpdateL(const TDesC8& aBuffer);
	
	// From CActive
	virtual void DoCancel();
	virtual void RunL();
	
	CLbsPrivacyNotifierBase();

	void StartTimer(const TTimeIntervalMicroSeconds& aCompletionDelay);

	void CompleteMessage(TInt aRet);
	void ProcessBuffer(const TDesC8& aBuffer, TMessageType aMsgType);

	//From MReadPropPipeObserver
	//void OnReadyToReadL(CPrivFwReadPropPipe& aPipe);

	//From MLbsTestChannelRequestObserver
	virtual void ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

private:
	void SendRequestDetails(TMessageType aMsgType, TPosVerifyCancelReason aCancelReason, TLbsExternalRequestInfo& aRequestInfo);
	void ConvertToTestRequestInfo(TLbsTestRequestInfo& aTestRequestInfo, TLbsExternalRequestInfo& aRequestInfo);
	

protected:
	TNotifierInfo iInfo;
	RTimer	iTimer;
	RMessagePtr2 iMessage;
	TInt	iReplySlot;

//	CPrivFwWritePropPipe*		iResponseWriter;
//	CPrivFwNotifyReadPropPipe*	iMsgReader;
    TTimeIntervalMicroSeconds*	iCompletionDelay;
    TPosQNRequestId				iCompletionRequestId;
    TResponseType				iCompletionResponse;
	TBool						iPipesCreated;

	TBool						iChannelCreated;
	CTestRequestChannel* 		iHandlerChannel;
	CTestRequestChannel* 		iProxyChannel;
	
	enum TNotifierType
		{
		/** Notifier type not set. */
		ENotifierTypeUnknown = 0,
		/** This notifier will only support notifications. */
		ENotifierTypeNotify,
 		/** This notifier will only support Verifications. */
		ENotifierTypeVerify
		};
	TNotifierType	iNotifierType;
	};

class CLbsNotifyNotifier : public CLbsPrivacyNotifierBase
	{
public:
	//	Construction
	static CLbsPrivacyNotifierBase* NewL();
	virtual TNotifierInfo RegisterL();
	virtual TNotifierInfo Info() const;
	void Cancel();
	void RunL();

	virtual TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);

protected:
	CLbsNotifyNotifier();
private:
	void ConstructL();
private:
	TNotifierInfo iInfo;
	};

class CLbsVerifyNotifier : public CLbsPrivacyNotifierBase
	{
public:
	//	Construction
	static CLbsPrivacyNotifierBase* NewL();
	virtual TNotifierInfo RegisterL();
	virtual TNotifierInfo Info() const;
	
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);

protected:
	CLbsVerifyNotifier();
	void SetPropertyL(TInt aVal);
private:
	void ConstructL();
private:
	TNotifierInfo iInfo;
	};


#endif // __LBSPRIVNOTIFIER_H_
