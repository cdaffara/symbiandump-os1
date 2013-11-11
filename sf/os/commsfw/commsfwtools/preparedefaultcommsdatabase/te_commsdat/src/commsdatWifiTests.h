
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
// Test of Commsdat Wifi Security tables
// 
//

#if (!defined COMMSDAT_WIFI_TESTS_H)
#define COMMSDAT_WIFI_TESTS_H
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "Te_commsdatSuiteStepBase.h"
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;


_LIT(KCommsdat197Step,"commsdat197Step");
class CCommsdat197Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat197Step();
	~CCommsdat197Step(){};
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat198Step,"commsdat198Step");
class CCommsdat198Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat198Step();
	~CCommsdat198Step(){};
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat199Step,"commsdat199Step");
class CCommsdat199Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat199Step();
	~CCommsdat199Step(){};
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};

_LIT(KCommsdat200Step,"commsdat200Step");
class CCommsdat200Step : public CTe_commsdatSuiteStepBase
	{
public:
	CCommsdat200Step();
	~CCommsdat200Step(){};
	virtual TVerdict doTestStepL();
private:
    CMDBSession* iCDSession;
	};
	
#endif  //COMMSDATWIFITESTS.H

