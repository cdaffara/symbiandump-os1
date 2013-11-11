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

#ifndef MEDIACLIENTPOLICYSERVERSESSION_H
#define MEDIACLIENTPOLICYSERVERSESSION_H

#include <e32std.h>

/**
 @file
 @internalComponent
 
*/
class RMediaClientPolicyServerSession : public RSessionBase
    {
public:

    RMediaClientPolicyServerSession();
    
    TInt Connect(); 
    TInt SendMessage( TInt aFunction, const TIpcArgs &aArgs ) const;
    };

#endif // MEDIACLIENTPOLICYSERVERSESSION_H

