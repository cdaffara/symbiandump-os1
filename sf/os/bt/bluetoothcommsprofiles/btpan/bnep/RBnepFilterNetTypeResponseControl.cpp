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
#include "BnepTypes.h"
#include "RBnepFilterNetTypeResponseControl.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Allocate the memory required for a BnepFilterNetTypeResponse control.  
   Called for locally originated controls only.
   @internalComponent
*/
void RBnepFilterNetTypeResponseControl::InitL ()
    {
    AllocL(KMaxSizeOfFilterNetTypeResponse);
    }

/**
   Set the response code for a network protocol type response
   @internalComponent
*/
void RBnepFilterNetTypeResponseControl::SetNetTypeResponseL (TFilterNetTypeResponse aResponse)
    {
    LOG_FUNC
    if(KMaxSizeOfFilterNetTypeResponse <= Length())  //,BnepUtils::Panic(Bnep::EFrameTooSmall));
        {
        TPckgBuf<TUint8> controlType(static_cast<TUint8>(EBnepFilterNetTypeResponse));
        CopyIn(controlType, KControlTypeOffset);
        TBuf8<sizeof(TUint16)> response;
        response.SetMax();
        BigEndian::Put16(&response[0], static_cast<TUint16>(aResponse));
        CopyIn(response,KFilterNetTypeResponseFieldOffset);
        TrimEnd(KFilterNetTypeResponseFieldOffset + sizeof(TUint16));
        DUMPFRAME(_L8("RBnepFilterNetTypeResponseControl"));
        }
    else
        {
        User::Leave(KErrUnderflow);
        }
    }


