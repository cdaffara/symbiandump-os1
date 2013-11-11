// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Bearer manager.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#include <remcon/remconbearerplugin.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/remconbearerbulkinterface.h>
#include <remcon/bearerparams.h>
#include <remcon/bearersecurity.h>
#include <remcon/remconconverterinterface.h>
#include "bearermanager.h"
#include "utils.h"
#include "server.h"
#include "session.h"
#include "remconmessage.h"
#include "connections.h"

PANICCATEGORY("bearerman");

#ifdef __FLOG_ACTIVE
#define LOGBEARERS	LogBearers()
#else
#define LOGBEARERS
#endif
 
static TBool RemConAddrsMatch(const TRemConAddress& aFirstAddr, const TRemConAddress& aSecondAddr)
	{
	return aFirstAddr == aSecondAddr;
	}

CBearerManager* CBearerManager::NewL(CRemConServer& aServer)
	{
	LOG_STATIC_FUNC
	CBearerManager* self = new(ELeave) CBearerManager(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP1(self);
	return self;
	}

CBearerManager::~CBearerManager()
	{
	LOG_FUNC;

	// We do not check these are empty before cleaning them up. There are no 
	// cancel methods for connect/disconnect in the bearer API.
	iCurrentlyBeingConnected.Close();
	iCurrentlyBeingDisconnected.Close();
	
	LOGBEARERS;
	// Destroy all bearer instances.
	iBearers.ResetAndDestroy();
	LOGBEARERS;

	iBearerIfs.Close();

	// Clean up the security policy collection.
	TSglQueIter<TBearerSecurity> iter(iBearerSecurityPolicies);
	TBearerSecurity* sec;
	while ( ( sec = iter++ ) != NULL )
		{
		iBearerSecurityPolicies.Remove(*sec);
		delete sec;
		}
	}

CBearerManager::CBearerManager(CRemConServer& aServer)
:	iBearerSecurityPolicies(_FOFF(TBearerSecurity, iLink)),
	iServer(aServer)
	{
	LOG_FUNC
	}

void CBearerManager::ConstructL()
	{
	LOG_FUNC;

	// Instantiate all bearers at construction time.
	LoadAllBearersL();

	LOGBEARERS;
	}

MRemConBearerInterface* CBearerManager::BearerIf(TUid aBearerUid) const
	{
	LOG_FUNC;
	LOG1(_L("\taBearerUid = 0x%08x"), aBearerUid);

	MRemConBearerInterface* bearerIf = NULL;

	const TUint numBearerIfs = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < numBearerIfs ; ++ii )
		{
		if ( iBearerIfs[ii].iBearerUid == aBearerUid )
			{
			// In the current implementation we only have three bearer interface 
			// UIDs. This UID is tied to the version of MRemConBearerInterface.
			ASSERT_DEBUG(iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface1) || 
						 iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface2) ||
						 iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface3));
			bearerIf = iBearerIfs[ii].iIf;
			break;
			}
		}

	LOG1(_L("\tbearerIf = 0x%08x"), bearerIf);
	return bearerIf;
	}

MRemConBearerInterfaceV2* CBearerManager::BearerIfV2(TUid aBearerUid) const
	{
	LOG_FUNC;
	LOG1(_L("\taBearerUid = 0x%08x"), aBearerUid);
	
	MRemConBearerInterfaceV2* bearerIfV2 = NULL;
	
	const TUint numBearerIfs = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < numBearerIfs ; ++ii )
		{
		if ( iBearerIfs[ii].iBearerUid == aBearerUid )
			{
			// In the current implementation we only have two bearer interface 
			// UIDs. This UID is tied to the version of MRemConBearerInterface.
			ASSERT_DEBUG(iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface1) || 
						 iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface2) ||
						 iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface3));
			
			if((iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface2)) ||
			   (iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface3)))
				{
				bearerIfV2 = iBearerIfs[ii].iIfV2;
				}
			
			break;
			}
		}
	
	LOG1(_L("\tbearerIfV2 = 0x%08x"), bearerIfV2);
	return bearerIfV2;
	}

