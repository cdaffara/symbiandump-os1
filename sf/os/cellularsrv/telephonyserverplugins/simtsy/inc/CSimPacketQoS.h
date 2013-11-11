// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file contains the definitions of the Simulator TSY Packet Quality of Service class.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMPACKETQOS_H__
#define __CSIMPACKETQOS_H__

#include <et_phone.h>
#include <etelqos.h>
#include "utils.h"
#include "csimtimer.h"
#include "CSimPubSubChange.h"

struct TProfileConfig	//< Used for holding the GPRS Requested and Minimum Profile parameters 
	{					//< read from the configuration file.
	TUint8 iMinPrecedence;
	TUint8 iReqPrecedence;
	TUint8 iMinDelay;
	TUint8 iReqDelay;
	TUint8 iMinReliability;
	TUint8 iReqReliability;
	TUint8 iMinPeekThroughput;
	TUint8 iReqPeekThroughput;
	TUint8 iMinMeanThroughput;
	TUint8 iReqMeanThroughput;
	};

struct TNegProfileConfig	//< Used for holding the GPRS Negotiated Profile Parameters
	{						//< read from the configuration file.
	TUint8 iNegPrecedence;
	TUint8 iNegDelay;
	TUint8 iNegReliability;
	TUint8 iNegPeekThroughput;
	TUint8 iNegMeanThroughput;
	};

struct TProfileCapsGPRS	//< Used for holding the GPRS Capability Profile Parameters
	{		
	TUint8 iPrecedence;
	TUint8 iDelay;
	TUint8 iReliability;
	TUint8 iPeak;
	TUint8 iMean;
	};


struct TProfileReqR99			//< Used for holding the Rel99 Release Profile parameters
	{
		TName iContextName;
		TUint8 iReqTraffic;
		TUint8 iMinTraffic;
		TUint8 iReqDeliveryOrder;
		TUint8 iMinDeliveryOrder;
		TUint8 iReqErroneousSDU;
		TUint8 iMinErroneousSDU;
		TInt   iMaxSDUSize;
		TInt   iMinSDUSize;
		TInt   iReqUpLinkTBitRate;
		TInt   iReqDownLinkTBitRate;
		TInt   iMinUpLinkTBitRate;
		TInt   iMinDownLinkTBitRate;
		TUint8 iReqTBitErrorRatio;
		TUint8 iMinTBitErrorRatio;
		TUint8 iReqTSDUErrorRatio;
		TUint8 iMinTSDUErrorRatio;
		TUint8 iReqTTrafficHandlingPriority;
		TUint8 iMinTTrafficHandlingPriority;
		TInt   iReqTransferDelay;
		TInt   iMinTransferDelay;
		TInt   iReqGuaranteedUpLinkTBitRate;
		TInt   iReqGuaranteedDownLinkTBitRate;
		TInt   iMinGuaranteedUpLinkTBitRate;
		TInt   iMinGuaranteedDownLinkTBitRate;
	};

struct TProfileNegR99				//< Used for holding the Neg99 Negotiated Profile parameters
	{
		TName iContextName;
		TUint8 iDuration;
		TUint8 iNegTraffic;
		TUint8 iNegDeliveryOrder;
		TUint8 iNegErroneousSDUDelivery;
		TInt iNegMaxSDUSize;
		TUint8 iNegTBitErrorRatio;
		TUint8 iNegTSDUErrorRatio;
		TUint8 iNegTrafficHandlingPriority;
		TInt   iNegTransferDelay;
		TInt   iNegGuaranteedUpLinkBitRate;
		TInt   iNegGuaranteedDownLinkBitRate;
		TInt   iNegMaxUpLinkRate;
		TInt   iNegMaxDownLinkRate;
	};

struct TProfileCapsR99				//< Used for holding the Rel99 Capability Profile parameters
	{
		TUint8 iTraffic;
		TUint8 iDeliveryOrderReq;
		TUint8 iDeliverErroneousSDU;
		TUint8 iBER;
		TUint8 iSDUErrorRatio;
		TUint8 iTrafficHandlingPriority;
	};
	
struct TProfileCapsR5 : TProfileCapsR99	//< Used for holding the Rel5 Capability Profile parameters
	{
		TBool  iSignallingIndication;
		TUint8 iNegTSourceStatisticsDescriptor;
	};

struct TSetProfileFail			// struct to SetProfile() call to fail
	{
		TInt iNumberOfTimes;
		TInt iFailErrorCode;
	};
	
struct TProfileReqR5 : TProfileReqR99	//< Used for holding the Rel5 Release Profile parameters
	{
		TBool  iSignallingIndication;
		TUint8 iReqTSourceStatisticsDescriptor;
	};

struct TProfileNegR5 : 	TProfileNegR99	//< Used for holding the Neg5 Negotiated Profile parameters
	{
		TBool  iSignallingIndication;
		TUint8 iNegTSourceStatisticsDescriptor;
	};


