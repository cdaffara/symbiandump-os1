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
//

/**
 @file
 @internalComponent
*/

#ifndef _TE_MMSENDMOLR_H_
#define _TE_MMSENDMOLR_H_

#include "TE_EtelMMTestStepBase.h"

	
class CTestSendMolr : public CEtelMMLbsTestStepBase
	{
public:
	virtual enum TVerdict doTestStepL();
private:
	/**
		TStartMolrV7 lists the details sent by UE to Network when a Mobile Originating Location Reqquest(MOLR) request is being made.
	*/
//	RMobileLocationServices::TStartMolrV7    iStartMolr;
	/**
		TMolrReturnResultV7 lists all the MOLR return results that can be expected for the MOLR request being made.	
	*/		
	RMobileLocationServices::TMolrReturnResultV7 iMolrReturnResult;
	/**
		iTestCount holds test count number
	*/	
	TInt iTestCount;		
	};
	
#endif
