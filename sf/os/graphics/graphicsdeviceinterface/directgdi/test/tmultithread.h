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

/**
 @file
 @internalComponent 
 @test
*/

#ifndef TMULTITHREAD_H
#define TMULTITHREAD_H

#include "tdirectgdi_test_step_base.h"

_LIT(KNameThreadOne, "CTMultiThread_ThreadOne");
_LIT(KNameThreadTwo, "CTMultiThread_ThreadTwo");
_LIT(KMultiThreadSemaphore, "CTMultiThread_Sempaphore");

class CTMultiThread : public CTDirectGdiStepBase
	{
public:
	CTMultiThread();
	~CTMultiThread();
	
private:
	void TestDirectGdiMultipleThreadIndependenceL();
	static TInt ThreadOneStart(TAny* aInfo);
	static TInt ThreadTwoStart(TAny* aInfo);
	
	void TestShareEGLImageBetweenSources_MultithreadedL();
	static TInt ThreadEGLImageStart(TAny* aInfo);	
	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	// from CTDirectGdiStepBase
	void RunTestsL();
	
	RThread iThread1, iThread2;

private:	
	TInt iCaseNum;
	};		
	
_LIT(KTMultiThreadStep,"TMultiThread");

#endif /*TMULTITHREAD_H*/
