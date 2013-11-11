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


#if (!defined __T_AVDTP_SERVICE_CAPABILITY_H__)
#define __T_AVDTP_SERVICE_CAPABILITY_H__

//	User Includes
#include "DataWrapperBase.h"


class TAvdtpServiceCapability;

class CT_AvdtpServiceCapabilityData : public CDataWrapperBase
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


protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_AvdtpServiceCapabilityData();

	virtual TAvdtpServiceCapability*	GetAvdtpServiceCapability() const = 0;

	inline void DoCmdCategory(const TDesC& aSection);
	inline void DoCmdAsProtocol(const TDesC& aSection);
	inline void DoCmdParse(const TDesC& aSection);
	inline void DoCmdAllocFromPDUL(const TDesC& aSection);
	inline void DoCmdAllocFromPckgL(const TDesC& aSection);

	};

#endif /* __T_AVDTP_SERVICE_CAPABILITY_H__ */
