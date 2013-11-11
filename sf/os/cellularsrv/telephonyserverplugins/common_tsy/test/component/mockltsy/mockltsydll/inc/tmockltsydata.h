// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef MOCKHWRMPLUGINDATA_H
#define MOCKHWRMPLUGINDATA_H

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <string.h>
#include "cmocksyengine.h"
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>

#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/serviceapi/cmmsmsutility.h>
#include <ctsy/rmmcustomapi.h>

#include "mmlist.h"
#include "pcktlist.h"


/**
enum for comparison of Serialiesed data

RBuf shouldn't be passed to Comparator, but this is done for compare 
Serialiesed data.

May be in future comparison of Serialiesed data should be changed....
*/

enum EDataTypes
	{
		EptrPhoneBookStoreEntry = 0,			// CPhoneBookStoreEntry*
		EArrayPtrSegOfPhoneBookStoreEntry		// CArrayPtrSeg<CPhoneBookStoreEntry>*
	};


/**
Class to compare 2 objects
*/
template <class T1>
class TComparator
    {
public:
    static TBool IsEqual(const T1& aData1, const T1& aData2)
        {
        // default implementation, need specialization
        return (memcmp(&aData1,&aData2,sizeof(T1))==0);
        }    
    };

/**
TComparator specialsiation for T*
*/
template <class T>
class TComparator<T*>
    {
public:
    static TBool IsEqual(const T* aData1, const T* aData2)
	    {
	    TBool result(EFalse);
	    
		if( aData1 == aData2 )
			{
			result = ETrue;
			}
		else if ( aData1 && aData2 )
			{
		    result = TComparator<T>::IsEqual(*aData1, *aData2);
			}

	    return result;                
	    }    
    };

/**
Class to serialize objects
*/
template <class T>
class TSerializer
    {
public:
    static void SerialiseL(const T& aData, RBuf8& aBuffer)
        {
	    TPckg<T> dataPckg(aData);
    	aBuffer.Create(dataPckg);
        }    
    static void DeserialiseL(const TDesC8& aPackedData, T& aData)
		{
	    TPckg<T> dataPckg(aData);
    	dataPckg.Copy(aPackedData.Left(dataPckg.Size()));
		}
    };

/**
Class to destroy objects
*/
template <class T>
class TDestroyer
    {
public:
    static void Destroy(T& /*aData*/){}    
    };

/**
TDestroyer specialisation for T*
*/
template <class T>
class TDestroyer<T*>
    {
public:
    static void Destroy(T* aData)
    	{
    	delete aData;
    	}    
    };

/**
Class to initialise objects
*/
template <class T>
class TInitialiser
    {
public:
    static void Initialise(T& /*aData*/){}    
    };

/**
TInitialiser specialisation for CArrayPtrFlat<T>*
*/
template <class T>
class TInitialiser<T*>
    {
public:
    static void Initialise(T*& aData)
	    {
   	   	aData = NULL;
    	}
    };

/**
Class to construct objects
*/
template <class T>
class TConstructor
    {
public:
    static void ConstructL(T*& aData)
    	{
    	aData = new (ELeave) T;
    	}    
    };

/**
Class to cleanup objects
*/
template <class T>
class TCleaner
    {
public:
    static void Cleanup(TAny* self)
    	{
		TDestroyer<T*>::Destroy(static_cast<T*>(self));
    	}    
    };

/**
Data w/o params
*/
class TMockLtsyData0 : public MMockSyEngineData
    {
public: 
    IMPORT_C TBool operator == (const TDesC8& aPackedData) const;           
    };

/**
Data with 1 param
*/
template <class T1> 
class TMockLtsyData1 : public TMockLtsyData0
    {
public: 
    TMockLtsyData1(T1& aData1);
public: 
    const T1& Data1() const;
    T1* Data1Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
public:
	void PushL();
	virtual	void Close();
protected:
    T1& iData1;
    };

/**
Owned data with 1 param
*/  
template <class T1>
class TMockLtsyData1Buf : public TMockLtsyData1<T1>
    {
public: 
    TMockLtsyData1Buf();

public: //from TMockLtsyData1
	virtual	void Close();
private:
    T1 iData1Own;
    };

/**
Data with 2 params
*/
template <class T1, class T2>   
class TMockLtsyData2 : public TMockLtsyData1<T1>
    {
public:
    TMockLtsyData2(T1& aData1,  T2& aData2);
public: 
    const T2& Data2() const;
    T2* Data2Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T2& iData2;
    };
    
/**
Owned data with 2 param
*/
template <class T1, class T2>
class TMockLtsyData2Buf : public TMockLtsyData2<T1, T2>
    {
public: 
    TMockLtsyData2Buf();

public: //from TMockLtsyData1
	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    };

/**
Data with 3 params
*/
template <class T1, class T2, class T3>   
class TMockLtsyData3 : public TMockLtsyData2<T1, T2>
    {
public:
    TMockLtsyData3(T1& aData1,  T2& aData2, T3& aData3);
public: 
    const T3& Data3() const;
    T3* Data3Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T3& iData3;
    };

/**
Owned data with 3 params
*/
template <class T1, class T2, class T3>
class TMockLtsyData3Buf : public TMockLtsyData3<T1, T2, T3>
    {
public: 
    TMockLtsyData3Buf();

public: //from TMockLtsyData1
	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    T3 iData3Own;
    };

/**
Data with 4 params
*/
template <class T1, class T2, class T3, class T4>
class TMockLtsyData4 : public TMockLtsyData3<T1, T2, T3>
    {
public:
    TMockLtsyData4(T1& aData1,  T2& aData2, T3& aData3, T4& aData4);
public:
    const T4& Data4() const;
    T4* Data4Ptr();
public:
    TBool operator == (const TDesC8& aPackedData) const;
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T4& iData4;
    };

/**
Owned data with 4 params
*/
template <class T1, class T2, class T3, class T4>
class TMockLtsyData4Buf : public TMockLtsyData4<T1, T2, T3, T4>
    {
public:
    TMockLtsyData4Buf();

public: //from TMockLtsyData1
	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    T3 iData3Own;
    T4 iData4Own;
    };

/**
Data with 5 params
*/
template <class T1, class T2, class T3, class T4, class T5>
class TMockLtsyData5 : public TMockLtsyData4<T1, T2, T3, T4>
    {
public:
    TMockLtsyData5(T1& aData1,  T2& aData2, T3& aData3, T4& aData4, T5& aData5);
public:
    const T5& Data5() const;
    T5* Data5Ptr();
public:
    TBool operator == (const TDesC8& aPackedData) const;
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T5& iData5;
    };

/**
Owned data with 5 params
*/
template <class T1, class T2, class T3, class T4, class T5>
class TMockLtsyData5Buf : public TMockLtsyData5<T1, T2, T3, T4, T5>
    {
public:
    TMockLtsyData5Buf();

public: //from TMockLtsyData1
	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    T3 iData3Own;
    T4 iData4Own;
    T5 iData5Own;
    };



/**
Data with 6 params
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
class TMockLtsyData6 : public TMockLtsyData5<T1, T2, T3, T4, T5>
    {
public:
    TMockLtsyData6(T1& aData1,  T2& aData2, T3& aData3, T4& aData4, T5& aData5, T6& aData6);
public:
    const T6& Data6() const;
    T6* Data6Ptr();
public:
    TBool operator == (const TDesC8& aPackedData) const;
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T6& iData6;
    };

/**
Owned data with 6 params
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
class TMockLtsyData6Buf : public TMockLtsyData6<T1, T2, T3, T4, T5, T6>
    {
public:
    TMockLtsyData6Buf();

public: //from TMockLtsyData1
	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    T3 iData3Own;
    T4 iData4Own;
    T5 iData5Own;
    T6 iData6Own;
    };


/**
Call data with w/o params params
*/
class TMockLtsyCallData0 : public MMockSyEngineData
    {
public:
    IMPORT_C TMockLtsyCallData0();
    IMPORT_C TMockLtsyCallData0(TInt aCallId, RMobilePhone::TMobileService aCallMode);
public: 
    IMPORT_C TInt CallId() const;
    IMPORT_C RMobilePhone::TMobileService CallMode() const;
public: 
    IMPORT_C TBool operator == (const TDesC8& aPackedData) const;           
    IMPORT_C void SerialiseL(RBuf8& aBuffer) const;
    IMPORT_C void DeserialiseL(const TDesC8& aPackedData);
protected:
    TInt iCallId;
    RMobilePhone::TMobileService iCallMode;
    };
    

/**
Call data with 1 params params
*/
template <class T1> 
class TMockLtsyCallData1 : public TMockLtsyCallData0
    {
public: 
    TMockLtsyCallData1(T1& aData1);
    TMockLtsyCallData1(TInt aCallId, RMobilePhone::TMobileService aCallMode, T1& aData1);
public:
    const T1& Data1() const;
    T1* Data1Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
public:
	virtual	void Close();
protected:
    T1& iData1;
    };

/**
Call data with 1 owned data params
*/
template <class T1> 
class TMockLtsyCallData1Buf : public TMockLtsyCallData1<T1>
    {
public: 
    TMockLtsyCallData1Buf();
    TMockLtsyCallData1Buf(TInt aCallId, RMobilePhone::TMobileService aCallMode);
public://from TMockLtsyCallData1
	virtual	void Close();
private:
    T1 iData1Own;
    };
        
/**
Call data with 2 params
*/
template <class T1, class T2>   
class TMockLtsyCallData2 : public TMockLtsyCallData1<T1>
    {
public:
    TMockLtsyCallData2(T1& aData1,  T2& aData2);
    TMockLtsyCallData2(TInt aCallId, RMobilePhone::TMobileService aCallMode, T1& aData1,  T2& aData2);
public:
    const T2& Data2() const;
    T2* Data2Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T2& iData2;
    };
    
/**
Call data with 2 owned data params
*/
template <class T1, class T2>   
class TMockLtsyCallData2Buf : public TMockLtsyCallData2<T1, T2>
    {
public: 
    TMockLtsyCallData2Buf();
    TMockLtsyCallData2Buf(TInt aCallId, RMobilePhone::TMobileService aCallMode);
public://from TMockLtsyCallData1
	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    };
   
//**************************

class TPacketQoSGPRSNegotiated : public RPacketQoS::TQoSGPRSNegotiated
	{
public:
	IMPORT_C TPacketQoSGPRSNegotiated();
public: 
    IMPORT_C TBool operator == (const TPacketQoSGPRSNegotiated& aPackedData) const;            
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
    IMPORT_C void InternalizeL(RReadStream& aStream);
	};

class TPacketQoSR99_R4Negotiated : public RPacketQoS::TQoSR99_R4Negotiated
	{
public:
	IMPORT_C TPacketQoSR99_R4Negotiated();
public: 
    IMPORT_C TBool operator == (const TPacketQoSR99_R4Negotiated& aPackedData) const;            
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
    IMPORT_C void InternalizeL(RReadStream& aStream);
	};

class TPacketQoSR5Negotiated : public RPacketQoS::TQoSR5Negotiated
	{
public:
	IMPORT_C TPacketQoSR5Negotiated();
public: 
    IMPORT_C TBool operator == (const TPacketQoSR5Negotiated& aPackedData) const;            
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
    IMPORT_C void InternalizeL(RReadStream& aStream);
	};

/**
Packet QoS data w/o params
*/
class TMockLtsyPacketQoSData0 : public MMockSyEngineData
    {
public: 
	IMPORT_C TMockLtsyPacketQoSData0();
    IMPORT_C TMockLtsyPacketQoSData0(TPacketQoSGPRSNegotiated&   aQoSGPRSNegotiated,
                                     TPacketQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
    						         TPacketQoSR5Negotiated&     aQoSR5_Negotiated);
public: 
	IMPORT_C TPacketQoSGPRSNegotiated&   QoSGPRSNegotiated  ();
	IMPORT_C TPacketQoSR99_R4Negotiated& QoSR99_R4Negotiated();
	IMPORT_C TPacketQoSR5Negotiated&     QoSR5Negotiated    ();
public: 
    IMPORT_C TBool operator == (const TDesC8& aPackedData) const;           
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
	TPacketQoSGPRSNegotiated   iQoSGPRSNegotiated;
    TPacketQoSR99_R4Negotiated iQoSR99_R4Negotiated;
    TPacketQoSR5Negotiated     iQoSR5_Negotiated;
    };
    
/**
Packet QoS data with 1 param
*/
template <class T1> 
class TMockLtsyPacketQoSData1 : public TMockLtsyPacketQoSData0
    {
public: 
    TMockLtsyPacketQoSData1(T1& aData1);
    TMockLtsyPacketQoSData1(TPacketQoSGPRSNegotiated&   aQoSGPRSNegotiated,
                            TPacketQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
    						TPacketQoSR5Negotiated&     aQoSR5_Negotiated,
    						T1& aData1);
public: 
    const T1& Data1() const;
    T1* Data1Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T1& iData1;
    };

/**
Packet data with 1 owned data params
*/
template <class T1> 
class TMockLtsyPacketQoSData1Buf : public TMockLtsyPacketQoSData1<T1>
    {
public: 
    TMockLtsyPacketQoSData1Buf();
    TMockLtsyPacketQoSData1Buf(TPacketQoSGPRSNegotiated&   aQoSGPRSNegotiated,
                               TPacketQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
    						   TPacketQoSR5Negotiated&     aQoSR5_Negotiated);
public:
	void Close();
private:
    T1 iData1Own;
    };

//**************************

/**
MediaAuthorizationData w/o params
*/
class TMockLtsyContextData0 : public MMockSyEngineData
    {
public:     
	IMPORT_C TMockLtsyContextData0();
    IMPORT_C TMockLtsyContextData0(RPacketContext::TContextConfigGPRS& aConfig,
                                   TInt aContextConfigurationType);
public: 
	IMPORT_C RPacketContext::TContextConfigGPRS& Config();
	IMPORT_C TInt& ContextConfigurationType();
public: 
    IMPORT_C TBool operator== (const TDesC8& aPackedData) const;           
    IMPORT_C void SerialiseL(RBuf8& aBuffer) const;
    IMPORT_C void DeserialiseL(const TDesC8& aPackedData);
protected:
	RPacketContext::TContextConfigGPRS iConfig;
	TInt iContextConfigurationType;        
    };

/**
Data with 1 param
*/
template <class T1> 
class TMockLtsyContextData1 : public TMockLtsyContextData0
    {
public: 
    TMockLtsyContextData1(T1& aData1);
    TMockLtsyContextData1(RPacketContext::TContextConfigGPRS& aConfig,
                                     TInt aContextConfigurationType,
                                     T1& aData1);
public: 
    const T1& Data1() const;
    T1* Data1Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
	void PushL();
	virtual	void Close();
protected:
    T1& iData1;
    };

