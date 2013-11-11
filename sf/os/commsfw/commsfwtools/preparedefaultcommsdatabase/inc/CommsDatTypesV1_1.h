// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#if (!defined COMMSDATTYPESV1_1_H)
#define       COMMSDATTYPESV1_1_H

#include <metadatabase.h>
#include <commsdattypeinfov1_1.h>
#include <cdbcols.h>
#include <dial.h>

#include <etelqos.h>


namespace CommsDat
{


//
// COMMSDAT RECORD BASE CLASS
// Contains fields common to all records
//

class CCDRecordBase : public CMDBRecordBase
/**
Base class for wrapper containers for supported CommsDat recordtypes
Alternatively the class CMDBGenericRecord can be used to express any record.
@publishedAll
@released
*/
	{
	public:

		IMPORT_C CCDRecordBase(TMDBElementId aElementId); 			//< Import CCDRecordBase constructor from another DLL.

		IMPORT_C static CMDBRecordBase* RecordFactoryL(TMDBElementId); //< Import function RecordFactoryL from another DLL.				 
		
		IMPORT_C static CMDBRecordBase* CreateCopyRecordL(CMDBRecordBase& aCopyFromRecord); //< Import function CreateCopyRecordL from another DLL.
	
        void ConstructL();
	protected:

		EXP_DATA_VTABLE
			
	public:
		
		// Element members

		CMDBField<TInt>	 iRecordTag; 	//<User-defined numeric tag for this record.  Should be unique in this table.   
        
        CMDBField<TDesC> iRecordName; 	//< User-defined name for this record.  Should be unique in this table.

	private:

        CCDRecordBase(){}
	};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//
// BASE CLASS FOR ALL SERVICE RECORDS
// Contains fields common to all service records
//
/**
@publishedAll
@released
*/
class CCDParamsRecordBase : public CCDRecordBase
	{
	public:
		// Constructor
		/**
		@internalComponent
		*/
		CCDParamsRecordBase();
		/**
		@internalComponent
		*/
		CCDParamsRecordBase(TMDBElementId aElementId);

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	protected:

		DATA_VTABLE
    
   	public:

		// Field Declarations shared by all Params Records
		CMDBField<TUint32>		iSTypeId;
	
	private:
	
	    static const SRecordTypeInfo* const iRecordInfo;
	};
#endif

//
// BASE CLASS FOR ALL SERVICE RECORDS
// Contains fields common to all service records
//
/**
@publishedAll
@released
*/
class CCDServiceRecordBase : public CCDRecordBase
	{
	public:
		// Constructor
		/**
		@internalComponent
		*/
		CCDServiceRecordBase();
		/**
		@internalComponent
		*/
		CCDServiceRecordBase(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	protected:

		DATA_VTABLE
    
   	public:

		// Field Declarations shared by all Service Records
		CMDBField<TBool>	iServiceEnableLlmnr;	//< Specifies whether Link-local multicast name resolution is enabled.
	
	private:
	
	    static const SRecordTypeInfo* const iRecordInfo;
	};



//
// BASE CLASS FOR ALL BEARER RECORDS
// Contains fields common to all bearer records
//
/**
@publishedAll
@released
*/
class CCDBearerRecordBase : public CCDRecordBase
	{
	public:

		// Constructor
		/**
		@internalComponent
		*/
		CCDBearerRecordBase();
		/**
		@internalComponent
		*/
		CCDBearerRecordBase(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;} 

	protected:

		DATA_VTABLE

    public:

		// Field Declarations shared by all Bearer Records
        CMDBField<TDesC>					iBearerAgent; //< Name of the agent to be used by a particular bearer

	private:

        static const SRecordTypeInfo* const iRecordInfo;
	};



class CCDIAPRecord;

//
// 02/  CONNECTION PREFERENCES RECORD
//
// Forward declaration
/**
@publishedAll
@released
*/
class CCDConnectionPrefsRecord : public CCDRecordBase
	{
	public :
		/**
		@internalComponent
		*/
		CCDConnectionPrefsRecord();
		/**
		@internalComponent
		*/
		CCDConnectionPrefsRecord(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;} 
        
        DATA_VTABLE
        
	public :
		// Member Elements
		CMDBField<TUint32>		        iRanking; 			//< IAP ranking- An IAP with ranking of 1 is attempted first and so forth. An IAP with a ranking of 0 will never be attempted for connection.
		CMDBField<TCommDbConnectionDirection>	iDirection; //< Specifies the direction of a connection.	
		CMDBField<TUint32>		        iBearerSet;   		//< Bearers which can be used for a connection.
		CMDBField<TUint32>		        iDialogPref;  		//< Specifies whether the system, on a connection attempt being started, should: a) offer the user a menu of the available connection options; b) warn that a connection is about to take place; c) connect without further notifying the user. Takes a value from the enum TCommDbDialogPref. Default value is ECommDbDialogPrefUnknown. 
		CMDBRecordLink<CCDIAPRecord>	iDefaultIAP;  		//< The record id of the preferred IAP to connect to.
    private:
        static const SRecordTypeInfo* const iRecordInfo;
	};



//Forward defines
class CCDLocationRecord;
class CCDModemBearerRecord;
class CCDNetworkRecord;
class CCDAgentLookupRecord;
class CCDWAPAccessPointRecord;
class CCDAccessPointRecord;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class CCDTierRecord;
#endif

//
// 03/  GLOBAL SETTINGS RECORD
//
/**
@publishedAll
@released
*/
class CCDGlobalSettingsRecord : public CCDRecordBase
	{
	public :

		// Constructor
		/**
		@internalComponent
		*/
		CCDGlobalSettingsRecord();
		/**
		@internalComponent
		*/
		CCDGlobalSettingsRecord(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

		DATA_VTABLE
		
	 public:
	
		CMDBRecordLink<CCDWAPAccessPointRecord>		                        iWAPAccessPoint; 		
		CMDBField<TUint32>                                                  iMaxConnectionAttempts;				//< Number of attempts to establish a connection.
		CMDBField<TUint32>                                                  iMaxRedialAttempts;					//< Maximum redial attempts before report failure to user.

		CMDBField<TUint32>                                                  iSMSBearer;							//< Default bearer for SMS on the phone.
		CMDBField<TUint32>                                                  iSMSReceiveMode;					//< Clients (SMS Stack on the phone) can receive an SMS either before or after the message has been stored. If the client receives a new message, which the phone has already stored and has acknowledged, then the client does not have to do anything further. If the client receives an unstored new message which the phone has not acknowledged, then the client has the responsibility of attempting to store the message and then either "acking" or "nacking" that message to the network.


		CMDBField<TUint32>                                                  iGPRSAttachMode;					//< Determines when the GPRS terminal should attach to the network. i.e. attach at the boot-up of the device or at a later time.
		CMDBField<TUint32>                                                  iAcceptIncomingGPRS;				//< Specifies whether or not to allow incoming requests for PDP context activation to be automatically accepted (when there is a waiting application).
		CMDBField<TUint32>                                                  iGPRSClassCBearer;				//< The preferred bearer when the device (phone) is forced into GPRS Class C operation.
		
		CMDBRecordLink<CCDModemBearerRecord>                                iModemForDataAndFax;
		CMDBRecordLink<CCDModemBearerRecord>                                iModemForPhoneServicesAndSMS;

		CMDBRecordLink<CCDLocationRecord>                                   iLocationForDataAndFax;				//< Default local location info for data and fax calls.
		CMDBRecordLink<CCDLocationRecord>                                   iLocationForPhoneServicesAndSMS;	//< Sets the default location from which you are dialling for phone services such as a SMS service
		
		CMDBField<TUint32>							                        iMaxMBufHeap;						//< Maximum RMBuf heap size.
		CMDBRecordLink<CCDAgentLookupRecord>                                iDefaultAgent;						//< Record id of an agent from the AgentLookup table.

		CMDBRecordLink<CCDNetworkRecord>                                    iDefaultNetwork;					//< Default network to connect to.

		CMDBField<TDesC>                                                    iBearerAvailabilityCheckTSY;		//< The name of the TSY that should be used for bearer availability checking. If this global setting is not found then the TSY specified in ModemBearer is used.
		
		//This field differentiate the Bravo selection from the 399 selection
		//(needed for implicit scenarios where prefs are not available)
		CMDBRecordLink<CCDAccessPointRecord>								iDefaultSnap;						//<CDMA PARAMETER- Not currently supported.

		//if SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		//@deprecated use CCDTierRecord::iPromptUser instead
		CMDBField<TBool>													iPromptForSnap; 
		//endif

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		CMDBRecordLink<CCDTierRecord>                                       iDefaultTier;
#endif

CMDBField<TCommsDatWlanRegDomain>											iRegulatoryDomain;

		CMDBField<TBool>                                                  	iWmmEnabled;
		CMDBField<TBool>													iEnableCountryCode;
		CMDBField<TCommsDatWlanRegDomain>									iDefaultRegulatoryDomain;
	private:
	
		static const SRecordTypeInfo* const iRecordInfo;
	};



//
// 04/  NETWORK RECORD
//
/**
@publishedAll
@released
*/
class CCDNetworkRecord : public CCDRecordBase
	{
	public :
		/**
		@internalComponent
		*/
		CCDNetworkRecord();
		/**
		@internalComponent
		*/
		CCDNetworkRecord(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}	

    	DATA_VTABLE

	public:
       		CMDBField<TDesC>		    iHostName;	//< Name of the host of the network

