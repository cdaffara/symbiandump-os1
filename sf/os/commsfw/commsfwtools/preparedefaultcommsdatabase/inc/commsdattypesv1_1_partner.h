// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms Data Type Definitions version 1.1
// Container classes expressing Symbian OS Comms Data Types that can be stored in the Comms Repository
// Type Definitions are version controlled.  New versions may be introduced from time to time.
// Then older versions are marked as deprecated and given a planned removal date.
// This version introduced with Release 9.1
// 
//

/**
 @file
 @publishedPartner
 @released
*/


#if (!defined COMMSDATTYPESV1_1_PARTNER_H)
#define       COMMSDATTYPESV1_1_PARTNER_H


#include <commsdattypesv1_1.h>

namespace CommsDat
{


//
// 12/  DEFAULT WCDMA RECORD
//
/**
@publishedPartner
@released
*/
class CCDDefaultWCDMARecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDDefaultWCDMARecord();
		/**
		@internalComponent
		*/
		CCDDefaultWCDMARecord(TMDBElementId aId);

		DATA_VTABLE
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}		

	public :

		// Member Elements

		CMDBField<TUint32>		    iUsage;								//< The usage of this default.
		CMDBField<TDesC>		        iAPN;							//< Access point name.
		CMDBField<TUint32>		    iPdpType;							//< PDP is a network protocol which is used by packet switching networks to communicate with GPRS networks. IPV6 and IPV4 are examples of PDP types.
		CMDBField<TDesC>		    	iPdpAddress;					//< PDP address of phone.
		CMDBField<TUint32>		    iPrecedence;						//< Default quality of service precedence class. Values for this are defined by the enum RPacketQoS::TQoSPrecedence.
		CMDBField<TUint32>	    	iDelay;								//< Default quality of service delay class. Values for this are defined by the enum RPacketQoS::TQoSDelay.
		CMDBField<TUint32>	    	iReliability;						//< Default quality of service reliability class. Values for this are defined by the enum RPacketQoS::TQoSReliability.
		CMDBField<TUint32>	    	iPeakThroughput;					//< Minimum quality of service peak throughput class. This field has been deprecated and may be removed in the future. 
		CMDBField<TUint32>	    	iMeanThroughput;					//< Requested quality of service mean throughput class. This field has been deprecated and may be removed in the future. 
		CMDBField<TUint32>		    iMinPrecedence;						//< Minimum quality of service precedence class. This field has been deprecated and may be removed in the future. 
		CMDBField<TUint32>	    	iMinDelay;							//< Minimum quality of service delay class This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	    	iMinReliability;					//< Minimum quality of service reliability class. This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	    	iMinPeakThroughput;					//< Minimum quality of service peak throughput class. This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	    	iMinMeanThroughput;					//< Minimum quality of service mean throughput class. This field has been deprecated and may be removed in the future.
		CMDBField<TBool>			iDataCompression;					//< Specifies whether to compress data.
		CMDBField<TBool>			iHeaderCompression;					//< Specifies whether IP header compression is on.
		CMDBField<TBool>			iUseEdge;							//< Specifies whether to use EDGE technology.
		CMDBField<TBool>			iAnonymousAccess;					//< This tells the MS (Mobile Station) whether to try anonymous access or not. Anonymous access allows the mobile device to connect to a network without having to authenticate identity. To access the network, the MS (Mobile Station) will just use a random identity.

    private:
    
    	static const SRecordTypeInfo* const iRecordInfo;
	};


	//
	// TIER RECORD
	// A Tier groups one or more APs that represent a single technology or a related group of technologies
	// So a tier identifies the type of an AP
	// A Tier is governed by a component called a Tier Manager
	// Each Tier and each Tier Manager has a unique name and unique numeric identifier
	// -----------   ---------------------
	//| ID | NAME | | PREFERENCE RESOLVER |
	//|-----------| |---------------------
	//|  * |    * | |                   * |
	// -----------   ---------------------
	// ID                     - Access point type's (tier's) unique ID.
	// NAME                   - Access point type's (tier's) unique name.
	// TIER MANAGER NAME      - Tier's preference resolver id interpreted by the framework.
	// TIER MANAGER UID       - Tier's preference resolver id interpreted by the framework.

	/**
	@publishedPartner
	@released
	*/
	class CCDTierRecord : public CCDRecordBase
		{
		public:
			CCDTierRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:
			CMDBField<TUint32> iTierImplUid; 	 // Tier's Implement Uid 
			CMDBField<TDesC> iTierThreadName;        //Tier's thread name.
			CMDBField<TDesC> iTierManagerName;       //Tier's manager name.
			CMDBField<TBool> iPromptUser;          //Tier's default access point.
			CMDBRecordLink<CCDAccessPointRecord> iDefaultAccessPoint;  //Tier's default access point.

	    private:
	        static const SRecordTypeInfo* const iRecordInfo;
		};

	//
	// MCPR TYPE RECORD
	// -----------  
	//| ID | NAME | 
	//|-----------| 
	//|  * |    * | 
	// -----------  
	/**
	@publishedPartner
	@released
	*/
	class CCDMCprRecord : public CCDRecordBase
		{
		public:
			CCDMCprRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:
			CMDBField<TUint32> iMCprUid;            //MCpr's UID.

	    private:
	        static const SRecordTypeInfo* const iRecordInfo;
		};

	//
	// CPR TYPE RECORD
	/**
	@publishedPartner
	@released
	*/
	class CCDCprRecord : public CCDRecordBase
		{
		public:
			CCDCprRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:
			CMDBField<TUint32> iCprUid;            //Cpr's UID.

	    private:
	        static const SRecordTypeInfo* const iRecordInfo;
		};

	//
	// SCPR TYPE RECORD
	/**
	@publishedPartner
	@released
	*/
	class CCDSCprRecord : public CCDRecordBase
		{
		public:
			CCDSCprRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:
			CMDBField<TUint32> iSCprUid;            //SCpr's UID.

	    private:
	        static const SRecordTypeInfo* const iRecordInfo;
		};

	//
	// PROTOCOL TYPE RECORD
	/**
	@publishedPartner
	@released
	*/
	class CCDProtocolRecord : public CCDRecordBase
		{
		public:
			CCDProtocolRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:
			CMDBField<TUint32> iProtocolUid;            //Protocol's UID.
			CMDBField<TUint32> iProtocolConfigLoaderUid;            // ecom plugin to load protocol's config
	    private:
	        static const SRecordTypeInfo* const iRecordInfo;
		};


	//
	// BEARER TYPE RECORD
	// Identifies the components needed to build an AP to perform a particular service
	/**
	@publishedPartner
	@released
	*/
	class CCDBearerTypeRecord : public CCDRecordBase
		{
		public:
			CCDBearerTypeRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:
			CMDBRecordLink<CCDTierRecord>	    iTier;
			CMDBRecordLink<CCDMCprRecord>	    iMCpr;
			CMDBRecordLink<CCDCprRecord>	    iCpr;
			CMDBRecordLink<CCDSCprRecord>	    iSCpr;
			CMDBRecordLink<CCDProtocolRecord>	iProtocol;

	    private:
	        static const SRecordTypeInfo* const iRecordInfo;
		};
	
	//
	// 0B/  WLAN SERVICE EXTENSION RECORD
	//
	class CCDEAPSecRecord;

	/**
	@publishedPartner
	@released
	*/
	class CCDWLANServiceExtRecord : public CCDServiceExtRecordBase
		{
		public:
			/**
			@internalComponent
			*/
			CCDWLANServiceExtRecord();
			/**
			@internalComponent
			*/
			CCDWLANServiceExtRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:

			// Member Data
			CMDBField<TDesC>						iWLANProfile;
			CMDBField<TDesC>						iWLANSSID;
			CMDBField<TBool>						iWLANEncryptionStatus;
			CMDBField<TCommsDatWlanEncrytionType>	iWLANEncryptionType;
			CMDBField<TUint>						iWLANDefaultWepKey ;
			CMDBField<TBool>						iWLANEncryptKey1Valid;
			CMDBField<TDesC>						iWLANWepKey1;
			CMDBField<TBool>						iWLANEncryptKey2Valid;
			CMDBField<TDesC>						iWLANWepKey2;
			CMDBField<TBool>						iWLANEncryptKey3Valid;
			CMDBField<TDesC>						iWLANWepKey3;
			CMDBField<TBool>						iWLANEncryptKey4Valid;
			CMDBField<TDesC>						iWLANWepKey4;
			CMDBField<TUint>						iWLANRanking;
			CMDBField<TCommsDatWlanDialogPref>		iWLANDialogPref;
			CMDBField<TCommsDatWlanNetworkType>		iWLANNetworkType;
			CMDBField<TUint>						iWLANFragThres;
			CMDBField<TUint>						iWLANRTSThres;
			CMDBField<TCommsDatWlanDesTransRate>	iWLANDesTransRate;
			CMDBField<TUint>						iWLANBeaconInt;
			CMDBField<TUint>						iWLANChannelId;
			CMDBField<TUint>						iWLANAuthenticationMode;
			CMDBField<TCommsDatWlanPowerSaveMode>	iWLANPowerSaveMode;
			CMDBField<TBool>						iWLANAllowUnencryptMode;
			CMDBField<TBool>						iWLANAllowShortPre;
			CMDBField<TCommsDatWlanPreambleType>	iWLANPreambleType;
			CMDBField<TUint>						iWLANTxPowerLevel;
			CMDBField<TDesC>						iWLANEapPresharedKey;
			CMDBField<TCommsDatWlanRegDomain>		iWLANRegDomain;
			CMDBRecordLink<CCDEAPSecRecord>			iWLANSecData;

	    private:
	    
	    	static const SRecordTypeInfo* const iRecordInfo;
		};
	
	//
	// 1A/  SECURE SOCKET PROTO RECORD
	//
	/**
	@publishedPartner
	@released
	*/
	class CCDSecureSocketRecord : public CCDRecordBase
		{
		public:
			/**
			@internalComponent
			*/
			CCDSecureSocketRecord();
			/**
			@internalComponent
			*/
			CCDSecureSocketRecord(TMDBElementId aElementId);

			DATA_VTABLE

	        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		public:

			// Member Data
			CMDBField<TDesC>	    iSSProtoName;
			CMDBField<TDesC>	iSSProtoLibrary;

	    private:

			static const SRecordTypeInfo* const iRecordInfo;
		};


} // namespace CommsDat

#endif // COMMSDATTYPESV1_1_PARTNER_H
