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

#ifndef TESTBUFFER_H
#define TESTBUFFER_H

#include <testframework.h>

/* 
Test for video frame buffer
MM-MMF-VIDREND-U-0001
*/
class RTestVideoFrameBufferStep : public RTestStep
	{
public:
	static RTestVideoFrameBufferStep* NewL();

protected:
	// from RTestStep;
	TVerdict DoTestStepL();

private:
	RTestVideoFrameBufferStep();
	};

#endif // TESTBUFFER_H
