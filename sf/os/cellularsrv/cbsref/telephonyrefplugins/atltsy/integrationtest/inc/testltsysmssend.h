//
// testltsysmssend.h
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#ifndef TESTLTSYSMSSEND_H
#define TESTLTSYSMSSEND_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsSend
 *
 */
class CTestLtsySmsSend : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsSend();
	static CTestLtsySmsSend* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsSend* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsSend(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void SendSmsL();
	void FillSmsAttibutes(const TDesC& aTelNumber);
private:
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 iSmsAttr;

	}; // class CTestLtsySmsSend
_LIT(KCTestLtsySmsSend,"CTestLtsySmsSend");

#endif // TESTLTSYSMSSEND_H
