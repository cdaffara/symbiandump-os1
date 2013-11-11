// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>
#include <bitdev.h>	// for CFbsScreenDevice

#include <ecom/ecom.h>

// Test system includes
#include <testframework.h>
#include "TSI_MmfRecog.h"

#include "../../../../MMF/src/Recognizer/RecMmf.h"

// Added since we implement MApaAppStarter, which has been future-proofed.
EXPORT_C MApaAppStarter::MApaAppStarter()
	{
	}

EXPORT_C void MApaAppStarter::MApaAppStarter_Reserved1()
	{
	}
	
EXPORT_C void MApaAppStarter::MApaAppStarter_Reserved2()
	{
	}	

/**
 * Set the test step result
 * @param	"TVerdict aVerdict"
 *			Set the test step result to aVerdict if it is worse than the current result
 */
void RTestStepCompare::SetVerdict(TVerdict aVerdict)
	{
	if(aVerdict>iTestStepResult)
		iTestStepResult = aVerdict;
	}

/**
 * Compare two integers and set the test step result to EFail if they do not match
 * @param	"TInt aActual"
 *			The value to test
 * @param	"TInt aExpect"
 *			What aActual is expected to be
 * @param	"const TDesC& aErrorMessage"
 *			An error message if the values do not match.
 */
void RTestStepCompare::Compare(TInt aActual, TInt aExpect, const TDesC& aErrorMessage)
	{
	if(aActual!=aExpect)
		{
		ERR_PRINTF1(aErrorMessage);
		ERR_PRINTF3(_L("Value is:%d Expected:%d"),aActual,aExpect);
		SetVerdict(EFail);
		}
	}

/**
 * Compare two UID's and set the test step result to EFail if they do not match
 * @param	"TUid aActual"
 *			The value to test
 * @param	"TUid aExpect"
 *			What aActual is expected to be
 * @param	"const TDesC& aErrorMessage"
 *			An error message if the values do not match.
 */
void RTestStepCompare::Compare(TUid aActual, TUid aExpect, const TDesC& aErrorMessage)
	{
	if(aActual!=aExpect)
		{
		ERR_PRINTF1(aErrorMessage);
		ERR_PRINTF3(_L("Value is:%X Expected:%X"),aActual.iUid,aExpect.iUid);
		SetVerdict(EFail);
		}
	}

/**
 * Compare two descriptors and set the test step result to EFail if they do not match
 * @param	"const TDesC& aActual"
 *			The descriptor to test
 * @param	"const TDesC& aExpect"
 *			What aActual is expected to be
 * @param	"const TDesC& aErrorMessage"
 *			An error message if the values do not match.
 */
void RTestStepCompare::Compare(const TDesC& aActual, const TDesC& aExpect, const TDesC& aErrorMessage)
	{
	if(aActual.Ptr()==aExpect.Ptr())
		{
		ERR_PRINTF1(_L("Comparing same pointer!"));
		SetVerdict(EFail);
		}

	if(aActual!=aExpect)
		{
		ERR_PRINTF1(aErrorMessage);
		ERR_PRINTF3(_L("Value is:[%S] Expected:[%S]"),&aActual,&aExpect);
		SetVerdict(EFail);
		}
	}

/**
 * Compare two descriptors and set the test step result to EFail if they do not match
 * @param	"const TDesC8& aActual"
 *			The descriptor to test
 * @param	"const TDesC8& aExpect"
 *			What aActual is expected to be
 * @param	"const TDesC& aErrorMessage"
 *			An error message if the values do not match.
 */
void RTestStepCompare::Compare(const TDesC8& aActual, const TDesC8& aExpect, const TDesC& aErrorMessage)
	{
	if(aActual.Ptr()==aExpect.Ptr())
		{
		ERR_PRINTF1(_L("Comparing same pointer!"));
		SetVerdict(EFail);
		}

	if(aActual!=aExpect)
		{
		ERR_PRINTF1(aErrorMessage);
		TInt err1;
		TInt err2;

		HBufC* actual = NULL; 
		TRAP(err1, actual = HBufC::NewL(aActual.Length()));
		HBufC* expect = NULL;
		TRAP(err2, expect = HBufC::NewL(aExpect.Length()));

		//if we could not allocate memory simply ignore
		if((err1 == KErrNone) && (err2 == KErrNone))
			{
			actual->Des().Copy(aActual);
			expect->Des().Copy(aExpect);
			ERR_PRINTF3(_L("Value is:[%S] Expected:[%S]"),actual,expect);
			}
		delete actual; actual = NULL;
		delete expect; expect = NULL;

		SetVerdict(EFail);
		}
	}
	
