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
* Description: Base class for all the class representing
* location informations.
*
*/


#include <e32std.h>
#include <lbs/lbslocationclasstypes.h>
#include <lbs/lbslocationcommon.h>


// ---------------------------------------------------------------------------
// CLbsLocationInfoBase::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsLocationInfoClassType CLbsLocationInfoBase::Type()
	{
	return ELbsAreaInfoUnknownClass;
	}

// ---------------------------------------------------------------------------
// CLbsLocationInfoBase::~CLbsLocationInfoBase()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsLocationInfoBase::~CLbsLocationInfoBase()
	{
	}

// ---------------------------------------------------------------------------
// CLbsLocationInfoBase::InternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsLocationInfoBase::InternaliseL( RReadStream& aStream )
    {
    DoInternaliseL( aStream );
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfoBase::ExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsLocationInfoBase::ExternaliseL( RWriteStream& aStream ) const
    {
    DoExternaliseL( aStream );
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfoBase::CLbsLocationInfoBase()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsLocationInfoBase::CLbsLocationInfoBase()
    {
    }

// ---------------------------------------------------------------------------
// CLbsAreaInfoBase::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsAreaInfoClassType CLbsAreaInfoBase::Type()
    {
	return ELbsAreaInfoUnknownClass;
    }

// ---------------------------------------------------------------------------
// CLbsAreaInfoBase::~CLbsAreaInfoBase()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsAreaInfoBase::~CLbsAreaInfoBase()
    {
    }

// ---------------------------------------------------------------------------
// CLbsAreaInfoBase::~CLbsAreaInfoBase()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsAreaInfoBase::CLbsAreaInfoBase()
    {
    }

// ---------------------------------------------------------------------------
// CLbsAreaInfoBase::InternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsAreaInfoBase::InternaliseL( RReadStream& aStream )
    {
    DoInternaliseL( aStream );
    }

// ---------------------------------------------------------------------------
// CLbsAreaInfoBase::ExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsAreaInfoBase::ExternaliseL( RWriteStream& aStream )
    {
    DoExternaliseL( aStream );
    }
