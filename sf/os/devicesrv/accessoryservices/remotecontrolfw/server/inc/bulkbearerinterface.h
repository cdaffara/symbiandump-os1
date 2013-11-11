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

#ifndef BULKBEARERINTERFACE_H
#define BULKBEARERINTERFACE_H

#include <e32base.h>
#include <e32hashtab.h>
#include <remcon/remconbearerbulkobserver.h>
#include "bearermanager.h"

class CRemConBulkServer;
class CRemConMessage;
class TBearerSecurity;
class MRemConBearerBulkInterface;
class TClientInfo;

NONSHARABLE_CLASS(CBulkBearerInterface) : public CBase, public MRemConBearerBulkObserver
	{
public:
	/** 
	Bearer manager construction.
	@return Ownership of a new CBulkBearerInterface object
	*/
	static CBulkBearerInterface* NewL(CRemConBulkServer& aServer, CBearerManager& aBearerManager);

	~CBulkBearerInterface();

public:
	void BulkClientAvailable(const TRemConClientId& aClient);
	void BulkClientRemoved(const TRemConClientId& aClient);
	
	/** Send a message over a specific connection and return the result 
	immediately. */
	TInt Send(CRemConMessage& aMsg);
	
	/**
	@param aBearerUid The bearer with which to check the security policy with.
	@param aMessage The message to check the policy against.
	@return non-zero if the security check passes, zero otherwise.
	*/
	TBool CheckPolicy(TUid aBearerUid, const TClientInfo& aClientInfo);

private:
	CBulkBearerInterface(CRemConBulkServer& aServer, CBearerManager& aBearerManager);
	void ConstructL(CBearerManager& aBearerManager);
	void StopBearers();

private: // from MRemConBearerBulkObserver
	TInt MrcbboDoNewCommand(const TRemConAddress& aAddr);
	TInt MrcbboDoNewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	TUint MrcbboDoNewTransactionId();
	void MrcbboDoCommandExpired(TUint aTransactionId);
	TInt MrcbboDoSetAddressedClient(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	TInt MrcbboDoRemoveAddressing(const TRemConAddress& aAddr);
	
private: // utility
#ifdef __FLOG_ACTIVE
	void LogBearers() const;
#endif
	
	void NewCommandL(const TRemConAddress& aAddr, const TRemConClientId& aClient);

	MRemConBearerBulkInterface* BearerIf(TUid aUid) const;

private: // unowned	
	RArray<CBearerManager::TBulkInterfaceInfo> iBearerIfs;
	RHashMap<TRemConAddress, TRemConClientId> iAddressedClients; // Stores client id by RemCon addr

private: // owned
	TUint iRunningTransactionId;
	TSglQueIter<TBearerSecurity> iSecurityPoliciesIter;

private: // unowned
	CRemConBulkServer& iServer;
	};

#endif // BULKBEARERINTERFACE_H