	private:

		static const SRecordTypeInfo* const iRecordInfo;

	};



//
//  05/  LOCATION RECORD
//
/**
@publishedAll
@released
*/
class CCDLocationRecord : public CCDRecordBase
	{
	public :

		// Constructors
		/**
		@internalComponent
		*/
		CCDLocationRecord();
		/**
		@internalComponent
		*/
		CCDLocationRecord(TMDBElementId aElementId);

			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}  	

		DATA_VTABLE


	public:
		// MemberData
		CMDBField<TDesC>	        iIntlPrefixCode;				//< Prefix code to use for international calls.
		CMDBField<TDesC>	        iNatPrefixCode;					//< Prefix code to use for national calls.
		CMDBField<TDesC>		    iNatCode;						//< Country code.
		CMDBField<TDesC>		    iAreaCode;						//< Area code.
		CMDBField<TDesC>		    iDialOutCode;					//< Number to dial for an outside line.
		CMDBField<TDesC>	        iDisableCallWaitingCode;		//< Code to dial to disable call waiting facility.
		CMDBField<TBool>		iMobile;							//< Specifies whether the device is a mobile phone.
		CMDBField<TBool>		iUsePulseDial;						//< Specifies whether to use pulse dialling.
		CMDBField<TBool>	    iWaitForDialTone;					//< Specifies whether to wait for the dial tone.
		CMDBField<TUint32>	    iPauseAfterDialOut;					//< This specifies the length of the pause (in seconds) after using the dial-out code. In the dial string this delay is represented as a comma. The number indicates the value of that comma.


    private:

		static const SRecordTypeInfo* const iRecordInfo;
	};

//
// 06/  IAP RECORD
//
/**
@publishedAll
@released
*/
class CCDIAPRecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDIAPRecord();
		/**
		@internalComponent
		*/
		CCDIAPRecord(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}		
    
    	DATA_VTABLE

	public:

		// Member Data
		CMDBField<TDesC>                        iServiceType;			//< The name of a servicing table. E.g. DialOutISP.
		CMDBRecordLink<CCDServiceRecordBase>	iService;				//< Every IAP provides a specific service.  The field IAPService provides a link between the IAP and the servicing table, It tells us which record to use from the servicing table. For example, an IAP could provide the DialOutISP service.  Say IAPService = 1, this would mean the settings of first record in the DialOutISP table is provided by this particular IAP.
		CMDBField<TDesC>                        iBearerType;			//< The name of the bearer to be used for the IAP.
		CMDBRecordLink<CCDBearerRecordBase>	    iBearer;				//< The record id from the Bearer table specifying the bearer to be used with the IAP.
		CMDBRecordLink<CCDNetworkRecord>	    iNetwork;				//< The record id of the network to be used with the IAP. The IAP will connect to this network.
		CMDBField<TUint32>	                    iNetworkWeighting;		//< When there are multiple IAPs that wish to connect to the same network, this subset of IAPs is given a weighting. This determines the order of connection to that particular network. The IAP with the highest weighting will receive the first connection attempt and if that fails, the next highest IAP is attempted and so forth. This is an alternative way to make a connection if the ConnectionPreferences ranking system is not preferred.
		CMDBRecordLink<CCDLocationRecord>	    iLocation;				//< The record id specifying a location for the IAP. This field links the Location table and the IAP table together. The location determines factors such as areacode prefix dialling etc.
#ifdef SYMBIAN_NETWORKING_UMTSR5	
		CMDBField<TUint32>	                    iAppSid;   // Marks this IAP for exclusive use of a particular Application
#endif
    private:
    
	    static const SRecordTypeInfo* const iRecordInfo;
	};



//
// 07/  WAP ACCESS POINT RECORD
//
/**
@publishedAll
@released
*/
class CCDWAPAccessPointRecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDWAPAccessPointRecord();
		/**
		@internalComponent
		*/
		CCDWAPAccessPointRecord(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}			

		DATA_VTABLE        

	public :

		// Member Elements
		CMDBField<TDesC>    iWAPCurrentBearer;								//< The name of the table from which to read the bearer information. This can be WAP_IP_BEARER or WAP_SMS_BEARER.
		CMDBField<TDesC>    iWAPStartPage;									//< WAP start page URL.

    private:
    
		static const SRecordTypeInfo* const iRecordInfo;
	};


//Forward declaration
class CCDChargecardRecord;

//
// 08/  DIAL OUT ISP SERVICE RECORD
//
/**
@publishedAll
@released
*/
class CCDDialOutISPRecord : public CCDServiceRecordBase
	{

	public:
		/**
		@internalComponent
		*/
		CCDDialOutISPRecord();
		/**
		@internalComponent
		*/
		CCDDialOutISPRecord(TMDBElementId aId);

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


	public:

		// Member Data

		CMDBField<TDesC>	                        iDescription;			//< Description of the ISP.
		CMDBField<TUint32>	                    iType;						//< Type of browser protocol used. The term 'browser protocol' is used to differentiate between the standards that are used when browsing web pages. For example HTML for general internet standards and WML for WAP standards.
		CMDBField<TDesC>	                        iDefaultTelNum;			//< Default phone number for an ISP, used to dial-up to the ISP.
		CMDBField<TBool>	                    iDialResolution;			//< Specifies whether to add additional information from the Location table such as AreaCode, NatCode to the ISP number (DefaultTelNum) to be dialled.
		CMDBRecordLink<CCDChargecardRecord>	    iChargecard;				//< This is an optional field which is set when the ISP has permission from the user to use their chargecard details such as their account number.	CCDDialOutISP ::iChargecard
		CMDBField<TBool>	                    iUseLoginScript;			//< Specifies whether to login with a script or not.
		CMDBField<TDesC>	                        iLoginScript;			//< The actual script used to login to the ISP.
		CMDBField<TBool>	                    iPromptForLogin;			//< Prompt user to enter login information to start a session with the ISP, i.e. username and password.
		CMDBField<TDesC>	                        iLoginName;				//< User login name.
		CMDBField<TDesC>	                        iLoginPass;				//< User login password.
		CMDBField<TBool>	                    iDisplayPct;				//< Specifies whether to display the post connection terminal (PCT) (without scanning the login script for READ commands).		
		CMDBField<TDesC> 	                    iIfParams;					//< Interface parameter string-used to specify the lower layer that the interface protocol is to use.
		CMDBField<TDesC>	                        iIfNetworks;			//< Comma separated list of network protocols to be used by the NIF.
		CMDBField<TBool>	                    iIfPromptForAuth;			//< Specifies whether or not to request an authentication username and password. This depends on whether or not the user wishes to connect to the ISP's NIF(s).
		CMDBField<TDesC>	                        iIfAuthName;			//< Prompt user to enter authentication username. Used when a user wishes to connect to the ISP's NIF(s). E.g. PPP.
		CMDBField<TDesC>	                        iIfAuthPass;			//< Prompt user to enter authentication password used by an interface protocol such as PPP. Used when a user wishes to connect to the ISP's NIF(s). E.g. PPP.
		CMDBField<TUint32>	                    iIfAuthRetries;				//< Number of times to retry authentication if it fails.
		CMDBField<TBool>	                    iIfCallbackEnabled;			//< Specifies whether call-back is enabled.  Call-back is usually due to security or toll-saving reasons.
		CMDBField<TUint32>	                    iIfCallbackType;			//< The Internet Engineering Task Force (IETF) Callback type. For more detailed information of the meaning of the possible values for this field, please refer to information regarding the Call-back Control Protocol (CBCP) and the Microsoft CBCP.
		CMDBField<TDesC8>	                        iIfCallbackInfo;		//< Info for call-back request if enabled. This information could for example be a phone number.
		CMDBField<TUint32>	                    iCallbackTimeout;			//< Time duration to wait for the ISP to call back and establish a connection. In microseconds (if call-back is enabled).
		CMDBField<TBool>	                    iIfServerMode;				//< PPP in server mode?
		CMDBField<TBool>	                    iIpAddrFromServer;			//< Specifies whether to get the IP address from the ISP.
		CMDBField<TDesC>	                        iIpAddr;				//< Static IP Address (if required). This is the IP address of a NIF. This is used if the address is not dynamically allocated by the ISP.
		CMDBField<TDesC>	                        iIpNetMask;				//< IP netmask of the NIF.
		CMDBField<TDesC>	                        iIpGateway;				//< IP address of the Gateway.
		CMDBField<TBool>	                    iIpDnsAddrFromServer;		//< Specifies whether to get the DNS address (IPV4) from the ISP.
		CMDBField<TDesC>	            iIpNameServer1;						//< Static address of the Primary IPV4 DNS server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.
		CMDBField<TDesC>	            iIpNameServer2;						//< Static address of the Secondary IPV4 DNS server (if required). May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TBool>	        iIp6DnsAddrFromServer;					//< Specifies whether to get the DNS address (IPV6) from the ISP.
		CMDBField<TDesC>	            iIp6NameServer1;					//< Static address of the Primary IPV6 name server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.
		CMDBField<TDesC>	            iIp6NameServer2;					//< Static address of the Secondary IPV6 name server if required. May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TDesC>	        iIpAddrLeaseValidFrom;					//< Start of the address lease for dynamic address allocation by the ISP (e.g. a DHCP assigned IP address). Start time information stating when the IP address was leased out. The format is as follows:  dd/mm/yyyy hr:mins AM/PM, e.g. 04/08/2006 6:21 PM.
		CMDBField<TDesC>	        iIpAddrLeaseValidTo;					//< End of the address lease for dynamic address allocation by the ISP (e.g. a DHCP assigned IP address). End time information stating when the IP address lease finishes. The format is as follows:  dd/mm/yyyy hr:mins AM/PM, e.g. 08/08/2006 6:21 PM.
		CMDBField<TDesC>	    iConfigDaemonManagerName;					//< Name of the ECOM daemon manager plug-in for NIFMAN used to load a specific configuration daemon.
		CMDBField<TDesC>	            iConfigDaemonName;					//< Name of the daemon used for address configuration. 
		CMDBField<TBool>	        iEnableIpHeaderComp;					//< Specifies whether to enable IP header compression protocol.
		CMDBField<TBool>	        iEnableLcpExtension;					//< Specifies whether to enable LCP extension protocol. 
		CMDBField<TBool>	        iDisablePlainTextAuth;					//< Specifies whether to disable plain text authentication. If disabled, PAP cannot be used- CHAP will be used instead.
		CMDBField<TBool>	                iEnableSwComp;					//< Specifies whether to enable software compression. Here, the data payload is compressed, which is separate to header compression. Enabling this field will enable compression such as Microsoft or Predictor compression.
		CMDBField<TUint32>	                iBearerName;					//< Name of the bearer used to establish and transmit data over the connection
		CMDBField<TUint32>	                iBearerSpeed;					//< Baud rate of the bearer.
		CMDBField<TUint32>	                    iBearerCe;					//< Defines the quality of service (QoS) of the connection.
		CMDBField<TDesC8>	                iInitString;					//< Initialisation string for the bearer, used specifically when a modem is in use. It is used to initialise (init) the modem by sending it a string of commands, typically Hayes commands.
		CMDBField<TUint32>	                iBearerType;					//< The bearer's chosen method to transmit data. This can be of 2 types - either CSD or HSCSD.  
		CMDBField<TUint32>	            iChannelCoding;						//< The channel coding used for the connection. This field can only be used when the BearerType field has a value of "HSCSD".
		CMDBField<TUint32>	                        iAiur;					//< Air interface user rate (AIUR) - The AIUR (Air Interface User Rate) is the rate indicated between a Mobile Terminal (Mobile Station) and the IWF (Interworking function) for transparent and non-transparent data services.
		CMDBField<TUint32>	        iRequestedTimeSlots;					//< Requested number of time slots for HSCSD.
		CMDBField<TUint32>	            iMaximumTimeSlots;					//< Maximum number of time slots for HSCSD which could be requested during the connection.
		CMDBField<TUint32>	            iBearerService;						//< Bearer service refers to a service that allows transmission of data between NIFs. It defines the correct service to use when setting up the data connection.
		CMDBField<TUint32>	            iBearerProtocol;					//< A set of standards governed by the ITU-T allowing different bearers to communicate with each other. Bearer protocol definitions are typically referred to by the letter V followed by a number.
		CMDBField<TUint32>	                iRlpVersion;					//< Radio Link Protocol (RLP)- is an automatic repeat request (ARQ) protocol used over a wireless interface. RLP is differentiated by version numbers.
		CMDBField<TUint32>	                    iIwfToMs;					//< Integer specifying network to MS (Mobile Station) window size. IWF stands for Interworking Function and MS stands for Mobile Station.
		CMDBField<TUint32>	                    iMsToIwf;					//< Integer specifying MS (Mobile Station) to network window size.
		CMDBField<TUint32>	                    iAckTimer;					//< Acknowledgement Timer: takes an integer specifying RLP ack timeout in microseconds.
		CMDBField<TUint32>	    iRetransmissionAttempts;					//< Specifies number of RLP retransmission attempts.
		CMDBField<TUint32>	            iResequencePeriod;					//< Specifies the period when packets are being resequenced.
		CMDBField<TUint32>	            iV42Compression;					//< Enumeration specifying 1) Whether or not to set V.42 Compression 2) Direction that compression is used. V.42 compression follows the V.42bis protocol which is a data compression protocol used on top of the bearer protocol.
		CMDBField<TUint32>	                iV42Codewords;					//< Specifies the V.42 codewords if V.42 compression is used.  A codeword represents a string of characters in compressed form.
		CMDBField<TUint32>	                iV42MaxLength;					//< Specifies the maximum string length of the data before V.42 compression.
		CMDBField<TUint32>	                iAsymmetry;						//< For HSCSD. An enumeration to specify whether 1) asymmetrical bias on the connection is preferred 2) Type of asymmetrical bias used. Note: The service provided by uplink and downlink is different.
		CMDBField<TBool>	            iUserInitUpgrade;					//< Specifies whether the user will initialise service upgrade over HSCSD or not
		CMDBField<TBool>	                    iUseEdge;					//< Specifies whether to use EDGE technology.

    private:
    
    	static const SRecordTypeInfo* const iRecordInfo;
    };




