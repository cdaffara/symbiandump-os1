// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _EIRMANAGER_H
#define _EIRMANAGER_H

#include <e32base.h>
#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/eirmanshared.h>

class CLinkMgrProtocol;
class MHCICommandQueue;

// Magic numbers

const TUint8 KOfferPending		= 0x01;
const TUint8 KNeedOffer 		= 0x02;

const TInt KLengthInvalid = -1;

// Reserved bytes for each tag, this is used by the 1st iteration of calculation
const TInt KReservedNameBytes = 101;
const TInt KReservedUuid16Bytes = 50;
const TInt KReservedUuid128Bytes = 50;
const TInt KReservedManufacturerSpecificDataBytes = 22;
const TInt KReservedTxPowerLevelBytes = 3;

const TInt KDeviceNameGranularity = 1;
const TInt K16BitUuidGranularity = 2;
const TInt K128BitUuidGranularity = 16;

const TInt KWriteEirTimeout = 2000000; // 2 seconds

class MEirManagerNotifiee
	{
public:
	virtual void MemnEirBlockAvailable(TEirTag aTag, TUint aSpaceForTag)=0;
	};

class REirOwnerDetails
	{
public:
	HBufC8*			iTagData;
	MEirManagerNotifiee*	iNotifiee;
	TEirDataMode 	iEirDataMode;
	TInt			iOfferedBytes;
	TInt			iRequiredBytes;
	TUint8			iState;
	};

class CEirManager;

NONSHARABLE_CLASS(CWriteEirTimer) : public CTimer
	{
	public:
		static CWriteEirTimer* NewL(CEirManager& aEirManager);
		void Start();
	private:
		CWriteEirTimer(CEirManager& aEirManager);
		void ConstructL();
		void RunL();
		TInt RunError(TInt aError);
	private:
		CEirManager& iEirManager;
	};

NONSHARABLE_CLASS(CEirManager) : public CBase, public MHCICommandQueueClient
	{
public:
	static CEirManager* NewL(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol);
	~CEirManager();

	TInt RegisterTag(TEirTag aTag, MEirManagerNotifiee& aNotifiee);
	void DeregisterTag(TEirTag aTag);
	TInt SetData(TEirTag aTag, const TDesC8& aData, TEirDataMode aEirDataMode);
	
	// from MHCICommandQueueClient
	void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
	TInt NewData(TEirTag aTag, TInt aRequiredBytes);
	void TryToUpdateHostController();
	void UpdateHostControllerL();

private:
	CEirManager(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol);
	void ConstructL();
	
	void UnhookClient(TEirTag aTag);
	inline TBool TagValid(TEirTag aTag) const;
	TBool TagAllowed(TEirTag aTag) const;
	TBool TagPresent(TEirTag aTag) const;
	TBool OffersPending() const;
	TBool NeedToOffer() const;
	TInt TagCount() const;
	void NotifyClients();
	void DoNotify(TEirTag aTag);
	void FindTagData(TEirTag aTag, HBufC8**& aOutPointerToData);
	TInt EIRPacketSize() const;
	void ComputeOfferLengths();
	void ComputeClientInitialOffer(TEirTag aTag, TInt& aTotalSpare, TInt aReservedBytes,
								   TBool aIsAllOrNothing = EFalse);
	void ComputeClientFinalOffer(TEirTag aTag, TInt& aTotalSpare, TUint aGranularity,
								 TBool aIsAllOrNothing = EFalse);
	void WriteExtendedInquiryResponseL(TBool aFECRequired, const TDesC8& aEir);

private:
	TFixedArray<REirOwnerDetails, EEirTagRESERVED>  iNotifiees;
	TEirTag					iCurrentlySettingTag;
	TUint					iLengthForNotifiee;
	TBool					iOffersStale;
	TUint					iCmdId;
	TUint					iCmdCount;

	// unowned
	MHCICommandQueue& 		iCommandQueue;
	CLinkMgrProtocol&		iLinkMgrProtocol;
	// owned
	CWriteEirTimer*			iWriteEirTimer;
	};
	
#include "eirmanager.inl"

#endif // _EIRMANAGER_H
