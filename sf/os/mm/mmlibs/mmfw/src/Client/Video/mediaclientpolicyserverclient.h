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

#ifndef MEDIACLIENTPOLICYSERVERCLIENT_H
#define MEDIACLIENTPOLICYSERVERCLIENT_H

#include "mediaclientpolicyserversession.h"
#include <e32base.h>
#include <graphics/surface.h>

const TInt KFocusSurfaceChanged = 9; // taken from Policy Server

NONSHARABLE_CLASS(CMediaClientPolicyServerClient) : public CBase
    {
private:
    class TFocusSurfaceChangedEvent
        {
        public:
            TSurfaceId iSurfaceId;
            TBool iForeground;        
        };

public:
    static CMediaClientPolicyServerClient* NewL();
    virtual ~CMediaClientPolicyServerClient();

public:
    TInt Connect();
    void Close();
    void SetSurface(const TSurfaceId& aSurfaceId);
    void FocusChanged(TBool aForeground);  
    TBool IgnoreProcess(TSecureId aId);
    
private:
    CMediaClientPolicyServerClient();

private:
    RMediaClientPolicyServerSession iSession;
    TSurfaceId iSurfaceId;
    };

#endif // MEDIACLIENTPOLICYSERVERCLIENT_H
