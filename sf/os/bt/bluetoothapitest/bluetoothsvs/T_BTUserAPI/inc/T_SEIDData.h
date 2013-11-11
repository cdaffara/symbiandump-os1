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


#if (!defined __T_SEID_DATA_H__)
#define __T_SEID_DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

class TSEID;

class CT_SEIDData : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_SEIDData();
	
	/**
	* Two phase constructor
	*/
	static CT_SEIDData*	NewL();

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
CT_SEIDData();
	void ConstructL();

private:
	void DestroyData();
	
	inline void DoCmdnewL(const TDesC& aSection);
	inline void DoCmdSEID(const TDesC& aSection);
	inline void DoCmdPacketValue(const TDesC& aSection);
	inline void DoCmdIsValid(const TDesC& aSection);
	inline void DoCmdSet(const TDesC& aSection);
	inline void DoCmdCompareL(const TDesC& aSection);
	inline void DoCmdNegativeCompareL(const TDesC& aSection);
	inline void DoCmd_Spare(const TDesC& /*aSection*/);
	inline void DoCmdFromPacketL(const TDesC& /*aSection*/);
	inline void DoCmdReset(const TDesC& /*aSection*/);
	inline void DoCmdIsLocal(const TDesC& aSection);
	inline void DoCmdValue(const TDesC& aSection); 
	
	static void CleanupOperation(TAny* aAny);

private:
	TSEID* iData;
	};

#endif /* __T_SEID_DATA_H__*/
