// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @released
*/

#if !defined(__ET_STRUCT_H__)
#define __ET_STRUCT_H__

#if !defined(__ETEL_H__)
#include "etel.h"
#endif
/**
@internalComponent
*/
struct TCallInfoIndex
	{
	TInt iIndex;
	RLine::TCallInfo iInfo;
	};
/**
@internalComponent
*/
struct TLineInfoIndex
	{
	TInt iIndex;
	RPhone::TLineInfo iInfo;
	};

#endif
