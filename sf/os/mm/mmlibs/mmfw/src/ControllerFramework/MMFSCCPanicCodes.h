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
//

#ifndef __MMF_SCC_PANIC_CODES_H__
#define __MMF_SCC_PANIC_CODES_H__

#include <e32std.h>

enum TMmfSCCPanic
	{
	ENoGetVideoCodec,
	ENoGetSourceVideoTypes,
	ENoGetSourceAudioTypes
	};

GLDEF_C void Panic(TMmfSCCPanic aError);

#endif //__MMF_SCC_PANIC_CODES_H__
