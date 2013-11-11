// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file testltsygetmessagestoreinfo.h
// 

#ifndef TESTLTSYGETMESSAGESTOREINFO_H
#define TESTLTSYGETMESSAGESTOREINFO_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsyGetMessageStoreInfo
 *
 */
class CTestLtsyGetMessageStoreInfo : public CTestLtsySmsBase
	{
public:

	~CTestLtsyGetMessageStoreInfo();
	static CTestLtsyGetMessageStoreInfo* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsyGetMessageStoreInfo* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsyGetMessageStoreInfo(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void GetMessageStoreInfoL();
	void CheckMessageStoreInfoL();
private:
   	RMobilePhoneStore::TMobilePhoneStoreInfoV1       iInfo;	

	}; // class CTestLtsyGetMessageStoreInfo
_LIT(KCTestLtsyGetMessageStoreInfo,"CTestLtsyGetMessageStoreInfo");

#endif // TESTLTSYGETMESSAGESTOREINFO_H
