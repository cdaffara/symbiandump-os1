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
// This file contains definitions of the test step classes for Test Framework
// 
//


#ifndef __TSU_MMTSTH22_H__
#define __TSU_MMTSTH22_H__

 
// File Logger Test : MM-TSTH-U-2201
class RTestMmTsthU2201 : public RTestStep
	{
public:
	static RTestMmTsthU2201* NewL();
	RTestMmTsthU2201();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();
	};

#endif // __TSU_MMTSTH22_H__
