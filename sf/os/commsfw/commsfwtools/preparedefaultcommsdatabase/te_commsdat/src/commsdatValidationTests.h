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
* CCommsdat100Step
* CCommsdat101Step
* CCommsdat102Step
* CCommsdat103Step
* CCommsdat104Step
* CCommsdat105Step
* 
*
*/



/**
 @file commsdatValidationTests.h
*/
#if (!defined __COMMSDAT_VALIDATION_TESTS_H__)
#define __COMMSDAT_VALIDATION_TESTS_H__
#include "Te_commsdatSuiteStepBase.h"
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

const TInt KConnPrefRecordId = 1;
const TInt KIAPRecordId = 81;

using namespace CommsDat;
using namespace Meta;

//Base class for connection prefernces validation tests
//------------------------------------------------------------------------------------------
class CConnectionPrefValidationTestBase : public CTe_commsdatSuiteStepBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void deleteAllL();
	};

// Test Step declerations
//------------------------------------------------------------------------------------------
_LIT(KCommsdat100Step,"commsdat100Step");
class CCommsdat100Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat100Step();
	~CCommsdat100Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat101Step,"commsdat101Step");
class CCommsdat101Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat101Step();
	~CCommsdat101Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat102Step,"commsdat102Step");
class CCommsdat102Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat102Step();
	~CCommsdat102Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat103Step,"commsdat103Step");
class CCommsdat103Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat103Step();
	~CCommsdat103Step(){};
	virtual TVerdict doTestStepL();
	};
	
_LIT(KCommsdat104Step,"commsdat104Step");
class CCommsdat104Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat104Step();
	~CCommsdat104Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat105Step,"commsdat105Step");
class CCommsdat105Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat105Step();
	~CCommsdat105Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat106Step,"commsdat106Step");
class CCommsdat106Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat106Step();
	~CCommsdat106Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat107Step,"commsdat107Step");
class CCommsdat107Step : public CConnectionPrefValidationTestBase 
	{
public:
	CCommsdat107Step();
	~CCommsdat107Step(){};
	virtual TVerdict doTestStepL();
	};

_LIT(KCommsdat332Step,"commsdat332Step");
class CCommsdat332Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat332Step();
	~CCommsdat332Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat333Step,"commsdat333Step");
class CCommsdat333Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat333Step();
	~CCommsdat333Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat334Step,"commsdat334Step");
class CCommsdat334Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat334Step();
	~CCommsdat334Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat335Step,"commsdat335Step");
class CCommsdat335Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat335Step();
	~CCommsdat335Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat336Step,"commsdat336Step");
class CCommsdat336Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat336Step();
	~CCommsdat336Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat337Step,"commsdat337Step");
class CCommsdat337Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat337Step();
	~CCommsdat337Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat338Step,"commsdat338Step");
class CCommsdat338Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat338Step();
	~CCommsdat338Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat339Step,"commsdat339Step");
class CCommsdat339Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat339Step();
	~CCommsdat339Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat340Step,"commsdat340Step");
class CCommsdat340Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat340Step();
	~CCommsdat340Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat341Step,"commsdat341Step");
class CCommsdat341Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat341Step();
	~CCommsdat341Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat342Step,"commsdat342Step");
class CCommsdat342Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat342Step();
	~CCommsdat342Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat343Step,"commsdat343Step");
class CCommsdat343Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat343Step();
	~CCommsdat343Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat344Step,"commsdat344Step");
class CCommsdat344Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat344Step();
	~CCommsdat344Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat345Step,"commsdat345Step");
class CCommsdat345Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat345Step();
	~CCommsdat345Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat346Step,"commsdat346Step");
class CCommsdat346Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat346Step();
	~CCommsdat346Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat347Step,"commsdat347Step");
class CCommsdat347Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat347Step();
	~CCommsdat347Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat348Step,"commsdat348Step");
class CCommsdat348Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat348Step();
	~CCommsdat348Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat349Step,"commsdat349Step");
class CCommsdat349Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat349Step();
	~CCommsdat349Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat350Step,"commsdat350Step");
class CCommsdat350Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat350Step();
	~CCommsdat350Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat351Step,"commsdat351Step");
class CCommsdat351Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat351Step();
	~CCommsdat351Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat352Step,"commsdat352Step");
class CCommsdat352Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat352Step();
	~CCommsdat352Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat353Step,"commsdat353Step");
class CCommsdat353Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat353Step();
	~CCommsdat353Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat354Step,"commsdat354Step");
class CCommsdat354Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat354Step();
	~CCommsdat354Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat355Step,"commsdat355Step");
class CCommsdat355Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat355Step();
	~CCommsdat355Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat356Step,"commsdat356Step");
class CCommsdat356Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat356Step();
	~CCommsdat356Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat357Step,"commsdat357Step");
class CCommsdat357Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat357Step();
	~CCommsdat357Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat358Step,"commsdat358Step");
class CCommsdat358Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat358Step();
	~CCommsdat358Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat359Step,"commsdat359Step");
class CCommsdat359Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat359Step();
	~CCommsdat359Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat360Step,"commsdat360Step");
class CCommsdat360Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat360Step();
	~CCommsdat360Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat361Step,"commsdat361Step");
class CCommsdat361Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat361Step();
	~CCommsdat361Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat362Step,"commsdat362Step");
class CCommsdat362Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat362Step();
	~CCommsdat362Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat363Step,"commsdat363Step");
class CCommsdat363Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat363Step();
	~CCommsdat363Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat364Step,"commsdat364Step");
class CCommsdat364Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat364Step();
	~CCommsdat364Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat365Step,"commsdat365Step");
class CCommsdat365Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat365Step();
	~CCommsdat365Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat366Step,"commsdat366Step");
class CCommsdat366Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat366Step();
	~CCommsdat366Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat367Step,"commsdat367Step");
class CCommsdat367Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat367Step();
	~CCommsdat367Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat368Step,"commsdat368Step");
class CCommsdat368Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat368Step();
	~CCommsdat368Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat369Step,"commsdat369Step");
class CCommsdat369Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat369Step();
	~CCommsdat369Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat370Step,"commsdat370Step");
class CCommsdat370Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat370Step();
	~CCommsdat370Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat371Step,"commsdat371Step");
class CCommsdat371Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat371Step();
	~CCommsdat371Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat372Step,"commsdat372Step");
class CCommsdat372Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat372Step();
	~CCommsdat372Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat380Step,"commsdat380Step");
class CCommsdat380Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat380Step();
	~CCommsdat380Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat381Step,"commsdat381Step");
class CCommsdat381Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat381Step();
	~CCommsdat381Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat382Step,"commsdat382Step");
class CCommsdat382Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat382Step();
	~CCommsdat382Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat383Step,"commsdat383Step");
class CCommsdat383Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat383Step();
	~CCommsdat383Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat384Step,"commsdat384Step");
class CCommsdat384Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat384Step();
	~CCommsdat384Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat385Step,"commsdat385Step");
class CCommsdat385Step : public CConnectionPrefValidationTestBase
	{
public:
	CCommsdat385Step();
	~CCommsdat385Step();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
private:
	void CreateSessionsL();
	
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

#endif // __COMMSDAT_VALIDATION_TESTS_H__
