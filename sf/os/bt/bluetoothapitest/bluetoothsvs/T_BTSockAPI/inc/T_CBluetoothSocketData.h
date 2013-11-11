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


#if (!defined __T_CBLUETOOTH_SOCK_DATA_H__)
#define __T_CBLUETOOTH_SOCK_DATA_H__

//	User Includes
#include "DataWrapperBase.h"
#include "T_BluetoothSocketNotifier.h"

const TInt	DATASIZE = 128;


class CT_CBluetoothSocketData : public CDataWrapperBase, private MBluetoothSocketNotifier
	{
public:
	/**
	* Public destructor
	*/
	~CT_CBluetoothSocketData();

	/**
	* Two phase constructor
	*/
	static CT_CBluetoothSocketData*	NewL();

	/**
	* Returns the RSocketServer instance needed so that session sockets can be created correctly
	*/
	RSocketServ&	GetRServer();

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
	CT_CBluetoothSocketData();
	void	ConstructL();

private:
	//	MBluetoothSocketNotifier implementation
	virtual void	HandleConnectCompleteL(TInt aErr);
	virtual void	HandleAcceptCompleteL(TInt aErr);
	virtual void	HandleShutdownCompleteL(TInt aErr);
	virtual void	HandleSendCompleteL(TInt aErr);
	virtual void	HandleReceiveCompleteL(TInt aErr);
	virtual void	HandleIoctlCompleteL(TInt aErr);
	virtual void	HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);

    // Implementation of DoCommandL for each of the commands
	inline void	DoCmdAccept(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdActivateBasebandEventNotifier(const TDesC& /*aSection*/,const TInt aAsyncErrorIndex);
	inline void	DoCmdActivateSniffRequester();
	inline void	DoCmdActivateParkRequester();
	inline void	DoCmdAllowLowPowerModes(const TDesC& aSection);
	inline void	DoCmdAllowRoleSwitch();
	inline void	DoCmdBind(const TDesC& aSection);
	inline void	DoCmdCancelAccept();
	inline void	DoCmdCancelAll();
	inline void	DoCmdCancelBasebandEventNotifier();
	inline void	DoCmdCancelConnect();
	inline void	DoCmdCancelIoctl();
	inline void	DoCmdCancelLowPowerModeRequester(const TDesC& aSection);
	inline void	DoCmdCancelRead();
	inline void	DoCmdCancelRecv();
	inline void	DoCmdCancelSend();
	inline void	DoCmdCancelWrite();
	inline void	DoCmdCloseSocketServer();
	inline void	DoCmdConnect(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdGetDisconnectData();
	inline void	DoCmdGetOpt(const TDesC& aSection);
	inline void	DoCmdInfo(const TDesC& aSection);
	inline void	DoCmdIoctl(const TDesC& aSection,const TInt aAsyncErrorIndex);
	inline void	DoCmdListen(const TDesC& aSection);
	inline void	DoCmdLocalName(const TDesC& aSection);
	inline void	DoCmdLocalPort(const TDesC& aSection);
	inline void	DoCmdName();
	inline void	DoCmdNewL(const TDesC& aSection,TBool aUseLC);
	inline void	DoCmdPhysicalLinkState(const TDesC& aSection);
	inline void	DoCmdPreventLowPowerModes(const TDesC& aSection);
	inline void	DoCmdPreventRoleSwitch();
	inline void	DoCmdRead(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdRecv(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdRecvFrom(const TDesC& aSection,const TInt aAsyncErrorIndex);
	inline void	DoCmdRecvOneOrMore(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdRemoteName(const TDesC& aSection);
	inline void	DoCmdRequestChangeSupportedPacketTypes(const TDesC& aSection);
	inline void	DoCmdRequestMasterRole();
	inline void	DoCmdRequestSlaveRole();
	inline void	DoCmdSend(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdSendTo(const TDesC& aSection,const TInt aAsyncErrorIndex);
	inline void	DoCmdSetAutomaticSniffMode(const TDesC& aSection);
	inline void	DoCmdSetLocalPort(const TDesC& aSection);
	inline void	DoCmdSetOpt(const TDesC& aSection);
	inline void	DoCmdSetOption(const TDesC& aSection);
	inline void	DoCmdSetRsocketServerFromCBTSocket(const TDesC& aSection);
	inline void	DoCmdSetTransferAble();
	inline void	DoCmdShutdown(const TDesC& aSection,const TInt aAsyncErrorIndex);
	inline void	DoCmdSetNotifierL(const TDesC& aSection);
	inline void	DoCmdTestMBSN_ExtensionInterfaceL(const TDesC& aSection);
	inline void	DoCmdTransfer(const TDesC& aSection);
	inline void	DoCmdWrite(const TDesC& aSection,const TInt aAsyncErrorIndex);

	static void CleanupOperation(TAny* aAny);

public:
	CT_BluetoothSocketNotifier*	iData;

private:
	TBuf8<DATASIZE>				iDataToRead8;
	TBuf8<DATASIZE>				iDataAtDisconnect8;
	TBuf8<DATASIZE>				iDisconnectDes8;

	TBool						iHasConnectDataToExpect;
	TBuf<DATASIZE>				iConnectDataToExpect;

	TBool						iHasAcceptDataToExpect;
	TBuf<DATASIZE>				iAcceptDataToExpect;

	TBool						iHasReceiveDataToExpect;
	TBuf<DATASIZE>				iReceiveDataToExpect;

	TBuf8<DATASIZE> 			iConnectData8;
	TPckgBuf<TUint16> 			iMTUOptionBuffer;
	
	TInt                        iConnectIndex;
	TInt                        iAcceptIndex;
	TInt                        iShutdownIndex;
	TInt                        iIoctlIndex;
	TInt                        iNotifierIndex;
	TInt                        iReceiveIndex;
	TInt                        iSendIndex;
	
	TBool                       iConnectFlag;
	TBool                       iAcceptFlag;
	TBool                       iIoctlFlag;
	TBool                       iNotifierFlag;
	TBool                       iReceiveFlag;
	TBool                       iSendFlag;
	TBool						iReadFlag;
	TBool						iRecvFlag;
	TBool						iWriteFlag;
	};

#endif /* __T_CBLUETOOTH_SOCK_DATA_H__ */
