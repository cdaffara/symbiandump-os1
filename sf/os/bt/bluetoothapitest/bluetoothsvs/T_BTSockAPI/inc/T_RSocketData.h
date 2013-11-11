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


#if (!defined __T_RSOCKET_DATA_H__)
#define __T_RSOCKET_DATA_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <es_sock.h> // TSockAddr address 
#include <bt_sock.h> // Bluetooth address classes


class CT_RSocketData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_RSocketData*	NewL();
	
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
	* Destructor
	*/
	~CT_RSocketData();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_RSocketData();
	
	/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();	
	
private:
	void CloseSocketServer();

	void RSocketOpen(const TDesC& aSection);
	void Info(const TDesC& aSection);

private:
	
	// An object of TSockAddr class in order to retain state between calls    
    RSocket               iRSocket;    
    RSocketServ			iSocketServer;
	};

#endif /* __T_RSOCKET_DATA_H__ */
