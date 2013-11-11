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
* CCommsdat170Step
* CCommsdat171Step
* CCommsdat172Step
* 
*
*/



/**
 @file commsdatConcurrencyTests.h
*/
#if (!defined __COMMSDAT_CONCURRENCY_TESTS_H__)
#define __COMMSDAT_CONCURRENCY_TESTS_H__
#include "Te_commsdatSuiteStepBase.h"
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;

_LIT(KCommsdat170Step,"commsdat170Step");
class CCommsdat170Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat170Step();
	~CCommsdat170Step(){};
	
	virtual TVerdict doTestStepL();
private:
	static TInt ThreadFunction(TAny *aPtr);
	static TInt DoThreadFunctionL(TAny *aPtr);
	};


_LIT(KCommsdat171Step,"commsdat171Step");
class CCommsdat171Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat171Step();
	~CCommsdat171Step(){};
	
	virtual TVerdict doTestStepL();
private:
	static TInt ThreadFunction(TAny *aPtr);
	static TInt DoThreadFunctionL(TAny *aPtr);
	};

_LIT(KCommsdat172Step,"commsdat172Step");
class CCommsdat172Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat172Step();
	~CCommsdat172Step(){};
	
	virtual TVerdict doTestStepL();
private:
	static TInt ThreadFunction(TAny *aPtr);
	static TInt DoThreadFunctionL(TAny *aPtr);
	}; 

#endif // __COMMSDAT_CONCURRENCY_TESTS_H__ 