/**
Owned data with 1 param
*/  
template <class T1>
class TMockLtsyContextData1Buf : public TMockLtsyContextData1<T1>
    {
public: 
    TMockLtsyContextData1Buf();
    TMockLtsyContextData1Buf(RPacketContext::TContextConfigGPRS& aConfig,
                             TInt aContextConfigurationType);

	virtual	void Close();
private:
    T1 iData1Own;
    };

/**
Data with 2 params
*/
template <class T1, class T2>   
class TMockLtsyContextData2 : public TMockLtsyContextData1<T1>
    {
public:
    TMockLtsyContextData2(T1& aData1,  T2& aData2);
    TMockLtsyContextData2(RPacketContext::TContextConfigGPRS& aConfig,
                          TInt aContextConfigurationType,
                          T1& aData1, T2& aData2);
public: 
    const T2& Data2() const;
    T2* Data2Ptr();
public: 
    TBool operator== (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T2& iData2;
    };
    
/**
Owned data with 2 param
*/
template <class T1, class T2>
class TMockLtsyContextData2Buf : public TMockLtsyContextData2<T1, T2>
    {
public: 
    TMockLtsyContextData2Buf();
    TMockLtsyContextData2Buf(RPacketContext::TContextConfigGPRS& aConfig,
                             TInt aContextConfigurationType);

	virtual	void Close();
private:
    T1 iData1Own;
    T2 iData2Own;
    };

//**************************

/**
PhoneBook data with w/o params params
*/
class TMockLtsyPhoneBookData0 : public MMockSyEngineData
    {
public:
    IMPORT_C TMockLtsyPhoneBookData0();
    IMPORT_C TMockLtsyPhoneBookData0(TName& aPhoneBookName);
public: 
    IMPORT_C TName& PhoneBookName();
public: 
    IMPORT_C TBool operator == (const TDesC8& aPackedData) const;           
    IMPORT_C void SerialiseL(RBuf8& aBuffer) const;
    IMPORT_C void DeserialiseL(const TDesC8& aPackedData);
protected:
    TName iPhoneBookName;
    };
    

/**
PhoneBook data with 1 params params
*/
template <class T1> 
class TMockLtsyPhoneBookData1 : public TMockLtsyPhoneBookData0
    {
public: 
    TMockLtsyPhoneBookData1(T1& aData1);
    TMockLtsyPhoneBookData1(TName& aPhoneBookName, T1& aData1);
public:
    const T1& Data1() const;
    T1* Data1Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
public:
	void PushL();
	virtual	void Close();
protected:
    T1& iData1;
    };

/**
PhoneBook data with 1 owned data params
*/
template <class T1> 
class TMockLtsyPhoneBookData1Buf : public TMockLtsyPhoneBookData1<T1>
    {
public: 
    TMockLtsyPhoneBookData1Buf();
    TMockLtsyPhoneBookData1Buf(TName& aPhoneBookName);    
public: //from TMockLtsyPhoneBookData1
	virtual	void Close();    
protected:
    T1 iData1Own;    
    friend class TMockLtsyPhoneBookData1<T1>;
    };
        
/**
PhoneBook data with 2 params
*/
template <class T1, class T2>   
class TMockLtsyPhoneBookData2 : public TMockLtsyPhoneBookData1<T1>
    {
public:
    TMockLtsyPhoneBookData2(T1& aData1,  T2& aData2);
    TMockLtsyPhoneBookData2(TName& aPhoneBookName, T1& aData1,  T2& aData2);
public:
    const T2& Data2() const;
    T2* Data2Ptr();
public: 
    TBool operator == (const TDesC8& aPackedData) const;            
    void SerialiseL(RBuf8& aBuffer) const;
    void DeserialiseL(const TDesC8& aPackedData);
protected:
    T2& iData2;
    };
    
/**
PhoneBook data with 2 owned data params
*/
template <class T1, class T2>   
class TMockLtsyPhoneBookData2Buf : public TMockLtsyPhoneBookData2<T1, T2>
    {
public: 
    TMockLtsyPhoneBookData2Buf();
    TMockLtsyPhoneBookData2Buf(TName& aPhoneBookName);
public: //from TMockLtsyPhoneBookData2
	virtual	void Close(); 
private:
    T1 iData1Own;
    T2 iData2Own;
    friend class TMockLtsyPhoneBookData2<T1, T2>;
    };

struct TLockSettingPassword
    {
    RMobilePhone::TMobilePhoneLock iLock;
    RMobilePhone::TMobilePhoneLockSetting iSetting;
    RMobilePhone::TMobilePassword iPassword;
    };

//**************************

/**
TMockLtsyData1 constructor
*/
template <class T1> 
TMockLtsyData1<T1>::TMockLtsyData1(T1& aData1)
    : iData1(aData1)
    {
    }

template <class T1> 
void TMockLtsyData1<T1>::PushL()
	{
	CleanupClosePushL(*this);
	}
	
template <class T1> 
void TMockLtsyData1<T1>::Close()
	{
	//do nothing should be overriden if needed
	}

/**
TMockLtsyData1Buf constructor
*/
template <class T1> 
TMockLtsyData1Buf<T1>::TMockLtsyData1Buf()
    : TMockLtsyData1<T1>(iData1Own)
    {
    TInitialiser<T1>::Initialise(iData1Own);
    }
    
/**
Destroy data
*/
template <class T1> 
void TMockLtsyData1Buf<T1>::Close()
    {
    TDestroyer<T1>::Destroy(iData1Own);
    }
    
/**
Return Data1
*/
template <class T1> 
const T1& TMockLtsyData1<T1>::Data1() const
    {
    return iData1;
    }

/**
Return a pointer to data 1
*/
template <class T1> 
T1* TMockLtsyData1<T1>::Data1Ptr()
    {
    return &iData1;
    }

/**
Compare with a descriptor
*/
template <class T1> 
TBool TMockLtsyData1<T1>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyData1Buf<T1> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
       	return EFalse;
    	}
	data.PushL();
	TMockLtsyData1* dataPtr = static_cast< TMockLtsyData1<T1>* >( &data );
    TBool res = TComparator<T1>::IsEqual(iData1, dataPtr->iData1);
	CleanupStack::PopAndDestroy(&data);
	return res;
    }

/**
Serialise to a buffer
*/
template <class T1> 
void TMockLtsyData1<T1>::SerialiseL(RBuf8& aBuffer) const
    {
    TSerializer<T1>::SerialiseL(iData1, aBuffer);
    }
    
/**
Deserialise from a buffer
*/
template <class T1> 
void TMockLtsyData1<T1>::DeserialiseL(const TDesC8& aPackedData)
    {
    TSerializer<T1>::DeserialiseL(aPackedData, iData1);
    }


/**
TMockLtsyData2 constructor
*/
template <class T1, class T2>   
TMockLtsyData2<T1,T2>::TMockLtsyData2(T1& aData1,  T2& aData2)
    : TMockLtsyData1<T1>(aData1) , iData2(aData2)
    {
    }
    
/**
TMockLtsyData2Buf constructor
*/
template <class T1, class T2>   
TMockLtsyData2Buf<T1, T2>::TMockLtsyData2Buf()
    : TMockLtsyData2<T1, T2>(iData1Own, iData2Own)
    {
    TInitialiser<T1>::Initialise(iData1Own);
    TInitialiser<T2>::Initialise(iData2Own);
    }

/**
Destroy data
*/
template <class T1, class T2>   
void TMockLtsyData2Buf<T1, T2>::Close()
    {
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data2
*/
template <class T1, class T2>   
const T2& TMockLtsyData2<T1,T2>::Data2() const
    {
    return iData2;
    }

/**
Return a pointer to data 2
*/
template <class T1, class T2>   
T2* TMockLtsyData2<T1,T2>::Data2Ptr()
    {
    return &iData2;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2>   
TBool TMockLtsyData2<T1,T2>::operator == (const TDesC8& aPackedData) const
	{
	TMockLtsyData2Buf<T1,T2> data;
	TRAPD(err, data.DeserialiseL(aPackedData));
	if (err)
		{
		data.Close();
		return EFalse;
		}
	data.PushL();
	TMockLtsyData2* dataPtr = static_cast< TMockLtsyData2<T1,T2>* >( &data );
	TBool res = TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1) && 
	            TComparator<T2>::IsEqual(this->iData2, dataPtr->iData2);
	CleanupStack::PopAndDestroy(&data);
	return res;
	}

/**
Serialise to a buffer
*/
template <class T1, class T2>       
void TMockLtsyData2<T1,T2>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);
	
	dataBuf.Close();
    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }
    
/**
Deserialise from a buffer
*/
template <class T1, class T2>       
void TMockLtsyData2<T1,T2>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	TUint dataSize(0);
	TPckg<TUint> sizeData(dataSize);
	sizeData.Copy(aPackedData.Left(uintLen));
	cursor += uintLen;
			
    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;
			
    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
    }


/**
TMockLtsyData3 constructor
*/
template <class T1, class T2, class T3>
TMockLtsyData3<T1,T2,T3>::TMockLtsyData3(T1& aData1, T2& aData2, T3& aData3)
    : TMockLtsyData2<T1, T2>(aData1, aData2) , iData3(aData3)
    {
    }

/**
TMockLtsyData3Buf constructor
*/
template <class T1, class T2, class T3>
TMockLtsyData3Buf<T1, T2, T3>::TMockLtsyData3Buf()
    : TMockLtsyData3<T1, T2, T3>(iData1Own, iData2Own, iData3Own)
    {
    TInitialiser<T1>::Initialise(iData1Own);
    TInitialiser<T2>::Initialise(iData2Own);
    TInitialiser<T3>::Initialise(iData3Own);
    }

/**
Destroy data
*/
template <class T1, class T2, class T3>
void TMockLtsyData3Buf<T1, T2, T3>::Close()
    {
    TDestroyer<T3>::Destroy(iData3Own);
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data3
*/
template <class T1, class T2, class T3>
const T3& TMockLtsyData3<T1,T2,T3>::Data3() const
    {
    return iData3;
    }

/**
Return a pointer to data 3
*/
template <class T1, class T2, class T3>
T3* TMockLtsyData3<T1,T2,T3>::Data3Ptr()
    {
    return &iData3;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2, class T3>
TBool TMockLtsyData3<T1,T2,T3>::operator == (const TDesC8& aPackedData) const
	{
	TMockLtsyData3Buf<T1,T2,T3> data;
	TRAPD(err, data.DeserialiseL(aPackedData));
	if (err)
		{
		data.Close();
		return EFalse;
		}
	data.PushL();
	TMockLtsyData3* dataPtr = static_cast< TMockLtsyData3<T1,T2,T3>* >( &data );
	TBool res = TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1) &&
	            TComparator<T2>::IsEqual(this->iData2, dataPtr->iData2) &&
				TComparator<T3>::IsEqual(this->iData3, dataPtr->iData3);
	CleanupStack::PopAndDestroy(&data);
	return res;
	}

/**
Serialise to a buffer
*/
template <class T1, class T2, class T3>
void TMockLtsyData3<T1,T2,T3>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T3>::SerialiseL(this->iData3, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1, class T2, class T3>
void TMockLtsyData3<T1,T2,T3>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);

	TUint dataSize(0);
	TPckg<TUint> sizeData(dataSize);
	sizeData.Copy(aPackedData.Left(uintLen));
	cursor += uintLen;

    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T3>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData3);
    }






/**
TMockLtsyData4 constructor
*/
template <class T1, class T2, class T3, class T4>
TMockLtsyData4<T1,T2,T3,T4>::TMockLtsyData4(T1& aData1, T2& aData2, T3& aData3, T4& aData4)
    : TMockLtsyData3<T1, T2, T3>(aData1, aData2, aData3) , iData4(aData4)
    {
    }

/**
TMockLtsyData4Buf constructor
*/
template <class T1, class T2, class T3, class T4>
TMockLtsyData4Buf<T1, T2, T3, T4>::TMockLtsyData4Buf()
    : TMockLtsyData4<T1, T2, T3, T4>(iData1Own, iData2Own, iData3Own, iData4Own)
    {
    TInitialiser<T1>::Initialise(iData1Own);
    TInitialiser<T2>::Initialise(iData2Own);
    TInitialiser<T3>::Initialise(iData3Own);
    TInitialiser<T4>::Initialise(iData4Own);
    }

/**
Destroy data
*/
template <class T1, class T2, class T3, class T4>
void TMockLtsyData4Buf<T1, T2, T3, T4>::Close()
    {
    TDestroyer<T4>::Destroy(iData4Own);
    TDestroyer<T3>::Destroy(iData3Own);
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data4
*/
template <class T1, class T2, class T3, class T4>
const T4& TMockLtsyData4<T1,T2,T3,T4>::Data4() const
    {
    return iData4;
    }

/**
Return a pointer to data 4
*/
template <class T1, class T2, class T3, class T4>
T4* TMockLtsyData4<T1,T2,T3,T4>::Data4Ptr()
    {
    return &iData4;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2, class T3, class T4>
TBool TMockLtsyData4<T1,T2,T3,T4>::operator == (const TDesC8& aPackedData) const
	{
	TMockLtsyData4Buf<T1,T2,T3,T4> data;
	TRAPD(err, data.DeserialiseL(aPackedData));
	if (err)
		{
		data.Close();
		return EFalse;
		}
	data.PushL();
	TMockLtsyData4* dataPtr = static_cast< TMockLtsyData4<T1,T2,T3,T4>* >( &data );
	TBool res = TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1) &&
	            TComparator<T2>::IsEqual(this->iData2, dataPtr->iData2) &&
				TComparator<T3>::IsEqual(this->iData3, dataPtr->iData3) &&
				TComparator<T4>::IsEqual(this->iData4, dataPtr->iData4);
	CleanupStack::PopAndDestroy(&data);
	return res;
	}

/**
Serialise to a buffer
*/
template <class T1, class T2, class T3, class T4>
void TMockLtsyData4<T1,T2,T3,T4>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T3>::SerialiseL(this->iData3, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T4>::SerialiseL(this->iData4, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1, class T2, class T3, class T4>
void TMockLtsyData4<T1,T2,T3,T4>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);

	TUint dataSize(0);
	TPckg<TUint> sizeData(dataSize);
	sizeData.Copy(aPackedData.Left(uintLen));
	cursor += uintLen;

    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T3>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData3);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T4>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData4);
    }

/**
TMockLtsyData5 constructor
*/
template <class T1, class T2, class T3, class T4, class T5>
TMockLtsyData5<T1,T2,T3,T4,T5>::TMockLtsyData5(T1& aData1, T2& aData2, T3& aData3, T4& aData4, T5& aData5)
    : TMockLtsyData4<T1, T2, T3, T4>(aData1, aData2, aData3, aData4) , iData5(aData5)
    {
    }

