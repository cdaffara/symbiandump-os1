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

#ifndef LBSLASBTDATAREADER_H
#define LBSLASBTDATAREADER_H

//************************************************************************************************************
//System
#include <e32base.h>
#include <es_sock.h>
#include <bt_sock.h>
#include <btsdp.h>
#include <BTExtNotifiers.h>

const TInt KMaxBtDataRead   = 200;

class MLasDataSourceObserver;

//************************************************************************************************************
//   CLasBtDataReader
//************************************************************************************************************
class CLasBtDataReader : public CActive, public MSdpAgentNotifier, public MSdpAttributeValueVisitor
	{
public:
	static CLasBtDataReader* NewL(MLasDataSourceObserver* aObserver);
	void ConstructL();
	~CLasBtDataReader();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	
	// From MSdpAgentNotifier
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);
	
	// From MSdpAttributeValueVisitor
	virtual void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
   virtual void StartListL(CSdpAttrValueList &aList);
   virtual void EndListL();

	TInt OpenDataSource();
	void CloseDataSource();
	void MonitorDataSource();
	void CancelMonitor();

private:
	CLasBtDataReader(MLasDataSourceObserver* aObserver);

	void CompleteSelf(TInt aReason);
	
	void StartBtSourceSearchL();
	
	void DoStartBtSourceQueryL();
	TInt StartBtSourceQuery();

	TInt GetBtDeviceAddr(TRfcommSockAddr& aAddr);
	
	void CopyBufferData(TDes8& aData);

private:
	RSocketServ     iSs;
	RSocket         iBtSock;
	TSockXfrLength  iXfrLength;
	TRfcommSockAddr iBtDevice;
	CSdpAgent*      iSdpAgent;
	RNotifier       iNotifier;
	
	TBTDeviceSelectionParamsPckg iSelectionFilterPckg;
	TBTDeviceResponseParamsPckg  iResultPckg;
	
	enum TBtConnectState {EStateDeviceUnknown, EStateSearching, EStateDisconnected, EStateQuerying,EStateConnecting, EStateOnLine, EStateError};
	
	TProtocolDesc   iBtProtoInfo;
	TBtConnectState iBtConnectState;

	MLasDataSourceObserver* iObserver;
	TBool iMonitoring;

	TBuf8<KMaxBtDataRead> iData;
	TInt iErrorCount;
	};


#endif // LBSLASBTDATAREADER_H
