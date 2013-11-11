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

#ifndef TCOPYRECT_H_
#define TCOPYRECT_H_

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

class CTCopyRect : public CTDirectGdiStepBase
	{
public:
	CTCopyRect();
	~CTCopyRect();
	
private:
 	void TestCopyRectL();
 	void TestCopyRectInvalidParametersL();
 	void CopyRectAlphaL();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiCopyRectStep,"TDirectGDICopyRect");

#endif /*TCOPYRECT_H_*/
