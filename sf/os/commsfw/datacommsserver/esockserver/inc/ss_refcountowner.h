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
//

/**
 @file
 @Internal
 @released
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/

#ifndef __SS_REFCOUNTOWNER_H__
#define __SS_REFCOUNTOWNER_H__

#include <e32base.h>
#include <elements/nm_node.h>
#include <comms-infras/es_event.h>


class CParameterBundleBase;
namespace ESock
{

class CRefCountOwnerBase : public CBase, protected Messages::ASimpleNodeIdBase
/**
Base container class useful for passing reference counted objects around the stack.
When a node receives an object based on this, it calls Open to increment the access
counter and ensure the object does go away while it's being used. When the node is
finished with the object it calls Close on it. The object will then delete itself if
it's access counter is zero.

Objects of this type will delete themselves asynchronously.
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
    {
public:
	/**
	Relinquish access to the object. Decrements and access counter and may trigger deletion of the point and of the object
	*/
    IMPORT_C void Close();
	/**
	Gain access to the object. Increments the access counter
	*/
    IMPORT_C void Open();

protected:
	/**	
	@param aPtr pointer to the real object to be contained
	 */
    IMPORT_C CRefCountOwnerBase(const TAny* aPtr);

    IMPORT_C ~CRefCountOwnerBase();

	/**
	Receive a message. CRefCountOwnerBase objects can only receive TCFDataClient::TStop, which will trigger deletion of the ptr and deletion of the object
	*/
   	IMPORT_C virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

    /**
	Delete the object pointed to by iPtr. Derived classes must implement this.
	 */
	virtual void DeletePtr() = 0;

protected:
	/**
	@param aPtr CObject to delete
	*/
    static void DeletePtr(CObject* aPtr)
        {
        aPtr->Close();
        }

	/**
	@param aPtr CBase to delete
	*/
    static void DeletePtr(const CBase* aPtr)
        {
        delete aPtr;
        }

	/**
	@param aPtr Pointer to a Meta::SMetaData to delete
	*/
    static void DeletePtr(const Meta::SMetaData* aPtr)
        {
        delete aPtr;
        }

protected:
	/**
	Pointer to the contained object
	*/
    TAny*       iPtr;

	/**
	Sanity checking member. Set to true after first open. Prevents dead objects being used.
	Only used in udeb, but not conditional to avoid any urel/udeb BC issues
	*/
    TBool		iRefOpened; 

private:
    TInt        iRefCount;
    };

template <class T>
class CRefCountOwner : public CRefCountOwnerBase
/*
Use this class as a wrapper for a pointer that you need to send
(with a TCFMessage) and give away the ownership of (as with
subconnection events). This class solves two problems:
(1) having multiple recipients for the message (no recipient
    can determine whether it should destroy the pointer).
(2) having recipients in a different thread than the
    sender. One mustn't dealocate memory alocated in a different
    thread.
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
    {
public:
	/**
	@param Pointer to object of type T
	*/
    CRefCountOwner(T* aPtr)
        : CRefCountOwnerBase(aPtr)
        {
        }

public:
	/**
	@return pointer to the contained object, or NULL if it hasn't been set
	*/
    T* Ptr()
        {
        return static_cast<T*>(iPtr);
        }

	/**
	@return pointer to the contained object
	@leave KErrNotReady if the pointer hasn't been set
	*/
    T* PtrL()
        {
        if(iPtr==NULL)
	        {
	        User::Leave(KErrNotReady);
			}
        return static_cast<T*>(iPtr);
        }

protected:
	/**
	Delete the pointer. 
	 */
    virtual void DeletePtr()
        {
        CRefCountOwnerBase::DeletePtr(Ptr());
        }
    };


/**
Typed version of the CRefCountOwner specifically for posting sub connection notification events
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
typedef CRefCountOwner<const CSubConNotificationEvent> CRefCountOwnedSubConNotification;

typedef CRefCountOwner<const ESock::XPlaneEvent> CRefCountOwnedNotification;
	
/**
Typed version of the CRefCountOwner specifically for posting parameter bundles
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
typedef CRefCountOwner<const CParameterBundleBase> CRefCountOwnedParameterBundle;


}



#endif // __SS_REFCOUNTOWNER_H__