/**
TMockLtsyData5Buf constructor
*/
template <class T1, class T2, class T3, class T4, class T5>
TMockLtsyData5Buf<T1, T2, T3, T4, T5>::TMockLtsyData5Buf()
    : TMockLtsyData5<T1, T2, T3, T4, T5>(iData1Own, iData2Own, iData3Own, iData4Own, iData5Own)
    {
    TInitialiser<T1>::Initialise(iData1Own);
    TInitialiser<T2>::Initialise(iData2Own);
    TInitialiser<T3>::Initialise(iData3Own);
    TInitialiser<T4>::Initialise(iData4Own);
    TInitialiser<T5>::Initialise(iData5Own);
    }

/**
Destroy data
*/
template <class T1, class T2, class T3, class T4, class T5>
void TMockLtsyData5Buf<T1, T2, T3, T4, T5>::Close()
    {
    TDestroyer<T5>::Destroy(iData5Own);
    TDestroyer<T4>::Destroy(iData4Own);
    TDestroyer<T3>::Destroy(iData3Own);
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data5
*/
template <class T1, class T2, class T3, class T4, class T5>
const T5& TMockLtsyData5<T1,T2,T3,T4,T5>::Data5() const
    {
    return iData5;
    }

/**
Return a pointer to data 5
*/
template <class T1, class T2, class T3, class T4, class T5>
T5* TMockLtsyData5<T1,T2,T3,T4,T5>::Data5Ptr()
    {
    return &iData5;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2, class T3, class T4, class T5>
TBool TMockLtsyData5<T1,T2,T3,T4,T5>::operator == (const TDesC8& aPackedData) const
	{
	TMockLtsyData5Buf<T1,T2,T3,T4,T5> data;
	TRAPD(err, data.DeserialiseL(aPackedData));
	if (err)
		{
		data.Close();
		return EFalse;
		}
	data.PushL();
	TMockLtsyData5* dataPtr = static_cast< TMockLtsyData5<T1,T2,T3,T4,T5>* >( &data );
	TBool res = TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1) &&				
	            TComparator<T2>::IsEqual(this->iData2, dataPtr->iData2) &&				
				TComparator<T3>::IsEqual(this->iData3, dataPtr->iData3) &&
				TComparator<T4>::IsEqual(this->iData4, dataPtr->iData4) &&			
				TComparator<T5>::IsEqual(this->iData5, dataPtr->iData5);											
	CleanupStack::PopAndDestroy(&data);
	return res;
	}

/**
Serialise to a buffer
*/
template <class T1, class T2, class T3, class T4, class T5>
void TMockLtsyData5<T1,T2,T3,T4,T5>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T3>::SerialiseL(this->iData3, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T4>::SerialiseL(this->iData4, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T5>::SerialiseL(this->iData5, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1, class T2, class T3, class T4, class T5>
void TMockLtsyData5<T1,T2,T3,T4,T5>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);

	TUint dataSize(0);
	TPckg<TUint> sizeData(dataSize);
	sizeData.Copy(aPackedData.Left(uintLen));
	cursor += uintLen;

    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T3>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData3);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T4>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData4);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T5>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData5);    
    }    

//TMockLtsyData6
/**
TMockLtsyData6 constructor
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
TMockLtsyData6<T1,T2,T3,T4,T5,T6>::TMockLtsyData6(T1& aData1, T2& aData2, T3& aData3, T4& aData4, T5& aData5, T6& aData6)
    : TMockLtsyData5<T1, T2, T3, T4, T5>(aData1, aData2, aData3, aData4, aData5) , iData6(aData6)
    {
    }

/**
TMockLtsyData6Buf constructor
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
TMockLtsyData6Buf<T1, T2, T3, T4, T5, T6>::TMockLtsyData6Buf()
    : TMockLtsyData6<T1, T2, T3, T4, T5, T6>(iData1Own, iData2Own, iData3Own, iData4Own, iData5Own, iData6Own)
    {
    TInitialiser<T1>::Initialise(iData1Own);
    TInitialiser<T2>::Initialise(iData2Own);
    TInitialiser<T3>::Initialise(iData3Own);
    TInitialiser<T4>::Initialise(iData4Own);
    TInitialiser<T5>::Initialise(iData5Own);
    TInitialiser<T6>::Initialise(iData6Own);
    }

/**
Destroy data
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
void TMockLtsyData6Buf<T1, T2, T3, T4, T5, T6>::Close()
    {
    TDestroyer<T6>::Destroy(iData6Own);
    TDestroyer<T5>::Destroy(iData5Own);
    TDestroyer<T4>::Destroy(iData4Own);
    TDestroyer<T3>::Destroy(iData3Own);
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data6
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
const T6& TMockLtsyData6<T1,T2,T3,T4,T5,T6>::Data6() const
    {
    return iData6;
    }

/**
Return a pointer to data 6
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
T6* TMockLtsyData6<T1,T2,T3,T4,T5,T6>::Data6Ptr()
    {
    return &iData6;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
TBool TMockLtsyData6<T1,T2,T3,T4,T5,T6>::operator == (const TDesC8& aPackedData) const
	{
	TMockLtsyData6Buf<T1,T2,T3,T4,T5,T6> data;
	TRAPD(err, data.DeserialiseL(aPackedData));
	if (err)
		{
		data.Close();
		return EFalse;
		}
	data.PushL();
	TMockLtsyData6* dataPtr = static_cast< TMockLtsyData6<T1,T2,T3,T4,T5,T6>* >( &data );
	TBool res = TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1) &&
	            TComparator<T2>::IsEqual(this->iData2, dataPtr->iData2) &&
				TComparator<T3>::IsEqual(this->iData3, dataPtr->iData3) &&
				TComparator<T4>::IsEqual(this->iData4, dataPtr->iData4) &&
				TComparator<T5>::IsEqual(this->iData5, dataPtr->iData5) &&
				TComparator<T6>::IsEqual(this->iData6, dataPtr->iData6);
	CleanupStack::PopAndDestroy(&data);
	return res;
	}

/**
Serialise to a buffer
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
void TMockLtsyData6<T1,T2,T3,T4,T5,T6>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T3>::SerialiseL(this->iData3, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T4>::SerialiseL(this->iData4, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T5>::SerialiseL(this->iData5, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
    TSerializer<T6>::SerialiseL(this->iData6, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1, class T2, class T3, class T4, class T5, class T6>
void TMockLtsyData6<T1,T2,T3,T4,T5,T6>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);

	TUint dataSize(0);
	TPckg<TUint> sizeData(dataSize);
	sizeData.Copy(aPackedData.Left(uintLen));
	cursor += uintLen;

    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T3>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData3);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T4>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData4);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T5>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData5);
    cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;

    TSerializer<T6>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData6);
    }    



//*********************************************


/**
TMockLtsyCallData1 constructor
*/
template <class T1> 
TMockLtsyCallData1<T1>::TMockLtsyCallData1(T1& aData1)
    : iData1(aData1)
    {
    }
    
/**
TMockLtsyCallData1 constructor
*/
template <class T1> 
TMockLtsyCallData1<T1>::TMockLtsyCallData1(TInt aCallId, RMobilePhone::TMobileService aCallMode, T1& aData1)
    : TMockLtsyCallData0(aCallId,aCallMode), iData1(aData1)
    {
    }
    
/**
TMockLtsyCallData1Buf constructor
*/
template <class T1> 
TMockLtsyCallData1Buf<T1>::TMockLtsyCallData1Buf()
    : TMockLtsyCallData1<T1>(iData1Own)
        {
        }
        
/**
TMockLtsyCallData1Buf constructor
*/
template <class T1> 
TMockLtsyCallData1Buf<T1>::TMockLtsyCallData1Buf(TInt aCallId, RMobilePhone::TMobileService aCallMode)
    : TMockLtsyCallData1<T1>(aCallId,aCallMode, iData1Own)
        {
        }

/**
Return Data1
*/
template <class T1> 
const T1& TMockLtsyCallData1<T1>::Data1() const
    {
    return iData1;
    }

/**
Return a pointer to data 1
*/
template <class T1> 
T1* TMockLtsyCallData1<T1>::Data1Ptr()
    {
    return &iData1;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1> 
TBool TMockLtsyCallData1<T1>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyCallData1Buf<T1> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}
        
    TBool res = (this->iCallId==data.iCallId) && (this->iCallMode==data.iCallMode) && 
        TComparator<T1>::IsEqual(this->iData1, data.Data1());
    data.Close();
    return res;
    }

/**
Serialise to a buffer
*/
template <class T1> 
void TMockLtsyCallData1<T1>::SerialiseL(RBuf8& aBuffer) const
    {
    RDesWriteStream stream;
    aBuffer.Create(2*sizeof(TInt32) + sizeof(TInt32)+ sizeof(T1));
    stream.Open(aBuffer);
    stream << static_cast<TInt32>(iCallId);
    stream << static_cast<TInt32>(iCallMode);
    TPckg<T1> data1Pckg(this->iData1);
    stream << data1Pckg;
    stream.Close();
    }

/**
Deserialise from a buffer
*/
template <class T1> 
void TMockLtsyCallData1<T1>::DeserialiseL(const TDesC8& aPackedData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);
    TInt32 val;
    stream >> val;
    iCallId = val;
    stream >> val;
    TPckg<T1> data1Pckg(this->iData1);
    stream >> data1Pckg;
    iCallMode = static_cast<RMobilePhone::TMobileService>(val);
    stream.Close();
    }


/**
TMockLtsyCallData2 constructor
*/
template <class T1, class T2>   
TMockLtsyCallData2<T1,T2>::TMockLtsyCallData2(T1& aData1,  T2& aData2)
    : TMockLtsyCallData1<T1>(aData1), iData2(aData2)
    {
    }

/**
TMockLtsyCallData2 constructor
*/
template <class T1, class T2>   
TMockLtsyCallData2<T1,T2>::TMockLtsyCallData2(TInt aCallId, RMobilePhone::TMobileService aCallMode, 
         T1& aData1,  T2& aData2)
    : TMockLtsyCallData1<T1>(aCallId, aCallMode, aData1), iData2(aData2)
    {
    }
    
/**
TMockLtsyCallData2Buf constructor
*/
template <class T1, class T2>   
TMockLtsyCallData2Buf<T1, T2>::TMockLtsyCallData2Buf()
    : TMockLtsyCallData2<T1, T2>(iData1Own, iData2Own)
        {
        }
        
/**
TMockLtsyCallData2Buf constructor
*/
template <class T1, class T2>   
TMockLtsyCallData2Buf<T1, T2>::TMockLtsyCallData2Buf(TInt aCallId, RMobilePhone::TMobileService aCallMode)
    : TMockLtsyCallData2<T1, T2>(aCallId,aCallMode, iData1Own, iData2Own)
        {
        }

/**
Return Data2
*/
template <class T1, class T2>   
const T2& TMockLtsyCallData2<T1,T2>::Data2() const
    {
    return iData2;
    }

/**
Return a pointer to data 2
*/
template <class T1, class T2>   
T2* TMockLtsyCallData2<T1,T2>::Data2Ptr()
    {
    return &iData2;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2>   
TBool TMockLtsyCallData2<T1,T2>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyCallData2Buf<T1, T2> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}
        
    TBool res = (this->iCallId==data.iCallId) && (this->iCallMode==data.iCallMode) &&
        TComparator<T1>::IsEqual(this->iData1, data.Data1()) && 
        TComparator<T2>::IsEqual(this->iData2, data.Data2());
    data.Close();
    return res;
    }


/**
Serialise to a buffer
*/
template <class T1, class T2>       
void TMockLtsyCallData2<T1,T2>::SerialiseL(RBuf8& aBuffer) const
    {
    RDesWriteStream stream;
    // space for id and mode + 2 descriptors with length
    aBuffer.Create(2*sizeof(TInt32) +2*sizeof(TInt32)+ sizeof(T1) + sizeof(T2));
    stream.Open(aBuffer);
    stream << static_cast<TInt32>(this->iCallId);
    stream << static_cast<TInt32>(this->iCallMode);
    TPckg<T1> data1Pckg(this->iData1);
    stream << data1Pckg;
    TPckg<T2> data2Pckg(this->iData2);
    stream << data2Pckg;
    stream.Close();
    }
    
/**
Deserialise from a buffer
*/
template <class T1, class T2>       
void TMockLtsyCallData2<T1,T2>::DeserialiseL(const TDesC8& aPackedData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);
    TInt32 val;
    stream >> val;
    this->iCallId = val;
    stream >> val;
    this->iCallMode = static_cast<RMobilePhone::TMobileService>(val);
    TPckg<T1> data1Pckg(this->iData1);
    stream >> data1Pckg;
    TPckg<T2> data2Pckg(this->iData2);
    stream >> data2Pckg;
    stream.Close();
    }
    
    
template <class T1> 
void TMockLtsyCallData1<T1>::Close()
	{
	//do nothing, should be overriden
	} 

template <class T1> 
void TMockLtsyCallData1Buf<T1>::Close()
	{
	TDestroyer<T1>::Destroy(iData1Own);
	} 
	
template <class T1, class T2> 
void TMockLtsyCallData2Buf<T1, T2>::Close()
	{
	TDestroyer<T2>::Destroy(iData2Own);
	TDestroyer<T1>::Destroy(iData1Own);
	} 
    
/**
TComparator Specialisation for RMobileCall::TMobileCallInfoV1
*/
template<>
class TComparator<RMobileCall::TMobileCallInfoV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TMobileCallInfoV1>::IsEqual(const RMobileCall::TMobileCallInfoV1& aData1, const RMobileCall::TMobileCallInfoV1& aData2);
    };
/**
TComparator Specialisation for RMobileCall::TMobileCallInfoV8
*/
template<>
class TComparator<RMobileCall::TMobileCallInfoV8>
    {
public: 
	IMPORT_C static TBool TComparator<RMobileCall::TMobileCallInfoV8>::IsEqual(const RMobileCall::TMobileCallInfoV8& aData1, const RMobileCall::TMobileCallInfoV8& aData2);
    };

/**
TComparator Specialisation for RMobileCall::TMobileCallParamsV1
*/
template<>
class TComparator<RMobileCall::TMobileCallParamsV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TMobileCallParamsV1>::IsEqual(const RMobileCall::TMobileCallParamsV1& aData1, const RMobileCall::TMobileCallParamsV1& aData2);
    };

/**
TComparator Specialisation for RMobileCall::TMobileCallParamsV7
*/
template<>
class TComparator<RMobileCall::TMobileCallParamsV7>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TMobileCallParamsV7>::IsEqual(const RMobileCall::TMobileCallParamsV7& aData1, const RMobileCall::TMobileCallParamsV7& aData2);
    };

