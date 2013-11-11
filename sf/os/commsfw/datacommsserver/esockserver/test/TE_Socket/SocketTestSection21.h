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
// SocketTestSection21.cpp
// 
//

#if (!defined __SOCKETTEST_21_H__)
#define __SOCKETTEST_21_H__

#include "TestStepSocket.h"

class CSocketTest21_1 : public CTestStepSocket
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest21_2 : public CTestStepSocket
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

#endif //(__SOCKETTEST_7_H__)

