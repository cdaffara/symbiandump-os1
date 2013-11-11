// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client side implementation for the LbsX3P Api.
// 
//

#include <e32base.h>
#include <e32debug.h>
#include <lbspositioninfo.h>
#include "lbsnrhserverdata.h"
#include "LbsX3PImpl.h"
#include "lbsprocessuiddefs.h"
#include "messageutils.h"
#include "lbsx3pdata.h"
#include "lbsnrhmessageenums.h"
#include "lbssystemcontroller.h"


//
// RNrhServer
//

/* The X3P Implementation hides the details of the 
 * Privacy Controller from the class CLbsPrivacyController, which uses only the
 * published API.
 *
 * The privacy comntroller is implemented here as a client of the Network
 * Request Handler, which includes a server to handle requests from this 
 * component.
 *
 * Generic operation on thet client side is implemented by the
 * code in lbsloccommon\serverframework.
  */
RNrhServer::RNrhServer()
	{
	// nothing to do
	}
 	
RNrhServer::~RNrhServer()
	{
	// nothing to do
	}

TVersion RNrhServer::Version()
	{
	return TVersion(KNrhServerMajorVersionNumber,
					KNrhServerMinorVersionNumber,
					KNrhServerBuildVersionNumber);
	}

TInt RNrhServer::StartServer(TServerStartParams& /*aParams*/)
	{
	// Start LBS from LbsRoot    
    TRAPD(err, StartLbsL());
    return (err);
	}

void RNrhServer::StartLbsL()
	{
    RLbsSystemController systemController;
    RProcess process;
    CleanupClosePushL(process);
	systemController.OpenL(process.SecureId());
	CleanupClosePushL(systemController);
	
	TInt startErr = systemController.RequestSystemStartup();
	if (startErr != ELbsStartupSuccess)
		{
		User::Leave(KErrCouldNotConnect);
		}
	
	CleanupStack::PopAndDestroy(2, &process);
	}

//
// RNrhSession
//

RNrhSession::RNrhSession() :
	iTransmittedPosInfoPtr(NULL, 0),
	iTransmitPositionParamsPtr(NULL, 0),
	iReferencePosInfoPtr(NULL, 0),
	iReferencePositionParamsPtr(NULL, 0)
	{
	// nothing to do
	}
	
RNrhSession::~RNrhSession()
	{
	// nothing to do
	}

void RNrhSession::TransmitPosition(const TDesC& aDestinationId,
								   TUint aTransmitPriority,
								   TRequestStatus& aStatus, 
								   TPositionInfo& aTransmittedPosInfo)
	{	
	// iDestinationId is too big to fit in the buffer;
	// complete the request early with an error code	
	if (aDestinationId.Length() > iTransmitPositionParams.iDestinationId.MaxLength())
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrTooBig);
		}
	
	iTransmitPositionParams.iDestinationId = aDestinationId;
	iTransmitPositionParams.iPriority = aTransmitPriority;
	iTransmitPositionParams.iPositionInfoClassType = aTransmittedPosInfo.PositionClassType();
	iTransmitPositionParams.iPositionInfoClassSize = aTransmittedPosInfo.PositionClassSize();
	
	iTransmitPositionParamsPtr.Set(reinterpret_cast<TUint8*>(&iTransmitPositionParams),
								   sizeof(iTransmitPositionParams),
								   sizeof(iTransmitPositionParams));
	iTransmittedPosInfoPtr.Set(reinterpret_cast<TUint8*>(&aTransmittedPosInfo), 
							   aTransmittedPosInfo.PositionClassSize(),
							   aTransmittedPosInfo.PositionClassSize());
	TIpcArgs args(&iTransmitPositionParamsPtr, &iTransmittedPosInfoPtr);
	SendReceive(EX3pTransmitPosition, args, aStatus);
	}

void RNrhSession::TransmitPosition(const TDesC& aDestinationId,TUint aTransmitPriority,
						                 TRequestStatus& aRefPosStatus, TPositionInfo& aRefPosInfo,
						  TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo)
	{
	// iDestinationId is too big to fit in the buffer;
	// complete the request early with an error code	
	if (aDestinationId.Length() > iTransmitPositionParams.iDestinationId.MaxLength())
		{
		TRequestStatus* refStatus = &aRefPosStatus;
		TRequestStatus* transmittedStatus = &aTransmittedPosStatus;
		User::RequestComplete(refStatus, KErrTooBig);
		User::RequestComplete(transmittedStatus, KErrTooBig);
		}
		
	ReportReferenceLocation(aRefPosStatus, aRefPosInfo);
	TransmitPosition(aDestinationId, aTransmitPriority,
					 aTransmittedPosStatus, aTransmittedPosInfo);
	}

