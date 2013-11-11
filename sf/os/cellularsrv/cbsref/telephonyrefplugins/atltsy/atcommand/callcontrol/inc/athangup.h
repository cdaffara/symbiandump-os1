// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Hangup a ongoning call

#ifndef ATHANGUP_H_
#define ATHANGUP_H_

//system include

//user include
#include "atcommandbase.h"

//class forward
class MAllCallReleaseCompleteObserver;

class CATHangUp : public CAtCommandBase
	{
public:
	/**
	 * Factory function
	 * 
	 */
	static CATHangUp* NewL(CGlobalPhonemanager& aGloblePhone,
			               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Factory function
	 * 
	 */
	static CATHangUp* NewLC(CGlobalPhonemanager& aGloblePhone,
			               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Destrctor
	 * 
	 */
	virtual ~CATHangUp();
	
	/**
	 * From CAtCommandBase
	 * 
	 */
	virtual void ExecuteCommand();
	
	/**
	 * From CAtCommandBase
	 * 
	 */
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	
	/**
	 * From CAtCommandBase
	 * 
	 */		
	virtual void ParseResponseL(const TDesC8& aResponseBuf);	
	
	/**
	 * From CRequestBase
	 * 
	 */	
	virtual void StartRequest();
	
	/**
	 * Add the call release observer
	 */
	void AddAllCallReleaseObserver(MAllCallReleaseCompleteObserver* aObserver);
	
	/**
	 * Reset the internal states
	 */
	void InitVariable();
	
public:
	
	/**
	 * Define the hungup opertion
	 * 
	 */
	enum THangupOperator
		{
		EUnknownAPI,
		ECustomAPI,
		ERMobilePhoneAPI
		};
	
	/**
	 * Set the hangup opertion type
	 * 
	 */
	void SetHangupOperator(THangupOperator aOperator);
	
protected:
	/**
	 * Constructor
	 * 
	 */
	CATHangUp(CGlobalPhonemanager& aGloblePhone,
			  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * 2nd constructor
	 * 
	 */
	void ConstructL();
	
	/**
	 * From CAtCommandBase
	 * 
	 */
	virtual void Complete();
	
private:
	/**
	 * Handle IO errors
	 * 
	 */
	void HandleIOError();
	
	/**
	 * Handle response error
	 * 
	 */
	void HandleResponseError();
	
	/**
	 * Handle response OK
	 * 
	 */
	void HandleDisconnectedComplete();		

private:
	/**
	 * Define the states for hanging up a call
	 * 
	 */
	enum THangUpStep
		{
		EATHangUpNotInProgress,
		EATHangUpWaitForWriteComplete,
		EATHangUpReadComplete
		};
	
private:
	/**
	 * I/O error when Read or Write
	 * include timeout
	 */
	TInt iIOStatus;
	
	/**
	 * At command return errors
	 * 
	 */
	TInt iATResult;
		
	/**
	 * Identifier step
	 * 
	 */
	THangUpStep iHangupStep;
	
	/**
	 * The hangup opertion type
	 * 
	 */
	THangupOperator iHangupOperator;
	
	/**
	 * Notify all release
	 * 
	 */
	MAllCallReleaseCompleteObserver* iAllCallReleaseObserver;
	
	};

#endif /*ATHANGUP_H_*/
