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


#ifndef __T_DATA_RSOCKET__
#define __T_DATA_RSOCKET__

//	User includes
#include "T_ActiveSocket.h"
#include "T_DataCommsBase.h"

//	Epoc includes
#include <in_sock.h>

/**
 * Test Active Notification class
 *
 */
class CT_DataRSocket : public CT_DataCommsBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_DataRSocket*	NewL();

	/**
	* Public destructor
	*/
	~CT_DataRSocket();

	/**
	* Process a command read from the ini file
	*
	* \param aTestStep	test step requiring command to be processed
	* \param aCommand	the command to process
	* \param aEntry		the entry in the ini file requiring the command to be processed
	*
	* \return ETrue if the command is processed
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_DataRSocket();

	/**
	* Second phase construction
	*/
	void	ConstructL();

private:
	void	DoCmdClose();
	void	DoCmdOpenL(const TDesC& aSection);
	void	DoCmdSetLocalPort(const TDesC& aSection);
	void	DoCmdListenL(const TDesC& aSection);
	void 	DoCmdIoctlL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdAcceptL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdConnectL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdBind(const TDesC& aSection);
	void	DoCmdSendL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdSendToL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdReadL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdRecvL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdRecvOneOrMoreL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdRecvFromL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdGetOpt(const TDesC& aSection);
	void	DoCmdSetOpt(const TDesC& aSection);
	void	DoCmdName();
	void	DoCmdTransferL(const TDesC& aSection);
	void	DoCmdGetDisconnectData();
	void 	DoCmdWriteL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void 	DoCmdInfo(const TDesC& aSection);
	void 	DoCmdLocalName(const TDesC& aSection);
	void 	DoCmdLocalPort(const TDesC& aSection);
	void 	DoCmdRemoteName(const TDesC& aSection);
	void 	DoCmdCancelConnect();
	void 	DoCmdCancelAccept();
	void 	DoCmdCancelWrite();
	void 	DoCmdCancelRead();
	void	DoCmdCancelRecv();
	void	DoCmdCancelSend();
	void	DoCmdCancelIoctl();
	void	DoCmdCancelAll();
	void	DoCmdShutdownL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdConstructorL();
	void	DoCmdDestructor();

	TBool	GetIPAddress(const TDesC& aSection, const TDesC& aIpAddrKeyName, TPtrC& ipAddress);
	TBool	GetSocketName(TName& aSocketName);
	HBufC8*	GetTextDataL(const TDesC& aSection, const TDesC& aFileName, const TDesC& aData);
	TBool	ConvertToFlag(const TDesC& aFlagStr, TUint& aFlag);
	TUint	GetFlags(const TDesC& aSection, const TDesC& aFlagKeyName);
	TUint 	GetLocalPortFromSocketL(const TDesC& aSection);

	//	MActiveCallback implementation
	void		RunL(CActive* aActive, TInt aIndex);
	void		DoCancel(CActive* aActive, TInt aIndex);

	CActiveSocket*		RemoveActiveTypeFromArray(CActive* aActive);

private:
	RPointerArray<CActiveSocket>	iActiveCommand;

	RSocket*						iSocket;
	RFs								iFs;

	TName							iSocketName;
	TBool							iSocketNameSet;
	};
	
_LIT(KDataRSocket,			"RSocket");

#endif /* __T_DATA_RSOCKET__ */
