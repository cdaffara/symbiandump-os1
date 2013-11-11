/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#if (!defined __T_BLUETOOTH_SYNCHRONOUS_LINK_DATA_H__)
#define __T_BLUETOOTH_SYNCHRONOUS_LINK_DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

// EPOC includes
#include <bt_sock.h>

const TInt	KDataSize = 128;

class CT_BluetoothSynchronousLinkData : public CDataWrapperBase, private MBluetoothSynchronousLinkNotifier
	{
public:
/**
	* Public destructor
	*/
	~CT_BluetoothSynchronousLinkData();
	
	/**
	* Two phase constructor
	*/
	static CT_BluetoothSynchronousLinkData*	NewL();

	/**
	* Process a command read from the ini file
	*
	* @param aCommand			The command to process
	* @param aSection			The section in the ini containing data for the command
	* @param aAsyncErrorIndex	Command index for async calls to return errors to
	*
	* @return					ETrue if the command is processed
	*
	* @leave					System wide error
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
		
	/**
	* Return a pointer to the object that the data wraps
	*
	* @return	pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();

	/**
	* Set the object that the data wraps
	*
	* @param	aObject object that the wrapper is testing
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	SetObjectL(TAny* aObject);

	/**
	* The object will no longer be owned by this
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	DisownObjectL();
	
	inline virtual TCleanupOperation CleanupOperation();

protected:
	CT_BluetoothSynchronousLinkData();
	void ConstructL();

private:
	//	MBluetoothSynchronousLinkNotifier implementation
	virtual void	HandleSetupConnectionCompleteL(TInt aErr);
	virtual void	HandleDisconnectionCompleteL(TInt aErr);
	virtual void	HandleAcceptConnectionCompleteL(TInt aErr);
	virtual void	HandleSendCompleteL(TInt aErr);
	virtual void	HandleReceiveCompleteL(TInt aErr);
	
	void DestroyData();
	
	// Implementation of DoCommandL for each of the commands
	inline void	DoCmdNewL(const TDesC& aSection);
	inline void	DoCmdNewLC(const TDesC& aSection);
	inline void	DoCmdSetupConnection(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdCancelSetup();
	inline void	DoCmdDisconnect(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdSend(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdCancelSend();
	inline void	DoCmdReceive(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdCancelReceive();
	inline void	DoCmdAcceptConnection(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdCancelAccept();
	inline void	DoCmdSetCoding(const TDesC& aSection);
	inline void	DoCmdCoding(const TDesC& aSection);
	inline void	DoCmdSetMaxBandwidth(const TDesC& aSection);
	inline void	DoCmdBandwidth();
	inline void	DoCmdSetMaxLatency(const TDesC& aSection);
	inline void	DoCmdLatency(const TDesC& aSection);
	inline void	DoCmdSetRetransmissionEffort(const TDesC& aSection);
	inline void	DoCmdRetransmissionEffort(const TDesC& aSection);
	inline void	DoCmdRemoteName(const TDesC& aSection);
	inline void	DoCmdSetNotifier(const TDesC& aSection);
	inline void	DoCmdDestructor();
	inline void	DoCmdMBSLN_ExtensionInterfaceL(const TDesC& aSection);
	inline void	DoCmdRSocketServerConnect();
	inline void	DoCmdRSocketServerClose();
	
	static void CleanupOperation(TAny* aAny);
	
private:
	CBluetoothSynchronousLink*		iData;
	TInt							iErrForSetupConnection;
	TInt							iErrForSend;
	TInt							iErrForReceive;
	TInt							iErrForAcceptConnection;
	TInt							iErrForDisconnect;
	TBTDevAddr						iBTDeveAddr;
	TBTSyncPackets*					ibtSyncPackets;
	TInt							iSetupIndex;
	TInt							iDisconnectionIndex;
	TInt							iAcceptIndex;
	TInt							iSendIndex;
	TInt							iReceiveIndex;
	};

#endif /* __T_BLUETOOTH_SYNCHRONOUS_LINK_DATA_H__*/
