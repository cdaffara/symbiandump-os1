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

#ifndef PHYSICALLINKHELPER_H_
#define PHYSICALLINKHELPER_H_

#include "debug.h"
#include <bt_sock.h>
#include "physicallinks.h"
#include "linkutil.h"
#include <bluetooth/hci/hciutil.h> 

// A struct to allow both single event and combination events to be passed for requesting
// notification.  This effectively moves the cast to a TInt to below the API rather than
// being required by the client.
struct TNotifyEvent
	{
public:
	TNotifyEvent(TBTPhysicalLinkStateNotifier aSingleEvent) : iEvent(aSingleEvent) {};
	TNotifyEvent(TBTPhysicalLinkStateNotifierCombinations aComboEvent) : iEvent(aComboEvent) {};

	TInt NotifyEvent() const {return iEvent;};
private:
	TInt iEvent;
	};


NONSHARABLE_CLASS(CPhysicalLinkHelper) : public CBase, public MSocketNotify
	{
public:
	~CPhysicalLinkHelper();

	// From MSocketNotify
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	void CanClose(TDelete aDelete=EDelete);
	void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	void Disconnect();
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8 *aBuf);
	void NoBearer(const TDesC8& /*aConnectionInf*/) {};
	void Bearer(const TDesC8& /*aConnectionInf*/) {};

	virtual void StartHelper() = 0;

	inline const TBTDevAddr& BDAddr() const;

protected:
	CPhysicalLinkHelper(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink);
	void BaseConstructL();

	void DisableLPM();
	
	void QueueTimer(TTimeIntervalMicroSeconds32 aTimerVal);
	void RemoveTimer();
	void NotifyBasebandEvent(TNotifyEvent aEvent);
	void CancelNotify();
	
	TBool IsEPRSupported() const;
	
	virtual void TimerExpired() = 0;
	virtual void HandleError(TInt aError) = 0;
	virtual void EventReceived(TBTBasebandEventNotification& aEvent) = 0;
	
private:
	static TInt EventReceivedCallBack(TAny* aThis);
	void DoEventReceivedCallBack();
	
	static TInt TimerExpiredCallBack(TAny* aThis);
	void DoTimerExpiredCallBack();
	
protected:
	CPhysicalLinksManager&			iLinkMgr;
	CPhysicalLink&					iLink;
	
private:
	// Async Callback to notify baseband event received.
	CAsyncCallBack*					iEventReceivedCallBack;
	TBTBasebandEventNotification	iBasebandEvent;
	CBTProxySAP*					iBTProxySAP;  
	
	TDeltaTimerEntry				iTimerEntry;
	};

inline const TBTDevAddr& CPhysicalLinkHelper::BDAddr() const
	{
	return iLink.BDAddr();   
	}

#endif //PHYSICALLINKHELPER_H_