/**
TComparator Specialisation for RMobileCall::TMobileDataCallParamsV8
*/
template<>
class TComparator<RMobileCall::TMobileDataCallParamsV8>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TMobileDataCallParamsV8>::IsEqual(const RMobileCall::TMobileDataCallParamsV8& aData1, const RMobileCall::TMobileDataCallParamsV8& aData2);
    };


 /**
TComparator Specialisation for TContextConfiguration
*/
template<>
class TComparator<TContextConfiguration>
    {
public: 
    IMPORT_C static TBool TComparator<TContextConfiguration>::IsEqual(const TContextConfiguration& aData1, const TContextConfiguration& aData2);
    };

/**
TComparator Specialisation for TMmMbmsMonitorServiceListParams
*/
template<>
class TComparator<CPcktMbmsMonitoredServiceList>
    {
public: 
    IMPORT_C static TBool TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(const CPcktMbmsMonitoredServiceList& aData1, const CPcktMbmsMonitoredServiceList& aData2);
    };


/**
TComparator Specialisation for RPacketContext::CTFTMediaAuthorizationV3
*/
template<>
class TComparator<RPacketContext::CTFTMediaAuthorizationV3>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::CTFTMediaAuthorizationV3>::
            IsEqual(const RPacketContext::CTFTMediaAuthorizationV3& aData1, 
                    const RPacketContext::CTFTMediaAuthorizationV3& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TMobilePhoneCFChangeV1
*/
template<>
class TComparator<RMobilePhone::TMobilePhoneCFChangeV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhoneCFChangeV1>::IsEqual(const RMobilePhone::TMobilePhoneCFChangeV1& aData1, const RMobilePhone::TMobilePhoneCFChangeV1& aData2);
    };

/**
TComparator Specialisation for TInfoName
*/
template<>
class TComparator<TInfoName>
    {
public: 
    IMPORT_C static TBool TComparator<TInfoName>::IsEqual(const TInfoName& aData1, const TInfoName& aData2);
    };

/**
TComparator Specialisation for TContextParams
*/
template<>
class TComparator<TContextParams>
    {
public: 
    IMPORT_C static TBool TComparator<TContextParams>::IsEqual(const TContextParams& aData1, const TContextParams& aData2);
    };

/**
TComparator Specialisation for TENStoreResponse
*/
template<>
class TComparator<TENStoreResponse>
    {
public: 
    IMPORT_C static TBool TComparator<TENStoreResponse>::IsEqual(const TENStoreResponse& aData1, const TENStoreResponse& aData2);
    };

/**
TComparator Specialisation for TCbsCbmiAndLangAndFilter
*/
template<>
class TComparator<TCbsCbmiAndLangAndFilter>
    {
public: 
    IMPORT_C static TBool TComparator<TCbsCbmiAndLangAndFilter>::
                              IsEqual(const TCbsCbmiAndLangAndFilter& aData1,
                                      const TCbsCbmiAndLangAndFilter& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TImsAuthenticateDataV5
*/
template<>
class TComparator<RMobilePhone::TImsAuthenticateDataV5>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TImsAuthenticateDataV5>::
                              IsEqual(const RMobilePhone::TImsAuthenticateDataV5& aData1,
                                      const RMobilePhone::TImsAuthenticateDataV5& aData2);
    };

/**
TComparator Specialisation for RMobileENStore::TEmergencyNumber
*/
template<>
class TComparator<RMobileENStore::TEmergencyNumber>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileENStore::TEmergencyNumber>::
                          IsEqual(const RMobileENStore::TEmergencyNumber& aData1,
                                  const RMobileENStore::TEmergencyNumber& aData2);
    };
     
/**
TComparator Specialisation for RPacketContext::TContextConfigGPRS
*/
template<>
class TComparator<RPacketContext::TContextConfigGPRS>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::TContextConfigGPRS>::IsEqual(const RPacketContext::TContextConfigGPRS& aData1, const RPacketContext::TContextConfigGPRS& aData2);
    };
    
/**
TComparator Specialisation for RPacketContext::TContextConfigR99_R4 
*/
template<>
class TComparator<RPacketContext::TContextConfigR99_R4>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::TContextConfigR99_R4>::IsEqual(const RPacketContext::TContextConfigR99_R4& aData1, const RPacketContext::TContextConfigR99_R4& aData2);
    };
    
/**
TComparator Specialisation for RPacketContext::TContextConfig_R5 
*/
template<>
class TComparator<RPacketContext::TContextConfig_R5>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::TContextConfig_R5>::IsEqual(const RPacketContext::TContextConfig_R5& aData1, const RPacketContext::TContextConfig_R5& aData2);
    };
    
/**
TComparator Specialisation for RMobilePhone::TMobilePhoneSecurityCode 
*/
template<>
class TComparator<RMobilePhone::TMobilePhoneSecurityCode>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhoneSecurityCode>::IsEqual(const RMobilePhone::TMobilePhoneSecurityCode& aData1, const RMobilePhone::TMobilePhoneSecurityCode& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TCodeAndUnblockCode 
*/
template<>
class TComparator<RMobilePhone::TCodeAndUnblockCode>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TCodeAndUnblockCode>::IsEqual(const RMobilePhone::TCodeAndUnblockCode& aData1, const RMobilePhone::TCodeAndUnblockCode& aData2);
    };
    
/**
TComparator Specialisation for RMobilePhone::TAPNEntryV3 
*/
template<>
class TComparator<RMobilePhone::TAPNEntryV3>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TAPNEntryV3>::IsEqual(const RMobilePhone::TAPNEntryV3& aData1, const RMobilePhone::TAPNEntryV3& aData2);
    };
    
    
/**
TComparator Specialisation for CMobilePhoneNetworkListV2*
	// This Comparator is not used
	// but to prevent  ambiguity in case of using this Comparator
	// EFalse is returned
	
	// if you need to compare data of CMobilePhoneNetworkListV2* - type 
	// please, be free to fill in this Comparator 
*/
template<>
class TComparator<CMobilePhoneNetworkListV2*>
    {
public: 
    IMPORT_C static TBool TComparator<CMobilePhoneNetworkListV2*>::IsEqual(const CMobilePhoneNetworkListV2* aData1, const CMobilePhoneNetworkListV2* aData2);
    };
    
    
//TComparator Specialisation for RMmCustomAPI::TSimFileInfo
template<>
class TComparator<RMmCustomAPI::TSimFileInfo>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimFileInfo>::IsEqual(const RMmCustomAPI::TSimFileInfo& aData1, const RMmCustomAPI::TSimFileInfo& aData2);
    };


/**
TMockLtsyPhoneBookData1 constructor
*/
template <class T1> 
TMockLtsyPhoneBookData1<T1>::TMockLtsyPhoneBookData1(T1& aData1)
    : iData1(aData1)
    {
    }
    
/**
TMockLtsyPhoneBookData1 constructor
*/
template <class T1> 
TMockLtsyPhoneBookData1<T1>::TMockLtsyPhoneBookData1(TName& aPhoneBookName, T1& aData1)
    : TMockLtsyPhoneBookData0(aPhoneBookName), iData1(aData1)
    {
    }
    
template <class T1> 
void TMockLtsyPhoneBookData1<T1>::PushL()
	{
	CleanupClosePushL(*this);
	}
	
template <class T1> 
void TMockLtsyPhoneBookData1<T1>::Close()
	{
	//do nothing should be overriden if needed
	}

    
/**
TMockLtsyPhoneBookData1Buf constructor
*/
template <class T1> 
TMockLtsyPhoneBookData1Buf<T1>::TMockLtsyPhoneBookData1Buf()
    : TMockLtsyPhoneBookData1<T1>(iData1Own)
        {
    	TInitialiser<T1>::Initialise(iData1Own);    
        }
        
/**
TMockLtsyPhoneBookData1Buf constructor
*/
template <class T1> 
TMockLtsyPhoneBookData1Buf<T1>::TMockLtsyPhoneBookData1Buf(TName& aPhoneBookName)
    : TMockLtsyPhoneBookData1<T1>(aPhoneBookName, iData1Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
        }

template <class T1> 
void TMockLtsyPhoneBookData1Buf<T1>::Close()
	{
	TDestroyer<T1>::Destroy(iData1Own);
	}        


/**
Return Data1
*/
template <class T1> 
const T1& TMockLtsyPhoneBookData1<T1>::Data1() const
    {
    return iData1;
    }

/**
Return a pointer to data 1
*/
template <class T1> 
T1* TMockLtsyPhoneBookData1<T1>::Data1Ptr()
    {
    return &iData1;
    }

/**
Compare with a descriptor
Non-Leaving
*/
template <class T1> 
TBool TMockLtsyPhoneBookData1<T1>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyPhoneBookData1Buf<T1> data;  
    TRAPD(err, data.DeserialiseL(aPackedData));    
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}       
        
   	TBool ret =  (this->iPhoneBookName==data.iPhoneBookName) &&     
        TComparator<T1>::IsEqual(this->iData1, data.Data1());
    data.Close();
    return ret;        
    }

/**
Serialise to a buffer
*/
template <class T1> 
void TMockLtsyPhoneBookData1<T1>::SerialiseL(RBuf8& aBuffer) const
    {
    RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	
	// serialise data into temprory buffer
	TSerializer<T1>::SerialiseL(iData1, dataBuf);
	
	TInt size = sizeof (TName) + dataBuf.Size();
	aBuffer.Create(size);	
	
	// put TName into aBuffer
	TPckg<TName> pckgName(iPhoneBookName);
	aBuffer.Append(pckgName);
	
	// put Serialised data into aBuffer
	aBuffer.Append(dataBuf);
	
	CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1> 
void TMockLtsyPhoneBookData1<T1>::DeserialiseL(const TDesC8& aPackedData)
    {
		
	TName name;
	TPckg<TName> pckgName(name);
	pckgName.Copy( aPackedData.Left(sizeof(TName)) );
	
	this->iPhoneBookName = name;
			
    TSerializer<T1>::DeserialiseL(aPackedData.Mid ( sizeof(TName) ), this->iData1);    
    }


/**
TMockLtsyPhoneBookData2 constructor
*/
template <class T1, class T2>   
TMockLtsyPhoneBookData2<T1,T2>::TMockLtsyPhoneBookData2(T1& aData1,  T2& aData2)
    : TMockLtsyPhoneBookData1<T1>(aData1), iData2(aData2)
    {
    }

/**
TMockLtsyPhoneBookData2 constructor
*/
template <class T1, class T2>   
TMockLtsyPhoneBookData2<T1,T2>::TMockLtsyPhoneBookData2(TName& aPhoneBookName, 
         T1& aData1,  T2& aData2)
    : TMockLtsyPhoneBookData1<T1>(aPhoneBookName, aData1), iData2(aData2)
    {
    }
    
/**
TMockLtsyPhoneBookData2Buf constructor
*/
template <class T1, class T2>   
TMockLtsyPhoneBookData2Buf<T1, T2>::TMockLtsyPhoneBookData2Buf()
    : TMockLtsyPhoneBookData2<T1, T2>(iData1Own, iData2Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
    	TInitialiser<T2>::Initialise(iData2Own);
        }
        
/**
TMockLtsyPhoneBookData2Buf constructor
*/
template <class T1, class T2>   
TMockLtsyPhoneBookData2Buf<T1, T2>::TMockLtsyPhoneBookData2Buf(TName& aPhoneBookName)
    : TMockLtsyPhoneBookData2<T1, T2>(aPhoneBookName, iData1Own, iData2Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
    	TInitialiser<T2>::Initialise(iData2Own);
        }
 
 /**
Destroy data
*/
template <class T1, class T2>   
void TMockLtsyPhoneBookData2Buf<T1, T2>::Close()
    {
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data2
*/
template <class T1, class T2>   
const T2& TMockLtsyPhoneBookData2<T1,T2>::Data2() const
    {
    return iData2;
    }

/**
Return a pointer to data 2
*/
template <class T1, class T2>   
T2* TMockLtsyPhoneBookData2<T1,T2>::Data2Ptr()
    {
    return &iData2;
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2>   
TBool TMockLtsyPhoneBookData2<T1,T2>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyPhoneBookData2Buf<T1, T2> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}
        
    TBool ret = (this->iPhoneBookName==data.iPhoneBookName) && 
        TComparator<T1>::IsEqual(this->iData1, data.iData1) && 
        TComparator<T2>::IsEqual(this->iData2, data.iData2);
    data.Close();    
    return ret;
    }


/**
Serialise to a buffer
*/
template <class T1, class T2>       
void TMockLtsyPhoneBookData2<T1,T2>::SerialiseL(RBuf8& aBuffer) const
    {
    RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TName) + sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);


	TPckg<TName> pckgName(this->iPhoneBookName);
	
	// put TName into aBuffer
	aBuffer.Append(pckgName);

	// put size and data 1 into aBuffer
	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);
	
	dataBuf.Close();
    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);
	
	// put size and data 1 into aBuffer
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }
    
/**
Deserialise from a buffer
*/
template <class T1, class T2>       
void TMockLtsyPhoneBookData2<T1,T2>::DeserialiseL(const TDesC8& aPackedData)
    {
   	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	
	TName name;
	TPckg<TName> pckgName(name);
	pckgName.Copy( aPackedData.Left(sizeof(TName)) );
	cursor += sizeof (TName);
	
	this->iPhoneBookName = name;
	
	TUint dataSize(0);
	TPckg<TUint> sizeData(dataSize);
	sizeData.Copy(aPackedData.Mid(cursor,uintLen));
	cursor += uintLen;
			
    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
	cursor += dataSize;

	sizeData.Copy(aPackedData.Mid(cursor, uintLen));
	cursor += uintLen;
			
    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
    }   

//********************

typedef CArrayPtrSeg<CPhoneBookStoreEntry>* BookStoreEntryArrayPtr;


/**
TComparator Specialisation for CArrayPtrSeg<CPhoneBookStoreEntry>*
*/
template<>
class TComparator<BookStoreEntryArrayPtr>
    {
public: 
    IMPORT_C static TBool TComparator< BookStoreEntryArrayPtr >::IsEqual(const BookStoreEntryArrayPtr &aData1, 
                    const BookStoreEntryArrayPtr &aData2);
    };



typedef RMobilePhoneBookStore::TPBIndexAndNumEntries* IAndEPtr;

/**
TComparator Specialisation for RMobilePhoneBookStore::TPBIndexAndNumEntries*
*/
template<>
class TComparator<IAndEPtr>
    {
public: 
    IMPORT_C static TBool TComparator<IAndEPtr>::IsEqual(const IAndEPtr &aData1, const IAndEPtr &aData2);
    };



typedef CPhoneBookStoreEntry* StoreEntryPtr;


/**
TComparator Specialisation for CPhoneBookStoreEntry*
*/
template<>
class TComparator<StoreEntryPtr>
    {
public: 
    IMPORT_C static TBool TComparator<StoreEntryPtr>::IsEqual(const StoreEntryPtr &aData1, const StoreEntryPtr &aData2);
    };

