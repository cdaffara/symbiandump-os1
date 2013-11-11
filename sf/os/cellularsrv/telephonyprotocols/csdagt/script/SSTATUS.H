/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Script Status header
* 
*
*/



/**
 @file SStatus.h
*/



#ifndef __SSTATUS_H__
#define	__SSTATUS_H__

#include <e32base.h>


class CScriptStatus : public CBase
/**
CScriptLabelMan, CScriptVarMan and CScriptCommandBase derived from this class. 

@internalComponent
*/
	{
public:
	CScriptStatus(TInt& aOffset,TPtrC& aLine,TBool& aSkip,TBool& aSkipModeToggleReq);
	CScriptStatus(const CScriptStatus& aStatus);
	~CScriptStatus();
public:
	TInt& iOffset;
	TPtrC& iLine;
	TBool& iSkip;
	TBool& iSkipModeToggleReq;
	};

#endif
