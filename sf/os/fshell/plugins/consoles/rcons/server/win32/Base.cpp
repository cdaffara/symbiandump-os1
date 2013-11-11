// Base.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// Description:
// implementation of the CBase class.

#include "Misc.h"
#include "stdafx.h"
#include "Base.h"

LPCTSTR KUnamed = TEXT("Unamed");

CBase::~CBase()
	{
	}

void CBase::SetName(LPCTSTR aName)
	{
	iName = aName;
	}

CBase::CBase()
	: iName(KUnamed)
	{
	}