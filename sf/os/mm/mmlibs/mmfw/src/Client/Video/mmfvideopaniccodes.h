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

#ifndef __MMF_VIDEO_PANIC_CODES_H__
#define __MMF_VIDEO_PANIC_CODES_H__

#include <e32std.h>

enum TMmfVideoPanic
	{
	ENoVideoTypeL
	};

GLDEF_C void Panic(TMmfVideoPanic aError);

#endif //__MMF_VIDEO_PANIC_CODES_H__