MRemConBearerInterfaceV3* CBearerManager::BearerIfV3(TUid aBearerUid) const
	{
	LOG_FUNC;
	LOG1(_L("\taBearerUid = 0x%08x"), aBearerUid);
	
	MRemConBearerInterfaceV3* bearerIfV3 = NULL;
	
	const TUint numBearerIfs = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < numBearerIfs ; ++ii )
		{
		if ( iBearerIfs[ii].iBearerUid == aBearerUid )
			{
			// In the current implementation we only have three bearer interface 
			// UIDs. This UID is tied to the version of MRemConBearerInterfaceV3.
			ASSERT_DEBUG(iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface1) || 
						 iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface2) ||
						 iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface3));
			
			if(iBearerIfs[ii].iIfUid == TUid::Uid(KRemConBearerInterface3))
				{
				bearerIfV3 = iBearerIfs[ii].iIfV3;
				}
			
			break;
			}
		}
	
	LOG1(_L("\tbearerIfV3 = 0x%08x"), bearerIfV3);
	return bearerIfV3;
	}

void CBearerManager::LoadAllBearersL()
	{
	LOG_FUNC;

	// Use ECOM to instantiate each implementation of the bearer plugin 
	// interface.
	const TUid KUidRemoteControlBearerPluginInterface = TUid::Uid(KRemConBearerInterfaceUid);
	RImplInfoPtrArray implementations;
	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidRemoteControlBearerPluginInterface, 
		noResolverParams, 
		KRomOnlyResolverUid, 
		implementations);
	CleanupResetAndDestroyPushL(implementations);
	const TUint count = implementations.Count();
	LOG1(_L("\tnumber of implementations of bearer plugin interface: %d"), count);
	// There should be at least one bearer present. If there are no bearers in 
	// the ROM, then there shouldn't be RemCon server present either due to 
	// the waste of ROM. This is why the client-side is a separate DLL, so it 
	// can be included to satisfy any static linkages, without bringing the 
	// unnecessary bulk of the server with it.
	ASSERT_ALWAYS(count != 0);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CImplementationInformation* impl = implementations[ii];
		ASSERT_DEBUG(impl);
		LOG(_L("\tloading bearer with:"));
		LOG1(_L("\t\timplementation uid 0x%08x"), impl->ImplementationUid());
		LOG1(_L("\t\tversion number %d"), impl->Version());
		TBuf8<KMaxName> buf8;
		buf8.Copy(impl->DisplayName());
		LOG1(_L8("\t\tdisplay name \"%S\""), &buf8);
		LOG1(_L("\t\tROM only %d"), impl->RomOnly());
		LOG1(_L("\t\tROM based %d"), impl->RomBased());

		TBearerParams params(impl->ImplementationUid(), *this);
		CRemConBearerPlugin* bearer = CRemConBearerPlugin::NewL(params);
		CleanupStack::PushL(bearer);
		LEAVEIFERRORL(iBearers.Append(bearer));
		CLEANUPSTACK_POP1(bearer);

		// Also get information about the interface the bearer presents.
		// Look for latest interface first
		TInterfaceInfo ifInfo;
		ifInfo.iIfUid = TUid::Uid(0);
		
		// Control interfaces
		ifInfo.iIfV3 = reinterpret_cast<MRemConBearerInterfaceV3*>(bearer->GetInterface(TUid::Uid(KRemConBearerInterface3)));
		ifInfo.iIfV2 = reinterpret_cast<MRemConBearerInterfaceV2*>(bearer->GetInterface(TUid::Uid(KRemConBearerInterface2)));
		ifInfo.iIf = reinterpret_cast<MRemConBearerInterface*>(bearer->GetInterface(TUid::Uid(KRemConBearerInterface1)));
		if(ifInfo.iIfV3)
			{
			ifInfo.iIfUid = TUid::Uid(KRemConBearerInterface3);
			}
		else if(ifInfo.iIfV2)
			{
			ifInfo.iIfUid = TUid::Uid(KRemConBearerInterface2);
			}
		else if(ifInfo.iIf)
			{
			ifInfo.iIfUid = TUid::Uid(KRemConBearerInterface1);
			}

		LOG1(_L8("\t\tcontrol interface (V1) = [0x%08x]"), ifInfo.iIf);
		LOG1(_L8("\t\tcontrol interface (V2) = [0x%08x]"), ifInfo.iIfV2);
		LOG1(_L8("\t\tcontrol interface (V3) = [0x%08x]"), ifInfo.iIfV3);
		// If the bearer doesn't support the basic bearer API, panic server 
		// startup.
		ASSERT_ALWAYS(ifInfo.iIf);

		ifInfo.iBearerUid = impl->ImplementationUid();
		ifInfo.iControllerCount = 0;
		ifInfo.iTargetCount = 0;
		LEAVEIFERRORL(iBearerIfs.Append(ifInfo));

		TBearerSecurity* sec = new(ELeave) TBearerSecurity(bearer->Uid(), ifInfo.iIf->SecurityPolicy());
		iBearerSecurityPolicies.AddLast(*sec);
		}

	CleanupStack::PopAndDestroy(&implementations);
	}

