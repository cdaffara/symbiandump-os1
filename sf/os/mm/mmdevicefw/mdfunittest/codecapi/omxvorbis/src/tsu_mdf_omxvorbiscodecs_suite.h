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
// This contains CTestSuiteOmxVorbisCodecs
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TSU_MDF_OMXVORBISCODECS_SUITE_H
#define TSU_MDF_OMXVORBISCODECS_SUITE_H

#include <testframework.h>

/**
 *
 * CTestSuiteOmxVorbisCodecs defines the test suite for OmxVorbisCodecs tests
 * 
 *
 *
 */
class  CTestSuiteOmxVorbisCodecs : public CTestSuite 
	{
public:
		CTestSuiteOmxVorbisCodecs();
		void InitialiseL(void);
		virtual ~CTestSuiteOmxVorbisCodecs();
		virtual TPtrC GetVersion( void ) const;
	};

#endif // TSU_MDF_OMXVORBISCODECS_SUITE_H