// dummy Hal call for heap balance on target
// Need to do this BEFORE doing any allocation testing !	
void RTestStepCompare::DummyHalCallL()
	{
	TInt err = KErrNone;
	TInt displayMode = EColor256;
	CFbsScreenDevice* screenDevice = NULL;	
	while(displayMode < EColorLast) 
		{
		TRAP(err, screenDevice = CFbsScreenDevice::NewL(_L("NotUsed"),(TDisplayMode)displayMode));
		delete screenDevice;
		screenDevice = NULL;	
		if(err == KErrNone)
			{
			break;
			}
		displayMode++;
		}			
	User::LeaveIfError(err);
	}


/**
 * Constructor for TTestFileInfo helper class
 *
 * @xxxx
 */
RTestMmfRecog::TTestFileInfo::TTestFileInfo(const TDesC& aFileName, const TDesC8& aMimeType) :
	iFileName(aFileName),
	iMimeType(aMimeType)
	{
	}

RTestMmfRecog::RTestMmfRecog(const TDesC& aTestName) 
	{
	iTestStepName = aTestName;
	// need to up the default heap size to handle the Application Architecture Server
	// Start App server
	iHeapSize = 0x40000;	
	}


RTestMmfRecog* RTestMmfRecog::NewL(const TDesC& aTestName)
	{
	RTestMmfRecog* self = new(ELeave) RTestMmfRecog(aTestName);
	return self;
	}


_LIT(KWavFileName, "\\AclntITestData\\test.wav");
_LIT8(KWavMimeType,"audio/wav");	

_LIT(KAuFileName, "\\AclntITestData\\test16m2.au");
_LIT8(KAuMimeType,"audio/basic");	

TVerdict RTestMmfRecog::OpenL() 
	{

	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	// Connect to bitmap server
	User::LeaveIfError(RFbsSession::Connect());

	// test.wav
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KWavFileName, KWavMimeType)));

	// Au
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KAuFileName, KAuMimeType)));
	
	return iTestStepResult = EPass;
	}

void RTestMmfRecog::Close() 
	{
	iPluginData.Close();

	iFs.Close();
	
	RFbsSession::Disconnect();

//	delete iScheduler;
	}

/** 
 *
 * Retrieve MIMEType by recognizer API & verify against expected result
 *
 * @param		"TTestFileInfo& aCodecData"
 *
 */
void RTestMmfRecog::VerifyMimeTypesL(const TTestFileInfo& aCodecData)
	{
	TBuf8<KMaxDataTypeLength> mimeType;

// code to access the MMF recognizer DLL via RApaLsSession::AppForDocument()
	TInt ret;

	RApaLsSession myApaLsSession;
	User::LeaveIfError(myApaLsSession.Connect());
	CleanupClosePushL(myApaLsSession);
	
	TUid uid;
	uid.iUid = 0;
	TDataType dataType;
	ret = myApaLsSession.AppForDocument(aCodecData.iFileName, uid, dataType);

	TBuf16<KMaxDataTypeLength> strReturned;
	TBuf16<KMaxDataTypeLength> strExpected;
	strReturned = dataType.Des();
	strExpected.Copy(aCodecData.iMimeType);
	INFO_PRINTF4(_L("    Expected Mime type  \"%S\", returned MIME type = \"%S\", returned app UID = %X)"), &strExpected, &strReturned, uid);
	
	if (ret == KErrNone)
		{
		// get a local copy of the the mime type for Compare()...
		mimeType = dataType.Des8();
		}
	else
		{
		INFO_PRINTF1(_L("    Recognizer failed to recognize file type"));
		User::Leave(KErrNotFound	);
		}


	CleanupStack::PopAndDestroy(1,&myApaLsSession); // myApaLsSession


	Compare(mimeType,aCodecData.iMimeType,_L("MIME Type incorrect"));

	}

TVerdict RTestMmfRecog::DoTestStepL()
	{
	DummyHalCallL();

	__MM_HEAP_MARK;
	TInt noOfPlugins = iPluginData.Count();
	TInt error = KErrNone;

	for(TInt pluginNo = 0; pluginNo < noOfPlugins; pluginNo++)
		{
		INFO_PRINTF2(_L("%S"),&iPluginData[pluginNo].iFileName);
		TRAP(error,VerifyMimeTypesL(iPluginData[pluginNo]));
		Compare(error,KErrNone,_L("Unexpected error code"));
		}

	__MM_HEAP_MARKEND;

	return iTestStepResult;

	}
	
