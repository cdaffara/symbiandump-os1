// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NODEMESSAGES_SELECTOR_H
#define SYMBIAN_NODEMESSAGES_SELECTOR_H

#include <connpref.h>
#include <comms-infras/metadata.h>
#include <elements/nm_signatures.h>

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/cfmacro.h>
#include <comms-infras/api_ext_msg.h>

#include <es_enum.h>
#include <comms-infras/ss_commsprov.h>
#include <comms-infras/es_availability.h>

namespace ConnStates
{
	class TSelectMetaPlane;
}

class TConnProviderInfo;

namespace ESock
{

/**
TConnProviderInfo based preferences. Can be passed into RConnection::Start
to specify what connection should be started. The handling of this preference
is implementation specific.

@publishedPartner
@released
*/

class TConnProviderInfoPref : public TConnPref
	{
public:
	IMPORT_C explicit TConnProviderInfoPref(const TConnProviderInfo& aProviderInfo);

	IMPORT_C const TConnProviderInfo& Info() const;
	};

/**
Selection preferences object used internally by the 3 plane comms implementation.

@publishedPartner
@released
*/
class TSelectionPrefs : public Meta::SMetaData
    {
public:
	/**
	Uid and TypeId for SMetaData
	*/
	enum {EUid = 0x10272C79, ETypeId = 1};

	/**
	Scopes for which the selection can take place.
	
	Each of the flags may be set, alone, all or in any combinations
	*/
	enum TSelectionScope
		{
		/**
		For initialisation only
		*/
		ENone               = 0x00,
		/**
		Select only from preexisting providers. Do not create new ones.
		*/
		ESelectFromExisting = 0x01,
		/**
		Select only the top provider.
		*/
		EExplicitConnection = 0x02,
		/**
		Do not request a comms binder.
		*/
		ERequestCommsBinder = 0x04  
		};

	/**
	Flags to indicate what type of selection is taking place.
	*/
    enum TSelectionFlags
        {
		/**
		Selecting for a monitor access point. This access point does not affect the idleness
		of a node.
		*/
		EMonitor = 0x01,
		/**
		Selecting for an access point wishing to attach to a specified access point which 
		already exists
		*/
        EAttach  = 0x02	
        };

public:
    IMPORT_C TSelectionPrefs();
	/**
	@param aSelectionScope Scope for which the selection will take place
	*/
    IMPORT_C TSelectionPrefs(TSelectionScope aSelectionScope);
	/**
	@param aPref Preferences used to decide which access point to select
	*/
    IMPORT_C TSelectionPrefs(const TConnPref& aPref);

	/**
	@param aPref Preferences used to decide which access point to select
	*/
    inline void SetPrefs(const TConnPref& aPref);

	/**
	@return Mutable connection preferences of this selection preference.
	*/	
	inline TConnPref& Prefs();

	/**
	@return Immutable connection preferences of this selection preference.
	*/
	inline const TConnPref& Prefs() const;

	/**
	@return ETrue if no preferences has been set in the selection preferences object
	*/
	inline TBool IsEmpty() const;

	/**
	@return the scope of the selection preferences
	*/
    inline TUint Scope() const;

	/**
	@param aScope a new scope for the selection preferences
	*/
    inline void SetScope(TUint aScope);

	/**
	@return The flags which have been set for the selection preferences
	*/
    inline TUint Flags() const;

	/**
	@param aFlags New flags for set for the selection preferences. Overwrites the old flags
	*/
    inline void SetFlags(TUint aFlags);

	/**
	@return The subsession unique id of the subsession which initiated this selection
	*/
	inline TSubSessionUniqueId SubSessionUniqueId() const;

	/**
	@param aSubSessionUniqueId The subsession unique id of the subsession initiated this selection
	*/	
	inline void SetSubSessionUniqueId(TSubSessionUniqueId aSubSessionUniqueId);

