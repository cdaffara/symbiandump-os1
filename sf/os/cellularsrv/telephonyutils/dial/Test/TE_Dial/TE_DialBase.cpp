// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dial API Test base test code.
// 
//

/**
 @file
*/

#include "TE_DialBase.h"


CDialTestStep::CDialTestStep()
	{
	// NOP
	} // CDialTestStep::CDialTestStep


CDialTestStep::~CDialTestStep()
	{
	// NOP
	} // CDialTestStep::~CDialTestStep


TVerdict CDialTestStep::doTestStepPreambleL()
	{
	//
	// Mark for memory leaks!!!
	//
	__UHEAP_MARK;

	//
	// Create an Active Scheduler...
	//
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);

	return TestStepResult();
	} // CDialTestStep::doTestStepPreambleL


TVerdict CDialTestStep::doTestStepPostambleL()
	{
	delete iScheduler;
	iScheduler = NULL;
	
	//
	// Check the heap for memory leaks...
	//
	__UHEAP_MARKEND;

	return TestStepResult();
	} // CDialTestStep::doTestStepPostambleL