// RTestMmfFormat
/**
 * Constructor for TTestFileInfo helper class
 *
 * @xxxx
 */
RTestMmfFormat::TTestFileInfo::TTestFileInfo(const TDesC& aFileName, const TDesC8& aMimeType) :
	iFileName(aFileName),
	iMimeType(aMimeType)
	{
	}

RTestMmfFormat::RTestMmfFormat(const TDesC& aTestName) 
	{
	iTestStepName = aTestName;
	// need to up the default heap size to handle the Application Architecture Server
	// Start App server
	iHeapSize = 0x40000;	
	}


RTestMmfFormat* RTestMmfFormat::NewL(const TDesC& aTestName)
	{
	RTestMmfFormat* self = new(ELeave) RTestMmfFormat(aTestName);
	return self;
	}


_LIT(KFileName1, "\\AclntITestData\\Test.abc");
_LIT8(KMimeType1,"");	

_LIT(KFileName2, "\\AclntITestData\\Test.xyz");
_LIT8(KMimeType2,"");	

_LIT(KSmsLongData, "sms:+91-9444250553,+91-9840080265,+91-9884309532,+91-9884309533,+91-9884309534,+91-9884309535,+91-9884309536?body=SMS messages can provide an alternative to a mailto URIs or tel or fax URIs. When a sms URI is activated, the user agent MAY start a program for sending an SMS message, just as mailto may open a mail client");
_LIT8(KUrlSmsMimeType,"X-Epoc-Url/sms");	

TVerdict RTestMmfFormat::OpenL() 
	{

	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	// Connect to bitmap server
	User::LeaveIfError(RFbsSession::Connect());

	// test.abc
	TTestFileInfo temp1(KFileName1, KMimeType1);
	User::LeaveIfError(iPluginData.Append(temp1));

	// test.xyz
	TTestFileInfo temp2(KFileName2, KMimeType2);
	User::LeaveIfError(iPluginData.Append(temp2));
	
	// ensure the recognizer copes with data longer than KMaxFileName
	TTestFileInfo temp3(KSmsLongData, KUrlSmsMimeType);
	User::LeaveIfError(iPluginData.Append(temp3));

	return iTestStepResult = EPass;
	}

void RTestMmfFormat::Close() 
	{
	iPluginData.Close();

	iFs.Close();
	
	RFbsSession::Disconnect();

	}

/** 
 *
 * Retrieve MIMEType by recognizer API & verify against expected result
 *
 * @param		"TTestFileInfo& aCodecData"
 *
 */
void RTestMmfFormat::VerifyMimeTypesL(const TTestFileInfo& aCodecData)
	{
	TBuf8<KMaxDataTypeLength> mimeType;

// code to access the MMF recognizer DLL via RApaLsSession::AppForDocument()
	TInt ret;

	RApaLsSession myApaLsSession;
	User::LeaveIfError(myApaLsSession.Connect());
	CleanupClosePushL(myApaLsSession);
	
	TUid uid;
	uid.iUid = 0;
	TDataType dataType;
	ret = myApaLsSession.AppForDocument(aCodecData.iFileName, uid, dataType);

	TBuf16<KMaxDataTypeLength> strReturned;
	TBuf16<KMaxDataTypeLength> strExpected;
	strReturned = dataType.Des();
	strExpected.Copy(aCodecData.iMimeType);
	INFO_PRINTF4(_L("    Expected Mime type  \"%S\", returned MIME type = \"%S\", returned app UID = %X)"), &strExpected, &strReturned, uid);
	
	if (ret == KErrNone)
		{
		// get a local copy of the the mime type for Compare()...
		mimeType = dataType.Des8();
		}
	else
		{
		INFO_PRINTF1(_L("    Recognizer failed to recognize file type"));
		User::Leave(KErrNotFound	);
		}


	CleanupStack::PopAndDestroy(1,&myApaLsSession); // myApaLsSession


	Compare(mimeType,aCodecData.iMimeType,_L("MIME Type incorrect"));

	}

