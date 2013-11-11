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
// State transitions for the CLbsSuplPushImpl class from the EWaitingAck state
// 
//

#if (!defined __TE_LBSSUPLPUSHWAITINGACKTRANS__)
#define __TE_LBSSUPLPUSHWAITINGACKTRANS__

#include <lbs/lbssuplpush.h>

#include "Te_LbsSuplPushBaseTrans.h"

/**
The SuplInit transition for the CLbsSuplPushImpl class from the EWaitingAck state

@see CTe_LbsSuplPushBaseSuplInit
*/
class CTe_LbsSuplPushWaitingAckSuplInit : public CTe_LbsSuplPushBaseSuplInit
	{
public:
	static CTe_LbsSuplPushWaitingAckSuplInit* NewLC(CTe_LbsSuplPushStateStep& aStep);
	
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	virtual TInt State();
	virtual TInt WrongBusyPropState();
	virtual TInt RPropertyGetErrorState();
	virtual TInt RPropertySetErrorState();
	
private:
	CTe_LbsSuplPushWaitingAckSuplInit(CTe_LbsSuplPushStateStep& aStep);
	};
	
/**
The OnTimerEvent transition for the CLbsSuplPushImpl class from the EWaitingAck state

@see CTe_LbsSuplPushBaseOnTimerEvent
*/	
class CTe_LbsSuplPushWaitingAckOnTimerEvent : public CTe_LbsSuplPushBaseOnTimerEvent
	{
public:
	static CTe_LbsSuplPushWaitingAckOnTimerEvent* NewLC(CTe_LbsSuplPushStateStep& aStep);
	
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	virtual TInt SingleSuccessState();
	virtual TInt SeveralSuccessState();
	virtual TInt SingleErrorState();
	virtual TInt SeveralErrorState();
	
private:
	CTe_LbsSuplPushWaitingAckOnTimerEvent(CTe_LbsSuplPushStateStep& aStep);
	};

	
/**
The RunL transition for the CLbsSuplPushImpl class from the EWaitingAck state

@see CTe_LbsSuplPushBaseRunL
*/	
class CTe_LbsSuplPushWaitingAckRunL : public CTe_LbsSuplPushBaseRunL
	{
public:
	static CTe_LbsSuplPushWaitingAckRunL* NewLC(CTe_LbsSuplPushStateStep& aStep);
	
	virtual void OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId,
			TDesC8& aMsg);
	
	virtual void RunTransL();
	
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	virtual TInt SingleSuccessState();
	virtual TInt SeveralSuccessState();
	virtual TInt SingleErrorState();
	virtual TInt SeveralErrorState();
	
private:
	CTe_LbsSuplPushWaitingAckRunL(CTe_LbsSuplPushStateStep& aStep);
	void CheckErrorsL(TInt aFunction, TInt aError, TUint aMessagesInQueue);
	
private:
	/** The function where error must happen */
	TInt iFunction;
	/** The error that must happen */
	TInt iError;
	/** Number of func calls (including the current) after which the error happens */
	TInt iCount;

	};

		
#endif //__TE_LBSSUPLPUSHWAITINGACKTRANS__

