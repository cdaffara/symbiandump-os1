/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* @internalTechnology 
*/

#ifndef _A3F_TRACE_CTXT_DEF_H_
#define _A3F_TRACE_CTXT_DEF_H_

// MACROS
//#define DISABLE_SYNTAX_CHECK
#define DISABLE_GROUP_CHECKS

struct TTraceName
    {
    const TUint32 iId;
    const TUint32 iGrpId;
#ifdef __WINS__
    const TText16* const iName;
#else
    const wchar_t* const iName;
#endif
    };
    
static const TTraceName TRACENFO[] = 
    {
        { CtxTest,           0x00000001, L"Test" },
        { CtxDevSound,       0x00000002, L"DevSound" },
        { CtxDsProxy,        0x00000004, L"DevSound proxy" },
        { CtxDsStarter,      0x00000008, L"DevSound starter" },
        { CtxDsServer,       0x00000010, L"DevSound server" },
        { CtxDsdaptation,    0x00000020, L"DevSound adaptation" },
        { CtxHwDevice,       0x00000040, L"HW device" },
        { CtxPolicyProxy,    0x00000080, L"Policy proxy" },
        { CtxPolicyServer,   0x00000100, L"Policy server" },
        { CtxPolicyCtrl,     0x00000200, L"Policy control" },
        { CtxMmRm,           0x00000400, L"Mm resource manager" },
        { CtxAudioServer,    0x00000800, L"Audio server" },
        { CtxAsProxy,        0x00001000, L"Audio server proxy" },
        { CtxAsServer,       0x00002000, L"Audio server server" },
        { CtxAsCtrl,         0x00004000, L"Audio server control" },
        { CtxAsEap,          0x00008000, L"Audio server EAP" },
        { CtxMdaif,          0x00010000, L"MDAIF" },
        { CtxAaMm,           0x00020000, L"Message mapper" },
        { CtxRadio,          0x00040000, L"Radio" }
    };

#define CtxDefaultGroups _CtxDefaultGroups()


inline TInt _CtxDefaultGroups()
	{
        return TRACENFO[CtxTest].iGrpId         | 
               TRACENFO[CtxDevSound].iGrpId     | 
               TRACENFO[CtxDsProxy].iGrpId      | 
               TRACENFO[CtxDsStarter].iGrpId    | 
               TRACENFO[CtxDsServer].iGrpId     | 
               TRACENFO[CtxDsdaptation].iGrpId  | 
               TRACENFO[CtxHwDevice].iGrpId     | 
               TRACENFO[CtxPolicyProxy].iGrpId  | 
               TRACENFO[CtxPolicyServer].iGrpId | 
               TRACENFO[CtxPolicyCtrl].iGrpId   | 
               TRACENFO[CtxMmRm].iGrpId         | 
               TRACENFO[CtxAudioServer].iGrpId  | 
               TRACENFO[CtxAsProxy].iGrpId      | 
               TRACENFO[CtxAsServer].iGrpId     | 
               TRACENFO[CtxAsCtrl].iGrpId       | 
               TRACENFO[CtxAsEap].iGrpId        | 
               TRACENFO[CtxMdaif].iGrpId        | 
               TRACENFO[CtxAaMm].iGrpId         | 
               TRACENFO[CtxRadio].iGrpId;
	}

#endif  // _A3F_TRACE_CTXT_DEF_H_
            
// End of File
