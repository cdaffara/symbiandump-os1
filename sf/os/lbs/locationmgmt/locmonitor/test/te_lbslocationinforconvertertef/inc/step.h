/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __STEP_H__
#define __STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbslocationinfoconvertersuitestepbase.h"

#include <lbs/lbslocationinfoconverter.h>
#include <lbs/lbslocationresolver.h>
#include <lbs/lbslocationinfoconvertercommon.h>
#include <lbs/lbslocationclasstypes.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationgsminfo.h>
#include <lbs/lbslocationgsmneighbourinfo.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include <lbs/lbslocationwcdmaneighbourinfo.h>
#include <lbs/lbslocationwlaninfo.h>
#include <LbsPosition.h> 

#include "Te_locationinfoconversionobserver.h"
#include "Te_locationresolverobserver.h"

class CStep : public CTe_LbsLocationInfoConverterSuiteStepBase
	{
public:
	CStep();
	~CStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
    virtual void ConvertLocationInfoL(TInt conversionprefs=0, TInt requestedinfo=0, TInt fntype=0);
    virtual void ConvertLocationInfo2L(TInt conversionprefs=0, TInt requestedinfo=0, TInt fntype=0);

    virtual TInt CreateLocationInfoConverterL();
//    virtual TInt SetCoordinateInfoL( CStifItemParser& aItem );
    
    virtual void SetGSMCellInfoL( TInt fntype=1, TInt countrycode=-2, TInt networkcode=-2, 
            TInt locationcode=-2, TInt cellid=-2, TInt timingadvance=-2, TInt Rxlev=-2, TInt nNeignbourcells=0, 
            TInt arfcn=-2, TInt bsic=-2, TInt NRxlev=-2);
    virtual TInt SetGSMCellInfo2L( TInt fntype=1, TInt countrycode=-2, TInt networkcode=-2, 
            TInt locationcode=-2, TInt cellid=-2, TInt timingadvance=-2, TInt Rxlev=-2, TInt nNeignbourcells=0, 
            TInt arfcn=-2, TInt bsic=-2, TInt NRxlev=-2);
    virtual TInt SetWCDMACellInfoL(TInt fntype, TInt countrycode=-2,
            TInt networkcode=-2, TInt cellid=-2, TInt nNeignbourcells=0, TInt scramblingcode=-2, 
            TInt neighbourucid=-2, TInt neighbourscode=-2, TInt neighbourpathloss=-2, 
            TInt neighboursignalstrength=-2);
    virtual TInt SetWLanInfoL(TInt fntype=1, TInt macidint=-2,  TInt signalstrength=-2);
    virtual void AddLocationInfoL();
    virtual void AddLocationInfo2L();
    
    virtual TInt ValidateLocationInfo(TInt requestedinfo=1);
    virtual TInt ValidateLocationInfo2(TInt requestedinfo=1);
    virtual TInt MiscL(TInt testcase);
    virtual TInt SetConversionPrefs(TLbsConversionPrefs aSetConversionPrefs=0);
    virtual TInt GetAndCheckConversionPrefs(TLbsConversionPrefs aSetConversionPrefs=0);
    virtual TInt Cleanup();
    
private:
    TInt iApiId;
    TInt iPluginUid;
    CLbsLocationResolver* iLbsLocationResolver;
    CLbsLocationResolver* iLbsLocationResolver2;
    CLbsLocationInfoConverter* iLbsLocationInfoConverter;
    CLbsLocationInfo* iLbsLocationInfo;
    CTeLocationResolverObserver* iResolverObserver;
    CTeLocationResolverObserver* iResolverObserver2;
    CTelocationinfoconversionobserver* iObserver;
    RLbsLocationInfoBaseArray iLbsLocationInfoArray;
    RLbsLocationInfoBaseArray iLbsLocationInfoArrayTemp;
    
    // for testing multipls conversions
    CTelocationinfoconversionobserver* iObserver2;
    CLbsLocationInfoConverter* iLbsLocationInfoConverter2;
    CLbsLocationInfo* iLbsLocationInfo2;
    
    RLbsLocationInfoBaseArray iLbsLocationInfoArray2;
    RLbsLocationInfoBaseArray iLbsLocationInfoArrayTemp2;
	
private:
	void CallL(TInt aId);
    void test_2698_4_1_1_1L();
    void test_2698_4_1_1_3L();
    void test_2698_4_1_1_4L();
    void test_2698_4_1_1_5L();
    void test_2698_4_1_1_6L();
    void test_2698_4_1_1_7L();
    void test_2698_4_1_1_8L();
    void test_2698_4_1_1_9L();
    void test_2698_4_1_1_10L();
    void test_2698_4_1_1_11L();
    void test_2698_4_1_1_12L();
    void test_2698_4_1_1_13L();
    void test_2698_4_1_2L();
    void test_2698_4_1_3L();
    void test_2698_4_1_4L();
    void test_2698_4_1_5L();
    void test_2698_4_1_6L();
    void test_2698_4_1_7L();
    void test_2698_4_1_8L();
    void test_2698_4_1_9L();
    void test_2698_4_1_10L();
    void test_2698_4_1_11L();
    void test_2698_4_1_12L();
    void test_2698_4_1_13L();
    void test_2698_4_1_14L();
    void test_2698_4_1_15L();
    void test_2698_4_1_16L();
    void test_2698_4_1_17L();
    void test_2698_4_1_18L();
    void test_2698_4_1_19L();
    void test_2698_4_1_20L();
    void test_2698_4_1_21L();
    void test_2698_4_1_22L();
    void test_2698_4_1_23L();
    void test_2698_4_1_24L();
    void test_2698_4_1_25L();
    void test_2698_4_1_26L();
    void test_2698_4_1_27L();
    void test_2698_4_1_28_1L();
    void test_2698_4_1_28_1_3L();
    void test_2698_4_1_28_1_4L();
    void test_2698_4_1_28_1_5L();
    void test_2698_4_1_28_1_6L();
    void test_2698_4_1_28_1_7L();
    void test_2698_4_1_28_1_8L();
    void test_2698_4_1_28_1_9L();
    void test_2698_4_1_28_1_10L();
    void test_2698_4_1_28_2L();
    void test_2698_4_1_29L();
    void test_2698_4_1_30L();
    void test_2698_4_1_31L();
    void test_2698_4_1_32L();
    void test_2698_4_1_33L();
    void test_2698_4_1_34L();
    void test_2698_4_1_35L();
    void test_2698_4_1_36L();
    void test_2698_4_1_37L();
    void test_2698_4_1_38L();
    void test_2698_4_1_39L();
    void test_2698_4_1_40L();
    void test_2698_4_1_41L();
    void test_2698_4_1_42L();
    void test_2698_4_1_43L();
    void test_2698_4_1_44L();
    void test_2698_4_1_45L();
    void test_2698_4_1_46L();
    void test_2698_4_1_47_1L();
    void test_2698_4_1_47_2L();
    void test_2698_4_1_47_3L();
    void test_2698_4_1_49_1L();
    void test_2698_4_1_49_2L();
    void test_2698_4_1_51L();
    void test_2698_4_1_52L();
    void test_2698_4_1_53L();
    void test_2698_4_2_1L();
    void test_2698_4_2_2L();
    void test_2698_4_2_3L();
    void test_2698_4_2_4L();
    void test_2698_4_2_5L();
    void test_2698_4_2_6L();
    void test_2698_4_2_7L();
    void test_2698_4_2_8L();
    void test_2698_4_2_9L();
    void test_2698_4_2_10L();
    void test_2698_4_2_11L();
    void test_2698_4_2_12L();
    void test_2698_4_2_13L();
    void test_2698_4_2_14L();
    void test_2698_4_2_15L();
    void test_2698_4_2_16L();
    void test_2698_4_2_17L();
    void test_2698_4_2_18L();
    void test_2698_4_2_19L();
    void test_2698_4_2_20L();
    void test_2698_4_2_21L();
    void test_2698_4_2_22L();
    void test_2698_4_3_1_1L();
    void test_2698_4_3_2_1L();
    void test_2698_4_3_2_2L();
    void test_2698_4_3_4L();
    void test_2698_4_3_5L();
    void test_2698_4_3_6L();
    void test_2698_4_3_7L();
    void test_2698_4_3_8L();
    void test_2698_4_3_9L();
    void test_2698_4_3_10L();
    void test_2698_4_4_1L();
    void test_2698_4_4_2L();
    void test_2698_4_4_3L();
    void test_2698_4_4_4L();
    void test_2698_4_5_1L();
    void test_2698_4_5_2L();
    void test_2698_5_1_1L();
    void test_2698_5_1_2L();
    void test_2698_5_1_3L();
    void test_2698_5_1_4L();
    void test_2698_5_1_5L();
    void test_2698_5_1_6L();
    void test_2698_5_1_7L();
    void test_2698_5_1_8L();
    void test_2698_5_1_9L();
    void test_2698_5_1_10L();
    void test_2698_5_1_11L();
    void test_2698_5_1_12L();
    void test_2698_5_1_13L();
    void test_2698_5_1_14L();
    void test_2698_5_1_15L();
    void test_2698_5_1_16L();
    void test_2698_5_1_17L();
    void test_2698_5_1_18L();
    void test_2698_5_1_19L();
    void test_2698_5_1_20L();
    void test_2698_5_1_21L();
    void test_2698_5_1_22L();
    void test_2698_5_1_23L();
    void test_2698_5_1_24L();
    void test_2698_5_1_25L();
    void test_2698_5_1_26L();
    void test_2698_5_1_27L();
    void test_2698_5_2_1L();
    void test_2698_5_2_2L();
    void test_2698_5_2_3L();
    void test_2698_5_2_4L();
    void test_2698_5_2_5L();
    void test_2698_5_2_6L();
    void test_2698_5_3_1L();
    void test_2698_5_3_2L();
    void test_2698_5_3_3L();
    void test_2698_5_3_4L();
    void test_2698_5_4_1L();
    void test_MultipleSessionsOneAfterAnotherL();
    void test_MultipleCoversionsAtSameTimeL();
    void test_MultipleResolversAtSameTimeL();
    void test_SetAndGetConversionPrefs();
    void test_2698_5_4_2L();
    void test_cacheL();
    void test_cache2L();
    void test_cache3L();
	};

_LIT(KStep,"Step");

#endif
