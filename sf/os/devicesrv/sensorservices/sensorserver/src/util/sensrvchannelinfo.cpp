/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The channel basic information
*
*/


#include <sensrvchannelinfo.h>
#include "sensrvtrace.h"


// Tracing macro for channel info contents
#ifdef API_TRACE_DEBUG    
#define _TRACE_CHANNEL_INFO_ \
TBuf<KSensrvLocationLength> location;\
location.Copy(iLocation);\
TBuf<KSensrvVendorIdLength> vendor;\
vendor.Copy(iVendorId);\
API_TRACE(( _L("Sensor Util - TSensrvChannelInfo::TSensrvChannelInfo - iChannelId=%d, iContextType=%d, iQuantity=%d, iChannelType=%d, iLocation=%S, iVendorId=%S, iDataItemSize=%d, iChannelDataTypeId=%d, iReserved=%d"),iChannelId,iContextType,iQuantity,iChannelType,&location,&vendor,iDataItemSize,iChannelDataTypeId,iReserved)); 
#else
#define _TRACE_CHANNEL_INFO_
#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TSensrvChannelInfo::TSensrvChannelInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvChannelInfo::TSensrvChannelInfo() :
    iChannelId( 0 ),
    iContextType( 0 ),
    iQuantity( 0 ), 
    iChannelType( 0 ),
    iDataItemSize( 0 ),
    iReserved3( 0 ),
    iChannelDataTypeId( 0 ),
    iReserved( 0 ),
    iReserved2( 0 )
    {
    _TRACE_CHANNEL_INFO_
    }

// ---------------------------------------------------------------------------
// TSensrvChannelInfo::TSensrvChannelInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvChannelInfo::TSensrvChannelInfo( TInt aContextType, 
                                                 TInt aQuantity, 
                                                 TSensrvChannelTypeId aChannelType,
                                                 const TDesC8& aLocation,
                                                 const TDesC8& aVendorId,
                                                 TSensrvChannelDataTypeId aChannelDataTypeId ) 
    : iChannelId( 0 ), 
      iContextType( aContextType ), 
      iQuantity( aQuantity ),
      iChannelType( aChannelType ), 
      iLocation( aLocation ),
      iVendorId( aVendorId ),
      iDataItemSize( 0 ),
      iReserved3( 0 ),
      iChannelDataTypeId( aChannelDataTypeId ),
      iReserved( 0 ),
      iReserved2( 0 )
    {
    _TRACE_CHANNEL_INFO_
    } 
    
// ---------------------------------------------------------------------------
// TSensrvChannelInfo::TSensrvChannelInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvChannelInfo::TSensrvChannelInfo( TSensrvChannelId aChannelId, 
                                                 TInt aContextType, 
                                                 TInt aQuantity, 
                                                 TSensrvChannelTypeId aChannelType,
                                                 const TDesC8& aLocation,
                                                 const TDesC8& aVendorId, 
                                                 TInt aDataItemSize,
                                                 TSensrvChannelDataTypeId aChannelDataTypeId ) 
    : iChannelId( aChannelId ), 
      iContextType( aContextType ), 
      iQuantity( aQuantity ),
      iChannelType( aChannelType ), 
      iLocation( aLocation ),
      iVendorId( aVendorId ),
      iDataItemSize( aDataItemSize ),
      iReserved3( 0 ),
      iChannelDataTypeId( aChannelDataTypeId ),
      iReserved( 0 ),
      iReserved2( 0 )
    {
    _TRACE_CHANNEL_INFO_
    } 

// ---------------------------------------------------------------------------
// TSensrvChannelInfo::TSensrvChannelInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvChannelInfo::TSensrvChannelInfo( const TSensrvChannelInfo& aChannelInfo ) 
    : iChannelId( aChannelInfo.iChannelId ), 
      iContextType( aChannelInfo.iContextType ), 
      iQuantity( aChannelInfo.iQuantity ), 
      iChannelType( aChannelInfo.iChannelType ), 
      iLocation( aChannelInfo.iLocation ), 
      iVendorId( aChannelInfo.iVendorId ),
      iDataItemSize( aChannelInfo.iDataItemSize ),
      iReserved3( 0 ),
      iChannelDataTypeId( aChannelInfo.iChannelDataTypeId ),
      iReserved( 0 ),
      iReserved2( 0 )
    {
    _TRACE_CHANNEL_INFO_
    } 

// ---------------------------------------------------------------------------
// TSensrvChannelInfo::IsMatch()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TSensrvChannelInfo::IsMatch( const TSensrvChannelInfo &aInfo ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvChannelInfo::IsMatch()" ) ) );
                
    if ( aInfo.iContextType           && aInfo.iContextType != iContextType ) 
        { return EFalse; }
    if ( aInfo.iQuantity              && aInfo.iQuantity    != iQuantity )    
        { return EFalse; }
    if ( aInfo.iChannelType           && aInfo.iChannelType != iChannelType ) 
        { return EFalse; }
    if ( aInfo.iLocation.Length() > 0 && aInfo.iLocation    != iLocation )    
        { return EFalse; }
    if ( aInfo.iVendorId.Length() > 0 && aInfo.iVendorId    != iVendorId )    
        { return EFalse; }
    if ( aInfo.iChannelDataTypeId     && aInfo.iChannelDataTypeId != iChannelDataTypeId )  
        { return EFalse; } 
        
    API_TRACE( ( _L( "Sensor Util - TSensrvChannelInfo::IsMatch - return ETrue" ) ) );    
    return ETrue;
    }
   
// ---------------------------------------------------------------------------
// TSensrvChannelInfo::IsComplete()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TSensrvChannelInfo::IsComplete() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvChannelInfo::IsComplete()" ) ) );
    
    if ( !iContextType )       { return EFalse; }
    if ( !iQuantity )          { return EFalse; }
    if ( !iChannelType )       { return EFalse; }
    if ( !iLocation.Length() ) { return EFalse; }
    if ( !iVendorId.Length() ) { return EFalse; }
    if ( !iChannelDataTypeId ) { return EFalse; }
    
    API_TRACE( ( _L( "Sensor Util - TSensrvChannelInfo::IsComplete return ETrue" ) ) );
    return ETrue;
    }
    
// End of file
