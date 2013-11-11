// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU3GPLIBRARYAPINEGATIVE_H
#define TSU3GPLIBRARYAPINEGATIVE_H

#include "tsu3gplibraryapi.h"

	
class C3GPLibParser_Negative : public C3GPLibParser_base
	{
public:
	void CallVideoApiL(C3GPParse* aParse, TInt aExpectedError);
	void CallAudioApiL(C3GPParse* aParse, TInt aExpectedError);
	void CallBufferModeApiL(C3GPParse* aParse, TInt aExpectedError);
	void CallCommonApiL(C3GPParse* aParse, TInt aExpectedError);
	void CallVideoDecoderInfoApiL(C3GPParse* aParse, TInt aExpectedError);
	void CallAudioDecoderInfoApiL(C3GPParse* aParse, TInt aExpectedError);
	void CallUDTApiL(C3GPParse* aParse, TUint32 aUdtType, TUint& aIndex, TInt aExpectedError, T3GPUdtaLocation aLocation = E3GPUdtaVideoTrak);
	};
	
_LIT(KTestCase_0101, "MM-3GP-PARSE-U-0101-CP");
class C3GPLibParser_0101 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0101();
	TVerdict doTestStepL();
private:
	void EmptyFileFailedCorruptL(C3GPParse* aParser);
	};

_LIT(KTestCase_0102, "MM-3GP-PARSE-U-0102-CP");
class C3GPLibParser_0102 : public CTestStep
	{
public:
	C3GPLibParser_0102();
	TVerdict doTestStepL();
	};

_LIT(KTestCase_0104, "MM-3GP-PARSE-U-0104-CP");
class C3GPLibParser_0104 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0104();
	TVerdict doTestStepL();
	};

_LIT(KTestCase_0106, "MM-3GP-PARSE-U-0106-CP");
class C3GPLibParser_0106 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0106();
	TVerdict doTestStepL();
private:
	void AllApiFailedNotReadyL(C3GPParse* aParser);
	};

_LIT(KTestCase_0107, "MM-3GP-PARSE-U-0107-CP");
class C3GPLibParser_0107 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0107();
	TVerdict doTestStepL();
private:
	void InvalidFrameTypeL(C3GPParse* aParser);
	};
		
_LIT(KTestCase_0108, "MM-3GP-PARSE-U-0108-CP");
class C3GPLibParser_0108 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0108();
	TVerdict doTestStepL();
private:
	void AllInvalidFrameIndexL(C3GPParse* aParser);
	};

_LIT(KTestCase_0109, "MM-3GP-PARSE-U-0109-CP");
class C3GPLibParser_0109 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0109();
	TVerdict doTestStepL();
private:
	void InvalidFrameTypePanicL(C3GPParse* aParser);
	};

_LIT(KTestCase_0110, "MM-3GP-PARSE-U-0110-CP");
class C3GPLibParser_0110 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0110();
	TVerdict doTestStepL();
private:
	void InvalidUdtLocationL(C3GPParse* aParser);
	};

_LIT(KTestCase_0111, "MM-3GP-PARSE-U-0111-CP");
class C3GPLibParser_0111 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0111();
	TVerdict doTestStepL();
private:
	void AllVideoAudioApiFailedL(C3GPParse* aParser);
	};

_LIT(KTestCase_0119, "MM-3GP-PARSE-U-0119-CP");
class C3GPLibParser_0119 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0119();
	TVerdict doTestStepL();
private:
	void SeekTestL(C3GPParse* aParser);
	};

_LIT(KTestCase_0120, "MM-3GP-PARSE-U-0120-CP");
class C3GPLibParser_0120 : public C3GPLibParser_Negative
	{
public:
	C3GPLibParser_0120();
	TVerdict doTestStepL();
private:
	void VideoAttributesL(C3GPParse* aParser);
	};

_LIT(KTestCase_0126, "MM-3GP-PARSE-U-0126-CP");
class C3GPLibParser_0126 : public CTestStep
	{
public:
	C3GPLibParser_0126();
	TVerdict doTestStepL();
	};	

_LIT(KTestCase_0127, "MM-3GP-PARSE-U-0127-CP");
class C3GPLibParser_0127 : public C3GPLibParser_0014
	{
public:
	C3GPLibParser_0127();
	TVerdict doTestStepL();
private:
	void TestEmptyBufferL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0130, "MM-3GP-PARSE-U-0130-CP");
class C3GPLibParser_0130 : public C3GPLib_AsyncObserver
	{
public:
	C3GPLibParser_0130();
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	void ReadFrameAsyncWoOpenL(C3GPParse* aParser);
private:
	CActiveScheduler* iScheduler;
	};

_LIT(KTestCase_0131, "MM-3GP-PARSE-U-0131-CP");
class C3GPLibParser_0131 : public C3GPLib_AsyncObserver
	{
public:
	C3GPLibParser_0131();
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	void ReadFrameAsyncAfterOpenL(C3GPParse* aParser);
private:
	CActiveScheduler* iScheduler;
	};

_LIT(KTestCase_0135, "MM-3GP-PARSE-U-0135-CP");
class C3GPLibParser_0135 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0135();
	TVerdict doTestStepL();
private:
	void GetFrameTypeNegativeL(C3GPParse* aParser);
	};

_LIT(KTestCase_0136, "MM-3GP-PARSE-U-0136-CP");
class C3GPLibParser_0136 : public C3GPLib_AsyncObserver
	{
public:
	C3GPLibParser_0136();
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
private:
	void ReadFrameAsyncInBufferModeL(C3GPParse* aParser);
	void OpenFileInBufferModeL(C3GPParse& aParser);
	
private:
	CActiveScheduler* iScheduler;
	};

_LIT(KTestCase_0137, "MM-3GP-PARSE-U-0137-CP");
class C3GPLibParser_0137 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0137();
	TVerdict doTestStepL();
private:
	void GetVideoAudioAttributesL(C3GPParse* aParser);
	};

_LIT(KTestCase_0138, "MM-3GP-PARSE-U-0138-CP");
class C3GPLibParser_0138 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0138();
	TVerdict doTestStepL();
private:
	void ReadFrameL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0139, "MM-3GP-PARSE-U-0139-CP");
class C3GPLibParser_0139 : public C3GPLib_AsyncObserver
	{
public:
	C3GPLibParser_0139();
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();	
private:
	void ReadFrameAsyncL(C3GPParse* aParser);
	TInt DoReadFrameAsync(C3GPParse* aParser, TBool aCancel = EFalse);
private:
	CActiveScheduler* iScheduler;
	};	

_LIT(K3GPSingleDataSourceTest, "3GPSingleDataSourceTest");
class C3GPSingleDataSourceTest : public CTestStep
	{
	public:
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();		
		TVerdict doTestStepL();
	
	public:
		C3GPParse* 	iParser;
	};

#endif
	
