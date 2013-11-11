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
 @file te_locmonitorstep5.h
*/
#ifndef TE_LOCMONITORSTEP5_H
#define TE_LOCMONITORSTEP5_H

#include <test/TestExecuteStepBase.h>
#include "te_geolocmonitorstepbase.h"
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include "LocInfoConversionHandler.h"

class CTe_LocMonitorStep5 : public CTe_LocMonitorStepBase
	{

public:
	CTe_LocMonitorStep5();
	~CTe_LocMonitorStep5();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

    TInt RLbsLocInfoConverter_Connect1L();
    TInt RLbsLocInfoConverter_Connect2L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo1L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo2L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo3L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo4L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo5L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo6L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo7L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo8L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo9L();
    TInt RLbsLocInfoConverter_ConvertLocationInfo10L();
    TInt RLbsLocInfoConverter_Cancel1L();
    TInt RLbsLocInfoConverter_Cancel2L();
    TInt RLbsLocInfoConverter_Cancel3L();
    TInt RLbsLocInfoConverter_Cancel4L();
    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();
    
 

private:    // Data
 CLocInfoConversionHandler* iConversionHandler; 
 
 CLocInfoConversionHandler* iConversionHandler1; 
	};

_LIT(KLocMonitorStep5,"te_locmonitorstep5");

#endif
