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
* CCommsdatInitAttributes
* CCommsdat130Step
* CCommsdat131Step
* CCommsdat132Step
* CCommsdat133Step
* CCommsdat134Step
* CCommsdat135Step
* CCommsdat136Step
* CCommsdat137Step
* CCommsdat138Step
* CCommsdat139Step
* CCommsdat140Step
* CCommsdat141Step
* CCommsdat142Step
* 
*
*/



/**
 @file commsdatAttributeTests.h
*/
#if (!defined __COMMSDAT_ATTRIBUTE_TESTS_H__)
#define __COMMSDAT_ATTRIBUTE_TESTS_H__
#include "Te_commsdatSuiteStepBase.h"
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>


using namespace CommsDat;
using namespace Meta;

_LIT(KCommsdatInitAttributes,"CommsdatInitAttributes");
class CCommsdatInitAttributes : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdatInitAttributes();
	~CCommsdatInitAttributes(){};
	virtual TVerdict doTestStepL();
	};


_LIT(KCommsdat130Step,"commsdat130Step");
class CCommsdat130Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat130Step();
	~CCommsdat130Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat131Step,"commsdat131Step");
class CCommsdat131Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat131Step();
	~CCommsdat131Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat132Step,"commsdat132Step");
class CCommsdat132Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat132Step();
	~CCommsdat132Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat133Step,"commsdat133Step");
class CCommsdat133Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat133Step();
	~CCommsdat133Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat134Step,"commsdat134Step");
class CCommsdat134Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat134Step();
	~CCommsdat134Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat135Step,"commsdat135Step");
class CCommsdat135Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat135Step();
	~CCommsdat135Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat136Step,"commsdat136Step");
class CCommsdat136Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat136Step();
	~CCommsdat136Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat137Step,"commsdat137Step");
class CCommsdat137Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat137Step();
	~CCommsdat137Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat138Step,"commsdat138Step");
class CCommsdat138Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat138Step();
	~CCommsdat138Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat139Step,"commsdat139Step");
class CCommsdat139Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat139Step();
	~CCommsdat139Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat140Step,"commsdat140Step");
class CCommsdat140Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat140Step();
	~CCommsdat140Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat141Step,"commsdat141Step");
class CCommsdat141Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat141Step();
	~CCommsdat141Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat142Step,"commsdat142Step");
class CCommsdat142Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat142Step();
	~CCommsdat142Step(){};
	virtual TVerdict doTestStepL();
	};

#endif // __COMMSDAT_ATTRIBUTE_TESTS_H__  
