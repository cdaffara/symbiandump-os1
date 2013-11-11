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


#ifndef __T_DATA_RCONNECTION__
#define __T_DATA_RCONNECTION__

//	User includes
#include "T_DataCommsBase.h"

#include <testexecutestepbase.h>

// Epoc includes
#include <es_sock.h>
#include <es_enum.h>

const	TInt	KMaxServTypeName	= 50;

/**
 * Test Active Notification class
 *
 */
class CT_DataRConnection : public CT_DataCommsBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_DataRConnection*	NewL();

	/**
	* Public destructor
	*/
	~CT_DataRConnection();

	/**
	* Process a command read from the ini file
	*
	* \param aTestStep	test step requiring command to be processed
	* \param aCommand	the command to process
	* \param aSection	the entry in the ini file requiring the command to be processed
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
	CT_DataRConnection();

	/**
	* Second phase construction
	*/
	void	ConstructL();

private:
	// DoCommandL delegated test methods
	TInt		DoCmdOpenL( const TDesC& aSection );
	TInt		DoCmdClose();
	TInt		DoCmdStart( const TDesC& aSection, const TInt aAsyncErrorIndex);
	TInt		DoCmdStop( const TDesC& aSection );
	TInt		DoCmdProgressNotification( const TDesC& aSection, const TInt aAsyncErrorIndex );
	TInt		DoCmdCancelProgressNotification();
	TInt		DoCmdProgress();
	TInt		DoCmdLastProgressError( const TDesC& aSection );
	TInt		DoCmdServiceChangeNotification( const TDesC& aSection, const TInt aAsyncErrorIndex );
	TInt		DoCmdCancelServiceChangeNotification();
	TInt		DoCmdGetIntSetting( const TDesC& aSection );
	TInt		DoCmdGetBoolSetting( const TDesC& aSection );
	TInt		DoCmdGetDesSettingL( const TDesC& aSection );
	TInt		DoCmdGetLongDesSettingL( const TDesC& aSection );
	TInt		DoCmdName();
	TInt		DoCmdEnumerateConnections( const TDesC& aSection );
	TInt		DoCmdGetConnectionInfo( const TDesC& aSection );
	TInt		DoCmdAllInterfaceNotification(const TInt aAsyncErrorIndex);
	TInt		DoCmdCancelAllInterfaceNotification();
	TInt		DoCmdAttachL( const TDesC& aSection );
	TInt		DoCmdConstructorL();
	TInt		DoCmdDestructor();

	//	MActiveCallback implementation
	virtual void		RunL(CActive* aActive, TInt aIndex);
	virtual void		DoCancel(CActive* aActive, TInt aIndex);

	// Handle asynchronous commands when they complete
	void				CompleteProgressNotification( CActive* aActive, TInt aIndex );
	void				CompleteServiceChangeNotification( TInt aIndex );
	void				CompleteAllInterfaceNotification( TInt aIndex );

private:
	CActiveCallback*						iActive;
	CActiveCallback*						iActiveProgress;
	CActiveCallback*						iActiveServiceChange;
	CActiveCallback*						iActiveAllInterface;
	RConnection*							iConnection;

	// Stored parameters for async testing within RunL
	TNifProgressBuf					iActualProgressBuf;
	TInterfaceNotificationBuf		iActualIfNotification;
	TUint32							iActualNewISPId;
	TInt							iExpectedNewISPId;
	TBuf<KMaxServTypeName>			iActualNewServiceType;
	TPtrC							iExpectedNewServiceType;
	TUint							iExpectedStage;
	};

_LIT(KDataRConnection,		"RConnection");
	
#endif /* __T_DATA_RCONNECTION__ */