    EXPORT_DATA_VTABLE_AND_FN

private:
	TConnPref iPrefs;
    union
        {
        TUint iSelectionParams; //used by meta data offset
        struct //used by node
            {
    		TUint iScope : 16;
    		TUint iFlags : 16;
            } iS;
        } iU;
	TSubSessionUniqueId iSubSessionUniqueId;
    };

void TSelectionPrefs::SetPrefs(const TConnPref& aPref)
   	{
   	iPrefs = aPref;
   	}

TConnPref& TSelectionPrefs::Prefs()
	{
	return iPrefs;
	}

const TConnPref& TSelectionPrefs::Prefs() const
	{
	return iPrefs;
	}

TBool TSelectionPrefs::IsEmpty() const
	{
	return iPrefs.ExtensionId() == TConnPref::EConnPrefUnknown;
	}

TUint TSelectionPrefs::Scope() const
    {
    return iU.iS.iScope;
    }

void TSelectionPrefs::SetScope(TUint aScope)
    {
    iU.iS.iScope = aScope;
    }

TUint TSelectionPrefs::Flags() const
    {
    return iU.iS.iFlags;
    }

void TSelectionPrefs::SetFlags(TUint aFlags)
    {
    iU.iS.iFlags = aFlags;
    }

TSubSessionUniqueId TSelectionPrefs::SubSessionUniqueId() const
	{
	return iSubSessionUniqueId;
	}

void TSelectionPrefs::SetSubSessionUniqueId(TSubSessionUniqueId aSubSessionUniqueId)
	{
	iSubSessionUniqueId = aSubSessionUniqueId;
	}

/**
Selection preferences override. By default a mcpr will select using the selection prefs
which were used to create it. The selection preferences override can be used to make them use
something else.

@publishedPartner
@released
*/
class TOverridenSelectionPrefsExt : public Meta::SMetaData
    {
public:
	enum {EUid = 0x10272C79, ETypeId = 2};

public:
	/**
	@param aTierId The tier on which the preferences will be used
	@param aPrefs The preferences which will override the default preferences
	 */
    IMPORT_C explicit TOverridenSelectionPrefsExt(TUid aTierId, const TSelectionPrefs& aPrefs);

    EXPORT_DATA_VTABLE_AND_FN

	TSelectionPrefs iPrefs;
	TUid iTierId;
    };

/**
Panic catagory for RConnPrefList panics
*/
_LIT (RConnPrefListPanic,"RConnPrefListPanic");

/**
Null element panic code. An attempt was made to access an element in the
preferences list that didn't exist
*/
const TInt ENullElement = 1;

/**
This class will be used as a handle to TConnPrefList. During construction of
the stack this class will be sent in messages as TConnPrefList cannot be sent
in every message. The overhead of this would be too great (as it would have to
be serialized and deserialized every time the message was sent). A reference
cannot be sent as this would not work across processes.


@publishedPartner
@released
*/
class RConnPrefList
	{
	friend class ConnStates::TSelectMetaPlane;
public:
	/**
	Base class for RConnPrefList iterators
	*/
	class TIterBase
		{
	protected:
		/**
		@param aConnPrefList Connection preference list to iterate over
		*/
		TIterBase(RConnPrefList &aConnPrefList)
			: iConnPrefList(aConnPrefList), iMasterIndex(-1)
				{

				}

		/**
		@param aType Only return preferences of this type
		@param aIndex Index of preference requested
		@return The requested connection preference, or NULL if no preference of the specified type was found at aIndex
		*/
		IMPORT_C Meta::SMetaData* AtIndex(const Meta::STypeId& aType, TInt aIndex);

	protected:
		RConnPrefList& iConnPrefList;
		TInt iMasterIndex;
		};

	template<class TYPE>
	/**
	This iterator will return objects of the templated class. It will only
	return objects from the list that are of the type passed in as the template parameter
	*/
	class TIter : public TIterBase
		{
		public:
		    /**
			@param aConnPrefList Connection preferences to iterate over
			*/
			TIter(RConnPrefList &aConnPrefList)
			: TIterBase(aConnPrefList)
				{
				iIndex = 0;
				}

		    /**
			This will return the instance of the given type in the list at the
			point specified by aIndex

			@param aIndex index of item to return
			@return The item found at the index, or NULL if not found
			*/
			TYPE* operator[](TInt aIndex)
				{
				return static_cast<TYPE*>(AtIndex(Meta::STypeId::CreateSTypeId(TYPE::EUid,TYPE::ETypeId) , aIndex));
				}

		    /**
 			@return the next instance of the given tpye in the list or NULL if no more instances remain
			*/
			TYPE* operator++(TInt)
				{
				return static_cast<TYPE*>((*this)[iIndex++]);
				}

		    /**
			@return a reference to the current object pointed to by the iterator
			*/
			TYPE& operator*()
				{
				TYPE* ptr = NULL;
				if (iMasterIndex >= 0)
					{
					ptr = static_cast<TYPE*>(iConnPrefList[iMasterIndex]);
					}
				else
					{
					ptr = (*this)[iIndex];
					}
				__ASSERT_ALWAYS(ptr != NULL,User::Panic(RConnPrefListPanic, ENullElement));
				return *ptr;
				}

		    /**
		    @return a pointer to the current object pointed to by the iterator
			*/
			TYPE* operator->()
				{
				TYPE* ptr = NULL;
				if (iMasterIndex >= 0)
					{
					ptr = static_cast<TYPE*>(iConnPrefList[iMasterIndex]);
					}
				else
					{
					ptr = (*this)[iIndex];
					}
				__ASSERT_ALWAYS(ptr != NULL,User::Panic(RConnPrefListPanic, ENullElement));
				return ptr;
				}

