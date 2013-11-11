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
// Record the infomation related to hangup commands

//system include

//user include
#include "athangupcommandrecords.h"
#include "mslogger.h"

TATHanupCommandInfo::TATHanupCommandInfo()
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting TATHanupCommandInfo::TATHanupCommandInfo()"));	
	iCallId = -1;
	iHangupCause = -1;
	}

/**
 * 
 *
 */
CATHangupCommandRecords* CATHangupCommandRecords::NewL()
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::NewL()"));
	CATHangupCommandRecords* self = CATHangupCommandRecords::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CATHangupCommandRecords* CATHangupCommandRecords::NewLC()
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::NewLC()"));
	CATHangupCommandRecords* self = new (ELeave) CATHangupCommandRecords;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CATHangupCommandRecords::~CATHangupCommandRecords()
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::~CATHangupCommandRecords()"));
	iHangupInfoArray.Close();
	}

CATHangupCommandRecords::CATHangupCommandRecords()
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::CATHangupCommandRecords()"));
	iHangupActive = EFalse;
	}

void CATHangupCommandRecords::ConstructL()
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::ConstructL()"));
	}

TInt CATHangupCommandRecords::AddHangupCommandInfo(TInt aCallId, TInt aHangupCase)
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::AddHangupCommandInfo()"));
	TATHanupCommandInfo tInfo;
	tInfo.iCallId = aCallId;
	tInfo.iHangupCause = aHangupCase;
	TInt nRes = iHangupInfoArray.Append(tInfo);
	return nRes;
	}

TInt CATHangupCommandRecords::GetFirstHangupCommandInfoAndRemoveIt(TATHanupCommandInfo& aInfo)
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::GetFirstHangupCommandInfoAndRemoveIt()"));
	TInt nCount = iHangupInfoArray.Count();
	LOGTEXT2( _L8("[Ltsy CallControl] Hang up command information array count = %d"), nCount);
	
	if (nCount > 0)
		{
		aInfo.iCallId = iHangupInfoArray[0].iCallId;
		aInfo.iHangupCause = iHangupInfoArray[0].iHangupCause;
		iHangupInfoArray.Remove(0);
		return KErrNone;
		}
	return KErrNotFound;
	}

void CATHangupCommandRecords::SetHangupActiveFlag(TBool aIsActive)
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::SetHangupActiveFlag()"));
	iHangupActive = aIsActive;
	}

TBool CATHangupCommandRecords::GetHangupActiveFlag() const
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CATHangupCommandRecords::GetHangupActiveFlag()"));
	return iHangupActive;
	}


TBool CATHangupCommandRecords::IsHaveRecords() const
	{
	if (iHangupInfoArray.Count() > 0)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

//End of file
