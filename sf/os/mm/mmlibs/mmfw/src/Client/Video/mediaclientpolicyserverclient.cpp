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

#include "mediaclientpolicyserverclient.h"
#include <e32std.h>
#include "mediaclientvideotrace.h"

CMediaClientPolicyServerClient* CMediaClientPolicyServerClient::NewL()
    {
    DEBUG_PRINTF("CMediaClientPolicyServerClient::NewL +++");
    
    CMediaClientPolicyServerClient* self = new( ELeave ) CMediaClientPolicyServerClient();
    
    DEBUG_PRINTF("CMediaClientPolicyServerClient::NewL ---");
    return self;
    }

CMediaClientPolicyServerClient::~CMediaClientPolicyServerClient()
    {
    DEBUG_PRINTF("CMediaClientPolicyServerClient::~CMediaClientPolicyServerClient +++");
    iSession.Close();
    DEBUG_PRINTF("CMediaClientPolicyServerClient::~CMediaClientPolicyServerClient ---");
    }

void CMediaClientPolicyServerClient::SetSurface(const TSurfaceId& aSurfaceId)
    {
    DEBUG_PRINTF("CMediaClientPolicyServerClient::SetSurface +++");
            
    if( aSurfaceId.IsNull() )
        {
        DEBUG_PRINTF("CMediaClientPolicyServerClient::SetSurface NULL SurfaceId received");
        }
    else
        {
        iSurfaceId = aSurfaceId;
        DEBUG_PRINTF5("CMediaClientPolicyServerClient::SetSurface - iSurfaceId %08x:%08x:%08x:%08x", iSurfaceId.iInternal[3], iSurfaceId.iInternal[2], iSurfaceId.iInternal[1], iSurfaceId.iInternal[0]);
        }
    
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::SetSurface ---");
    }

void CMediaClientPolicyServerClient::FocusChanged(TBool aForeground)    
    {
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::FocusChanged +++");

    DEBUG_PRINTF5("CMediaClientPolicyServerClient::FocusChanged - iSurfaceId %08x:%08x:%08x:%08x", iSurfaceId.iInternal[3], iSurfaceId.iInternal[2], iSurfaceId.iInternal[1], iSurfaceId.iInternal[0]);
    DEBUG_PRINTF2("CMediaClientPolicyServerClient::FocusChanged - Focus %d", aForeground);  

    if( iSurfaceId.IsNull() )
        {
        DEBUG_PRINTF(" CMediaClientPolicyServerClient::FocusChanged SurfaceId is NULL ... ignore focus change event");
        }
    else
        {
        DEBUG_PRINTF(" CMediaClientPolicyServerClient::FocusChanged Send focus change event to PS");
        TFocusSurfaceChangedEvent focusChangedMessage;
        focusChangedMessage.iSurfaceId = iSurfaceId;
        focusChangedMessage.iForeground = aForeground;
        TPckgBuf<TFocusSurfaceChangedEvent> buffer(focusChangedMessage);
        TIpcArgs messageArguments(&buffer);
        
        TInt error = iSession.SendMessage(KFocusSurfaceChanged, messageArguments);
    
        if(error)
            {
            DEBUG_PRINTF2(" CMediaClientPolicyServerClient::FocusChanged SendMessage failed, error = %d", error);
            }
        }    
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::FocusChanged ---");
    }

TBool CMediaClientPolicyServerClient::IgnoreProcess(TSecureId aId)
    {
    DEBUG_PRINTF("CMediaClientPolicyServerClient::IgnoreProcess +++");
    
    TBool ignore = (
            (aId == 0x10281EF2) ||  // aknnfysrv
            (aId == 0x10207218) ||  // akncapserver
            (aId == 0x10003a4a) ||  // eiksrvs
			(aId == 0x20022FC5)     // Hbdevicedialogappserver
            );
    
    DEBUG_PRINTF2("CMediaClientPolicyServerClient::IgnoreProcess --- return %d", ignore);
    return ignore;
    }

TInt CMediaClientPolicyServerClient::Connect()
    {
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::Connect +++");
    TInt error = iSession.Connect();
    DEBUG_PRINTF2(" CMediaClientPolicyServerClient::Connect --- return %d", error);   
    return error;
    }

void CMediaClientPolicyServerClient::Close()
    {
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::Close +++");
    iSession.Close();
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::Close ---");   
    }

CMediaClientPolicyServerClient::CMediaClientPolicyServerClient()
    {
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::CMediaClientPolicyServerClient +++");
    iSurfaceId = TSurfaceId::CreateNullId();
    DEBUG_PRINTF(" CMediaClientPolicyServerClient::CMediaClientPolicyServerClient ---");
    }
