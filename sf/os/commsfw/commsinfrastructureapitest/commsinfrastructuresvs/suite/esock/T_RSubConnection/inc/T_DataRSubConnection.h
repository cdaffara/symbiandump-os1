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


#ifndef __T_DATA_RSUBCONNECTION__
#define __T_DATA_RSUBCONNECTION__

//User includes
#include "T_DataCommsBase.h"

//Epoc includes
#include <es_sock.h>
#include <cs_subconparams.h>

_LIT(KDataRSubConnection,	"RSubConnection");

/**
 * Test Active Notification class
 *
 */
class CT_DataRSubConnection : public CT_DataCommsBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_DataRSubConnection*	NewL();

	/**
	* Public destructor
	*/
	~CT_DataRSubConnection();

	/**
	* Process a command read from the ini file
	*
	* \param aTestStep	test step requiring command to be processed
	* \param aCommand	the command to process
	* \param aSection	the section in the ini file requiring the command to be processed
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
	CT_DataRSubConnection();

	/**
	* Second phase construction
	*/
	void	ConstructL();

private:
	// DoCommandL delegated test methods
	void				DoCmdOpenL( const TDesC& aSection );
	void				DoCmdClose();
	void				DoCmdAddL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	void				DoCmdRemoveL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	void				DoCmdEventNotificationL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	void				DoCmdCancelEventNotification();
	void				DoCmdGetParametersL(const TDesC& aSection);
	void				DoCmdSetParametersL(const TDesC& aSection);
	void				DoCmdControlL(const TDesC& aSection);
	void				DoCmdConstructorL();
	void				DoCmdDestructor();

	//	MActiveCallback implementation
	virtual void		RunL(CActive* aActive, TInt aIndex);
	virtual void		DoCancel(CActive* aActive, TInt aIndex);

private:
	CActiveCallback*						iActiveAdd;
	CActiveCallback*						iActiveRemove;
	CActiveCallback*						iActiveEventNotification;
	RSubConnection*							iSubConnection;
	TInt									iOutstanding;

	// Stored parameters for async testing within RunL
	RArray<RSubConnection::TEventFilter>	iEventFilterArray;
	TNotificationEventBuf					iActualEventBuf;
	};

#endif /* __T_DATA_RSUBCONNECTION__ */
