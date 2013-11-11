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
// @file atchld.h
// This contains CATChld which hold, resume, release, swap one call.
// 

#ifndef ATCHLD_H_
#define ATCHLD_H_

//system include

//user include
#include "atcommandbase.h"

class CATChld : public CAtCommandBase
	{
public:
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATChld* NewL(CGlobalPhonemanager& aGloblePhone,
			             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATChld* NewLC(CGlobalPhonemanager& aGloblePhone,
			              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CATChld();
	
	/**
	* Virtual function. Inherited from CAtCommandBase
	* 
	* Execute AT Command
	*/
	virtual void ExecuteCommand();
		
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
	
	/**
	* Start AT request
	* 
	*/
	virtual void StartRequest();	
	
public:
	
	/**
	 * IPC type
	 * 
	 */
	enum TIpcType
		{
		ELtsyReqCallHangup = 0,   //Invoke function HandleHangUpReqL(TInt aCallId, TInt aHangupCause)     
		ELtsyReqCallHold = 1,	  //Invoke function HandleHoldReqL(TInt aCallId)
		ELtsyReqCallResume = 2,	  //Invoke function HandleResumeReqL(TInt aCallId)
		ELtsyReqCallSwap = 3	  //Invoke function HandleSwapReqL(TInt aHeldCallId, TInt aConnectedCallId)
		};
	
	/**
	 * which identify AT type
	 * 
	 */
	enum TChldType
		{
		EReleaseAllHeldCallOrUDUB = 0,
		EReleaseAllActiveCallAndAcceptHeldWaiting = 1,
		EHeldAllActiveCallAndAcceptHeldWaiting = 2,  //For held a call and resume a call
		EAddHeldCallToConversation = 3,
		ECallTranster =  4,
		ERleaseSpecificCall = 5,
		EHeldAllActiveCallExceptCallX = 6
		};
	
	/**
	 *Set IPC type 
	 * 
	 * @param aIpcType
	 */
	void SetIpcType(TIpcType aIpcType);	
	
	/**
	 * Set chld type
	 * 
	 * @param aManageType
	 */
	void SetChldType(TChldType aManageType);
	
	/**
	 * Set call id
	 * 
	 * @param aCallId
	 */
	void SetCallId(TInt aCallId);
	
	/**
	 * Set connected call id
	 * 
	 * @param aConnectedCallId
	 */
	void SetConnectedCallId(TInt aConnectedCallId);
	
	/**
	 * Set hang up case 
	 * 
	 * @param aHangupCase
	 */
	void SetHangupCase(TInt aHangupCase);
	
	/**
	 * Init member
	 * 
	 */
	void InitVariable();
	
	/**
	 * Prevent second holding when the aHoldCallId is holded
	 * 
	 * @param aHoldCallId
	 */
	TBool IsHoldCall(TInt aHoldCallId);
	
	/**
	 * Prevent second resumeing when the aActiveCallId is actived
	 * 
	 * @param aActiveCallId
	 */
	TBool IsActiveCall(TInt aActiveCallId);
	
protected:
	
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CATChld(CGlobalPhonemanager& aGloblePhone,
			CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/	
	void ConstructL();

	/**
	* The last execute end of this command
	* 
	*/	
	virtual void Complete();
	
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
	void HandleCHLDComplete();
	
	/**
	 * Hang up call complete
	 * 
	 */
	void HandleHangupComplete();
	
	/**
	 * Hold call complete
	 * 
	 */
	void HandleHoldComplete();
	
	/**
	 * Resume call complete
	 * 
	 */
	void HandleResumeComplete();
	
	/**
	 * Swap call complete
	 * 
	 */
	void HandleSwapComplete();
	
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
	TATStep iATStep;
	
	/**
	 * 
	 * 
	 */
	TIpcType iIpcType;
	
	/**
	 * 
	 * 
	 */
	TChldType iChldType;
	
	/**
	 * for hangup/hold/resum/swap
	 * 
	 */
	TInt iCallId;
	
	/**
	 * for swap
	 * 
	 */
	TInt iConnectedCallId;
	
	/**
	 * for hangup
	 * 
	 */
	TInt iHangupCause;
	
	};

#endif /*ATCHLD_H_*/