#ifdef __FLOG_ACTIVE
void CBearerManager::LogBearers() const
	{
	const TUint count = iBearers.Count();
	LOG1(_L("\tNumber of bearers = %d"), count);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		const CRemConBearerPlugin* const bearer = iBearers[ii];
		ASSERT_DEBUG(bearer);
		LOG3(_L("\t\tbearer %d [0x%08x], Uid = 0x%08x"), 
			ii, 
			bearer,
			bearer->Uid()
			);
		}
	}
#endif // __FLOG_ACTIVE

TInt CBearerManager::Send(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOG4(_L("\taMsg.Addr.BearerUid = 0x%08x, aMsg.InterfaceUid = 0x%08x, aMsg.MsgType = %d, aMsg.OperationId = 0x%02x"), 
		aMsg.Addr().BearerUid(), aMsg.InterfaceUid(), aMsg.MsgType(), aMsg.OperationId());

	MRemConBearerInterface* const bearerIf = BearerIf(aMsg.Addr().BearerUid());
	MRemConBearerInterfaceV2* const bearerIfV2 = BearerIfV2(aMsg.Addr().BearerUid());
	MRemConBearerInterfaceV3* const bearerIfV3 = BearerIfV3(aMsg.Addr().BearerUid());
	// For connection-oriented sends, the session protects against trying to 
	// connect to a non-existent bearer. For connectionless sends, the TSP can 
	// indicate whatever connections it like, but if it indicates one 
	// belonging to a non-existent bearer, that's panicked.
	ASSERT_DEBUG(bearerIf); 

	TInt ret = KErrNone;

	switch ( aMsg.MsgType() )
		{
	case ERemConNotifyCommand:
	    // We originate a transaction identifier as it's a new outgoing 
	    // notify message.
		aMsg.TransactionId() = MrcboDoNewTransactionId();
		ASSERT_DEBUG(bearerIfV3);
		ret = bearerIfV3->SendNotifyCommand(aMsg.InterfaceUid(), 
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
	case ERemConCommand:
		// We originate a transaction identifier as it's a new outgoing 
		// message.
		aMsg.TransactionId() = MrcboDoNewTransactionId();
		ret = bearerIf->SendCommand(aMsg.InterfaceUid(), 
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

	case ERemConResponse:
		ret = bearerIf->SendResponse(aMsg.InterfaceUid(), 
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
		if (bearerIfV2)
			{
			bearerIfV2->SendReject(aMsg.InterfaceUid(), 
					aMsg.OperationId(), 
					aMsg.TransactionId(),
					aMsg.Addr());
			}
		break;
		}
	default:
		DEBUG_PANIC_LINENUM; // the session protects us against this
		break;
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}


TInt CBearerManager::Connect(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TInt ret = KErrNone;
	if ( iCurrentlyBeingConnected.Find(aAddr, RemConAddrsMatch) == KErrNotFound )
		{
		MRemConBearerInterface* const bearerIf = BearerIf(aAddr.BearerUid());
		// The caller should have checked that the interface existed before 
		// calling this.
		ASSERT_DEBUG(bearerIf);
		// Make a note of the address. If we can't do this then fail the connect.
		ret = iCurrentlyBeingConnected.Append(aAddr);
		if ( ret == KErrNone )
			{
			bearerIf->ConnectRequest(aAddr);
			}
		}
	
	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TInt CBearerManager::Disconnect(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TInt ret = KErrNone;
	if ( iCurrentlyBeingDisconnected.Find(aAddr, RemConAddrsMatch) == KErrNotFound )
		{
		MRemConBearerInterface* const bearerIf = BearerIf(aAddr.BearerUid());
		// The caller should have checked that the interface existed before 
		// calling this.
		ASSERT_DEBUG(bearerIf);
		// Make a note of the address. If we can't do this then fail the 
		// disconnect.
		ret = iCurrentlyBeingDisconnected.Append(aAddr);
		if ( ret == KErrNone )
			{
			bearerIf->DisconnectRequest(aAddr);
			}
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TBool CBearerManager::BearerExists(TUid aBearerUid) const
	{
	LOG_FUNC;
	LOG1(_L("\taBearerUid = 0x%08x"), aBearerUid);

	TBool ret = EFalse;
	if ( BearerIf(aBearerUid) != NULL )
		{
		ret = ETrue;
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TBool CBearerManager::CheckPolicy(TUid aBearerUid, const RMessage2& aMessage)
	{
	LOG_FUNC;
	LOG1(_L("\taBearerUid = 0x%08x"), aBearerUid);

	MRemConBearerInterface* bearerIf = BearerIf(aBearerUid);
	// The caller should have checked that the interface existed before 
	// calling this.
	ASSERT_DEBUG(bearerIf);
	TBool ret = bearerIf->SecurityPolicy().CheckPolicy(aMessage);

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

void CBearerManager::TargetClientAvailable(TRemConClientId aId, const TPlayerType& aClientType, const TPlayerSubType& aClientSubType, const TDesC8& aName)
	{
	LOG_FUNC;
		
	const TUint count = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		// maintain the controller and target count for each bearer 
		// tell the bearer if the count has increased to 1
		// by doing this in this loop we are sure we only 
		// tell the bearer when we need to */
		MRemConBearerInterface* const bearerIf = iBearerIfs[ii].iIf;
		ASSERT_DEBUG(bearerIf);
		iBearerIfs[ii].iTargetCount++;
		if (1 == iBearerIfs[ii].iTargetCount)
			{
			bearerIf->ClientStatus(TBool(iBearerIfs[ii].iControllerCount), TBool(iBearerIfs[ii].iTargetCount));
			}
		
		MRemConBearerInterfaceV3* const bearerIfV3 = iBearerIfs[ii].iIfV3;
		if(bearerIfV3)
			{
			bearerIfV3->ClientAvailable(aId, aClientType, aClientSubType, aName);
			}
		}
	}

void CBearerManager::TargetFeaturesUpdated(TRemConClientId aId, const TPlayerType& aPlayerType, const TPlayerSubType& aPlayerSubType, const TDesC8& aName)
    {
    LOG_FUNC;
        
    const TUint count = iBearerIfs.Count();
    for ( TUint ii = 0 ; ii < count ; ++ii )
        {
        MRemConBearerInterfaceV3* const bearerIfV3 = iBearerIfs[ii].iIfV3;

        if(bearerIfV3)
            {
            bearerIfV3->TargetFeaturesUpdated(aId, aPlayerType, aPlayerSubType, aName);
            }
        }
    }

void CBearerManager::ControllerClientAvailable()
	{
	LOG_FUNC;
	
	RArray<TUid> supportedInterfaces;
	TInt err = iServer.ControllerSupportedInterfaces(supportedInterfaces);
	LOG2(_L("\tGot %d supported interfaces with result %d"), supportedInterfaces.Count(), err);
	
	const TUint count = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		// maintain the controller and target count for each bearer 
		// tell the bearer if the count has increased to 1
		// by doing this in this loop we are sure we only 
		// tell the bearer when we need to
		MRemConBearerInterface* const bearerIf = iBearerIfs[ii].iIf;
		ASSERT_DEBUG(bearerIf);
		iBearerIfs[ii].iControllerCount++;
		if (1 == iBearerIfs[ii].iControllerCount)
			{
			bearerIf->ClientStatus(TBool(iBearerIfs[ii].iControllerCount), TBool(iBearerIfs[ii].iTargetCount));
			}

		if(!err)
			{
			// If we know what the supported interface are we can tell the bearer
			MRemConBearerInterfaceV3* const bearerIfV3 = iBearerIfs[ii].iIfV3;

			if(bearerIfV3)
				{
				bearerIfV3->ControllerFeaturesUpdated(supportedInterfaces);
				}
			}
		}
	
	supportedInterfaces.Close();
	}

void CBearerManager::ClientConnectionOriented(TUid aUid)
	{
	LOG_FUNC;

	const TUint count = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		MRemConBearerInterface* const bearerIf = iBearerIfs[ii].iIf;
		ASSERT_DEBUG(bearerIf);
		// maintain the controller and target count for each bearer 
		// target count won't change for this
		// Controller won't change if we are the bearer being targetted by the controller
		// it will go down if we're not.
		// Tell the bearer if the controller count has decreased to zero
		if (aUid != iBearerIfs[ii].iBearerUid)
			{
			iBearerIfs[ii].iControllerCount--;
			if (0 == iBearerIfs[ii].iControllerCount)
				{
				bearerIf->ClientStatus(TBool(iBearerIfs[ii].iControllerCount), TBool(iBearerIfs[ii].iTargetCount));
				}
			}
		}
	}

void CBearerManager::ClientConnectionless(TUid aUid)
	{
	LOG_FUNC;
	
	RArray<TUid> supportedInterfaces;
	TInt err = iServer.ControllerSupportedInterfaces(supportedInterfaces);
	LOG2(_L("\tGot %d supported interfaces with result %d"), supportedInterfaces.Count(), err);

	const TUint count = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		MRemConBearerInterface* const bearerIf = iBearerIfs[ii].iIf;
		MRemConBearerInterfaceV3* const bearerIfV3 = iBearerIfs[ii].iIfV3;
		ASSERT_DEBUG(bearerIf);

		// maintain the controller and target count for each bearer 
		// target count won't change for this
		// Controller won't change if we were the bearer being targetted by the controller
		// it will go up if we're not.
		// tell the bearer if the controller count has increased to 1 and provide
		// it with the current feature list.	   
		if (aUid != iBearerIfs[ii].iBearerUid)
			{
			iBearerIfs[ii].iControllerCount++;
			if (1 == iBearerIfs[ii].iControllerCount)
				{
				bearerIf->ClientStatus(TBool(iBearerIfs[ii].iControllerCount), TBool(iBearerIfs[ii].iTargetCount));

				if(!err && bearerIfV3)
					{
					bearerIfV3->ControllerFeaturesUpdated(supportedInterfaces);
					}
				}
			}
		}
	
	supportedInterfaces.Close();
	}

void CBearerManager::ClientClosed(TBool aController, TUid aUid, TRemConClientId aClientId)
	{
	LOG_FUNC;
	LOG1(_L("\taController = %x"), aController);
	
	RArray<TUid> supportedInterfaces;
	TInt err = KErrNone;
	if(aController)
		{
		err = iServer.ControllerSupportedInterfaces(supportedInterfaces);
		LOG2(_L("\tGot %d supported interfaces with result %d"), supportedInterfaces.Count(), err);
		}

	const TUint count = iBearerIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		MRemConBearerInterface* const bearerIf = iBearerIfs[ii].iIf;
		MRemConBearerInterfaceV3* const bearerIfV3 = iBearerIfs[ii].iIfV3;
		ASSERT_DEBUG(bearerIf);
		// maintain the controller and target count for each bearer 
		// the target count may change for this
		// Controller won't change if we were the bearer being targetted by the controller
		// it will go up if we're not.
		if (aController)
			{
			// so if the aUid is not null then the closed session affects only
			// the bearer it was pointing at. If the uid is NULL then its affecting
			// all bearers
			// tell the bearer if controller or target count has reached zero.
			// If there are controllers left then let the bearer know the current
			// feature set.
			if ((aUid == iBearerIfs[ii].iBearerUid) || (KNullUid == aUid))
				{
				iBearerIfs[ii].iControllerCount--;
				if (0 == iBearerIfs[ii].iControllerCount)
					{
					bearerIf->ClientStatus(TBool(iBearerIfs[ii].iControllerCount), TBool(iBearerIfs[ii].iTargetCount));
					}
				else if(!err && bearerIfV3)
					{
					bearerIfV3->ControllerFeaturesUpdated(supportedInterfaces);
					}
				}
			}
		else
			{
			iBearerIfs[ii].iTargetCount--;
			if (0 == iBearerIfs[ii].iTargetCount)
				{
				bearerIf->ClientStatus(TBool(iBearerIfs[ii].iControllerCount), TBool(iBearerIfs[ii].iTargetCount));
				}
			if(bearerIfV3)
				{
				bearerIfV3->ClientNotAvailable(aClientId);
				}
			}
		}
	
	supportedInterfaces.Close();
	}

TInt CBearerManager::MrcboDoNewResponse(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	
	TRAPD(err, NewResponseL(aAddr));

	LOG1(_L("\terr = %d"), err);
	return err;
	}
TInt CBearerManager::MrcboDoNewNotifyResponse(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	
	TRAPD(err, NewNotifyResponseL(aAddr));

	LOG1(_L("\terr = %d"), err);
	return err;
	}

void CBearerManager::NewResponseL(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	// Get the calling bearer from aAddr, call GetResponse on it, parse the 
	// new message and find the originating command in the 'sent commands' 
	// log. Give the new message to the session which sent that command. 
	MRemConBearerInterface* bearerIf = BearerIf(aAddr.BearerUid());
	ASSERT_DEBUG(bearerIf);
	TUid interfaceUid;
	TUint transactionId;
	TUint operationId;
	RBuf8 data;
	TRemConAddress addr;
	LEAVEIFERRORL(bearerIf->GetResponse(interfaceUid,
		transactionId,
		operationId, 
		data, 
		addr));
	LOG4(_L("\treceived response with interfaceUid [0x%08x], operationId 0x%02x, transactionId %d, data.Length = %d"), 
		interfaceUid, operationId, transactionId, data.Length());
	// We now own what's pointed to by 'data'.
	CleanupClosePushL(data);

	CRemConMessage* msg = CRemConMessage::NewL(
		aAddr,
		KNullClientId,
		ERemConResponse,
		ERemConMessageDefault,
		interfaceUid,
		operationId,
		data,
		0, // session ID as yet unknown
		transactionId);
	CLEANUPSTACK_POP1(&data); // owned by msg
	// Give the new response to the server to find the corresponding outgoing 
	// command we sent, then use that to route the new response to a client 
	// session.
	iServer.NewResponse(*msg); // ownership of msg is always taken by NewResponse
	}

void CBearerManager::NewNotifyResponseL(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	// Get the calling bearer from aAddr, call GetResponse on it, parse the 
	// new message and find the originating command in the 'sent commands' 
	// log. Give the new message to the session which sent that command. 
	MRemConBearerInterfaceV3* bearerIfV3 = BearerIfV3(aAddr.BearerUid());
	ASSERT_DEBUG(bearerIfV3);
	TUid interfaceUid;
	TUint transactionId;
	TUint operationId;
	RBuf8 data;
	TRemConAddress addr;
	TRemConMessageSubType subMessageType;
	LEAVEIFERRORL(bearerIfV3->GetNotifyResponse(interfaceUid,
		transactionId,
		operationId, 
		data, 
		addr,
		subMessageType));
	LOG4(_L("\treceived response with interfaceUid [0x%08x], operationId 0x%02x, transactionId %d, data.Length = %d"), 
		interfaceUid, operationId, transactionId, data.Length());
	// We now own what's pointed to by 'data'.
	CleanupClosePushL(data);

	CRemConMessage* msg = CRemConMessage::NewL(
		aAddr,
		KNullClientId,
		ERemConResponse,
		subMessageType,
		interfaceUid,
		operationId,
		data,
		0, // session ID as yet unknown
		transactionId);
	CLEANUPSTACK_POP1(&data); // owned by msg
	// Give the new response to the server to find the corresponding outgoing 
	// command we sent, then use that to route the new response to a client 
	// session.
	iServer.NewNotifyResponse(*msg); // ownership of msg is always taken by NewResponse
	}

TInt CBearerManager::MrcboDoNewCommand(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TRAPD(err, NewCommandL(aAddr, KNullClientId));

	LOG1(_L("\terr = %d"), err);
	return err;
	}

TInt CBearerManager::MrcboDoNewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TRAPD(err, NewCommandL(aAddr, aClient));

	LOG1(_L("\terr = %d"), err);
	return err;
	}

void CBearerManager::NewCommandL(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	// Get the calling bearer from aAddr and get the new command from it.
	MRemConBearerInterface* const bearerIf = BearerIf(aAddr.BearerUid());
	ASSERT_DEBUG(bearerIf);
	TUid interfaceUid;
	TUint transactionId;
	TUint operationId;
	RBuf8 data;
	TRemConAddress addr;
	LEAVEIFERRORL(bearerIf->GetCommand(interfaceUid,
		transactionId, 
		operationId, 
		data, 
		addr));
	LOG3(_L("\treceived command with interfaceUid [0x%08x], operationId 0x%02x, data.Length = %d"), 
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

TInt CBearerManager::MrcboDoNewNotifyCommand(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TRAPD(err, NewNotifyCommandL(aAddr, KNullClientId));

	LOG1(_L("\terr = %d"), err);
	return err;
	}

TInt CBearerManager::MrcboDoNewNotifyCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	TRAPD(err, NewNotifyCommandL(aAddr, aClient));

	LOG1(_L("\terr = %d"), err);
	return err;	}

void CBearerManager::NewNotifyCommandL(const TRemConAddress& aAddr, const TRemConClientId& aClient)
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());

	// Get the calling bearer from aAddr and get the new command from it.
	MRemConBearerInterfaceV2* const bearerIfV2 = BearerIfV2(aAddr.BearerUid());
	
	// We only get here because the bearer has told us it's got a notify.  In
	// order to use notifies it must use the V2 bearer interface.
	ASSERT_DEBUG(bearerIfV2); 
	
	TUid interfaceUid;
	TUint transactionId;
	TUint operationId;
	RBuf8 data;
	TRemConAddress addr;
	LEAVEIFERRORL(bearerIfV2->GetNotifyCommand(interfaceUid,
		transactionId, 
		operationId, 
		data, 
		addr));
	LOG3(_L("\treceived command with interfaceUid [0x%08x], operationId 0x%02x, data.Length = %d"), 
		interfaceUid, operationId, data.Length());
	// We now own what's pointed to by 'data'.
	CleanupClosePushL(data);

	CRemConMessage* msg = CRemConMessage::NewL(
		aAddr,
		aClient,
		ERemConNotifyCommand,
		ERemConNotifyCommandAwaitingInterim,
		interfaceUid,
		operationId,
		data,
		0, // session ID as yet unknown
		transactionId);
	// 'msg' now has a pointer to the memory pointed to by 'data', and owns 
	// it.
	CLEANUPSTACK_POP1(&data);
	// Give the new command to the server, which takes ownership of it. 
	iServer.NewNotifyCommand(*msg);
	}

TInt CBearerManager::MrcboDoConnectIndicate(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;

	// Just call the handler for new connections.
	TInt ret = iServer.HandleConnection(aAddr, KErrNone);
	LOG1(_L("\tret = %d"), ret);

	return ret;
	}

void CBearerManager::MrcboDoDisconnectIndicate(const TRemConAddress& aAddr)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	
	// Just call the handler for removed connections.
	iServer.RemoveConnection(aAddr, KErrNone);
	}

TInt CBearerManager::MrcboDoConnectConfirm(const TRemConAddress& aAddr, TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());

	TInt index = iCurrentlyBeingConnected.Find(aAddr, RemConAddrsMatch);
	ASSERT_DEBUG(index != KErrNotFound);
	iCurrentlyBeingConnected.Remove(index);

	TInt ret = iServer.HandleConnection(aAddr, aError);

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

void CBearerManager::MrcboDoDisconnectConfirm(const TRemConAddress& aAddr, TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());

	TInt index = iCurrentlyBeingDisconnected.Find(aAddr, RemConAddrsMatch);
	ASSERT_DEBUG(index != KErrNotFound);
	iCurrentlyBeingDisconnected.Remove(index);

	if ( aError == KErrNone )
		{
		// Remove connection and complete notifications.
		iServer.RemoveConnection(aAddr, aError);
		}
	}

