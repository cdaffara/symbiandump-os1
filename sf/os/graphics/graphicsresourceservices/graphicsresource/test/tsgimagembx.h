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

#ifndef TSGIMAGEMBX_H
#define TSGIMAGEMBX_H

#include "tgraphicsresourceteststepbase.h"

class CTSgImageMbx : public CTSgTestStepBase
	{
public:
	CTSgImageMbx();
	~CTSgImageMbx();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
private:
	void TestGetPixelFormatsMBXL();
	void TestGetImageInterfacesMBXL();
	};

_LIT(KTSgImageMbx,"TSgImageMbx");

#endif /*TSGIMAGEMBX_H*/
