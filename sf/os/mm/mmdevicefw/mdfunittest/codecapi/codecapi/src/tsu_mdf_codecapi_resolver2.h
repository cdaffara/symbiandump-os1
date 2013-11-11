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
// Header file: More resolver related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_resolver2.h
*/

#ifndef TSU_MDF_CODECAPI_RESOLVER2_H
#define TSU_MDF_CODECAPI_RESOLVER2_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0050-HP
 * Loads a video encoder using the FourCC and the Custom Resolver
 *
 * @class RTestStepCodecApiLoadVideoEncoder 
 *
 */
class RTestStepCodecApiLoadVideoEncoder : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiLoadVideoEncoder();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0051-HP
 * Loads a video decoder using the FourCC and the Custom Resolver
 *
 * @class RTestStepCodecApiLoadVideoDecoder 
 *
 */
class RTestStepCodecApiLoadVideoDecoder : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiLoadVideoDecoder();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0040-HP
 * Find a video PU using opaque data.
 *
 * @class RTestStepCodecApiLoadVideoOpaqueData 
 *
 */
class RTestStepCodecApiLoadVideoOpaqueData : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiLoadVideoOpaqueData();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1040-HP
 * Try and find a video PU using invalid opaque data.
 *
 * @class RTestStepCodecApiLoadVideoOpaqueDataInvalid 
 *
 */
class RTestStepCodecApiLoadVideoOpaqueDataInvalid : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiLoadVideoOpaqueDataInvalid();
	virtual TVerdict DoTestStepL();
	};

#endif // TSU_MDF_CODECAPI_RESOLVER2_H


