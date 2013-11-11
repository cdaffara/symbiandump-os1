// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestStepActrlController.h
// This file contains Test step implementations for
// controller methods.
//


#if (!defined __TESTSTEP_CONTROLLER_H__)
#define __TESTSTEP_CONTROLLER_H__

#include <e32std.h>
#include <e32base.h>
#include "TS_MMF_ACTRL_TestStep.h"

/**
 *
 * CTestStepResetController
 * This class tests resetting the controller
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepResetController : public CTestStepActrl
	{
public:
	CTestStepResetController();
	~CTestStepResetController();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepRemoveDataSink
 * This class tests removing a data sink
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepRemoveDataSink : public CTestStepActrl
	{
public:
	CTestStepRemoveDataSink();
	~CTestStepRemoveDataSink();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepRemoveDataSource
 * This class tests removing a data source
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepRemoveDataSource : public CTestStepActrl
	{
public:
	CTestStepRemoveDataSource();
	~CTestStepRemoveDataSource();
	virtual TVerdict  DoTestStepL( void );
	};

/**
 *
 * CTestStepMemoryAllocation
 * This class tests memory allocation on the plugin
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepMemoryAllocation : public CTestStepActrl
	{
public:
	CTestStepMemoryAllocation();
	~CTestStepMemoryAllocation();
	virtual TVerdict  DoTestStepL( void );
	};

#endif // ( __TESTSTEP_CONTROLLER_H__)
