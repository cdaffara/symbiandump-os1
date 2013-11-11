/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __T_RPOSITIONER_DATA_H__
#define __T_RPOSITIONER_DATA_H__

#include "datawrapperbase.h"

#include <activecallback.h>
#include <blockitems.h>
#include <datadictionary.h>
#include <dataaccess.h>
#include <e32def.h>
#include <e32cmn.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <lbssatellite.h>
#include <tefresult.h>
#include <testblockcontroller.h>

/** 
 * This class is used to test the API's of RPositionServer and RPositioner 
 * classes of lbs API.
 */
class CT_RPositionerData : public CDataWrapperBase
	{
public:
	static CT_RPositionerData * NewL();
	~CT_RPositionerData();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RPositionerData ();
	void ConstructL();

protected:
	void DoCmdClose();
	void DoCmdOpenL(const TTEFSectionName& aSection);
	void DoCmdSetRequestor(const TTEFSectionName& aSection);
	void DoCmdSetUpdateOptions(const TTEFSectionName& aSection);
	void DoCmdGetPositionL(const TTEFSectionName& aSection);	
	
	/** Sets the Module ID that will be used when the RPositioner object tries to open a subsession with positioner server.*/
	void DoCmdUtilitySetPositionerModuleIdL(const TTEFSectionName& aSection);
	
	/** Compares the actual GPS position with the expected position to determine if it is good enough.*/
	TBool VerifyPositionIfNeeded( const TPosition& aPosition, TCoordinate aExpectedCoordinate, TReal aMaximumError );

	/** Compares the actual horizontal accuracy with the expected value to determine if is good enough.*/
	TBool VerifyHorizontalAccuracyIfNeeded( const TPosition& aPosition, TReal aExpectedPositionAccuracy);
	
	/** Compares the actual altitude with the expected value to determine if it is good enough.*/
	TBool VerifyAltitudeIfNeeded( const TPosition& aPosition, TReal aExpectedAltitude, TReal aMaximumAltitudeError );
	
	/** Compares the actual vertical accurracy with the expected value to determine if it is good enough.*/
	TBool VerifyAltitudeAccuracyIfNeeded( const TPosition& aPosition , TReal aAltitudeAccuracyLimit);

	/** Compares the actual speed with the expected value to determine if it is good enough.*/
	TBool VerifySpeedIfNeeded( const TPosition& aPosition, TBool aCheckSpeed, TReal aMaxSpeedError, TReal aExpectedSpeed );

	/** Compares the actual data quality status with the expected value to determine if it is good enough.*/
	TBool VerifyDataQualityIfNeededL(const TPositionInfo& aPosInfo, TPositionModuleStatus::TDataQualityStatus aExpectedDataQualityStatus,TPtrC aPositionServerInstanceName);

	/** Compares the actual age of the position data with the expected value to determine if it is not too old.*/
	TBool VerifyMaxAgeIfNeeded(const TPosition& aPosition);

	/** Compares the postion query count with the expected value to determine if is good enough.*/
	TBool VerifySuccessfulPositionQueryCount(TInt aRequired);

private:
	CConsoleBase* iCons;
	RPositioner iPositioner;
	TPositionModuleId iPositionerModuleId;
	TPosition iPreviousPosition;
	TTime iMaxAgeVerifyTime;
	TBool iPositionerUseModuleId;
	TInt iSuccessfulPositionQueryCount;
	TInt iMaxAge;
	
	static const TEnumEntryTable iEnumRequestorType[];
	static const TEnumEntryTable iEnumRequestorFormat[];
	static const TEnumEntryTable iEnumDataQualityStatus[];
	static const TEnumEntryTable iEnumDeviceStatus[];
	};

#endif // __T_RPOSITIONER_DATA_H__
	