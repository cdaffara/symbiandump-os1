// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalTechnology
*/

#ifndef CONNECTIONQUERYSET_H
#define CONNECTIONQUERYSET_H

#include <metadatabase.h>
#include <commsdattypeinfov1_1.h>
#include <comms-infras/es_commsdataobject.h>
#include <comms-infras/ss_commsdataobject.h>
#include <comms-infras/api_ext_list.h>
#include <comms-infras/connectionqueryfactory.h>
#include <comms-infras/commsdataobjectfactory.h>

#include <es_enum_internal.h>

const TUint KConnectionQuerySetUid = 0x10282FF4;
const TUint KTunnelConnQuerySetUid = 0x10285E07;
_LIT(KPanicIllegalLayer, "Illegal layer number");

namespace ESock
{
class XConnectionQuerySet;
class MQueryConnSettingsApiExt;

class XConnectionQueryBase : public Meta::SMetaDataECom
	{
	
public:
	enum TQueryFlags
		{
		ENotAnswered	= 0x01, // Either Not Attempted or Failed
		EAnswered		= 0x02, // Answered Successfully
		EFailed			= 0x04,	// Attempted and Failed (see error code for reason)
		ENotAttempted	= 0x08, // The query has not been attempted (as in nothing has set failed or answered)
		};
		
	enum TQueryDataType
		{
		ENull = 0,
		EBool,
		EInt,
		EUint,
		EText8,
		EText16,
		};

	IMPORT_C static XConnectionQueryBase* LoadL(TPtrC8& aDes);

	TBool Match(const XConnectionQueryBase& aQuery) const
		{
		return iElementId == aQuery.iElementId;
		}
		
	inline CommsDat::TMDBElementId ElementId() const
		{
		return iElementId;
		}

	inline TQueryDataType Type() const
		{
		return iType;
		}

	inline TUint32 Flags() const
		{
		const TUint KSupportedFlags = (ENotAnswered | EAnswered | EFailed | ENotAttempted);
		return iQueryFlags & KSupportedFlags;
		}

	inline TInt Error() const
		{
		return iError;
		}

	inline TBool IsQueryAnswered() const
		{
		return (iQueryFlags & EAnswered) == EAnswered;
		}

	IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf) = 0;
	
protected:
	XConnectionQueryBase()
		: iQueryFlags(ENotAnswered|ENotAttempted), iError(KErrNone),
		iElementId(0), iType(XConnectionQueryBase::ENull)
		{
		}

	XConnectionQueryBase(CommsDat::TMDBElementId aElementId, XConnectionQueryBase::TQueryDataType aType)
		: iQueryFlags(ENotAnswered|ENotAttempted), iError(KErrNone),
		iElementId(aElementId), iType(aType)
		{
		}

	inline void SetElementId(CommsDat::TMDBElementId aElementId)
		{
		iElementId = aElementId;
		}

	void SetError(TInt aError);
	void SetQueryAnswered();

private:
	void SetFlags(TUint32 aFlags);
	void ClearFlags(TUint32 aFlags);
		
protected:
	EXPORT_DATA_VTABLE_AND_FN

private:
	TUint32 iQueryFlags;
	TInt iError;
	TQueryDataType iType;
	CommsDat::TMDBElementId iElementId;
	};



class XBoolQuery : public XConnectionQueryBase
	{
	friend class CConnectionQueryFactory;
	friend class XConnectionQueryBase;
	
public:
	static inline XBoolQuery* NewL(CommsDat::TMDBElementId aElementId);
	static inline XBoolQuery* NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet);

	IMPORT_C virtual ~XBoolQuery();
		
	inline const TBool Data() const
		{
		return iData;
		}
		
protected:
	// From XConnectionQueryBase
	IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf);
	
