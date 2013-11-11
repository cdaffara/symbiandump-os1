// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestSuite_MMF_A2DPBLUETOOTH_SVR 
// 
//

#ifndef __TSU_MMF_A2DPBLUETOOTH_SVR_SUITE_H_
#define __TSU_MMF_A2DPBLUETOOTH_SVR_SUITE_H_

#include <TestFramework.h>

class CTestStep_MMF_A2DPBLUETOOTH_SVR; //forward reference

/**
 *
 * CTestSuite_MMF_A2DPBLUETOOTH_SVR
 *
 * @lib "TSU_MMF_A2DPBLUETOOTH_SVR.lib"
 *
 */
class  CTestSuite_MMF_A2DPBLUETOOTH_SVR : public CTestSuite
	{
public:
	void InitialiseL( void );
	virtual ~CTestSuite_MMF_A2DPBLUETOOTH_SVR();
	virtual TPtrC GetVersion( void ) const;
private:
	};


#endif /* TSU_MMF_A2DPBLUETOOTH_SVR_SUITE_H_ */
