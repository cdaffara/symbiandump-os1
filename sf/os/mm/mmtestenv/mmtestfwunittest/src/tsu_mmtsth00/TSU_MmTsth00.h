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
// This file contains definitions of the test step classes for TSU_MMTSTH00
// 
//

#ifndef __TSU_MMTSTH00_H__
#define __TSU_MMTSTH00_H__

 
// TestFrameworkMain : MM-TSTH-U-0001

class RTestMmTsthU0001 : public RTestStep
	{
public:
	static RTestMmTsthU0001* NewL();
	RTestMmTsthU0001();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();

	};

#endif // __TSU_MMTSTH00_H__
