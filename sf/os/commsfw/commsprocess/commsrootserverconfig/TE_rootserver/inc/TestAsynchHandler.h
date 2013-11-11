// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestAsynchHandler which is an active object to
// handle the asynch calls from the RootServer during testing

#if (!defined __TESTASYNCHHANDLER_H__)
#define __TESTASYNCHHANDLER_H__

#include "c32root.h"
#include <e32property.h>

class CTestAsynchHandler: public CActive
    {
public:
	// construct/destruct
	static CTestAsynchHandler* NewLC(RRootServ* aRootServer);
	~CTestAsynchHandler();

	// requests
    void TryLoad( const TRSStartModuleParams& aParams, const TDesC8& aIniData );
	void TryUnLoad( const TCFModuleName& aName, TRSUnLoadType aType );
	void TryBind( TRSBindingInfo& aBindInfo ); 
	void TryUnBind( TRSUnBindingInfo& aUnbindInfo );
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

private:
	// construct/destruct
	CTestAsynchHandler();
	void ConstructL(RRootServ* aRootServer);

	// from CActive
    void RunL();
	void DoCancel();

    RRootServ* iRootServer;
	RTimer iTimer;
    TAsynchHandlerStates iState;
	RProperty iPropertyDeath;
    };

#endif /* __TESTASYNCHHANDLER_H__ */
