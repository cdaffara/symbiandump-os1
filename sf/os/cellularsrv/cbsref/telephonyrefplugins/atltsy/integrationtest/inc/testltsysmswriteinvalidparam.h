//
// testltsysmswriteinvalidparam.h
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#ifndef TESTLTSYSMSWRITEINVALIDPARAM_H
#define TESTLTSYSMSWRITEINVALIDPARAM_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsWriteInvalidParam
 *
 */
class CTestLtsySmsWriteInvalidParam : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsWriteInvalidParam();
	static CTestLtsySmsWriteInvalidParam* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsWriteInvalidParam* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsWriteInvalidParam(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void WriteSmsL();
	void WriteSms1L();
	void WriteSms2L();
	void WriteSms3L();
	void WriteSms4L();
private:
	void FillWriteSmsAttibutesL(TBuf8<400> aTpdu,RMobilePhone::TMobileAddress  aMsgSca);
	}; // class CTestLtsySmsWriteInvalidParam
_LIT(KCTestLtsySmsWriteInvalidParam,"CTestLtsySmsWriteInvalidParam");

#endif // TESTLTSYSMSWRITEINVALIDPARAM_H
