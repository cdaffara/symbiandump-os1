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
// Bulk Bearer interface.
//



/**
 @file
 @internalComponent
*/

#include "bulkbearerinterface.h"

#include <remcon/remconbearerbulkinterface.h>
#include <remcon/bearersecurity.h>
#include <remcon/clientinfo.h>

#include "bulkserver.h"
#include "remconmessage.h"
#include "utils.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("bulkif");
#endif // _DEBUG

static TBool RemConAddrsMatch(const TRemConAddress& aFirstAddr, const TRemConAddress& aSecondAddr)
	{
	LOG_STATIC_FUNC
	LOG(_L("aFirstAddr = ..."));
	LOGHEXDESC(aFirstAddr.Addr());
	LOG(_L("aSecondAddr = ..."));
	LOGHEXDESC(aSecondAddr.Addr());
	return aFirstAddr == aSecondAddr;
	}

static TUint32 RemConAddrHash(const TRemConAddress& aAddr)
	{
	LOG_STATIC_FUNC
	TBuf8<sizeof(TUid) + TRemConAddress::KMaxAddrSize> buf;
	buf.Append(TPckgC<TUid>(aAddr.BearerUid()));
	buf.Append(aAddr.Addr());
	LOG(_L("Hashing aAddr ..."));
	LOGHEXDESC(buf);
	TUint32 hash = DefaultHash::Des8(buf);
	LOG1(_L("hash = 0x%08x"), hash);
	return hash;
	}

CBulkBearerInterface* CBulkBearerInterface::NewL(CRemConBulkServer& aServer, CBearerManager& aBearerManager)
	{
	LOG_STATIC_FUNC;
	CBulkBearerInterface* self = new(ELeave) CBulkBearerInterface(aServer, aBearerManager);
	CleanupStack::PushL(self);
	self->ConstructL(aBearerManager);
	CleanupStack::Pop(self);
	return self;
	}

CBulkBearerInterface::~CBulkBearerInterface()
	{
	LOG_FUNC;
	
	StopBearers();

	iAddressedClients.Close();	
	iBearerIfs.Close();
	}

CBulkBearerInterface::CBulkBearerInterface(CRemConBulkServer& aServer, CBearerManager& aBearerManager)
	: iAddressedClients(RemConAddrHash, RemConAddrsMatch)
	, iSecurityPoliciesIter(aBearerManager.BearerSecurityPolicies())
	, iServer(aServer)
	{
	LOG_FUNC;
	}

void CBulkBearerInterface::ConstructL(CBearerManager& aBearerManager)
	{
	LOG_FUNC;
	
	aBearerManager.BulkInterfacesL(iBearerIfs);
	if(iBearerIfs.Count() == 0)
		{
		LEAVEL(KErrNotSupported);
		}
	
	TInt err = KErrNone;
	TBool oneStarted = EFalse;
	for(TInt i=0; i<iBearerIfs.Count(); i++)
		{
		ASSERT_DEBUG(iBearerIfs[i].iIf);
		err = iBearerIfs[i].iIf->MrcbbiStartBulk(*this);
		// if we couldn't start bulk service this is of no
		// use to us.  Throw it in the bin.
		if(err)
			{
			iBearerIfs.Remove(i);
			i--;
			}
		else
			{
			oneStarted = ETrue;
			}
		}
	if(!oneStarted)
		{
		LEAVEL(KErrNotSupported);
		}
	
	// Don't store the bearer manager - the less we interact with it the better.
	}

void CBulkBearerInterface::StopBearers()
	{
	for(TInt i=0; i<iBearerIfs.Count(); i++)
		{
		iBearerIfs[i].iIf->MrcbbiStopBulk();
		}
	}

void CBulkBearerInterface::BulkClientAvailable(const TRemConClientId& aClient)
	{
	LOG_FUNC;
	ASSERT_DEBUG(aClient != KNullClientId);
	
	for(TInt i=0; i<iBearerIfs.Count(); i++)
		{
		ASSERT_DEBUG(iBearerIfs[i].iIf);
		iBearerIfs[i].iIf->MrcbbiBulkClientAvailable(aClient);
		}
	}

