// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// These test steps create audio converter utilities whose
// controllers share a single heap.
// 
//


#ifndef TESTSTEPCONVERTERSHAREDHEAP_H
#define TESTSTEPCONVERTERSHAREDHEAP_H

#include<mda/client/utility.h>
/**
 *
 * Creates multiple file converters whose controllers share a single heap.
 * Plays one to make sure they work OK.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepConverterSharedHeapMultiFile :	public	CTestStepUnitMMFAudClient,
												public	MMdaObjectStateChangeObserver	
	{
protected:
	enum TTestType
		{
		ENone,
		EConvert,
		EPanic
		};
	
public:
	static CTestStepConverterSharedHeapMultiFile* NewL( const TDesC& aName, TBool aMixHeapStyle = EFalse );
	~CTestStepConverterSharedHeapMultiFile();
	enum TVerdict DoTestStepPreambleL(void);
	enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();	

	TInt GetAudConverterSourceHandle(CMdaAudioConvertUtility* aRecorder,
														 TMMFMessageDestination* /*aSrcHandle*/);
		
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
    
protected:
	CTestStepConverterSharedHeapMultiFile( const TDesC& aName, TBool aMixHeapStyle );
	enum TVerdict DoTestL( TTestType aTestType );

private:
	CMdaAudioConvertUtility* CreateAndOpenConverterL( TInt aIteration );

protected:
	TInt iError;
	TBool iMixHeapStyle;
	
private:
	RPointerArray<CMdaAudioConvertUtility> iConverters; // array of converters
	CMdaAudioConvertUtility* iConvert;
	CFileMan* iFileMan;
	TPtrC iFileName2;
	};

#endif // TESTSTEPCONVERTERSHAREDHEAP_H
