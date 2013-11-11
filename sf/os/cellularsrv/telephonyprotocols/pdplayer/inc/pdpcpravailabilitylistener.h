// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PDP Connection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/


#ifndef PDPCPRAVAILABILITYLISTENER_H_
#define PDPCPRAVAILABILITYLISTENER_H_

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <etelpckt.h>
#include "PDPProvision.h"
#include "MPDPCPRBearerObserver.h"

class CPDPCPRDynamicCapsListener : public CActive
	{
public:
	static CPDPCPRDynamicCapsListener* NewL(const CTSYProvision* aTsyProvision);
	~CPDPCPRDynamicCapsListener();
	TUint DynamicCaps();
	void NotifyDynamicCapsChange(MPDPCPRBearerObserver* aObserver);
	TBool IsDynamicCapsSupported();
	
	// CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CPDPCPRDynamicCapsListener(const CTSYProvision* aTsyProvision);
	void ConstructL();
	void GetPhoneInfoL();
	void InitialiseDynamicCaps();
	
private:
	RTelServer iTelServer;
	RTelServer::TPhoneInfo iPhoneInfo;
	RMobilePhone iPhone;
	RPacketService iPacketService;
	RPacketService::TStatus iPacketServiceStatus;
	const CTSYProvision* iTsyProvision;
	MPDPCPRBearerObserver* iObserver;
	RPacketService::TDynamicCapsFlags iDynCaps;		// TDynamicCapsFlags is TUint typedef
	};


class CPDPCPRNetworkModeListener : public CActive
	{
public:
	static CPDPCPRNetworkModeListener* NewL(const CTSYProvision* aTsyProvision);
	~CPDPCPRNetworkModeListener();
	RMobilePhone::TMobilePhoneNetworkMode NetworkMode();
	void NotifyNetworkModeChange(MPDPCPRBearerObserver* aObserver);
	
	// CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CPDPCPRNetworkModeListener(const CTSYProvision* aTsyProvision);
	void ConstructL();
	void GetPhoneInfoL();
	void InitialiseNetworkMode();
	
private:
	RTelServer iTelServer;
	RTelServer::TPhoneInfo iPhoneInfo;
	RMobilePhone iPhone;
	const CTSYProvision* iTsyProvision;
	MPDPCPRBearerObserver* iObserver;
	RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;
	};

#endif /*PDPCPRAVAILABILITYLISTENER_H_*/
