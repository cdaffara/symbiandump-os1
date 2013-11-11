// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TCOPYRECT_H__
#define __TCOPYRECT_H__


#include "te_graphicsperformanceSuiteStepBase.h"


class CTCopyRect : public CTe_graphicsperformanceSuiteStepBase
	{
public:
    virtual ~CTCopyRect();
	CTCopyRect();
	
private:
	// From CTe_graphicsperformanceSuiteStepBase
	virtual TVerdict doTestStepL();
	
	void CopyRectL();
	
private:	
	TRect iSourceRect;
	};
	
_LIT(KTCopyRect,"tcopyrect");

#endif
