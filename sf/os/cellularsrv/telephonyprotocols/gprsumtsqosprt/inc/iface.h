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

#ifndef __GUQOSIFACE_H__
#define __GUQOSIFACE_H__

#include <comms-infras/nifif.h>
#include <timeout.h>
#include <networking/umtsnifcontrolif.h>

class CPdpContext;
class CRequestBase;
class CFlowData;

enum TNifStatus
	{
	EUnknown,
	EReady,
	EPending,
	EClosed
	};

const TUint KMaxEvaluationPrecedences = 256;

class CModuleGuqos;

// represents a Nif instance containing PDP contexts (there is one Nif instance per primary Pdp context)
class CNif : public CBase, public MNifEvent
	{
public:
	static CNif* NewL(CNifIfBase& aInterface, CModuleGuqos& aModule);
	~CNif();

	TInt NewPdpContext();
	TInt PrimaryContextCreated(const TContextParameters& aParams);
	void SecondaryContextCreated(const TContextParameters& aParams);
	void DeletePdpContext(CPdpContext* aContext);

	CPdpContext* FindContext(TInt aContextId);
	CPdpContext* FindChannel(TInt aChannelId);

	void AddRequest(CRequestBase& aRequest);

	void CancelPendingRequest(CFlowData* aFlowData);
	void CancelPendingRequest(CPdpContext* aContext);
	TInt FindEvaluationPrecedence();
	void RecomputeEvaluationPrecedences();

	void CloseRequest(CRequestBase* aRequest);

	TInt Event(CProtocolBase* aProtocol, TUint aName, TDes8& aOption, TAny* aSource=0);
	TInt NetworkStatusEvent(const TNetworkParameters& aNetworkEvent);

	inline CPdpContext* PrimaryPdpContext() const;
	void IssueRequest();
	inline void RemoveContext(CPdpContext& aContext);
	inline CNifIfBase& Interface() const;
	inline CModuleGuqos& Module() const;
	inline void SetStatus(const TNifStatus& aStatus);
	inline const TNifStatus& Status() const;
	inline CPdpContext* DefaultPdpContext();
	inline TUint32 IapId() const;

	// Return a reference to initialized context parameters.
	TContextParameters& ContextParameters();
protected:
	CNif(CNifIfBase& aInterface, CModuleGuqos& aModule);
	void ConstructL();
	TInt RegisterEventHandler();
	TInt SetEvents(TBool aValue);
	void SetDefaultQoS();

private:
	void SelectNewDefaultContext();
	void AddContext(CPdpContext& aContext);
	TInt CloseInterface();

	CNifIfBase& iNif;				// CNif always has a valid CNifIfBase
	CModuleGuqos& iModule;

	CPdpContext* iPrimary;			// Primary PDP context (cannot be deleted while CNif exists)

	TSglQue<CPdpContext> iContexts;

	TUint32 iIapId;
	TSglQueLink iNext;
	TNifStatus iStatus;

	TSglQue<CRequestBase> iPending;
	CRequestBase* iCurrentRequest;
	TUint32 iPendingSequence;		// Temporary hack to detect iPending modifications

	//?? This really should be a bitmap, and not waste even 8 bits where 1 would do...
	TUint8 iEvaluationPrecedenceMap[KMaxEvaluationPrecedences];

	// Every NIF Control call requires use of this LARGE object. Include
	// it in here, so that it does not need to be constantly allocated
	// from heap or stack! Unfortunately, despite it being a T-class, it
	// actually contains C-class pointers and RArrays!!! A major goof
	// in specification!!!
	TContextParameters iParameters;

	friend class CNifManager;
public:
	void RunPendingRequests();
	RTimeout iTimeout;
	};

typedef TSglQueIter<CPdpContext> TContextIter;

// Inline methods
inline CPdpContext* CNif::PrimaryPdpContext() const
	{ return iPrimary; }

inline void CNif::RemoveContext(CPdpContext& aContext)
	{ iContexts.Remove(aContext); }

//lint -e{1763} would like return to be const (now 'indirectly modifies')
inline CNifIfBase& CNif::Interface() const
	{ return iNif; }

inline CModuleGuqos& CNif::Module() const
	{ return iModule; }

inline void CNif::SetStatus(const TNifStatus& aStatus)
	{ iStatus = aStatus; }

inline const TNifStatus& CNif::Status() const
	{ return iStatus; }


inline CPdpContext* CNif::DefaultPdpContext()
	{ return iPrimary; }

inline TUint32 CNif::IapId() const
	{ return iIapId; };


class CNifManager : public CBase
	{
public:
	static CNifManager* NewL();
	~CNifManager();

	CNif* CreateNifL(CNifIfBase& aInterface, CModuleGuqos& aModule);
	void DeleteNif(CNifIfBase* aInterface);
	void DeleteNif(CNif* aInterface);
	CNif* FindInterface(const CNifIfBase *aIface);
	CPdpContext* FindChannel(TInt aChannelId);
	void CancelPendingRequests(CFlowData* aFlowData);

protected:
	CNifManager();
	void ConstructL();

private:
	TSglQue<CNif> iNifs;
	};

#endif