private:
	inline XBoolQuery()
		: XConnectionQueryBase(0, XConnectionQueryBase::EBool)
		{
		}

    inline XBoolQuery(CommsDat::TMDBElementId aElementId)
    	: XConnectionQueryBase(aElementId, XConnectionQueryBase::EBool)
		{
		}

	inline void SetData(TBool aData)
		{
		iData = aData;
		}

private:
	EXPORT_DATA_VTABLE_AND_FN

private:
	TBool iData;
	};
	
	
class XUintQuery : public XConnectionQueryBase
	{
	friend class CConnectionQueryFactory;
	friend class XConnectionQueryBase;
	
public:
	static inline XUintQuery* NewL(CommsDat::TMDBElementId aElementId);
	static inline XUintQuery* NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet);

	IMPORT_C virtual ~XUintQuery();
			
	inline const TUint32 Data() const
		{
		return iData;
		}
		
protected:		
	// From XConnectionQueryBase
	IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf);
	
private:
	inline XUintQuery()
		: XConnectionQueryBase(0, XConnectionQueryBase::EUint)
		{
		}

    inline XUintQuery(CommsDat::TMDBElementId aElementId)
    	: XConnectionQueryBase(aElementId, XConnectionQueryBase::EUint)
		{
		}

	inline void SetData(TUint32 aData)
		{
		iData = aData;
		}

private:
	EXPORT_DATA_VTABLE_AND_FN

private:
	TUint32 iData;
	};



class XIntQuery : public XConnectionQueryBase
	{
	friend class CConnectionQueryFactory;
	friend class XConnectionQueryBase;
	
public:
	static inline XIntQuery* NewL(CommsDat::TMDBElementId aElementId);
	static inline XIntQuery* NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet);

	IMPORT_C virtual ~XIntQuery();
		
	inline const TInt Data() const
		{
		return iData;
		}
		
protected:	
	// From XConnectionQueryBase
	IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf);
	
private:
	inline XIntQuery()
		: XConnectionQueryBase(0, XConnectionQueryBase::EInt)
		{
		}

    inline XIntQuery(CommsDat::TMDBElementId aElementId)
    	: XConnectionQueryBase(aElementId, XConnectionQueryBase::EInt)
		{
		}

	inline void SetData(TInt aData)
		{
		iData = aData;
		}

private:
	EXPORT_DATA_VTABLE_AND_FN

private:
	TInt iData;
	};


class XText8Query : public XConnectionQueryBase
	{
	friend class CConnectionQueryFactory;
	friend class XConnectionQueryBase;
	
public:
	static inline XText8Query* NewL(CommsDat::TMDBElementId aElementId);
	static inline XText8Query* NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet);

	IMPORT_C virtual ~XText8Query();
		
	inline const TDesC8& Data() const
		{
		return iData;
		}
		
protected:		
	// From XConnectionQueryBase
	IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf);
	
private:
	inline XText8Query()
		: XConnectionQueryBase(0, XConnectionQueryBase::EText8)
		{
		}

    inline XText8Query(CommsDat::TMDBElementId aElementId)
    	: XConnectionQueryBase(aElementId, XConnectionQueryBase::EText8)
		{
		}

	inline void SetData(const TDesC8& aData)
		{
		iData.Close();
		iData.Create(aData);
		}

private:
	EXPORT_DATA_VTABLE_AND_FN

private:
	RBuf8 iData;
	};
	

class XText16Query : public XConnectionQueryBase
	{
	friend class CConnectionQueryFactory;
	friend class XConnectionQueryBase;
	
public:
	static inline XText16Query* NewL(CommsDat::TMDBElementId aElementId);
	static inline XText16Query* NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet);

	IMPORT_C virtual ~XText16Query();
		
	inline const TDesC16& Data() const
		{
		return iData;
		}
		
protected:		
	// From XConnectionQueryBase
	IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf);
	
private:
	inline XText16Query()
		: XConnectionQueryBase(0, XConnectionQueryBase::EText16)
		{
		}

    inline XText16Query(CommsDat::TMDBElementId aElementId)
    	: XConnectionQueryBase(aElementId, XConnectionQueryBase::EText16)
		{
		}
		
