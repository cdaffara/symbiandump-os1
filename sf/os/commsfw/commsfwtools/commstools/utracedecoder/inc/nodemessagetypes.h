// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NODEMESSAGETYPES_H
#define NODEMESSAGETYPES_H

struct TMessageId
    {
    TMessageId()
        {
        iRealmId = 0;
        iMessageId = 0;
        }

    unsigned short int iMessageId;
    unsigned int iRealmId;
    };


struct STypeId
    {
    STypeId()
        {
        iUid = 0;
        iType = 0;
        }

    unsigned int iUid;
    unsigned int iType;
    };
    
    
struct SMessageHeader
    {
    STypeId iTypeId;
    unsigned int iLength;
    };


struct SRuntimeContextBase
    {
    unsigned char iSize;
    unsigned char iSalt;
    unsigned short iThread;
    unsigned int iScope;
    };

#endif
// NODEMESSAGETYPES_H

