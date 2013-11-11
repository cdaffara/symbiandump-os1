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
//

/**
 @file
 @internalTechnology
 @test
*/


#ifndef LBSNETSIMTESTGATEWAY_H
#define LBSNETSIMTESTGATEWAY_H

#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsassistancedatabuilderset.h>


// forwards
class MLbsNetSimObserver;
class TLbsPrivacyResponse;
class TPositionInfo;

class RLbsNetSim : public RSessionBase
	{
	public:
		// Constructor
		IMPORT_C RLbsNetSim();
		IMPORT_C ~RLbsNetSim();
		
		// Server methods
		IMPORT_C TInt ConnectL(MLbsNetSimObserver* aObserver);
		IMPORT_C void Close();
		
		// Methods
		IMPORT_C void RegisterLcsMoLrL(const TDesC& aData);
		IMPORT_C void ReleaseLcsMoLrL(TInt aReason);
		IMPORT_C void ReleaseLcsLocationNotificationL(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
		IMPORT_C void MeasurementReportLocationL(const TPositionInfoBase& aPosition);
		IMPORT_C void MeasurementReportLocationRequestMoreAssistanceDataL(const TLbsAssistanceDataGroup& aFilter);
		IMPORT_C void MeasurementControlFailureL(TInt aReason);

	private:
		static TInt StartServer();
	public:
		// So inner classes can call sendreceive
		void SendReceive(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const
			{
			RSessionBase::SendReceive(aFunction, aArgs, aStatus);
			}
		TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs) const
			{
			return RSessionBase::SendReceive(aFunction, aArgs);
			}
		void SendReceive(TInt aFunction,TRequestStatus& aStatus) const
			{
			RSessionBase::SendReceive(aFunction, aStatus);
			}
		TInt SendReceive(TInt aFunction) const
			{
			return RSessionBase::SendReceive(aFunction);
			}
		TInt Send(TInt aFunction) const
			{
			return RSessionBase::Send(aFunction);
			}
	private:
		//
		class CNotificationMeasurementControlLocation : public CActive
			{
			public:
				static CNotificationMeasurementControlLocation* NewL(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				~CNotificationMeasurementControlLocation();
				
				void RunL();
				void DoCancel();
			private:
				// Constuctor
				CNotificationMeasurementControlLocation(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				void ConstructL();
			
				RLbsNetSim* 			iNetSim;
				MLbsNetSimObserver*		iObserver;
				
				// Params
				TPositionInfo					iPosition;
				TPckg<TPositionInfo>			iPositionPkg;
				TLbsNetPosRequestQuality		iQuality;
				TPckg<TLbsNetPosRequestQuality>	iQualityPkg;
				RLbsAssistanceDataBuilderSet	iAssistanceData;
				HBufC8*							iAssistanceDataBuffer;
				TPtr8					iPtr;
			};
		//
		class CNotificationRegisterLcsLocation : public CActive
			{
			public:
				CNotificationRegisterLcsLocation(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				~CNotificationRegisterLcsLocation();
				
				void RunL();
				void DoCancel();
			private:
				RLbsNetSim* 			iNetSim;
				MLbsNetSimObserver*		iObserver;
				
				// Params
				TLbsExternalRequestInfo			iRequestInfo;
				TPckg<TLbsExternalRequestInfo>	iRequestInfoPkg;
				TLbsNetPosRequestPrivacy		iRequestPrivacy;
				TPckg<TLbsNetPosRequestPrivacy>	iRequestPrivacyPkg;
			};
		//
		class CNotificationCancelPrivacyRequest : public CActive
			{
			public:
				CNotificationCancelPrivacyRequest(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				~CNotificationCancelPrivacyRequest();
				
				void RunL();
				void DoCancel();
			private:
				RLbsNetSim* 			iNetSim;
				MLbsNetSimObserver*		iObserver;
				
				// Params
				TInt					iReason;
				TPckg<TInt>				iReasonPkg;
			};
		//
		class CMeasurementReportLocation : public CActive
			{
			public:
				CMeasurementReportLocation(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				~CMeasurementReportLocation();
				
				void SendL(const TPositionInfo& aPosition);
				void RunL();
				void DoCancel();
			private:
				RLbsNetSim* 			iNetSim;
				MLbsNetSimObserver*		iObserver;
				
				// Params
				TInt					iReason;
				TPckg<TInt>				iReasonPkg;
				TPositionInfo			iPosition;
				TPckg<TPositionInfo>	iPositionPkg;
				TBool					iFireNotification;
				TPckg<TBool>			iFireNotificationPkg;
			};
		//
		class CNotificationNetworkGone : public CActive
			{
			public:
				CNotificationNetworkGone(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				~CNotificationNetworkGone();
				
				void RunL();
				void DoCancel();
			private:
				RLbsNetSim* 			iNetSim;
				MLbsNetSimObserver*		iObserver;
			};			
		class CNotificationResetAssistanceData : public CActive
			{
			public:
				CNotificationResetAssistanceData(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver);
				~CNotificationResetAssistanceData();
				
				void RunL();
				void DoCancel();
			private:
				RLbsNetSim* 			iNetSim;
				MLbsNetSimObserver*		iObserver;
				TLbsAssistanceDataGroup iMask;
				TPckg<TLbsAssistanceDataGroup> iMaskPkg;
			};			
	private:
		MLbsNetSimObserver*		iObserver;
		
		// A-Sync vairables: RegisterLcsMoLr
		HBufC*							iRegisterLcsMoLrData;
		// A-Sync vairables: ReleaseLcsMoLr
		TInt							iReleaseLcsMoLrReason;
		TPckg<TInt>						iReleaseLcsMoLrReasonPkg;
		// A-Sync vairables: RequestMoreAssistanceData
		TLbsAssistanceDataGroup			iRequestMoreAssistanceDataFilter;
		TPckg<TLbsAssistanceDataGroup>	iRequestMoreAssistanceDataFilterPkg;
		// A-Sync vairables: ReleaseLcsLocationNotification
		CLbsNetworkProtocolBase::TLbsPrivacyResponse		iReleaseLcsLocationNotificationResponse;
		TPckg<CLbsNetworkProtocolBase::TLbsPrivacyResponse> iReleaseLcsLocationNotificationResponsePkg;
		
		
		
		
		// Message helpers
		CMeasurementReportLocation*		iMeasurementReportLocation;
		
		// Notifications
		CNotificationMeasurementControlLocation*	iNotificationMeasurementControlLocation;
		CNotificationRegisterLcsLocation*			iNotificationRegisterLcsLocation;
		CNotificationCancelPrivacyRequest*			iNotificationCancelPrivacyRequest;
		CNotificationNetworkGone*					iNotificationNetworkGone;
		CNotificationResetAssistanceData*			iNotificationResetAssistanceData;
		
	};

#endif // LBSNETSIMTESTGATEWAY_H

