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
// @file atclcc.h
// This contains CATClcc which query incoming or waitting call information.
// 


#ifndef ATCLCC_H_
#define ATCLCC_H_

//system include

//user include
#include "atcommandbase.h"

//class forward
class MCallInformationQueryCompleteNotify;

class CATClcc : public CAtCommandBase
	{
public:
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATClcc* NewL(CGlobalPhonemanager& aGloblePhone,
			             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CATClcc* NewLC(CGlobalPhonemanager& aGloblePhone,
			              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CATClcc();
	
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
	
	/**
	 * Init member variable
	 * 
	 */
	void InitVariable();
	
	/**
	 * Add query call information complete obsever 
	 * 
	 */
	void AddQueryCallInformationCompleteNotify(MCallInformationQueryCompleteNotify* aQueryCallInfoNotify);

public:
	
	/**
	 * AT execute result
	 * 
	 */
	enum TATProcessStates
		{
		EProcessNotInProgress,
		EProcessInProgress,
		EProcessError,
		EProcessOK
		};
	
	/**
	 * Get AT Process status
	 * 
	 */
	TATProcessStates GetProcessStates() const;
	
	/**
	 * Set AT Process status
	 * 
	 */
	void SetProcessStates(TATProcessStates aProcessState);
	
protected:
	
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CATClcc(CGlobalPhonemanager& aGloblePhone,
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
	void HandleCLCCComplete();	
	
	/**
	 * Parse AT clcc result
	 * 
	 */
	void ParseCLCCResponseL(const TDesC8& aResponseBuf);
	
private:
	
	/**
	 * At step
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
	 * Find OK result
	 * 
	 */
	TBool iOKFounded;
	
	/**
	 * Identifier step
	 * 
	 */
	TATStep iATStep;
	
	/**
	 * Process at command states
	 * 
	 */
	TATProcessStates iProcessStates;
	
	/**
	 * Observer which notify querying call information result
	 * 
	 */
	MCallInformationQueryCompleteNotify* iQueryCallInfoNotify;
	};

#endif /*ATCLCC_H_*/