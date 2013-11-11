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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <e32cmn.h>
#include <gdi.h>

const TInt KNumDispModes = 14;
const TPtrC KDisplayModeNames[KNumDispModes] =
	{
	_L("ENone"),
	_L("EGray2"), 
	_L("EGray4"), 
	_L("EGray16"), 
	_L("EGray256"),
	_L("EColor16"), 
	_L("EColor256"), 
	_L("EColor64K"), 
	_L("EColor16M"),
	_L("ERgb"),
	_L("EColor4K"),
	_L("EColor16MU"),
	_L("EColor16MA"),
	_L("EColor16MAP"),
	};

/** Namespace for grouping useful utility methods for use with the test framework.
 */
namespace TestUtils
	{
	TPtrC DisplayModeToString(const TDisplayMode aMode);
	}


#endif // TESTUTILS_H
