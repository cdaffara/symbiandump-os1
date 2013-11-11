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
// Base state transition classes  for the CLbsSuplPushImpl class
// 
//

#if (!defined __TE_LBSSUPLPUSHBASETRANS__)
#define __TE_LBSSUPLPUSHBASETRANS__

#include <lbs/lbssuplpush.h>
#include <lbs/lbssuplpushreceiver.h>
#include "lbssuplpushimpl.h"

class CLbsSuplPushImpl;

class CTe_LbsSuplPushStateStep;
	
/**
The base class for all state transitions for the CLbsSuplPushImpl class.
*/
class CTe_LbsSuplPushBaseTrans : public CBase, public MLbsSuplPushObserver
	{
public:	

	/**
	Runs the various scenarios for this transition.

	@leave If a error happens, it leaves with one of the system error codes.
	 */
	virtual void RunTransL() = 0;
	
	virtual void OnSuplInitComplete(TLbsSuplPushChannel aChannel, 
			TLbsSuplPushRequestId aReqId, TInt aError, TInt aReserved);
	
protected:
	CTe_LbsSuplPushBaseTrans(CTe_LbsSuplPushStateStep& aStep);
	
	/**
	Constructs the CSuplPushImpl object in the specific state. Places the created object on 
	the cleanup stack.
	
	@return A pointer on the created CSuplPushImpl object.
	
	@leave If a error happens, it leaves with one of the system error codes.
	*/	
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC() = 0;

protected:
	/** A reference on the test step owning this object */
	CTe_LbsSuplPushStateStep& iStep;	
	};
	

/**
The base class for all SuplInit state transitions for the CLbsSuplPushImpl class.

@see CTe_LbsSuplPushBaseTrans
*/
class CTe_LbsSuplPushBaseSuplInit : public CTe_LbsSuplPushBaseTrans
	{
public:
	virtual void RunTransL();
	
protected:
	CTe_LbsSuplPushBaseSuplInit(CTe_LbsSuplPushStateStep& aStep);
	
	/**
	Returns the possible main states the object is after the transition.
	
	@return The possible main states the object is after the transition.
	*/
	virtual TInt State() = 0;
	
	/**
	Returns the possible states object switches to after the transition when there is a wrong
	BUSY property set.
	
	@return The possible states object switches to after the transition when there is a wrong
	BUSY property set.
	*/
	virtual TInt WrongBusyPropState() = 0;
	
	/**
	Returns the possible states object switches to after the transition when the RProperty::Get
	function fails. 
	
	@return The possible states object switches to after the transition when the RProperty::Get
	function fails.
	*/
	virtual TInt RPropertyGetErrorState() = 0;
	
	/**
	Returns the possible states object switches to after the transition when the RProperty::Set
	function fails. 
	
	@return The possible states object switches to after the transition when the RProperty::Set
	function fails.
	*/
	virtual TInt RPropertySetErrorState() = 0;
	};

/**
The base class for all OnTimerEvent state transitions for the CLbsSuplPushImpl class.

@see CTe_LbsSuplPushBaseTrans
*/
class CTe_LbsSuplPushBaseOnTimerEvent : public CTe_LbsSuplPushBaseTrans
	{
public:
	virtual void RunTransL();
	
protected:
	CTe_LbsSuplPushBaseOnTimerEvent(CTe_LbsSuplPushStateStep& aStep);
	
	virtual void OnSuplInitComplete(TLbsSuplPushRequestId aReqId, TInt aError, TInt aReserved);
	
	/**
	Returns the possible states object switches to after the transition when no error happens and
	there is a single message in the queue. 
	
	@return The possible states object switches to after the transition when no error happens and
	there is a single message in the queue. 
	*/
	virtual TInt SingleSuccessState() = 0;
	
	/**
	Returns the possible states object switches to after the transition when no error happens and
	there are several messages in the queue. 
	
	@return The possible states object switches to after the transition when no error happens and
	there are several messages in the queue. 
	*/
	virtual TInt SeveralSuccessState() = 0;
	
	/**
	Returns the possible states object switches to after the transition when a error happens and
	there is a single message in the queue. 
	
	@return The possible states object switches to after the transition when a error happens and
	there is a single message in the queue. 
	*/
	virtual TInt SingleErrorState() = 0;
	
	/**
	Returns the possible states object switches to after the transition when a error happens and
	there are several messages in the queue. 
	
	@return The possible states object switches to after the transition when a error happens and
	there are several messages in the queue. 
	*/
	virtual TInt SeveralErrorState() = 0;
	
private:
	void CheckErrorsL(TInt aFunction, TInt aError, TUint aMessagesInQueue);
	};

/**
The base class for all RunL state transitions for the CLbsSuplPushImpl class.

@see CTe_LbsSuplPushBaseTrans
*/
class CTe_LbsSuplPushBaseRunL : public CTe_LbsSuplPushBaseTrans, 
	public MLbsSuplPushRecObserver
	{
public:
	virtual void RunTransL();
	
protected:
	CTe_LbsSuplPushBaseRunL(CTe_LbsSuplPushStateStep& aStep);
	
	
	/**
	Returns the possible states object switches to after the transition when a error happens and
	there is a single message in the queue. 
	
	@return The possible states object switches to after the transition when a error happens and
	there is a single message in the queue. 
	*/
	virtual TInt SingleErrorState() = 0;
	
	};
			
	
#endif //__TE_LBSSUPLPUSHBASETRANS__

