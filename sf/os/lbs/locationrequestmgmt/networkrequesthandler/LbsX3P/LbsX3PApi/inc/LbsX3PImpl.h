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
// Classes that implement the functionality for the LbsX3p Api.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSX3PIMPL_H_
#define LBSX3PIMPL_H_

#include <lbs/lbsx3p.h>
#include "lbsx3pdata.h"
#include "rsecuresessionbase.h"


class TPositionInfo;
class TLbsTransmitPositionOptions;

	
// This is really a session with server
NONSHARABLE_CLASS(RNrhServer) : public RSecureSessionBase
	{
public:
	RNrhServer();
	~RNrhServer();
	TVersion Version();
	TInt StartServer(TServerStartParams& aParams);
	
	void StartLbsL();
	};	
	
// This is really a subsession of the above
NONSHARABLE_CLASS(RNrhSession) : public RSecureSubSessionBase
	{
public:
	RNrhSession();
	~RNrhSession();
	
	void TransmitPosition(const TDesC& aDestinationID,TUint aTransmitPriority, 
							TRequestStatus& aStatus, TPositionInfo& aTransmittedPosInfo);
							
	void TransmitPosition(const TDesC& aDestinationID,TUint aTransmitPriority,
						  TRequestStatus& aRefPosStatus, TPositionInfo& aRefPosInfo,
						  TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo);

	void CancelTransmitPosition();

	TInt SetTransmitOptions(const TLbsTransmitPositionOptions& aTransmitOptions);
	TInt GetTransmitOptions(TLbsTransmitPositionOptions& aTransmitOptions) const;

private:

	void ReportReferenceLocation(TRequestStatus& aStatus, TPositionInfo& aReferencePosInfo);

private:
	// variables used by asynchronous functions
	TPtr8 iTransmittedPosInfoPtr;
	TPtr8 iTransmitPositionParamsPtr;
	TTransmitPositionParams iTransmitPositionParams;
	
	TPtr8 iReferencePosInfoPtr;
	TPtr8 iReferencePositionParamsPtr;
	TTransmitPositionParams iReferencePositionParams;
	};


/*
*/
NONSHARABLE_CLASS(CLbsTransmitPositionServerImpl) : public CBase
	{
public:
	static CLbsTransmitPositionServerImpl* NewL();
	~CLbsTransmitPositionServerImpl();
	
	RNrhServer& Server();

private:
	void ConstructL();
	CLbsTransmitPositionServerImpl();

private:
	RNrhServer iServer;
	TServerStartParams 	iParams; // MUST live for as long as the client
	};

/*
*/	
NONSHARABLE_CLASS(CLbsTransmitPositionImpl) : public CBase
	{
public:
	static CLbsTransmitPositionImpl* NewL(RLbsTransmitPositionServer& aServer);
	~CLbsTransmitPositionImpl();

public:
	void TransmitPosition(const TDesC& aDestinationID,TUint aTransmitPriority, 
						  TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo);
	
	void TransmitPosition(const TDesC&aDestinationID,TUint aTransmitPriority,
						  TRequestStatus& aRefPosStatus, TPositionInfo& aRefPosInfo,
						  TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo);
	
	void CancelTransmitPosition();

	TInt SetTransmitOptions(const TLbsTransmitPositionOptions& aTransmitOptions);
	TInt GetTransmitOptions(TLbsTransmitPositionOptions& aTransmitOptions) const;

private:
	void ConstructL(RLbsTransmitPositionServer& aServer);
	CLbsTransmitPositionImpl();
	
private:
	RNrhSession iSession;
	};


#endif // LBSX3PIMPL_H_

