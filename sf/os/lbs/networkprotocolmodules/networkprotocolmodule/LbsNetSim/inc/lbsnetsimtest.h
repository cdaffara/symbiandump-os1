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

/**
 @file
 @internalTechnology
 @test
*/

#ifndef LBSNETSIMTEST_H
#define LBSNETSIMTEST_H

class MLbsNetSimTestObserver;

#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>

#include <s32mem.h>

/**
*/
class RLbsNetSimTest : public RSessionBase
	{
	public:
		// Error message specific to pretending the network is not available
		static const TInt KNetSimNetworkNotAvailable = 10071;

		// Constructor
		IMPORT_C RLbsNetSimTest();
		IMPORT_C ~RLbsNetSimTest();
		
		// Server methods
		IMPORT_C TInt ConnectL(MLbsNetSimTestObserver* aObserver);
		IMPORT_C void Close();
		
		// Methods
		IMPORT_C TBool SetReferenceLocation(TPosition& aLocation);
		IMPORT_C TBool SetReferenceLocation(TDesC8& aLocation);
		IMPORT_C TBool SetResponseTime(TInt32 aTime, TBool aSticky);
		IMPORT_C TBool SetResponseError(TInt32 aError, TBool aSticky);
		IMPORT_C TBool SetDefaultAssitanceDataFilter(TLbsAssistanceDataGroup& aFilter);
		IMPORT_C TBool SetMoreAssitanceDataFilter(TLbsAssistanceDataGroup& aFilter);
		IMPORT_C TBool ClearAssistanceDataFilters();
		IMPORT_C TBool SetAssistanceDataProvider(TUid aProvider);
		IMPORT_C TBool SetEmergenyStatus(TBool aEmergency);
		IMPORT_C TBool SetRoamingStatus(TBool aRoaming);
		IMPORT_C TBool SetQuality(TLbsNetPosRequestQuality& aQuality);
		IMPORT_C TBool SendResetAssistanceData(TLbsAssistanceDataGroup aMask);
		
		//
		IMPORT_C void StartNetworkPrivacyRequest(TLbsNetPosRequestPrivacy aType, TLbsExternalRequestInfo& aRequest);
		IMPORT_C void CancelNetworkPrivacyRequest();
		IMPORT_C void StartNetworkLocationRequest();
		IMPORT_C void CancelNetworkLocationRequest();
		
		//
		IMPORT_C TBool SetStepMode(TBool aStepMode);
		IMPORT_C TBool Next(TInt aError);
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
		TInt Send(TInt aFunction,const TIpcArgs& aArgs) const
			{
			return RSessionBase::Send(aFunction, aArgs);
			}
	private:
		/**
		*/
		class CNotificationConnect : public CActive	
			{
			public:
				CNotificationConnect(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationConnect();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
			};
		/**
		*/
		class CNotificationDisconnect : public CActive
			{
			public:
				CNotificationDisconnect(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationDisconnect();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
			};
		/**
		*/
		class CNotificationRegisterLcsMoLr : public CActive
			{
			public:
				CNotificationRegisterLcsMoLr(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationRegisterLcsMoLr();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;

				// Argument Data
				TBuf<256>				iData;
			};
		/**
		*/
		class CNotificationReleaseLcsMoLr : public CActive
			{
			public:
				CNotificationReleaseLcsMoLr(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationReleaseLcsMoLr();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TInt					iReason;
				TPckg<TInt>				iReasonPkg;
			};
		/**
		*/
		class CMessageReleaseLcsLocationNotification : public CActive
			{
			public:
				CMessageReleaseLcsLocationNotification(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CMessageReleaseLcsLocationNotification();
				
				void Send(TLbsNetPosRequestPrivacy& aType, TLbsExternalRequestInfo& aRequestInfo);
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TLbsNetPosRequestPrivacy 		iType;
				TPckg<TLbsNetPosRequestPrivacy> iTypePkg;
				TLbsExternalRequestInfo 		iRequestInfo;
				TPckg<TLbsExternalRequestInfo> 	iRequestInfoPkg;
				CLbsNetworkProtocolBase::TLbsPrivacyResponse			iResponse;
				TPckg<CLbsNetworkProtocolBase::TLbsPrivacyResponse>		iResponsePkg;
			};
		/**
		*/
		class CMessageStartNetworkLocationRequest : public CActive
			{
			public:
				CMessageStartNetworkLocationRequest(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CMessageStartNetworkLocationRequest();
				
				void Send();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
			};
		/**
		*/
		class CNotificationMeasurementReport : public CActive
			{
			public:
				CNotificationMeasurementReport(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationMeasurementReport();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TPositionInfo			iPosition;
				TPckg<TPositionInfo>	iPositionPkg;
			};
		/**
		*/
		class CNotificationMeasurementReportRequestMoreAssitanceData : public CActive
			{
			public:
				CNotificationMeasurementReportRequestMoreAssitanceData(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationMeasurementReportRequestMoreAssitanceData();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TLbsAssistanceDataGroup			iFilter;
				TPckg<TLbsAssistanceDataGroup>	iFilterPkg;
			};
		/**
		*/
		class CNotificationMeasurementReportControlFailure : public CActive
			{
			public:
				CNotificationMeasurementReportControlFailure(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationMeasurementReportControlFailure();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TInt					iReason;
				TPckg<TInt>				iReasonPkg;
			};
		/**
		*/
		class CNotificationMeasurementControlLocation : public CActive
			{
			public:
				static CNotificationMeasurementControlLocation* NewL(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationMeasurementControlLocation();
				void RunL();
				void DoCancel();
			private:
				CNotificationMeasurementControlLocation(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				void ConstructL();
			
			
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TPositionInfo					iPosition;
				TPckg<TPositionInfo>			iPositionPkg;
				TLbsNetPosRequestQuality		iQuality;
				TPckg<TLbsNetPosRequestQuality>	iQualityPkg;
				HBufC8*							iAssistanceData;
				TPtr8							iPtr;
			};
		/**
		*/
		class CNotificationFacilityLcsMoLrResult : public CActive
			{
			public:
				CNotificationFacilityLcsMoLrResult(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* iObserver);
				~CNotificationFacilityLcsMoLrResult();
				void RunL();
				void DoCancel();
			private:
				RLbsNetSimTest*			iTest;
				MLbsNetSimTestObserver*	iObserver;
				
				// Args
				TInt					iReason;
				TPckg<TInt>				iReasonPkg;
				TPositionInfo			iPosition;
				TPckg<TPositionInfo>	iPositionPkg;
			};
			
	private:
		MLbsNetSimTestObserver*		iObserver; // Observer for callbacks
		
		// Notifications
		CNotificationConnect*										iNotificationConnect;
		CNotificationDisconnect*									iNotificationDisconnect;
		CNotificationRegisterLcsMoLr*								iNotificationRegisterLcsMoLr;
		CNotificationReleaseLcsMoLr*								iNotificationReleaseLcsMoLr;
		CNotificationMeasurementReport*								iNotificationMeasurementReport;
		CNotificationMeasurementReportRequestMoreAssitanceData*		iNotificationMeasurementReportRequestMoreAssitanceData;
		CNotificationMeasurementReportControlFailure*				iNotificationMeasurementReportControlFailure;
		CNotificationMeasurementControlLocation*					iNotificationMeasurementControlLocation;
		CNotificationFacilityLcsMoLrResult*							iNotificationFacilityLcsMoLrResult;
		
		// Message handlers
		CMessageReleaseLcsLocationNotification*						iMessageReleaseLcsLocationNotification;
		CMessageStartNetworkLocationRequest*						iMessageStartNetworkLocationRequest;
	};


#endif // LBSNETSIMTEST_H
