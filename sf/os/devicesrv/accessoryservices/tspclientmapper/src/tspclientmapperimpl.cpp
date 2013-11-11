/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This interface is used to add mappings between the process
*                that listens the remote control and is assosiated with
*                another function.
*
*/


// INCLUDE FILES
#include <e32property.h>
#include "RemConDebug.h"
#include "tspclientmapperimpl.h"
#include "tspclientmapperprivatepskeys.h"

_LIT( KTspClientMapperSynch, "TspClientMapperSynch_10282CD6" );
// ============================ MEMBER FUNCTIONS ===============================
_LIT_SECURITY_POLICY_C1(KNoCapability, ECapability_None);

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::CRemConTspController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CTspClientMapperImpl::CTspClientMapperImpl()
	{
	COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::CTspClientMapperImpl()" );
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::CTspClientMapperImpl() - return" );
	}

// Destructor
CTspClientMapperImpl::~CTspClientMapperImpl()
	{
	COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::~CTspClientMapperImpl()" );
    CloseMutex();
    }

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::NewL
// -----------------------------------------------------------------------------
//
CTspClientMapperImpl* CTspClientMapperImpl::NewL()
    {
	COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::NewL()" );
	CTspClientMapperImpl* self = new( ELeave ) CTspClientMapperImpl();
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::NewL() - return" );
    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CTspClientMapperImpl::ConstructL
// Constructor function. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTspClientMapperImpl::ConstructL()
    {
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::ConstructL()" );
    
    OpenMutexL();
    
    TTspClientMapperSynch synchObj( iMutex );
    
    TFixedArray< TProcessId, 10 > arrayCheck;
    TPckg< TFixedArray< TProcessId, 10 > > bufCheck( arrayCheck );
    TInt err = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyPlaying,
                bufCheck );
    if( err == KErrNotFound )
        {
        COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::ConstructL() - Define the P&S keys, this should only be done in the boot" );
        RProperty::Define(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyPlaying,
                    RProperty::EByteArray,
                    KNoCapability, 
                    KNoCapability,
                    RProperty::KMaxPropertySize );
        RProperty::Define(
                        KTspClientMapperProperty,
                        KTspClientMapperKeyRegistered,
                        RProperty::EByteArray,
                        KNoCapability, 
                        KNoCapability,
                        RProperty::KMaxPropertySize );
        RProperty::Define(
                        KTspClientMapperProperty,
                        KTspClientMapperKeyStopped,
                        RProperty::EByteArray,
                        KNoCapability, 
                        KNoCapability,
                        RProperty::KMaxPropertySize );
        
        TFixedArray< TProcessId, 10 > array;
        TPckg< TFixedArray< TProcessId, 10 > > buf( array );
        const TProcessId processIdNull( 0 );
        for( TInt i = 0; i != 10; i++ )
            {
            array[ i ] = processIdNull;
            }
        err = RProperty::Set(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyPlaying,
                    buf );
        err = RProperty::Set(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyRegistered,
                    buf );
        err = RProperty::Set(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyStopped,
                    buf );
        }
    
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::ConstructL() - return" );
    }


