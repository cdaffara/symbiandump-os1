// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "cmmsmsextinterface.h"
#include "cmmphonetsy.h"

// ======== MEMBER FUNCTIONS ========

CMmSmsExtInterface::CMmSmsExtInterface()
    {
    }

CMmSmsExtInterface::~CMmSmsExtInterface()
    {
    }

// --------------------------------------------------------------------------- 
// CMmSmsExtInterface::CompleteReceiveMessageL 
// Default implementation of CompleteReceiveMessage method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
// 
TInt CMmSmsExtInterface::CompleteReceiveMessage(
    TSmsMsg* /*aSmsMsg*/, 
    TAny* /*aReceiveMessageParamsPtr*/,
    TDes8* /*aReceiveMessagePduPtr*/ )
    {
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsExtInterface::MessageStoreInfo 
// Default implementation of MessageStoreInfo method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsExtInterface::MessageStoreInfo(
    TAny* /*aGetMessageStoreInfoPtr*/, 
    TInt /*aTotalEntries*/, 
    TInt /*aUsedEntries*/ )
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------- 
// CMmSmsExtInterface::StoreInfo 
// Default implementation of StoreInfo method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsExtInterface::StoreInfo(
    TAny* /*aGetInfoInfoPtr*/, 
    TInt /*aTotalEntries*/, 
    TInt /*aUsedEntries*/ )
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------- 
// CMmSmsExtInterface::CompleteReadAllSmsPhase1L 
// Default implementation of CompleteReadAllSmsPhase1L method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsExtInterface::CompleteReadAllSmsPhase1L(
    CArrayPtrFlat<TSmsMsg>* /*aSmsListArray*/, 
    RMobilePhone::TClientId /*aReadAllId*/, 
    CArrayPtrFlat<CListReadAllAttempt>* /*aSmsReadAllArray*/, 
    TInt* /*aReadAllBufSizePtr*/ )
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------- 
// CMmSmsExtInterface::GetSupportedEntry 
// Default implementation of GetSupportedEntry method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsExtInterface::GetSupportedEntry()
    {
    return KErrNotSupported;
    }

//  End of File  
