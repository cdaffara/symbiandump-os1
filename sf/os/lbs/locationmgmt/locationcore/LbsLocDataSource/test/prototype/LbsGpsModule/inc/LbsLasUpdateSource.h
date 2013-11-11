// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSLASUPDATESOURCE_H
#define LBSLASUPDATESOURCE_H

#include <e32base.h>
#include <Lbs.h>
#include <LbsSatellite.h>
#include <lbs/LbsLocDataSourceGpsBase.h>

#include "LbsTimer.h"
#include "LbsLasNmeaSource.h"

class MLasUpdateSourceObserver
	{
	public:
		virtual void HandleLocationUpdate(const TPositionInfoBase& aPosInfo) = 0;
		virtual void HandleDeviceStatusUpdate(TPositionModuleStatus::TDeviceStatus aDeviceStatus) = 0;
		virtual void HandleQualityStatusUpdate(TPositionModuleStatus::TDataQualityStatus aDataQuality) = 0;
		virtual void Shutdown() = 0;
	};

class CLasNmeaSource;

class CLasUpdateSource : public CBase, public MLasNmeaSourceObserver, public MLbsTimerObserver
	{
	public:
		static CLasUpdateSource* NewL(MLasUpdateSourceObserver* aObserver);
	   ~CLasUpdateSource();
	   
		void RequestLocationUpdate(const TTime& aTargetTime);

		void CancelLocationRequest();
	   
		void SetPowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode);

		void HandleTimer(TInt aTimerId, const TTime& aTargetTime);

		void HandleNmeaSentence(const TPtrC8* aNmeaFields, TInt aNumFields);
		
   private:
      CLasUpdateSource(MLasUpdateSourceObserver* aObserver);
		void ConstructL();  
	   
		void SetTargetTimer();
	   
	   void ConvertDecimalMinutesToDecimalDegrees(TReal& aCoord);
	   
	   void ExtractValueL(const TPtrC8& aField, TReal32& aValue);
	   void ExtractDegreesL(const TPtrC8& aField, TReal64& aDegrees);

	   void ExtractTimeL(const TPtrC8& aTime, TDateTime& aTheTime);
	   void ExtractDateL(const TPtrC8& aDate, TDateTime& aTheDate);

	   void PublishLocationUpdate(TPositionSatelliteInfo& aCourseInfo);

	   void ExtractRmcDataL(const TPtrC8* aNmeaFields, TInt aNumFields, TPositionSatelliteInfo& aInfo, TTime& aRmcTime);
	   void ExtractGgaDataL(const TPtrC8* aNmeaFields, TInt aNumFields, TPositionSatelliteInfo& aInfo, TTime& aGgaTime);
	   	   
	private:
		enum TSourceStatus {ESourceUnknown, ESourceClosed, ESourceOpening, ESourceReady, ESourceActive, ESourceError};
		TSourceStatus iSourceStatus;

	private:
		MLasUpdateSourceObserver* iObserver; 
		TPositionSatelliteInfo iSatInfo;
		CLasNmeaSource*     iNmeaSource;
		CLbsTimer*          iTimer;
		
		TTime iRmcTime;
		TTime iGgaTime;
	};

#endif //LBSLASUPDATESOURCE_H