private:
	EXPORT_DATA_VTABLE_AND_FN

private:
	RBuf16 iData;
	};



class TConnectionQueryIter
	{
public:
	explicit inline TConnectionQueryIter(const Meta::RMetaDataContainer<XConnectionQueryBase>& aQueries, TUint aFilter)
	    : iQueries(aQueries), iFilter(aFilter), iIndex(0)
        {
        }

    inline void SetToLast()
        {
        iIndex = iQueries.Count() - 1;
        }

	inline XConnectionQueryBase* operator++(TInt /*aInd*/) //-postfix
	    {
	    XConnectionQueryBase* query = Find(iIndex, +1, 1);
	    iIndex++;
	    return query;
	    }

	inline XConnectionQueryBase* operator++()              //-prefix
	    {
	    ++iIndex;
	    return Find(iIndex, +1, 1);
	    }

	inline XConnectionQueryBase* operator--(TInt /*aInd*/) //-postfix
	    {
	    XConnectionQueryBase* query = Find(iIndex, -1, 1);
	    iIndex--;
	    return query;
	    }
	
	inline XConnectionQueryBase* operator[](TInt aInd)
	    {
	    // assuming at present that array indices start from beginning of array
	    // rather than current location of iterator, and array access doesn't
	    // change location of iterator.
		TInt prevIndex = iIndex;
		TInt start = 0;
	    XConnectionQueryBase* query = Find(start, +1, aInd + 1);
		iIndex = prevIndex;
		return query;
	    }

	inline XConnectionQueryBase* operator*()
		{
		return Find(iIndex, +1, 0);
		}


private:
    XConnectionQueryBase* Find(TInt& aIndex, TInt aDirection, TInt aCount)
    	{
    	while (aIndex >= 0 && aIndex < iQueries.Count())
    	    {
    	    XConnectionQueryBase* query = static_cast<XConnectionQueryBase* const>(iQueries[aIndex]);
    	    if ((query->Flags() & iFilter) && (--aCount <= 0))
    	        {
    	        iIndex = aIndex;
    	        return query;
    	        }
    	    aIndex += aDirection;
    	    }
    	return NULL;
    	}


private:
    const Meta::RMetaDataContainer<XConnectionQueryBase>& iQueries;
    TUint iFilter;
    TInt iIndex;
	};



class CConnectionQuerySet;
class XConnectionQuerySet : public XCommsDataObject
	{
	friend class CCommsDataObjectFactory;
	friend class CCommsDataObject<CConnectionQuerySet, XConnectionQuerySet>;
	friend class CConnectionQueryFactory;
	
private:
	/**
	Null query object used internally for finding a query with a particular
	element id
	@internalComponent
	*/
	class XNullQuery : public XConnectionQueryBase	
		{
		friend class ESock::CConnectionQueryFactory;
		friend class ESock::XConnectionQuerySet;
		
	public:
		static inline XNullQuery* NewL(CommsDat::TMDBElementId aElementId)
			{
			XNullQuery* query = static_cast<XNullQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::ENull)));
			query->SetElementId(aElementId);
			return query;
			}

	protected:		
		// From XConnectionQueryBase
		IMPORT_C virtual void GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf);
	
	private:
		XNullQuery()
			{
			}

		XNullQuery(CommsDat::TMDBElementId aElementId)
			: XConnectionQueryBase(aElementId, XConnectionQueryBase::ENull)
			{
			}
			
	private:
		EXPORT_DATA_VTABLE_AND_FN
		};

