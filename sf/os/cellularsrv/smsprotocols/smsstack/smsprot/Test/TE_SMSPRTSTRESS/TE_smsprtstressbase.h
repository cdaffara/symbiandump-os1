// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_SMSPRTBASE_H
#define TE_SMSPRTBASE_H

//#include <connect/sbeclient.h>

#include "smsstacktestutilities.h"
#include "smsstackbaseteststeps.h"
#include "Gsmumsg.h"
#include "gsmuset.h"
#include "gsmubuf.h"
#include "smsustrm.h"
#include "smspdudb.h"

//using namespace conn;

const TInt KActiveReceiverPriority = CActive::EPriorityStandard;

class CSmsPrtStressTestStep : public CSmsBaseTestStep
{
public:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

public:
//	CSBEClient*  iSecureBackupEngine;
};

#endif
