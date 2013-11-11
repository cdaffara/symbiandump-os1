/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 @file te_locmonitorstep7.h
*/
#ifndef TE_LOCMONITORSTEP7_H
#define TE_LOCMONITORSTEP7_H

#include <test/TestExecuteStepBase.h>
#include "te_geolocmonitorstepbase.h"
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include "LocInfoConversionHandler.h"

class CTe_LocMonitorStep7 : public CTe_LocMonitorStepBase
	{

public:
	CTe_LocMonitorStep7();
	~CTe_LocMonitorStep7();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

    TInt RLbsLocInfoConverter_ConvertLocationInfo7L();
    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();
    
 
private:    // Data
 CLocInfoConversionHandler* iConversionHandler; 
 
 CLocInfoConversionHandler* iConversionHandler1; 
	};

_LIT(KLocMonitorStep7,"te_locmonitorstep7");

#endif
