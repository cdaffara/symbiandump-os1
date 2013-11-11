// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMmfCtlfrmSuite 
// 
//

#ifndef TSI_MMFCTLFRMSUITE_H_
#define TSI_MMFCTLFRMSUITE_H_

class RTestStepMmfCtlfrm;

/**
 *
 * @class CTestMmfCtlfrmSuite
 *
 */
class  CTestMmfCtlfrmSuite : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestMmfCtlfrmSuite();
	TPtrC GetVersion() const;
	};

#endif // TSI_MMFCTLFRMSUITE_H_ 