TVerdict RTestMmfFormat::DoTestStepL()
	{
	DummyHalCallL();

	__MM_HEAP_MARK;
	TInt noOfPlugins = iPluginData.Count();
	TInt error = KErrNone;

	for(TInt pluginNo = 0; pluginNo < noOfPlugins; pluginNo++)
		{
		if (iPluginData[pluginNo].iFileName.Length() <= KMaxLogLineLength)
			{
			INFO_PRINTF2(_L("%S"),&iPluginData[pluginNo].iFileName);
			INFO_PRINTF2(_L("Verifying the MIME type of the file %S"),&iPluginData[pluginNo].iFileName);
			}
		else
			{
			INFO_PRINTF2(_L("Verifying the MIME type of data with length > %d"), KMaxLogLineLength);
			}
		TRAP(error,VerifyMimeTypesL(iPluginData[pluginNo]));
		Compare(error,KErrNone,_L("Unexpected error code"));
		}

	__MM_HEAP_MARKEND;

	return iTestStepResult;

	}

// from MMdaObjectStateChangeObserver
void RTestMmfFormat::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/)
	{
	CActiveScheduler::Stop();
	}

//
// CRecogWrapper - wrap opening of recognizer - either ecom or .mdl
//

class CRecogWrapper : public CBase
	{
public:
	static CRecogWrapper* NewLC(CLog* aLog, TBool aAssumeOOM=EFalse);
	~CRecogWrapper();
	CApaMmfRecognizer* Recognizer() { return iRecognizer; }
private:
	CRecogWrapper(CLog* aLog, TBool aAssumeOOM);
	void ConstructL();
private:
	CApaMmfRecognizer* iRecognizer;
	CLog*const iLog;
	const TBool iAssumeOOM;
	TUid iDtorKey;
	};
	
CRecogWrapper::CRecogWrapper(CLog* aLog, TBool aAssumeOOM):
	iLog(aLog),
	iAssumeOOM(aAssumeOOM)
	{
	}
	
CRecogWrapper* CRecogWrapper::NewLC(CLog* aLog, TBool aAssumeOOM)
	{
	CRecogWrapper* result = new (ELeave) CRecogWrapper(aLog, aAssumeOOM);
	CleanupStack::PushL(result);
	result->ConstructL();
	return result;
	}
	
void CRecogWrapper::ConstructL()
	{
	const TUid KMmfRecImp = {0x101F7C41};
	
	iRecognizer = static_cast<CApaMmfRecognizer*>(
					 reinterpret_cast<CApaDataRecognizerType*>(
						REComSession::CreateImplementationL(KMmfRecImp, iDtorKey)));
	}
	
CRecogWrapper::~CRecogWrapper()
	{
	delete iRecognizer;
	REComSession::DestroyedImplementation(iDtorKey);
//	REComSession::Close();
	}
	
//
/*
 * Constructor for TTestFileInfo helper class
 *
 * @xxxx
 */
RTestMmfRecogData::TTestFileInfo::TTestFileInfo(const TDesC& aFileName, const TDesC& aMimeType, const CApaDataRecognizerType::TRecognitionConfidence aConfidence) :
	iFileName(aFileName),
	iMimeType(aMimeType),
	iConfidence(aConfidence)
	{
	}

RTestMmfRecogData::RTestMmfRecogData(const TDesC& aTestName) 
	{
	iTestStepName = aTestName;
	// need to up the default heap size to handle the Application Architecture Server
	// Start App server
	iHeapSize = 0x40000;	
	}


RTestMmfRecogData* RTestMmfRecogData::NewL(const TDesC& aTestName)
	{
	RTestMmfRecogData* self = new(ELeave) RTestMmfRecogData(aTestName);
	return self;
	}


_LIT(KWavWavDataFile,"\\AclntITestData\\recog_wav.wav");	//wav file - correct suffix
_LIT(KWavAuDataFile, "\\AclntITestData\\recog_wav.au");		//wav file - incorrect suffix
_LIT(KWavDataFile,	 "\\AclntITestData\\recog_wav");		//wav file - no suffix
_LIT(KAuAuDataFile,  "\\AclntITestData\\recog_au.au");		//au  file - correct suffix 
_LIT(KAuWavDataFile, "\\AclntITestData\\recog_au.wav");		//au  file - incorrect suffix  
_LIT(KRawWavDataFile,"\\AclntITestData\\recog_raw.wav");	//raw file - incorrect suffix

_LIT(KWavMimeDataType,"audio/wav");	
_LIT(KAuMimeDataType,"audio/basic");	

const CApaDataRecognizerType::TRecognitionConfidence KConfidenceProbable = CApaDataRecognizerType::EProbable;
const CApaDataRecognizerType::TRecognitionConfidence KConfidencePossible = CApaDataRecognizerType::EPossible;
const CApaDataRecognizerType::TRecognitionConfidence KConfidenceNotRecognized = CApaDataRecognizerType::ENotRecognized;

