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
#include "RBnepFilterMultiAddrResponseControl.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Allocate the memory required for a BnepFilterMultiAddrResponse control.
   Called for locally originated controls only.
   @internalComponent
*/
void RBnepFilterMultiAddrResponseControl::InitL ()
    {
    AllocL(KMaxSizeOfMultiAddrResponse);
    }

/**
   Set the response code for a multicast address response
   @internalComponent
*/
void RBnepFilterMultiAddrResponseControl::SetMultiAddrResponseL (TFilterMultiAddrResponse aResponse)
    {
    LOG_FUNC
    if(KMaxSizeOfMultiAddrResponse <= Length())  //,BnepUtils::Panic(Bnep::EFrameTooSmall));
        {
        TPckgBuf<TUint8> controlType(static_cast<TUint8>(EBnepFilterMultiAddrResponseMsg));
        CopyIn(controlType, KControlTypeOffset);
        TBuf8<sizeof(TUint16)> response;
        response.SetMax();
        BigEndian::Put16(&response[0], static_cast<TUint16>(aResponse));
        CopyIn(response,KMultiAddrResponseFieldOffset);
        TrimEnd(KMultiAddrResponseFieldOffset + sizeof(TUint16));
        //BigEndian::Put16((First()->Ptr() + KMultiAddrResponseFieldOffset), static_cast<TUint16>(aResponse));
        DUMPFRAME(_L8("RBnepFilterMultiAddrResponseControl"));
        }
    else
        {
        User::Leave(KErrUnderflow);
        }
    }