		    /**
			@return ETrue if there are no more elements to iterate over. EFalse, otherwise
			*/
			TBool IsEnd()
				{
				if((*this)[iIndex] == NULL)
					{
					return ETrue;
					}
				else
					{
					return EFalse;
					}
				}

		    /**
			Remove the current object from the iterators associated RConnPrefList
			@return a pointer to the removed object. The caller is responsible for the deletion of the object returns.
			*/
			TYPE* Remove()
				{
				TYPE* ptr = NULL;
				if (iMasterIndex >= 0)
					{
					ptr = static_cast<TYPE*>(iConnPrefList[iMasterIndex]);
					}
				else
					{
					ptr = (*this)[iIndex];
					}

				iConnPrefList.Remove(iMasterIndex);
				(*this)[iIndex];
				return ptr;
				}

		private:
			TInt iIndex;
		};

public:
	IMPORT_C RConnPrefList();

	/**
	Open and initialise with data from passed in RConnPrefList
	@param aPrefList RConnPrefList to copy data from. The copy is shallow.
	 */
	IMPORT_C void Open(RConnPrefList& aPrefList);

	/**
	Close the RConnPrefList object
	*/
	IMPORT_C void Close();

	/**
    Create an iterator for template parameter TYPE. The iterator will be able to
	iterate over all the members of the RConnPrefList which match the type specified.
	@return A new iterator object for this connection preferences list
	*/
	template<class TYPE>
	RConnPrefList::TIter<TYPE> getIter()
		{
		return RConnPrefList::TIter<TYPE>(*this);
		}

	/**
	Append a preference to connection preferences list
	@param aFamily Preference to append
	@leave System wide error code.
	*/
	IMPORT_C void AppendL(SMetaDataECom* aFamily);

	/**
	@param aIndex index of requested object
	@return The object at aIndex
	*/
	IMPORT_C SMetaData* operator[](TInt aIndex);

	/**
	Remove an object from the RConnPrefList
	@param aIndex the index of the object to remove
	*/
	IMPORT_C void Remove(TInt aIndex);

	/**
	@return the number of preferences in the list
	*/
	IMPORT_C TInt Count();

private:
	TInt Open(TConnPrefList* aObject);
	TConnPrefList* iObject;
	};


/**
Signature for a message which can carry selection preferences
*/
struct TSigSelectionPrefs : public Messages::TSignatureBase
    {
protected:
	inline TSigSelectionPrefs() {}

	/**
	@param aMessageId Id of the message this signature is being created for.
	*/
	TSigSelectionPrefs(const Messages::TNodeSignal::TMessageId& aMessageId)
	:	Messages::TSignatureBase(aMessageId)
		{}

	/**
	@param aMessageId Id of the message this signature is being created for.
	@param aSelectionPrefs Selection preferences object to be passed in the message
	*/
	TSigSelectionPrefs(const Messages::TNodeSignal::TMessageId& aMessageId, const Meta::SMetaData& aSelectionPrefs)
	:	Messages::TSignatureBase(aMessageId)
	    {
	    //TODO: the copy can be optimised out when SMetaDataNetCtr handlers supported.
	    iSelectionPrefs.Copy(aSelectionPrefs);
	    }
public:
	DECLARE_MVIP_CTR(TSigSelectionPrefs)
   	EXPORT_DATA_VTABLE_AND_FN

	/**
	Selection preferences object passed in the message
	*/
	TSelectionPrefs iSelectionPrefs;
    };

/**
Signature for a message which can carry a list of connection preferences
*/
struct TSigConnPrefList : public Messages::TSignatureBase
    {
protected:
	inline TSigConnPrefList() {}

	/**
	@param aMessageId Id of the message this signature is being created for.
	*/	
	TSigConnPrefList(const Messages::TNodeSignal::TMessageId& aMessageId)
	:	Messages::TSignatureBase(aMessageId)
	    {}

	/**
	@param aMessageId Id of the message this signature is being created for.
	@param aConnPrefList List of connection preferences to be passed in this message
	*/
	TSigConnPrefList(const Messages::TNodeSignal::TMessageId& aMessageId, RConnPrefList& aConnPrefList)
	:	Messages::TSignatureBase(aMessageId)
	    {
	    iConnPrefList = aConnPrefList;
	    }
public:
	DECLARE_MVIP_CTR(TSigConnPrefList)
   	EXPORT_DATA_VTABLE_AND_FN

	/**
	List of connection preferences passed in this message
	*/
   	RConnPrefList iConnPrefList;
    };


/**
Message signature template for a message which can carry selection preferences

@see TSigSelectionPrefs
*/
template<TInt id, TInt32 realm>
struct TMessageSigSelectionPrefs : public TSigSelectionPrefs, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigSelectionPrefs() :
		TSigSelectionPrefs(Messages::TNodeSignal::TMessageId(id, realm))
		{ }