//
// 09/  DIAL IN ISP RECORD
//
/**
@publishedAll
@released
*/
class CCDDialInISPRecord : public CCDServiceRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDDialInISPRecord();
		/**
		@internalComponent
		*/
		CCDDialInISPRecord(TMDBElementId aId);

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


	public:

		// Member Data
		CMDBField<TDesC>	            iDescription;  					//< Description of the ISP.
		CMDBField<TUint32>	                    iType;					//< Type of browser protocol used. The term 'browser protocol' is used to differentiate between the standards that are used when browsing web pages. For example HTML for general internet standards and WML for WAP standards.
		CMDBField<TBool>	        iUseLoginScript;					//< Specifies whether to login with a script or not
		CMDBField<TDesC>	            iLoginScript;					//< The actual script used to login to the ISP
		CMDBField<TUint32>	        iAuthentication;					//< Reserved for future use.
		CMDBField<TUint32>	        iIfAuthRetries;						//< Number of times to retry authentication if it fails.
		CMDBField<TDesC>	                iIfParams;					//< Interface parameter string-used to specify the lower layer that the interface protocol is to use.
		CMDBField<TDesC>	            iIfNetworks;					//< Comma separated list of network protocols to be used by the NIF.
		CMDBField<TBool>	        iIpAddrFromServer;					//< Specifies whether to get the IP address from the ISP.
		CMDBField<TDesC>	                iIpAddr;					//< Static IP Address (if required). This is the IP address of a NIF. This is used if the address is not dynamically allocated by the ISP.
		CMDBField<TDesC>	            iIpNetMask;						//< IP netmask of the NIF.
		CMDBField<TDesC>	            iIpGateway;						//< IP address of the Gateway.
		CMDBField<TBool>	    iIpDnsAddrFromServer;					//< Specifies whether to get the DNS address (IPV4) from the ISP.
		CMDBField<TDesC>	        iIpNameServer1;						//< Static address of the Primary IPV4 DNS server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.
		CMDBField<TDesC>	        iIpNameServer2;						//< Static address of the Secondary IPV4 DNS server (if required). May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TBool>	    iIp6DnsAddrFromServer;					//< Specifies whether to get the DNS address (IPV6) from the ISP.
		CMDBField<TDesC>	        iIp6NameServer1;					//< Static address of the Primary IPV6 name server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.
		CMDBField<TDesC>	        iIp6NameServer2;					//< Static address of the Secondary IPV6 name server if required. May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TBool>	    iEnableIpHeaderComp;					//< Specifies whether to enable IP header compression protocol.
		CMDBField<TBool>	    iEnableLcpExtension;					//< Specifies whether to enable LCP extension protocol. 
		CMDBField<TBool>	    iDisablePlainTextAuth;					//< Specifies whether to disable plain text authentication. If disabled, PAP cannot be used- CHAP will be used instead.
		CMDBField<TBool>	            iEnableSwComp;					//< Specifies whether to enable software compression. Here, the data payload is compressed, which is separate to header compression. Enabling this field will enable compression such as Microsoft or Predictor compression.
		CMDBField<TUint32>	            iBearerName;					//< Name of the bearer used to establish and transmit data over the connection.
		CMDBField<TUint32>	            iBearerSpeed;					//< Baud rate of the bearer.
		CMDBField<TUint32>	                iBearerCe;					//< Defines the quality of service (QoS) of the connection.
		CMDBField<TDesC8>	            iInitString;					//< Initialisation string for the bearer, used specifically when a modem is in use. It is used to initialise (init) the modem by sending it a string of commands, typically Hayes commands.
		CMDBField<TBool>	                iUseEdge;					//< Specifies whether to use EDGE technology.


    private:
	
		static const SRecordTypeInfo* const iRecordInfo;
	};



