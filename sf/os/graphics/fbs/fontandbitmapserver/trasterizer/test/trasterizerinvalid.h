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

#ifndef TRASTERIZERINVALID_H
#define TRASTERIZERINVALID_H

#include "trasterizerbase.h"

_LIT(KTRasterizerInvalidStep, "TRasterizerInvalid");


/** Class for testing how the example rasterizer performs when
passed invalid parameters.
 */
class CTRasterizerInvalid : public CTRasterizerBase
	{
public:
	CTRasterizerInvalid();	
	
	// From CStepStep	
	virtual TVerdict doTestStepL();	
	
protected:					
	void TestGetInterfaceL();
	};

#endif // TRASTERIZERINVALID_H
