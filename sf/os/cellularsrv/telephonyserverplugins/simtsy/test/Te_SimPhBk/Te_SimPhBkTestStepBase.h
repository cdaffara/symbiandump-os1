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
//

#ifndef __TE_SIMPHBK_TESTSTEP_BASE__
#define __TE_SIMPHBK_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <mpbutil.h>
#include "SimTsy.h"
#include "mmretrieve.h"

_LIT(KSimtsyName,"SIM");

#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (TestStepResult() != EPass ){\
		INFO_PRINTF1(_L("Failed Checkpoint:"));\
		INFO_PRINTF1(msg);\
		}\
	}

class CSimPhBkTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	static void CleanUp(TAny *aPtr) ;

protected:
	void GetGoodNumber(TDes& aNum) ;
	TInt ClearPhoneBookL(const TDesC& aPhoneBookName) ;
	void CheckPhoneBookInfoL(RMobilePhoneStore::TMobilePhoneStoreInfoV1& aInfo) ;
	void WriteBatchOfEntriesToThePhoneL() ;
	// TCleanupItem operator for objects of this class
	operator TCleanupItem() ;

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMobileLine iLine;
	RMobileCall iCall;
	TBool iMESupported;
	TBool iMTSupported;
	};

#endif // __TE_SIMPHBK_TESTSTEP_BASE__
