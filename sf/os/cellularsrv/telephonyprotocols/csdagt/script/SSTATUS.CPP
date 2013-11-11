// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Script Status header
// 
//

/**
 @file SSTATUS.CPP
*/



#include "sstatus.h"

CScriptStatus::CScriptStatus(TInt& aOffset,TPtrC& aLine,TBool& aSkip,TBool& aSkipModeToggleReq)
	: iOffset(aOffset), iLine(aLine), iSkip(aSkip), iSkipModeToggleReq(aSkipModeToggleReq)
	{}

CScriptStatus::CScriptStatus(const CScriptStatus& aStatus)
	: iOffset(aStatus.iOffset), iLine(aStatus.iLine), iSkip(aStatus.iSkip), iSkipModeToggleReq(aStatus.iSkipModeToggleReq)
	{}


