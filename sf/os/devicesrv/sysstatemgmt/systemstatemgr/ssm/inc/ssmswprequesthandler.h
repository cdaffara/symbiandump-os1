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

#ifndef __SSMSWPREQUESTHANDLER_H__
#define __SSMSWPREQUESTHANDLER_H__

#include <e32base.h>
#include <ssm/ssmswppolicy.h>

class TSsmSwp;
class CSsmSwpTransitionScheduler;
class MSsmSwpPolicyResolverProxy;
class MCleSessionProxy;

/**
This helper Interface can be used for testing or to monitor the progress of the tranisitons
by wrapping the scheduler and registering the wrapper class as a monitor.

@internalComponent
@released

*/
class MSwpHandlerMonitor 
	{
public:
	virtual void SwpRequestMade(const TSsmSwp& aRequest) = 0;
	};

/**
 The handler class is responsible for receiving requests, seeking approval and then scheduling
 them to run according to the current strategy. 
 The current strategy is fixed in this implementation (this could change in future).
 The strategy is to queue all requests to run sequencially using a transition scheduler.
 @internalComponent
 @released
 */
class CSsmSwpRequestHandler : public CBase
	{

public: // methods
	static CSsmSwpRequestHandler* NewL();
	static CSsmSwpRequestHandler* NewLC();
	~CSsmSwpRequestHandler();
	
	void SubmitRequestL(const TSsmSwp& aRequest);
	void SubmitRequestL(const TSsmSwp& aRequest, const RMessage2& aMessage);
	void Cancel(CSession2* aSession);
	
	inline CSsmSwpTransitionScheduler& SwpTransitionScheduler() const {return *iSwpTransitionScheduler;}

	void SetSwpHandlerMonitor(MSwpHandlerMonitor* aMonitor);
	void SetSwpPolicyResolverProxy(MSsmSwpPolicyResolverProxy* aPolicyResolverProxy);
	void SetCleSessionProxy(MCleSessionProxy* aCleSessionProxy);
	void PerformCommandListValidation(TBool aSetting);
#ifdef _DEBUG
	void CleanupSwpTransitionScheduler();
#endif
	
private: // methods
	CSsmSwpRequestHandler(); // only accessible from factory methods
	void ConstructL();
	void DoSubmitRequestL(const TSsmSwp& aRequest, const RMessage2* aMessage);

	void NotifyRequestMade(const TSsmSwp& aRequest);

private: // members

	/** the scheduler that hold pending requests */
	CSsmSwpTransitionScheduler* iSwpTransitionScheduler;

	/** Optional Monitor handling functions */
	MSwpHandlerMonitor* iSwpHandlerMonitor;
	MSsmSwpPolicyResolverProxy* iSwpPolicyResolverProxy;
	MCleSessionProxy* iCleSessionProxy;
	TBool iPerformCommandListValidation;
	};

#endif
