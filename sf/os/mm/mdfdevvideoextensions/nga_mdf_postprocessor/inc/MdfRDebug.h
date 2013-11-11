/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __MDF_RDEBUG_H__
#define __MDF_RDEBUG_H__

#include <e32svr.h>
#include <e32def.h>
#include <flogger.h>

#ifdef _DEBUG
        #define PP_DEBUG RDebug::Print
#else
        #define PP_DEBUG 
#endif

#endif // __MDF_RDEBUG_H__ 
