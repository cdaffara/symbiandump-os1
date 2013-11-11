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
 @internalComponent
*/


#ifndef LINKESTABLISHMENT_H
#define LINKESTABLISHMENT_H

#include <e32base.h>

class CDelay;
class CHCTLBcsp;

class TLinkState;
class CLinkStateFactory;

NONSHARABLE_CLASS(CLinkEstablishment) :public CBase
/**
CLinkEstablishment
Provides the initial link establishment entity
Owns
	CLinkStateFactory (State Pattern)
  	
This class is not intended for derivation.
*/
	{
public:
	void QueueReadForNextFrame(void);
	static CLinkEstablishment* NewL(CHCTLBcsp& aHCTLbcsp);
	~CLinkEstablishment();
	void ProcessLinkMsg(const TDesC8& aLinkMsg);


	void SendLinkMsg(const TDesC8& aLinkMsg);

	void SetState(const TLinkState& aLinkState);   //<Change state of device


	//TLinkState Stub Functions

	void HandleLinkMsgSync();
	void HandleLinkMsgSyncResp();
	void HandleLinkMsgConf();
	void HandleLinkMsgConfResp();

	void StartShyTimer();
	void StartConfTimer();

	void StopShyTimer();
	void StopConfTimer();

	void Choke();
	void UnChoke();
	
 	void HandlePeerReset();
 	void Reset();	

	TBool Muzzled();
	void ResetMuzzled();
private:
	CLinkEstablishment(CHCTLBcsp& aHCTLbcsp);

    void ConstructL();	

	void ProcessConfResp();
	void ProcessSyncResp();
	void LinkStateMachine(const TDesC8& aLinkMsg);	//processes received link messages
	void HandleConfTimeout();
	void HandleShyTimeout();
	static TInt ConfTimeout(TAny* aLink);
	static TInt ShyTimeout(TAny* aLink);

private:
	enum TBcspLinkStateTransition
		{
		ESyncRecvd,
		EConfRecvd
		};
		
	CHCTLBcsp& iHctlBcsp;
	CLinkStateFactory* iLinkStateFactory;
	const TLinkState* iLinkState;

	CDelay* iShyTimer;
	CDelay* iConfTimer;

	TInt iConfCnt;
	};
 #endif //LINKESTABLISHMENT_H
 