void CBulkBearerInterface::BulkClientRemoved(const TRemConClientId& aClient)
	{
	LOG_FUNC;
	ASSERT_DEBUG(aClient != KNullClientId);
	
	for(TInt i=0; i<iBearerIfs.Count(); i++)
		{
		ASSERT_DEBUG(iBearerIfs[i].iIf);
		iBearerIfs[i].iIf->MrcbbiBulkClientNotAvailable(aClient);
		}
	// Bearer has been notified so remove the bulk client from any addressing.
	// Apologses for the O(n!) code below...it's the only way we can do it
	// without allocating memory (and the size of the table shouldn't be big
	// enough to cause problems.)
	const TRemConClientId* val = NULL;
	do
		{
		THashMapIter<TRemConAddress, TRemConClientId> iter(iAddressedClients);
		val = iter.CurrentValue();
		do
			{
			if(val && *val == aClient)
				{
				const TRemConAddress* key = iter.CurrentKey();
				ASSERT_DEBUG(key);
				TRemConAddress addr = *key;
				iAddressedClients.Remove(addr);
				// modified the hash map so we must discard the current iterator.
				break;
				}
			}
		while(val = iter.NextValue(), val);
		}
	while(val);
	}

MRemConBearerBulkInterface* CBulkBearerInterface::BearerIf(TUid aBearerUid) const
	{
	LOG_FUNC;
	LOG1(_L8("\taBearerUid = 0x%08x"), aBearerUid);

	MRemConBearerBulkInterface* bearerIf = NULL;

	const TUint numBearerIfs = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < numBearerIfs ; ++ii )
		{
		if ( iBearerIfs[ii].iBearerUid == aBearerUid )
			{
			bearerIf = iBearerIfs[ii].iIf;
			break;
			}
		}

	LOG1(_L8("\tbearerIf = 0x%08x"), bearerIf);
	return bearerIf;
	}

TBool CBulkBearerInterface::CheckPolicy(TUid aBearerUid, const TClientInfo& aClientInfo)
	{
	LOG_FUNC;
	TBool ret = EFalse;
	iSecurityPoliciesIter.SetToFirst();
	TBearerSecurity* sec = NULL;
	while(sec = iSecurityPoliciesIter++, sec)
		{
		if(sec->BearerUid() == aBearerUid)
			{
			if(aClientInfo.Message().IsNull()) // already been complete, so we have to rely on a process handle
				{
				RProcess process;
				TInt err = process.Open(aClientInfo.ProcessId(), EOwnerThread);
				if(err == KErrNone)
					{
					ret = sec->SecurityPolicy().CheckPolicy(process);
					}
				// else we failed to open the handle...so we cannot do any more - fail check.
				process.Close();
				}
			else
				{
				ret = sec->SecurityPolicy().CheckPolicy(aClientInfo.Message());
				}
			break;
			}
		}
	LOG1(_L("\tret = %d"), ret);
	return ret;
	}


