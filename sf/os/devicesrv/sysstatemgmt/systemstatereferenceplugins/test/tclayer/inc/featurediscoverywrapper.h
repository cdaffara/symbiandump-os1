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
// Contains wrapper helper functions for test code to control the test wrappers
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef FEATUREDISCOVERYWRAPPER_H_
#define FEATUREDISCOVERYWRAPPER_H_

#include <e32std.h>

class FeatureDiscoveryWrapper
{
public:
	static void SetAmaSupported();
	static void SetAmaNotSupported();
	static TBool GetAmaSupported();
	
private:
	static TBool iAmaSupported;
};

#endif /*FEATUREDISCOVERYWRAPPER_H_*/