public:
	enum 
		{
		iUid = KConnectionQuerySetUid
		};

	typedef Meta::RMetaDataContainer<XConnectionQueryBase> RMetaDataQueryContainer;
	
	static inline XConnectionQuerySet* NewL()
		{
		return static_cast<XConnectionQuerySet*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CCommsDataObjectFactory::iUid, KConnectionQuerySetUid)));
		}

	inline TConnectionQueryIter GetQueryIter(TUint aFilter) const
		{
		return TConnectionQueryIter(iQueries, aFilter);	
		}
	
	inline void AddQueryL(XConnectionQueryBase* const aQuery)
		{
		if (!aQuery)
			{
			User::Leave(KErrArgument);
			}
			
		iQueries.AppendL(aQuery);	
		}
		
		
	inline void ResetAndDestroy()
		{
		iQueries.ResetAndDestroy();
		}
		

	inline XConnectionQueryBase* FindQuery(CommsDat::TMDBElementId aElementId) const
		{
		return FindQuery(XNullQuery(aElementId));
		}

	IMPORT_C XConnectionQueryBase* FindQuery(const XConnectionQueryBase& aQuery) const;

	IMPORT_C virtual ~XConnectionQuerySet();
	
	// From XCommsDataObject
	IMPORT_C virtual TProgressAction DispatchL(TAny* aItfPtr, ESock::MPlatsecApiExt* aPlatsecItf);
		
protected:
	inline XConnectionQuerySet()
		: XCommsDataObject(
			(TUint)(XCommsDataObject::EGetSupported | XCommsDataObject::EProgressive),
			(TUint) MQueryConnSettingsApiExt::KInterfaceId)
		{
		}

protected:
	TUint iFilter;

private:
	EXPORT_DATA_VTABLE_AND_FN
protected:
	RMetaDataQueryContainer iQueries;
	};




class CConnectionQuerySet : public CCommsDataObject<CConnectionQuerySet, XConnectionQuerySet>
	{
protected:
    inline CConnectionQuerySet(XConnectionQuerySet* aDataObject)
        : CCommsDataObject<CConnectionQuerySet, XConnectionQuerySet>(aDataObject)
        {
        }
	};

// ---------------- Inline methods ----------------
inline XBoolQuery* XBoolQuery::NewL(CommsDat::TMDBElementId aElementId)
	{
	XBoolQuery* self = static_cast<XBoolQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EBool)));
	self->SetElementId(aElementId);
	return self;
	}

inline XBoolQuery* XBoolQuery::NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet)
	{
	XBoolQuery* self = static_cast<XBoolQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EBool)));
	self->SetElementId(aElementId);
	CleanupDeletePushL(self);
	aQuerySet.AddQueryL(self);
	CleanupStack::Pop();
	return self;
	}



inline XUintQuery* XUintQuery::NewL(CommsDat::TMDBElementId aElementId)
	{
	XUintQuery* self = static_cast<XUintQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EUint)));
	self->SetElementId(aElementId);
	return self;
	}

inline XUintQuery* XUintQuery::NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet)
	{
	XUintQuery* self = static_cast<XUintQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EUint)));
	self->SetElementId(aElementId);
	CleanupDeletePushL(self);
	aQuerySet.AddQueryL(self);
	CleanupStack::Pop();
	return self;
	}



inline XIntQuery* XIntQuery::NewL(CommsDat::TMDBElementId aElementId)
	{
	XIntQuery* self = static_cast<XIntQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EInt)));
	self->SetElementId(aElementId);
	return self;
	}

inline XIntQuery* XIntQuery::NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet)
	{
	XIntQuery* self = static_cast<XIntQuery*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EInt)));
	self->SetElementId(aElementId);
	CleanupDeletePushL(self);
	aQuerySet.AddQueryL(self);
	CleanupStack::Pop();
	return self;
	}



inline XText8Query* XText8Query::NewL(CommsDat::TMDBElementId aElementId)
	{
	XText8Query* self = static_cast<XText8Query*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EText8)));
	self->SetElementId(aElementId);
	return self;
	}