TInt CBulkBearerInterface::Send(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOG3(_L8("\taMsg.Addr.BearerUid = 0x%08x, aMsg.InterfaceUid = 0x%08x, aMsg.OperationId = 0x%02x"), 
		aMsg.Addr().BearerUid(), aMsg.InterfaceUid(), aMsg.OperationId());

	MRemConBearerBulkInterface* const bearerIf = BearerIf(aMsg.Addr().BearerUid());
	// Unlike the control server, there is no connection oriented nature, and so 
	// the messages are all connectionless.  However these should all be in response
	// to a message (as only responses are currently supported).
	ASSERT_DEBUG(bearerIf);	

	TInt ret = KErrNone;

	switch ( aMsg.MsgType() )
		{
	case ERemConResponse:
		ret = bearerIf->MrcbbiSendResponse(aMsg.InterfaceUid(), 
			aMsg.OperationId(), 
			aMsg.TransactionId(),
			aMsg.OperationData(), 
			aMsg.Addr());
		if ( ret == KErrNone )
			{
			// On success, the bearer takes ownership of the message data.
			aMsg.OperationData().Assign(NULL);
			}
		break;
	case ERemConReject:
		{
		ASSERT_DEBUG(aMsg.OperationData().Length() == 0);
		bearerIf->MrcbbiSendReject(aMsg.InterfaceUid(), 
			aMsg.OperationId(), 
			aMsg.TransactionId(),
			aMsg.Addr());
		break;
		}
	default:
		DEBUG_PANIC_LINENUM; // the session protects us against this
		break;
		}

	LOG1(_L8("\tret = %d"), ret);
	return ret;
	}

TInt CBulkBearerInterface::MrcbboDoNewCommand(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LOG1(_L8("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TRemConClientId clientId;
	TRAPD(err, clientId = iAddressedClients.FindL(aAddr));
	
	if(err == KErrNone)
		{
		TRAP(err, NewCommandL(aAddr, clientId));
		}

	LOG1(_L8("\terr = %d"), err);
	return err;
	}

TInt CBulkBearerInterface::MrcbboDoNewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG_FUNC;
	LOG1(_L8("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	LOG1(_L8("\taClient = 0x%08x"), aClient);

	TRAPD(err, NewCommandL(aAddr, aClient));

	LOG1(_L8("\terr = %d"), err);
	return err;
	}

void CBulkBearerInterface::NewCommandL(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG_FUNC;
	LOG1(_L8("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	LOG1(_L8("\taClient = 0x%08x"), aClient);

	// Get the calling bearer from aAddr and get the new command from it.
	MRemConBearerBulkInterface* const bearerIf = BearerIf(aAddr.BearerUid());
	ASSERT_DEBUG(bearerIf);
	TUid interfaceUid;
	TUint transactionId;
	TUint operationId;
	RBuf8 data;
	TRemConAddress addr;
	LEAVEIFERRORL(bearerIf->MrcbbiGetCommand(interfaceUid,
		transactionId, 
		operationId, 
		data, 
		addr));
	LOG3(_L8("\treceived command with interfaceUid [0x%08x], operationId 0x%02x, data.Length = %d"), 
		interfaceUid, operationId, data.Length());
	// We now own what's pointed to by 'data'.
	CleanupClosePushL(data);

	CRemConMessage* msg = CRemConMessage::NewL(
		aAddr,
		aClient,
		ERemConCommand,
		ERemConMessageDefault,
		interfaceUid,
		operationId,
		data,
		0, // session ID as yet unknown
		transactionId);
	// 'msg' now has a pointer to the memory pointed to by 'data', and owns 
	// it.
	CLEANUPSTACK_POP1(&data);
	// Give the new command to the server, which takes ownership of it. 
	iServer.NewCommand(*msg);
	}

TUint CBulkBearerInterface::MrcbboDoNewTransactionId()
	{
	LOG_FUNC;
	TUint newId = iRunningTransactionId;

	if ( iRunningTransactionId == KMaxTUint )
		{
		iRunningTransactionId = 0;
		}
	else
		{
		++iRunningTransactionId;
		}
	
	LOG1(_L8("CBulkBearerInterface::MrcbboDoNewTransactionId newId = %d"), newId);
	return newId;
	}

void CBulkBearerInterface::MrcbboDoCommandExpired(TUint aTransactionId)
	{
	LOG_FUNC;
	iServer.CommandExpired(aTransactionId);
	}

TInt CBulkBearerInterface::MrcbboDoSetAddressedClient(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG_FUNC;
	return iAddressedClients.Insert(aAddr, aClient); // hash map should cover uniqueness
	}

TInt CBulkBearerInterface::MrcbboDoRemoveAddressing(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	return iAddressedClients.Remove(aAddr);
	}



