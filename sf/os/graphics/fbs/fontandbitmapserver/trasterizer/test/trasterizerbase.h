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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TRASTERIZERBASE_H
#define TRASTERIZERBASE_H

#include <test/testexecutestepbase.h>
#include <graphics/fbsrasterizer.h>


class CTRasterizerBase : public CTestStep
	{
public:	
	virtual ~CTRasterizerBase();
	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();	
	virtual TVerdict doTestStepPostambleL();
	
protected:
	void GetExampleRasterizerL();
	
protected:
	CFbsRasterizer* iRasterizer;
	};

#endif // TRASTERIZERBASE_H
