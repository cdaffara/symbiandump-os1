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


#ifndef LBSNETSIMTESTOBSERVER_H
#define LBSNETSIMTESTOBSERVER_H

#include <lbs/lbsnetprotocolbase.h>

class MLbsNetSimTestObserver
	{
	public:
		/**
		Used in the NotifyError function.
		*/
		enum EFunction
			{
			EStartNetworkPrivacyRequest,
			EStartNetworkLocationRequest,
			};
	
		/**
		Called when a client connects to the Gateway API
		*/
		virtual void Connected() = 0;
		
		/**
		Called when a client disconnects from the Gateway API
		*/
		virtual void Disconnected() = 0;
		
		/**
		Start of an MoLr (FROM: Gateway)
		
		@param aData The data to start the MoLr. If this is blank then we are starting an MoLr, if not then an X3P
		*/
		virtual void NotifyRegisterLcsMoLr(const TDesC& aData) = 0;
		
		/**
		End of an MoLr (FROM: Gateway)
		
		@param aData The reason the MoLr ended
		*/
		virtual void NotifyReleaseLcsMoLr(TInt aReason) = 0;
		
		/**
		A measurement control (TO: Gateway)
		
		@param aPosition	the position
		@param aData		the assitance data
		@param aQuality		the quality of the data
		*/
		virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
													  const RLbsAssistanceDataBuilderSet& aData, 
													  const TLbsNetPosRequestQuality& aQuality) = 0;
		
		/**
		The response to a privacy request (FROM: Gateway)
		
		@param aResult the result
		*/
		virtual void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult) = 0;
		
		/**
		The response to an MoLr (TO: Gateway)
		
		@param aReason the response code
		@param aPosition the position
		*/
		virtual void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition) = 0;
		
		/**
		The response to an MoLr (FROM: Gateway)
		
		@param aPosition the position
		*/
		virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition) = 0;
		
		/**
		A request for more assitance data (FROM: Gateway)
		*/
		virtual void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter) = 0;
		
		/**
		A measurement report failed
		
		@param aReason The reason for failure
		*/
		virtual void NotifyMeasurementReportControlFailure(TInt aReason) = 0;
		
		/**
		Used for giving errors
		
		@param aFunction The function that caused the error
		@param aError The error
		*/
		virtual void NotifyError(EFunction aFunction, TInt aError) = 0;
	};



#endif // LBSNETSIMTESTOBSERVER_H
