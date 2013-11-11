/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CHWRMHapticsRespData class implementation 
*
*/


#include <s32strm.h>             // RReadStream,RWriteStream
#include <hwrmhapticsrespdata.h>

const TInt32 KHWRMHapticsRespProtocolVersion = 1;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewLC() - Symbian constructor, leaves pointer to CleanupStack
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsRespData* CHWRMHapticsRespData::NewLC( TInt aErrorCode,
                                                            const TDesC8& aData )
    {
    CHWRMHapticsRespData* self = new ( ELeave ) CHWRMHapticsRespData();
    CleanupStack::PushL( self );
    self->ConstructL( aErrorCode, aData );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsRespData::~CHWRMHapticsRespData()
    {
    delete iData;
    }

// ---------------------------------------------------------------------------
// ExternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CHWRMHapticsRespData::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( KHWRMHapticsRespProtocolVersion );
    aStream.WriteInt32L( iErrorCode );
    aStream.WriteInt32L( iData->Length() );
    aStream << *iData;
    }

// ---------------------------------------------------------------------------
// InternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CHWRMHapticsRespData::InternalizeL( RReadStream& aStream )
    {
    TInt32 intToken; // 32 bit buffer

    aStream >> intToken; // read protocol version
    
    if( intToken != KHWRMHapticsRespProtocolVersion )
        {
        User::Leave( KErrNotSupported );
        }
        
    // Delete the current values
    delete iData;
    iData = NULL;
    
    aStream >> intToken; // read errorCode
    iErrorCode = intToken;
    
    aStream >> intToken; // read iData length

    iData = HBufC8::NewL( aStream, intToken );
    }

// ---------------------------------------------------------------------------
// ErrorCode()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHWRMHapticsRespData::ErrorCode()
    {
    return iErrorCode;
    }

// ---------------------------------------------------------------------------
// Data()
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHWRMHapticsRespData::Data()
    {
    return *iData;
    }

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsRespData::CHWRMHapticsRespData()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CHWRMHapticsRespData::ConstructL( TInt aErrorCode, const TDesC8& aData )
    {
    iErrorCode = aErrorCode;

    iData = aData.AllocL();
    }


