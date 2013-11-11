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
* Script Executor inline functions.
* 
*
*/



/**
 @file Sscrexec.inl
*/

#ifndef __SSCREXEC_INL__
#define __SSCREXEC_INL__

#include "SSCREXEC.H"
#include "SSCRREAD.H"
#include "ND_SCR.H"

inline TBool CScriptExecutor::ReadFound() const
/**
Return read found flag.
*/
	{
	return iReadFound;
	}

inline TBool CScriptExecutor::ReadPctPending() const
/**
Return read from PCT penging flag.
*/
	{
	return iReadFromPctPending;
	}

inline TBool CScriptExecutor::IsScriptSet() const
/**
Return is script set flag.
*/
	{
	return iScriptReader->IsScriptSet();
	}

inline TBool CScriptExecutor::RequestUsePct() const
/**
Return is PCT used flag.
*/
	{
	return iScript->UsePct();
	}

inline 	const TDesC& CScriptExecutor::BcaStack() const
	{
	ASSERT(iScript);
	return iScript->BcaStack();
	}

inline TInt CScriptExecutor::IapId() const
	{
	ASSERT(iScript);
	return iScript->IapId();
	}

#endif
