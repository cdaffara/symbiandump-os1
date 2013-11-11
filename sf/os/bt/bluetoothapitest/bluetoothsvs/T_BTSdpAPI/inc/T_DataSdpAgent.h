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


#if (!defined __T_DATA_SDP_AGENT_H__ )
#define __T_DATA_SDP_AGENT_H__

//	User Includes
#include "T_DataSdpAttrValueNil.h"
#include "DataWrapperBase.h"

//	EPOC includes
#include <bt_sock.h> // Bluetooth address classes

/**
 * Test Active Notification class
 *
 */
class CT_DataSdpAgent : public CDataWrapperBase, private MSdpAgentNotifier
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_DataSdpAgent*	NewL();

	/**
	* Public destructor
	*/
	~CT_DataSdpAgent();

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
	virtual TAny*	GetObject()	{ return iSdpAgent; }

	/**
	* Set the object that the data wraps
	*
	* @param	aObject object that the wrapper is testing
	*
	*/
	virtual void	SetObjectL(TAny* aAny);

	/**
	* The object will no longer be owned by this
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	DisownObjectL();

	/**
	* Query to see if there are any outstanding requests
	*
	* @return ETrue if there are any outstanding requests
	*/
	//virtual TBool	Outstanding() { return iOutstanding>0; }
	
	/**
	* Export the attribute value array which contains the request results
	*
	* @return RPointerArray
	*/
	const RPointerArray<CSdpAttrValue>& GetAttrValArray();
	
	inline virtual TCleanupOperation CleanupOperation();
	
protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_DataSdpAgent();

	/**
	* Second phase construction
	*/
	void	ConstructL();

private:
	//	MSdpAgentNotifier implementation
	virtual void	AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError);
	virtual void	AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
	virtual void	NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);

	static void CleanupOperation(TAny* aAny);

	/**
	* Helper methods
	*/
	void			DestroyData();

	//CSdpAgent	helper methods
    inline void		DoCmdNewL(const TDesC& aSection);
    inline void		DoCmdNewLC(const TDesC& aSection);
    inline void		DoCmdSetRecordFilterL(const TDesC& aSection);
    inline void		DoCmdNextRecordRequestL(const TDesC& aSection, const TInt aIndex);
    inline void		DoCmdSetAttributePredictorListL(const TDesC& aSection);
    inline void		DoCmdAttributeRequestL(const TDesC& aSection, const TInt aIndex);
    inline void		DoCmdCancel();
    inline void		DoCmdDestructor();
    inline void		DoCmdMSAN_ExtensionInterfaceL();

private:
	CSdpAgent*           			iSdpAgent;
	CSdpAttrValueList*    			iAttrValueList;
	TInt							iExpectedRecordsCount;
    TSdpServRecordHandle            iSSRHandle;
	RPointerArray<CSdpAttrValue>	iAttrValArray;
	TInt                            iAttrReqIndex;
	TInt                            iNextRecReqIndex;
	};

#endif /* __T_DATA_SDP_AGENT_H__*/