typedef CDesCArray* DesCArrayPtr;

/**
TComparator Specialisation for CDesCArray*
*/
template<>
class TComparator<DesCArrayPtr>
    {
public: 
    IMPORT_C static TBool TComparator<DesCArrayPtr>::IsEqual(const DesCArrayPtr &aData1, const DesCArrayPtr &aData2);
    };
    
        
   //**************************************************************

/**
TMockLtsyPacketQoSData1 constructor
*/
template <class T1> 
TMockLtsyPacketQoSData1<T1>::TMockLtsyPacketQoSData1(T1& aData1)
    : iData1(aData1)
    {
    }
    
/**
TMockLtsyPhoneBookData1 constructor
*/
template <class T1> 
TMockLtsyPacketQoSData1<T1>::TMockLtsyPacketQoSData1(TPacketQoSGPRSNegotiated& aQoSGPRSNegotiated,
	                                                 TPacketQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
                                                     TPacketQoSR5Negotiated& aQoSR5_Negotiated,
                                                     T1& aData1) 
    : TMockLtsyPacketQoSData0(aQoSGPRSNegotiated, 
                              aQoSR99_R4Negotiated, 
                              aQoSR5_Negotiated), iData1(aData1)
    {
    }

/**
Return Data1
*/
template <class T1> 
const T1& TMockLtsyPacketQoSData1<T1>::Data1() const
    {
    return iData1;
    }

   //**************************************************************
/**
Return a pointer to data 1
*/
template <class T1> 
T1* TMockLtsyPacketQoSData1<T1>::Data1Ptr()
    {
    return &iData1;
    }

/**
Serialise to a buffer
*/
template <class T1> 
void TMockLtsyPacketQoSData1<T1>::SerialiseL(RBuf8& aBuffer) const
    {
    RDesWriteStream stream;
    aBuffer.Create(sizeof(TInt32) + 
                   sizeof(TPacketQoSGPRSNegotiated  ) + 
                   sizeof(TPacketQoSR99_R4Negotiated) + 
                   sizeof(TPacketQoSR5Negotiated    ) + 
                   sizeof(T1));
    stream.Open(aBuffer);
    stream << static_cast<TPacketQoSGPRSNegotiated>  (iQoSGPRSNegotiated  );
    stream << static_cast<TPacketQoSR99_R4Negotiated>(iQoSR99_R4Negotiated);
    stream << static_cast<TPacketQoSR5Negotiated>    (iQoSR5_Negotiated   );
    TPckg<T1> data1Pckg(this->iData1);
    stream << data1Pckg;
    stream.Close();
    }

/**
Deserialise from a buffer
*/
template <class T1> 
void TMockLtsyPacketQoSData1<T1>::DeserialiseL(const TDesC8& aPackedData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);

    TPacketQoSGPRSNegotiated   valQoSGPRSNegotiated;
    TPacketQoSR99_R4Negotiated valQoSR99_R4Negotiated;
    TPacketQoSR5Negotiated     valiQoSR5_Negotiated;
    
    stream >> valQoSGPRSNegotiated;
    stream >> valQoSR99_R4Negotiated;
    stream >> valiQoSR5_Negotiated;
    
    iQoSGPRSNegotiated   = valQoSGPRSNegotiated;
    iQoSR99_R4Negotiated = valQoSR99_R4Negotiated;
    iQoSR5_Negotiated    = valiQoSR5_Negotiated;

    TPckg<T1> data1Pckg(this->iData1);
    stream >> data1Pckg;
    stream.Close();
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1> 
TBool TMockLtsyPacketQoSData1<T1>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyPacketQoSData1Buf<T1> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}
        
    TBool res =  (this->iQoSGPRSNegotiated   == data.iQoSGPRSNegotiated)   && 
           (this->iQoSR99_R4Negotiated == data.iQoSR99_R4Negotiated) && 
           (this->iQoSR5_Negotiated    == data.iQoSR5_Negotiated)    && 
           TComparator<T1>::IsEqual(this->iData1, data.Data1());
    data.Close();
    return res;
    }

/**
TMockLtsyPacketQoSData1Buf constructor
*/
template <class T1> 
TMockLtsyPacketQoSData1Buf<T1>::TMockLtsyPacketQoSData1Buf()
    : TMockLtsyPacketQoSData1<T1>(iData1Own)
        {
        }
        
/**
TMockLtsyPacketQoSData1Buf constructor
*/
template <class T1> 
TMockLtsyPacketQoSData1Buf<T1>::TMockLtsyPacketQoSData1Buf(TPacketQoSGPRSNegotiated& aQoSGPRSNegotiated,
	                                                       TPacketQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
                                                           TPacketQoSR5Negotiated& aQoSR5_Negotiated)
    : TMockLtsyPacketQoSData1<T1>(aQoSGPRSNegotiated, aQoSR99_R4Negotiated, aQoSR5_Negotiated, iData1Own)
        {
        }

template <class T1>         
void TMockLtsyPacketQoSData1Buf<T1>::Close()
	{
	TDestroyer<T1>::Destroy(iData1Own);
	}

//**************************************************************

/**
TMockLtsyContextData1 constructor
*/

template <class T1> 
TMockLtsyContextData1<T1>::TMockLtsyContextData1(T1& aData1)
    : iData1(aData1)
    {
    }

/**
TMockLtsyContextData1 constructor
*/
template <class T1> 
TMockLtsyContextData1<T1>::TMockLtsyContextData1(
                                     RPacketContext::TContextConfigGPRS& aConfig,
                                     TInt aContextConfigurationType,
                                     T1& aData1)
    : TMockLtsyContextData0(aConfig, aContextConfigurationType), iData1(aData1)
    {
    }

template <class T1> 
void TMockLtsyContextData1<T1>::Close()
	{
	//do nothing should be overriden if needed
	}

/**
Destroy data
*/
template <class T1> 
void TMockLtsyContextData1Buf<T1>::Close()
    {
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Destroy data
*/
template <class T1, class T2>   
void TMockLtsyContextData2Buf<T1, T2>::Close()
    {
    TDestroyer<T2>::Destroy(iData2Own);
    TDestroyer<T1>::Destroy(iData1Own);
    }

/**
Return Data1
*/
template <class T1> 
const T1& TMockLtsyContextData1<T1>::Data1() const
    {
    return iData1;
    }

/**
Return a pointer to data 1
*/
template <class T1> 
T1* TMockLtsyContextData1<T1>::Data1Ptr()
    {
    return &iData1;
    }

template <class T1> 
void TMockLtsyContextData1<T1>::PushL()
	{
	CleanupClosePushL(*this);
	}

/**
Serialise to a buffer
*/
template <class T1> 
void TMockLtsyContextData1<T1>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T1>::SerialiseL(this->iData1, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

	dataBuf.Close();
	TMockLtsyContextData0::SerialiseL(dataBuf);

	newSize += dataBuf.Size();
	aBuffer.ReAllocL(newSize);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1> 
void TMockLtsyContextData1<T1>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	if (aPackedData.Size())
		{
		TUint dataSize(0);
		TPckg<TUint> sizeData(dataSize);
		sizeData.Copy(aPackedData.Left(uintLen));
		cursor = uintLen;
				
	    TSerializer<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData1);
		cursor += dataSize;

		dataSize = aPackedData.Length() - cursor;
		TMockLtsyContextData0::DeserialiseL(aPackedData.Mid(cursor, dataSize));
		}
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1> 
TBool TMockLtsyContextData1<T1>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyContextData1Buf<T1> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}
	data.PushL();
	TMockLtsyContextData1* dataPtr = static_cast< TMockLtsyContextData1<T1>* >( &data );
    TBool res = (this->iContextConfigurationType == 
                 dataPtr->iContextConfigurationType)                  && 
                 TComparator<RPacketContext::TContextConfigGPRS>::
                 	IsEqual(this->iConfig, dataPtr->iConfig)          && 
                 TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1);
	CleanupStack::PopAndDestroy(&data);
	return res;
    }

/**
TMockLtsyContextData1Buf constructor
*/
template <class T1> 
TMockLtsyContextData1Buf<T1>::TMockLtsyContextData1Buf()
    : TMockLtsyContextData1<T1>(iData1Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
        }
        
/**
TMockLtsyContextData1Buf constructor
*/
template <class T1> 
TMockLtsyContextData1Buf<T1>::TMockLtsyContextData1Buf(
                                         RPacketContext::TContextConfigGPRS& aConfig,
	                                     TInt aContextConfigurationType)
    : TMockLtsyContextData1<T1>(aConfig, aContextConfigurationType, iData1Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
        }

/**
TMockLtsyContextData2 constructor
*/
template <class T1, class T2> 
TMockLtsyContextData2<T1, T2>::TMockLtsyContextData2(T1& aData1, T2& aData2)
    : TMockLtsyContextData1<T1>(aData1), iData2(aData2)
    {
    }

/**
TMockLtsyContextData1 constructor
*/
template <class T1, class T2> 
TMockLtsyContextData2<T1, T2>::TMockLtsyContextData2(
                                          RPacketContext::TContextConfigGPRS& aConfig,
                                          TInt aContextConfigurationType,
                                          T1& aData1, T2& aData2)
    : TMockLtsyContextData1<T1>(aConfig, 
                                           aContextConfigurationType, 
                                           aData1), iData2(aData2)
    {
    }

/**
Return Data2
*/
template <class T1, class T2> 
const T2& TMockLtsyContextData2<T1, T2>::Data2() const
    {
    return iData2;
    }

/**
Return a pointer to Data2
*/
template <class T1, class T2> 
T2* TMockLtsyContextData2<T1, T2>::Data2Ptr()
    {
    return &iData2;
    }

/**
Serialise to a buffer
*/
template <class T1, class T2> 
void TMockLtsyContextData2<T1, T2>::SerialiseL(RBuf8& aBuffer) const
    {
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

    TSerializer<T2>::SerialiseL(this->iData2, dataBuf);

	TUint dataSize(0);
	dataSize = dataBuf.Size();
	TInt newSize = sizeof(TUint) + dataSize;
	aBuffer.Create(newSize);

	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);
	
	dataBuf.Close();
	TMockLtsyContextData1<T1>::SerialiseL(dataBuf);

	dataSize = dataBuf.Size();
	newSize += sizeof(TUint) + dataSize;
	aBuffer.ReAllocL(newSize);

	aBuffer.Append(sizeData);
	aBuffer.Append(dataBuf);

    CleanupStack::PopAndDestroy( &dataBuf );
    }

/**
Deserialise from a buffer
*/
template <class T1, class T2> 
void TMockLtsyContextData2<T1, T2>::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	if (aPackedData.Size())
		{
		TUint dataSize(0);
		TPckg<TUint> sizeData(dataSize);
		sizeData.Copy(aPackedData.Left(uintLen));
		cursor = uintLen;
				
	    TSerializer<T2>::DeserialiseL(aPackedData.Mid(cursor, dataSize), this->iData2);
		cursor += dataSize;

		sizeData.Copy(aPackedData.Mid(cursor, uintLen));
		cursor += uintLen;
				
	    TMockLtsyContextData1<T1>::DeserialiseL(aPackedData.Mid(cursor, dataSize));
		}
    }

/**
Compare with a descriptor
Non-leaving
*/
template <class T1, class T2> 
TBool TMockLtsyContextData2<T1, T2>::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyContextData2Buf<T1, T2> data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
    	{
    	data.Close();
    	return EFalse;
    	}        
    data.PushL();
	TMockLtsyContextData2* dataPtr = static_cast< TMockLtsyContextData2<T1, T2>* >( &data );
    TBool result = (this->iContextConfigurationType == dataPtr->iContextConfigurationType) && 
                    TComparator<RPacketContext::TContextConfigGPRS>::
                    	IsEqual(this->iConfig, dataPtr->iConfig) && 
                    TComparator<T2>::IsEqual(this->iData2, dataPtr->iData2) &&
                    TComparator<T1>::IsEqual(this->iData1, dataPtr->iData1) ;

	CleanupStack::PopAndDestroy(&data);
	return result;
    }

/**
TMockLtsyContextData2Buf constructor
*/
template <class T1, class T2> 
TMockLtsyContextData2Buf<T1, T2>::TMockLtsyContextData2Buf()
    : TMockLtsyContextData2<T1, T2>(iData1Own, iData2Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
        TInitialiser<T2>::Initialise(iData2Own);
        }
        
/**
TMockLtsyContextData2Buf constructor
*/
template <class T1, class T2> 
TMockLtsyContextData2Buf<T1, T2>::TMockLtsyContextData2Buf(
                                             RPacketContext::TContextConfigGPRS& aConfig,
	                                         TInt aContextConfigurationType)
    : TMockLtsyContextData2<T1, T2>(aConfig, aContextConfigurationType, 
                                               iData1Own, iData2Own)
        {
        TInitialiser<T1>::Initialise(iData1Own);
        TInitialiser<T2>::Initialise(iData2Own);
        }

//**************************************************************

/**
TComparator Specialisation for CArrayFixFlat<RPacketContext::TPacketFilterV2>*
*/
template<>
class TComparator<CArrayFixFlat<RPacketContext::TPacketFilterV2>*>
    {
public: 
    IMPORT_C static TBool TComparator<CArrayFixFlat<RPacketContext::TPacketFilterV2>*>::IsEqual(const CArrayFixFlat<RPacketContext::TPacketFilterV2>* aData1, const CArrayFixFlat<RPacketContext::TPacketFilterV2>* aData2);
    };

    
/**
TComparator Specialisation for RMobileONStore::TMobileONEntryV1Pckg*
*/
template<>
class TComparator<RMobileONStore::TMobileONEntryV1Pckg*>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileONStore::TMobileONEntryV1Pckg*>::IsEqual(const RMobileONStore::TMobileONEntryV1Pckg* aData1, RMobileONStore::TMobileONEntryV1Pckg* aData2);
    };

/**
TComparator Specialisation for RMobileONStore::TMobileONEntryV1
*/
template<>
class TComparator<RMobileONStore::TMobileONEntryV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileONStore::TMobileONEntryV1>::IsEqual(const RMobileONStore::TMobileONEntryV1 aData1, RMobileONStore::TMobileONEntryV1 aData2);
    };


/**
TComparator Specialisation for RCall::TCommPort
*/
template<>
class TComparator<RCall::TCommPort>
    {
public: 
    IMPORT_C static TBool TComparator<RCall::TCommPort>::IsEqual(const RCall::TCommPort aData1, const RCall::TCommPort aData2);
    };

/**
TComparator Specialisation for RMobilePhone::::TMobilePhonePasswordChangeV2
*/
template<>
class TComparator<RMobilePhone::TMobilePhonePasswordChangeV2>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhonePasswordChangeV2>::IsEqual(const RMobilePhone::TMobilePhonePasswordChangeV2 aData1, RMobilePhone::TMobilePhonePasswordChangeV2 aData2);
    };