// -----------------------------------------------------------------------------
// CTspClientMapperImpl::SetTargetClient
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTspClientMapperImpl::SetTspTargetClient(
    TTspClientMappingType aMappingType,
    TProcessId aProcessId )
	{
    COM_TRACE_2( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() aMappingType=%d, aProcessId=%d", aMappingType, (TInt)aProcessId );
    
    if( aMappingType > EStoppedClients )
        {
        COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() EPlayingClients - Enumeration not in range" );
        return;
        }
    
    TTspClientMapperSynch synchObj( iMutex );
    
    TFixedArray< TProcessId, 10 > array;
    TPckg< TFixedArray< TProcessId, 10 > > buf( array );
    TFixedArray< TProcessId, 10 > array1;
    const TProcessId processIdNull( 0 );
    switch( aMappingType )
        {
        case EPlayingClients:
            {
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() EPlayingClients " );
            
            TInt err = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyPlaying,
                buf );
            COM_TRACE_1( "Error in get play is %d ", err );
            if( err == KErrNotFound )
                {
                // Hasn't been set yet so define the key
                TInt err1 = RProperty::Define(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyPlaying,
                    RProperty::EByteArray,
                    KNoCapability, 
                    KNoCapability,
                    RProperty::KMaxPropertySize  );
                COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() Error in play define is %d ", err1 );
                }
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() Before append " );
            
            array1.Reset();
            array1[ 0 ] = aProcessId;
            if( err != KErrNotFound )
                {
                for( TInt i = 0; i != buf().Count() - 1; i++ )
                    {
                    if( ( aProcessId != buf().operator[]( i ) )
                     || ( aProcessId == processIdNull ) )
                        {
                        array1[ i + 1 ] = buf().operator[]( i );
                        }
                    else
                        {
                        buf().operator[]( i ) = processIdNull;
                        i--;
                        }
                    }
                }
            TPckg< TFixedArray< TProcessId, 10 > > buf1( array1 );
            COM_TRACE_1( "Appended process id %d ", (TInt)buf1().operator[]( 0 ) );
            err = RProperty::Set(
                KTspClientMapperProperty,
                KTspClientMapperKeyPlaying,
                buf1 );
            if( err != KErrNone )
                {
                COM_TRACE_1( "Error in play set is %d ", err );
                }
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() EPlayingClients Set " );
            break;
            }
        case ERegisteredClients:
            {
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() ERegisteredClients " );
            
            TInt err = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyRegistered,
                buf );
            if( err == KErrNotFound )
                {
                // Hasn't been set yet so define the key
                err = RProperty::Define(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyRegistered,
                    RProperty::EByteArray,
                    KNoCapability, 
                    KNoCapability,
                    RProperty::KMaxPropertySize  );
                COM_TRACE_1( "Error in register define is %d ", err );
                }
            array1.Reset();
            array1[ 0 ] = aProcessId;
            if( err != KErrNotFound )
                {
                for( TInt i = 0; i != buf().Count() - 1; i++ )
                    {
                    if( ( aProcessId != buf().operator[]( i ) )
                     || ( aProcessId == processIdNull ) )
                        {
                        array1[ i + 1 ] = buf().operator[]( i );
                        }
                    else
                        {
                        buf().operator[]( i ) = processIdNull;
                        i--;
                        }
                    }
                }
            TPckg< TFixedArray< TProcessId, 10 > > buf1( array1 );
            RProperty::Set(
                KTspClientMapperProperty,
                KTspClientMapperKeyRegistered,
                buf1 );
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() ERegisteredClients Set " );
            break;
            }
        case EStoppedClients:
            {
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() EStoppedClients " );
            TInt err = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyStopped,
                buf );
            if( err == KErrNotFound )
                {
                // Hasn't been set yet so define the key
                err = RProperty::Define(
                    KTspClientMapperProperty,
                    KTspClientMapperKeyStopped,
                    RProperty::EByteArray,
                    KNoCapability, 
                    KNoCapability,
                    RProperty::KMaxPropertySize  );
                COM_TRACE_1( "Error in stop define is %d ", err );
                }
            array1.Reset();
            array1[ 0 ] = aProcessId;
            if( err != KErrNotFound )
                {
                for( TInt i = 0; i != buf().Count() - 1; i++ )
                    {
                    if( ( aProcessId != buf().operator[]( i ) )
                     || ( aProcessId == processIdNull ) )
                        {
                        array1[ i + 1 ] = buf().operator[]( i );
                        }
                    else
                        {
                        buf().operator[]( i ) = processIdNull;
                        i--;
                        }
                    }
                }
            TPckg< TFixedArray< TProcessId, 10 > > buf1( array1 );
            RProperty::Set(
                KTspClientMapperProperty,
                KTspClientMapperKeyStopped,
                buf1 );
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() EStoppedClients Set " );
            break;
            }
        default:
            {
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() Switch case - Default " );
            }
        }
    
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClient() - return" );
	}
	
