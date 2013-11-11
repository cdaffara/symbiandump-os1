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
 @file commsdatSnapTests.h
*/
#if (!defined __COMMSDAT_SNAP_TESTS_H__)
#define __COMMSDAT_SNAP_TESTS_H__
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "Te_commsdatSuiteStepBase.h"
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;

_LIT(KCommsdat190Step,"commsdat190Step");
class CCommsdat190Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat190Step();
	~CCommsdat190Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat191Step,"commsdat191Step");
class CCommsdat191Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat191Step();
	~CCommsdat191Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat192Step,"commsdat192Step");
class CCommsdat192Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat192Step();
	~CCommsdat192Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat193Step,"commsdat193Step");
class CCommsdat193Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat193Step();
	~CCommsdat193Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};	

_LIT(KCommsdat194Step,"commsdat194Step");
class CCommsdat194Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat194Step();
	~CCommsdat194Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat195Step,"commsdat195Step");
class CCommsdat195Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat195Step();
	~CCommsdat195Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat196Step,"commsdat196Step");
class CCommsdat196Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat196Step();
	~CCommsdat196Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};
//---------------------------------------
//				FIELD TESTS
//---------------------------------------
//I couldnt' follow the numbering here because the 197Step is already 
//reserved for wifi testing.
_LIT(KCommsdat307Step,"commsdat307Step");
class CCommsdat307Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat307Step();
	~CCommsdat307Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat308Step,"commsdat308Step");
class CCommsdat308Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat308Step();
	~CCommsdat308Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat309Step,"commsdat309Step");
class CCommsdat309Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat309Step();
	~CCommsdat309Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat310Step,"commsdat310Step");
class CCommsdat310Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat310Step();
	~CCommsdat310Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat311Step,"commsdat311Step");
class CCommsdat311Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat311Step();
	~CCommsdat311Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat312Step,"commsdat312Step");
class CCommsdat312Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat312Step();
	~CCommsdat312Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat313Step,"commsdat313Step");
class CCommsdat313Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat313Step();
	~CCommsdat313Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat314Step,"commsdat314Step");
class CCommsdat314Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat314Step();
	~CCommsdat314Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat315Step,"commsdat315Step");
class CCommsdat315Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat315Step();
	~CCommsdat315Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat316Step,"commsdat316Step");
class CCommsdat316Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat316Step();
	~CCommsdat316Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat317Step,"commsdat317Step");
class CCommsdat317Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat317Step();
	~CCommsdat317Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat318Step,"commsdat318Step");
class CCommsdat318Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat318Step();
	~CCommsdat318Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat319Step,"commsdat319Step");
class CCommsdat319Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat319Step();
	~CCommsdat319Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat320Step,"commsdat320Step");
class CCommsdat320Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat320Step();
	~CCommsdat320Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat321Step,"commsdat321Step");
class CCommsdat321Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat321Step();
	~CCommsdat321Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat322Step,"commsdat322Step");
class CCommsdat322Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat322Step();
	~CCommsdat322Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat323Step,"commsdat323Step");
class CCommsdat323Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat323Step();
	~CCommsdat323Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat324Step,"commsdat324Step");
class CCommsdat324Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat324Step();
	~CCommsdat324Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat325Step,"commsdat325Step");
class CCommsdat325Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat325Step();
	~CCommsdat325Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat326Step,"commsdat326Step");
class CCommsdat326Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat326Step();
	~CCommsdat326Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat327Step,"commsdat327Step");
class CCommsdat327Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat327Step();
	~CCommsdat327Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat328Step,"commsdat328Step");
class CCommsdat328Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat328Step();
	~CCommsdat328Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat329Step,"commsdat329Step");
class CCommsdat329Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat329Step();
	~CCommsdat329Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat330Step,"commsdat330Step");
class CCommsdat330Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat330Step();
	~CCommsdat330Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat331Step,"commsdat331Step");
class CCommsdat331Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat331Step();
	~CCommsdat331Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3311Step,"commsdat3311Step");
class CCommsdat3311Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3311Step();
	~CCommsdat3311Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3312Step,"commsdat3312Step");
class CCommsdat3312Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3312Step();
	~CCommsdat3312Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3313Step,"commsdat3313Step");
class CCommsdat3313Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3313Step();
	~CCommsdat3313Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3314Step,"commsdat3314Step");
class CCommsdat3314Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3314Step();
	~CCommsdat3314Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3315Step,"commsdat3315Step");
class CCommsdat3315Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3315Step();
	~CCommsdat3315Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3316Step,"commsdat3316Step");
class CCommsdat3316Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3316Step();
	~CCommsdat3316Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3317Step,"commsdat3317Step");
class CCommsdat3317Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3317Step();
	~CCommsdat3317Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3318Step,"commsdat3318Step");
class CCommsdat3318Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3318Step();
	~CCommsdat3318Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};

_LIT(KCommsdat3319Step,"commsdat3319Step");
class CCommsdat3319Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat3319Step();
	~CCommsdat3319Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession_v11;
    CMDBSession* iCDSession_vLatest;
	};


#endif  //__COMMSDAT_SNAP_TESTS_H__