/**
TComparator Specialisation for RMobilePhone::::TMobilePhonePasswordChangeV2
*/
template<>
class TComparator<TLockSettingPassword>
    {
public: 
    IMPORT_C static TBool TComparator<TLockSettingPassword>::IsEqual(const TLockSettingPassword& aData1, const TLockSettingPassword& aData2);
    };

/**
TConstructor specialisation for RMobilePhone::TMultimodeType
*/
template <>
class TConstructor<RMobilePhone::TMultimodeType>
    {
public:
    IMPORT_C static void ConstructL(RMobilePhone::TMultimodeType*& aData);    
    };

/**
TDestroyer specialisation for RMobilePhone::TMultimodeType*
*/
template <>
class TDestroyer<RMobilePhone::TMultimodeType*>
    {
public:
    IMPORT_C static void Destroy(RMobilePhone::TMultimodeType* aData);    
    };


//*********************************************

/**
Class to compare 2 objects specialisation for CArrayFixFlat<T1>*
*/
template <class T1>
class TComparator<CArrayFixFlat<T1>*>
	{
public: 
	static TBool IsEqual(const CArrayFixFlat<T1>* aData1, const CArrayFixFlat<T1>* aData2)
		{
		TBool res(EFalse);
		
		if( aData1 == aData2 )
			{
			res = ETrue;
			}
		else if ( aData1 && aData2 )
			{
			res = (aData1->Count() == aData2->Count());
			for (TInt i = 0; res && i<aData1->Count(); i++)
				{
				res = TComparator<T1>::IsEqual((*aData1)[i], (*aData2)[i]);
				}
			}
		
		return res;
		}
	};

/**
Class to compare 2 objects specialisation for CArrayPtrFlat<T1>*
*/
template <class T1>
class TComparator<CArrayPtrFlat<T1>*>
	{
public: 
	static TBool IsEqual(const CArrayPtrFlat<T1>* aData1, const CArrayPtrFlat<T1>* aData2)
		{
		TBool res(EFalse);
		
		if( aData1 == aData2 )
			{
			res = ETrue;
			}
		else if ( aData1 && aData2 )
			{
			res = (aData1->Count() == aData2->Count());
			for (TInt i = 0; res && i<aData1->Count(); i++)
				{
				res = TComparator<T1>::IsEqual(*((*aData1)[i]), *((*aData2)[i]));
				}
			}
		
		return res;
		}
	};

//*********************************************

/**
TSerializer specialisation for CArrayPtrFlat<T>*
*/
template <class T>
class TSerializer<CArrayPtrFlat<T>*>
    {
public:
    static void SerialiseL(const CArrayPtrFlat<T>* aData, RBuf8& aBuffer);
    static void DeserialiseL(const TDesC8& aPackedData, CArrayPtrFlat<T>*& aData);
    };

template <class T>
void TSerializer<CArrayPtrFlat<T>*>::SerialiseL(const CArrayPtrFlat<T>* aData, RBuf8& aBuffer)
	{
	if ( aData )
		{
		TInt newBufSize( sizeof(TUint) );
		aBuffer.CreateL(newBufSize);

		TUint dataSize = aData->Count();
		TPckg<TUint> sizeData(dataSize);
		aBuffer.Append(sizeData);

		for ( TInt i = 0 ; i < aData->Count() ; i++ )
			{
			RBuf8 entry;
			CleanupClosePushL(entry);
			TSerializer<T>::SerialiseL(*((*aData)[i]), entry);

			newBufSize += sizeof(TUint) + entry.Size();
			aBuffer.ReAllocL( newBufSize );

			dataSize = entry.Size();
			aBuffer.Append( sizeData );
			aBuffer.Append( entry );

			CleanupStack::PopAndDestroy( &entry );
			}
		}
	else
		{
		aBuffer.CreateL(KNullDesC8);
		}
	}

template <class T>
void TSerializer<CArrayPtrFlat<T>*>::DeserialiseL(const TDesC8& aPackedData, CArrayPtrFlat<T>*& aData)
	{
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	TDestroyer<CArrayPtrFlat<T>*>::Destroy(aData);
    TInitialiser<CArrayPtrFlat<T>*>::Initialise(aData);
	
	if ( aPackedData.Size() )
		{
		TUint noEntries;
		TPckg<TUint> noEntriesData(noEntries);
		noEntriesData.Copy( aPackedData.Left(uintLen) );
		cursor += uintLen;
	
		aData = new ( ELeave ) CArrayPtrFlat<T>( 1 );
		
		for ( TUint i = 0 ; i < noEntries ; i++ )
			{
			T* entry(NULL);
			TConstructor<T>::ConstructL(entry);
			CleanupStack::PushL( TCleanupItem(TCleaner<T>::Cleanup, entry) );
			
			TUint entrySize(0);
			TPckg<TUint> entrySizeData(entrySize);
			entrySizeData.Copy( aPackedData.Mid(cursor, uintLen) );
			cursor += uintLen;
			
		    TSerializer<T>::DeserialiseL(aPackedData.Mid(cursor, entrySize), *entry);
			cursor += entrySize;

			aData->AppendL(entry);
			CleanupStack::Pop( entry );
			}
		}
	}

/**
TDestroyer specialisation for CArrayPtrFlat<T>*
*/
template <class T>
class TDestroyer<CArrayPtrFlat<T>*>
    {
public:
    static void Destroy(CArrayPtrFlat<T>*& aData)
		{
	    if (aData)
	    	{
	    	aData->ResetAndDestroy();
	    	delete aData;
	    	aData = NULL;
	    	}
		}
    };

//*********************************************

/**
TSerializer specialisation for CArrayFixFlat<T>*
*/
template <class T>
class TSerializer<CArrayFixFlat<T>*>
    {
public:
    static void SerialiseL(const CArrayFixFlat<T>* aData, RBuf8& aBuffer);
    static void DeserialiseL(const TDesC8& aPackedData, CArrayFixFlat<T>*& aData);
    };

template <class T>
void TSerializer<CArrayFixFlat<T>*>::SerialiseL(const CArrayFixFlat<T>* aData, RBuf8& aBuffer)
	{
	if ( aData )
		{
		TInt newBufSize( sizeof(TUint) );
		aBuffer.CreateL(newBufSize);

		TUint dataSize = aData->Count();
		TPckg<TUint> sizeData(dataSize);
		aBuffer.Append(sizeData);

		for ( TInt i = 0 ; i < aData->Count() ; i++ )
			{
			RBuf8 entry;
			CleanupClosePushL(entry);
			TSerializer<T>::SerialiseL((*aData)[i], entry);

			newBufSize += sizeof(TUint) + entry.Size();
			aBuffer.ReAllocL( newBufSize );

			dataSize = entry.Size();
			aBuffer.Append( sizeData );
			aBuffer.Append( entry );

			CleanupStack::PopAndDestroy( &entry );
			}
		}
	else
		{
		aBuffer.CreateL(KNullDesC8);
		}
	}

template <class T>
void TSerializer<CArrayFixFlat<T>*>::DeserialiseL(const TDesC8& aPackedData, CArrayFixFlat<T>*& aData)
	{
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	TDestroyer<CArrayFixFlat<T>*>::Destroy(aData);
    TInitialiser<CArrayFixFlat<T>*>::Initialise(aData);
	
	if ( aPackedData.Size() )
		{
		TUint noEntries;
		TPckg<TUint> noEntriesData(noEntries);
		noEntriesData.Copy( aPackedData.Left(uintLen) );
		cursor += uintLen;
	
		aData = new ( ELeave ) CArrayFixFlat<T>( 1 );
		
		for ( TUint i = 0 ; i < noEntries ; i++ )
			{
			T* entry(NULL);
			TConstructor<T>::ConstructL(entry);
			CleanupStack::PushL( TCleanupItem(TCleaner<T>::Cleanup, entry) );
			
			TUint entrySize(0);
			TPckg<TUint> entrySizeData(entrySize);
			entrySizeData.Copy( aPackedData.Mid(cursor, uintLen) );
			cursor += uintLen;
			
		    TSerializer<T>::DeserialiseL(aPackedData.Mid(cursor, entrySize), *entry);
			cursor += entrySize;

			aData->AppendL(*entry);
			CleanupStack::PopAndDestroy( entry );
			}
		}
	}

/**
TDestroyer specialisation for CArrayFixFlat<T>*
*/
template <class T>
class TDestroyer<CArrayFixFlat<T>*>
    {
public:
    static void Destroy(CArrayFixFlat<T>*& aData)
		{
	    if (aData)
	    	{
	    	aData->Reset();
	    	delete aData;
	    	aData = NULL;
	    	}
		}
    };
//*********************************************

/**
TSerializer specialisation for T*
*/
template <class T>
class TSerializer<T*>
    {
public:
    static void SerialiseL(const T* aData, RBuf8& aBuffer);
    static void DeserialiseL(const TDesC8& aPackedData, T*& aData);
    };

template <class T>
void TSerializer<T*>::SerialiseL(const T* aData, RBuf8& aBuffer)
    {
    if ( aData )
        {
        TSerializer<T>::SerialiseL(*aData, aBuffer);
        }
    else
        {
        aBuffer.CreateL(KNullDesC8);
        }
    }

template <class T>
void TSerializer<T*>::DeserialiseL(const TDesC8& aPackedData, T*& aData)
    {
    TDestroyer<T*>::Destroy(aData);
    TInitialiser<T*>::Initialise(aData);

    if ( aPackedData.Size())
        {
        TConstructor<T>::ConstructL(aData);
        CleanupStack::PushL( TCleanupItem(TCleaner<T>::Cleanup, aData));

        TSerializer<T>::DeserialiseL(aPackedData, *aData);
        CleanupStack::Pop( aData ); 
        
        }
    }


//*********************************************

/**
TConstructor specialisation for RPacketContext::CTFTMediaAuthorizationV3
*/
template <>
class TConstructor<RPacketContext::CTFTMediaAuthorizationV3>
    {
public:
    IMPORT_C static void ConstructL(RPacketContext::CTFTMediaAuthorizationV3*& aData);    
    };

/**
TSerializer specialisation for RPacketContext::CTFTMediaAuthorizationV3
*/
template <>
class TSerializer<RPacketContext::CTFTMediaAuthorizationV3>
    {
public:
    IMPORT_C static void SerialiseL(const RPacketContext::CTFTMediaAuthorizationV3& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, RPacketContext::CTFTMediaAuthorizationV3& aData);
    };
    
/**
TSerializer specialisation for CPhoneBookStoreEntry*
*/
template <>
class TSerializer<CPhoneBookStoreEntry*>
    {
public:
    IMPORT_C static void SerialiseL(const CPhoneBookStoreEntry* aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CPhoneBookStoreEntry*& aData);
    };
    
/**
TDestroyer specialisation for CPhoneBookStoreEntry*
*/
template <>
class TDestroyer<CPhoneBookStoreEntry*>
    {
public:
    IMPORT_C static void Destroy(CPhoneBookStoreEntry*& aData);    
    };

    
/**
TSerializer specialisation for CArrayPtrSeg<CPhoneBookStoreEntry>*
*/
template <>
class TSerializer<CArrayPtrSeg<CPhoneBookStoreEntry>*>
    {
public:
    IMPORT_C static void SerialiseL(const CArrayPtrSeg<CPhoneBookStoreEntry>* aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CArrayPtrSeg<CPhoneBookStoreEntry>*& aData);
    };
 
/**
TSerializer specialisation for TContextConfiguration
*/
template <>
class TSerializer<TContextConfiguration>
    {
public:
    IMPORT_C static void SerialiseL(const TContextConfiguration& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, TContextConfiguration& aData);
    }; 
    
/**
TSerializer specialisation for TContextConfiguration
*/
template <>
class TSerializer<RMmCustomAPI::TOperatorLogo>
    {
public:
    IMPORT_C static void SerialiseL(const RMmCustomAPI::TOperatorLogo& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, RMmCustomAPI::TOperatorLogo& aData);
    }; 

/**
TDestroyer specialisation for TContextConfiguration
*/
template <>
class TDestroyer<RMmCustomAPI::TOperatorLogo>
    {
public:
    IMPORT_C static void Destroy(RMmCustomAPI::TOperatorLogo& aData);    
    }; 


/**
TDestroyer specialisation for TContextConfiguration
*/
template <>
class TDestroyer<TContextConfiguration>
    {
public:
    IMPORT_C static void Destroy(TContextConfiguration& aData);    
    }; 
    
  
/**
TSerializer specialisation for TMmMbmsMonitorServiceListParams
*/
template <>
class TSerializer<TMmMbmsMonitorServiceListParams>
    {
public:
    IMPORT_C static void SerialiseL(const TMmMbmsMonitorServiceListParams& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, TMmMbmsMonitorServiceListParams& aData);
    }; 
    
/**
TDestroyer specialisation for TMmMbmsMonitorServiceListParams
*/
template <>
class TDestroyer<TMmMbmsMonitorServiceListParams>
    {
public:
    IMPORT_C static void Destroy(TMmMbmsMonitorServiceListParams& aData);    
    };   
  
   
    
/**
TDestroyer specialisation for CArrayPtrSeg<CPhoneBookStoreEntry>*
*/
template <>
class TDestroyer<CArrayPtrSeg<CPhoneBookStoreEntry>*>
    {
public:
    IMPORT_C static void Destroy(CArrayPtrSeg<CPhoneBookStoreEntry>*& aData);    
    };

/**
TDestroyer specialisation for TDesC8*
*/
template <>
class TDestroyer<TDesC8*>
    {
public:
    IMPORT_C static void Destroy(TDesC8* aData);    
    };

/**
TDestroyer specialisation for TSendSmsDataAndAttributes
*/
template <>
class TDestroyer<TSendSmsDataAndAttributes>
    {
public:
    IMPORT_C static void Destroy(TSendSmsDataAndAttributes& aData);    
    };


/**
TInitialiser specialisation for CArrayPtrFlat<T>*
*/
template <>
class TInitialiser<TSendSmsDataAndAttributes>
    {
public:
    static void Initialise(TSendSmsDataAndAttributes& aData)
	    {
   	   	aData.iAttributes = NULL;
   	   	aData.iMsgData = NULL;
    	}
    };


/**
TSerializer specialisation for CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>*
*/
template <>
class TSerializer<RPacketContext::CTFTMediaAuthorizationV3*>
    {
public:
    IMPORT_C static void SerialiseL(const RPacketContext::CTFTMediaAuthorizationV3* aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, RPacketContext::CTFTMediaAuthorizationV3*& aData);
    };
    