TVerdict RTestMmfRecogData::OpenL() 
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );

	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	// Connect to bitmap server
	User::LeaveIfError(RFbsSession::Connect());

	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KWavWavDataFile, KWavMimeDataType,KConfidenceProbable)));
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KWavAuDataFile, KWavMimeDataType,KConfidencePossible)));
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KWavDataFile, KWavMimeDataType,KConfidencePossible)));
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KAuAuDataFile, KAuMimeDataType,KConfidenceProbable)));
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KAuWavDataFile, KAuMimeDataType,KConfidencePossible)));
	User::LeaveIfError(iPluginData.Append(TTestFileInfo (KRawWavDataFile, KNullDesC(),KConfidenceNotRecognized)));
	
	return iTestStepResult = EPass;
	}

void RTestMmfRecogData::Close() 
	{
	iPluginData.Close();
	iFs.Close();
	RFbsSession::Disconnect();
	delete iScheduler; iScheduler = NULL;
	}

/** 
 *
 * Retrieve MIMEType by recognizer API & verify against expected result
 *
 * @param		"TTestFileInfo& aCodecData"
 *
 */
void RTestMmfRecogData::VerifyDataL(const TTestFileInfo& aCodecData)
	{
	CRecogWrapper* wrapper = CRecogWrapper::NewLC(iSuite->LogSystem());
			
	CApaMmfRecognizer* recognizer = wrapper->Recognizer();

	TUint bufSize = recognizer->PreferredBufSize();

	RFile file;
	User::LeaveIfError(file.Open(iFs, aCodecData.iFileName, EFileRead|EFileShareAny));
	CleanupClosePushL(file);
	
	HBufC8* imageData = HBufC8::NewLC(bufSize);
	TPtr8 imageDataPtr(imageData->Des());
	User::LeaveIfError(file.Read(imageDataPtr));

	// pad the data out otherwise the recognizer won't even look at it
    imageDataPtr.SetLength(bufSize);	

	recognizer->DoRecognizeL(aCodecData.iFileName, imageDataPtr);
	
	const TBuf<KMaxDataTypeLength> mimeType(recognizer->MimeType().Des());
	const TInt confidence = recognizer->Confidence();

	INFO_PRINTF3(_L("Confidence: Expected [%d] Got [%d]"),aCodecData.iConfidence,confidence);
	Compare(confidence,aCodecData.iConfidence,_L("Confidence not as expected"));

	INFO_PRINTF3(_L("MimeType: Expected [%S] Got [%S]"),&aCodecData.iMimeType,&mimeType);
	Compare(mimeType,aCodecData.iMimeType,_L("MIME type not as expected"));

	CleanupStack::PopAndDestroy(3,wrapper);		// imageData, file, wrapper
	}

TVerdict RTestMmfRecogData::DoTestStepL()
	{
	DummyHalCallL();

	__MM_HEAP_MARK;
	TInt noOfPlugins = iPluginData.Count();
	TInt error = KErrNone;

	for(TInt pluginNo = 0; pluginNo < noOfPlugins; pluginNo++)
		{
		INFO_PRINTF1(_L(""));
		INFO_PRINTF2(_L("%S"),&iPluginData[pluginNo].iFileName);
		TRAP(error,VerifyDataL(iPluginData[pluginNo]));
		Compare(error,KErrNone,_L("Unexpected error code"));
		}

	__MM_HEAP_MARKEND;

	return iTestStepResult;

	}
//

// negative test
RTestMmfRecogNeg::RTestMmfRecogNeg(const TDesC& aTestName) 
	{
	iTestStepName = aTestName;
	// need to up the default heap size to handle the Application Architecture Server
	// Start App server
	iHeapSize = 0x40000;	
	}


RTestMmfRecogNeg* RTestMmfRecogNeg::NewL(const TDesC& aTestName)
	{
	RTestMmfRecogNeg* self = new(ELeave) RTestMmfRecogNeg(aTestName);
	return self;
	}

_LIT(KRawFileName, "\\AclntITestData\\Test.xyz");

 TVerdict RTestMmfRecogNeg::OpenL() 
	{

	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	// Connect to bitmap server
	User::LeaveIfError(RFbsSession::Connect());
	
	return iTestStepResult = EPass;
	}

void RTestMmfRecogNeg::Close() 
	{
	iFs.Close();
	RFbsSession::Disconnect();
	}

/** 
 *
 * Retrieve MIMEType by recognizer API & verify against expected result
 *
 * @param		"TTestFileInfo& aCodecData"
 *
 */
