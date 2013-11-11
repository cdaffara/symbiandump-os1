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
// State transitions for the CLbsSuplPushImpl class from the EWaitingBusy state
// 
//

#if (!defined __TE_LBSSUPLPUSHWAITINGBUSYTRANS__)
#define __TE_LBSSUPLPUSHWAITINGBUSYTRANS__

#include <lbs/lbssuplpush.h>

#include "Te_LbsSuplPushBaseTrans.h"

/**
The SuplInit transition for the CLbsSuplPushImpl class from the EWaitingBusy state

@see CTe_LbsSuplPushBaseSuplInit
*/
class CTe_LbsSuplPushWaitingBusySuplInit : public CTe_LbsSuplPushBaseSuplInit
	{
public:
	static CTe_LbsSuplPushWaitingBusySuplInit* NewLC(CTe_LbsSuplPushStateStep& aStep);
	
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	virtual TInt State();
	virtual TInt WrongBusyPropState();
	virtual TInt RPropertyGetErrorState();
	virtual TInt RPropertySetErrorState();
	
private:
	CTe_LbsSuplPushWaitingBusySuplInit(CTe_LbsSuplPushStateStep& aStep);
	};
	

/**
The RunL transition for the CLbsSuplPushImpl class from the EWaitingBusy state

@see CTe_LbsSuplPushBaseRunL
*/
class CTe_LbsSuplPushWaitingBusyRunL : public CTe_LbsSuplPushBaseRunL
	{
public:
	static CTe_LbsSuplPushWaitingBusyRunL* NewLC(CTe_LbsSuplPushStateStep& aStep);
	
	virtual void OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId,
			TDesC8& aMsg);
	
	virtual void RunTransL();
	
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	virtual TInt SingleSuccessState();
	virtual TInt SeveralSuccessState();
	virtual TInt SingleErrorState();
	virtual TInt SeveralErrorState();
	
private:
	CTe_LbsSuplPushWaitingBusyRunL(CTe_LbsSuplPushStateStep& aStep);
	};

		
#endif //__TE_LBSSUPLPUSHWAITINGBUSYTRANS__

