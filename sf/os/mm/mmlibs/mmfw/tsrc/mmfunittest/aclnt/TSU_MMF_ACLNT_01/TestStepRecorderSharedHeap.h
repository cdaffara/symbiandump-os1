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
// These test steps create audio recorder utilities whose
// controllers share a single heap.
// 
//


#ifndef TESTSTEPRECORDERSHAREDHEAP_H
#define TESTSTEPRECORDERSHAREDHEAP_H

#include<mda/client/utility.h>

/**
 *
 * Creates multiple file recorders whose controllers share a single heap.
 * Plays one to make sure they work OK.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSharedHeapMultiFilePlayer :	public	CTestStepUnitMMFAudClient,
													public  MMdaObjectStateChangeObserver	
	{
protected:
	enum TTestType
		{
		ENone,
		ERecord,
		EPanic
		};
	
public:
	static CTestStepRecorderSharedHeapMultiFilePlayer* NewL( const TDesC& aName, TBool aMixHeapStyle = EFalse );
	~CTestStepRecorderSharedHeapMultiFilePlayer();
	enum TVerdict DoTestStepPreambleL(void);
	enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();	
	
TInt GetAudRecorderSourceHandle(CMdaAudioRecorderUtility* aRecorder,
														 TMMFMessageDestination* /*aSrcHandle*/);

	// from MMdaObjectStateChangeObserver
	void  MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode);

protected:
	CTestStepRecorderSharedHeapMultiFilePlayer( const TDesC& aName, TBool aMixHeapStyle );
	enum TVerdict DoTestL( TTestType aTestType );

private:
	CMdaAudioRecorderUtility* CreateAndOpenRecorderL( TInt aIteration );

protected:
	TInt iError;
	TBool iMixHeapStyle;
	
private:
	RPointerArray<CMdaAudioRecorderUtility> iRecorders; // array of recorders
	};

		
#endif // TESTSTEPRECORDERSHAREDHEAP_H
