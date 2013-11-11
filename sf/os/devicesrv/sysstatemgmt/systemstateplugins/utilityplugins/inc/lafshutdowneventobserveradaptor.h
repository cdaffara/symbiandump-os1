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
//

#ifndef __LAFSHUTDOWNEVENTOBSERVERADAPTOR_H__
#define __LAFSHUTDOWNEVENTOBSERVERADAPTOR_H__

#include <e32base.h>
#include <ssm/ssmloadlafshut.h> 
#include <ssm/ssmutility.h>
#include <shutdownsrv.h>

_LIT(KPanicShutdownEventObserverAdaptor, "ShutdownEventObserverAdaptor");

enum TShutdownObserverAdaptorPanicCodes
	{
	EInvalidSession = 1
	};

/**
Shutdown event observer adaptor class.
@internalComponent
*/
NONSHARABLE_CLASS(CLafShutdownEventObserverAdaptor) : public MSsmUtility, public MShutdownEventObserver
	{
public:
	static IMPORT_C MSsmUtility* NewL();

	// from MSsmUtility
	void InitializeL();
	void StartL();
	void Release();

	// from MShutdownEventObserver
	void HandleShutdownEventL(MSaveObserver::TSaveType aAction,TBool aPowerOff, TPowerState aEvent = EPwStandby);
	CArrayFix<TThreadId>* ClientArrayLC();
	TBool IsClientHung(TThreadId aId) const;
	void GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const;

private:
	CLafShutdownEventObserverAdaptor();
	~CLafShutdownEventObserverAdaptor();

private:
	class RShutdownSrvSession: public RSessionBase
		{
	public:
		void HandleShutdownEventL(MSaveObserver::TSaveType aAction,TBool aPowerOff, TPowerState aEvent = EPwStandby);
		CArrayFix<TThreadId>* ClientArrayLC();
		TBool IsClientHung(TThreadId aId) const;
		void GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const;

		TInt ConnectL();
	private:
		TInt StartServerL();
		};

private:
    MSsmLoadLafShut* iLafShutdown;
    RLibrary iLafShutLib;
	RShutdownSrvSession iShutdownSrvSession;
	};

#endif	//__LAFSHUTDOWNEVENTOBSERVERADAPTOR_H__

