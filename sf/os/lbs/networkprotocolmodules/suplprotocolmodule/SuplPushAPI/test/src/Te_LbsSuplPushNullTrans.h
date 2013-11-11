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
// Creation transition for the CLbsSuplPushImpl class
// 
//

#if (!defined __TE_LBSSUPLPUSHNULLTRANS__)
#define __TE_LBSSUPLPUSHNULLTRANS__

#include <lbs/lbssuplpush.h>

class CLbsSuplPushImpl;

/**
Creation transition for the CLbsSuplPushImpl class.
*/
class CTe_LbsSuplPushNullNewL : public CBase, public MLbsSuplPushObserver
	{
public:
	static CTe_LbsSuplPushNullNewL* NewLC();
	
	void RunTransL();
	
	virtual void OnSuplInitComplete(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId,
			TInt aError, TInt aReserved);
	
private:
	CTe_LbsSuplPushNullNewL();

private:
	/** An instance of the class tested */
	CLbsSuplPushImpl* iPushImpl;
	};
	
#endif //__TE_LBSSUPLPUSHNULLTRANS__