inline XText8Query* XText8Query::NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet)
	{
	XText8Query* self = static_cast<XText8Query*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EText8)));
	self->SetElementId(aElementId);
	CleanupDeletePushL(self);
	aQuerySet.AddQueryL(self);
	CleanupStack::Pop();
	return self;
	}



inline XText16Query* XText16Query::NewL(CommsDat::TMDBElementId aElementId)
	{
	XText16Query* self = static_cast<XText16Query*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EText16)));
	self->SetElementId(aElementId);
	return self;
	}

inline XText16Query* XText16Query::NewL(CommsDat::TMDBElementId aElementId, XConnectionQuerySet& aQuerySet)
	{
	XText16Query* self = static_cast<XText16Query*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CConnectionQueryFactory::iUid, XConnectionQueryBase::EText16)));
	self->SetElementId(aElementId);
	CleanupDeletePushL(self);
	aQuerySet.AddQueryL(self);
	CleanupStack::Pop();
	return self;
	}


class CTunnelConnQuerySet;
/**
For connection data query in a tunnel comms stack
The default constructor is for query on the bottom connection
@internalComponent
*/
class XTunnelConnQuerySet : public XConnectionQuerySet
	{
friend class CCommsDataObject<CTunnelConnQuerySet, XTunnelConnQuerySet>;

private:
	EXPORT_DATA_VTABLE_AND_FN
	TInt iLayerNum;

public:
	enum TTunnelQuerySpecLayer
		{
		EBottomIAPLayer	= -1,
		};
	
	enum TTunnelQueryPanics
		{
		ETunnelQueryInvalidLayer = 1,
		};

public:
	inline XTunnelConnQuerySet() : iLayerNum(EBottomIAPLayer)
		{
		};

	// (aLayerNum - 1) means the number of layer(s) be skipped
	// aLayerNum must be greater than zero
	inline XTunnelConnQuerySet(TInt aLayerNum) : iLayerNum(aLayerNum)
		{
		__ASSERT_ALWAYS(((0 == aLayerNum || aLayerNum < EBottomIAPLayer)), User::Panic(KPanicIllegalLayer, ETunnelQueryInvalidLayer));
		};

public:
	enum 
	{
	iUid = KTunnelConnQuerySetUid
	};
	// Default one, gets the bottom IAP
	static inline XTunnelConnQuerySet* NewL()
		{
		XTunnelConnQuerySet* self = static_cast<XTunnelConnQuerySet*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CCommsDataObjectFactory::iUid, KTunnelConnQuerySetUid)));
		return self;
		};
	
	// Get the aLayerNumTH layer IAP
	static inline XTunnelConnQuerySet* NewL(TInt aLayerNum)
		{
		XTunnelConnQuerySet* self = static_cast<XTunnelConnQuerySet*>(Meta::SMetaDataECom::NewInstanceL(Meta::STypeId::CreateSTypeId(CCommsDataObjectFactory::iUid, KTunnelConnQuerySetUid)));
		self->SetLayerNum(aLayerNum);
		return self;
		};

	IMPORT_C virtual TProgressAction DispatchL(TAny *aItfPtr, ESock::MPlatsecApiExt* aPlatsecItf);

private:
	void inline SetLayerNum(TInt aLayerNum)
		{
		iLayerNum = aLayerNum;
		};
	};


class CTunnelConnQuerySet : public CCommsDataObject<CTunnelConnQuerySet, XTunnelConnQuerySet>
{
protected:
	/**
	C'tor for the CCommsDataObject class.
	@param aDataObject A ptr to the data object to take ownership of
	 */
	inline CTunnelConnQuerySet(XTunnelConnQuerySet* aDataObject);
};
 
	// ---------------- Inline methods ----------------
	inline CTunnelConnQuerySet::CTunnelConnQuerySet(XTunnelConnQuerySet* aDataObject) 
		: CCommsDataObject<CTunnelConnQuerySet, XTunnelConnQuerySet>(aDataObject)
	{};

} // namespace ESock

#endif
// CONNECTIONQUERYSET_H


