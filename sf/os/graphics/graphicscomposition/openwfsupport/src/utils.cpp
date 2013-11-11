// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// utils.cpp: provides utilities to be used by openwfc-interop
//

#include <e32std.h>
#include "openwfcpanic.h"

void Panic(TOpenWfcPanic aPanic)
	{
	_LIT(KPanic, "WF-INTEROP");
	User::Panic(KPanic, aPanic);
	}
