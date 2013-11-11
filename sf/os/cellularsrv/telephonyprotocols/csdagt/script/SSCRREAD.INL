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
* Script Reader and Executor inline functions
* 
*
*/



/**
 @file Sscrread.inl
*/

#ifndef __SSCRREAD_INL__
#define __SSCRREAD_INL__

#include "SSCRREAD.H"

inline void CScriptReader::SetLoggingOff()
/**
Set logging on to EFalse.
*/
	{
	iLoggingOn=EFalse;
	}

inline void CScriptReader::SetLoggingOn()
/**
Set logging on to ETrue.
*/
	{
	iLoggingOn=ETrue;
	}

#endif
