/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class used by client applications to convert location 
* information
*
*/

#include <e32base.h>
#include <lbs/lbslocationinfoconverter.h>
#include <lbs/lbslocationinfoconvertercommon.h>
#include "lbslocationinfoconverterimpl.h"


EXPORT_C TVersion MLbsLocationInfoConverterObserver::Version()  const
	{
	return TVersion(1, 0, 0);
	}



//----------------------------------------------------------------
// CLbsLocationInfoConverter::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C CLbsLocationInfoConverter* CLbsLocationInfoConverter::NewL(
                                     MLbsLocationInfoConverterObserver& aObserver, 
                                     const TUid aConverterModuleId )
    {
    CLbsLocationInfoConverter* self = new( ELeave ) CLbsLocationInfoConverter;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver,aConverterModuleId );
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C CLbsLocationInfoConverter* CLbsLocationInfoConverter::NewL(
                                     MLbsLocationInfoConverterObserver& aObserver )
    {
    CLbsLocationInfoConverter* self = new( ELeave ) CLbsLocationInfoConverter;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver,TUid::Uid( KNullUidValue ) );
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::~CLbsLocationInfoConverter
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C CLbsLocationInfoConverter::~CLbsLocationInfoConverter()
    {
    delete iLocationInfoConverterImpl;
    iLocationInfoConverterImpl = NULL;
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void CLbsLocationInfoConverter::ConvertLocationInfoL( 
                                CLbsLocationInfoBase& aLocationInfo,
                                const TLbsConversionPrefs aConversionPrefs,
                                const TLbsConversionOutputInfoMask aRequestedInfo )
    {
    iLocationInfoConverterImpl->ConvertLocationInfoL( aLocationInfo,aConversionPrefs,
                                                  aRequestedInfo );
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void CLbsLocationInfoConverter::ConvertLocationInfoL( 
                                RLbsLocationInfoBaseArray& aLocationInfoArray,
                                const TLbsConversionPrefs aConversionPrefs, 
                                const TLbsConversionOutputInfoMask aRequestedInfo )
    {
    iLocationInfoConverterImpl->ConvertLocationInfoL( aLocationInfoArray,
                                            aConversionPrefs,aRequestedInfo );
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::CancelConvertLocationInfo
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void CLbsLocationInfoConverter::CancelConvertLocationInfo()
    {
    iLocationInfoConverterImpl->CancelConvertLocationInfo();
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationInfoConverter::ConstructL( 
                        MLbsLocationInfoConverterObserver& aObserver, 
                        const TUid aConverterModuleId )
    {
    iLocationInfoConverterImpl = CLbsLocationInfoConverterImpl::NewL(
                                         aObserver,aConverterModuleId );
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverter::CLbsLocationInfoConverter
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationInfoConverter::CLbsLocationInfoConverter()
    {
    
    }
