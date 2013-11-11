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
// TSU_MMF_CTLFRMSuit_SDRMe.h
// 
//


#ifndef __TSU_MMF_CTLFRM_SUITE_SDRM_H_
#define __TSU_MMF_CTLFRM_SUITE_SDRM_H_

#include <testframework.h>

class CTestStep_MMF_CTLFRM_SDRM; //forward reference

/**
 *
 * CTestSuite, which is the base class for 
 * the CTestSuite_MMF_CTLFRM_SDRM.
 *
 * @lib "TSU_MMF_CTLFRM_SDRM.lib"
 *
 * @xxxx
 *
 */
class  CTestSuite_MMF_CTLFRM_SDRM : public CTestSuite
	{
	public:
	
		void InitialiseL( void );
		virtual ~CTestSuite_MMF_CTLFRM_SDRM();
		virtual TPtrC GetVersion( void ) const;
	
	private:

	};


#endif /* TSU_MMF_CTLFRM_SUITE_H_ */
