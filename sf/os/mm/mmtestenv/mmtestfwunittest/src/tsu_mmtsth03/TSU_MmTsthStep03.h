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

#ifndef __TSU_MMTSTHSTEP03_H__
#define __TSU_MMTSTHSTEP03_H__

#include "TestFramework/Filename.h"

class CTestSuite;
class CTSUMmTsthSuite03;

// base class for all the TSU_MMTSTH03 suite test steps

class RTSUMmTsthStep03 : public RTestStep
	{
public:
	virtual TVerdict OpenL();
	virtual void Close();

protected:
	// our CFileName for testing
	CFileName* iFileName;
	};

#endif //__TSU_MMTSTHSTEP03_H__