class CSimPacketContext;
class CSimPacketQoS : public CSubSessionExtBase, public MTimerCallBack, public MPubSubChangeCallback
	{
public:
	static CSimPacketQoS* NewL(CSimPhone* aPhone, CSimPacketContext* aPacketContext);
	CSimPacketQoS(CSimPhone* aPhone, CSimPacketContext* aPacketContext);
	void ConstructL();
	~CSimPacketQoS();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	TInt PerformProfileChange();
	void ClearChangeQoSPending();

private:
	TInt SetProfile(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig);
	TInt SetProfileCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetProfile(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig);
	TInt GetProfileCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetProfileCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig);
	TInt GetProfileCapsCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyProfileChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig);
	TInt NotifyProfileChangedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt MatchContext(TProfileNegR99& R99NegProfile);
	TInt MatchContext(TProfileReqR99& R99ReqProfile);
	
	TInt MatchContext(TProfileNegR5& R5NegProfile);
	TInt MatchContext(TProfileReqR5& R5ReqProfile);
	
	void CompleteNotifications(TProfileNegR99& aProfile);
	void CompleteNotifications(TProfileNegR5& aProfile);

	void PubSubCallback(TInt aRegStat);

	// methods to retrieve config file settings
	void GetGPRSReqProfilesL();
	void GetGPRSNegProfilesL();
	void GetR99ReqProfilesL();
	void GetR99NegProfilesL();
	void GetR99NetworkNotificationsL();
	void GetGPRSQosProfileCaps();
	void GetR99QosProfileCaps();
	void GetProfileFailSettings();
	void GetR5ReqProfilesL();
	void GetR5NegProfilesL();
	void GetR5NetworkNotificationsL();
	void GetR5QosProfileCaps();
	void GetSetQoSSettings();
	

private: //Data
	CSimPhone* iPhone;
	CSimPacketContext* iPacketContext;				//< pointer to the parent class CSimPacketContext
	CArrayFixFlat<TProfileConfig>* iProfiles;		//< Holds a list of requested GPRS profiles read from the config file
	CArrayFixFlat<TNegProfileConfig>* iNegProfiles;	//< Holds a list of Negotiated GPRS profiles read from the config file
		
	CArrayFixFlat<TProfileReqR99>* iReqR99Profiles;
	CArrayFixFlat<TProfileNegR99>* iNegR99Profiles;
	CArrayFixFlat<TProfileCapsR99>* iCapsR99Profiles;

	CArrayFixFlat<TProfileReqR5>* iReqR5Profiles;
	CArrayFixFlat<TProfileNegR5>* iNegR5Profiles;
		
	TNotificationType iNetworkNotificationType;
	CArrayFixFlat<TProfileNegR99>* iQosNetworkNotifications;
	CArrayFixFlat<TProfileNegR5>* iQosR5NetworkNotifications;

	TInt iCurrentNeg;			//< index for the iNegProfiles.  indicates the next profile to use.
	TInt iCurrentNeg99;
	TInt iCurrentNegR5;
	TBool iSetProfilePending;
	
	TInt iCurrentProfileRel;	//< Release format used to set this QoS (only used for R99_R4 and R5 formats)
	
	TNotifyData	iNotifyProfileGPRS;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
	TNotifyData	iNotifyProfileR99;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
	TNotifyData	iNotifyProfileR5;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
	TProfileCapsGPRS iCapsGPRS;	//< holds the GPRS Profile capabilities parameters
		
	TProfileReqR99 iReq99;
	TProfileNegR99 iNegR99;
	TProfileCapsR99 iCapsR99;
	TProfileCapsR5 iCapsR5;

	TProfileNegR99 iQosNetworkNotification; 
	TSetProfileFail iSetProfileFail;
	/******/

	TInt iSetProfileCallCount;
	TInt iQosNetworkNotificationCount;
	TInt iQosR5NetworkNotificationCount;
	TBool iUseNetworkNotificationBool;
	virtual void TimerCallBack(TInt aId);

	CSimTimer* iTimer;					//< pointer to the timer object
	CSimTimer* iSetQoSTimer;			//< pointer to the timer object used for set qos delay

	TBool iTimerStartedBool;

	CSimPubSubChange* iSimQoSChange;
	
 	TInt iSetQoSDelay;	//< Used to hold the delay before setting qos
 	TInt iSetQoSDelayAt; //< Which set qos operation should the delay be introduced
 	
 	class TSetQoSData	//< Class used to construct an array of qos data 
 		{
 		public:
 		static TInt Find(CArrayFixFlat<TSetQoSData>* aSetQoSArray, TTsyReqHandle aTsyReqHandle, TInt& aIndex);
 		TTsyReqHandle iTsyReqHandle;
 		TDesC8* iData;
 		};
 	CArrayFixFlat<TSetQoSData> *iSetQoSData; //< array used to store qos data prior to set qos delay	
	
	};

#endif
