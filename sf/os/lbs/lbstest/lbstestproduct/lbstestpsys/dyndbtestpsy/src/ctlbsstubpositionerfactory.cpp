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
#include "ctlbsstubpositionerfactory.h"
#include "ctlbsstubpositioner.h"
#include "ctlbsstubnmeareader.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsStubPositionerFactory::CT_LbsStubPositionerFactory()
    {
    }

    
// Destructor
CT_LbsStubPositionerFactory::~CT_LbsStubPositionerFactory()
    {
    }


// ---------------------------------------------------------
// CT_LbsStubPositionerFactory::CreateSubSessionL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosSubSession* CT_LbsStubPositionerFactory::CreateSubSessionL(
    TUint aType)
	{
    if (aType == KPosCapsNMEA)
        {
        CPosNmeaReaderSubSession* subsession = CPosNmeaReaderSubSession::NewL();
        CleanupClosePushL(*subsession); 
            
        subsession->SetNmeaReader(REINTERPRET_CAST(CPosNmeaReader*,
                                                  CreateHandlerL(aType)));
        CleanupStack::Pop(); 
        return subsession;
        }
    else if (aType == KPosCapsPositioner)
        {                            
        CPositionerSubSession* subsession = CPositionerSubSession::NewL();
        CleanupClosePushL(*subsession);
        subsession->SetPositioner(REINTERPRET_CAST(CPositioner*,
                                                  CreateHandlerL(aType)));
        CleanupStack::Pop();
        return subsession;
        }   
    
    User::Leave(KErrNotSupported);
    return NULL;
	}

// ---------------------------------------------------------
// CT_LbsStubPositionerFactory::CreateHandlerL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosHandler* CT_LbsStubPositionerFactory::CreateHandlerL(
    TUint aType)
	{
    if (aType == KPosCapsNMEA)
        {
        return CT_LbsStubNmeaReader::NewL();
        }
    else if (aType == KPosCapsPositioner)
        {                            
        return CT_LbsDynStubPositioner::NewL();            
        }                                  
    User::Leave(KErrNotSupported);
    return NULL;
	}

//  End of File  
