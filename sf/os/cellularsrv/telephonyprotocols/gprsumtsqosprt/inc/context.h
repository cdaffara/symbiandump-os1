// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <networking/umtsnifcontrolif.h> // ..for TContextConfig, etc..
#include <timeout.h>

#include "parameters.h"	// ..for TPacketFilter, etc..

class CRequestBase;
class TFTInfo;
class RExtensionData;
class CFlowData;
class CNif;

/*
 *   The returned sblp sub error should be between 1-7 in the PCO buffer
 *   see TS 29.207,  annex D for details
 */
enum TSblpSubErrorRange
	{
	ESblpSubErrorValueMinimum = 1,
	ESblpSubErrorValueMaximum = 7
	};


const TUint KMaxPacketFilters	= 8;

class RContextConfig : public TContextConfig
	/**
	* Add some access functions to the TContextConfig
	*/
	{
public:
	// Allow access to TFTInfo without needing a copy
	inline TTFTInfo& TFTInfo() { return iTFTInfo; }
	};

class CPdpContext : public CBase
	{
	friend class CNif;
	friend class CRequestBase;
public:

    static CPdpContext* NewL(CNif& aNifItem, const TContextType& aType, TUint8 aContextId);
	
	~CPdpContext();

	TInt Activate();
	void ActivateReply(CRequestBase* aRequest, const TContextParameters& aParams);

	TInt ModifyActive();
	void ModifyActiveReply(CRequestBase* aRequest, const TContextParameters& aParams);

	TInt SetQoS();
	TInt SetQoS(const TQoSParameters& aGeneric, const TQoSRequested& aUmts);
	void SetQoSReply(CRequestBase* aRequest, const TContextParameters& aParams);

    TInt ModifyTft(const TTFTOperationCode& aTFTOperationCode, const TTFTInfo& aTft);
	void ModifyTftReply(CRequestBase* aRequest, const TContextParameters& aParams);

	void ParametersChangedEvent(const TContextParameters& aParams);
	void DeleteEvent(const TContextParameters& aParams);
	
	TInt Delete();
	TInt AddPacketFilter(CFlowData& aFlow, TTFTInfo& aTft);
	TInt RemovePacketFilter(TTFTInfo& aTft, const CFlowData* aFlow = NULL);

	void Block();
	void UnBlock();
	inline TUint IsFlowBlocked() const;

	void FlowAttached();
	void FlowDetached();
	void SetContextStatus(const RPacketContext::TContextStatus& aStatus);

	TInt GetQoSRanking();
	TBool IsPrimary() const;

	inline TInt RefCount() const; 
	inline TInt ContextId() const;
	inline TInt ChannelId() const;
	inline void SetChannelId(TInt aChannelId);
	inline CNif& Nif();
	inline const RExtensionData& Extension() const;
	inline TBool ContextActive() const;
	inline TBool ContextInactive() const;
	inline RPacketContext::TContextStatus ContextStatus() const;
	inline TInt TrafficClass() const;
	inline TDblQue<CFlowData>& Flows();
	inline TContextType ContextType() const;

protected:
	TSglQueLink iNext;
    
    CPdpContext(CNif& aNifItem, TContextType aType, TUint8 aContextId);
	static TBool CheckSblpErrorInPcoBuffer(const TContextParameters& aParams, TUint8& parsedSblpErrorValue);

	void ConstructL();
	void GenerateEvent(TUint aEventType, const TContextParameters& aParams);
	TBool MatchPacketFilter(const TPacketFilter& aFlow, const TPacketFilter& aFilter) const;
	TInt FindPacketFilterId();
	void Update(const TContextParameters& aParams);

private:
    void NotifyBlockingState(TUint aOldBlocking);

    TInt iRefs;								// count of attached flows (just for information)

	// Blocked/Unblocked status as indicated by NIF. If NIF reports
	// "blocked", the context stays blocked until NIF reports "unblock"
	// This is totally independent of context status and flow blocking:
	// flows are blocked if context is "blocked" *OR* status is not active.
	TUint8 iBlocked;
	
	TUint8 iContextId;
	TInt iChannelId;
	TContextType iContextType;
	RPacketContext::TContextStatus iContextStatus;

	CNif& iNifItem;							// Nif interface
	TDblQue<CFlowData> iFlows;				// Flow list

	// Currently Active filters.
	TInt iNumFilters;
	TPacketFilter iFilters[KMaxPacketFilters];

	// Current parameters/configuration
	TQoSNegotiated	iNegotiated;
public:
	// Timeout handling
	void Timeout();
	RTimeout iTimeout;
	};

// inline methods
inline TContextType CPdpContext::ContextType() const
	{ return iContextType; }

inline TInt CPdpContext::ContextId() const
	{ return iContextId; }

inline TInt CPdpContext::ChannelId() const
	{ return iChannelId; }

inline void CPdpContext::SetChannelId(TInt aChannelId)
	{ iChannelId = aChannelId; }

inline CNif& CPdpContext::Nif()
	{ return iNifItem; }

inline TInt CPdpContext::RefCount() const
	{ return iRefs; }

inline TBool CPdpContext::ContextActive() const
	{ return (iContextStatus == RPacketContext::EStatusActive); }

inline TBool CPdpContext::ContextInactive() const
	{ return (iContextStatus == RPacketContext::EStatusInactive); }

inline RPacketContext::TContextStatus CPdpContext::ContextStatus() const
	{ return iContextStatus; }

inline TInt CPdpContext::TrafficClass() const
	{ return iNegotiated.iTrafficClass; }

inline TDblQue<CFlowData>& CPdpContext::Flows()
	{ return iFlows; }

inline TUint CPdpContext::IsFlowBlocked() const
	// Flows are blocked if either explicit blocking from NIF is on,
	// or the context is not active.
	{ return iBlocked || !ContextActive(); }

typedef TDblQueIter<CFlowData> TDblFlowIter;

#endif