	explicit TMessageSigSelectionPrefs(const Meta::SMetaData& aSelectionPrefs)
	:	TSigSelectionPrefs(Messages::TNodeSignal::TMessageId(id, realm), aSelectionPrefs)
	    {
	    }
    };

/**
Message signature template for a message which can carry a list of connection preferences

@see TSigConnPrefList
*/
template<TInt id, TInt32 realm>
  struct TMessageSigConnPrefList : public TSigConnPrefList, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigConnPrefList()
	:	TSigConnPrefList(Messages::TNodeSignal::TMessageId(id, realm))
	    {}

	explicit TMessageSigConnPrefList(RConnPrefList& aCustomPrefs)
	:	TSigConnPrefList(Messages::TNodeSignal::TMessageId(id, realm), aCustomPrefs)
	    {}
    };

/**
Signature for a select complete message.
*/
struct TSigSelectComplete : public Messages::TSignatureBase
    {
protected:
	inline TSigSelectComplete() {}

	/**
	@param aMessageId Id of the message this signature is being created for.
	@param aNodeId    Node id of node which has been created
	@param aProviderInfo Provider info of access point which has been created.
	*/
	TSigSelectComplete(const Messages::TNodeSignal::TMessageId& aMessageId, const Messages::TNodeId& aNodeId, const TProviderInfo& aProviderInfo)
	:	Messages::TSignatureBase(aMessageId),
		iNodeId(aNodeId),
		iProviderInfo(aProviderInfo)
	    {
	    }
public:
	DECLARE_MVIP_CTR(TSigSelectComplete)
   	EXPORT_DATA_VTABLE_AND_FN
	/**
	Node id of node which has been created
	*/
   	Messages::TNodeId iNodeId;
	/**
	Provider info of the access point which has been created
	*/
   	TProviderInfo iProviderInfo;
    };

/**
Message signature template for a select complete message

@see TSigSelectComplete
 */
template<TInt id, TInt32 realm>
struct TMessageSigSelectComplete : public TSigSelectComplete, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigSelectComplete(const Messages::TNodeId& aNodeId, const TProviderInfo aProviderInfo)
	:	TSigSelectComplete(Messages::TNodeSignal::TMessageId(id, realm), aNodeId, aProviderInfo)
		{
		}
    };

/**
Message signature template for availability subscription message
 */
DECLARE_MESSAGE_SIG_1(SigAvailabilitySubscriptionOptions, TAvailabilitySubscriptionOptions, AvailabilitySubscriptionOptions)
/**
Message signature for availability status message
*/
DECLARE_MESSAGE_SIG_1(SigAvailabilityStatus, TAvailabilityStatus, AvailabilityStatus)

/**
Message interface for selector nodes
Nodes wishing to implement this interface must implement all message protocols in the interface.

@publishedPartner
@released
*/
class TCFSelector
	{
  private:
  	enum
 	{
 	ECFSimpleSelect					= 1,
	ECFSelect		                = 2,
	ECFSelectComplete				= 3
	};

public:
    enum { ERealmId = 0x10285F4E }; //UID allocated on 4/6/08 from KUidNodeMessageRealms

    /**
	Selection request sent to TierManagers (usually by MCPRs). Can only carry one type of selection preferences at a time, and for this
	reason, using TSelect is preferred. It should be noted however, that multiple nodes may be returned in exchange for this single
	preference, for example in the case of SNAP preferences.

    This request is completed only after TSelectComplete with a NULL address has been received by the requestor.

    @li Type: Request
    @li Fallible: Yes

	@param iSelectionPrefs preferences used by the selection process to choose nodes
    */
    typedef TMessageSigSelectionPrefs<ECFSimpleSelect, TCFSelector::ERealmId> TSimpleSelect;


    /**
	Selection request sent to Tier Managers (usually by MCPRs or CSR). Carries an RConnPrefList instance, the objects in the list are used 
	during the construction of the stack. Multiple nodes may be returned by the selection activity. This request is completed only after
	TSelectComplete with a NULL address has been received by the requestor.

	@li Type: Request
	@li Fallible: Yes

	@param aConnPrefList an instance of RConnPrefList
	*/
    typedef TMessageSigConnPrefList<ECFSelect, TCFSelector::ERealmId> TSelect;

	/**
	Response to TSelect or TSelectNextLayer. Carries address to selected providers (MCPRs). A selection request is completed only after
	TSelectComplete with a NULL address has been received by the requestor. This response also carries some additional information about
	the selected provider.

    @li Type: Response to TSimpleSelect or TSelect
    @li Fallible:  No

	@param iNodeId node id of the node which has been selected
   	@param iProviderInfo additional information about the created node
	*/
    typedef TMessageSigSelectComplete<ECFSelectComplete, TCFSelector::ERealmId> TSelectComplete;
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_SELECTOR_H

