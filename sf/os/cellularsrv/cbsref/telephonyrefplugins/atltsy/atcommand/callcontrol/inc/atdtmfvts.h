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
// @file atdtmfvts.h
// This contains CATDtmfVts which send the DTMF to network.
// 

#ifndef ATDTMFVTS_H_
#define ATDTMFVTS_H_

//system include
#include <callbacktimer.h>
//user include
#include "atcommandbase.h"

//const define
const TInt KLtsyTonesLength = 200;


//class forward
class CGlobalPhonemanager;
class CCtsyDispatcherCallback;

class CATDtmfVts : public CAtCommandBase, 
				   public MTimerObserver
	{
public:
	/**
	 * Factory funciton
	 * 
	 */
	static CATDtmfVts* NewL(CGlobalPhonemanager& aGloblePhone,
			                CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Factory function
	 * 
	 */
	static CATDtmfVts* NewLC(CGlobalPhonemanager& aGloblePhone,
			                 CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CATDtmfVts();
	
	/**
	 * Execute the AT command
	 * 
	 */
	virtual void ExecuteCommand();
		
	/**
	 * Start the request
	 * 
	 */
	virtual void StartRequest();
	
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
	 * This call id is must an active call id
	 * 
	 */
	TInt SetCallId(TInt aCallId);
	
	/**
	 * Must be invoke when Execucommand
	 * 
	 */
	TInt SetDtmfString(const TDesC& aDtmfString);
	
	/**
	 * Check if the char is the DTMF tones
	 * 
	 */
	TBool CharIsDtmf(const TChar& aDtmfChar);
	
	/**
	 * Check if the string are DTMF tones
	 * 
	 */
	TBool StringIsDtmf(const TDesC& aDtmfString);
	
	/**
     * Reset the all the flag
	 * 
	 */
	void InitVariable();		
public:
	/**
	 * Define the DTMF type
	 * 
	 */
	enum TDtmfWorkType
		{
		EDtmfUnknow,
		EDtmfSendOneTone,   //Send One Char
		EDtmfSendMoreTones, //Send string tones
		EDtmfStopOneTone,   //Stop One char
		EDtmfCancelMoreTones //Cancel More Tones
		};
	
	/**
	 * 
     * Set the DTMF Type
	 */
	void SetDtmfWorkType(TDtmfWorkType aDtmfWorkType);
protected:	
	/**
	 * Constructor
	 * 
	 */
	CATDtmfVts(CGlobalPhonemanager& aGloblePhone,
			   CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * 2nd constructor
	 * 
	 */
	void ConstructL();	
private:
	/**
	 * Overwite the Parent's complete
	 * 
	 */
	virtual void Complete();
	
	/**
	 * Checck if there is any active call current
	 * 
	 */
	TBool IsHaveActiveCall(TInt aActiveCallId);	
	
	/**
	 * From MTimerObserver 
	 * 
	 */
    virtual void TimerRun(TInt aError);
    
    /**
     * Start the timer
     * 
     */
    void StartTimer();	
    
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
	 * Handle the complete event
	 * 
	 */
	void HandleSendDtmfTonesSuccess();
	
private:
	
	/**
	 * Define the state of sending the DTMF tone
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
	 * The sending is done successfully
	 * 
	 */
	TBool iOKFounded;
	
	/**
	 * Identifier step
	 * 
	 */
	TATStep iAnswerStep;	
	
	/**
	 * The calll Id
	 * 
	 */
	TInt iCallId;
	
	/**
	 * The DTMF type
	 * 
	 */
	TDtmfWorkType iDtmfWorkType;
	
	/**
	 * The timer active object
	 * 
	 */
	CCallbackTimer* iCallbackTimer;	
	};

#endif /*ATDTMFVTS_H_*/
