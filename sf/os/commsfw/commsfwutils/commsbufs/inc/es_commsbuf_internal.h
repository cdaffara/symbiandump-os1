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
//

#ifndef __ESCOMMSBUFINTERNAL_H__
#define __ESCOMMSBUFINTERNAL_H__

#define ALIGN_MASK    0x03
#define ALIGN_UP(n)   (((n)+(ALIGN_MASK)) & ~ALIGN_MASK)
#define ALIGN_DOWN(n) ((n) & ~ALIGN_MASK)
#define IS_ALIGNED(p) ((((TUint32)(p)) & ALIGN_MASK)==0)

#include <es_commsbuf.h>
#include <comms-infras/commsbufq.h>
#include <comms-infras/commsbufasyncrequest.h>
#endif