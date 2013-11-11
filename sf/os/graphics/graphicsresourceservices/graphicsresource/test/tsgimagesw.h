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

#ifndef TSGIMAGESW_H
#define TSGIMAGESW_H

#include "tgraphicsresourceteststepbase.h"

class CTSgImageSw : public CTSgTestStepBase
	{
public:
	CTSgImageSw();
	~CTSgImageSw();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
private:
	void TestGetPixelFormatsSWL();
	void TestGetImageInterfacesSWL();
	void TestGetImageInterfaceAddressInfo();
	};

_LIT(KTSgImageSw,"TSgImageSw");

#endif /*TSGIMAGESW_H*/
