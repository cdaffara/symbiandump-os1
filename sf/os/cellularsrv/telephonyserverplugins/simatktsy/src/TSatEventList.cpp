// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : TSatEventList.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Event list - related functionality of Sat Tsy
// Version     : 1.0
//




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TSatEventListTraces.h"
#endif

#include "TSatEventList.h"   // Class header

// -----------------------------------------------------------------------------
// TSatEventList::TSatEventList
// TSatEventList C++ constructor
// -----------------------------------------------------------------------------
//
TSatEventList::TSatEventList
        (
        void 
        ) : iEvents( 0 )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS ,TSATEVENTLIST_CTOR_1,  "CSAT: TSatEventList::TSatEventList" );
    }

// -----------------------------------------------------------------------------
// TSatEventList::IsEventEnabled
// Check if event is ín the set of enabled events
// -----------------------------------------------------------------------------
//
TBool TSatEventList::IsEnabled
        (
        RSat::TEventList aEvent
        ) const
    {

    if( iEvents & TUint32( aEvent ) )
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSATEVENTLIST_ISENABLED_1,  "CSAT: TSatEventList::IsEventEnabled, Event Enabled: %d", aEvent );
        return ETrue;
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSATEVENTLIST_ISENABLED_2,  "CSAT: TSatEventList::IsEventEnabled, Event Not enabled" );
        return EFalse;
        }

    }

// -----------------------------------------------------------------------------
// TSatEventList::RemoveFromEventList
// Remove event from set of enabled events
// -----------------------------------------------------------------------------
//
void TSatEventList::RemoveFromEventList
        (
        RSat::TEventList aEvent
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSATEVENTLIST_REMOVEFROMEVENTLIST_1,  "CSAT: TSatEventList::RemoveFromEventList" );
    iEvents &= ~TUint32( aEvent );
    }

// -----------------------------------------------------------------------------
// TSatEventList::AddToEventList
// Add event to set of enabled events
// -----------------------------------------------------------------------------
//
void TSatEventList::AddToEventList
        (
        RSat::TEventList aEvent
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSATEVENTLIST_ADDTOEVENTLIST_1,  "CSAT: TSatEventList::AddToEventList" );
    iEvents |= TUint32( aEvent );
    }

// -----------------------------------------------------------------------------
// TSatEventList::SetEventList
// Set the list of enabled events
// -----------------------------------------------------------------------------
//
void TSatEventList::SetEventList
        (
        TUint32 aEvent
        )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TSATEVENTLIST_SETEVENTLIST_1,  "CSAT: TSatEventList::SetEventList aEvent:%d", aEvent );
    iEvents = aEvent;
    }

// End of File
