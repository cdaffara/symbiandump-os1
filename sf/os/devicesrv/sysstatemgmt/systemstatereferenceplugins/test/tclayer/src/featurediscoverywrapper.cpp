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

#include "featurediscoverywrapper.h"

#include <featdiscovery.h>
#include <ssm/ssmuiproviderdll.h>

/**
 * Wrapper for CSsmUiSpecific::IsAmaStarterSupported() 
 * 
 * @test
 * @internalComponent 
 */
TBool CSsmUiSpecific::IsAmaStarterSupported()
	{
	return FeatureDiscoveryWrapper::GetAmaSupported();
	}


/**
 * Sets AMAs as supported
 * 
 * @test
 * @internalComponent
 */
void FeatureDiscoveryWrapper::SetAmaSupported()
	{
	iAmaSupported = ETrue;
	}

/**
 * Sets AMAs as not supported
 * 
 * @test
 * @internalComponent
 */
void FeatureDiscoveryWrapper::SetAmaNotSupported()
	{
	iAmaSupported = EFalse;
	}

/**
 * Returns true if AMAs are supported
 * 
 * @test
 * @internalComponent
 */
TBool FeatureDiscoveryWrapper::GetAmaSupported()
	{
	return iAmaSupported;
	}

/**
 * Stores if AMAs are supported
 * 
 * @test
 * @internalComponent
 */
TBool FeatureDiscoveryWrapper::iAmaSupported = ETrue;