// -----------------------------------------------------------------------------
// CTspClientMapperImpl::SetTspTargetClientToOtherType
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTspClientMapperImpl::SetTspTargetClientToOtherType( 
    TTspClientMappingType aMappingType,
    TProcessId aProcessId )
	{
    COM_TRACE_2( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClientToOtherType() aMappingType=%d, aProcessId=%d", aMappingType, (TInt)aProcessId );
    TInt returnErr( KErrNone );
    if( aMappingType > EStoppedClients )
        {
        returnErr = KErrNotSupported;
        return returnErr;
        }
    
    TTspClientMapperSynch synchObj( iMutex );
    
    TInt err = RemoveTspTargetClient( EPlayingClients, aProcessId );
    if( err == KErrNotFound )
        {
        err = RemoveTspTargetClient( ERegisteredClients, aProcessId );
        if( err == KErrNotFound )
            {
            err = RemoveTspTargetClient( EStoppedClients, aProcessId );
            if( err == KErrNotFound )
                {
                return err;
                }
            }
        }
    
    SetTspTargetClient( aMappingType, aProcessId );
    
	COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::SetTspTargetClientToOtherType() - return" );
	return returnErr;
	}	

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::RemoveTspTargetClientL
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTspClientMapperImpl::RemoveTspTargetClient(
    TTspClientMappingType aMappingType,
    TProcessId aProcessId )
	{
	const TProcessId processIdNull( 0 );
	TInt returnErr( KErrNone ); 
	COM_TRACE_2( "[TspClientMapperImpl] CTspClientMapperImpl::RemoveTspTargetClientL() aMappingType=%d, aProcessId=%d", aMappingType, (TInt)aProcessId );
    if( aMappingType > EStoppedClients )
        {
        returnErr = KErrNotSupported;
        }
    TFixedArray< TProcessId, 10 > array;
    array.Reset();
    TPckg< TFixedArray< TProcessId, 10 > > buf( array );
    TInt position( KErrNotFound );
    
    TTspClientMapperSynch synchObj( iMutex );
    
    switch( aMappingType )
        {
        case EPlayingClients:
            {
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::RemoveTspTargetClientL() EPlayingClients " );
            
            returnErr = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyPlaying,
                buf );
            if( returnErr != KErrNotFound )
                {
                for( TInt aaa = 0; aaa != buf().Count(); aaa++ )
                    {
                    if( aProcessId == buf().operator[]( aaa ) )
                        {
                        position = aaa;
                        buf().operator[]( aaa ) = processIdNull;
                        }
                    }
                if( position != KErrNotFound )
                    {
                    returnErr = RProperty::Set(
                        KTspClientMapperProperty,
                        KTspClientMapperKeyPlaying,
                        buf );
                    }
                else
                    {
                    returnErr = KErrNotFound;
                    }
                }
            break;
            }
        case ERegisteredClients:
            {
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::RemoveTspTargetClientL() ERegisteredClients " );
            
            returnErr = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyRegistered,
                buf );
            if( returnErr != KErrNotFound )
                {
                for( TInt aaa = 0; aaa != buf().Count(); aaa++ )
                    {
                    if( aProcessId == buf().operator[]( aaa ) )
                        {
                        position = aaa;
                        buf().operator[]( aaa ) = processIdNull;
                        }
                    }
                if( position != KErrNotFound )
                    {
                    returnErr = RProperty::Set(
                        KTspClientMapperProperty,
                        KTspClientMapperKeyRegistered,
                        buf );
                    }
                else
                    {
                    returnErr = KErrNotFound;
                    }
                }
            break;
            }
        case EStoppedClients:
            {
            
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::RemoveTspTargetClientL() EStoppedClients " );
            returnErr = RProperty::Get(
                KTspClientMapperProperty,
                KTspClientMapperKeyStopped,
                buf );
            if( returnErr != KErrNotFound )
                {
                for( TInt aaa = 0; aaa != buf().Count(); aaa++ )
                    {
                    if( aProcessId == buf().operator[]( aaa ) )
                        {
                        position = aaa;
                        buf().operator[]( aaa ) = processIdNull;
                        }
                    }
                if( position != KErrNotFound )
                    {
                    returnErr = RProperty::Set(
                        KTspClientMapperProperty,
                        KTspClientMapperKeyStopped,
                        buf );
                    }
                else
                    {
                    returnErr = KErrNotFound;
                    }
                }
            break;
            }
        default:
            {
            
            COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::RemoveTspTargetClientL() Default - LEAVE " );
            returnErr = KErrGeneral;
            }
        }
	COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::RemoveTspTargetClientL() - return" );
	return returnErr;
	}
	
