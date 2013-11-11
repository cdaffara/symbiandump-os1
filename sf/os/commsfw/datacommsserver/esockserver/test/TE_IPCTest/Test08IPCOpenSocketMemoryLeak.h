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
// Contains declaration of test 08 that :
// - check against memory leak during open socket method
// under low memory conditions;
// 
//

#if (!defined __TEST08IPCOPENSOCKETMEMORYLEAK_H__)
#define __TEST08IPCOPENSOCKETMEMORYLEAK_H__

#include "TestStepIPC.h"

class CTest08IPCOpenSocketMemoryLeak : public CTestStepIPC
	{
	public:
		static const TDesC& GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};
#endif //(__TEST08IPCOPENSOCKETMEMORYLEAK_H__)

