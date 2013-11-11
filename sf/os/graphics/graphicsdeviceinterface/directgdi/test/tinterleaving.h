// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TINTERLEAVING_H__
#define __TINTERLEAVING_H__

#include "tdirectgdi_test_step_base.h"

/**
Base class for Interleaving family tests.
*/
class CTInterleaving : public CTDirectGdiStepBase
	{
public:
	CTInterleaving();
	~CTInterleaving();
	
private:
	// Main test methods
	void TestSyncL();
	void TestAsyncL(TBool aDebug=EFalse);

	// Helper functions
	void DrawDirectGdiCheckboardToTarget();
	void DrawDirectGdiBarsToTarget();
	void DrawImageToTargetL(TSgDrawableId aImageId);

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	virtual TVerdict doTestStepPostambleL();
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiInterleavingStep,"TDirectGDIInterleaving");

#endif
