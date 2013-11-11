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


#if (!defined __T_AVDTP_MEDIACODEC_CAPABILITIES_DATA_H__)
#define __T_AVDTP_MEDIACODEC_CAPABILITIES_DATA_H__


class CT_AvdtpMediaCodecCapabilitiesData : public CT_AvdtpServiceCapabilityData
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
	CT_AvdtpMediaCodecCapabilitiesData();

	virtual TAvdtpServiceCapability*	GetAvdtpServiceCapability() const = 0;

	inline void DoCmdMediaType(const TDesC& aSection);
	inline void DoCmdMediaCodecType(const TDesC& aSection);
	inline void DoCmdMinWindowSize(const TDesC& aSection);
	inline void DoCmdSetMinWindowSize(const TDesC& aSection);
	inline void AllocCodecFromPDUL(const TDesC& aSection);

	};

#endif /* __T_AVDTP_MEDIACODEC_CAPABILITIES_DATA_H__ */
