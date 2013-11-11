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
// @file panic.cpp
// Panic codes and panic function. This header should be kept private and not exported to
// /epoc32/include/

#include <e32std.h>		/// for GLDEF_C and User::
#include "panic.h"		/// Header for this file

//
// Panic Function
//
GLDEF_C void Panic(TTSYPanic aPanicNumber)
	{
	_LIT(panicText,"PhoneTsy");
	_LIT(extension,".TSY");

	TName myTsyName(panicText);

	#ifdef TSY_INSTANCE
		TBuf16<16> tgt;
		tgt.Num(TSY_INSTANCE);
		myTsyName.Append(tgt);
	#endif
	myTsyName.Append(extension);

	User::Panic(myTsyName,aPanicNumber);
	}
