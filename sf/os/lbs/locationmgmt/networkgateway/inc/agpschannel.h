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
// Definition of assistance data channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef AGPSCHANNEL_H_
#define AGPSCHANNEL_H_

#include <e32hashtab.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsnetinternalapi.h"
#include "lbsassistancedatacacheapi.h"

#include "lbsagpsngmsgs.h"

//
// Asssitance Data Channel
//

/** Observer for the A-GPS interface
*/
class MAgpsObserver
	{
public:
	virtual void OnAssistanceDataRequest(TLbsAsistanceDataGroupInt aDataItemMask) = 0;
	virtual void OnSelfLocationRequest(const TLbsNetSessionIdInt& aSessionId, const TLbsNetPosRequestOptionsAssistanceInt& aOptions) = 0;
	virtual void OnSelfLocationCancel(const TLbsNetSessionIdInt& aSessionId, TInt aReason) = 0;
	virtual void OnSystemStatusAdvice(TBool aTracking) = 0;
	};

/** 
Buffers incoming assistance data msgs.
Used when a channel is busy.
*/
class RAssistanceDataMsgBuffer
	{
public:
	RAssistanceDataMsgBuffer();
	void OpenL();
	void Close();
	
	void Write(const TLbsNetAssistanceDataResponseMsg& aMsg);
	void Read(TLbsNetAssistanceDataResponseMsg& aMsg);
	TBool IsEmpty() const;
	
private:

private:
	class TBufferItem
		{
	public:
		TBufferItem();
		
		TBool iValid;
		TInt  iReason;
		};
	RArray<TBufferItem> iBuffer;
	TBool				iEmpty;
	TBufferItem iErrGeneric;
	};
	
/** Communicates with the AGPS manager over an internal API 
*/
class CAgpsChannel : public CActive, private MLbsNetChannelObserver
	{
public:
	static CAgpsChannel* NewL(MAgpsObserver& aObserver);
	~CAgpsChannel();
	
    void SendAssistanceDataResponse(TInt aError, 
    								TLbsAsistanceDataGroupInt aMask, 
    								const RLbsAssistanceDataBuilderSet& aData);
    void SendSessionComplete(TInt aReason, const TLbsNetSessionIdInt& aSessionId);

private:
	CAgpsChannel(MAgpsObserver& aObserver);
	void ConstructL();
	
	// from MLbsNetChannelObserver
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	// Helpers	
	void SetAssistanceDataCacheL(const RLbsAssistanceDataBuilderSet& aData);
	
private:
	TLbsNetSessionCompleteAgpsMsg iSessionCompleteMsgBuffer;
	TBool					      iSessionCompleteMsgValid;
	RAssistanceDataMsgBuffer      iAssistanceDataMsgBuffer;
	
	MAgpsObserver& 		 iObserver;
	RAssistanceDataCache iAssistanceDataCache;
	RLbsNetChannel 		 iAGPSChannel;
	};

#endif // AGPSCHANNEL_H_
