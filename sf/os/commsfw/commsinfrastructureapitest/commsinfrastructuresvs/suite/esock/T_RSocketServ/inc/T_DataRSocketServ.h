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


#if (!defined __T_RSOCKETSERV_DATA_H__)
#define __T_RSOCKETSERV_DATA_H__

//	User Includes
#include "T_DataCommsBase.h"

class CT_DataRSocketServ: public CT_DataCommsBase
	{
public:
	CT_DataRSocketServ();
	
	/**
	* Public destructor
	*/
	~CT_DataRSocketServ();
	
	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_DataRSocketServ*	NewL();
	
	/**
	* Process a command read from the ini file
	*
	* @param	aCommand the command to process
	* @param	aSection the section in the ini file requiring the command to be processed
	*
	* @leave	system wide error
	*
	* @return	ETrue if the command is processed
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	* Return a pointer to the object that the data wraps
	*
	* @return	pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();
	
	void RunL(CActive* aActive, TInt aIndex);
	void DoCancel(CActive* aActive, TInt aIndex);
	
private:
	void ConstructL();
	void DoCmdClearExclusiveMode();
	void DoCmdConnect(const TDesC& aSection);
	void DoCmdFindProtocol(const TDesC& aSection);
	void DoCmdGetProtocolInfo(const TDesC& aSection);
	void DoCmdInstallExtension(const TDesC& aSection);
	void DoCmdNumProtocols(const TDesC& aSection);
	void DoCmdSetExclusiveMode(const TInt aAsyncErrorIndex);
	void DoCmdStartProtocolL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void DoCmdStopProtocolL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void DoCmdVersion(const TDesC& aSection);
	void DoCmdConstructorL();
	void DoCmdDestructor();

private:
    RSocketServ*				iSocketServ;    

	CActiveCallback*			iActiveStartProtocol;
	CActiveCallback*			iActiveStopProtocol;
	CActiveCallback*			iActiveSetExclusiveMode;
	
	TUint						iNumOfProtocols;
	TProtocolDesc				iProtocolDesc;
	};

_LIT(KDataRSocketServ,		"RSocketServ");

#endif /* __T_RSOCKETSERV_DATA_H__ */
