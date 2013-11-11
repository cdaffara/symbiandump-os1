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
// Graphics Resource - definitions common to both kernel side and user side
//

#ifndef SGCOMMON_H
#define SGCOMMON_H

#include <e32cmn.h>
#include <sgresource/sgconst.h>

const TInt KSgMaxMetaDataSize = 512;

NONSHARABLE_CLASS(TSgImageMetaData)
	{
public:
#ifndef __KERNEL_MODE__
	TSgImageMetaData(): iSizeInPixels(TSize::EUninitialized) {}
#endif
public:
	TSize iSizeInPixels;
	TInt iPixelFormat;
	};

const TUint32 KSgUsageBitMask = 0xFFFF;

enum TSgResourceAttributes
	{
	ESgResourceAttrCpuCached = 0x00010000
	};

#endif // SGCOMMON_H
