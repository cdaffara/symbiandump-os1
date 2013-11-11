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
 @file te_geolocmonitorstepbase.h
*/

#ifndef TE_LOCMONITORSTEPBASE_H
#define TE_LOCMONITORSTEPBASE_H
#include <test/TestExecuteStepBase.h>
#include <e32property.h>
#include "lbstimer.h"
#include "rlbslocmonitorsession.h"
#include "rlbslocmonitorareapositioner.h"
#include "lbssatellite.h"
#include "lbsareainfo.h"
#include "lbslocmonitorserverdata.h"
#include "te_geolocmonitorsuitedefs.h"
#include "lbsinternalinterface.h"
#include "lbsnetinternalapi.h"

#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationinfo.h>



/****************************************************************************
* Base Test Step class for te_geolocmonitorsuite test steps
* 
****************************************************************************/
class CTe_LocMonitorStepBase : public CTestStep
	{
	
public:
	
	enum TTestType
	        {
	        EPositive,
	        
	        ENegative
	        };

	virtual ~CTe_LocMonitorStepBase();
	CTe_LocMonitorStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	   /**
	     * Helper methods.
	     */
	    CLbsPositionInfo* CreatePositionInfoObjectLC();
	    
	    CLbsGsmCellInfo* CreateGsmCellInfoObjectLC();
	    
	    CLbsWcdmaCellInfo* CreateWcdmaCellInfoObjectLC();
	    
	    CLbsWlanInfo* CreateWlanInfoObjectLC();
	    
	    CLbsLocationInfo* CreateLocationInfoObjectLC( TLbsAreaInfoClassType aAreaType );
	    
	    void AppendAreaInfoObjectToLocationInfoL( CLbsLocationInfo* aLocationInfo,                    
	                                                TLbsAreaInfoClassType aAreaType );
	    
	    void CheckPositionConvertedInfoL( CLbsLocationInfo* aLocationInfo,
	                                                TTestType aTestType = EPositive );
	    
	    void CheckGsmCellConvertedInfoL( CLbsLocationInfo* aLocationInfo,
	                                                TTestType aTestType = EPositive  );
	    
	    void CheckWcdmaCellConvertedInfoL( CLbsLocationInfo* aLocationInfo,
	                                                TTestType aTestType = EPositive  );
	    
	    void CheckWlanConvertedInfoL( CLbsLocationInfo* aLocationInfo,
	                                                TTestType aTestType = EPositive  );
		


protected:

	
private:

	CActiveScheduler* iActiveScheduler;
	};


/**************************************************************************
* AO to wait for the Location Monitor database feedback (in a P&S property)
****************************************************************************/







#endif
