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
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <es_sock.h>
#include "RBnepNotUnderstoodResponseControl.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Allocate some buffer space for this object.
   @internalComponent
*/
void RBnepNotUnderstoodResponseControl::InitL ()
    {
    LOG_FUNC
    AllocL(KMaxSizeOfCommandNotUnderstoodResponse);
    }

/**
   Write an unknown control code into the BNEP not understood control response
   @internalComponent
*/
void RBnepNotUnderstoodResponseControl::SetUnknownControlType (TUint8 aUnknownControlType)
    {
    LOG_FUNC
    TPckgBuf<TUint8> controlType(static_cast<TUint8>(EBnepControlCommandNotUnderstood));
    TPckgBuf<TUint8> unknownControlType(aUnknownControlType);
    CopyIn(controlType, KControlTypeOffset);
    CopyIn(unknownControlType, KUnknownControlTypeResponseFieldOffset);
    DUMPFRAME(_L8("RBnepNotUnderstoodResponseControl"));
    }

