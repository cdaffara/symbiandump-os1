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
 @internalComponent
*/

#ifndef OOBDATA_H
#define OOBDATA_H

#include <e32base.h>
#include <bttypes.h>
#include <bluetooth/hcicommandqueueclient.h>
#include "internaltypes.h"

class CPairingServer;
class MHCICommandQueue;

class COobDataManager;
class CBTSecMan;


NONSHARABLE_CLASS(TOobData)
	{
public:
	TOobData(const TBTDevAddr& aAddr, const TBluetoothSimplePairingHash& aHashC, const TBluetoothSimplePairingRandomizer& aRandomizerR);
	
	const TBTDevAddr& Addr() const;
	const TBluetoothSimplePairingHash& HashC() const;
	const TBluetoothSimplePairingRandomizer& RandomizerR() const;
	
	static TBool IsForAddress(const TBTDevAddr* aAddr, const TOobData& aOobData);
	
private:
	TBTDevAddr iAddr;
	TBluetoothSimplePairingHash iHashC;
	TBluetoothSimplePairingRandomizer iRandomizerR;
	};



NONSHARABLE_CLASS(XOobLocalDataObserver)
	{
friend class COobDataManager;
private:
	virtual void XoldoLocalOobDataRetrieved(TInt aResult, const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer) = 0;
	
private:
	TBTDblQueLink	iLink;
	};


NONSHARABLE_CLASS(COobDataManager)
	: public CBase
	, public MHCICommandQueueClient
	{
public:
	static COobDataManager* NewL(CBTSecMan& aSecMan);
	~COobDataManager();
	
	void SetHCICommandQueue(MHCICommandQueue& aCommandQueue);
	void ClearHCICommandQueue();
	
public:
	void RefreshLocalOobData();
	void ReadLocalOobData(XOobLocalDataObserver& aObserver);
	void CancelReadLocalOobData(XOobLocalDataObserver& aObserver);
	void ProvideRemoteOobDataL(const TOobData& aData);
	void ClearRemoteOobData(const TBTDevAddr& aAddr);

	TBool GetRemoteOobData(const TBTDevAddr& aAddr, TBluetoothSimplePairingHash& aHashC, TBluetoothSimplePairingRandomizer& aRandomizerR);
	TBool HasRemoteOobData(const TBTDevAddr& aAddr);
	
	MHCICommandQueue& CommandQueueL() const;

private:
	COobDataManager(CBTSecMan& aSecMan);
	void ConstructL();
	void MakeLocalOobRequestToController();
	void MakeLocalOobRequestToControllerL();
	void CompleteQueuedLocalOobObservers(TInt aResult);
	
private: // from MHCICommandQueueClient
	void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
private:
	enum TLocalOobState
		{
		ELocalOobInvalid,
		ELocalOobUpdating,
		ELocalOobValid,
		};
	
	
private: // owned
	// Remote OOB Data
	RArray<TOobData>					iOobDataFromPeers;
	
	// Local OOB Data
	TLocalOobState						iLocalOobState;
	TBluetoothSimplePairingHash			iOobHashC;
	TBluetoothSimplePairingRandomizer	iOobRandomizerR;
	
private: // unowned
	TDblQue<XOobLocalDataObserver>		iLocalOobObservers;
	MHCICommandQueue*					iCommandQueue;
	CBTSecMan&							iSecMan;
	};


_LIT(KOobDataPanicCat, "OobDataManager");
enum TOobDataPanicCode
	{
	EOobDataPanicCommandQueueAlreadyProvided,
	EOobDataPanicMultipleOobEntriesForDevice,
	EOobDataPanicLocalOobObserverIsAlreadyQueuedDuringUpdate,
	EOobDataPanicLocalOobObserverIsAlreadyQueuedWhileInvalid,
	EOobDataPanicUnknownState,
	EOobDataPanicUnexpectedHciCommandAssociated,
	EOobDataPanicUnexpectedHciCommandAssociatedWithError,
	EOobDataPanicUnexpectedHciEventReceived,
	EOobDataPanicUnexpectedHciCommandCompleteEventReceived,
	EOobDataPanicErrorWhenFindingOobDataForProvision,
	EOobDataPanicErrorWhenFindingOobDataForClear,
	EOobDataPanicErrorWhenFindingOobDataForGet,
	EOobDataPanicErrorWhenFindingOobDataForHas,
	};

#endif // OOBDATA_H

