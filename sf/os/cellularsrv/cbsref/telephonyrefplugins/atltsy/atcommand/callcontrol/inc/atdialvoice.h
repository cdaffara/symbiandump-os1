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
// @file atdialvoice.h
// This contains CATDialVoice which dial a voice call.
// 

#ifndef ATDIALVOICE_H_
#define ATDIALVOICE_H_

//system include
#include <etelmm.h>

//user include
#include "atcommandbase.h"
#include "unsolicitedparams.h"
#include "athangup.h"
#include "allcallreleasecompleteobserver.h"

class CATDialVoice : public CAtCommandBase, public MAllCallReleaseCompleteObserver
	{
public:
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATDialVoice* NewL(CGlobalPhonemanager& aGloblePhone,
			                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATDialVoice* NewLC(CGlobalPhonemanager& aGloblePhone,
			                   CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CATDialVoice();
	
	/**
	* Virtual function. Inherited from CAtCommandBase
	* 
	* Execute AT Command
	*/
	virtual void ExecuteCommand();
		
	/**
	* Virtual function. Inherited from CAtCommandBase
	* 
	* @param aResponseBuf Line buf reading from baseband 
	*/
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	
	/**
	* Will be called by AT Manager whenever a event was triggered
	* 
	* @param aEventSource
	* @param aStatus
	*/
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	
	/**
	* Start AT request
	* 
	*/
	virtual void StartRequest();
	
	/**
	 * set telephone number
	 * 
	 * @param aTelNum
	 */
	void SetTelephoneNumber(const TDesC8& aTelNum);
	
	/**
	 * ETrue : Emergency call
	 * EFalse : Normal call
	 */
	void SetEmergnecyCallFlag(TBool aIsEmergencyCall = EFalse);
	
	/**
	 * Init member
	 * 
	 */
	void InitVariable();
	
	/**
	 * From MAllCallReleaseCompleteObserver
	 * 
	 * @param aError
	 */
	virtual void ReleaseAllCallComplete(TInt aError);
	
protected:
	
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CATDialVoice(CGlobalPhonemanager& aGloblePhone,
			     CCtsyDispatcherCallback& aCtsyDispatcherCallback); 
	
	/**
	* 2nd Constructor
	* 
	*/	
	void ConstructL();
	
	/**
	* The last execute end of this command
	* 
	*/	
	virtual void Complete();
	
	/**
	 * Parse +Wind at command
	 * 
	 * @param aParams class TUnsolicitedParams
	 * @param aCommandBuf buffer of at command
	 */
	TInt ParseUnsolicitedCommandBufL(TUnsolicitedParams& aParams, const TDesC8& aCommandBuf);
	
	/**
	 * Parse +Wind at command
	 * 
	 * @param aParams class TUnsolicitedParams
	 */
	TInt ProcessUnsolicitedCallCreated(const TUnsolicitedParams& aParams);
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
	 * Handle call dialing complete
	 * 
	 */
	void HandleDiallingComplete();
	
	/**
	 * Handle call connecting complete
	 * 
	 */
	void HandleConnectingComplete();
	
	/**
	 * Handle call connected complete
	 * 
	 */
	void HandleConnectedComplete();
		
private:
	
	/**
	 * Dial voice call step
	 * 
	 */
	enum TDialVoicStep
		{
		EATDialNotInProgress,
		EATWaitForWriteComplete,
		EATWaitForDiallingComplete,
		EATWaitForConnectingComplete,
		EATWaitForConnectedComplete
		};
	
private:
	
	/**
	 * Telephony number
	 * 
	 */
	TBuf8<RMobilePhone::KMaxMobileTelNumberSize> iTelNum;
	
	/**
	 * call id
	 * 
	 */
	TInt iCallId;
	
	/**
	 * I/O error when Read or Write
	 * include timeout
	 */
	TInt iStatus;
	
	/**
	 * At Command return errors
	 * 
	 */
	TInt iResult;
	
	/**
	 * OK response is founded
	 * 
	 */
	TBool iOKFounded;
	
	/**
	 * State about reading and writing
	 * 
	 */
	TDialVoicStep iDialStep;
	
	/**
	 * True : Emergency Call
	 * False : Normal Call
	 * 
	 */
	TBool iIsEmergencyCall;
	
	/**
	 * When Emergency call release all call
	 */
	CATHangUp* iATH;
	};

#endif /*ATDIALVOICE_H_*/
