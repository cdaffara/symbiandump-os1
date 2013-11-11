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
#include <lbs/lbslocationresolver.h>
#include <lbs/lbslocationinfoconvertercommon.h>
#include "lbslocationresolverimpl.h"


EXPORT_C TVersion MLbsLocationResolverObserver::Version()  const
	{
	return TVersion(1, 0, 0);
	}



//----------------------------------------------------------------
// CLbsLocationResolver::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C CLbsLocationResolver* CLbsLocationResolver::NewL(
                                     MLbsLocationResolverObserver& aObserver, 
                                     const TUid aConverterModuleId )
    {
    CLbsLocationResolver* self = new( ELeave ) CLbsLocationResolver;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver,aConverterModuleId );
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------
// CLbsLocationResolver::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C CLbsLocationResolver* CLbsLocationResolver::NewL(
                                     MLbsLocationResolverObserver& aObserver )
    {
    CLbsLocationResolver* self = new( ELeave ) CLbsLocationResolver;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver,TUid::Uid( KNullUidValue ) );
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------
// CLbsLocationResolver::~CLbsLocationResolver
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C CLbsLocationResolver::~CLbsLocationResolver()
    {
    delete iLocationResolverImpl;
    iLocationResolverImpl = NULL;
    }

//----------------------------------------------------------------
// CLbsLocationResolver::ResolveLocationL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void CLbsLocationResolver::ResolveLocationL(const CLbsLocationInfo& aLocationInfo)
    {
    iLocationResolverImpl->ResolveLocationL(aLocationInfo);
    }


//----------------------------------------------------------------
// CLbsLocationResolverImpl::SetConversionPreferences
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C TInt CLbsLocationResolver::SetConversionPreferences(TLbsConversionPrefs aConversionPrefs)
    {
    return iLocationResolverImpl->SetConversionPreferences(aConversionPrefs);
    }

//----------------------------------------------------------------
// CLbsLocationResolverImpl::GetConversionPref
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C TLbsConversionPrefs CLbsLocationResolver::ConversionPreferences()
    {
    return iLocationResolverImpl->ConversionPreferences();
    }

//----------------------------------------------------------------
// CLbsLocationResolver::CancelResolveLocation
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void CLbsLocationResolver::CancelResolveLocation()
    {
    iLocationResolverImpl->CancelResolveLocation();
    }

//----------------------------------------------------------------
// CLbsLocationResolver::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocationResolver::ConstructL( 
                        MLbsLocationResolverObserver& aObserver, 
                        const TUid aConverterModuleId )
    {
    iLocationResolverImpl = CLbsLocationResolverImpl::NewL(
                                         aObserver,aConverterModuleId );
    }

//----------------------------------------------------------------
// CLbsLocationResolver::CLbsLocationResolver
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocationResolver::CLbsLocationResolver()
    {
    
    }
