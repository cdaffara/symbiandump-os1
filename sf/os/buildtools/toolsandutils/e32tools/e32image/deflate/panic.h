// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// e32tools\petran\Szip\panic.h
// 
//

#ifndef __PANIC_H__
#define __PANIC_H__

#include <e32std.h>

enum TPanic
	{
	EHuffmanTooManyCodes=0,
	EHuffmanInvalidCoding=1,
	EHuffmanBufferOverflow=2,
	EHuffmanOutOfMemory=3,
	EHuffmanCorruptFile=4,

	};

void Panic(TPanic aPanic);

#endif

