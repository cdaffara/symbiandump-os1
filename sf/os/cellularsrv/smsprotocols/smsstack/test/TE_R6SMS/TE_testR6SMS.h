// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_TESTR6SMS_H
#define TE_TESTR6SMS_H

#include <test/testexecuteserverbase.h>

class CTestCSmsMessageAdditionalAttributes1 : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestCSmsMessageAdditionalAttributes2 : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestCSmsMessageAdditionalAttributes3 : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestR6CSmsMessage : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestAddingIEsViaNewAndExistingInterfaces : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestIECategoryDefinitions : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestR6DCS : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

#endif
