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
// Header file: Extra tests for PREQ1024 MDF Codec API, primarily aimed
// at code coverage for opaque data.
// 
//

/**
 @file tsu_mdf_codecapi_opaque.h
*/

#ifndef TSU_MDF_CODECAPI_OPAQUE_H
#define TSU_MDF_CODECAPI_OPAQUE_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0300-HP
 * Create opaque data with an L tag.
 *
 * @class RTestStepCodecApiOpaqueDataL 
 *
 */
class RTestStepCodecApiOpaqueDataL : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiOpaqueDataL();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0301-HP
 * Create opaque data with a P tag.
 *
 * @class RTestStepCodecApiOpaqueDataP
 *
 */
class RTestStepCodecApiOpaqueDataP : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiOpaqueDataP();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0302-HP
 * Create opaque data with an I tag.
 *
 * @class RTestStepCodecApiOpaqueDataI 
 *
 */
class RTestStepCodecApiOpaqueDataI : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiOpaqueDataI();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0303-HP
 * Create opaque data with an S or D tag.
 *
 * @class RTestStepCodecApiOpaqueDataS 
 *
 */
class RTestStepCodecApiOpaqueDataS : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiOpaqueDataS();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0304-HP
 * Create opaque data with no tag.
 *
 * @class RTestStepCodecApiOpaqueDataNone 
 *
 */
class RTestStepCodecApiOpaqueDataNone : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiOpaqueDataNone();
	virtual TVerdict DoTestStepL();
	};

#endif // TSU_MDF_CODECAPI_OPAQUE_H


