/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_LbsAssistanceDataSuiteServer.h
*/

#if (!defined __TE_LBSASSISTANCEDATA_SERVER_H__)
#define __TE_LBSASSISTANCEDATA_SERVER_H__
#include <test/testexecuteserverbase.h>
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistancebadsatlist.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsassistancenavigationmodel.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceionosphericmodel.h>
#include <lbs/lbsassistanceutcmodel.h>
#include <lbs/lbsassistancereferencelocation.h>

class CTe_LbsAssistanceDataSuite : public CTestServer
	{
public:
	static CTe_LbsAssistanceDataSuite* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

    //-----------------------------------------------------------
	//--- Property builders/readers shared by some test steps ---
	//-----------------------------------------------------------

	// navigation model 
	RUEPositioningGpsNavigationModelBuilder iNavigationModelBuilder;
	RNavigationModelSatInfoArrayBuilder iNavModelSatInfoArrayBuilder;
	RNavigationModelSatInfoBuilder iNavModelSatInfoBuilder;
	REphemerisParameterBuilder iEphemerisBuilder;
	RSubFrame1ReservedArrayBuilder iSf1RsrvdArrayBuilder;
	
	RUEPositioningGpsNavigationModelReader iNavigationModelReader;
	RNavigationModelSatInfoArrayReader iNavModelSatInfoArrayReader;
	RNavigationModelSatInfoReader iNavModelSatInfoReader;
	REphemerisParameterReader iEphemerisReader;
    RSubFrame1ReservedArrayReader iSf1RsrvdArrayReader;
    
	// gps utc model
	RUEPositioningGpsUtcModelBuilder iGpsUtcModelBuilder;

	RUEPositioningGpsUtcModelReader iGpsUtcModelReader;
	
	// utran gsp reference time [not a root class]
	RUtranGpsReferenceTimeBuilder iUtranGpsRefTimeBuilder;
	RPrimaryCpichInfoBuilder iPrimaryCpichInfoBuilder;
	
	RUtranGpsReferenceTimeReader iUtranGpsRefTimeReader;
	RPrimaryCpichInfoReader iPrimaryCpichInfoReader;
	
    // rrlp gsp reference time [not a root class]
    RRrlpGpsReferenceTimeBuilder iRrlpGpsRefTimeBuilder;
    
    RRrlpGpsReferenceTimeReader iRrlpGpsRefTimeReader;
    
	// reference location
	RReferenceLocationBuilder iReferenceLocationBuilder;
	REllipsoidPointAltitudeEllipsoideBuilder iEllipsoidPointAltBuilder;
	
	RReferenceLocationReader iReferenceLocationReader;
	REllipsoidPointAltitudeEllipsoideReader iEllipsoidPointAltReader;
	
	// gps ionospheric model 
	RUEPositioningGpsIonosphericModelBuilder iGpsIonModelBuilder;
	RUEPositioningGpsIonosphericModelReader iGpsIonModelReader;
	
	// gps almanac
	RUEPositioningGpsAlmanacBuilder iGpsAlmanacBuilder;
	RAlmanacSatInfoArrayBuilder iAlmanacSatInfoArrayBuilder;
	RAlmanacSatInfoBuilder iAlmanacSatInfoBuilder;
	
	RUEPositioningGpsAlmanacReader iGpsAlmanacReader;
	RAlmanacSatInfoArrayReader iAlmanacSatInfoArrayReader;
	RAlmanacSatInfoReader  iAlmanacSatInfoReader;
	
	// acquistion assistance
	RUEPositioningGpsAcquisitionAssistanceBuilder iAcquistionAssistanceRootBuilder;
	RAcquisitionSatInfoArrayBuilder iAcquisitionSatInfoArrayBuilder;
	RAcquisitionSatInfoBuilder iAcquisitionSatInfoBuilder;
	RExtraDopplerInfoBuilder iExtraDopplerInfoBuilder;
	RTAzimuthAndElevationBuilder iAzimuthAndElevationBuilder;
	
	RUEPositioningGpsAcquisitionAssistanceReader iAcquistionAssistanceRootReader;
	RAcquisitionSatInfoArrayReader iAcquisitionSatInfoArrayReader;
	RAcquisitionSatInfoReader iAcquisitionSatInfoReader;
	RExtraDopplerInfoReader iExtraDopplerInfoReader;
	RTAzimuthAndElevationReader iAzimuthAndElevationReader;
	
	//  gps reference time
	RUEPositioningGpsReferenceTimeBuilder iGpsReferenceTimeBuilder;
	RGpsTowAssistArrayBuilder iGpsTowAssistArrayBuilder;
	RGpsTowAssistBuilder iGpsTowAssistBuilder;
	
	RUEPositioningGpsReferenceTimeReader iGpsReferenceTimeReader;
	RGpsTowAssistArrayReader iGpsTowAssistArrayReader;
	RGpsTowAssistReader iGpsTowAssistReader;
	
	// bad satellite list
	RBadSatListBuilder iBadSatListBuilder;
	RBadSatArrayBuilder iBadSatArrayBuilder;
	
	RBadSatListReader iBadSatListReader;
	RBadSatArrayReader iBadSatArrayReader;

    // ---------------------------------------------------
    // Other data shared by some steps
    // ---------------------------------------------------
    
    TInt iArrayIndex;

	};

#endif
