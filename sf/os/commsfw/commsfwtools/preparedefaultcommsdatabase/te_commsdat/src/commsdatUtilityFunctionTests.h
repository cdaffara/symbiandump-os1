/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CCommsdat115Step
* CCommsdat116Step
* CCommsdat117Step
* CCommsdat118Step
* 
*
*/



/**
 @file commsdatUtilityFunctionTests.h
*/
#if (!defined __COMMSDAT_UTILITY_FUNCTION_TESTS_H__)
#define __COMMSDAT_UTILITY_FUNCTION_TESTS_H__
#include "Te_commsdatSuiteStepBase.h"
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>
#include <commsdatutils.h>

using namespace CommsDat;
using namespace Meta;
using namespace CommsDatUtils;

_LIT(KCommsdat115Step,"commsdat115Step");
class CCommsdat115Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat115Step();
	~CCommsdat115Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat116Step,"commsdat116Step");
class CCommsdat116Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat116Step();
	~CCommsdat116Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat117Step,"commsdat117Step");
class CCommsdat117Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat117Step();
	~CCommsdat117Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat118Step,"commsdat118Step");
class CCommsdat118Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat118Step();
	~CCommsdat118Step(){};
	virtual TVerdict doTestStepL();
	};

#endif // __COMMSDAT_UTILITY_FUNCTION_TESTS_H__ 
