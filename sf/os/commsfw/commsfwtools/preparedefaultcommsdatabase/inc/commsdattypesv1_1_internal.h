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
 @Internal
 @released
*/


#if (!defined COMMSDATTYPESV1_1_INTERNAL_H)
#define       COMMSDATTYPESV1_1_INTERNAL_H


#include <commsdattypesv1_1.h>

namespace CommsDat
{

//
// 1B/ AGENTLOOKUPTABLE RECORD
//
/**
@internalComponent
@released
*/
class CCDAgentLookupRecord : public CCDRecordBase
	{
	public:
		/**
		@internalComponent
		*/
		CCDAgentLookupRecord();
		/**
		@internalComponent
		*/
		CCDAgentLookupRecord(TMDBElementId aElementId);

		DATA_VTABLE

        const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:

		// Member Data
		CMDBField<TDesC>  	iAgentFriendlyName;
		CMDBField<TDesC>	 	iAgentFileName;


    private:

        static const SRecordTypeInfo* const iRecordInfo;
	};

//
// 20/  BASE DUMMY SECURITY SETTINGS RECORD
//
/**
@internalComponent
*/
class CCDBaseSecRecord : public CCDRecordBase
	{
	public:

		CCDBaseSecRecord();

		CCDBaseSecRecord(TMDBElementId aElementId);

		DATA_VTABLE

    private:

	   	static const SRecordTypeInfo* const iRecordInfo;

	};


//
// 21/  EAP SECURITY SETTINGS RECORD
//
/**
@internalComponent
*/
class CCDEAPSecRecord : public CCDBaseSecRecord
	{
	public:

		CCDEAPSecRecord();

		CCDEAPSecRecord(TMDBElementId aElementId);

		DATA_VTABLE

		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:

		// Member Data

		CMDBField<TCommsDatEAPSECOuterEAPType>	iEAPSecOuterEAPType;
		CMDBField<TDesC> 						iEAPSecId;
		CMDBField<TDesC>						iEAPSecPassword;
		CMDBField<TInt>  						iEAPSecConfigId;
		CMDBRecordLink<CCDBaseSecRecord>		iEAPSecData;	
        CMDBField<TDesC>  						iEAPSecVendorId;
        CMDBField<TInt>  						iEAPSecVendorType;        

    private:

	   	static const SRecordTypeInfo* const iRecordInfo;

	};

//
// 21/  TUNNELLED EAP SETTINGS RECORD
//
/**
@internalComponent
*/
class CCDTunEAPRecord : public CCDBaseSecRecord
	{
	public:

		CCDTunEAPRecord();

		CCDTunEAPRecord(TMDBElementId aElementId);

		DATA_VTABLE

		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:

		// Member Data

		CMDBField<TCommsDatTunEapInnerType>		iTUNEAPInnerType;
		CMDBRecordLink<CCDEAPSecRecord>			iTUNEAPData;

    private:

	   	static const SRecordTypeInfo* const iRecordInfo;
	};


//
// 22/  EAP-TLS SETTINGS RECORD
//
/**
@internalComponent
*/
class CCDEAPTLSRecord : public CCDBaseSecRecord
	{
	public:

		CCDEAPTLSRecord();

		CCDEAPTLSRecord(TMDBElementId aElementId);

		DATA_VTABLE

		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:

		// Member Data

		CMDBField<TDesC>						iEAPTLSClientCertName;

    private:

	   	static const SRecordTypeInfo* const iRecordInfo;


	};


//
// 23/  LEAP SETTINGS RECORD
//
/**
@internalComponent
*/
class CCDLEAPRecord : public CCDBaseSecRecord
	{
	public:

		CCDLEAPRecord();

		CCDLEAPRecord(TMDBElementId aElementId);

		DATA_VTABLE

		const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

	public:

		// Member Data

		CMDBField<TInt>							iLEAPTimeout;

    private:

	   	static const SRecordTypeInfo* const iRecordInfo;



	};



//
// 12/  EAP-SIM PROTOCOL RECORD
//
/**
@internalComponent
*/
class CCDEapSimProtocolRecord : public CCDRecordBase
	{
  public:

	CCDEapSimProtocolRecord();
	CCDEapSimProtocolRecord(TMDBElementId aElementId);

	const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
    
	DATA_VTABLE

  public:

	CMDBField<TUint32> iProtocolId;
	CMDBField<TDesC8> iPseudonym;
	CMDBField<TBool> iUseNaiRealm;
	CMDBField<TDesC8> iNaiRealm;
	CMDBField<TUint8> iMinRands;
    
  private:

  	static const SRecordTypeInfo* const iRecordInfo;
	};


//
// 13/  EAP-AKA PROTOCOL RECORD
//
/**
@internalComponent
*/
class CCDEapAkaProtocolRecord : public CCDRecordBase
	{
  public:

	CCDEapAkaProtocolRecord();
	CCDEapAkaProtocolRecord(TMDBElementId aElementId);

	const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
    
	DATA_VTABLE

  public:

	CMDBField<TUint32> iProtocolId;
	CMDBField<TDesC8> iPseudonym;
	CMDBField<TBool> iUseNaiRealm;
	CMDBField<TDesC8> iNaiRealm;
    
  private:
  	static const SRecordTypeInfo* const iRecordInfo;
	};

//
// CONFIG ACCESS POINT RECORD
/**
@internalTechnology
*/
class CCDConfigAccessPointRecord : public CCDRecordBase
	{
    public:
	CCDConfigAccessPointRecord(TMDBElementId aElementId);
	
	DATA_VTABLE
	    
	const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}
	
    public:
	CMDBRecordLink<CCDSCprRecord>	               iSCpr;
	CMDBRecordLink<CCDProtocolRecord>	       iProtocol;
	CMDBField<TMDBElementId>                       iProtocolConfig;
	CMDBRecordLink<CCDConfigAccessPointRecord>     iLayerBelow;
	
    private:
        static const SRecordTypeInfo* const iRecordInfo;
	};

} // namespace CommsDat

#endif // COMMSDATTYPESV1_1_INTERNAL_H