//
//  0A/  LAN Service Record
//
typedef CCDRecordBase CCDServiceExtRecordBase;
/**
@publishedAll
@released
*/
class CCDLANServiceRecord : public CCDServiceRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDLANServiceRecord();
		/**
		@internalComponent
		*/
		CCDLANServiceRecord(TMDBElementId aElementId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}				

		DATA_VTABLE


	public:

		// Member Data
		CMDBField<TDesC>	                iIfNetworks;					//< Comma separated list of network protocols to be used by the NIF.
		CMDBField<TDesC>	                iIpNetmask;						//< IP netmask of the NIF.
		CMDBField<TDesC>	                iIpGateway;						//< IP address of the Gateway.
		CMDBField<TBool>	                iIpAddrFromServer;				//< Specifies whether to get the IP address from the ISP.
		CMDBField<TDesC>	                iIpAddr;						//< Static IP Address (if required). This is the IP address of a NIF. This is used if the address is not dynamically allocated by the ISP.
		CMDBField<TBool>	                iIpDnsAddrFromServer;			//< Specifies whether to get the DNS address (IPV4) from the ISP.
		CMDBField<TDesC>	                iIpNameServer1;					//< Static address of the Primary IPV4 DNS server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.	
		CMDBField<TDesC>	                iIpNameServer2;					//< Static address of the Secondary IPV4 DNS server (if required). May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TBool>	                iIp6DnsAddrFromServer;			//< Specifies whether to get the DNS address (IPV6) from the ISP.
		CMDBField<TDesC>	                iIp6NameServer1;				//< Static address of the Primary IPV6 name server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.
		CMDBField<TDesC>	                iIp6NameServer2;				//< Static address of the Secondary IPV6 name server if required. May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TDesC>	                iIpAddrLeaseValidFrom;			//< Start of the address lease for dynamic address allocation by the ISP (e.g. a DHCP assigned IP address). Start time information stating when the IP address was leased out. The format is as follows:  dd/mm/yyyy hr:mins AM/PM, e.g. 04/08/2006 6:21 PM.
		CMDBField<TDesC>	                iIpAddrLeaseValidTo;			//< End of the address lease for dynamic address allocation by the ISP (.e.g. a DHCP assigned IP address). End time information stating when the IP address lease finishes. The format is as follows:  dd/mm/yyyy hr:mins AM/PM, e.g. 08/08/2006 6:21 PM.
		CMDBField<TDesC>	                iConfigDaemonManagerName;		//< Name of the ECOM daemon manager plug-in for NIFMAN used to load a specific configuration daemon.
		CMDBField<TDesC>	                iConfigDaemonName;				//< Name of the daemon used for address configuration. 
		CMDBField<TDesC>	                iServiceExtensionTableName;		//< Name of the extension table.
		CMDBRecordLink<CCDServiceExtRecordBase>     iServiceExtensionTableRecordId;
	
    private:
	
		static const SRecordTypeInfo* const iRecordInfo;
	};


//
// 0C/  VPN SERVICE RECORD
//
/**
@publishedAll
@released
*/
class CCDVPNServiceRecord : public CCDServiceRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDVPNServiceRecord();
		/**
		@internalComponent
		*/
		CCDVPNServiceRecord(TMDBElementId aElementId);

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
	public:

		// Member Data
		CMDBField<TDesC>	                iServicePolicy;	///< Policy id of the policy file- this is a reference to the policy file.
		CMDBRecordLink<CCDIAPRecord>	    iServiceIAP;	///< Record id of the real IAP used by the IPSEC software to communicate with the VPN gateway.
		CMDBRecordLink<CCDAccessPointRecord>                  iServiceSNAP;       ///< Record id of the SNAP id used by the IPSEC software to communicate with the VPN gateway.
		CMDBRecordLink<CCDNetworkRecord>	iServiceNetwork;///< Record id of the real Network to be connected to by the virtual tunnel.

    private:
    
    	static const SRecordTypeInfo* const iRecordInfo;
	};


//
// 10/  WCDMA Packet Service Record
//
class CCDUmtsR99QoSAndOnTableRecord;
/**
@publishedAll
@released
*/

class CCDWCDMAPacketServiceRecord : public CCDServiceRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDWCDMAPacketServiceRecord();
		/**
		@internalComponent
		*/
		CCDWCDMAPacketServiceRecord(TMDBElementId aElementId);

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


	public:

		// Member Data
		CMDBField<TDesC>	                        iGPRSAPN;				//< When the mobile phone sets up a PDP context, the access point is chosen by specifying an APN.
		CMDBField<TUint32>	                iGPRSPDPType;					//< PDP is a network protocol which is used by packet switching networks to communicate with GPRS networks. IPV6 is an example of a PDP type supported by GPRS.
		CMDBField<TDesC>	                iGPRSPDPAddress;				//< PDP address of the phone.
		CMDBField<TUint32>	            iGPRSReqPrecedence;					//< Requested quality of service precedence class. This field has been deprecated and may be removed in the future. 
		CMDBField<TUint32>	                iGPRSReqDelay;					//< Requested quality of service delay class. This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	            iGPRSReqReliability;				//< Requested quality of service reliability class. This field has been deprecated amnd may be removed in the future.
		CMDBField<TUint32>	        iGPRSReqPeakThroughput;					//< Requested quality of service peak throughput. This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	        iGPRSReqMeanThroughput;					//< Requested quality of service mean throughput class. This field has been deprecated and may be removed in the future. 
		CMDBField<TUint32>	            iGPRSMinPrecedence;					//< Minimum quality of service precedence class. This field has been deprecated and may be removed in the future. 
		CMDBField<TUint32>	                iGPRSMinDelay;					//< Minimum quality of service delay class This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	            iGPRSMinReliability;				//< Minimum quality of service reliability class. This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	        iGPRSMinPeakThroughput;					//< Minimum quality of service peak throughput class. This field has been deprecated and may be removed in the future.
		CMDBField<TUint32>	        iGPRSMinMeanThroughput;					//< Minimum quality of service mean throughput class. This field has been deprecated and may be removed in the future.
		CMDBField<TBool>	        iGPRSDataCompression;					//< Specifies whether to compress data.
		CMDBField<TBool>	        iGPRSHeaderCompression;					//< Specifies whether IP header compression is on.
		CMDBField<TBool>	                iGPRSUseEdge;					//< Specifies whether to use EDGE technology. 
		CMDBField<TBool>	        iGPRSAnonymousAccess;					//< This tells the MS (Mobile Station) whether to try anonymous access or not. Anonymous access allows the mobile device to connect to a network without having to authenticate identity. To access the network, the MS (Mobile Station) will just use a random identity.
		CMDBField<TDesC>	                iGPRSIfParams;					//< Interface parameter string-used to specify the lower layer that the interface protocol is to use.
		CMDBField<TDesC>	                iGPRSIfNetworks;				//< Comma separated list of network protocols to be used by the NIF.
		CMDBField<TBool>	        iGPRSIfPromptForAuth;					//< Specifies whether or not to request an authentication username and password. This depends on whether or not the user wishes to connect to the ISP's NIF(s).
		CMDBField<TDesC>	                iGPRSIfAuthName;				//< Prompt user to enter authentication username. Used when a user wishes to connect to the ISP's NIF(s). E.g. PPP.
		CMDBField<TDesC>	                iGPRSIfAuthPass; 				//< Prompt user to enter authentication password used by an interface protocol such as PPP. Used when a user wishes to connect to the ISP's NIF(s). E.g. PPP.
		CMDBField<TUint32>	            iGPRSIfAuthRetries;					//< Number of times to retry authentication if it fails.
		CMDBField<TDesC>	                iGPRSIPNetMask;					//< IP netmask of the NIF.
		CMDBField<TDesC>	                iGPRSIPGateway;					//< IP address of the Gateway.
		CMDBField<TBool>  	        iGPRSIPAddrFromServer;					//<  Specifies whether to get the IP address from the ISP.
		CMDBField<TDesC>	                    iGPRSIPAddr;				//< Static IP Address (if required). This is the IP address of a NIF. This is used if the address is not dynamically allocated by the ISP.
		CMDBField<TBool>	    iGPRSIPDNSAddrFromServer;					//< Specifies whether to get the DNS address (IPV4) from the ISP.
		CMDBField<TDesC>	            iGPRSIPNameServer1;					//< Static address of the Primary IPV4 DNS server if required. This is used if the address of the name server is not dynamically allocated by the ISP. May be set to 0.0.0.0 to disable assigning the primary DNS address.
		CMDBField<TDesC>	            iGPRSIPNameServer2;					//< Static address of the Secondary IPV4 DNS server (if required). May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TBool>	    iGPRSIP6DNSAddrFromServer;					//< Specifies whether to get the DNS address (IPV6) from the ISP.
		CMDBField<TDesC>	            iGPRSIP6NameServer1;				//< Static address of the Secondary IPV6 name server if required. May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TDesC>	            iGPRSIP6NameServer2;				//< Static address of the Secondary IPV6 name server if required. May be set to 0.0.0.0 to disable assigning the secondary DNS address.
		CMDBField<TDesC>	    iGPRSIPAddrLeaseValidFrom;					//< Start of the address lease for dynamic address allocation by the ISP (.e.g. a DHCP assigned IP address). Start time information stating when the IP address was leased out. The format is as follows:  dd/mm/yyyy hr:mins AM/PM, e.g. 04/08/2006 6:21 PM.
		CMDBField<TDesC>	        iGPRSIPAddrLeaseValidTo;				//< End of the address lease for dynamic address allocation by the ISP (.e.g. a DHCP assigned IP address). End time information stating when the IP address lease finishes. The format is as follows:  dd/mm/yyyy hr:mins AM/PM, e.g. 08/08/2006 6:21 PM.
		CMDBField<TDesC>	iGPRSConfigDaemonManagerName;					//< Name of the ECOM daemon manager plug-in for NIFMAN used to load a specific configuration daemon.
		CMDBField<TDesC>	        iGPRSConfigDaemonName;					//< Name of the configuration daemon server. This server is used to provide further configuration for a connection, e.g. dynamic IP address assignment.
		CMDBField<TBool>	        iGPRSEnableLCPExtension;				//< Specifies whether to enable LCP extension protocol. 
		CMDBField<TBool>	    iGPRSDisablePlainTextAuth;					//< Specifies whether to disable plain text authentication. If disabled, PAP cannot be used- CHAP will be used instead.
		CMDBField<TUint32>            	        iGPRSAPType;				//< Service supports Internet only, WAP only or both. Values defined in TCommsDbIspType. 
		CMDBField<TUint32>	        iGPRSQOSWarningTimeOut;					//< If the requested QOS can not be satisfied, warn the user after this time in microseconds. Set to 0xffffffff to disable.
		CMDBRecordLink<CCDUmtsR99QoSAndOnTableRecord>	 iUmtsR99QoSAndOnTable; //< A record link between the Incoming/Outgoing GPRS table and the UmtsR99QosAndOn table
		CMDBField<TUint32> 			iGPRSR5DataCompression;
		CMDBField<TUint32> 			iGPRSR5HeaderCompression;
		CMDBField<TUint32> 			iGPRSPacketFlowIdentifier;
		CMDBField<TUint32> 			iGPRSUmtsGprsRelease;

    public:

        static const SRecordTypeInfo* const iRecordInfo;
    };

