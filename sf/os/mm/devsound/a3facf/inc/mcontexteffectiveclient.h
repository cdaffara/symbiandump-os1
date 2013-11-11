// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MCONTEXTEFFECTIVECLIENT_H
#define MCONTEXTEFFECTIVECLIENT_H

#include <e32base.h>
#include <a3f/a3fbase.h>

const TUid KSetClientInfoUid = {0x2002C396};
class MContextSetEffectiveClient
    {
public:
    /**
     * Add process id of effective client (usually the effective devsound client)
     * @param aActualProcessId process id of actual/effective client
     * @return An error code. KErrNone on success, otherwise one of the system wide error codes.
     */
    virtual TInt SetEffectiveClientInfo(const TProcessId& aActualProcessId) = 0;
    };

#endif // MCONTEXTEFFECTIVECLIENT_H
