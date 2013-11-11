// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The class representing CPositioner and assosiated request.
// 
//

#if (!defined __TE_LBSPSYPOSITIONER__)
#define __TE_LBSPSYPOSITIONER__

#include <e32base.h>
#include <lbs.h>
#include <lbssatellite.h>
#include <lbs/epos_cpositioner.h>

#include "te_active.h"
#include "EPos_MPosParameterObserver.h"
#include "EPos_MPosStatusObserver.h"

class CTe_PsyResponse;
class CTestExecuteLogger;

/**
The class representing CPositioner and assosiated request.
*/
class CTe_PsyPositioner : public CTe_Active, public MPosParameterObserver, public MPosStatusObserver
	{
private:
	enum TPosInfoType
		{
		EPosInfo,
		ECourseInfo,
		ESatInfo,
		EGenericInfo
		}; 
	
public:	
	static CTe_PsyPositioner* NewLC(const TUid& aUid, MTe_ActiveObserver& aObserver, 
			CTestStep&	aTestStep);
	virtual ~CTe_PsyPositioner();
	
	//From MPosParameterObserver
	virtual TInt GetRequiredPositionQuality(TPositionQuality& aPositionQuality) const;
	virtual void GetMaxAge(TTime& aMaxAge) const;
	virtual TBool IsPartialUpdateAllowed() const;
		
	virtual void GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const;
	virtual void SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut);
	virtual void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime);
		
	//From MPosStatusObserver
	virtual void ReportStatus(const TPositionModuleId& aImplementationUid, 
			const TPositionModuleStatus& aStatus);
		
	inline CTestExecuteLogger& Logger();
	inline void SetMaxAge(const TTime& aMaxAge);
	inline void SetAcceptPartialUpdates(TBool aAccept);
	inline void SetUpdateTimeout(TTimeIntervalMicroSeconds aUpdateTimeout);
	inline TTimeIntervalMicroSeconds PsyDefaultUpdateTimeout();
	
	IMPORT_C void NotifyPositionUpdate();
	IMPORT_C void CancelNotifyPositionUpdate();
	IMPORT_C void SimulateTimerErrorsL();
	
	TBool TrackingOverridden() const;
	void StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);
	void StopTracking();
				
	IMPORT_C void CheckPosUpdateReceivedL(const CTe_PsyResponse& aLbsResponse);
	IMPORT_C void CheckPosUpdateNotReceivedL();
	IMPORT_C void CheckPosUpdateFailedL(TInt aExpectedError);
	
	void CheckModuleStatusReceivedL(const CTe_PsyResponse& aLbsResponse);
	void CheckModuleStatusNotReceivedL();
		
protected:	
	virtual void RunL();
	virtual void DoCancel();
		
private:
	CTe_PsyPositioner(MTe_ActiveObserver& aObserver, CTestStep& aTestStep);
	void ConstructL(const TUid& aUid);
	TPositionInfo& PosInfo();
		
private:
	TUint iPositionerId;
	CPositioner* iPositioner;
	CTestStep& iTestStep;
	CTestExecuteLogger&	iLogger;
	
	TTime iMaxAge;
	TBool iAcceptPartialUpdates;	
	
	TBool iPosUpdateReceived;
	
	TPosInfoType iPosInfoType;
	TPositionInfo iPosInfo;
	TPositionCourseInfo	iCourseInfo;
	TPositionSatelliteInfo iSatInfo;
	HPositionGenericInfo* iGenericInfo;
	
	TBool iModStatusUpdated;
	TPositionModuleStatus iModStatus;
	
	TTimeIntervalMicroSeconds	iUpdateTimeout;
	TTimeIntervalMicroSeconds	iPsyDefaultUpdateTimeout;
	};

inline CTestExecuteLogger& CTe_PsyPositioner::Logger()
	{
	return iLogger;
	}

inline void CTe_PsyPositioner::SetMaxAge(const TTime& aMaxAge)
	{
	iMaxAge = aMaxAge;
	}

inline void CTe_PsyPositioner::SetAcceptPartialUpdates(TBool aAccept)
	{
	iAcceptPartialUpdates = aAccept;
	}

inline void CTe_PsyPositioner::SetUpdateTimeout(TTimeIntervalMicroSeconds aUpdateTimeout)
	{
	iUpdateTimeout = aUpdateTimeout;
	}

inline TTimeIntervalMicroSeconds CTe_PsyPositioner::PsyDefaultUpdateTimeout()
	{
	return iPsyDefaultUpdateTimeout;
	}
		
#endif //__TE_LBSPSYPOSITIONER__
