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

#ifndef __TDISPLAYCONFIGURATION_H__
#define __TDISPLAYCONFIGURATION_H__

#include <test/tefunit.h>
#include <w32std.h>
class CTestExecuteLogger;

class CTestDisplayConfiguration : public CTestFixture 
	{
public:
	static const CTestDisplayConfiguration& Instance();

	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
		
private:
	// Private tests
	void GRAPHICS_COMMONHEADER_0001L();
	void GRAPHICS_COMMONHEADER_0002L();
	void GRAPHICS_COMMONHEADER_0003L();
	void GRAPHICS_COMMONHEADER_0004L();
	void GRAPHICS_COMMONHEADER_0005L();
	void GRAPHICS_COMMONHEADER_0006L();
	void GRAPHICS_COMMONHEADER_0007L();
	void GRAPHICS_COMMONHEADER_0008L();
	void GRAPHICS_COMMONHEADER_0009L();
	void GRAPHICS_COMMONHEADER_0010L();
	void GRAPHICS_COMMONHEADER_0011L();
	void GRAPHICS_COMMONHEADER_0012L();
	void GRAPHICS_COMMONHEADER_0013L();
	};


#endif	//__TDISPLAYCONFIGURATION_H__
