//
// testltsygetmessagestoreinfo.h
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#ifndef TESTLTSYSMSSTOREGETINFO_H
#define TESTLTSYSMSSTOREGETINFO_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsStoreGetInfo
 *
 */
class CTestLtsySmsStoreGetInfo : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsStoreGetInfo();
	static CTestLtsySmsStoreGetInfo* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsStoreGetInfo* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsStoreGetInfo(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void GetSmsStoreInfoL();
	void CheckSmsStoreInfoL();
private:
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1   iInfo;	
	}; // class CTestLtsySmsStoreGetInfo
_LIT(KCTestLtsySmsStoreGetInfo,"CTestLtsySmsStoreGetInfo");

#endif // TESTLTSYSMSSTOREGETINFO_H
