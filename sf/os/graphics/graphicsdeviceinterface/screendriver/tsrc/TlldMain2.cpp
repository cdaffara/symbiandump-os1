// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <e32math.h>
#include <hal.h>
#include <bitdraw.h>
#include "tlld.h"

TInt E32Main()
	{
	CTrapCleanup* tc = CTrapCleanup::New();

	TInt temp=0;
	HAL::Get(KDefaultScreenNo, HALData::EDisplayColors, temp);	//force HAL memory allocation

	__UHEAP_MARK;

	TestLowLevel tll(1, _L("Low-level device driver test 2"));
	TRAPD(err, tll.TestMainL());
	tll(err == KErrNone);
	tll.Close();

	__UHEAP_MARKEND;
	delete tc;
	return 0;
	}