/**
TSerializer specialisation for CMobilePhoneNetworkListV2*
*/
template <>
class TSerializer<CMobilePhoneNetworkListV2*>
    {
public:
    IMPORT_C static void SerialiseL(const CMobilePhoneNetworkListV2* aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CMobilePhoneNetworkListV2*& aData);
    };
/**
TSerializer specialisation for TSendSmsDataAndAttributes
*/
template <>
class TSerializer<TSendSmsDataAndAttributes>
    {
public:
    IMPORT_C static void SerialiseL(const TSendSmsDataAndAttributes aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, TSendSmsDataAndAttributes& aData);
    };


/**
TSerializer specialisation for TDesC8*
*/
template <>
class TSerializer<TDesC8*>
    {
public:
    IMPORT_C static void SerialiseL(const TDesC8* aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, TDesC8*& aData);
    };

//*********************************************

/**
TComparator Specialisation for RMobilePhone::TMobilePhoneNetworkManualSelection
*/
template<>
class TComparator<RMobilePhone::TMobilePhoneNetworkManualSelection>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhoneNetworkManualSelection>::
    	IsEqual(const RMobilePhone::TMobilePhoneNetworkManualSelection& aData1, 
    	        const RMobilePhone::TMobilePhoneNetworkManualSelection& aData2);
    };

/**
TComparator Specialisation for TDesC8*
*/
template<>
class TComparator<TDesC8*>
    {
public: 
    IMPORT_C static TBool TComparator<TDesC8*>::
    	IsEqual(const TDesC8* aData1, const TDesC8* aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TMobileName
*/
template<>
class TComparator<RMobilePhone::TMobileName>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobileName>::IsEqual(const RMobilePhone::TMobileName& aData1, const RMobilePhone::TMobileName& aData2);
    };


/**
TConstructor specialisation for CMobilePhoneBroadcastIdList
*/
template <>
class TConstructor<CMobilePhoneBroadcastIdList>
    {
public:
    IMPORT_C static void ConstructL(CMobilePhoneBroadcastIdList*& aData);    
    };

/**
TSerializer specialisation for CMobilePhoneBroadcastIdList
*/
template <>
class TSerializer<CMobilePhoneBroadcastIdList>
    {
public:
    IMPORT_C static void SerialiseL(const CMobilePhoneBroadcastIdList& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CMobilePhoneBroadcastIdList& aData);
    };


/**
Class to compare 2 objects specialisation for CMobilePhoneEditableList<T1>*
*/
template <class T1>
class TComparator<CMobilePhoneEditableList<T1>*>
	{
public: 
	static TBool IsEqual(const CMobilePhoneEditableList<T1>* aData1, const CMobilePhoneEditableList<T1>* aData2)
		{
		TBool res(EFalse);
		
		if( aData1 == aData2 )
			{
			res = ETrue;
			}
		else if ( aData1 && aData2 )
			{
			res = (aData1->Enumerate() == aData2->Enumerate());
			
			TInt count = aData1->Enumerate();
			for (TInt i = 0; res && i<count; i++)
				{
				res = TComparator<T1>::IsEqual( aData1->GetEntryL(i), aData2->GetEntryL(i) );
				}
			
			}
		
		return res;
		}
	};

/**
TComparator Specialisation for CMobilePhoneBroadcastIdList
*/
template<>
class TComparator<CMobilePhoneBroadcastIdList>
    {
public: 
    IMPORT_C static TBool TComparator<CMobilePhoneBroadcastIdList>::
        IsEqual(const CMobilePhoneBroadcastIdList& aData1,
                const CMobilePhoneBroadcastIdList& aData2);
    };

/**
TComparator Specialisation for RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1
*/
template<>
class TComparator<RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1>::
        IsEqual(const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aData1,
                const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aData2);
    };

/**
TComparator Specialisation for TSendSmsDataAndAttributes
*/
template<>
class TComparator<TSendSmsDataAndAttributes>
    {
public: 
    IMPORT_C static TBool TComparator<TSendSmsDataAndAttributes>::
    	IsEqual(const TSendSmsDataAndAttributes& aData1, 
    	        const TSendSmsDataAndAttributes& aData2);
    };



/**
TConstructor specialisation for CMobilePhoneSmspList
*/
template <>
class TConstructor<CMobilePhoneSmspList>
    {
public:
    IMPORT_C static void ConstructL(CMobilePhoneSmspList*& aData);    
	};


/**
TComparator Specialisation for RMobilePhone::TAID
*/
template<>
class TComparator<RMobilePhone::TAID>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TAID>::
    	IsEqual(const RMobilePhone::TAID& aData1, 
    	        const RMobilePhone::TAID& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TAuthInfoV8
*/
template<>
class TComparator<RMobilePhone::TAuthInfoV8>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TAuthInfoV8>::
    	IsEqual(const RMobilePhone::TAuthInfoV8& aData1, 
    	        const RMobilePhone::TAuthInfoV8& aData2);
    };



/**
TComparator Specialisation for CMobilePhoneListBase
*/
template<>
class TComparator<CMobilePhoneListBase>
    {
public: 
    IMPORT_C static TBool TComparator<CMobilePhoneListBase>::
    	IsEqual(const CMobilePhoneListBase& aData1, 
    	        const CMobilePhoneListBase& aData2);
    };

/**
TSerializer specialisation for CMobilePhoneListBase
*/
template <>
class TSerializer<CMobilePhoneListBase>
    {
public:
    IMPORT_C static void SerialiseL(const CMobilePhoneListBase& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CMobilePhoneListBase& aData);
    };

/**
TComparator Specialisation for CMobilePhoneCWList
*/
template<>
class TComparator<CMobilePhoneCWList>
    {
public: 
    IMPORT_C static TBool TComparator<CMobilePhoneCWList>::
    	IsEqual(const CMobilePhoneCWList& aData1, 
    	        const CMobilePhoneCWList& aData2);
    };

/**
TSerializer specialisation for CMobilePhoneCWList
*/
template <>
class TSerializer<CMobilePhoneCWList>
    {
public:
    IMPORT_C static void SerialiseL(const CMobilePhoneCWList& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CMobilePhoneCWList& aData);
    };

/**
TConstructor specialisation for CMobilePhoneCWList
*/
template <>
class TConstructor<CMobilePhoneCWList>
    {
public:
    IMPORT_C static void ConstructL(CMobilePhoneCWList*& aData);    
    };

/**
TConstructor specialisation for CMobilePhoneCFList
*/
template <>
class TConstructor<CMobilePhoneCFList>
    {
public:
    IMPORT_C static void ConstructL(CMobilePhoneCFList*& aData);    
    };

/**
TComparator Specialisation for CMobilePhoneCFList
*/
template<>
class TComparator<CMobilePhoneCFList>
    {
public: 
    IMPORT_C static TBool TComparator<CMobilePhoneCFList>::
    	IsEqual(const CMobilePhoneCFList& aData1, 
    	        const CMobilePhoneCFList& aData2);
    };

/**
TSerializer specialisation for CMobilePhoneCFList
*/
template <>
class TSerializer<CMobilePhoneCFList>
    {
public:
    IMPORT_C static void SerialiseL(const CMobilePhoneCFList& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CMobilePhoneCFList& aData);
    };

//*********************************************

/**
TComparator Specialisation for RMmCustomAPI::TApdu
*/
template<>
class TComparator<RMmCustomAPI::TApdu>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TApdu>::
    	IsEqual(const RMmCustomAPI::TApdu& aData1, const RMmCustomAPI::TApdu& aData2);
    };

/**
TComparator Specialisation for RMmCustomAPI::TApduParameters
*/
template<>
class TComparator<RMmCustomAPI::TApduParameters>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TApduParameters>::
        IsEqual(const RMmCustomAPI::TApduParameters& aData1, const RMmCustomAPI::TApduParameters& aData2);
    };


/**
TComparator Specialisation for RMmCustomAPI::TEmerNumberCheckMode
*/
template<>
class TComparator<RMmCustomAPI::TEmerNumberCheckMode>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TEmerNumberCheckMode>::
    	IsEqual(const RMmCustomAPI::TEmerNumberCheckMode& aData1, const RMmCustomAPI::TEmerNumberCheckMode& aData2);
    };


/**
TComparator Specialisation for RMmCustomAPI::TSimLockPassword
*/
template<>
class TComparator<RMmCustomAPI::TSimLockPassword>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimLockPassword>::IsEqual(const RMmCustomAPI::TSimLockPassword& aData1, const RMmCustomAPI::TSimLockPassword& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TMobilePassword
*/
template<>
class TComparator<RMobilePhone::TMobilePassword>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePassword>::IsEqual(const RMobilePhone::TMobilePassword& aData1, const RMobilePhone::TMobilePassword& aData2);
    };

/**
TComparator Specialisation for RMmCustomAPI::TViagCacheRecordId
*/
template<>
class TComparator<RMmCustomAPI::TViagCacheRecordId>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TViagCacheRecordId>::
        IsEqual(const RMmCustomAPI::TViagCacheRecordId& aData1,
                const RMmCustomAPI::TViagCacheRecordId& aData2);
    };

/**
TComparator Specialisation for RMmCustomAPI::TViagCacheRecordId
*/
template<>
class TComparator<RMmCustomAPI::TViagCacheRecordContent>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TViagCacheRecordContent>::
        IsEqual(const RMmCustomAPI::TViagCacheRecordContent& aData1,
                const RMmCustomAPI::TViagCacheRecordContent& aData2);
	};

/**
TComparator Specialisation for RMmCustomAPI::TMmCellInfo
*/
template<>
class TComparator<RMmCustomAPI::TMmCellInfo>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TMmCellInfo>::
        IsEqual(const RMmCustomAPI::TMmCellInfo& aData1,
                const RMmCustomAPI::TMmCellInfo& aData2);
	};
	
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationEapSim
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationEapSim>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationEapSim>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationEapSim& aData1,
                const RMmCustomAPI::TSimAuthenticationEapSim& aData2);
	};
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationEapAka
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationEapAka>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationEapAka>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationEapAka& aData1,
                const RMmCustomAPI::TSimAuthenticationEapAka& aData2);
	};

/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationGbaBootstrap
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationGbaBootstrap>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationGbaBootstrap>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationGbaBootstrap& aData1,
                const RMmCustomAPI::TSimAuthenticationGbaBootstrap& aData2);
	};
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate& aData1,
                const RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate& aData2);
	};
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationGbaNafDerivation
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationGbaNafDerivation>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationGbaNafDerivation>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationGbaNafDerivation& aData1,
                const RMmCustomAPI::TSimAuthenticationGbaNafDerivation& aData2);
	};
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationMgvMskUpdate
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationMgvMskUpdate>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationMgvMskUpdate>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aData1,
                const RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aData2);
	};
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationMgvMtkGeneration
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationMgvMtkGeneration>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationMgvMtkGeneration>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aData1,
                const RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aData2);
	};
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationMgvMskDeletion
*/
template<>
class TComparator<RMmCustomAPI::TSimAuthenticationMgvMskDeletion>
    {
public: 
    IMPORT_C static TBool TComparator<RMmCustomAPI::TSimAuthenticationMgvMskDeletion>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationMgvMskDeletion& aData1,
                const RMmCustomAPI::TSimAuthenticationMgvMskDeletion& aData2);
	};

/**	
TComparator Specialisation for RMobilePhone::TMobilePhoneCspFileV1
*/
template<>
class TComparator<RMobilePhone::TMobilePhoneCspFileV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhoneCspFileV1>::
    	IsEqual(const RMobilePhone::TMobilePhoneCspFileV1& aData1, 
    	        const RMobilePhone::TMobilePhoneCspFileV1& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TMobilePhoneServiceTableV1
*/
template<>
class TComparator<RMobilePhone::TMobilePhoneServiceTableV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhoneServiceTableV1>::
    	IsEqual(const RMobilePhone::TMobilePhoneServiceTableV1& aData1, 
    	        const RMobilePhone::TMobilePhoneServiceTableV1& aData2);
    };

