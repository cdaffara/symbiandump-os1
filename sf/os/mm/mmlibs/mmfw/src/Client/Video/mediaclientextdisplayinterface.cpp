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

#include <mmf/plugin/mmfmediaclientextdisplayinterface.h>
#include <mmf/plugin/mmfmediaclientextdisplayinterface.hrh>
#include <e32cmn.h> 
#include <ecom/ecom.h>

void CleanupEComArray(TAny* aArray);

CExtDisplayConnectionProviderInterface* CExtDisplayConnectionProviderInterface::NewL()
    {
    RImplInfoPtrArray implInfoArray;
    TCleanupItem cleanup(CleanupEComArray, &implInfoArray);
    CleanupStack::PushL(cleanup);

    REComSession::ListImplementationsL(TUid::Uid(KMediaDisplayExtProviderInterfaceUid), implInfoArray);
    
    if(implInfoArray.Count() == 0)
        {
        CleanupStack::PopAndDestroy(); // implInfoArray
        return NULL;
        }
    
    CExtDisplayConnectionProviderInterface* self = 
        REINTERPRET_CAST(CExtDisplayConnectionProviderInterface*, REComSession::CreateImplementationL(
                implInfoArray[0]->ImplementationUid(), _FOFF(CExtDisplayConnectionProviderInterface, iInstanceKey)));
    
    CleanupStack::PopAndDestroy(); // implInfoArray
    return self;
    }

EXPORT_C CExtDisplayConnectionProviderInterface::~CExtDisplayConnectionProviderInterface()
    {
    REComSession::DestroyedImplementation(iInstanceKey);
    }

EXPORT_C CExtDisplayConnectionProviderInterface::CExtDisplayConnectionProviderInterface()
    {
    }

// CleanupEComArray function is used for cleanup support of locally declared arrays
void CleanupEComArray(TAny* aArray)
    {
    (static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
    (static_cast<RImplInfoPtrArray*> (aArray))->Close();
    }