void RNrhSession::ReportReferenceLocation(TRequestStatus& aRefPosStatus, TPositionInfo& aReferencePosInfo)
	{

	iReferencePositionParams.iPositionInfoClassType = aReferencePosInfo.PositionClassType();
	iReferencePositionParams.iPositionInfoClassSize = aReferencePosInfo.PositionClassSize();
	
	iReferencePositionParamsPtr.Set(reinterpret_cast<TUint8*>(&iReferencePositionParams),
								   sizeof(iReferencePositionParams),
								   sizeof(iReferencePositionParams));
								   
	iReferencePosInfoPtr.Set(reinterpret_cast<TUint8*>(&aReferencePosInfo), 
							   aReferencePosInfo.PositionClassSize(),
							   aReferencePosInfo.PositionClassSize());
	TIpcArgs args(&iReferencePositionParamsPtr, &iReferencePosInfoPtr);
	SendReceive(EX3pReportReferenceLocation , args, aRefPosStatus);
	}

void RNrhSession::CancelTransmitPosition()
	{
	TIpcArgs args;
	SendReceive(EX3pCancelTransmitPosition, args);
	}

TInt RNrhSession::SetTransmitOptions(const TLbsTransmitPositionOptions& aTransmitOptions)
	{
	TPckgC<TLbsTransmitPositionOptions> optionsPckg(aTransmitOptions);
	TIpcArgs args(&optionsPckg);
	return (SendReceive(EX3pSetTransmitOptions, args));
	}
	
TInt RNrhSession::GetTransmitOptions(TLbsTransmitPositionOptions& aTransmitOptions) const
	{
	TPckg<TLbsTransmitPositionOptions> optionsPckg(aTransmitOptions);
	TIpcArgs args(&optionsPckg);
	return (SendReceive(EX3pGetTransmitOptions, args));
	}

//
// CLbsTransmitPositionServerImpl
//

CLbsTransmitPositionServerImpl::CLbsTransmitPositionServerImpl()
	{
	}
		
CLbsTransmitPositionServerImpl::~CLbsTransmitPositionServerImpl()
	{
	iServer.Close();
	}
	
CLbsTransmitPositionServerImpl* CLbsTransmitPositionServerImpl::NewL()
	{
	CLbsTransmitPositionServerImpl* self = new (ELeave) CLbsTransmitPositionServerImpl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLbsTransmitPositionServerImpl::ConstructL()
	{
	/**
	 * See the ServerFramework code for the meaning of this code
	 */
	iParams.SetServerFileName(KNrhServerFileName);
	iParams.SetServerName(KNrhServerName);
	iParams.SetNumberOfServerSlots(-1); // Use messages from the global pool
	iParams.SetServerUids(KNullUid, KNullUid, KLbsNetRequestHandlerUid);
	
	User::LeaveIfError(iServer.Connect(iParams));
	}

RNrhServer& CLbsTransmitPositionServerImpl::Server()
	{
	return iServer;
	}

//
// CLbsTransmitPositionImpl
//

CLbsTransmitPositionImpl::CLbsTransmitPositionImpl()
	{
	
	}
	
CLbsTransmitPositionImpl::~CLbsTransmitPositionImpl()
	{
	iSession.Close();
	}

CLbsTransmitPositionImpl* CLbsTransmitPositionImpl::NewL(RLbsTransmitPositionServer& aServer)
	{
	CLbsTransmitPositionImpl* self = new (ELeave) CLbsTransmitPositionImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aServer);
	CleanupStack::Pop(self);
	return self;	
	}

void CLbsTransmitPositionImpl::ConstructL(RLbsTransmitPositionServer& aServer)
	{
	TNrhX3PData data;
	TNrhSubSessionType type(ENrhSubSessionX3P);
	User::LeaveIfError(iSession.Open<TNrhSubSessionType, TNrhX3PData>(
						aServer.Impl().Server(), 
						type,
						data));
	}

void CLbsTransmitPositionImpl::TransmitPosition(const TDesC& aDestinationID,
												TUint aTransmitPriority, 
												TRequestStatus& aTransmittedPosStatus, 
												TPositionInfo& aTransmittedPosInfo)
	{
	iSession.TransmitPosition(aDestinationID, aTransmitPriority,
							  aTransmittedPosStatus, aTransmittedPosInfo);
	}

void CLbsTransmitPositionImpl::TransmitPosition(const TDesC& aDestinationID,TUint aTransmitPriority,
						                 TRequestStatus& aRefPosStatus, TPositionInfo& aRefPosInfo,
						  TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo)
	{
	iSession.TransmitPosition(aDestinationID, aTransmitPriority, aRefPosStatus,aRefPosInfo,
						      aTransmittedPosStatus, aTransmittedPosInfo);
	}

void CLbsTransmitPositionImpl::CancelTransmitPosition()
	{
	iSession.CancelTransmitPosition();
	}

TInt CLbsTransmitPositionImpl::SetTransmitOptions(const TLbsTransmitPositionOptions& aTransmitOptions)
	{
	return (iSession.SetTransmitOptions(aTransmitOptions));
	}
	
TInt CLbsTransmitPositionImpl::GetTransmitOptions(TLbsTransmitPositionOptions& aTransmitOptions) const
	{
	return (iSession.GetTransmitOptions(aTransmitOptions));
	}