// ISO C++ compliant version of ADD_TEST_STEP which uses correct member function
// address syntax (e.g., &ClassName::MemberName rather than &MemberName).  This
// is necessary to compile without errors for the x86gcc target.
#define ADD_TEST_STEP_ISO_CPP(suiteClassName, stepName) \
	_LIT( KTest ## stepName, #stepName); \
	AddTestCase(lTestSuite, KTest ## stepName, &suiteClassName::stepName);
	
/**
TComparator Specialisation for TMbmsAction
*/
template <>
class TComparator<TMbmsAction>
    {
public: 
    IMPORT_C static TBool TComparator<TMbmsAction>::
        IsEqual(const TMbmsAction& aData1,
                const TMbmsAction& aData2);
    };

/**
TComparator Specialisation for TMbmsNetworkServiceStatus
*/

template <>
class TComparator<TMbmsNetworkServiceStatus>
    {
public: 
    IMPORT_C static TBool TComparator<TMbmsNetworkServiceStatus>::
        IsEqual(const TMbmsNetworkServiceStatus& aData1,
                const TMbmsNetworkServiceStatus& aData2);
    };
    
/**
TConstructor specialisation for CPcktMbmsMonitoredServiceList
*/
template <>
class TConstructor<CPcktMbmsMonitoredServiceList>
    {
public:
    IMPORT_C static void ConstructL(CPcktMbmsMonitoredServiceList*& aData);    
    };


/**
TSerializer specialisation for CPcktMbmsMonitoredServiceList
*/
template <>
class TSerializer<CPcktMbmsMonitoredServiceList>
    {
public:
    IMPORT_C static void SerialiseL(const CPcktMbmsMonitoredServiceList& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CPcktMbmsMonitoredServiceList& aData);
    };

/**
TConstructor specialisation for RPacketMbmsContext::CMbmsSession
*/
template <>
class TConstructor<RPacketMbmsContext::CMbmsSession>
    {
public:
    IMPORT_C static void ConstructL(RPacketMbmsContext::CMbmsSession*& aData);    
    };


/**
TSerializer specialisation for RPacketMbmsContext::CMbmsSession
*/
template <>
class TSerializer<RPacketMbmsContext::CMbmsSession>
    {
public:
    IMPORT_C static void SerialiseL(const RPacketMbmsContext::CMbmsSession& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, RPacketMbmsContext::CMbmsSession& aData);
    };

/**
TComparator Specialisation for TMmMbmsActiveServiceListParams
*/

template <>
class TComparator<TMmMbmsActiveServiceListParams>
    {
public: 
    IMPORT_C static TBool TComparator<TMmMbmsActiveServiceListParams>::
        IsEqual(const TMmMbmsActiveServiceListParams& aData1,
                const TMmMbmsActiveServiceListParams& aData2);
    };
    

/**
TComparator Specialisation for TMmMbmsMonitorServiceListParams
*/

template <>
class TComparator<TMmMbmsMonitorServiceListParams>
    {
public: 
    IMPORT_C static TBool TComparator<TMmMbmsMonitorServiceListParams>::
        IsEqual(const TMmMbmsMonitorServiceListParams& aData1,
                const TMmMbmsMonitorServiceListParams& aData2);
    };
    

/**
TSerializer specialisation for RPacketMbmsContext::CMbmsSession
*/
template <>
class TSerializer<TMmMbmsActiveServiceListParams>
    {
public:
    IMPORT_C static void SerialiseL(const TMmMbmsActiveServiceListParams& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, TMmMbmsActiveServiceListParams& aData);
    };

/**
TComparator specialisation for TDesC8
*/

template <>
class TComparator<TDesC8>
    {
public:
    static TBool IsEqual(const TDesC8& aData1, const TDesC8& aData2)
	    {
	    return 0==aData1.Compare(aData2);                
	    }    
    };

/**
TComparator specialisation for TDesC16
*/

template <>
class TComparator<TDesC16>
    {
public:
    static TBool IsEqual(const TDesC16& aData1, const TDesC16& aData2)
	    {
	    return 0==aData1.Compare(aData2);                
	    }    
    };

/**
TComparator specialisation for TPtrC8
*/

template <>
class TComparator<TPtrC8>
    {
public:
    static TBool IsEqual(const TPtrC8& aData1, const TPtrC8& aData2)
	    {
	    return TComparator<TDesC8>::IsEqual(aData1,aData2);
	    }    
    };


/**
TComparator specialisation for TBuf8
*/

template <>
class TComparator<TBufBase8>
    {
public:
    static TBool IsEqual(const TBufBase8& aData1, const TBufBase8& aData2)
	    {
	    return TComparator<TDesC8>::IsEqual(aData1,aData2);
	    }    
    };

/**
TComparator specialisation for TBuf16
*/

template <>
class TComparator<TBufBase16>
    {
public:
    static TBool IsEqual(const TBufBase16& aData1, const TBufBase16& aData2)
	    {
	    return TComparator<TDesC16>::IsEqual(aData1,aData2);
	    }    
    };

/**
TComparator specialisation for TBuf for any size
*/

template <TInt SIZE>
class TComparator<TBuf<SIZE> >
    {
public:
    static TBool IsEqual(const TBuf<SIZE>& aData1, const TBuf<SIZE>& aData2)
	    {
	    return TComparator<TDesC16>::IsEqual(aData1,aData2);
	    }    
    };

/**
TComparator specialisation for TBuf8 for any size
*/

template <TInt SIZE>
class TComparator<TBuf8<SIZE> >
    {
public:
    static TBool IsEqual(const TBuf8<SIZE>& aData1, const TBuf8<SIZE>& aData2)
	    {
	    return TComparator<TDesC8>::IsEqual(aData1,aData2);
	    }    
    };


//Symbian does not provide a typedef for TBufBase -> TBufBase8/TBufBase16. 
//Unfortunately one of the member variables of TTerminalRespData is defined as 
//a TBuf so could either be a TBufBase8 or a TBufBase16 depending on build environment.
//solution is to define a TBufBase typedef in the same way as TBuf is defined in order
//to properly build a generic comparator for TTerminalRespData.
#if defined(_UNICODE) && !defined(__KERNEL_MODE__)
typedef TBufBase16 TBufBase;
#else
typedef TBufBase8 TBufBase;
#endif

/**
TComparator specialisation for RMobileUssdMessaging::TMobileUssdAttributesV1
*/

template <>
class TComparator<RMobileUssdMessaging::TMobileUssdAttributesV1>
    {
public:
    IMPORT_C static TBool IsEqual(const RMobileUssdMessaging::TMobileUssdAttributesV1& aData1, const RMobileUssdMessaging::TMobileUssdAttributesV1& aData2);
    };

/**
TComparator specialisation for RMobilePhone::TMobilePhoneCBChangeV1
*/

template <>
class TComparator<RMobilePhone::TMobilePhoneCBChangeV1>
    {
public:
    IMPORT_C static TBool IsEqual(const RMobilePhone::TMobilePhoneCBChangeV1& aData1, const RMobilePhone::TMobilePhoneCBChangeV1& aData2);
    };

/**
TSerializer specialisation for CMobilePhoneBroadcastIdList
*/
template <>
class TSerializer<CMobilePhoneSmspList>
    {
public:
    IMPORT_C static void SerialiseL(const CMobilePhoneSmspList& aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, CMobilePhoneSmspList& aData);
    };

/**
TComparator Specialisation for CMobilePhoneBroadcastIdList
*/
template<>
class TComparator<CMobilePhoneSmspList>
    {
public: 
    IMPORT_C static TBool TComparator<CMobilePhoneSmspList>::
        IsEqual(const CMobilePhoneSmspList& aData1,
                const CMobilePhoneSmspList& aData2);
    };

/**
TComparator Specialisation for RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1
*/
template<>
class TComparator<RMobileSmsMessaging::TMobileSmspEntryV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileSmsMessaging::TMobileSmspEntryV1>::
        IsEqual(const RMobileSmsMessaging::TMobileSmspEntryV1& aData1,
                const RMobileSmsMessaging::TMobileSmspEntryV1& aData2);
    };

/**
TComparator specialisation for TName
*/
template <>
class TComparator<TName>
    {
public:
    static TBool IsEqual(const TName& aData1, const TName& aData2)
	    {
	    return 0 == aData1.Compare(aData2);
	    }
	};

/**
TComparator Specialisation for RMobilePhone::TMobileAddress
*/
template<>
class TComparator<RMobilePhone::TMobileAddress>
    {
public: 
	IMPORT_C static TBool TComparator<RMobilePhone::TMobileAddress>::IsEqual(const RMobilePhone::TMobileAddress& aData1, const RMobilePhone::TMobileAddress& aData2);
    };

/**
TComparator Specialisation for RPacketContext::TMiscProtocolBuffer
*/
template<>
class TComparator<RPacketContext::TMiscProtocolBuffer>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::TMiscProtocolBuffer>::
            IsEqual(const RPacketContext::TMiscProtocolBuffer& aData1, 
                    const RPacketContext::TMiscProtocolBuffer& aData2);
    };

/**
TComparator Specialisation for RPacketContext::TGSNAddress
*/
template<>
class TComparator<RPacketContext::TGSNAddress>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::TGSNAddress>::
            IsEqual(const RPacketContext::TGSNAddress& aData1, 
                    const RPacketContext::TGSNAddress& aData2);
    };

/**
TComparator Specialisation for RPacketContext::TProtocolAddress
*/
template<>
class TComparator<RPacketContext::TProtocolAddress>
    {
public: 
    IMPORT_C static TBool TComparator<RPacketContext::TProtocolAddress>::
            IsEqual(const RPacketContext::TProtocolAddress& aData1, 
                    const RPacketContext::TProtocolAddress& aData2);
    };

/**
TComparator Specialisation for RMobilePhone::TMobilePhonePasswordChangeV1
*/
template<>
class TComparator<RMobilePhone::TMobilePhonePasswordChangeV1>
    {
public: 
    IMPORT_C static TBool TComparator<RMobilePhone::TMobilePhonePasswordChangeV1>::IsEqual(const RMobilePhone::TMobilePhonePasswordChangeV1 aData1, RMobilePhone::TMobilePhonePasswordChangeV1 aData2);
    };

/**
TSerializer specialisation for TDesC16*
*/
template <>
class TSerializer<TDesC16*>
    {
public:
    IMPORT_C static void SerialiseL(const TDesC16* aData, RBuf8& aBuffer);
    IMPORT_C static void DeserialiseL(const TDesC8& aPackedData, TDesC16*& aData);
    };

/**
TComparator Specialisation for RMobileSmsMessaging::TMobileSmsSendAttributesV1
*/

template<>
class TComparator<RMobileSmsMessaging::TMobileSmsAttributesV1>
	{
public:
	IMPORT_C static TBool TComparator<RMobileSmsMessaging::TMobileSmsAttributesV1>::
		IsEqual(const RMobileSmsMessaging::TMobileSmsAttributesV1& aData1,
				const RMobileSmsMessaging::TMobileSmsAttributesV1& aData2);
	};

//*********************************************


/**
TComparator Specialisation for RMobileSmsMessaging::TMobileSmsSendAttributesV1
*/

template<>
class TComparator<RMobileSmsMessaging::TMobileSmsSendAttributesV1>
	{
public:
	IMPORT_C static TBool TComparator<RMobileSmsMessaging::TMobileSmsSendAttributesV1>::
		IsEqual(const RMobileSmsMessaging::TMobileSmsSendAttributesV1& aData1,
				const RMobileSmsMessaging::TMobileSmsSendAttributesV1& aData2);
	};

//*********************************************

/**
TComparator Specialisation for RMobilePhone::TMobileSmsGsmTpdu
*/

template<>
class TComparator<RMobileSmsMessaging::TMobileSmsGsmTpdu>
	{
public:
	IMPORT_C static TBool TComparator<RMobileSmsMessaging::TMobileSmsGsmTpdu>::
		IsEqual(const RMobileSmsMessaging::TMobileSmsGsmTpdu& aData1,
				const RMobileSmsMessaging::TMobileSmsGsmTpdu& aData2);
	};

//*********************************************

/**
TComparator Specialisation for RMobileSmsStore::TMobileGsmSmsEntryV1
*/

template<>
class TComparator<RMobileSmsStore::TMobileGsmSmsEntryV1>
	{
public:
	IMPORT_C static TBool TComparator<RMobileSmsStore::TMobileGsmSmsEntryV1>::
		IsEqual(const RMobileSmsStore::TMobileGsmSmsEntryV1& aData1,
				const RMobileSmsStore::TMobileGsmSmsEntryV1& aData2);
	};

//*********************************************
/**
TComparator Specialisation for RMobileSmsStore::TMobileSmsEntryV1
*/
template<>
class TComparator<RMobileSmsStore::TMobileSmsEntryV1>
	{
public:
	IMPORT_C static TBool TComparator<RMobileSmsStore::TMobileSmsEntryV1>::
		IsEqual(const RMobileSmsStore::TMobileSmsEntryV1& aData1,
				const RMobileSmsStore::TMobileSmsEntryV1& aData2);
	};


//*********************************************
/**
TComparator Specialisation for RMobilePhoneStore::TMobilePhoneStoreEntryV1
*/
template<>
class TComparator<RMobilePhoneStore::TMobilePhoneStoreEntryV1>
	{
public:
	IMPORT_C static TBool TComparator<RMobilePhoneStore::TMobilePhoneStoreEntryV1>::
		IsEqual(const RMobilePhoneStore::TMobilePhoneStoreEntryV1& aData1,
				const RMobilePhoneStore::TMobilePhoneStoreEntryV1& aData2);
	};


/**
Class to compare 2 objects specialisation for RArray<T1>*
*/
template <class T1>
class TComparator<RArray<T1> >
	{
public: 
	static TBool IsEqual(const RArray<T1>& aData1, const RArray<T1>& aData2)
		{
		TBool res(EFalse);
		
		if( &aData1 == &aData2 )
			{
			res = ETrue;
			}
		else if ( &aData1 && &aData2 )
			{
			res = (aData1.Count() == aData2.Count());
			for (TInt i = 0; res && i<aData1.Count(); i++)
				{
				res = TComparator<T1>::IsEqual(aData1[i], aData2[i]);
				}
			}
		
		return res;
		}
	};

/**
TSerializer specialisation for RArray<T>
*/
template <class T>
class TSerializer<RArray<T> >
    {
public:
    static void SerialiseL(const RArray<T>& aData, RBuf8& aBuffer);
    static void DeserialiseL(const TDesC8& aPackedData, RArray<T>& aData);    
    };

template <class T>
void TSerializer<RArray<T> >::SerialiseL(const RArray<T>& aData, RBuf8& aBuffer)
	{
	TInt newBufSize( sizeof(TUint) );
	aBuffer.CreateL(newBufSize);

	TUint dataSize = aData.Count();
	TPckg<TUint> sizeData(dataSize);
	aBuffer.Append(sizeData);

	for ( TInt i = 0 ; i < aData.Count() ; i++ )
		{
		RBuf8 entry;
		CleanupClosePushL(entry);
		TSerializer<T>::SerialiseL( aData[i], entry );

		newBufSize += sizeof(TUint) + entry.Size();
		aBuffer.ReAllocL( newBufSize );

		dataSize = entry.Size();
		aBuffer.Append( sizeData );
		aBuffer.Append( entry );

		CleanupStack::PopAndDestroy( &entry );
		}
	}

template <class T>
void TSerializer<RArray<T> >::DeserialiseL(const TDesC8& aPackedData, RArray<T>& aData)
	{
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);
	
	TDestroyer<RArray<T> >::Destroy(aData);
    TInitialiser<RArray<T> >::Initialise(aData);
	
	if ( aPackedData.Size() )
		{
		TUint noEntries;
		TPckg<TUint> noEntriesData(noEntries);
		noEntriesData.Copy( aPackedData.Left(uintLen) );
		cursor += uintLen;
			
		for ( TUint i = 0 ; i < noEntries ; i++ )
			{
			T* entry(NULL);
			TConstructor<T>::ConstructL(entry);
			CleanupStack::PushL( TCleanupItem(TCleaner<T>::Cleanup, entry) );
			
			TUint entrySize(0);
			TPckg<TUint> entrySizeData(entrySize);
			entrySizeData.Copy( aPackedData.Mid(cursor, uintLen) );
			cursor += uintLen;
			
		    TSerializer<T>::DeserialiseL(aPackedData.Mid(cursor, entrySize), *entry);
			cursor += entrySize;

			aData.AppendL(*entry);
			CleanupStack::Pop( entry );
			}
		}	
	}


/**
 * TComparator Specialisation for RMobileCall::TMobileCallHscsdInfoV8
 **/
template<>
class TComparator<RMobileCall::TMobileCallHscsdInfoV8>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TMobileCallHscsdInfoV8>::IsEqual(const RMobileCall::TMobileCallHscsdInfoV8& aData1, const RMobileCall::TMobileCallHscsdInfoV8& aData2);
    };

 /**
 * TComparator Specialisation for RMobileCall::TMobileHscsdCallParamsV8
 **/
template<>
class TComparator<RMobileCall::TMobileHscsdCallParamsV8>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TMobileHscsdCallParamsV8>::IsEqual(const RMobileCall::TMobileHscsdCallParamsV8& aData1, const RMobileCall::TMobileHscsdCallParamsV8& aData2);
    };

/**
TComparator Specialisation for TIconId
*/
template<>
class TComparator<RMobileCall::TIconId>
    {
public: 
    IMPORT_C static TBool TComparator<RMobileCall::TIconId>::
    	IsEqual(const RMobileCall::TIconId& aData1, const RMobileCall::TIconId& aData2);
    };




#endif
