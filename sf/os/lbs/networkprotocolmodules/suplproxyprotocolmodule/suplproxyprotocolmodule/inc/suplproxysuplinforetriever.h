// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of class used to manage the handling of GetSuplInfo messages
// 
//
/**
 @file
 @internalComponent
 */

#ifndef SUPLPROXYSUPLINFORETRIEVER_H
#define SUPLPROXYSUPLINFORETRIEVER_H

#include <e32base.h>
#include <lbs/lbssuplproxyposclient.h>
#include <lbs/lbsreflocdatasourcebase.h>

class MSuplProxyPosProtocol;
class CSuplPosPayload;

class CSuplProxySuplInfoRetriever :  public CBase, public MLbsRefLocationObserver
	{
public:
	static CSuplProxySuplInfoRetriever* NewL(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId);
	~CSuplProxySuplInfoRetriever();
	
	void NewRequest(const RMessage2& aMessage, TInt aRequestId);
	void CancelRequest(const RMessage2& aMessage);
	TInt CancelRequest();
	void ResetState();
	void SetSlpAddress(const TDesC& aAddress);
	
	void ReportPosPayloadError(TInt aError);
	void PositioningPayloadToNetwork(TBool aSessionEnded, const CSuplPosPayload& aPositioningPayload);
	void EncodePosPayloadL(const CSuplPosPayload& aPositioningPayload);
	TBool SuplInfoPayloadReqPending();
    void UpdatePosPayloadInfo();
	void UpdatePendindDataReqMask(TInt aError, TLbsAsistanceDataGroup aDataRequestMask);
	
	// from MLbsRefLocationObserver
    void HandleRefPositionInformation(TInt aError, const TPositionInfoBase* aFirstPosition, const TPositionAreaInfoBase* aFistCellId, 
                const TPositionInfoBase* aSecondPosition, const TPositionAreaInfoBase* aSecondCellId);

private:
    CSuplProxySuplInfoRetriever(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId);
    void CompleteMessageIfReady();
    
    void LoadRefLocSrcL(TInt aRefLocSourceId);
	
private:
	// The outstanding message
	RMessage2 iMessage;
    // used to find out which data requested in GetSuplInfo and to build up a response 
    TSuplInfoRequestData iSuplInfoData;
    // interface for talking to LBS subsystem:
    MSuplProxyPosProtocol& iPosProtocol;
    // The outstanding data mask
    TInt iOutstandingItemMask;
    // Error reported by the positioning protocol (rrlp fsm)
    TInt	iPositioningError;
    // The source for the reference location
    CLbsRefLocationSourceBase* iRefLocationSrc;
    // Set if HandlePosPayloadL() has arrived before the HandleGetSuplInfo()
    TBool iPosPayloadAvailable;
    //To hold the encoded payload until HandleGetSuplInfo() arrives
    HBufC8* iPosPayloadBuffer;
    //This will be the last pospayload for this rrlp session
    TBool iLastPosPayload;
	};


#endif // SUPLPROXYSUPLINFORETRIEVER_H