/**
@publishedAll
@released
*/
class CCDOutgoingGprsRecord : public CCDWCDMAPacketServiceRecord
	{
	public:
		/**
		@internalComponent
		*/
		CCDOutgoingGprsRecord(TMDBElementId aElementId);

		DATA_VTABLE
	};

/**
@publishedAll
@released
*/
class CCDIncomingGprsRecord : public CCDWCDMAPacketServiceRecord
	{
	public:
		/**
		@internalComponent
		*/
		CCDIncomingGprsRecord(TMDBElementId aElementId);

		DATA_VTABLE
	};

/**
@publishedAll
@released
*/
class CCDUmtsR99QoSAndOnTableRecord: public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDUmtsR99QoSAndOnTableRecord();
		/**
		@internalComponent
		*/
		CCDUmtsR99QoSAndOnTableRecord(TMDBElementId aElementId);

		DATA_VTABLE

		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:
		CMDBField<RPacketQoS::TTrafficClass> 	iGPRSReqTrafficClass;              	//< Requested traffic class.    
		CMDBField<RPacketQoS::TTrafficClass> 	iGPRSMinTrafficClass;             	//< Minimum acceptable traffic class.
		CMDBField<RPacketQoS::TDeliveryOrder> 	iGPRSReqDeliveryOrder;      	   	//< Requested value for sequential SDU delivery.
		CMDBField<RPacketQoS::TDeliveryOrder> 	iGPRSMinDeliveryOrder;      	   	//< Minimum acceptable value for sequential SDU delivery.
		CMDBField<RPacketQoS::TErroneousSDUDelivery> 	iGPRSReqDeliverErroneousSDU;//< Requested value for erroneous SDU delivery.
		CMDBField<RPacketQoS::TErroneousSDUDelivery> 	iGPRSMinDeliverErroneousSDU;//< Minimum acceptable value for erroneous SDU delivery.
		CMDBField<TInt32> 	                  iGPRSReqMaxSDUSize;             		//< Request maximum SDU size.
		CMDBField<TInt32>                   	iGPRSMinAcceptableMaxSDUSize;	   	//< Minimum acceptable SDU size.
		CMDBField<TInt32>                   	iGPRSReqMaxUplinkRate;              //< Requested maximum bit rates on uplink. 
		CMDBField<TInt32>                   	iGPRSReqMinUplinkRate;              //< Requested minimum bit rates on uplink. 
		CMDBField<TInt32>                   	iGPRSReqMaxDownlinkRate;            //< Requested maximum bit rates on downlink. 
		CMDBField<TInt32>                   	iGPRSReqMinDownlinkRate;            //< Requested minimum bit rates on downlink. 
		CMDBField<RPacketQoS::TBitErrorRatio> 	iGPRSReqBER;                     	//< Requested target BER.
		CMDBField<RPacketQoS::TBitErrorRatio> 	iGPRSMaxBER;                      	//< Maximum acceptable target BER.
		CMDBField<RPacketQoS::TSDUErrorRatio> 	iGPRSReqSDUErrorRatio;           	//< Requested target SDU error ratio.
		CMDBField<RPacketQoS::TSDUErrorRatio> 	iGPRSMaxSDUErrorRatio;          	//< Maximum acceptable target SDU error ratio.
		CMDBField<RPacketQoS::TTrafficHandlingPriority> 	iGPRSReqTrafficHandlingPriority;	//< Requested traffic handling priority.
		CMDBField<RPacketQoS::TTrafficHandlingPriority> 	iGPRSMinTrafficHandlingPriority;	//< Minimum acceptable traffic handling priority.
		CMDBField<TInt32>                   	iGPRSReqTransferDelay;            	//< Requested transfer delay (in milliseconds).
		CMDBField<TInt32>                   	iGPRSMaxTransferDelay;           	//< Maximum acceptable  transfer delay (in milliseconds).
		CMDBField<TInt32>                   	iGPRSReqGuaranteedUplinkRate;       //< Requested guaranteed bit rates on uplink. 
		CMDBField<TInt32>                   	iGPRSMinGuaranteedUplinkRate;      	//< Minimum acceptable guaranteed bit rates on uplink.
		CMDBField<TInt32>                   	iGPRSReqGuaranteedDownlinkRate;  	//< Requested guaranteed bit rates on downlink.
		CMDBField<TInt32>                   	iGPRSMinGuaranteedDownlinkRate;	    //< Minimum acceptable guaranteed bit rates on downlink.
		CMDBField<TBool>                    	iGPRSSignallingIndication;		   	//< Signalling indication.
		CMDBField<TBool>                    	iGPRS_ImCnSignallingIndication;     //< IP Multimeida System (IMS) Core Network (CN) Signalling Indicator.
		CMDBField<RPacketQoS::TSourceStatisticsDescriptor> 	iGPRSSourceStatisticsDescriptor; //< Static source descriptor.		
	
	public:	
		static const SRecordTypeInfo* const iRecordInfo;	
	};
	
// TCommdbBearer for BearerTechnology field 
typedef TCommDbBearer TCommsDBBearerTechnology;	//< Bearer support type for field BearerTechnology

