// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "AsyncErrorKicker.h"
//	NB. With a little interface factoring, the dependency on rfcommsap.h could
//	be reduced to a dependency on some M class interface definition...
#include "rfcommsap.h"

CAsyncErrorKicker::CAsyncErrorKicker(CRfcommSAP& aSAP, TInt aPriority)
:	CAsyncOneShot(aPriority),
	iSAP(aSAP)
	{
	}
	
void CAsyncErrorKicker::SetError(TInt aErrorCode, TInt aErrorType)
	{
	iErrorCode=aErrorCode;
	iErrorType=aErrorType;
	}

void CAsyncErrorKicker::RunL()
	{
	iSAP.Error(iErrorCode, STATIC_CAST(CRfcommSAP::TErrorTypes,iErrorType));
	}

