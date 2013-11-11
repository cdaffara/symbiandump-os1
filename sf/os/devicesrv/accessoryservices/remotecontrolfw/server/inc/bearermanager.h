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
//

/**
 @file
 @internalComponent
*/

#ifndef BEARERMANAGER_H
#define BEARERMANAGER_H

#include <e32base.h>
#include <remcon/remconbearerobserver.h>
#include <remcon/playertype.h>

class CRemConBearerPlugin;
class CRemConServer;
class CRemConMessage;
class TBearerSecurity;
class MRemConBearerBulkInterface;
class MRemConBearerInterface;
class MRemConBearerInterfaceV2;
class MRemConBearerInterfaceV3;

/**
Bearer manager.
Owns bearer plugin instances, allows access to them, and observes them.
APIs are provided to bring up and down connections on specific 
TRemConAddresses. 
The bearer manager keeps track of connections which we are in the process or 
bringing up or down (i.e. by request). For connection and disconnection 
_indications_, it defers to the connection history system to check that, for 
example, a disconnect indication refers to a connection we actually already 
knew about.
*/
NONSHARABLE_CLASS(CBearerManager) : public CBase, public MRemConBearerObserver
	{
public:
	/** 
	Bearer manager construction.
	@return Ownership of a new CBearerManager object
	*/
	static CBearerManager* NewL(CRemConServer& aServer);

	/** Destructor. */
	~CBearerManager();
	
	
public:
	struct TBulkInterfaceInfo
		{
		MRemConBearerBulkInterface* iIf;
		
		TUid iIfUid;
		TUid iBearerUid;
		};

public:
	/** Send a message over a specific connection and return the result 
	immediately. */
	TInt Send(CRemConMessage& aMsg);
	
	/** called by the server when a target client has registered its available
	interfaces and is ready to be used by a remote. */
	void TargetClientAvailable(TRemConClientId aId, const TPlayerType& aClientType, const TPlayerSubType& aClientSubType, const TDesC8& aName);
	
	/** called by the server when a target client has updated its available interfaces*/
	void TargetFeaturesUpdated(TRemConClientId aId, const TPlayerType& aPlayerType, const TPlayerSubType& aPlayerSubType, const TDesC8& aName);
	
	/** Called by the server when a controller client has registered its 
	available interfaces.  
	*/
	void ControllerClientAvailable();
	
	/** called by the server when a client is going connection oriented.
	Informs each bearer in turn, if required, when controller count goes 1->0 */
	void ClientConnectionOriented(TUid aUid);
	
	/** called by the server when a client is going connection oriented.
	Informs each bearer in turn, if required, when controller count goes 0->1 */
	void ClientConnectionless(TUid aOldUid);

	/** called by the server when a client has closed (only if controller or target)
	calls back to server to see whether it needs to inform each bearer in turn */
	void ClientClosed(TBool aController, TUid aOldUid, TRemConClientId aClientId);
	
	/** Called by the server to get the security policies of the bearers. */
	TSglQue<TBearerSecurity>& BearerSecurityPolicies();

	/** Tries to bring up a connection on an address. 
	@return KErrNone- a bearer-level connect has been issued (or was already 
	outstanding) and will be confirmed in due course. Other than KErrNone- we 
	could not remember the address (no connection will be made). */
	TInt Connect(const TRemConAddress& aAddr);

	/** Tries to tear down a connection on an address. 
	@return KErrNone- a bearer-level disconnect has been issued (or was 
	already outstanding) and will be confirmed in due course. Other than 
	KErrNone- we could not remember the address (no disconnection will be 
	performed). */
	TInt Disconnect(const TRemConAddress& aAddr);

	/** @return ETrue- the bearer with the given UID exists in the system, 
	EFalse otherwise. */
	TBool BearerExists(TUid aBearerUid) const;

	/** Does a security check of the given client message against the given 
	bearer's security policy. 
	@return The result of TSecurityPolicy::CheckPolicy with the given message.
	*/
	TBool CheckPolicy(TUid aBearerUid, const RMessage2& aMessage);

	/** 
	Checks if a bearer is in the connecting state to the given remote address 
	@param aAddr - remote address 
	@return  - ETrue if it is the connecting state
	*/
	TBool IsConnecting(const TRemConAddress& aAddr) const;
	
	/** 
	Checks if a bearer is in the disconnecting state from the given remote address 
	@param aAddr - remote address
	@return ETrue - if it is in the disconnecting state
	*/
	TBool IsDisconnecting(const TRemConAddress& aAddr) const;
	
	TInt SetLocalAddressedClient(const TUid& aBearerUid, TRemConClientId aClientInfo);
	
	/**
	This function is called from the context of the bulk server thread and is used to retrieve the
	bulk interfaces (that are managed in a separate class).
	No locking is provided so it is expected that the GetInterface implementation is re-entrant with
	respect to distribution of requests.
	@param aBulkInterfaces The array to populate with bulk interface information.
	*/
	void BulkInterfacesL(RArray<TBulkInterfaceInfo>& aBulkInterfaces) const;
	
private:
	/** Constructor. */
	CBearerManager(CRemConServer& aServer);

	/** 2nd-phase construction. */
	void ConstructL();

private: // from MRemConBearerObserver
	TInt MrcboDoNewResponse(const TRemConAddress& aAddr);
	TInt MrcboDoNewNotifyResponse(const TRemConAddress& aAddr);
	TInt MrcboDoNewCommand(const TRemConAddress& aAddr);
	TInt MrcboDoNewNotifyCommand(const TRemConAddress& aAddr);
	TInt MrcboDoConnectIndicate(const TRemConAddress& aAddr);
	void MrcboDoDisconnectIndicate(const TRemConAddress& aAddr);
	TInt MrcboDoNewCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	TInt MrcboDoNewNotifyCommand(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	void MrcboDoSetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId);


	/** Called in response to a connection request by us. */
	TInt MrcboDoConnectConfirm(const TRemConAddress& aAddr, TInt aError);

	/** Called in response to a disconnection request by us. */
	void MrcboDoDisconnectConfirm(const TRemConAddress& aAddr, TInt aError);

	TUint MrcboDoNewTransactionId();

	TInt MrcboDoInterfaceToBearer(TUid aBearerUid,
		TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const;
	TInt MrcboDoBearerToInterface(TUid aBearerUid,
		const TDesC8& aInterfaceData, 
		const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const;
	
	void MrcboDoCommandExpired(TUint aTransactionId);
	TInt MrcboDoSupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids);
	TInt MrcboDoSupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations);
	
	TInt MrcboDoRegisterLocalAddressedClientObserver(const TUid& aBearerUid);
	TInt MrcboDoUnregisterLocalAddressedClientObserver(const TUid& aBearerUid);
	
private: // utility
	/** Called during (2-phase) construction to instantiate all the bearer 
	plugins, from ROM only. */
	void LoadAllBearersL();

#ifdef __FLOG_ACTIVE
	void LogBearers() const;
#endif
	
	void NewResponseL(const TRemConAddress& aAddr);
	void NewNotifyResponseL(const TRemConAddress& aAddr);
	void NewCommandL(const TRemConAddress& aAddr, const TRemConClientId& aClient);
	void NewNotifyCommandL(const TRemConAddress& aAddr, const TRemConClientId& aClient);

	MRemConBearerInterface* BearerIf(TUid aUid) const;
	MRemConBearerInterfaceV2* BearerIfV2(TUid aUid) const;
	MRemConBearerInterfaceV3* BearerIfV3(TUid aUid) const;

private: // unowned
	// Struct wrapping a pointer to a bearer interface object with the UID 
	// identifying that interface and the UID identifying the bearer supplying 
	// the interface.
	struct TInterfaceInfo
		{
		MRemConBearerInterface* iIf;
		MRemConBearerInterfaceV2* iIfV2;
		MRemConBearerInterfaceV3* iIfV3;
		
		TUid iIfUid;
		TUid iBearerUid;
		TInt iControllerCount;
		TInt iTargetCount;
		};

	RArray<TInterfaceInfo>	iBearerIfs;

private: // owned
	RPointerArray<CRemConBearerPlugin> iBearers;
	TUint iRunningTransactionId;

	/** All the security policies of the bearers in the system. These are 
	given to the TSP for reference when it is addressing outgoing commands. */ 
	TSglQue<TBearerSecurity> iBearerSecurityPolicies;

	/** Array of addresses we currently have a ConnectRequest outstanding on. 
	*/
	RArray<TRemConAddress> iCurrentlyBeingConnected;

	/** Array of addresses we currently have a DisconnectRequest outstanding 
	on. */
	RArray<TRemConAddress> iCurrentlyBeingDisconnected;
	
private: // unowned
	CRemConServer& iServer;
	};

#endif // BEARERMANAGER_H
