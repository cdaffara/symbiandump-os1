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
//

#ifndef __SSMSERVER_H__
#define __SSMSERVER_H__

#include <e32base.h>
#include <ssm/ssmstates.hrh>

class TSsmState;
class TSsmSwp;
class CSsmStateTransitionEngine;
class CSsmSwpRequestHandler;
class CSsmSwpPolicyResolver;
class CCleSessionProxy;
class CSsmUiSpecific;

/**
 Implements the API used to request changes to the
 system state or to request changes to the value of a system-wide property.
 The policy plug-in associated with the current system state or the requested 
 system wide property implements the required security policy in 
 MSsmStatePolicy::TransitionAllowed() or MSsmSwpPolicy::TransitionAllowed().
 @internalComponent
 @released
 */
class CSsmServer : public CPolicyServer
	{
public:
	enum TValidation
		{
		EDoValidateCommandLists, 
		ENoCommandListValidation
		};
	
	static CSsmServer* NewLC();
	static CSsmServer* NewLC(TUint16 aInitialState);
	static CSsmServer* NewLC(TUint16 aInitialState, TValidation aSetting);

#ifdef TEST_SSM_SERVER
	static CSsmServer* NewLC(const TDesC& aServerName, TUint16 aInitialState); //used by automated tests to avoid clash with production server name
#endif // TEST_SSM_SERVER

	~CSsmServer( );

	void RequestStateTransitionL(const TSsmState& aState, TInt aReason,	const RMessage2& aMessage );
	void RequestStateTransitionCancel(CSession2* aSession);
	void RequestSwpChangeL(const TSsmSwp&, const RMessage2& aMessage );
	void RequestSwpChangeCancel(CSession2* aSession);
	void RequestRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename);
#ifdef _DEBUG
	void RequestDeRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename);
	void DoCleanupStateTransitionEngine();
	void DoCleanupSwpRequestHandler();
#endif

private:
	CSsmServer( );
	void ConstructL(TUint16 aInitialState, TValidation aSetting );

	//from CServer2
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage ) const;

private:
	CSsmStateTransitionEngine* iStateTransitionEngine;

	CSsmSwpRequestHandler* iSwpRequestHandler;
	CSsmSwpPolicyResolver* iSwpResolver;
	CCleSessionProxy* iSwpCleSession;
	CSsmUiSpecific* iSsmUiSpecific;
	};

#endif
