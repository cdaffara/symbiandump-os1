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
 @internalComponent
 @released
*/

#ifndef LTSYPANICCODES_H
#define LTSYPANICCODES_H

#ifdef USING_CTSY_DISPATCHER

#include <e32base.h>

_LIT( KLtsyPanicCategory, "Ltsy" );

/** Ltsy panic values*/
enum TLtsyPanicCodes
    {
    ELtsyReceiveThreadDied,
    };

#endif //USING_CTSY_DISPATCHER

#endif //LTSYPANICCODES_H