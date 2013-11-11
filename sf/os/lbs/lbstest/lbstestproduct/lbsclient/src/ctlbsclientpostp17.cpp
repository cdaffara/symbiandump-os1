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



//  INCLUDES
#include "ctlbsclientpostp17.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp17::CT_LbsClientPosTp17(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent) 
	{  
	_LIT(KTestName, "TP17 - Open nonexisting PSY");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp17::~CT_LbsClientPosTp17()
	{
	}
// ---------------------------------------------------------
// CT_LbsClientPosTp17::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp17::StartL()
    {    
    TUid nonExistent;
    nonExistent.iUid = 0x01234DCBD;
    
    TBuf<100> output;        

    ConnectL();
    TInt err = OpenPositionerByName(nonExistent);
    if (err != KErrNotFound )
        {
        output.Format(output, KErrNotFound , err);
        LogErrorAndLeaveL(output);
        }
    Disconnect();
    }

//End of File
