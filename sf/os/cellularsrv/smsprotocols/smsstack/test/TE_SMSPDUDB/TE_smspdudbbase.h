// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef _TE_SMSPDUDBBASE_H_
#define _TE_SMSPDUDBBASE_H_

#include "smspdudb.h"
#include <e32base.h>
#include <f32file.h>
#include "e32test.h"
#include "c32comm.h"
#include "Gsmumsg.h"
#include "gsmubuf.h"
#include "smsstacktestutilities.h"

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

_LIT(KSmsPduDbTestFilename, "te_smspdudb.txt");
_LIT(KSmsPduDbTestOutput, "c:\\sms\\te_smspdudb_parsed.txt");

class CSmsPduDbTestStep : public CTestStep
{

public:
	void PrintBuffer(const CSmsPduDbBase& aPdu);
	void PrintMessage(TInt aIndex, const CSmsPduDbMessage& aMessage);
	void PrintPdu(TInt aIndex, const CSmsPduDbPdu& aPdu);
	void PrintSegment(TInt aIndex, const CSmsPduDbConcatSegment& aPdu);
	void PrintConcat(TInt aIndex, const CSmsPduDbConcat& aConcat);
	void DisplayCompileErrorL(const CTestConfigSection& aSection, const CTestConfigItem& aItem, TInt aError);


public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	RFs iFs;
	CSmsStackTestUtils* iSmsStackTestUtils;

};

#endif
