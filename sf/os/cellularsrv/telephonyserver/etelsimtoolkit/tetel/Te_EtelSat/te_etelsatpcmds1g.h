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
// The class declaration of a TestExecute class for testing EtelSat
// release 7 features.
// 
//

#ifndef TE_ETELSATPCMDS1G_H_
#define TE_ETELSATPCMDS1G_H_

class CTestSatPCmds1g : public CRSatTestStep
	{
public:
	CTestSatPCmds1g() ;
	~CTestSatPCmds1g(){} ;
	virtual enum TVerdict doTestStepL();
	
private:

	}; // class CTestSatPCmds1g

#endif /*TE_ETELSATPCMDS1G_H_*/
