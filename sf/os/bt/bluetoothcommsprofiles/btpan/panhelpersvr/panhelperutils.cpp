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
//

#include "panhelper.h"
using namespace PanHelper;

void PanHelper::PanHelperPanic(TPanHelperPanic aPanicCode)
/**
Cause a panic with the supplied panic code
*/
	{
	User::Panic(KPanHelperName, aPanicCode);
	}
