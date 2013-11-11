/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_EXTERNAL_LOCATE_LOG_EVENT_H
#define LBS_EXTERNAL_LOCATE_LOG_EVENT_H

#include <lbs/lbslogevent.h>
#include <lbs/lbsnetprotocolbase.h>

#include <lbs/lbsloccommon.h> 			// For TLbsExternalRequestInfo
#include <lbs/lbsnetcommon.h>			// For TLbsNetSessionId

/**
the privacy request parameter structure
*/
class TLbsLoggingPrivacyRequestParams
    {
public:    
	TLbsNetSessionId			iSessionId;  
	TLbsExternalRequestInfo		iRequestInfo;
	TLbsNetPosRequestPrivacy	iRequestPrivacy;
    };

/**
the privacy response parameter structure
*/
class TLbsLoggingPrivacyResponseParams
	{
public:	
	TLbsNetSessionId iSessionId;
	/** verification result to the privacy request */
	CLbsNetworkProtocolBase::TLbsPrivacyResponse iResult;
	} ;

/** log event object for external locate request */
class CLbsExternalLocateLogEvent : public CLbsLogEvent
	{
public:
	IMPORT_C static CLbsExternalLocateLogEvent* NewL(TUid aRequestOriginator,TPositionInfoBase* aRefPosInfo, TPositionInfoBase* aPosInfo);
	IMPORT_C void SetPrivacyRequestParams(const TLbsLoggingPrivacyRequestParams& aParams);
	IMPORT_C void SetPrivacyResponseParams(const TLbsLoggingPrivacyResponseParams& aParams);
	IMPORT_C void SetReferenceLocation(TPositionInfoBase* aPosInfo);
	~CLbsExternalLocateLogEvent();

	// for external locate
	IMPORT_C TPositionInfoBase* ReferenceLocation() const;
	IMPORT_C void SetRequestOriginator(TUid aRequestor);
	IMPORT_C TUid RequestOriginator() const;
	IMPORT_C TLbsLoggingPrivacyRequestParams PrivacyRequestParams() const;
	IMPORT_C TLbsLoggingPrivacyResponseParams PrivacyResponseParams() const;

protected:
	void DoExternalizeL(RWriteStream& aWriteStream) const;
	void DoInternalizeL(RReadStream& aReadStream);
	void DoUpdateL(const CLbsLogEvent& aSourceLogEvent);
	void DoCopyL(CLbsLogEvent*& aTargetLogEvent) const;
	TInt DataSize() const;
private:
	CLbsExternalLocateLogEvent();
	void ConstructL(TUid aRequestOriginator, TPositionInfoBase* aRefPosInfo, TPositionInfoBase* aPosInfo);
	void CopyPrivacyRequestParams(TLbsLoggingPrivacyRequestParams& aTargetPrivacyInfo, 
						   const TLbsLoggingPrivacyRequestParams& aSourcePrivacyInfo);
	void CopyPrivacyResponseParams(TLbsLoggingPrivacyResponseParams& aTargetPrivacyInfo, 
						   const TLbsLoggingPrivacyResponseParams& aSourcePrivacyInfo);
private:
	/** Descriptor of the request originator */
	TUid							iRequestOriginator;
	/** Privacy request/response info */
	TLbsLoggingPrivacyRequestParams	iPrivacyReqInfo;
	/** Indicate whether the MT-LR access is granted or not */
	TLbsLoggingPrivacyResponseParams			iPrivacyResInfo;
	/** The calculation method indicates MS-Based or Autonomous GPS is withint TPositionInfoBase */
	/** The positioning mode used to calculate this location fix. */
	// TPositionModuleInfo::TTechnologyType iPositionMode;
	/** The returned reference location information if the request is completed successfully */
	TPositionInfo* 					iRefPosInfo;
	}; // class CLbsExternalLocateLogEvent

#endif // LBS_EXTERNAL_LOCATE_LOG_EVENT_H
