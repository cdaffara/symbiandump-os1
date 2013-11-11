// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSI_MMFRECOG_H__
#define __TSI_MMFRECOG_H__

#include <testframework.h>

#include <fbs.h>
#include "TSI_MmfRecog.h"

// recognizer stuff:
#include <apffndr.h>
#include <apgcli.h>
#include <apfrec.h>
#include <mdaaudiosampleeditor.h>

// Base class providing comparison functions
//
class RTestStepCompare : public RTestStep
	{
public:
	void SetVerdict(TVerdict aVerdict);
	void Compare(TInt aActual, TInt aExpect, const TDesC& aErrorMessage);
	void Compare(TUid aActual, TUid aExpect, const TDesC& aErrorMessage);
	void Compare(const TDesC& aActual, const TDesC& aExpect, const TDesC& aErrorMessage);
	void Compare(const TDesC8& aActual, const TDesC8& aExpect, const TDesC& aErrorMessage);
protected:	
	void DummyHalCallL();
	};



class RTestMmfRecog : public RTestStepCompare
	{
public:
	static RTestMmfRecog* NewL(const TDesC& aTestName);

	TVerdict OpenL();
	void Close();
	virtual TVerdict DoTestStepL();

protected:
private:
	RTestMmfRecog(const TDesC& aTestName);

	class TTestFileInfo
		{
	public:
		TTestFileInfo(const TDesC& aFileName, const TDesC8& aMimeType);
		TInt AddInfoString(const TDesC& aInfoString);
		const TDesC& iFileName;
		const TDesC8& iMimeType;
		};
	void VerifyMimeTypesL(const TTestFileInfo& aCodecData);


private:
	RFs iFs;
	RArray<TTestFileInfo> iPluginData;

protected:
	};
	
class RTestMmfFormat : public RTestStepCompare, public MMdaObjectStateChangeObserver
	{
public:
	static RTestMmfFormat* NewL(const TDesC& aTestName);

	TVerdict OpenL();
	void Close();
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	RTestMmfFormat(const TDesC& aTestName);

	class TTestFileInfo
		{
	public:
		TTestFileInfo(const TDesC& aFileName, const TDesC8& aMimeType);
		TInt AddInfoString(const TDesC& aInfoString);
		const TDesC& iFileName;
		const TDesC8& iMimeType;
		};
	void VerifyMimeTypesL(const TTestFileInfo& aCodecData);


private:
	RFs iFs;
	RArray<TTestFileInfo> iPluginData;
	};

//Tests recognition (MIME type and confidence) of several audio files 
class RTestMmfRecogData : public RTestStepCompare
	{
public:
	static RTestMmfRecogData* NewL(const TDesC& aTestName);

	TVerdict OpenL();
	void Close();
	virtual TVerdict DoTestStepL();

private:
	RTestMmfRecogData(const TDesC& aTestName);

	class TTestFileInfo
		{
	public:
		TTestFileInfo(const TDesC& aFileName, const TDesC& aMimeType, const CApaDataRecognizerType::TRecognitionConfidence aConfidence);
		const TDesC& iFileName;
		const TDesC& iMimeType;
		const CApaDataRecognizerType::TRecognitionConfidence iConfidence;
		};
	void VerifyDataL(const TTestFileInfo& aCodecData);

private:
	CActiveScheduler* iScheduler;
	RFs iFs;
	RArray<TTestFileInfo> iPluginData;
	};

// Negative test which accesses the recognizer via RApaLsSession::AppForDocument()
class RTestMmfRecogNeg : public RTestStepCompare
	{
public:
	static RTestMmfRecogNeg* NewL(const TDesC& aTestName);

	TVerdict OpenL();
	void Close();
	virtual TVerdict DoTestStepL();

protected:
private:
	RTestMmfRecogNeg(const TDesC& aTestName);

	void VerifyMimeTypesL(const TDesC& aFileName);


private:
	RFs iFs;

protected:
	};


// negative test using direct access to recognizer
class RTestMmfRecogNegDirect : public RTestStepCompare
	{
public:
	static RTestMmfRecogNegDirect* NewL(const TDesC& aTestName);
    void VerifyMimeTypesL(const TDesC& aFileName);

protected:
	RTestMmfRecogNegDirect(const TDesC& aTestName);
	
	// from RTestStep
	TVerdict OpenL();
	void Close();
	TVerdict DoTestStepL();

private:
	RFs iFs;

private:
	RPointerArray<TDesC> iFileNames;

protected:
	CActiveScheduler* iScheduler;
	};

// Alloc test
//
class RTestMmfRecogAlloc : public RTestStepCompare
	{
public:
	static RTestMmfRecogAlloc* NewL(const TDesC& aTestName);

	TVerdict OpenL();
	void Close();
	virtual TVerdict DoTestStepL();

protected:
private:
	RTestMmfRecogAlloc(const TDesC& aTestName);
	void VerifyMimeTypesL(const TDesC& aFileName, TDes8& aMimeType, TInt& aConfidenceLevel);

private:
	RFs iFs;

	TPtrC iFileName;
	TPtrC8 iMimeType;

	CActiveScheduler* iScheduler;

protected:
	};

// recognizer via RApaLsSession::AppForDocument()
class RTestMmfRecogTestFileHandle : public RTestStepCompare
	{
public:
	static RTestMmfRecogTestFileHandle* NewL(const TDesC& aTestName);
	TVerdict OpenL();
	void Close();
	virtual TVerdict DoTestStepL();

protected:

private:
	RTestMmfRecogTestFileHandle(const TDesC& aTestName);
	void VerifyMimeTypesL(const TDesC& aFileName);

private:
	RFs iFs;

protected:
	};

#endif // __TSI_MMFRECOG_H__
