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


#if (!defined __T_RBUS_DEV_COMM_DATA_H__)
#define __T_RBUS_DEV_COMM_DATA_H__

//	User Includes
#include "T_RBusLogicalChannelData.h"
#include "T_ActiveCallbackIO.h"

//	EPOC includes
#include <d32comm.h>

/**
 * Test Active Notification class
 *
 */
class CT_RBusDevCommData : public CT_RBusLogicalChannelData
	{
public:
	static CT_RBusDevCommData*	NewL();
	~CT_RBusDevCommData();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RBusDevCommData();
	void	ConstructL();

	//	CT_RHandleBaseData implementation
	virtual RHandleBase*		GetHandleBase();

	//	CT_RBusLogicalChannelData implementation
	virtual RBusLogicalChannel*	GetBusLogicalChannel();

	//	MActiveCallback implementation
	virtual void	RunL(CActive* aActive, TInt aIndex);
 	virtual void	DoCancel(CActive* aActive, TInt aIndex);

private:
	void	DestroyData();

	TInt	DoCmdConstructor();
	void	DoCmdDestructor();
	void	DoCmdBreak(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdBreakCancel();
	void	DoCmdCapsL(const TDesC& aSection);
	void	DoCmdConfigL(const TDesC& aSection);
	void	DoCmdMinTurnaroundTime(const TDesC& aSection);
	void	DoCmdNotifyReceiveDataAvailable(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdNotifyReceiveDataAvailableCancel();
	void	DoCmdNotifySignalChange(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdNotifySignalChangeCancel();
	TInt	DoCmdOpen(const TDesC& aSection);
	TInt	DoCmdQueryReceiveBuffer(const TDesC& aSection);
	void	DoCmdReadL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdReadCancel();
	void	DoCmdReadOneOrMoreL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdReceiveBufferLength(const TDesC& aSection);
	void	DoCmdResetBuffers();
	TInt	DoCmdSetConfigL(const TDesC& aSection);
	TInt	DoCmdSetMinTurnaroundTime(const TDesC& aSection);
	TInt	DoCmdSetReceiveBufferLength(const TDesC& aSection);
	void	DoCmdSetSignals(const TDesC& aSection);
	void	DoCmdSignals(const TDesC& aSection);
	void	DoCmdVersionRequired(const TDesC& aSection);
	void	DoCmdWriteL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdWriteCancel();

	//	Helpers
	void	PrepareReadExpectedBufferLengthL(CT_ActiveCallbackIO& aCallback, const TDesC& aSection);

private:
	RBusDevComm*					iBusDevComm;
	HBufC8*							iBufferRead;
	TPtr8							iBufferReadPtr;
	TPtr8							iBufferReadOneOrMorePtr;
	TInt							iReadOneOrMoreLeft;
	TBool							iHasReadExpected;
	TInt							iBufferLength;
	TUint							iMinTurnaroundTime;
	TInt							iReceiveBufferLength;
	TUint							iSignals;
	TUint							iNotifySignals;
	TBool							iHasNotifySignalsExpected;
	TUint							iNotifySignalsExpected;
	CActiveCallbackBase*			iCallbackBreak;
	CActiveCallbackBase*			iCallbackNotifyReceiveDataAvailable;
	CActiveCallbackBase*			iCallbackNotifySignalChange;
	CT_ActiveCallbackIO*			iCallbackRead;
	CT_ActiveCallbackIO*			iCallbackReadOneOrMore;
	CT_ActiveCallbackIO*			iCallbackWrite;

	static const TEnumEntryTable	iEnumTableSignals[];
	};

#endif /* __T_RBUS_DEV_COMM_DATA_H__ */
