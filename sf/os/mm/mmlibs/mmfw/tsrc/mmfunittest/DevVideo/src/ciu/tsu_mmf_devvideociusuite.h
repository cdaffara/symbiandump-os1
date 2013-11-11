// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MMF_DEVVIDEO_CIU_SUITE_H
#define TSU_MMF_DEVVIDEO_CIU_SUITE_H

#include <testframework.h>

/**
 *
 * @class CTestMmfDevVideoCiuSuite
 *
 */
class CTestMmfDevVideoCiuSuite : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestMmfDevVideoCiuSuite();
	TPtrC GetVersion();
	};

#endif

