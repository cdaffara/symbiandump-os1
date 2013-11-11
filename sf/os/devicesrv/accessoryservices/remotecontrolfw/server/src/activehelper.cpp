// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Active helper implementation.
// 
//

/**
 @file 
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "activehelper.h"
#include "controllersession.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

CActiveHelper::CActiveHelper(CRemConControllerSession &aSession) : 
	CActive(CActive::EPriorityStandard),
	iSession(aSession)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}
	
void CActiveHelper::RunL()
	{
	LOG_FUNC;
	iSession.ProcessPendingMsgL();
	}
	
void CActiveHelper::DoCancel()
	{
	LOG_FUNC;
	}
	
CActiveHelper::~CActiveHelper()
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	Cancel();
	}

void CActiveHelper::Complete()
	{
	LOG_FUNC;
	iStatus = KRequestPending;
	SetActive();

	TRequestStatus *p = &iStatus;
	User::RequestComplete(p, KErrNone);
	}