//
// 13/  MODEM BEARER RECORD
//
/**
@publishedAll
@released
*/
class CCDModemBearerRecord : public CCDBearerRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDModemBearerRecord();
		/**
		@internalComponent
		*/
		CCDModemBearerRecord(TMDBElementId id);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}			

		DATA_VTABLE
		

	public:

		// MemberData
		CMDBField<TDesC>	                        iNifName;				//< A protocol name is specified for establishing a direct connection using the modem bearer.  The interface typically implements a particular network connection protocol such as PPP.
		CMDBField<TDesC>	                        iPortName;				//< The name of the Comm port for the modem to connect to
		CMDBField<TDesC>	                        iTsyName;				//< The name of the TSY. ETEL is the telephony server/common interface for Symbian. The TSY provides the hardware specific implementation to this.
		CMDBField<TDesC>	                        iCsyName;				//< The name of the CSY. CSY provides the Comm port specific implementation to C32 (Serial Comms server).
		CMDBField<TUint32>	    iLastSocketActivityTimeout;					//< Time (in seconds) to stay online when all socket activity has ceased.
		CMDBField<TUint32>	        iLastSessionClosedTimeout;				//< Time (in seconds)  to stay online when session has closed
		CMDBField<TUint32>	        iLastSocketClosedTimeout;				//< Time (in seconds) to stay online when socket has closed.
		CMDBField<TUint32>	                        iDataBits;				//< Number of data bits used for serial port configuration.
		CMDBField<TUint32>	                        iStopBits;				//< Number of stop bits used for serial port configuration. This marks the end of a unit of transmission, e.g. the end of a byte of data.
		CMDBField<TUint32>	                        iParity;				//< The type of parity checking to be used when data is transmitted over the bearer. This involves sending an extra bit known as the parity bit and can be used to detect corrupted data.
		CMDBField<TUint32>	                            iRate;				//< The baud rate of the modem. Measured in bits per second (bps). This specifies the maximum rate that data can be sent on this modem bearer.
		CMDBField<TUint32>	                    iHandshaking;				//< The handshaking process between modems is encapsulated into one integer. This integer can be viewed as a binary bitmask of bits, each bit representing one of the pins inside the RS232 serial cable. The integer value can actually be derived from taking a binary OR of each of the bits.  From this integer, Symbian OS is able to check whether individual pins have been set or not for the handshaking process. 
		CMDBField<TUint32>	                    iSpecialRate;				//< User defined baud rate for the modem. This field will contain a value only if the value of field Rate is EBpsSpecial.
		CMDBField<TUint32>	                        iXonChar;				//< Character used to signal to the transmitter to resume sending when using XON/XOFF handshaking.
		CMDBField<TUint32>	                        iXoffChar;				//< Character used to signal the transmitter to suspend sending when using XON/XOFF handshaking.
		CMDBField<TUint32>	                    iFaxClassPref;				//< The preferred fax modem class supported by the fax client. This is the command set used to control the modem.
		CMDBField<TUint32>	                    iSpeakerPref;				//< Preferred speaker mode.
		CMDBField<TUint32>	                iSpeakerVolPref;				//< Preferred speaker volume.
		CMDBField<TDesC8>	                    iModemInitString;				//< General modem initialisation string. It is used to initialise (init) the modem by sending it a series (string) of commands, typically Hayes commands. These commands configure the modem's options for things like error correction, data compression, flow control, and many other parameters. 
		CMDBField<TDesC8>	                iDataInitString;				//< Data initialisation string.
		CMDBField<TDesC8>	                iFaxInitString;					//< Fax initialisation string.
		CMDBField<TDesC8>	                iIspInitString;					//< Initialisation string specific to a particular ISP.
		CMDBField<TDesC>	                iDialPauseLength;				//< Command to modify the pause created during dialling using the comma character (',').
		CMDBField<TDesC>	                iCarrierTimeOut;				//< Command to set the time out the modem uses when establishing a link before giving up and returning to command mode.
		CMDBField<TDesC>	            iAutoAnswerRingCount;				//< Command to set the number of rings before the modem auto answers.
		CMDBField<TDesC>	            iSpeakerVolControlLow;				//< Command to set the modem speaker volume to low.
		CMDBField<TDesC>	        iSpeakerVolControlMedium;				//< Command to set the modem speaker volume to medium.
		CMDBField<TDesC>	        iSpeakerVolControlHigh;					//< Command to set the modem speaker volume to high.
		CMDBField<TDesC>	                iSpeakerAlwaysOff;				//< Command to set the modem speaker off.
		CMDBField<TDesC>	        iSpeakerOnUntilCarrier;					//< Command to set the modem speaker on.
		CMDBField<TDesC>	                iSpeakerAlwaysOn;				//< Command to set the modem speaker on until the carrier.
		CMDBField<TDesC>	        iSpeakerOnAfterUntilCarrier;			//< Command to set the modem speaker on except during dialling.
		CMDBField<TDesC>	            iDialToneWaitModifier;				//< The dial command modifier waits for dial tone.
		CMDBField<TDesC>	                iCallProgress1;					//< Disable busy and dial tone detection.
		CMDBField<TDesC>	                iCallProgress2;					//< Dial tone detection enabled, busy detection disabled.
		CMDBField<TDesC>	                iCallProgress3;					//< Dial tone detection disabled, busy detection enabled.
		CMDBField<TDesC>	                iCallProgress4;					//< Dial tone and busy detection enabled.
		CMDBField<TDesC>	                        iEchoOff;				//< Switch echo mode off.
		CMDBField<TDesC>	                    iVerboseText;				//< Switch verbose mode on.
		CMDBField<TDesC>	                        iQuietOff;				//< Switch quiet mode off.
		CMDBField<TDesC>	                        iQuietOn;				//< Switch quiet mode on.
		CMDBField<TDesC>	        iDialCommandStateModifier;				//< Dial command modifier- used to return to command mode after dialling.
		CMDBField<TDesC>	                        iOnLine;				//< Enter on-line mode from on-line command mode.
		CMDBField<TDesC>	            iResetConfiguration;				//< Reset the modem configurations.
		CMDBField<TDesC>	            iReturnToFactoryDefs;				//< Return the modem configuration to its factory defaults.
		CMDBField<TDesC>	                iDcdOnDuringLink;				//< Command the modem to only assert DCD when a carrier is actually detected, i.e. while the link is up.
		CMDBField<TDesC>	                    iDtrHangUp;					//< Command the modem to hang up the current call when the DTE drops the DTR line.
		CMDBField<TDesC>	                    iDsrAlwaysOn;				//< Command the modem to always assert DSR.
		CMDBField<TDesC>	                iRtsCtsHandshake;				//< Command the modem to use RTS/CTS flow control
		CMDBField<TDesC>	                iXonXoffHandshake;				//< Command the modem to use software flow control.
		CMDBField<TDesC>	                iEscapeCharacter;				//< The character used by the DTE to return to command mode from on-line mode.
		CMDBField<TDesC>	            iEscapeGuardPeriod;					//< Command the modem to use a particular escape sequence guard period.
		CMDBField<TDesC>	            iFaxClassInterrogate;				//< Command asking the modem which fax modes are supported.
		CMDBField<TDesC>	                        iFaxClass;				//< Command that sets the fax mode.
		CMDBField<TDesC>	                    iNoDialTone;				//< Modem response when no dial tone is detected.
		CMDBField<TDesC>	                            iBusy;				//< Modem response when a busy tone is detected.
		CMDBField<TDesC>	                        iNoAnswer;				//< Modem response when no answer is detected.
		CMDBField<TDesC>	                        iCarrier;				//< Carrier report message.
		CMDBField<TDesC>	                        iConnect;				//< Connection report message.
		CMDBField<TDesC>	            iCompressionClass5;					//< Compression Class 5 report message.
		CMDBField<TDesC>	            iCompressionV42bis;					//< Compression V.42 bis report message.
		CMDBField<TDesC>	                iCompressionNone;				//< No compression report message.
		CMDBField<TDesC>	                    iProtocolLapd;				//< LAPD protocol report message.
		CMDBField<TDesC>	                    iProtocolAlt;				//< ALT protocol report message.
		CMDBField<TDesC>	            iProtocolAltcellular;				//< ALT-CELLULAR report message.
		CMDBField<TDesC>	                    iProtocolNone;				//< No protocol report message.
		CMDBField<TDesC>	            iMessageCentreNumber;				//< Phone number of message centre.
		CMDBField<TUint32>	        iMessageValidityPeriod;					//< Validity period for SMS in minutes.
		CMDBField<TBool>	        iMessageDeliveryReport;					//< Specifies whether to produce an SMS delivery report.
		CMDBField<TUint32>	                iMinSignalLevel;				//< Minimum signal strength of the modem. This is the minimum signal level required for a connection to succeed. The connection will fail if below this level. Setting the column to NULL will turn off this functionality. The value must be in dBm with an offset of 10000 added to it. I.e. Value=dBm+10000.

		CMDBField<TUint32>	                        iCommRole;				//< Specifies whether comm port should be opened in the role of DTE or DCE. The content of this field is a bit mask. The value of this is ANDed with KModemCommRoleDCE. For example, say the value of CommRole is 1 then the role set is DCE, else the role set is DTE. (see KModemCommRoleDCE).
		CMDBField<TDesC>	        iControlChannelPortName;				//< The modem control channel port name.
		CMDBField<TUint32>	                    iSirSettings;			    //< This field sets the serial port's infra-red settings. This enables the range of infrared to be set, to modify the pulse width or even to completely shutdown serial infrared. Please refer to Comms Dat Ref Final documentation to see the list of values this field 'SirSettings' can take. 
		CMDBField<TDesC>	                    iBcaStack;					//< A list of BCAs (Baseband Channel Adapter). The BCA provides an interface for the hardware adaptation layer (HAL). More specifically, it is used to interface the R-Interface data plane transport with a NIF.
		CMDBField<TCommsDBBearerTechnology>     iBearerTechnology;			//< Specifies whether the bearer is a CSD/HSCSD bearer.
    
    private:
    
		static const SRecordTypeInfo* const iRecordInfo;
	};





//
// 14/  LAN BEARER RECORD
//
/**
@publishedAll
@released
*/
class CCDLANBearerRecord : public CCDBearerRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDLANBearerRecord();
		/**
		@internalComponent
		*/
		CCDLANBearerRecord(TMDBElementId aId);
		
			/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}			

		DATA_VTABLE        

	public:

    	CMDBField<TDesC>	        iLanBearerNifName;						//< A protocol name is specified for establishing a direct connection. The interface typically implements a particular network connection protocol such as PPP.
		CMDBField<TDesC>    	    iLanBearerLddFilename;					//< This is the actual .DLL filename for the LDD.
		CMDBField<TDesC>	        iLanBearerLddName;						//< This is the name of a particular LDD.
		CMDBField<TDesC>	        iLanBearerPddFilename;					//< This is the actual .DLL filename for the PDD.
   	    CMDBField<TDesC>          iLanBearerPddName;						//< This is the name of a particular PDD.
		CMDBField<TDesC>	        iLanBearerPacketDriverName;				//< The name of the packet driver. The packet driver can do the following: initiate access to a specific packet type, end access to a specific packet type, send a packet, get statistics on the NIF, and get information about the NIF.
		CMDBField<TUint32>        iLastSocketActivityTimeout;				//< Time (in seconds) to stay online when all socket activity has ceased.
		CMDBField<TUint32>        iLastSessionClosedTimeout;				//< Time (in seconds)  to stay online when session has closed.
		CMDBField<TUint32>	            iLastSocketClosedTimeout;			//< Time (in seconds)  to stay online when socket has closed.
		CMDBField<TCommsDBBearerTechnology>   iBearerTechnology;			//< Specifies whether the bearer is a CSD/HSCSD bearer.

    private:

		static const SRecordTypeInfo* const iRecordInfo;
	};


//
// 15/  VIRTUAL BEARER RECORD
//
/**
@publishedAll
@released
*/
class CCDVirtualBearerRecord : public CCDBearerRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDVirtualBearerRecord();
		/**
		@internalComponent
		*/
		CCDVirtualBearerRecord(TMDBElementId aId);

		DATA_VTABLE

		/** 
		Gets the following information: 
		- iTypeId
		- iValType 
		- iTypeAttr
		- iTypeName           
		
		@return Returns a const pointer to the record containing the data - client does not need to delete this pointer after use.
		*/
        const SRecordTypeInfo* GetRecordInfo() {return iRecordInfo;}			
	public:

		// Member Data
		CMDBField<TDesC>	    iVirtualBearerNifName;						//< The network interface name of the virtual bearer. 
		CMDBField<TUint32>	    iLastSocketActivityTimeout;					//< Time (in seconds) to stay online when all socket activity has ceased.
		CMDBField<TUint32>	    iLastSessionClosedTimeout;					//< Time (in seconds)  to stay online when socket has closed.
		CMDBField<TUint32>	    iLastSocketClosedTimeout;					//< Time (in seconds)  to stay online when socket has closed.
		CMDBField<TCommsDBBearerTechnology>     iBearerTechnology;			//< Specifies whether the bearer is a CSD/HSCSD bearer.

    private:
    
		static const SRecordTypeInfo* const iRecordInfo;
	};



