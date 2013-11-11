// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __SOCKETTEST_18_H)
#define __SOCKETTEST_19_H
#include "TestStepSocket.h"


class CSocketTest19_1 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL();
	};
	
class CSocketTest19_2 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL();
	};
	

#endif //(__SOCKETTEST_19_H)

