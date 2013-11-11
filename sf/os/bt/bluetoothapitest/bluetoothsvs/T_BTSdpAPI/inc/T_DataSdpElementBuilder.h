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


#if (!defined __T_DATA_SDP_ELEMENT_BUILDER_H__)
#define __T_DATA_SDP_ELEMENT_BUILDER_H__

// User includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <btsdp.h>

/**
 * Test Active Notification class
 *
 */
class CT_DataSdpElementBuilder
	{
public:
	CT_DataSdpElementBuilder(CDataWrapper& aDataWrapper);
	~CT_DataSdpElementBuilder();

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
	TBool	DoCommandL(MSdpElementBuilder& aElementBuilder, const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

private:
	/**
	* Helper methods
	*/
	inline void	DoCmdBuildUnknownL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdBuildNilL(MSdpElementBuilder& aElementBuilder);
	inline void	DoCmdBuildUintL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdBuildIntL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdBuildUUIDL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdBuildBooleanL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdBuildStringL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdBuildDESL(MSdpElementBuilder& aElementBuilder);
	inline void	DoCmdBuildDEAL(MSdpElementBuilder& aElementBuilder);
	inline void	DoCmdStartListL(MSdpElementBuilder& aElementBuilder);
	inline void	DoCmdEndListL(MSdpElementBuilder& aElementBuilder);
	inline void	DoCmdBuildURLL(MSdpElementBuilder& aElementBuilder, const TDesC& aSection);
	inline void	DoCmdMSEB_ExtensionInterfaceL(MSdpElementBuilder& aElementBuilder);

protected:
	CDataWrapper&	iDataWrapper;
	};

#endif /* __T_DATA_SDP_ELEMENT_BUILDER_H__ */
