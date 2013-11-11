// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Resolver related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_resolver.h
*/

#ifndef TSU_MDF_CODECAPI_RESOLVER_H
#define TSU_MDF_CODECAPI_RESOLVER_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0030-HP
 * Loads a PU using the FourCC and the Custom Resolver
 *
 * @class RTestStepCodecApiLoadFourCC 
 *
 */
class RTestStepCodecApiLoadFourCC : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiLoadFourCC();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0031-HP
 * Finds a PU using the FourCC and Custom Resolver with P8 src 
 *
 * @class RTestStepCodecApiFindP8InputFourCC 
 *
 */
class RTestStepCodecApiFindP8InputFourCC : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiFindP8InputFourCC();
	virtual TVerdict DoTestStepL();
	};	

//

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0032-HP
 * Finds a PU using the FourCC and Custom Resolver with PU8 destination
 *
 * @class RTestStepCodecApiFindPU8OutputFourCC 
 *
 */
class RTestStepCodecApiFindPU8OutputFourCC : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiFindPU8OutputFourCC();
	virtual TVerdict DoTestStepL();
	};	
		
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1030-HP
 * Attempts to find a PU using the FourCC and Custom Resolver with correct version but incorrect src
 *
 * @class RTestStepCodecApiFindIncorrectInputFourCC 
 *
 */
class RTestStepCodecApiFindIncorrectInputFourCC : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiFindIncorrectInputFourCC();
	virtual TVerdict DoTestStepL();
	};	
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1031-HP
 * Attempts to find a PU using the FourCC and Custom Resolver with incorrect implementation UID.
 *
 * @class RTestStepCodecApiFindIncorrectImpUid 
 *
 */
class RTestStepCodecApiFindIncorrectImpUid : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiFindIncorrectImpUid();
	virtual TVerdict DoTestStepL();
	};	
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1031-HP
 * Attempts to find a PU using the FourCC and Custom Resolver with incorrect interface UID.
 *
 * @class RTestStepCodecApiFindIncorrectInterfaceUid 
 *
 */
class RTestStepCodecApiFindIncorrectInterfaceUid : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiFindIncorrectInterfaceUid();
	virtual TVerdict DoTestStepL();
	};		

#endif // TSU_MDF_CODECAPI_RESOLVER_H


