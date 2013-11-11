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
 @file te_locmonitorStep6.h
*/
#ifndef TE_LOCMONITORSTEP6_H
#define TE_LOCMONITORSTEP6_H

#include <test/TestExecuteStepBase.h>
#include "te_geolocmonitorstepbase.h"
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include "LocInfoConversionHandler.h"

class CTe_LocMonitorStep6 : public CTe_LocMonitorStepBase
	{

public:
	CTe_LocMonitorStep6();
	~CTe_LocMonitorStep6();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

    TInt RLbsLocInfoConverter_ConvertLocationInfo6L();
    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();
    
 

private:    // Data
 CLocInfoConversionHandler* iConversionHandler; 
 
 CLocInfoConversionHandler* iConversionHandler1; 
	};

_LIT(KLocMonitorStep6,"te_locmonitorstep6");

#endif
