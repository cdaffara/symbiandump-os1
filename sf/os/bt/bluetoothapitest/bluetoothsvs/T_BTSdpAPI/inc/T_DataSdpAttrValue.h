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


#if (!defined __T_DATA_SDP_ATTR_VALUE_H__)
#define __T_DATA_SDP_ATTR_VALUE_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <btsdp.h>

/**
 * Test Active Notification class
 *
 */
class CT_DataSdpAttrValue : public CDataWrapperBase, public MSdpAttributeValueVisitor
	{
public:
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
			TPtr	GetVisitorResult();
			void	ResetVisitorResult();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_DataSdpAttrValue();

	virtual CSdpAttrValue*	GetSdpAttrValue() const = 0;

	//	MSdpAttributeValueVisitor default implementation
    virtual void	VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    virtual void	StartListL(CSdpAttrValueList &aList);
    virtual void	EndListL();

private:
	/**
	* Helper methods
	*/
	inline void	DoCmdAcceptVisitor(const TDesC& aSection);
	inline void	DoCmdBool(const TDesC& aSection);
	inline void	DoCmdDataSize(const TDesC& aSection);
	inline void	DoCmdDes(const TDesC& aSection);
	inline void	DoCmdDoesIntFit(const TDesC& aSection);
	inline void	DoCmdInt(const TDesC& aSection);
	inline void	DoCmdType(const TDesC& aSection);
	inline void	DoCmdUint(const TDesC& aSection);
	inline void	DoCmdUUID(const TDesC& aSection);
	inline void DoCmdMSAVV_ExtensionInterfaceL();

private:
	TPtr		iVisitorResult;
	};

#endif /* __T_DATA_SDP_ATTR_VALUE_H__ */
