// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ENCRYPTIONKEYREFRESHHELPER_H
#define ENCRYPTIONKEYREFRESHHELPER_H


#include <bt_sock.h>
#include <bluetooth/hci/hciutil.h> 

#include "debug.h"
#include "linkutil.h"
#include "physicallinkhelper.h"
#include "physicallinks.h"

// watchdog 
const TUint KTimeoutEncryptionKeyRefresh =   3000000;  // 3 s; 
 
class TEncryptionKeyRefresherState;
class CEncryptionKeyRefresherStateFactory;
NONSHARABLE_CLASS(CEncryptionKeyRefresher) : public CPhysicalLinkHelper
	{
	friend class TEncryptionKeyRefresherState;
	friend class TEkrStateIdle;
	friend class TEkrStateDisablingLPM;
	friend class TEkrStateRefreshingKey;

public:
	static CEncryptionKeyRefresher* NewL(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink);
	~CEncryptionKeyRefresher();

	virtual void StartHelper();
	
	void EncryptionKeyRefreshComplete(TInt aError);

private:
	CEncryptionKeyRefresher(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink);
	void ConstructL(); 
	
	virtual void TimerExpired();
	virtual void HandleError(TInt aError);
	virtual void EventReceived(TBTBasebandEventNotification& aEvent);
  
private:
	CEncryptionKeyRefresherStateFactory*	iStateFactory;
	const TEncryptionKeyRefresherState*		iState;            
	};

//--------------------------------------------------
// STATE FACTORY
//--------------------------------------------------

/**
   Factory for the EncryptionKeyRefresher states

   The states are flyweight classes
**/
NONSHARABLE_CLASS(CEncryptionKeyRefresherStateFactory) : public CBase
	{
public:
	static CEncryptionKeyRefresherStateFactory* NewL();

	enum TEncryptionKeyRefresherStates
		{
		EIdle,
		EDisablingLPM,
		ERefreshingKey,
		// Note that we don't need a state to enable LPM again
		// as this will happen implicitly when we close our 
		// proxy SAP.
		// *** keep next one last ***
		EEncryptionKeyRefresherMaxState,
		};
	
	const TEncryptionKeyRefresherState& GetState(TEncryptionKeyRefresherStates aState) const;
	TInt StateIndex(const TEncryptionKeyRefresherState* aState) const;
	
private:
	CEncryptionKeyRefresherStateFactory();
	void ConstructL();
	TFixedArray<TEncryptionKeyRefresherState*, EEncryptionKeyRefresherMaxState> iStates;
	};



//--------------------------------------------------
// STATES, base 
//--------------------------------------------------

NONSHARABLE_CLASS(TEncryptionKeyRefresherState)
	{
public:
	TEncryptionKeyRefresherState(CEncryptionKeyRefresherStateFactory& aFactory);
	
	virtual void Enter(CEncryptionKeyRefresher& aContext) const;
	virtual void Exit(CEncryptionKeyRefresher& aContext) const;

	virtual void Start(CEncryptionKeyRefresher& aContext) const;
	virtual void EventReceived(CEncryptionKeyRefresher& aContext, TBTBasebandEventNotification& aEvent) const;
	virtual void Error(CEncryptionKeyRefresher& aContext, TInt aErr) const;
	virtual void TimerExpired(CEncryptionKeyRefresher& aContext) const;
	virtual void EncryptionKeyRefreshComplete(CEncryptionKeyRefresher& aContext, TInt aError) const;

protected:
	// Exits old state, sets the new state, and enters it.
	void ChangeState(CEncryptionKeyRefresher& aContext, CEncryptionKeyRefresherStateFactory::TEncryptionKeyRefresherStates aState) const;
	void PanicInState(TLinkPanic aPanic) const;
	
protected:
	CEncryptionKeyRefresherStateFactory& iFactory;
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif
	};


//--------------------------------------------------
// STATES 
//--------------------------------------------------

NONSHARABLE_CLASS(TEkrStateIdle) : public TEncryptionKeyRefresherState
	{
public:
	TEkrStateIdle(CEncryptionKeyRefresherStateFactory& aFactory);

	virtual void Enter(CEncryptionKeyRefresher& aContext) const;
	virtual void Start(CEncryptionKeyRefresher& aContext) const;
	};


NONSHARABLE_CLASS(TEkrStateDisablingLPM) : public TEncryptionKeyRefresherState
	{
public:
	TEkrStateDisablingLPM(CEncryptionKeyRefresherStateFactory& aFactory);

	virtual void Enter(CEncryptionKeyRefresher& aContext) const;
	virtual void EventReceived(CEncryptionKeyRefresher& aContext, TBTBasebandEventNotification& aEvent) const;
	};

NONSHARABLE_CLASS(TEkrStateRefreshingKey) : public TEncryptionKeyRefresherState
	{
public:
	TEkrStateRefreshingKey(CEncryptionKeyRefresherStateFactory& aFactory);

	virtual void Enter(CEncryptionKeyRefresher& aContext) const;
	virtual void EncryptionKeyRefreshComplete(CEncryptionKeyRefresher& aContext, TInt aError) const;
	};


#ifdef __FLOG_ACTIVE
#define STATENAME(x)  iName=_L(x)
#else
#define STATENAME(x)
#endif


#endif /* ENCRYPTIONKEYREFRESHHELPER_H */
