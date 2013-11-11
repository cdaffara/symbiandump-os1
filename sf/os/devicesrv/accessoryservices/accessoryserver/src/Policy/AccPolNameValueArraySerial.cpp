/*
 * Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Private, Accessory Policy Name Value Array Serialization
 *
 */

// INCLUDE FILES
#include    <s32mem.h>
#include    <AccPolCommonNameValuePairs.h>
#include    <AccPolNameValueRecord.h>

#include    <accpolobjectcon.h>
#include    "AccClientServerMessages.h"

#include    "AccPolNameValueArraySerial.h"
#include    "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// TAccPolNameValueArraySerial::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolNameValueArraySerial::ExternalizeL( const RArray<
    TAccPolNameValueRecord>& aNameValueArray, RWriteStream& aStream )
    {
    COM_TRACE_( "[AccFW:ACCPOLICY] TAccPolNameValueArraySerial::ExternalizeL()" );

    TInt16 count( aNameValueArray.Count() );
    aStream.WriteInt16L( count );

    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    for( TInt16 i( 0 ); i < count; ++i )
        {
        TUint32 name;
        aNameValueArray[i].NameRecord().GetName( name );

        //write name to stream
        aStream.WriteUint16L( name );

        //set location bit
        TUint8 properties( 0 );
        if( aNameValueArray[i].ValueRecord().Location() == EAPVAccessory )
            {
            properties = 1;
            }

        //set type bits
        properties |= ( aNameValueArray[i].ValueRecord().Type() << 1 );

        TUint8 size( 0 );
        TInt value( 0 );
        TUint8 tempSize( 0 );

        //code size (bytes) of the value to propeties field (last 2 bits, size 1-4)
        if( aNameValueArray[i].ValueRecord().Type() != EAPVBool &&
            aNameValueArray[i].ValueRecord().Type() != EAPVNone )
            {
            value = aNameValueArray[i].ValueRecord().Value();

            if( aNameValueArray[i].ValueRecord().Type() == EAPVDes8 )
                {
                value = aNameValueArray[i].ValueRecord().ValueRef().Length();
                }

            if( aNameValueArray[i].ValueRecord().Type() == EAPVObject )
                {
                if( aNameValueArray[i].ValueRecord().Location() == EAPVPolicy
                    && !reinterpret_cast<CAccPolObjectCon*> (
                        aNameValueArray[i].ValueRecord().Value() ) )
                    {
                    User::Leave( KErrArgument );
                    }
                if( aNameValueArray[i].ValueRecord().Location() == EAPVPolicy
                    && reinterpret_cast<CAccPolObjectCon*> (
                        aNameValueArray[i].ValueRecord().Value() ) )
                    {
                    RBufWriteStream wStrm;
                    wStrm.Open( *buf );
                    wStrm.PushL();
                    reinterpret_cast<CAccPolObjectCon*> (
                        aNameValueArray[i].ValueRecord().Value() )->ExternalizeL( wStrm );
                    wStrm.CommitL();
                    MStreamBuf* snk = wStrm.Sink();
                    TStreamPos position = snk->TellL( MStreamBuf::EWrite );
                    value = position.Offset();
                    buf->Compress();
                    wStrm.Close();
                    wStrm.Pop();
                    buf->ResizeL( value );
                    }
                else
                    {
                    //location must be accessory
                    value = size;
                    }
                }
            TUint32 mask = 0xFF000000;
            size = 4;
            for( TUint8 sizeindex( 0 ); sizeindex < 3; sizeindex++ )
                {
                if( mask & value )
                    {
                    break;
                    }
                size--;
                mask = mask >> 8;
                }

            tempSize = size;
            size--;
            properties |= size << 6;
            }
        else
            {
            if( aNameValueArray[i].ValueRecord().Type() == EAPVBool )
                {
                if( aNameValueArray[i].ValueRecord().Value() == KErrUnknown )
                    {
                    //not defined bit on...
                    size = 0x02;
                    }
                else
                    {
                    //set boolean bit...
                    size = ( 1 & aNameValueArray[i].ValueRecord().Value() );
                    }
                }
            //write size to properties and properties to stream
            tempSize = size;
            properties |= size << 6;
            }

        aStream.WriteUint8L( properties );

        switch( aNameValueArray[i].ValueRecord().Type() )
            {
            case EAPVDes8:
            case EAPVUInt:
            case EAPVInt:
            case EAPVObject:
                {
                TInt32 writemask = 0xFF;
                for( TUint8 index( 0 ); index < tempSize; index++ )
                    {
                    aStream.WriteUint8L( ( TUint8 )( ( writemask & value )
                        >> ( index * 8 ) ) );
                    }

                if( aNameValueArray[i].ValueRecord().Type() == EAPVDes8 )
                    {
                    const TDesC8
                        & des( aNameValueArray[i].ValueRecord().ValueRef() );

                    for( TInt i( 0 ); i < value; i++ )
                        {
                        aStream.WriteUint8L( des[i] );
                        }
                    }

                if( aNameValueArray[i].ValueRecord().Type() == EAPVObject )
                    {
                    TPtr8 ptr( buf->Ptr( 0 ) );
                    const TDesC8& des( ptr );
                    for( TInt i( 0 ); i < value; i++ )
                        {
                        aStream.WriteUint8L( des[i] );
                        }
                    buf->Reset();
                    }
                }
                break;
            default:
                break;
            }
        }

    CleanupStack::PopAndDestroy( buf );

    COM_TRACE_( "[AccFW:ACCPOLICY] TAccPolNameValueArraySerial::ExternalizeL - return void" );
    }

//  End of File
