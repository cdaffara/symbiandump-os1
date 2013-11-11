/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
* @publishedAll
*/

#ifndef _A3F_TRACE_CTXT_H_
#define _A3F_TRACE_CTXT_H_

    
/**
@publishedAll
@released

This is TTraceCtxGroup

*/

enum TTraceCtxGroup {
        CtxTest,
        CtxDevSound,
        CtxDsProxy,
        CtxDsStarter,
        CtxDsServer,
        CtxDsdaptation,
        CtxHwDevice,
        CtxPolicyProxy,
        CtxPolicyServer,
        CtxPolicyCtrl,
        CtxMmRm,
        CtxAudioServer,
        CtxAsProxy,
        CtxAsServer,
        CtxAsCtrl,
        CtxAsEap,
        CtxMdaif,
        CtxAaMm,
        CtxRadio
};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <a3f/a3f_trace_ctxt_def.h>
#endif
#endif  // _A3F_TRACE_CTXT_H_
            
// End of File