//
// 16/ WAP SMS Bearer Record
//
/**
@publishedAll
@released
*/
class CCDWAPSMSBearerRecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDWAPSMSBearerRecord();
		/**
		@internalComponent
		*/
		CCDWAPSMSBearerRecord(TMDBElementId aElementId);

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
		CMDBRecordLink<CCDWAPAccessPointRecord>   iWAPAccessPointId;		//< ID of the  WAP Access Point record to which this bearer information refers.	
		CMDBField<TDesC>              		      iWAPGatewayAddress;		//< WAP gateway address: an IP address or phone number.
		CMDBField<TDesC>                          iWAPServiceCentreAddress; //< WAP service centre address.
		CMDBField<TUint8>                         iWAPWSPOption;			//< Whether connection-oriented or connectionless API should be used.
		CMDBField<TBool>                          iWAPSecurity;				//< Attempt secure WTLS connection to the gateway.

    private:

		static const SRecordTypeInfo* const iRecordInfo;
	};



//
// 17/ WAP IP BEARER
//
/**
@publishedAll
@released
*/
class CCDWAPIPBearerRecord : public CCDRecordBase
	{

	public:
		/**
		@internalComponent
		*/
		CCDWAPIPBearerRecord();
		/**
		@internalComponent
		*/
		CCDWAPIPBearerRecord(TMDBElementId aElementId);

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
		CMDBRecordLink<CCDWAPAccessPointRecord> iWAPAccessPointId;				//< ID of the  WAP Access Point record to which this bearer information refers.
		CMDBField<TDesC>                        iWAPGatewayAddress;				//< WAP gateway address: an IP address or phone number.
		CMDBRecordLink<CCDIAPRecord>            iWAPIAP;						//< Identifier of a record in the IAP table to be used.
		CMDBField<TUint8>                       iWAPWSPOption;					//< Set to an enum deciding whether WAP WSP is to be set to connectionless or connection oriented. 
		CMDBField<TBool>                        iWAPSecurity;					//< Attempt secure WTLS connection to the gateway.
		CMDBField<TUint32>                      iWAPProxyPort;					//< Proxy port number. Required for WAP2.0 only.
		CMDBField<TDesC>                        iWAPProxyLoginName;				//< Proxy login name. Required for WAP2.0 only.
		CMDBField<TDesC>                        iWAPProxyLoginPass;				//< Proxy login password. Required for WAP2.0 only.


    private:
	
		static const SRecordTypeInfo* const iRecordInfo;
	};




//
// 18/  CHARGECARD RECORD
//
/**
Reading of ChargeCard Table data is protected with ECDPrivate Attribute.  
This will require additional capabilities to read.

@publishedAll
@released
*/
class CCDChargecardRecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDChargecardRecord();
		/**
		@internalComponent
		*/
		CCDChargecardRecord(TMDBElementId aElementId);

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

	public:

		// Member Elements
		CMDBField<TDesC>        iAccountNo;		//< The account number to charge.
		CMDBField<TDesC>              iPin;		//< The pin number of the account to charge.
		CMDBField<TDesC>        iLocalRule;		//< An operator dependant rule specifying the order of dialling to be account number, PIN and phone number for local calls.
		CMDBField<TDesC>          iNatRule;		//< An operator dependant rule specifying the order of dialling to be account number, PIN and phone number for national calls.
		CMDBField<TDesC>         iIntlRule;		//< An operator dependant rule specifying the order of dialling to be account number, PIN and phone number for international calls.

    private:

		static const SRecordTypeInfo* const iRecordInfo;

	};

//
// 19/  PROXIES RECORD
//
/**
@publishedAll
@released
*/
class CCDProxiesRecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDProxiesRecord();
		/**
		@internalComponent
		*/
		CCDProxiesRecord(TMDBElementId aElementId);

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
		CMDBRecordLink<CCDServiceRecordBase>    iService;				//< ISP with which these proxies are associated. This field takes as its value the identifier of the record from the servicing table defined in field ProxyServiceType. For example, ProxyServiceType = DialOutISP, ISP = 1, this means the first record from table DialOutISP is used.
		CMDBField<TDesC>                        iServiceType;			//< The servicing table to which the ISP field refers. This is necessary as GPRS records as well as ISP records can have proxy settings associated with them. If this field is read and its length is zero, the value "DialOutISP" is returned, as it is assumed that any client that has not written this field is assuming that proxies are only available to the dial out ISP and not other service types.
		CMDBField<TBool>                        iUseProxyServer;		//< Specifies whether to use a proxy server.
		CMDBField<TDesC>                        iServerName;			//< Name of the host proxy server.
		CMDBField<TDesC>                        iProtocolName;			//< Name of the protocol for which this proxy can be used.
		CMDBField<TUint32>                      iPortNumber;			//< Port number for the proxy server.
		CMDBField<TDesC>                        iExceptions;			//< Semi-colon separated list of the addresses for which the proxy server should not be used.


    private:
    
        static const SRecordTypeInfo* const iRecordInfo;
	};



//
// 1C/ PAN_SERVICE_EXTENSIONS
//
/**
@publishedAll
@released
*/
class CCDPANServiceExtRecord : public CCDServiceRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDPANServiceExtRecord();
		/**
		@internalComponent
		*/
		CCDPANServiceExtRecord(TMDBElementId aElementId);

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

	public:

		// Member Data
		CMDBField<TUint32>  	iLocalRole;					//< PAN role that the local device will act in. To dynamically select a role, use ECommDbPanRoleUnknown.
		CMDBField<TUint32>	 	iPeerRole;					//< PAN role that the remote device will act in. To dynamically select a role, use ECommDbPanRoleUnknown.
		CMDBField<TDesC>  		iPeerMACAddresses;			//< A list of Bluetooth MAC addresses.  If one address is specified, then PAN agent will connect to it.  If more than one device address is specified (in a comma seperated list), then PAN agent will discover whether any of the devices are in range, and connect to one of them. Comma-separated list of Bluetooth MAC addresses, with no "0x" preamble and no colons separating the digits
		CMDBField<TBool>	 	iPromptForRemoteDevices;	//< Whether the agent should prompt the user to select the remote device to connect to.
		CMDBField<TBool>  		iDisableSdpQuery;			//< Whether the remote role defined above should be accepted without performing an SDP query to ensure the remote device supports the specified role.  Not recommended for use.
		CMDBField<TBool>	 	iAllowIncoming;				//< Whether the PAN agent should support incoming connections.
		CMDBField<TBool>	 	iPromptIfMACListFails;		//< Whether the agent should prompt the user to select the remote device to be connected to if we cannot connect to any device specified in the MAC list.
#ifdef SYMBIAN_NETWORKING_DHCPSERVER		
		CMDBField<TBool>	 	iNapServiceEnabled;
#endif // SYMBIAN_NETWORKING_DHCPSERVER		
    private:

        static const SRecordTypeInfo* const iRecordInfo;
	};

class CCDSelectionPolicyRecordBase;

//
// 1D/ ACCESS POINT RECORD
// -----------   ------------------------------------------------------------------------------------------------------------------------------
//| ID | NAME | | ACCESS POINT TYPE | MCPR TYPE | SELECTION POLICY | CPR TYPE | CPR CONFIG | SCPR TYPE | PROTOCOL TYPE | APPLICATION SECURE ID |
//|-----------| |------------------------------------------------------------------------------------------------------------------------------|
//|  * |    * | |                 * |         * |                * |        * |          * |         * |             * |                     * |
// -----------   ------------------------------------------------------------------------------------------------------------------------------
// ID                     - Access point's unique ID.
// NAME                   - Access point's unique name.
// ACCESS POINT TYPE      - Identifies the access point's (tier's) type (row in the CCDAccessPointTypeRecord table).
// MCPR TYPE
// SELECTION POLICY       - A policy id interpreted by the MCPR's "Next Layer Selector".
// CPR TYPE
// CPR CONFIG             - CPR's config id interpreted by a CPR.
// SCPR TYPE
// PROTOCOL TYPE
// APPLICATION SECURE ID  - Only application with this SID may acess this record (can we do that in CommsDat?)

/**
@publishedAll
@released
*/
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class CCDTierRecord;
class CCDMCprRecord;
class CCDCprRecord;
class CCDSCprRecord;
class CCDProtocolRecord;
#endif


