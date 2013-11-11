//
// testltsysmswrite.h
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#ifndef TESTLTSYSMSWRITE_H
#define TESTLTSYSMSWRITE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsWrite
 *
 */
class CTestLtsySmsWrite : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsWrite();
	static CTestLtsySmsWrite* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsWrite* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsWrite(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void WriteSmsL();
private:

	void FillWriteSmsAttibutesL(TBuf8<400> aTpdu,RMobilePhone::TMobileAddress  aMsgSca);
	}; // class CTestLtsySmsWrite
_LIT(KCTestLtsySmsWrite,"CTestLtsySmsWrite");

#endif // TESTLTSYSMSWRITE_H
