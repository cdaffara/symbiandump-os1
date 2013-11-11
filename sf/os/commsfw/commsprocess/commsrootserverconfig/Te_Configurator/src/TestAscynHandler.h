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
// TestAsynchHandler.cpp
// 
//

#if (!defined __TESTASCYNCHRONUSHANDLER_STEP_H__)
#define __TESTASCYNCHRONUSHANDLER_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_ConfiguratorSuiteStepBase.h"
#include <comms-infras/c32startcli.h>
#include <e32property.h>
using namespace RootServer;


class CCanceledLoadCpm : public CActive
	{

public:
	~CCanceledLoadCpm();
	static CCanceledLoadCpm* NewL();
	// requests
	void TryLoad(const TCFModuleName& aName);
	void TryUnLoad( const TCFModuleName& aName, TRSUnLoadType aType );
    void TryWaitForDeath(void);
	void TryCancelDeath();
	void WaitForTimer(TInt aTimeout_ms);

	typedef enum 
		{
		EIdle,
		ELoading,
		EUnLoading,
		EBinding,
		EUnBinding,
		EListeningforDeath,
		EWaitingforTimer
		} TAsynchHandlerStates;
	TAsynchHandlerStates GetiState(void) const { return iState; }
	RRsConfigurator& Configurator()
		{
		return iConfigurator;
		}
private:
	// construct/destruct
	CCanceledLoadCpm();
	void ConstructL();
	// from CActive
	void RunL();
	void DoCancel();
	TCFModuleName iName;
	RRsConfigurator iConfigurator;
	RTimer iTimer;
	TAsynchHandlerStates iState;
	RProperty iPropertyDeath;
	};

#endif

