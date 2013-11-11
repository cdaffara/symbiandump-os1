// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDE FILES
#include "ctlbspanicpsy.h"
#include "panicpsy.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPanicPsy::CT_LbsPanicPsy() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsPanicPsy::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
    // PSY Should just Panic
    _LIT(KPanicErr, "PANIC from PanicPSY");
    User::Panic(KPanicErr, 210);
    }

// Two-phased constructor.
CT_LbsPanicPsy* CT_LbsPanicPsy::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsPanicPsy* self = new (ELeave) CT_LbsPanicPsy;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsPanicPsy* CT_LbsPanicPsy::NewL(TAny* aConstructionParameters)
    {
    CT_LbsPanicPsy* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsPanicPsy::~CT_LbsPanicPsy()
    {
    }

// ---------------------------------------------------------
// CT_LbsPanicPsy::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPanicPsy::NotifyPositionUpdate(
	TPositionInfoBase& /*aPosInfo*/,
    TRequestStatus& /*aStatus*/)
	{
	_LIT(KPanicErr, "PANIC from PanicPSY");
    // PSY Should just Panic
    User::Panic(KPanicErr, 209);
    }



// ---------------------------------------------------------
// CT_LbsPanicPsy::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPanicPsy::CancelNotifyPositionUpdate()
	{
	}

// End of file


