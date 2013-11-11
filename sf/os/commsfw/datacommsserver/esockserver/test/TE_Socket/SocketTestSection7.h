// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for ESOCK basic socket test section 7
// 
//


#if (!defined __SOCKETTEST_7_H__)
#define __SOCKETTEST_7_H__

#include "TestStepSocket.h"

class CSocketTest7_1 : public CTestStepSocket
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest7_2 : public CTestStepSocket
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest7_3 : public CTestStepSocket
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest7_4 : public CTestStepSocket
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};


#endif //(__SOCKETTEST_7_H__)

