// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Header file for E32 Image implementation of the elf2e32 tool
// @internalComponent
// @released
// 
//

#define __PLACEMENT_NEW
#define __PLACEMENT_VEC_NEW_INLINE

#ifndef E32IMAGEDEFS_H
#define E32IMAGEDEFS_H

// Pick up E32Image definitions
// need the following to suppress interence from EPOC definitions
#define __PLACEMENT_NEW
#define __PLACEMENT_VEC_NEW_INLINE
#ifdef _L
#undef _L
#endif

#ifdef _S 
#undef _S
#endif
#include <e32ldr.h>
#include <f32image.h>
#include <e32def_private.h>

#endif  // E32IMAGEDEFS_H

