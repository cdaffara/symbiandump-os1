// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestSuite_MMF_SWCDWRAP 
// 
//

#ifndef __TSU_MMF_SWCDWRAP_SUITE_H_
#define __TSU_MMF_SWCDWRAP_SUITE_H_

#include <testframework.h>

class CTestStep_MMF_SWCDWRAP; //forward reference

/**
 *
 * CTestSuite_MMF_SWCDWRAP, which is the base class for 
 * the CMMFSWCodecWrapper MMF_SWCDWRAP suite.
 *
 * @lib "TSU_MMF_SWCDWRAP.lib"
 *
 */
class  CTestSuite_MMF_SWCDWRAP : public CTestSuite
	{
public:
	
	void InitialiseL( void );
	virtual ~CTestSuite_MMF_SWCDWRAP();
	virtual TPtrC GetVersion( void ) const;

private:

	};


#endif /* TSU_MMF_SWCDWRAP_SUITE_H_ */