TUint CBearerManager::MrcboDoNewTransactionId()
	{
	TUint newId = iRunningTransactionId;

	if ( iRunningTransactionId == KMaxTUint )
		{
		iRunningTransactionId = 0;
		}
	else
		{
		++iRunningTransactionId;
		}
	
	LOG1(_L("CBearerManager::MrcboDoNewTransactionId newId = %d"), newId);
	return newId;
	}

TInt CBearerManager::MrcboDoInterfaceToBearer(TUid aBearerUid,
		TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG4(_L("\taBearerUid = 0x%08x, aInterfaceUid = 0x%08x, aOperationId = 0x%02x, aMsgType = %d"), 
		aBearerUid, aInterfaceUid, aOperationId, aMsgType);

	MRemConConverterInterface* conv = iServer.Converter(aInterfaceUid, aBearerUid);
	TInt ret = KErrNotSupported;
	if ( conv )
		{
		ret = conv->InterfaceToBearer(aInterfaceUid, aOperationId, aData, aMsgType, aBearerData);
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TInt CBearerManager::MrcboDoBearerToInterface(TUid aBearerUid,
		const TDesC8& aInterfaceData, 
		const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taBearerUid = 0x%08x"), aBearerUid);

	MRemConConverterInterface* conv = iServer.Converter(aInterfaceData, aBearerUid);
	TInt ret = KErrNotSupported;
	if ( conv )
		{
		ret = conv->BearerToInterface(aBearerData, aInterfaceUid, aOperationId, aMsgType, aData);
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

void CBearerManager::MrcboDoSetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId)
	{
	LOG_FUNC
	LOG2(_L("\taBearerUid = 0x%08x, aId = %d"), aBearerUid, aId);
	
	iServer.SetRemoteAddressedClient(aBearerUid, aId);
	}

void CBearerManager::MrcboDoCommandExpired(TUint aTransactionId)
	{
	iServer.CommandExpired(aTransactionId);
	}

TSglQue<TBearerSecurity>& CBearerManager::BearerSecurityPolicies()
	{
	return iBearerSecurityPolicies;
	}

TBool CBearerManager::IsConnecting(const TRemConAddress& aAddr) const
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	
	TInt index = iCurrentlyBeingConnected.Find(aAddr, RemConAddrsMatch);
	TInt ret = ( index != KErrNotFound ) ? ETrue : EFalse;
	
	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

TBool CBearerManager::IsDisconnecting(const TRemConAddress& aAddr) const
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	
	TInt index = iCurrentlyBeingDisconnected.Find(aAddr, RemConAddrsMatch);
	TInt ret = ( index != KErrNotFound ) ? ETrue : EFalse;
	
	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

void CBearerManager::BulkInterfacesL(RArray<TBulkInterfaceInfo>& aBulkInterfaces) const
	{
	LOG_FUNC
	ASSERT_DEBUG(aBulkInterfaces.Count() == 0);
	CleanupResetPushL(aBulkInterfaces);
	MRemConBearerBulkInterface* bulkIf = NULL;
	for(TInt i=0; i<iBearers.Count(); i++)
		{
		ASSERT_DEBUG(iBearers[i]);
		bulkIf = reinterpret_cast<MRemConBearerBulkInterface*>(iBearers[i]->GetInterface(TUid::Uid(KRemConBearerBulkInterface1)));
		if(bulkIf)
			{
			TBulkInterfaceInfo ifInfo;
			ifInfo.iIf = bulkIf;
			ifInfo.iIfUid = TUid::Uid(KRemConBearerBulkInterface1);
			ifInfo.iBearerUid = iBearers[i]->Uid();
			aBulkInterfaces.AppendL(ifInfo);
			}
		}
	CleanupStack::Pop(&aBulkInterfaces);
	}

TInt CBearerManager::MrcboDoSupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids)
	{
	LOG_FUNC
	return iServer.SupportedInterfaces(aId, aUids);
	}

TInt CBearerManager::MrcboDoSupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	LOG_FUNC
	return iServer.SupportedOperations(aId, aInterfaceUid, aOperations);
	}

TInt CBearerManager::MrcboDoRegisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return iServer.RegisterLocalAddressedClientObserver(aBearerUid);
	}

TInt CBearerManager::MrcboDoUnregisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return iServer.UnregisterLocalAddressedClientObserver(aBearerUid);
	}

TInt CBearerManager::SetLocalAddressedClient(const TUid& aBearerUid, TRemConClientId aClientId)
	{
	LOG_FUNC;
	
	MRemConBearerInterfaceV3* const bearerIfV3 = BearerIfV3(aBearerUid);
	if(bearerIfV3)
		{
		return bearerIfV3->SetLocalAddressedClient(aClientId);
		}
	else
		{
		return KErrNotFound;
		}
	}


