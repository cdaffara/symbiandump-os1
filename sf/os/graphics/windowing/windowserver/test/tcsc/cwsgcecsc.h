/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* @file
* 
*
*/



#ifndef CWSGCECSC_H_
#define CWSGCECSC_H_

#include <test/tefunit.h>
#include <w32std.h>
#include "cwsgcecscbase.h"

class CWSGceCsc : public CWsGceCscBase
{
private:
	
public:
	CWSGceCsc();
	virtual ~CWSGceCsc();
	static CTestSuite* CreateSuiteL(const TDesC &aName);
	virtual void SetupL();
	virtual void TearDownL();
	void GRAPHICS_WSERV_CSC_101L();
	void GRAPHICS_WSERV_CSC_102L();
	void GRAPHICS_WSERV_CSC_103L();
	void GRAPHICS_WSERV_CSC_104L();
	void GRAPHICS_WSERV_CSC_105L();
private:
    void DrawBasicPattern (RWindow& aWin);
public:

private:
};

#endif /*CWSGCECSC_H_*/
