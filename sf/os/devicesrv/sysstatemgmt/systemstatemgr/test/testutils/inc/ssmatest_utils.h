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


#ifndef __SSMATEST_UTILS_H__
#define __SSMATEST_UTILS_H__

#include <e32std.h>
#include <e32base.h>
#include <w32std.h>


/**
This method is used to close all panic dialogs on the screen.
In some scenarios, specially during H4 testing were unlike the emulator the board is not re-started for each test,
a situation can arise when a test fails due to the presence of an unexpected dialog on the screen.
*/
void CloseAllPanicWindowsL();



#endif	// __SSMATEST_UTILS_H__
