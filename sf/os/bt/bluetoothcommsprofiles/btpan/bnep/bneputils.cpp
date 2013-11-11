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

#include "bneputils.h"

_LIT(KBnepPanic, "BnepDrv");
const TUint8 KBnepMultiCastFlag = 0x01;

/**
   Test the BT address to check if it is a multicast address.
*/
TBool BnepUtils::IsMulticast (const TBTDevAddr& aAddr)
    {
    return (aAddr[0] & KBnepMultiCastFlag);
    }
    
/**
   Test the BT address to check if it is a multicast address.
*/
void BnepUtils::Panic (Bnep::TBnepPanic aReason)
    {
    User::Panic(KBnepPanic, aReason); 
    }

