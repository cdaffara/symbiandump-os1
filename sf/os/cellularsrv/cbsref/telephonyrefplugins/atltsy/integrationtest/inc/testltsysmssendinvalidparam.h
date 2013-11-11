//
// testltsysmssendinvalidparam.h
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#ifndef TESTLTSYSMSSENDINVALIDPARAM_H
#define TESTLTSYSMSSENDINVALIDPARAM_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION

/**
 * CTestLtsySmsSendInvalidParam
 *
 */
class CTestLtsySmsSendInvalidParam : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsSendInvalidParam();
	static CTestLtsySmsSendInvalidParam* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsSendInvalidParam* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsSendInvalidParam(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void SendSmsL();
	void FillSmsAttibutes(const TDesC& aTelNumber);
	void SendSms1L();
	void FillSmsAttibutes1(const TDesC& aTelNumber);
	void SendSms2L();
	void FillSmsAttibutes2(const TDesC& aTelNumber);
private:
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 iSmsAttr;
	}; // class CTestLtsySmsSendInvalidParam
_LIT(KCTestLtsySmsSendInvalidParam,"CTestLtsySmsSendInvalidParam");

#endif // TESTLTSYSMSSENDINVALIDPARAM_H