void RTestMmfRecogNeg::VerifyMimeTypesL(const TDesC& aFileName)
	{
	TInt ret;

	// code to access the MMF recognizer DLL via RApaLsSession::AppForDocument()
	RApaLsSession myApaLsSession;
	User::LeaveIfError(myApaLsSession.Connect());
	CleanupClosePushL(myApaLsSession);
	
	TUid uid;
	uid.iUid = 0;
	TDataType dataType;
	ret = myApaLsSession.AppForDocument(aFileName, uid, dataType);

	if ((ret != KErrNone) || (dataType.Des8().Length() == 0))
		{
		INFO_PRINTF1(_L("    Recognizer (correctly) failed to recognize file type"));
		}
	else
		{
		INFO_PRINTF1(_L("    Recognizer recognizes file type !"));
		TBuf16<KMaxDataTypeLength> strReturned;
		strReturned = dataType.Des();
		INFO_PRINTF3(_L("    returned MIME type = \"%S\", returned app UID = %X)"), &strReturned, uid);
		User::Leave(KErrGeneral);
		}


	CleanupStack::PopAndDestroy(1,&myApaLsSession); // myApaLsSession
	}

TVerdict RTestMmfRecogNeg::DoTestStepL()
	{
	DummyHalCallL();

	__MM_HEAP_MARK;
	TInt error = KErrNone;

	INFO_PRINTF2(_L("%S"),&KRawFileName);
	TRAP(error,VerifyMimeTypesL(KRawFileName));
	Compare(error,KErrNone,_L("Unexpected error code"));

	__MM_HEAP_MARKEND;

	return iTestStepResult;

	}

// Alloc test

RTestMmfRecogAlloc::RTestMmfRecogAlloc(
	const TDesC& aTestName) 
	{
	iTestStepName = aTestName;

	// need to up the default heap size to handle the Application Architecture Server
	// Start App server
	iHeapSize = 0x40000;	
	}


RTestMmfRecogAlloc* RTestMmfRecogAlloc::NewL(const TDesC& aTestName)
	{
	RTestMmfRecogAlloc* self = new(ELeave) RTestMmfRecogAlloc(aTestName);
	return self;
	}

TVerdict RTestMmfRecogAlloc::OpenL() 
	{

	iScheduler = new(ELeave)CActiveScheduler;
    //[ push the scheduler on the stack ]
	CleanupStack::PushL( iScheduler );
	
	//[install the active scheduler ]
	CActiveScheduler::Install( iScheduler );
	
	// [Pop iScheduler ] 
	CleanupStack::Pop( iScheduler);
	
	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	iFileName.Set(KWavFileName);
	iMimeType.Set(KWavMimeType);

	return iTestStepResult = EPass;
	}

void RTestMmfRecogAlloc::Close() 
	{
	iFs.Close();
	
	//Destroy the scheduler
	delete iScheduler ;
	iScheduler = NULL;
	
	}

/** 
 *
 * Retrieve MIMEType by recognizer API & verify against expected result
 *
 * @param		"TTestFileInfo& aCodecData"
 *
 */
void RTestMmfRecogAlloc::VerifyMimeTypesL(
	const TDesC& aFileName, 
	TDes8& aMimeType,
	TInt& aConfidenceLevel)
	{
	
	// code to access the MMF recognizer DLL directly
	CRecogWrapper* wrapper = CRecogWrapper::NewLC(iSuite->LogSystem(), ETrue);
	CApaMmfRecognizer* myRecog= wrapper->Recognizer();

	TUint bufSize = myRecog->PreferredBufSize();
	
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead|EFileShareAny));	
	CleanupClosePushL(file);
	
	HBufC8* imageData = HBufC8::NewLC(bufSize);
	TPtr8 imageDataPtr(imageData->Des());
	User::LeaveIfError(file.Read(imageDataPtr));

	// call some extra function for good measure
	myRecog->SupportedDataTypeL(0);

	myRecog->DoRecognizeL(aFileName, imageDataPtr);
	
	// return the mime type and the confidence level to the caller
	aMimeType = myRecog->MimeType().Des8();
	aConfidenceLevel = myRecog->Confidence();

	CleanupStack::PopAndDestroy(3, wrapper);		// wrapper, file, imageData
	}

