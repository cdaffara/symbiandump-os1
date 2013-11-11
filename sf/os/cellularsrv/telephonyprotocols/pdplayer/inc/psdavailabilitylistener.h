// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// gprsavailabilitylistener.h
// 
//

#ifndef PSDAVAILABILITYLISTENER_H
#define PSDAVAILABILITYLISTENER_H

#include <comms-infras/coremcpractivities.h>
#include <networking/pdpprovision.h>
#include <etel.h>
#include <etelmm.h>

NONSHARABLE_CLASS(CPsdAvailabilityListener) : public CActive, public Messages::ASimpleNodeIdBase
/** Packet switched availability listeners

@internalTechnology
*/
	{
	enum TState {EInitialising, EChecking, EAttached, EUnAttached};

public:
	static CPsdAvailabilityListener* NewL(const Messages::TNodeCtxId& aAvailabilityActivity, const CTSYProvision& aTsyProvision, TUint aApId, TBool aUnavailableDueToContention);
	virtual ~CPsdAvailabilityListener();
	
#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
	void ReportContentionAvailabilityStatusIfRequired(const ESock::TAvailabilityStatus& aAvailabilityStatus);
#endif
	
private:
	CPsdAvailabilityListener(const Messages::TNodeCtxId& aAvailabilityActivity, const CTSYProvision& aTsyProvision, TUint aApId, TBool aUnavailableDueToContention);
	void StartL();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	// CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	const Messages::TNodeCtxId iAvailabilityActivity;
	const CTSYProvision* iTsyProvision;
	RMobilePhone iPhone;
	RTelServer iTelServer;
	RPacketService iPacketService;
	TInt iApId;
	RPacketService::TStatus iPsdStatus;
	TState iState;
	TBool iUnavailableDueToContention;
	};

#endif
