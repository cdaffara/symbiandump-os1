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

#ifndef TE_SMSEMSPRTBASE_H
#define TE_SMSEMSPRTBASE_H

#include <test/testexecuteserverbase.h>
#include <simtsy.h>
#include <testconfigfileparser.h>
#include <mmlist.h>
#include <mmretrieve.h>

#include "gsmubuf.h"
#include "Gsmumsg.h"
#include "gsmuset.h"

#include "smsuaddr.H"
#include "smsustrm.h"
#include "smspdudb.h"
#include "smsstacktestutilities.h"
#include "smsstackbaseteststeps.h"

#include "attributes.h"
#include "smspproc.h"
#include "EMSFormatIE.h"
#include "EMSSoundIE.h"
#include "EMSPictureIE.h"
#include "EMSUserPromptIE.h"
#include "EMSAnimationIE.h"
#include "EmsTestUtils.h"
#include "EMSObjectDistributionIE.h"

class CSmsEmsPrtTestStep : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    
	TBool SendReceiveMsgL(CSmsMessage& aMsg);
    
protected:
	RSocket iSocket;
    };

#endif // TE_SMSEMSPRTBASE_H
