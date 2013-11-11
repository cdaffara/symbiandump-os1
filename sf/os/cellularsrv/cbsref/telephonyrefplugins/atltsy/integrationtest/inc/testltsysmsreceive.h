//
// testltsysmsreceive.h
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#ifndef TESTLTSYSMSRECEIVE_H
#define TESTLTSYSMSRECEIVE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include
#include "testltsysmsbase.h"
#include "testltsymodel.h"
// CLASS DECLARATION


/**
 * CTestLtsySmsReceive
 *
 */
class CTestLtsySmsReceive : public CTestLtsySmsBase
	{
public:

	~CTestLtsySmsReceive();
	static CTestLtsySmsReceive* NewL(CTestLtsyModel& aTestModel);
	static CTestLtsySmsReceive* NewLC(CTestLtsyModel& aTestModel);
	CTestLtsySmsReceive(CTestLtsyModel& aTestModel);
	void ConstructL();
protected:	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void ReceiveSmsL();
private:

	/**
	*  Receiving message's TPDU
	*/
	TBuf8<KMessageLen> iRecvMsg;
	}; // class CTestLtsySmsReceive
_LIT(KCTestLtsySmsReceive,"CTestLtsySmsReceive");

#endif // TESTLTSYSMSRECEIVE_H