// -----------------------------------------------------------------------------
// CTspClientMapperImpl::GetTspTargetClientsL
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTspClientMapperImpl::GetTspTargetClients( 
    TTspClientMappingType aMappingType,
    RArray< TProcessId >& aPidArray )
	{
	TInt returnErr( KErrNone );
	if( aMappingType > EStoppedClients )
        {
        returnErr = KErrNotSupported;
        }
    TFixedArray< TProcessId, 10 > array;
    TPckg< TFixedArray< TProcessId, 10 > > buf( array );
    COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() aMappingType = %d", aMappingType );
	TInt count( 0 );
    
	TTspClientMapperSynch synchObj( iMutex );
	
    if( aMappingType == EPlayingClients )
        {
        COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - Getting playing clients" );
        returnErr = RProperty::Get(
            KTspClientMapperProperty,
            KTspClientMapperKeyPlaying,
            buf );
        COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() playing clients count = %d", buf().Count() );
        count = buf().Count();
        }
   if( aMappingType == ERegisteredClients )
        {
        COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - Getting registered clients" );
        returnErr = RProperty::Get(
            KTspClientMapperProperty,
            KTspClientMapperKeyRegistered,
            buf );
        COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() registered clients count = %d", buf().Count() );
        count = buf().Count();
        }
    if( aMappingType == EStoppedClients )
        {
        COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - Getting stopped clients" );
        returnErr = RProperty::Get(
            KTspClientMapperProperty,
            KTspClientMapperKeyStopped,
            buf );
        COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() stopped clients count = %d", buf().Count() );
        count = buf().Count();
        }
        
    for ( TInt i = 0; i < count; i++ )
        {
        if ( (TInt)buf().operator[]( i ) != 0 ) // count of registered clients is lost with TFixedArray, so rely on id being different from 0
            {
            COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() first client's Pid = %d", (TInt)buf().operator[]( i ) );
            returnErr = aPidArray.Append( buf().operator[]( i ) );
			if ( returnErr != KErrNone )
				{
				break;
				}
			}
        }
	COM_TRACE_1 ( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - returnErr: %d", returnErr );
	return returnErr;
	
	}

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::OpenMutexL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTspClientMapperImpl::OpenMutexL()
    {
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::InitilizeMutexL()" );
    
    TInt err( iMutex.CreateGlobal( KTspClientMapperSynch ) );
    
    if ( err != KErrNone )
        {
        if ( err == KErrAlreadyExists )
            {
            err = iMutex.OpenGlobal( KTspClientMapperSynch );
            
            if ( err != KErrNone )
                {
                COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::InitilizeMutexL(): OpenGlobal err=%d", err );
                User::Leave( err );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::CloseMutex
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTspClientMapperImpl::CloseMutex()
    {
    COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::CloseMutexL()" );
    
    if ( iMutex.IsHeld() )
            {
            iMutex.Signal();
            }
    iMutex.Close();
    }

// -----------------------------------------------------------------------------
// TTspClientMapperSynch::TTspClientMapperSynch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTspClientMapperImpl::TTspClientMapperSynch::TTspClientMapperSynch( RMutex& aMutex ) : iMutex( aMutex )
    {
    COM_TRACE_( "[TspClientMapperImpl] TTspClientMapperSynch::TTspClientMapperSynch()" );
    iMutex.Wait();
    }
// -----------------------------------------------------------------------------
// TTspClientMapperSynch::TTspClientMapperSynch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTspClientMapperImpl::TTspClientMapperSynch::~TTspClientMapperSynch()
    {
    COM_TRACE_( "[TspClientMapperImpl] TTspClientMapperSynch::~TTspClientMapperSynch()" );
    iMutex.Signal();
    }

// End of file
