// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcustomstubextTraces.h"
#endif

#include <ctsy/pluginapi/mmmessagemanagercallback.h>
//#include "Telephony.h"
#include "cmmcustomstubext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagerouter.h"

// ======== MEMBER FUNCTIONS ========

CMmCustomStubExt::CMmCustomStubExt()
    {
    }

CMmCustomStubExt::~CMmCustomStubExt() 
    {  
    }
    
// ---------------------------------------------------------------------------
// CMmCustomStubExt::DoExtFuncL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::DoExtFuncL( 
    const TTsyReqHandle /*aTsyReqHandle*/, 
    const TInt /*aIpc*/, 
    const TDataPackage& /*aPackage*/ ) 
    { 
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmCustomStubExt::SupportingIPC
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CMmCustomStubExt::SupportingIPC( 
    TInt /*aIpc*/ ) 
    {
    return EFalse; 
    } 

// ---------------------------------------------------------------------------
// CMmCustomStubExt::InitializeL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CMmCustomStubExt::InitializeL() 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSTUBEXT_INITIALIZEL_1,  "TSY: CMmCustomStubExt::InitializeL. " );
    }

// ---------------------------------------------------------------------------
// CMmCustomStubExt::SetMessageRouter
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CMmCustomStubExt::SetMessageRouter( 
    CMmMessageRouter* aMessageRouter )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSTUBEXT_SETMESSAGEROUTER_1,  "TSY: CMmCustomStubExt::SetMessageRouter. " );
    iMessageRouter = aMessageRouter;
    }

// ---------------------------------------------------------------------------
// Notification cannot be registered
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::RegisterNotification( 
    const TInt /*aIpc*/ ) 
    { 
    return KErrNotSupported; 
    }
    
// ---------------------------------------------------------------------------
// Because notification cannot be registered, they cannot unregister either
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::DeregisterNotification( 
    const TInt /*aIpc*/ )
    { 
    return KErrNotSupported; 
    }

// ---------------------------------------------------------------------------
// Dummy implementation for ReqModeL 
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmCustomStubExt::ReqModeL( 
    const TInt /*aIpc*/ ) 
    { 
    return 0; 
    }
    
// ---------------------------------------------------------------------------
// Returns number of slots
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::NumberOfSlotsL( 
    const TInt /*aIpc*/ )
    { 
    return 2; 
    }    
    
// ---------------------------------------------------------------------------
// Cancel services  
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//    
TInt CMmCustomStubExt::CancelService( 
    const TInt /*aIpc*/, 
    const TTsyReqHandle /*aTsyReqHandle*/ )
    { 
    return KErrGeneral; 
    }
    
// ---------------------------------------------------------------------------
// Completes request  
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//     
TInt CMmCustomStubExt::Complete( TInt /*aReqHandleType*/, 
    TInt /*aError*/, 
    TInt /*aIPC*/ )
    {
    return KErrNotSupported; 
    }    
            
// ---------------------------------------------------------------------------
// Return required platform security caps, EAlwaysPass 
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//              
TSecurityPolicy CMmCustomStubExt::GetRequiredPlatSecCaps( 
    const TInt /*aIpc*/ )
    { 
    return TSecurityPolicy(TSecurityPolicy::EAlwaysPass); 
    }
