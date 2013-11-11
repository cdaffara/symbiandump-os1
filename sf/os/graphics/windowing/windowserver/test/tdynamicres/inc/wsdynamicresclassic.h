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
// Test class that allows a script to set global values 
// 
//

#ifndef __WSDYNAMICRESCLASSIC_H__
#define __WSDYNAMICRESCLASSIC_H__

#include <test/tefunit.h>
#include <w32std.h>
class CTestExecuteLogger;

class CWsDynamicResClassic : public CTestFixture 
	{
public:
	static const CWsDynamicResClassic& Instance();

	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
		
private:
	// Private tests
	void GRAPHICS_WSERV_DYNAMICRES_0100L();
	};


#endif	//__WSDYNAMICRESCLASSIC_H__
