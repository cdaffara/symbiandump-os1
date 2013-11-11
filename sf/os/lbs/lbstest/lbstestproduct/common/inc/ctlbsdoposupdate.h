/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ctlbsdoposupd.h
*/
#ifndef  __CT_LBS_DOPOS_UPDATE_H__
#define  __CT_LBS_DOPOS_UPDATE_H__

#include <e32base.h>
#include <lbs.h>


_LIT(KCT_LbsDoPosUpdate, "CT_LbsDoPosUpdate");


// Async callbacks
class MT_LbsDoPosUpdateObserver
	{
public:
	virtual void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus) = 0;	// do we need the aStatus...
	};
	
	
class CT_LbsDoPosUpdate: public CActive
	{
public:
	~CT_LbsDoPosUpdate();
	static CT_LbsDoPosUpdate* NewL(MT_LbsDoPosUpdateObserver* aObserver, TPositionModuleInfo::TTechnologyType aTechnologyType = TPositionModuleInfo::ETechnologyUnknown);
    static CT_LbsDoPosUpdate* NewL(MT_LbsDoPosUpdateObserver* aObserver, const TPositionCriteriaBase& aCriteria);
	TInt SetOptions(const TPositionUpdateOptionsBase& aPosOption);
	void StartL(TPositionInfo& aPosInfo);
	void CancelRequest();
	TInt CancelRequestWithResult();
	void OpenPositioner(TPositionModuleInfo::TTechnologyType aTechnologyType = TPositionModuleInfo::ETechnologyUnknown);
	void ClosePositioner(void);
	void ConnectServer(void);
	void CloseServer(void);
	TInt CompleteRequest(TInt aRequestId);
	TInt DoCancelWithResult();
	
protected:
	//	Functions from CActive:
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
private:
	enum TOpenSubSessionType
		{
		EOpenSubSessionByModuleId,  //Open subsession with the location server by module id
		EOpenSubSessionByCriteria    //Open subsession with the location server by criteria 
		};
		
	CT_LbsDoPosUpdate(MT_LbsDoPosUpdateObserver* aObserver);
	
	void CT_LbsDoPosUpdate::ConstructL(TOpenSubSessionType aOpenSubSessionType,TPositionModuleInfo::TTechnologyType aTechnologyType, const TPositionCriteriaBase& aCriteria);
	
	// caller
	MT_LbsDoPosUpdateObserver* iObserver;
	
	// Self locate server.
	RPositionServer iServer;
	RPositioner iPositioner;
	};

#endif // __CT_LBS_DOPOS_UPDATE_H__