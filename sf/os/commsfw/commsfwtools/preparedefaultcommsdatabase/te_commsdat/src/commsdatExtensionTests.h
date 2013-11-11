/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file commsdatExtensionTests.h
*/
#if (!defined __COMMSDATCODEBASE_TESTS_H__)
#define __COMMSDATCODEBASE_TESTS_H__
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "Te_commsdatSuiteStepBase.h"
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdatutils.h>
#include <commsdattypesv1_1.h>
#include "commsdatValidationTests.h"
#include <commsdattypeinfov1_1.h>
#include "commsdatGenericRecordTests.h"

using namespace CommsDat;
using namespace Meta;
using namespace CommsDatUtils;

const TInt KLocationRecordId = 41;
const TInt KChargecardRecordId = 0;


_LIT(KCommsdat201Step,"commsdat201Step");
class CCommsdat201Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat201Step();
	~CCommsdat201Step(){}
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat202Step,"commsdat202Step");
class CCommsdat202Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat202Step();
	~CCommsdat202Step(){}
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat203Step,"commsdat203Step");
class CCommsdat203Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat203Step();
	~CCommsdat203Step(){}
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat204Step,"commsdat204Step");
class CCommsdat204Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat204Step();
	~CCommsdat204Step(){}		
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat205Step,"commsdat205Step");
class CCommsdat205Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat205Step();
	~CCommsdat205Step(){}
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat206Step,"commsdat206Step");
class CCommsdat206Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat206Step();
	~CCommsdat206Step(){}
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat207Step,"commsdat207Step");
class CCommsdat207Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat207Step();
	~CCommsdat207Step(){}
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};
	
_LIT(KCommsdat208Step,"commsdat208Step");
class CCommsdat208Step : public CConnectionPrefValidationTestBase 
 
	{
public:
	CCommsdat208Step();
	~CCommsdat208Step(){}
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat209Step,"commsdat209Step");
class CCommsdat209Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat209Step();
	~CCommsdat209Step(){}
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat210Step,"commsdat210Step");
class CCommsdat210Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat210Step();
	~CCommsdat210Step(){}
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat211Step,"commsdat211Step");
class CCommsdat211Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat211Step();
	~CCommsdat211Step(){}
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat212Step,"commsdat212Step");
class CCommsdat212Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat212Step();
	~CCommsdat212Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
private:
	CMDBSession* iCDSession;
	};

_LIT(KCommsdat213Step,"commsdat213Step");
class CCommsdat213Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat213Step();
	~CCommsdat213Step(){}
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat214Step,"commsdat214Step");
class CCommsdat214Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat214Step();
	~CCommsdat214Step(){}
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat215Step,"commsdat215Step");
class CCommsdat215Step : public CTe_commsdatSuiteStepBase 
	{
public:
	CCommsdat215Step();
	~CCommsdat215Step(){}
	virtual TVerdict doTestStepL();
	};

#endif  //__COMMSDATCODEBASE_TESTS_H__




