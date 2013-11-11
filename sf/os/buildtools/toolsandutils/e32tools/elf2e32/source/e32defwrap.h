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
// Wrapper for e32def.h to undefine _L macros to prevent
// warnings.
// @internalComponent
// @released

// 
//

#ifndef __E32DEFWRAP_H__
#define __E32DEFWRAP_H__

#ifdef _L
#undef _L
#endif



#include <e32def.h>
#include <e32def_private.h>

#ifdef _L
#undef _L
#endif


#endif  // E32IMAGEDEFS_H

