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
// This contains CTestSuite_MMF_SbcCodec 
// 
//

#ifndef __TSU_MMF_SBCCODEC_SUITE_H_
#define __TSU_MMF_SBCCODEC_SUITE_H_

#include <testframework.h>

/** @xxxx
 * @class CTestSuite_MMF_SbcCodec TSU_MMF_SbcCodecSuite.h "TSU_MMF_SbcCodecSuite.h"
 * @brief Test suite class for testing sbc codec.
 */
class  CTestSuite_MMF_SbcCodec : public CTestSuite
{
public:
	virtual ~CTestSuite_MMF_SbcCodec();
	void 	InitialiseL();
	TPtrC 	GetVersion();
};

#endif // __TSU_MMF_SBCCODEC_SUITE_H_