TVerdict RTestMmfRecogAlloc::DoTestStepL()
	{
	TBool testComplete = EFalse;
	TBuf8<KMaxDataTypeLength> mimeType;
	TInt confidenceLevel = CApaDataRecognizerType::ENotRecognized;

	INFO_PRINTF2(_L("%S"),&KWavFileName);

	for (TInt failCount = 1; !testComplete; failCount++)
		{
		TInt err = 0;
		__UHEAP_FAILNEXT(failCount);
		__MM_HEAP_MARK;

		TRAP(err, VerifyMimeTypesL(iFileName, mimeType, confidenceLevel));

		switch (err)
			{
			case KErrNoMemory :
				break;
			case KErrNone :
				{
				//attempt to allocate another cell. If this fails, we have tested all 
				//of the memory allocs carried out by NewL.
				TAny *testAlloc = User::Alloc(1);
				if (testAlloc==NULL)
					{
					testComplete = ETrue;
					failCount --;
					break;
					}
				User::Free(testAlloc);
				INFO_PRINTF1(_L("Test consumed error code!"));
				break;
				}
			default:
				{
				testComplete = ETrue;
				iTestStepResult = EFail;
				break;
				}
			}
		
		__MM_HEAP_MARKEND;
		__UHEAP_RESET;
		}

	if (confidenceLevel <= CApaDataRecognizerType::EUnlikely)
		{
		INFO_PRINTF1(_L("    - recognizer: failed to recognize file type -"));
		TBuf16<KMaxDataTypeLength> strReturned;
		TBuf16<KMaxDataTypeLength> strExpected;
		strReturned.Copy(mimeType);
		strExpected.Copy(iMimeType);
		INFO_PRINTF3(_L("Mime type not the same (returned = \"%S\", expected = \"%S\")"), &strReturned, &strExpected);

		User::Leave(KErrGeneral);
		}

	return iTestStepResult;

	}

//
// RTestMmfRecogNegDirect

_LIT(KImageFileNameA, "\\AclntITestData\\a.txt");
_LIT(KImageFileNameB, "\\AclntITestData\\b.txt");
_LIT(KImageFileNameG, "\\AclntITestData\\g.txt");
_LIT(KImageFileNameI, "\\AclntITestData\\i.txt");
_LIT(KImageFileNameM, "\\AclntITestData\\m.txt");
_LIT(KImageFileNameR, "\\AclntITestData\\r.txt");
_LIT(KImageFileNameGif, "\\AclntITestData\\gif.txt");


RTestMmfRecogNegDirect* RTestMmfRecogNegDirect::NewL(const TDesC& aTestName)
	{
	RTestMmfRecogNegDirect* self = new(ELeave) RTestMmfRecogNegDirect(aTestName);
	return self;
	}

RTestMmfRecogNegDirect::RTestMmfRecogNegDirect(const TDesC& aTestName) 
	{
	iTestStepName = aTestName;

	}

TVerdict RTestMmfRecogNegDirect::OpenL()
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );

	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	// Connect to bitmap server
	User::LeaveIfError(RFbsSession::Connect());

	iFileNames.Append(&KImageFileNameA);
    iFileNames.Append(&KImageFileNameB);
	iFileNames.Append(&KImageFileNameG);
	iFileNames.Append(&KImageFileNameI);
	iFileNames.Append(&KImageFileNameM);
	iFileNames.Append(&KImageFileNameR);
	iFileNames.Append(&KImageFileNameGif);

    return iTestStepResult;
	}

void RTestMmfRecogNegDirect::Close()
	{
	iFileNames.Reset();
	iFs.Close();
	RFbsSession::Disconnect();
	delete iScheduler;
	}

/**
 * 
 * Open images, retrieve frame info & verify
 * 
 */
TVerdict RTestMmfRecogNegDirect::DoTestStepL()
	{
	DummyHalCallL();

	__MM_HEAP_MARK;
	TInt noOfPlugins = iFileNames.Count();
	TInt error = KErrNone;


	for(TInt pluginNo = 0; pluginNo < noOfPlugins; pluginNo++)
		{
		INFO_PRINTF2(_L("%S"),iFileNames[pluginNo]);
		TRAP(error,VerifyMimeTypesL(*iFileNames[pluginNo]));
		Compare(error,KErrNone,_L("Unexpected error code"));
		}
	__MM_HEAP_MARKEND;

	return iTestStepResult;
	} 



