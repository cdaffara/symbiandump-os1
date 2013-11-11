// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#ifndef __PANIC_H__
#define __PANIC_H__

#include <e32std.h>

/** 
*/
_LIT(KCategory, "t_stress");

/** 
*/
enum TPanic
	{
	EPanic0,
	EPanic1,
	EPanic2,
	EPanic3,
	EPanic4,
	EPanic5,
	EPanic6,
	EPanic7,
	EPanic8,
	EPanic9,
	EPanic10,
	EPanic11,
	EPanic12,
	EPanic13,
	EPanic14,
	EPanic15,
	EPanic16,
	EPanic17,
	EPanic18,
	EPanic19,
	};

/** 
*/
static void Panic( TPanic aReason )
	{
	User::Panic(KCategory, aReason );
	}

#endif //__PANIC_H__
