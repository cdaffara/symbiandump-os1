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

#include <dscitem.h>
#include <dscstoredefs.h>
#include "dscdatabase.h"
#include "sysstartpanic.h"


/**
Used to create an instance of the CDscItem class
@return An instance of CDscItem
@leave One of the system-wide error codes
@publishedAll
@released
*/
EXPORT_C CDscItem* CDscItem::NewL()
	{
	CDscItem* self = CDscItem::NewLC();
	CleanupStack::Pop(self);
	return self;				
	}

/**
Used to create an instance of the CDscItem class.
The returned instance is put on the CleanupStack.
@return An instance of CDscItem
@leave One of the system-wide error codes
@publishedAll
@released
*/
EXPORT_C CDscItem* CDscItem::NewLC()
	{
	CDscItem* self = new (ELeave) CDscItem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 @see NewL
 @see SetFileParamsL
 */
EXPORT_C CDscItem* CDscItem::NewL(const TDesC& aFileName, const TDesC& aArgs)
	{
	CDscItem* self = CDscItem::NewLC(aFileName, aArgs);
	CleanupStack::Pop(self);
	return self;
	}

/**
 @see NewLC
 @see SetFileParamsL
 */
EXPORT_C CDscItem* CDscItem::NewLC(const TDesC& aFileName, const TDesC& aArgs)
	{
	CDscItem* self = CDscItem::NewLC();
	self->SetFileParamsL(aFileName, aArgs);
	return self;
	}

EXPORT_C CDscItem::~CDscItem()
	{
	delete iStartupProperties;
	}

CDscItem::CDscItem() :
	iDscId(TUid::Uid(KDefaultSymbianDsc))
	{
	}
	
void CDscItem::ConstructL()
	{
	iStartupProperties = CStartupProperties::NewL();
	}

/** 
Sets the Id of the Dynamic Startup Configuration list this item resides in. 
The dscId of a DSC in the database must be set with this method before adding, updating, or deleting this item in this DSC
@publishedAll
@released
@param aDscId Id of the DSC this item resides in.
*/
EXPORT_C void CDscItem::SetDscId(const TUid& aDscId)
	{
	iDscId=aDscId;
	}

/** 
Accessor function returning the DscId of the DSC this item was retrieved from
@publishedAll
@released
@return DscId property
*/		
EXPORT_C TUid CDscItem::DscId() const
	{
	return iDscId;
	}
	
/** 
Accessor function returning ItemId of the item
@publishedAll
@released
@return ItemId property
*/	
EXPORT_C TInt CDscItem::ItemId() const
	{
	return iItemId;
	}
	
/**
Accessor function returning the filename value of the item
@publishedAll
@released
@return iStartupProperties->FileName
@see CStartupProperties
*/
EXPORT_C TPtrC CDscItem::FileName() const
	{
	return iStartupProperties->FileName();
	}
	
/**
Accessor function returning the args value of the item
@publishedAll
@released
@return iStartupProperties->Args
@see CStartupProperties
*/
EXPORT_C TPtrC CDscItem::Args() const
	{
	return iStartupProperties->Args();
	}

/**
Accessor function returning the startup type value of the item
@publishedAll
@released
@return iStartupProperties->StartupType
@see CStartupProperties
*/
EXPORT_C TStartupType CDscItem::StartupType() const
	{
	return iStartupProperties->StartupType();
	}
	
/**
Accessor function returning the start method value of the item
@publishedAll
@released
@return iStartupProperties->StartMethod
@see CStartupProperties
*/
EXPORT_C TStartMethod CDscItem::StartMethod() const
	{
	return iStartupProperties->StartMethod();
	}
	
/**
Accessor function returning the number of retries value of the item
@publishedAll
@released
@return iStartupProperties->NoOfRetries
@see CStartupProperties
*/
EXPORT_C TInt CDscItem::NoOfRetries() const
	{
	return iStartupProperties->NoOfRetries();
	}
	
/**
Accessor function returning the timeout value of the item
@publishedAll
@released
@return iStartupProperties->Timeout
@see CStartupProperties
*/
EXPORT_C TInt CDscItem::Timeout() const
	{
	return iStartupProperties->Timeout();
	}

/**
Accessor function returning the monitored value of the item
@publishedAll
@released
@return iStartupProperties->Monitored
@see CStartupProperties
*/
EXPORT_C TBool CDscItem::Monitored() const
	{
	return iStartupProperties->Monitored();
	}

/**
Accessor function returning the viewless value of the item
@publishedAll
@released
@return iStartupProperties->Viewless
@see CStartupProperties
*/
EXPORT_C TBool CDscItem::Viewless() const
	{
	return iStartupProperties->Viewless();
	}

/**
Accessor function returning the StartInBackground value of the item
@publishedAll
@released
@return iStartupProperties->StartInBackground
@see CStartupProperties
*/
EXPORT_C TBool CDscItem::StartInBackground() const
	{
	return iStartupProperties->StartInBackground();
	}

/**
Sets the filename and args values of the item
@publishedAll
@released
@param aFileName filename of the component. 
@param aArgs arg list of the component. 
@leave KErrArgument - the filename is empty or one of the parameters exceeds KDscStoreMaxStringLength
@leave One of the system-wide error codes
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetFileParamsL(const TDesC& aFileName, const TDesC& aArgs)
	{
	
	if ((aFileName==KNullDesC) || (aFileName.Length() > CDscDatabase::MaxStringLength()) ||
		(aArgs.Length() > CDscDatabase::MaxStringLength()))
		{
		User::Leave(KErrArgument);
		}
	iStartupProperties->SetFileParamsL(aFileName, aArgs);
	}

/**
Sets the start method value of the item
@publishedAll
@released
@param aStartMethod How we want to start the component
@leave	KErrArgument - the start method is not either EFireAndForget or EWaitForStart
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetStartMethodL(TStartMethod aStartMethod)
	{
	if ((aStartMethod != EFireAndForget) && (aStartMethod != EWaitForStart))
		{
		User::Leave(KErrArgument);
		}
	iStartupProperties->SetStartMethod(aStartMethod);
	}

/**
Sets the number of retries value of the item
@publishedAll
@released
@param aNumRetries number of retries should the start fail
@leave	KErrArgument - the number of retries must be either 0 or 1
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetNoOfRetriesL(TInt aNumRetries)
	{
	if (aNumRetries != 0 && aNumRetries != 1)
		{
		User::Leave(KErrArgument);
		}
	iStartupProperties->SetNoOfRetries(aNumRetries);
	}
	
/**
Sets the timeout value of the item
@publishedAll
@released
@param aTimeout timeout between retries
@leave	KErrArgument - the timeout must not be negative
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetTimeoutL(TInt aTimeout)
	{
	if (aTimeout < 0)
		{
		User::Leave(KErrArgument);
		}
	iStartupProperties->SetTimeout(aTimeout);
	}

/**
Sets the monitored value value of the item
@publishedAll
@released
@param aMonitored Whether the component is to be monitored once it is started
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetMonitored(TBool aMonitored)
	{
	iStartupProperties->SetMonitored(aMonitored);
	}

/**
Sets the startup type value of the item
@publishedAll
@released
@param aType The startup type
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetStartupType(TStartupType aType)
	{
	iStartupProperties->SetStartupType(aType);
	}

/**
Sets the viewless value of the item
@publishedAll
@released
@param aViewless Whether the application is viewless or not
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetViewless(TBool aViewless)
	{
	iStartupProperties->SetViewless(aViewless);
	}

/**
Sets the StartInBackground value of the item
@publishedAll
@released
@param aStartInBackground Whether to start the application in the background or not
@see CStartupProperties
*/
EXPORT_C void CDscItem::SetStartInBackground(TBool aStartInBackground)
	{
	iStartupProperties->SetStartInBackground(aStartInBackground);
	}

	
/** 
Returns a const reference to the embedded startup properties object
@publishedAll
@released
@return Const reference to the embedded CStartupProperties object
@see CStartupProperties
*/		
EXPORT_C const CStartupProperties& CDscItem::StartupProperties() const
	{
		return (*iStartupProperties);
	}

/**
Reset all variables in this instance back to constructor defaults. 
This will enable to reuse this instance instead of having to delete it 
and create a new one.
@publishedAll
@released
*/
EXPORT_C void CDscItem::Reset()
	{
	iDscId = TUid::Uid(KDefaultSymbianDsc);
	iItemId = 0;
	iStartupProperties->Reset();
	}

/** 
Sets itemId parameter for the item. Can only be set once.
Normally the itemId is set by the DBMS, but if you know the itemId for a DscItem 
in the database you can create an empty CDscItem, set the itemId+dscId and let the DBMS look 
it up and fill in the other values.
@internalComponent
@released
@param aItemId Id of the item. 
@panic if you try to change the value
*/		
EXPORT_C void CDscItem::SetItemId(const TInt aItemId)
	{
	if(aItemId != iItemId)
		{
		__ASSERT_ALWAYS( 0==iItemId, PanicNow(KPanicDsc, EIdCannotChange));
		iItemId = aItemId;	
		}
	}
	
