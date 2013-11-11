// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#ifndef __TE_SIM_ADDITIONALPARAMS_TEST__
#define __TE_SIM_ADDITIONALPARAMS_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimPacketDefaultAdditionalParamsTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketDefaultAdditionalParamsTest();
 	virtual TVerdict doTestStepL();

private:
	
	/** A packet service */
	RPacketService iPacketService;
	
	/** A packet context */
	RPacketContext iPacketContext;
	};

class CSimPacketAdditionalParamsTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketAdditionalParamsTest();
 	virtual TVerdict doTestStepL();

private:

	/** A packet service */
	RPacketService iPacketService;

	/** A packet context */
	RPacketContext iPacketContext;
	};

class CSimPacketAdditionalParamsTest2 : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketAdditionalParamsTest2();
 	virtual TVerdict doTestStepL();

private:

	/** A packet service */
	RPacketService iPacketService;

	/** A packet context */
	RPacketContext iPacketContext;
	};
	
class CSimPacketAdditionalParamsTest3 : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketAdditionalParamsTest3();
 	virtual TVerdict doTestStepL();

private:

	/** A packet service */
	RPacketService iPacketService;

	/** A packet context */
	RPacketContext iPacketContext;
	};

class CSimPacketBinaryAdditionalParamsTest1 : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketBinaryAdditionalParamsTest1();
 	virtual TVerdict doTestStepL();

private:

	/** A packet service */
	RPacketService iPacketService;

	/** A packet context */
	RPacketContext iPacketContext;
	};
	
class CSimPacketBinaryAdditionalParamsTest2 : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketBinaryAdditionalParamsTest2();
 	virtual TVerdict doTestStepL();

private:

	/** A packet service */
	RPacketService iPacketService;

	/** A packet context */
	RPacketContext iPacketContext;
	};	

class CSimPacketDefaultBinaryAdditionalParamsTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketDefaultBinaryAdditionalParamsTest();
 	virtual TVerdict doTestStepL();

private:
	
	/** A packet service */
	RPacketService iPacketService;
	
	/** A packet context */
	RPacketContext iPacketContext;
	};
	
#endif // __TE_SIM_ADDITIONALPARAMS_TEST__
