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
// @file atanswer.h
// This contains CATAnswer which answering incomging call.
// 

#ifndef ATANSWER_H_
#define ATANSWER_H_

//system include

//user include
#include "atcommandbase.h"

class CATAnswer : public CAtCommandBase
	{
public:
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATAnswer* NewL(CGlobalPhonemanager& aGloblePhone,
			               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATAnswer* NewLC(CGlobalPhonemanager& aGloblePhone,
			                CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CATAnswer();
	
	/**
	 * Seting answer id
	 * 
	 * @param aCallId
	 */
	void SetAnswerCallId(TInt aCallId);
	
	/**
	 * Seting the call is or not 3rd party
	 * 
	 * 
	 * @param aIsIsvCall ETrue 3rd party  another EFalse
	 */
	void SetIsIsvCall(TBool aIsIsvCall);
	
	
	/**
	* Virtual function. Inherited from CAtCommandBase
	* 
	* Execute AT Command
	*/
	virtual void ExecuteCommand();
		
	/**
	* Start AT request
	* 
	*/
	virtual void StartRequest();
	
	/**
	* Will be called by AT Manager whenever a event was triggered
	* 
	* @param aEventSource
	* @param aStatus
	*/
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	
	/**
	* Virtual function. Inherited from CAtCommandBase
	* 
	* @param aResponseBuf Line buf reading from baseband 
	*/
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	
protected:
	
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CATAnswer(CGlobalPhonemanager& aGloblePhone, 
			  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/	
	void ConstructL();
	
	/**
	* The last execute end of this command
	*/	
	void Complete();
	
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
	void HandleConnectedComplete();		
	
private:
	
	/**
	 * AT command execute step
	 * 
	 */
	enum TATStep
		{
		EATNotInProgress,
		EATWaitForWriteComplete,
		EATReadComplete
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
	TATStep iAnswerStep;	
	
	/**
	 * answer call id
	 * 
	 */
	TInt iCallId;
	
	/**
	 * The flag of 3rd party call
	 * 
	 */
	TBool iIsIsvCall;
	
	};

#endif /*ATANSWER_H_*/
