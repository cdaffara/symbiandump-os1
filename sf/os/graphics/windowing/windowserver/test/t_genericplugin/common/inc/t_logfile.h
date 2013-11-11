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
 @internalComponent
*/

#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include <e32std.h>
#include <f32file.h>
#include "t_wservgenericpluginconsts.h"

class CLogFile: public CBase
	{
public:
	CLogFile();
	~CLogFile();
	static CLogFile* NewL();
	void ConstructL();
	void WriteToLogL(const TDesC& aDes);
#ifdef GRAPHICS_TEST_PLUGIN_NGA
	static void DeleteLogFileL();
#endif //GRAPHICS_TEST_PLUGIN_NGA
private:
	TBuf8<2> iEol8;
	RFs iFs;
	RFile iFile;
	};

#endif //GRAPHICS_TEST_PLUGIN_NGA

