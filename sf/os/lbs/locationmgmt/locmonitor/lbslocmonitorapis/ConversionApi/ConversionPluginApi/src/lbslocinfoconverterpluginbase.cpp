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
* Description: Implentation of functions shared between all 
* implementations of the LBS Network Protocol Module 
* interface/ECom plugin.
*
*/


#include <e32base.h>
#include <ecom/ecom.h>

#include <lbs/lbslocinfoconverterpluginbase.h>
#include <lbs/lbslocinfoconverterpluginclasstypes.h>


// ---------------------------------------------------------------------------
// MLbsLocInfoConverterPluginObserver::ExtendedInterface()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion MLbsLocInfoConverterPluginObserver::Version() const
    {
    return TVersion( 1, 0, 0 );
    }

// ---------------------------------------------------------------------------
// MLbsLocInfoConverterPluginObserver::ExtendedInterface()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* MLbsLocInfoConverterPluginObserver::ExtendedInterface( 
                TInt  /*aFunctionNumber*/,
                TAny* /*aPtr1*/,
                TAny* /*aPtr2*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CLbsLocInfoConverterPluginBase::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsLocInfoConverterPluginBase* CLbsLocInfoConverterPluginBase::NewL( 
                                    TUid aImplementationUid,
                                    TLbsLocInfoConverterPluginParams& aParams )
    {
    TAny* ptr = REComSession::CreateImplementationL( aImplementationUid, 
                         _FOFF( CLbsLocInfoConverterPluginBase, iDtor_ID_Key ),
                         reinterpret_cast<TAny*>( &aParams ) );

    return reinterpret_cast<CLbsLocInfoConverterPluginBase*>(ptr);
    }  

// ---------------------------------------------------------------------------
// CLbsLocInfoConverterPluginBase::~CLbsLocInfoConverterPluginBase()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsLocInfoConverterPluginBase::~CLbsLocInfoConverterPluginBase()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ---------------------------------------------------------------------------
// CLbsLocInfoConverterPluginBase::Version()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion CLbsLocInfoConverterPluginBase::Version() const
    {
    return TVersion( 1, 0, 0 );
    }

// ---------------------------------------------------------------------------
// CLbsLocInfoConverterPluginBase::ExtendedInterface()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CLbsLocInfoConverterPluginBase::ExtendedInterface(
                                    TInt  /*aFunctionNumber*/,
                                    TAny* /*aPtr1*/,
                                    TAny* /*aPtr2*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TLbsLocInfoConverterPluginParams::TLbsLocInfoConverterPluginParams()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsLocInfoConverterPluginParams::TLbsLocInfoConverterPluginParams(
           MLbsLocInfoConverterPluginObserver& aObserver ):iObserver(aObserver)
    {
    iClassType = ELbsLocInfoConverterPluginParamsClass;
    iClassSize = sizeof(TLbsLocInfoConverterPluginParams);
    }
		
// ---------------------------------------------------------------------------
// TLbsLocInfoConverterPluginParams::Observer()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C MLbsLocInfoConverterPluginObserver& TLbsLocInfoConverterPluginParams::Observer() const
	{
	return iObserver;
	}