void RTestMmfRecogNegDirect::VerifyMimeTypesL(const TDesC& aFileName)
    {
	// code to access the recognizer DLL directly
	CRecogWrapper* wrapper = CRecogWrapper::NewLC(iSuite->LogSystem());
	CApaMmfRecognizer* myRecog= wrapper->Recognizer();

	TUint bufSize = myRecog->PreferredBufSize();

	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead|EFileShareAny));
	CleanupClosePushL(file);
	
	HBufC8* imageData = HBufC8::NewLC(bufSize);
	TPtr8 imageDataPtr(imageData->Des());
	User::LeaveIfError(file.Read(imageDataPtr));

	// pad the data out otherwise the recognizer won't even look at it
    imageDataPtr.SetLength(bufSize);	

	myRecog->DoRecognizeL(aFileName, imageDataPtr);
	
	if (myRecog->Confidence() != CApaDataRecognizerType::ENotRecognized)
		{
		INFO_PRINTF1(_L("    - recognizer: incorrectly recognized file type -"));
		TBuf16<KMaxDataTypeLength> strReturned;
		strReturned.Copy(myRecog->MimeType().Des8());
    	INFO_PRINTF2(_L("Mime type returned = \"%S\")"), &strReturned);
		User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(3, wrapper);		// wrapper, file, imageData
    }
//

// file handle test
RTestMmfRecogTestFileHandle::RTestMmfRecogTestFileHandle(const TDesC& aTestName) 
	{
	iTestStepName = aTestName;
	// need to up the default heap size to handle the Application Architecture Server
	// Start App server
	iHeapSize = 0x40000;	
	}


RTestMmfRecogTestFileHandle* RTestMmfRecogTestFileHandle::NewL(const TDesC& aTestName)
	{
	RTestMmfRecogTestFileHandle* self = new(ELeave) RTestMmfRecogTestFileHandle(aTestName);
	return self;
	}

_LIT(KFileName, "\\AclntITestData\\recog_wav.wav");

 TVerdict RTestMmfRecogTestFileHandle::OpenL() 
	{

	// Connect to file server
	User::LeaveIfError(iFs.Connect());

	// Connect to bitmap server
	User::LeaveIfError(RFbsSession::Connect());
	
	return iTestStepResult = EPass;
	}

void RTestMmfRecogTestFileHandle::Close() 
	{
	iFs.Close();
	RFbsSession::Disconnect();
	}

/** 
 *
 * Retrieve MIMEType by recognizer API & verify against expected result
 *
 * @param		"TTestFileInfo& aCodecData"
 *
 */
void RTestMmfRecogTestFileHandle::VerifyMimeTypesL(const TDesC& aFileName)
	{
	TInt ret;
	// code to access the MMF recognizer DLL via RApaLsSession::AppForDocument()
	RApaLsSession myApaLsSession;
	User::LeaveIfError(myApaLsSession.Connect());
	CleanupClosePushL(myApaLsSession);
	TUid uid;
	uid.iUid = 0;
	TDataType dataType;
	RFile recFile;
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(recFile.Open(iFs, aFileName, EFileRead|EFileShareAny));
	CleanupClosePushL(recFile);
	ret = myApaLsSession.AppForDocument(recFile, uid, dataType);//here we call the GetMimeTypeL() by passin a file handle
	TBuf16<KMaxDataTypeLength> strReturned;
	_LIT(KDataType1,"audio/x-wav");
	_LIT(KDataType2,"audio/wav");
	TBufC16<KMaxDataTypeLength> strExpected1(KDataType1);
	TBufC16<KMaxDataTypeLength> strExpected2(KDataType2);
	strReturned = dataType.Des();
	INFO_PRINTF5(_L("    Expected Mime type  \"%S\" or \"%S\" returned MIME type = \"%S\", returned app UID = %X)"), &strExpected1, &strExpected2, &strReturned, uid);
	if (ret == KErrNone)
		{
		// get a local copy of the the mime type for Compare()...
		if(strReturned.Compare(strExpected1) == 0 || strReturned.Compare(strExpected2) == 0)
			{
			INFO_PRINTF1(_L("both expected and returned mime types are the same"));
			}
		else
			{
			INFO_PRINTF1(_L("GetMimeTypeL didnt return the expected MimeType"));
			User::Leave(KErrNotFound);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Recognizer failed to recognize file type"));
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(2,&myApaLsSession); // myApaLsSession
	}

TVerdict RTestMmfRecogTestFileHandle::DoTestStepL()
	{
	DummyHalCallL();
	__MM_HEAP_MARK;
	TInt error = KErrNone;
	INFO_PRINTF2(_L("%S"),&KFileName);
	TRAP(error,VerifyMimeTypesL(KFileName));
	Compare(error,KErrNone,_L("Unexpected error code"));
	__MM_HEAP_MARKEND;
	return iTestStepResult;
	}
