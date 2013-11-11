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

#ifndef LBS_LOG_EVENT_H
#define LBS_LOG_EVENT_H

#include <e32base.h>
#include <e32err.h>
#include <logwrap.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <logfilterandeventconstants.hrh>
#endif


#include <logcli.h>
#include <lbs/lbsloggerdefs.h>

#include <lbspositioninfo.h>
#include <lbssatellite.h>
/**
 class for the privacy request information 
*/
class TPrivacyRequestParams;

/**
 class for the privacy response information 
*/
class TPrivacyResponseParams;

enum TLbsRequestOutcome
/**
 enum for the request outcomes
*/
	{
	ELbsRequestOutcomeInconclusive =0,
	ELbsRequestOutcomeSuccess,
	ELbsRequestOutcomeFail,
	ELbsRequestOutcomeCancel,
	};
    
class CLbsLogEvent : public CBase
/**
 Base class for the classes representing LBS events to be logged. It provides the common 
 functionality (internalising, externalising, updating this class from another one and creating 
 a copy of this class).
 
 @released
*/
	{
public:
	IMPORT_C static CLbsLogEvent* NewL(const CLogEvent& aLogEvent);
	IMPORT_C CLogEvent* ExternalizeL() const;
	IMPORT_C void SetPositionInfo(TPositionInfoBase* aPosInfo);

	virtual ~CLbsLogEvent();
	void UpdateL(const CLbsLogEvent& aLogEvent);
	void UpdateRootEvent(CLogEvent* aTargetEvent);
	CLbsLogEvent* CopyL() const;
	
	// Inline functions
	IMPORT_C void SetDirection(TBool aDirection);
	IMPORT_C TBool Direction() const;
	IMPORT_C TUid EventType() const;
	IMPORT_C void SetEventType(TUid aId);
	IMPORT_C void SetRequestOutcome(TLbsRequestOutcome aRequestOutcome);
	IMPORT_C TLbsRequestOutcome RequestOutcome() const;
	IMPORT_C TBool IsCostInformationAvailable() const;
	IMPORT_C void SetCostInformation(TUint aCostInformation);
	IMPORT_C TUint CostInformation() const;
	IMPORT_C TPositionInfoBase* PositionInfo() const;
	
protected:
	CLbsLogEvent();
	void InternalizeL(const CLogEvent& aLogEvent);
	TPositionInfo* CopyPosInfoToEvent(const TPositionInfoBase* aSrcosInfo);
	void WritePosInfo(RWriteStream& aWriteStream, const TPositionInfo* aPosInfo) const;
	void ReadPosInfo(RReadStream& aReadStream, TPositionInfo*& aPosInfo);
	// for BC consideration, possible handling TAny* data member
	// called by derived 3rd party classes
	void BaseConstructL();
	virtual void DoExternalizeL(RWriteStream& aReadStream) const = 0;
	virtual void DoInternalizeL(RReadStream& aReadStream) = 0;
	virtual void DoUpdateL(const CLbsLogEvent& aSourceLogEvent) = 0;
	virtual void DoCopyL(CLbsLogEvent*& aTargetLogEvent) const = 0;
	virtual TInt DataSize() const = 0;
protected:
	/** The size of the data from a TPositionInfo that is written to disk */
	const TInt			iPosInfoWriteSize;
	/** The Location Request Event Type */
	TUid 				iEventType;
	/** The returned location information if the request is completed successfully */
	TPositionInfo*		iPosInfo;
	/** Data for BC consideration, and inherited by derived CLbsLog classes */
	TAny*				iReservedData;
	
private:
	/**	MO-LR (True) to R_LOG_DIR_OUT and MT-LR to R_LOG_DIR_IN */
	TBool				iDirection;
	/** The outcome of the request */
	TLbsRequestOutcome 	iRequestOutcome;
	/** Flag which specifies if the cost information is available or not */
	TBool 				iCostInformationAvailable;
	/** The cost information */
	TUint 				iCostInformation;
	}; // class CLbsLogEvent
	

#endif // LBS_LOG_EVENT_H
