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
// State transitions for the CLbsSuplPushImpl class from the ECreated state
// 
//

#if (!defined __TE_LBSSUPLPUSHCREATEDTRANS__)
#define __TE_LBSSUPLPUSHCREATEDTRANS__

#include <lbs/lbssuplpush.h>

#include "Te_LbsSuplCommon.h"
#include "Te_LbsSuplPushBaseTrans.h"


/**
The SuplInit transition from the ECreated state for the CLbsSuplPushImpl class.

@see CTe_LbsSuplPushBaseSuplInit
*/
class CTe_LbsSuplPushCreatedSuplInit : public CTe_LbsSuplPushBaseSuplInit
	{
public:
	static CTe_LbsSuplPushCreatedSuplInit* NewLC(CTe_LbsSuplPushStateStep& aStep);
	
protected:
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	virtual TInt State();
	virtual TInt WrongBusyPropState();
	virtual TInt RPropertyGetErrorState();
	virtual TInt RPropertySetErrorState();
	
private:
	CTe_LbsSuplPushCreatedSuplInit(CTe_LbsSuplPushStateStep& aStep);
	};
	
/**
The forbidden transitions from the ECreated state for the CLbsSuplPushImpl class.

@see CTe_LbsSuplPushBaseTrans
*/
class CTe_LbsSuplPushCreatedForbidden : public CTe_LbsSuplPushBaseTrans
	{
public:
	static CTe_LbsSuplPushCreatedForbidden* NewLC(CTe_LbsSuplPushStateStep& aStep);
	void RunTransL();
	virtual CLbsSuplPushImpl* MakeSuplPushImplLC();
	
private:
	CTe_LbsSuplPushCreatedForbidden(CTe_LbsSuplPushStateStep& aStep);
	};

		
#endif //__TE_LBSSUPLPUSHCREATEDTRANS__

