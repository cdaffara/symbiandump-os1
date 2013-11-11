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
#include "ctlbsclientlog.h"
#include <bautils.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsClientLog::CT_LbsClientLog()
    {
    }

// EPOC default constructor can leave.
void CT_LbsClientLog::ConstructL(CTestExecuteLogger& aLogger)
    {
    iLogger = aLogger;
    }

// Two-phased constructor.
EXPORT_C CT_LbsClientLog* CT_LbsClientLog::NewL(CTestExecuteLogger& aLogger)
    {
    CT_LbsClientLog* self = NewLC(aLogger);
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
EXPORT_C CT_LbsClientLog* CT_LbsClientLog::NewLC(CTestExecuteLogger& aLogger)
    {
    CT_LbsClientLog* self = new (ELeave) CT_LbsClientLog;
    CleanupStack::PushL(self);
    self->ConstructL(aLogger);
    return self;
    }

// Destructor
CT_LbsClientLog::~CT_LbsClientLog()
    {    
    }

void CT_LbsClientLog::Put(const TDesC& aLine)
   {
   INFO_PRINTF1(aLine);
   }

void CT_LbsClientLog::PutError(const TDesC& aLine)
   {
   ERR_PRINTF1(aLine);
   }
