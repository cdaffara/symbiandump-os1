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
// This contains CTestSuite_MMF_A2DPBLUETOOTH 
// 
//

#ifndef __TSU_MMF_A2DPBLUETOOTH_SUITE_H_
#define __TSU_MMF_A2DPBLUETOOTH_SUITE_H_

#include <TestFramework.h>

class CTestStep_MMF_A2DPBLUETOOTH; //forward reference

/**
 *
 * CTestSuite_MMF_A2DPBLUETOOTH
 *
 * @lib "TSU_MMF_A2DPBLUETOOTH.lib"
 *
 */
class  CTestSuite_MMF_A2DPBLUETOOTH : public CTestSuite
	{
public:
	void InitialiseL( void );
	virtual ~CTestSuite_MMF_A2DPBLUETOOTH();
	virtual TPtrC GetVersion( void ) const;
private:
	};


#endif /* TSU_MMF_A2DPBLUETOOTH_SUITE_H_ */
