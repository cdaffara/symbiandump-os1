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
#include "RBnepSetupConnectionResponseControl.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Retrieve the value from a connection setup response 
   @internalComponent
*/
void RBnepSetupConnectionResponseControl::ConnectionSetupResponseL (TBnepSetupConnectionResponseMessage& aResponse)
    {
    LOG_FUNC
    if( (First()->Ptr() + KSetupResponseFieldOffset + KSizeOfBnepResponseCode) <= (First()->EndPtr()) )
        {
        aResponse = static_cast<TBnepSetupConnectionResponseMessage>(BigEndian::Get16((First()->Ptr() + KSetupResponseFieldOffset)));
        }
    else
        {
        User::Leave(KErrOverflow);
        }
    }

/**
   Allocate the memory necessary for a response 
   @internalComponent
*/
void RBnepSetupConnectionResponseControl::InitL ()
    {
    LOG_FUNC
    AllocL(KMaxSizeOfSetupResponse);
    }

/**
   Set the value in a connection setup response 
   @internalComponent
*/
void RBnepSetupConnectionResponseControl::SetConnectionSetupResponseL (TBnepSetupConnectionResponseMessage aResponse)
    {
    LOG_FUNC
    if(KMaxSizeOfSetupResponse <= Length())
        {
        TPckgBuf<TUint8> controlType(static_cast<TUint8>(EBnepSetupConnectionResponseMessage));
        CopyIn(controlType, KControlTypeOffset);
        TBuf8<sizeof(TUint16)> response;
        response.SetMax();
        BigEndian::Put16(&response[0], static_cast<TUint16>(aResponse));
        CopyIn(response,KSetupResponseFieldOffset);
        TrimEnd(KSetupResponseFieldOffset + sizeof(TUint16));
        DUMPFRAME(_L8("RBnepSetupConnectionResponseControl"));
        }
    else
        {
        User::Leave(KErrUnderflow);
        }
    }


