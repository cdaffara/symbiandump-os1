/**
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
 @file te_earlyselflocpreventstepbase.h
*/
#if (!defined __TE_EARLYSELFLOCPREVENTSTEPBASE_H__)
#define __TE_EARLYSELFLOCPREVENTSTEPBASE_H__
#include <test/TestExecuteStepBase.h>
#include "te_agpsmanagerstepbase.h"


const TInt KStandardMaxFixTime = 10000000; //10 sec
const TInt KStandardAccuracy = 100; //100 meters 
const TInt KHighAccuracy = 5; //5 meters
const TReal64 KStandardLatitude = 10;
const TReal64 KStandardLongitude = 10;
const TTimeIntervalSeconds KOverlapTargetTime = 5; //5 sec
const TTimeIntervalSeconds KNonOverlapTargetTime = 10; //10 sec 


class CTe_EarlySelfLocPreventStepBase : public CTe_AgpsManagerStepBase
	{
public:	
	CTe_EarlySelfLocPreventStepBase();
	
	virtual TVerdict doTestStepL();
	
private:
	virtual void doTestPreambleImplL();
	virtual void doTestStepImplL() = 0;
	
protected:
	TTime iRequestedTargetTimeNRH;
	TLbsLocRequestQualityInt iRequestedQuality;	
	TLbsNetPosRequestMethodInt iRequestedPosMethods;
		
	TTime iResultedTargetTime;
	TLbsLocRequestQualityInt iResultedQuality;
	TLbsGpsOptions iResultedGpsOptions;
		
	TPositionExtendedSatelliteInfo iResultedStandardPosition;
	const TPositionInfoBase* iResultedStandardPositionPtr;
	
private:
	CLbsAdmin* iAdmin;
	CLbsAdmin::TGpsMode iOriginalMode;
	};


#endif //__TE_EARLYSELFLOCPREVENTSTEPBASE_H__
