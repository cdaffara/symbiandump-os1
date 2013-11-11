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
*
*/



/**
 @file commsdatMixedSnapValidationTests.h
*/
#if (!defined __COMMSDAT_MIXED_SNAP_VALIDATION_TESTS_H__)
#define __COMMSDAT_MIXED_SNAP_VALIDATION_TESTS_H__
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "Te_commsdatSuiteStepBase.h"
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;

_LIT(KCommsdat400Step,"commsdat400Step");
class CCommsdat400Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat400Step();
	~CCommsdat400Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat401Step,"commsdat401Step");
class CCommsdat401Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat401Step();
	~CCommsdat401Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat402Step,"commsdat402Step");
class CCommsdat402Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat402Step();
	~CCommsdat402Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat403Step,"commsdat403Step");
class CCommsdat403Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat403Step();
	~CCommsdat403Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat404Step,"commsdat404Step");
class CCommsdat404Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat404Step();
	~CCommsdat404Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat405Step,"commsdat405Step");
class CCommsdat405Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat405Step();
	~CCommsdat405Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat406Step,"commsdat406Step");
class CCommsdat406Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat406Step();
	~CCommsdat406Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat407Step,"commsdat407Step");
class CCommsdat407Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat407Step();
	~CCommsdat407Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat408Step,"commsdat408Step");
class CCommsdat408Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat408Step();
	~CCommsdat408Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat409Step,"commsdat409Step");
class CCommsdat409Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat409Step();
	~CCommsdat409Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

#endif  //__COMMSDAT_MIXED_SNAP_VALIDATION_TESTS_H__

