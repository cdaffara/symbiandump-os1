/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



/**
 @file ListPsyEntriesStep.h
 @internalTechnology
*/
#ifndef __LISTPSYENTRIES_STEP_H__
#define __LISTPSYENTRIES_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsbackuprestoresuitestepbase.h"

class CListPsyEntriesStep : public CTe_lbsbackuprestoreSuiteStepBase
	{
public:
	CListPsyEntriesStep();
	~CListPsyEntriesStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	static CListPsyEntriesStep* NewL();
	static CListPsyEntriesStep* NewLC();
	
	};

_LIT(KListPsyEntriesStep,"ListPsyEntriesStep");

#endif
