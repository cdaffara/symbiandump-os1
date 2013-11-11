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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSNETINTERNALAPI_H
#define LBSNETINTERNALAPI_H


#include <e32def.h>
#include <e32property.h>
#include "lbsnetinternalmsgtypes.h"

class MLbsNetChannelObserver;
class CNetChannelMonitor;

//
// NG to NRG and NG to AGPS Manager interfaces
//

/** Logical channel used for communication between various LBS components, like
Network Request Handler, Network Gateway and AGPS Manager.

Message are sent by calling SendMessage.

Messages are received via the Mixin callback
MLbsNetChannelObserver::ProcessNetChannelMessage.
*/
NONSHARABLE_CLASS(RLbsNetChannel)
	{
public:
	/**
	@see KLbsNetChannelIdMax */
	enum TLbsNetChannelId
		{
		EChannelUnknown		= -1,
		EChannelNRH2NG 		= 0,
		EChannelNG2NRH		= 1,
		EChannelAGPS2NG		= 2,
  		EChannelNG2AGPS		= 3,
  		EChannelSUPLPSY2NG	= 4,
  		EChannelNG2SUPLPSY	= 5
		};

	IMPORT_C static void InitializeL(TLbsNetChannelId aChannelId);
	IMPORT_C static void ShutDownL(TLbsNetChannelId aChannelId);
	
	IMPORT_C RLbsNetChannel();
	IMPORT_C void OpenL(TLbsNetChannelId aChannelId, MLbsNetChannelObserver& aObserver);
	IMPORT_C void Close();

	IMPORT_C void SendMessage(const TLbsNetInternalMsgBase& aMessage, TRequestStatus& aStatus);
	IMPORT_C void CancelSendMessageNotification();

private:
	RProperty iSendProperty;
	RProperty iSendMsgReadProperty;
	CNetChannelMonitor* iMonitor;
	};

/** Observer for incoming messages from a channel
*/
class MLbsNetChannelObserver
	{
public:	
	/** Called when a message arrives from a channel.
	@param aMessage The message. */
	virtual void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage) = 0;
	};

/**
Monitors for incoming messages from a channel.
*/
NONSHARABLE_CLASS(CNetChannelMonitor) : public CActive
	{
public:
	static CNetChannelMonitor* NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId, TUid aPropId, TUint aPropKey, TUint aAckPropKey);
	~CNetChannelMonitor();
		
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	void NotifyNetChannelMessage();
	CNetChannelMonitor(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
	void ConstructL(TUid aPropId, TUint aPropKey, TUint aAckPropKey);

private:
	MLbsNetChannelObserver&			 iObserver;
	RLbsNetChannel::TLbsNetChannelId iChannelId;
	
	RProperty iReceiveProperty;
	RProperty iReceiveMsgReadProperty;
	};
	
#endif // LBSNETINTERNALAPI_H