class CCDAccessPointRecord : public CCDRecordBase
	{
	public:
    	enum { KNoPolicy = -1 };
	
		CCDAccessPointRecord(TMDBElementId aElementId);

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

	public:
        /* @deprecated with SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
           Use iTier instead */
		CMDBField<TUint32>                              iAccessPointGID;    // Only present when record is a SNAP.  Gives to the SNAP Tier Id
        
		/* mapped (but not deprecated) with SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY*/
        CMDBRecordLink<CCDSelectionPolicyRecordBase>    iSelectionPolicy;   // Link to SelectionPolicy record
        
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

		CMDBRecordLink<CCDTierRecord> iTier;		//Tier. Type of the access point. Row in CCDTierRecord table.
		CMDBRecordLink<CCDMCprRecord> iMCpr;		//MCPR's type. Row in CCDMCprRecord table.
		CMDBRecordLink<CCDCprRecord> iCpr;			//CPR's type. Row in CCDCprRecord table.
		CMDBRecordLink<CCDSCprRecord> iSCpr;		//SCPR's type. Row in CCDSCprRecord table.
		CMDBRecordLink<CCDProtocolRecord> iProtocol;	//PRT's type. Row in CCDProtocolRecord table.
		CMDBField<TUint32> iCprConfig;				//CPR's config id. Value interpreted by a CPR.
		CMDBField<TUint32> iAppSID;					//Application SID.
		CMDBField<TDesC> iConfigAPIdList;                   // List of addition SCprs (from ConfigAccessPoint table) 
        //This field can be used to create custom selection policies which is _not_ use
        //the APPrioritySelectionPolicy record but store some value which is interpreted by the
        //given MCPr.
        CMDBField<TInt> iCustomSelectionPolicy;
		CMDBField<TUint32> iPriority;			//Access point priority.
#endif
    private:
        static const SRecordTypeInfo* const iRecordInfo;
	};

//
// 1E/ SELECTION POLICY RECORD
/**
@publishedAll
@released
*/
class CCDSelectionPolicyRecordBase : public CCDRecordBase
	{
	public:
		DATA_VTABLE

    protected:
		/**
		@internalComponent
		*/
		CCDSelectionPolicyRecordBase(TMDBElementId aElementId);
	};

// 1F/ IAP PRIORITY SELECTION POLICY RECORD
// -----------   ---------------------------------------------
//| ID | NAME | | IAP LINK 1 | IAP LINK 2 | ... | IAP LINK 15 |
//|-----------| |---------------------------------------------|
//|  * |    * | |          * |          * |   * |           * |
// -----------   ---------------------------------------------
// IAP LINK N - links to Nth IAP record.
/**
 @publishedAll
 @released
 @deprecated with SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 when Mapped to CCDAPPrioritySelectionPolicyRecord
 */
class CCDIAPPrioritySelectionPolicyRecord : public CCDSelectionPolicyRecordBase
	{
    public:
		enum { EMaxNrOfIaps = 15 };

    public:
		/**
		@internalComponent
		*/
		CCDIAPPrioritySelectionPolicyRecord(TMDBElementId aElementId);

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

	public:
		CMDBRecordLink<CCDIAPRecord> iIap1; 	//< IAP link 1 - links to first IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap2;		//< IAP link 2 - links to second IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap3;		//< IAP link 3 - links to third IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap4;		//< IAP link 4 - links to fourth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap5;		//< IAP link 5 - links to fifth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap6;		//< IAP link 6 - links to sixth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap7;		//< IAP link 7 - links to seventh IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap8;		//< IAP link 8 - links to eighth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap9;		//< IAP link 9 - links to ninth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap10;	//< IAP link 10 - links to tenth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap11;	//< IAP link 11- links to eleventh IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap12;	//< IAP link 12- links to twelveth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap13;	//< IAP link 13- links to thirteenth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap14;	//< IAP link 14- links to fourteenth IAP record.
		CMDBRecordLink<CCDIAPRecord> iIap15;	//< IAP link 15- links to fifteenth IAP record.
		CMDBField<TUint32> iIapCount;			//< Keeps count of the number of IAPs in the selection policy, for comparison purposes.

    private:
        static const SRecordTypeInfo* const iRecordInfo;
	};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//
// 1F/ AP PRIORITY SELECTION POLICY RECORD
// -----------   --------------------------
//| ID | NAME | | AP 1 | AP 2 | ... | AP N |
//|-----------| |--------------------------|
//|  * |    * | |    * |    * |   * |    * |
// -----------   --------------------------
/**
 @publishedAll
 @released
 */
class CCDAPPrioritySelectionPolicyRecord : public CCDSelectionPolicyRecordBase
	{
    public:
		enum { EMaxNrOfAps = 15 };

    public:
		CCDAPPrioritySelectionPolicyRecord(TMDBElementId aElementId);

		DATA_VTABLE

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:
		
		CMDBRecordLink<CCDAccessPointRecord>    iAp1;
		CMDBRecordLink<CCDAccessPointRecord>    iAp2;
		CMDBRecordLink<CCDAccessPointRecord>    iAp3;
		CMDBRecordLink<CCDAccessPointRecord>    iAp4;
		CMDBRecordLink<CCDAccessPointRecord>    iAp5;
		CMDBRecordLink<CCDAccessPointRecord>    iAp6;
		CMDBRecordLink<CCDAccessPointRecord>    iAp7;
		CMDBRecordLink<CCDAccessPointRecord>    iAp8;
		CMDBRecordLink<CCDAccessPointRecord>    iAp9;
		CMDBRecordLink<CCDAccessPointRecord>    iAp10;
		CMDBRecordLink<CCDAccessPointRecord>    iAp11;
		CMDBRecordLink<CCDAccessPointRecord>    iAp12;
		CMDBRecordLink<CCDAccessPointRecord>    iAp13;
		CMDBRecordLink<CCDAccessPointRecord>    iAp14;
		CMDBRecordLink<CCDAccessPointRecord>    iAp15;
		
		CMDBField<TUint32> iApCount;

    private:
        static const SRecordTypeInfo* const iRecordInfo;
	};


//
//  POLICY SELECTOR RECORD
//
/**
@publishedAll
@released
*/
class CCDPolicySelectorRecord : public CCDRecordBase
	{
	public:

		CCDPolicySelectorRecord();
		CCDPolicySelectorRecord(TMDBElementId aElementId);

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
    
    	DATA_VTABLE

	public:

		// Member Data
		CMDBField<TUint32>                    			iPolicyId;
	    CMDBField<TUid>                     			iAppUid;
		CMDBField<TDesC>                   				iSrcAddress;	// TInetAddr
		CMDBField<TDesC>                   				iSrcMask;		// TInetAddr
		CMDBField<TDesC>                   				iDstAddress;	// TInetAddr
		CMDBField<TDesC>                   				iDstMask;		// TInetAddr
		CMDBField<TInt>                        			iSrcPort;
		CMDBField<TInt>                        			iDstPort;
		CMDBField<TInt>                        			iSrcPortMax;
		CMDBField<TInt>                        			iDstPortMax;
		CMDBField<TUint32>                     			iProtocolId;
		CMDBField<TUint32>                     			iIapId;
		CMDBField<TInt>                        			iPriority;

    private:

        static const SRecordTypeInfo* const iRecordInfo;
	};


//
//  POLICY SELECTOR 2 PARAMS RECORD
//
/**
This table is introduced to allow linking to multimpe tables, for chosen policy selection.
Params tables are GenericQos, UmtsR99QoSAndOnTableRecord

@publishedAll
@released
*/
class CCDPolicySelector2ParamsRecord : public CCDRecordBase
	{
	public:

		CCDPolicySelector2ParamsRecord();
		CCDPolicySelector2ParamsRecord(TMDBElementId aElementId);

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
    
    	DATA_VTABLE

	public:

		// Member Data
		CMDBField<TUint32>                    			iPolicySelectorId;
		CMDBField<TUint32>                    			iParamsId;

    private:

        static const SRecordTypeInfo* const iRecordInfo;
	};

//
//  GENERIC QOS PARAMETER RECORD
//
/**
@publishedAll
@released
*/
class CCDGenericQosRecord : public CCDParamsRecordBase
	{
	public:

		CCDGenericQosRecord();
		CCDGenericQosRecord(TMDBElementId aElementId);

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
    
    	DATA_VTABLE

	public:

		// Member Data
		CMDBField<TInt>  iDownlinkBandwidth;
		CMDBField<TInt>  iUplinkBandwidth;
		CMDBField<TInt>  iDownLinkMaximumBurstSize;
		CMDBField<TInt>  iUpLinkMaximumBurstSize;
		CMDBField<TInt>  iDownLinkAveragePacketSize;
		CMDBField<TInt>  iUpLinkAveragePacketSize;
		CMDBField<TInt>  iDownLinkMaximumPacketSize;
		CMDBField<TInt>  iUpLinkMaximumPacketSize;
		CMDBField<TInt>  iDownLinkDelay;
		CMDBField<TInt>  iUpLinkDelay;
		CMDBField<TInt>  iDownLinkDelayVariation;
		CMDBField<TInt>  iUpLinkDelayVariation;
		CMDBField<TInt>  iDownLinkPriority;
		CMDBField<TInt>  iUpLinkPriority;
		CMDBField<TBool> iHeaderMode;
//		CMDBField<TDesC> iQosName;
    private:

        static const SRecordTypeInfo* const iRecordInfo;
	};

//
//  WIFI SCANENGINE RECORD
//
/**
@internalTechnology
*/
class CCDWifiScanEngineRecord : public CCDRecordBase
	{
	public:

		CCDWifiScanEngineRecord();
		CCDWifiScanEngineRecord(TMDBElementId aElementId);

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
    
    	DATA_VTABLE

	public:

		// Member Data
		CMDBField<TUint32>  iScanPeriodMs;
		CMDBField<TUint32>  iRSSIMin;
		CMDBField<TUint32>  iRSSIMax;
    private:

        static const SRecordTypeInfo* const iRecordInfo;
	};

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

} // end namespace CommsDat

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#endif

#endif
//COMMSDATTYPESV1_1_H

