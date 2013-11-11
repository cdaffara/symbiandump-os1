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
* Description: Implementation of CLbsLocationInfoConverterImpl class.
*
*/

#include "lbslocationinfoconverterimpl.h"
#include <lbs/lbslocationinfoconverter.h>


//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationInfoConverterImpl* CLbsLocationInfoConverterImpl::NewL(
                                     MLbsLocationInfoConverterObserver& aObserver, 
                                     const TUid aConverterModuleId )
    {
    CLbsLocationInfoConverterImpl* self = new( ELeave ) CLbsLocationInfoConverterImpl(
                                                                  aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aConverterModuleId );
    CleanupStack::Pop( self );
    return self;
    }


//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::~CLbsLocationInfoConverterImpl
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationInfoConverterImpl::~CLbsLocationInfoConverterImpl()
    {
    iLocInfoConverter.Close();
    iLocMoniterSession.Close();
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::CLbsLocationInfoConverterImpl
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationInfoConverterImpl::CLbsLocationInfoConverterImpl( 
                                  MLbsLocationInfoConverterObserver& aObserver ):
                                  CActive( EPriorityStandard ),
                                  iObserver( aObserver )
                                  
    {
    CActiveScheduler::Add( this );
    }
                                  
//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationInfoConverterImpl::ConstructL( const TUid aConverterModuleId )
    {
    iConversionModuleId = aConverterModuleId;
    User::LeaveIfError( iLocMoniterSession.Connect() );
    iLocInfoConverter.OpenL( iLocMoniterSession );
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationInfoConverterImpl::ConvertLocationInfoL( 
                                  CLbsLocationInfoBase& aLocationInfo,
                                  const TLbsConversionPrefs aConversionPrefs,
                                  const TLbsConversionOutputInfoMask aRequestedInfo )
    {
    iLocInfoConverter.ConvertLocationInfoL( aLocationInfo,aConversionPrefs,aRequestedInfo,
                                           iConversionModuleId,iStatus );
    SetActive();
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationInfoConverterImpl::ConvertLocationInfoL(
                                  RLbsLocationInfoBaseArray& aLocationInfoArray,
                                  TLbsConversionPrefs aConversionPrefs,
                                  const TLbsConversionOutputInfoMask aRequestedInfo )
    {
    iLocInfoConverter.ConvertLocationInfoL( aLocationInfoArray,aConversionPrefs,aRequestedInfo,
                                            iConversionModuleId,iStatus );
    SetActive();
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::CancelConvertLocationInfoL
// (other items were commented in a header).
//-------;---------------------------------------------------------
void CLbsLocationInfoConverterImpl::CancelConvertLocationInfo()
    {   
    Cancel();
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::RunL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationInfoConverterImpl::RunL()
    {   
    if( iStatus.Int() != KErrCancel )
        {
        iObserver.OnConversionComplete( iStatus.Int() );
        }
    }

//----------------------------------------------------------------
// CLbsLocationInfoConverterImpl::DoCancel
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationInfoConverterImpl::DoCancel()
    {   
    iLocInfoConverter.CancelConvertLocationInfo();
    }

// End of file

