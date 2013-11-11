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


#if (!defined __T_RFCOMM_REMOTE_PORT_PARAMS__DATA_H__)
#define __T_RFCOMM_REMOTE_PORT_PARAMS__DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

// EPOC includes
#include <bt_sock.h>

/**
 * Test RF COMM remote port parameters class
 *
 */
class CT_RfcommRemotePortParamsData : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_RfcommRemotePortParamsData();
	
	/**
	* Two phase constructor
	*/
	static CT_RfcommRemotePortParamsData*	NewL();

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
	CT_RfcommRemotePortParamsData();
	void ConstructL();

private:
	void DestroyData();
	
	inline void	DoCmdTRfcommRemotePortParams();
	inline void	DoCmdIsValid(const TDesC& aSection);
	
	inline void DoCmdGetXOnChar(const TDesC& aSection);
	inline void	DoCmdSetXOnChar(const TDesC& aSection);
	inline void DoCmdGetXOffChar(const TDesC& aSection);
	inline void DoCmdSetXOffChar(const TDesC& aSection);
	
	inline void	DoCmdGetFlowCtrl(const TDesC& aSection);
	inline void	DoCmdSetFlowCtrl(const TDesC& aSection);

	inline void	DoCmdUpdateFlowCtrlBit(const TDesC& aSection);
	inline void DoCmdUpdateWholeFlowCtrl(const TDesC& aSection);

	inline void	DoCmdGetBitRate(const TDesC& aSection);
	inline void	DoCmdSetBitRate(const TDesC& aSection);
	
	inline void	DoCmdGetDataBits(const TDesC& aSection);
	inline void	DoCmdSetDataBits(const TDesC& aSection);
	
	inline void	DoCmdGetStopBit(const TDesC& aSection);
	inline void	DoCmdSetStopBit(const TDesC& aSection);
	
	inline void	DoCmdGetParity(const TDesC& aSection);
	inline void	DoCmdSetParity(const TDesC& aSection);

	inline void	DoCmdUpdateFromRPNTransaction(const TDesC& aSection);
	
	static void CleanupOperation(TAny* aAny);

private:
	TRfcommRemotePortParams* iData;
	};

#endif /* __T_RFCOMM_REMOTE_PORT_PARAMS__DATA_H__*/
