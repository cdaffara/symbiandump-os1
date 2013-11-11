// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef FBSTRACE_H_
#define FBSTRACE_H_

#ifdef  GRAPHICS_TRACE
#  define FBS_OST(x) GRAPHICS_TRACE(x)
#  define FBS_OST_IF(arg, x) if(arg){GRAPHICS_TRACE(x)}
#else
#  ifdef _DEBUG
#    define FBS_OST(x) x
#    define FBS_OST_IF(arg, x) if(arg){FBS_OST(x)}
#  else
#    define FBS_OST(x)
#    define FBS_OST_IF(arg, x)
#  endif
#endif

#ifdef FBS_VERBOSE
#  define FBS_OST_VERBOSE(x) FBS_OST(x)
#  define FBS_OST_VERBOSE_IF(arg, x) FBS_OST_IF(arg, x)
#else
#  define FBS_OST_VERBOSE(x)
#  define FBS_OST_VERBOSE_IF(arg, x)
#endif

#endif /* FBSTRACE_H_ */
