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
// Comms Database Template Record Table View defintions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include "cdbtemp.h"
#include "CDBSTD.H"
#include <commdb.h>
#include "commdb_impl.H"

const TBool KTHidden				= ETrue;
const TBool KTNotReadOnly			= EFalse;
const TBool KTTemplateCall			= ETrue;

//
// CCommsDbTemplateRecord definitions
//

EXPORT_C CCommsDbTemplateRecord* CCommsDbTemplateRecord::NewL(CCommsDatabaseBase* aDb,const TDesC& aTableName)
/** Allocates and constructs a view, which includes only the template record, on 
the table whose name is defined in the specified descriptor and which is in 
the specified communications database.

The view is restricted to the single template record in the table.

The communications database, pointed to by aDb should already have been opened.

@param aDb A pointer to the communications database object. This pointer must 
not be NULL. 
@param aTableName A reference to a descriptor containing the name of a table 
in the communications database. 
@return A pointer to the view object which just includes the template record. */
	{
	CCommsDbTemplateRecord* r=new(ELeave) CCommsDbTemplateRecord();
	CleanupStack::PushL(r);
	r->ConstructL(aDb,aTableName);
	CleanupStack::Pop(r);
	return r;
	}

CCommsDbTemplateRecord::CCommsDbTemplateRecord()
/**
Default Constructor
*/
	{}

void CCommsDbTemplateRecord::ConstructL(CCommsDatabaseBase* aDb,const TDesC& aTableName)
	{
    iView=CCommsDbTableView::NewLC(aTableName, *aDb);       
	iView->iTableExt->SetShowHiddenRecords(ETrue);	// they've specifically asked for a template view
	CleanupStack::Pop(iView); 
    
    if (!iView->TableWithTemplateRecord())
		{
		User::Leave(KErrNotSupported);
        }
	//2aDb.
	}

EXPORT_C CCommsDbTemplateRecord::~CCommsDbTemplateRecord()
/** Frees all resources owned by this object, prior to its destruction. Specifically, 
it closes the view. */
	{
	delete iView;
	}

EXPORT_C TInt CCommsDbTemplateRecord::Modify()
/** Prepares to update the template record, if it exists. If the template record 
does not exist, it is created and left ready for modification. The function must 
be called before writing to any column. Once all changes to the template record 
are complete, a call must be made to either StoreModifications() or 
CancelModifications() as appropriate.

From v9.1 template record always exist

This function raises a CommsDbServer 1 or a CommsDbServer 2 panic if a previous 
call to Modify() has already been made.

This function must be called before any attempt is made to write to a column 
otherwise subsequent write operations raise a CommsDbServer 12 panic.

Once this function has completed successfully, no attempt can be made to read 
from a column until either StoreModifications() or CancelModifications() has 
been called to complete the record insertion operation, otherwise the read 
operations raise a CommsDbServer 10 panic.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
    TBool hidden = SetHiddenMask();

	TInt retval = Reposition();
	if (iRecordExists)
		{
		retval = iView->UpdateRecord();
		}
	else
		{
		// if record doesn't exist insert one
		retval = iView->InsertTemplateRecord();
		}

    ClearHiddenMask(hidden);
    
    return retval;
    }

EXPORT_C TInt CCommsDbTemplateRecord::Delete()
/** Deletes the template record.

This function raises a CommsDbServer 3 panic if a previous call to Modify() 
has been made.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability Dependent on table, see the guide page referenced below. */
	{
    TBool hidden = SetHiddenMask();

    TInt ret = Reposition();
	
	if(ret == KErrNone)
        {
	    if(iRecordExists)
		    {
		    ret = iView->DeleteRecord();
		    }
	    else
	    	{
		    ret = KErrNotFound;
		    }
        }

    ClearHiddenMask(hidden);

    return ret;
	}

EXPORT_C TInt CCommsDbTemplateRecord::StoreModifications()
/** Confirms changes made to the template record which were started by a call to 
Modify().

A call to Modify() must have previously been made otherwise the function raises 
a CommsDbServer 4 panic.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability Dependent on table, see the guide page referenced below. */
	{
	TInt ret=iView->DoPutRecordChanges(KTHidden,KTNotReadOnly,KTTemplateCall);
	if (ret==KErrNone)
		{
		iRecordExists=ETrue;
		}
	return ret;
	}

EXPORT_C void CCommsDbTemplateRecord::CancelModifications()
/** Abandons changes made to the template record which were started by a call to 
Modify().

A call to Modify() must have previously been made otherwise the function raises 
a CommsDbServer 5 panic.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
    TBool hidden = SetHiddenMask();
	iView->CancelRecordChanges();
    ClearHiddenMask(hidden);
	}

EXPORT_C void CCommsDbTemplateRecord::ReadTextL(const TDesC& aColumn, TDes8& aValue)
/** Reads narrow (ASCII) text located in a specific column within the template 
record and copies it to the specified 8 bit modifiable descriptor. The column 
is identified by the name supplied in the descriptor aColumn.

The maximum length of text expected by this function is the value of the constant 
KCommsDbSvrMaxColumnNameLength. The maximum length of aValue supplied by the 
caller can, therefore, be the same.

If the template record does not exist, this function will just return with an empty 
aValue desciptor.

This read operation must not occur if any of the operations involved in changing 
the template record are still outstanding, otherwise the function raises a 
CommsDbServer 10 panic. For example, the panic is raised if this function 
is called between calls to Modify() and StoreModifications().

The CommsDbServer 10 panic is also raised if this function is called after 
the view has been closed.

@param aColumn A reference to a descriptor containing the name of the column 
in the template record whose (narrow text) value is to be read. 
@param aValue A reference to an 8 bit descriptor passed by the caller.
@capability Dependent on table, see the guide page referenced below. */

	{
	Reposition();
	if (iRecordExists)
		{
		iView->ReadTextL(aColumn,aValue);
		}
	else
		{
		aValue.SetLength(0);
		}
	}

EXPORT_C void CCommsDbTemplateRecord::ReadTextL(const TDesC& aColumn, TDes16& aValue)
/** Reads wide (UNICODE) text located in a specific column within the template 
record and copies it to the specified 16 bit modifiable descriptor. The column 
is identified by the name supplied in the descriptor aColumn.

The maximum length of text expected by this function is the value of the constant 
KCommsDbSvrMaxColumnNameLength. The maximum length of aValue supplied by the 
caller can, therefore, be the same.

This read operation must not occur if any of the operations involved in changing 
the template record are still outstanding, otherwise the function raises a 
CommsDbServer 10 panic. For example, the panic is raised if this function 
is called between calls to Modify() and StoreModifications().

The CommsDbServer 10 panic is also raised if this function is called after 
the view has been closed.

If the template record does not exist, this function will just return with an empty 
aValue desciptor.

@param aColumn A reference to a descriptor containing the name of the column 
in the template record whose (wide text) value is to be read. 
@param aValue A reference to a 16 bit descriptor passed by the caller.
@capability Dependent on table, see the guide page referenced below. */
	{
	Reposition();
	if (iRecordExists)
		{
		iView->ReadTextL(aColumn,aValue);
		}
	else
		{
		aValue.SetLength(0);
		}
	}

EXPORT_C HBufC* CCommsDbTemplateRecord::ReadLongTextLC(const TDesC& aColumn)
/** Reads the long text located in a specific column within the template record 
and copies this text to a heap descriptor. The heap descriptor is allocated 
and its pointer returned by this function. The column is identified by the 
name supplied in the descriptor aColumn.

While the text in columns retrieved by the ReadTextL() functions is limited 
in length, there is no restriction on the length of long text.

If the template record does not exist, this function will just return an empty 
desciptor. Also in this case, the caller is still responsible for removing this 
object from the cleanup stack.

This read operation must not occur if any of the operations involved in changing 
a record are still outstanding, otherwise the function raises a CommsDbServer 
10 panic. For example, the panic is raised if this function is called between 
calls to Modify() and StoreModifications().

The CommsDbServer 10 panic is also raised if this function is called after 
the view has been closed.

@param aColumn A reference to a descriptor containing the name of a column 
in the template record. 
@return Pointer to a heap descriptor containing the long text. 
@capability Dependent on table, see the guide page referenced below. */
	{
	Reposition();
	HBufC* buf;
	if (iRecordExists)
		{
		buf=iView->ReadLongTextLC(aColumn);
		}
	else
		{
		buf=HBufC::NewLC(0);
		}
	return buf;
	}

EXPORT_C void CCommsDbTemplateRecord::ReadUintL(const TDesC& aColumn, TUint32& aValue)
/** Reads an unsigned integer value located in a specific column within the template 
record and copies it into the specified unsigned integer. The column is identified 
by the name supplied in the descriptor aColumn.

This read operation must not occur if any of the operations involved in changing 
the template record are still outstanding, otherwise the function raises a 
CommsDbServer 10 panic. For example, the panic is raised if this function 
is called between calls to Modify() and StoreModifications().

The CommsDbServer 10 panic is also raised if this function is called after 
the view has been closed.

If the template record does not exist then the function leaves with KErrUnknown.

@param aColumn A reference to a descriptor containing the name of the column 
in the template record whose (unsigned integer) value is to be read. 
@param aValue A reference to an unsigned integer passed by the caller. 
@capability Dependent on table, see the guide page referenced below. */
	{
	Reposition();
	if (iRecordExists)
		{
		iView->ReadUintL(aColumn,aValue);
		}
	else
		{
		User::Leave(KErrUnknown);
		}
	}

EXPORT_C void CCommsDbTemplateRecord::ReadBoolL(const TDesC& aColumn, TBool& aValue)
/** Reads a boolean value located in a specific column within the template record 
and copies it into the specified descriptor. The column is identified by the 
name supplied in the descriptor aColumn.

This read operation must not occur if any of the operations involved in changing 
the template record are still outstanding, otherwise the function raises a 
CommsDbServer 10 panic. For example, the panic is raised if this function 
is called between calls to Modify() and StoreModifications().

The CommsDbServer 10 panic is also raised if this function is called after 
the view has been closed.

If the template record does not exist then the function leaves with KErrUnknown.

@param aColumn A reference to a descriptor containing the name of the column 
in the template record whose (boolean) value is to be read. 
@param aValue A reference to a TBool passed by the caller. 
@capability Dependent on table, see the guide page referenced below. */
	{
	Reposition();
	if (iRecordExists)
		{
		iView->ReadBoolL(aColumn,aValue);
		}
	else
		{
		User::Leave(KErrUnknown);
		}
	}

EXPORT_C void CCommsDbTemplateRecord::ReadTypeAttribL(const TDesC& aColumn, TDbColType& aColType, TUint32& aAttrib)
/** Gets the type and the attributes of a specific column within the template record 
and puts them into a reference to an object and a reference to an unsigned 
integer respectively. The column is identified by the name supplied in the 
descriptor aColumn.

The column type is described by the TDbColType enumerator.

The column attributes are one or more of the values TDbCol::ENotNull and TDbCol::EAutoIncrement.

This function must not be called if any of the operations involved in changing 
the template record are still outstanding, otherwise the function raises a 
CommsDbServer 10 panic. For example, the panic is raised if this function 
is called between calls to Modify() and StoreModifications().

@param aColumn A reference to a descriptor containing the name of the column 
in the template record whose type and attributes are to be fetched. 
@param aColType A reference to a TDbColType object passed by the caller. On 
successful return from this function, contains a copy of the column type.
@param aAttrib A reference to an unsigned integer passed by the caller. On 
successful return from this function, contains a copy of the column attributes.
@capability Dependent on table, see the guide page referenced below. */
	{
	Reposition();
	iView->ReadTypeAttribL(aColumn,aColType,aAttrib);
	}

EXPORT_C void CCommsDbTemplateRecord::ReadColumnLengthL(const TDesC& aColumn, TInt& aLength)
/** Gets the length of a specific column within the template record and copies 
it to an integer passed by the caller. The column is identified by the name 
supplied in the descriptor aColumn.

This read operation must not occur if any of the operations involved in changing 
a template record are still outstanding, otherwise the function raises a CommsDbServer 
10 panic. For example, the panic is raised if this function is called between 
calls to Modify() and StoreModifications().

The CommsDbServer 10 panic is also raised if this function is called after 
the view has been closed.

If the template record does not exist, this function will just return with aLength set to 
zero.

@param aColumn A reference to a descriptor containing the name of a column 
in the template record. 
@param aLength A reference to an integer passed by the caller. On successful 
return from this function, contains a copy of the length of the column.
@capability Dependent on table, see the guide page referenced below. */
	{
	Reposition();
	if (iRecordExists)
		{
		iView->ReadColumnLengthL(aColumn,aLength);
		}
	else
		{
		aLength=0;
		}
	}

EXPORT_C void CCommsDbTemplateRecord::WriteTextL(const TDesC& aColumn, const TDesC8& aValue)
/** Writes the narrow (ASCII) text from the specified 8 bit descriptor to a specific 
column within the template record. The column is identified by the name supplied 
in the descriptor aColumn.

An earlier call to Modify() must have been made before calling this function 
otherwise the function raises a CommsDbServer 12 panic.

@param aColumn A reference to a descriptor containing the name of a column 
in the template record. 
@param aValue A reference to an 8 bit descriptor containing the narrow text 
to be written into the column. 
@leave  The function can leave for reasons defined by DBMS. It will also 
leave if the length of the text is greater than the maximum permitted - 
equal to the value of the constant KCommsDbSvrMaxColumnNameLength.
KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	iView->WriteTextL(aColumn,aValue);
	}

EXPORT_C void CCommsDbTemplateRecord::WriteTextL(const TDesC& aColumn, const TDesC16& aValue)
/** Writes the wide (Unicode) text from the specified 16 bit descriptor to a specific 
column within the template record. The column is identified by the name supplied 
in the descriptor aColumn.

An earlier call to Modify() must have been made before calling this function 
otherwise the function raises a CommsDbServer 12 panic.

The CommsDbServer 12 panic is also raised if this function is called after 
the view has been closed.

@param aColumn A reference to a descriptor containing the name of a column 
in the current record. 
@param aValue A reference to a 16 bit descriptor containing the wide text to 
be written into the column. 
@leave The function can leave for reasons defined by DBMS. It will also 
leave if the length of the text is greater than the maximum permitted - 
equal to the value of the constant KCommsDbSvrMaxColumnNameLength.
KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	iView->WriteTextL(aColumn,aValue);
	}

EXPORT_C void CCommsDbTemplateRecord::WriteLongTextL(const TDesC& aColumn, const TDesC& aValue)
/** Writes the long text from a specified descriptor to a specific column within 
the template record. The column is identified by the name supplied in the 
descriptor aColumn.

An earlier call to Modify() must have been made before calling this function 
otherwise the function raises a CommsDbServer 12 panic.

The CommsDbServer 12 panic is also raised if this function is called after 
the view has been closed.

While the text written by WriteTextL() functions is limited in length, there 
is no restriction on the length of long text.

@param aColumn A reference to a descriptor containing the name of a column 
in the template record. 
@param aValue A reference to a descriptor containing the long text to be written 
into the column. 
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	iView->WriteLongTextL(aColumn,aValue);
	}

EXPORT_C void CCommsDbTemplateRecord::WriteUintL(const TDesC& aColumn, const TUint32& aValue)
/** Writes a specified unsigned integer value to a specific column within the template 
record. The column is identified by the name supplied in the descriptor aColumn.

An earlier call to Modify() must have been made before calling this function 
otherwise the function raises a CommsDbServer 12 panic.

The CommsDbServer 12 panic is also raised if this function is called after 
the view has been closed.

The column being changed must not be the Id column (symbolic name COMMDB_ID) 
otherwise the function raises a CommsDbServer 13 panic.

@param aColumn A reference to a descriptor containing the name of a column 
in the template record. 
@param aValue A reference to an unsigned integer containing the value to be 
written into the column. 
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	iView->WriteUintL(aColumn,aValue);
	}

EXPORT_C void CCommsDbTemplateRecord::WriteBoolL(const TDesC& aColumn, const TBool& aValue)
/** Writes a specified boolean value to a specific column within the template record. 
The column is identified by the name supplied in the descriptor aColumn.

An earlier call to Modify() must have been made before calling this function 
otherwise the function raises a CommsDbServer 12 panic.

The CommsDbServer 12 panic is also raised if this function is called after 
the view has been closed.

@param aColumn A reference to a descriptor containing the name of a column 
in the template record. 
@param aValue A reference to TBool containing the value to be written into 
the column. 
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	iView->WriteBoolL(aColumn,aValue);
	}

EXPORT_C void CCommsDbTemplateRecord::GetTableName(TDes& aTableName) const
/** Gets the name of the table associated with this view and copy it into the descriptor 
supplied by the caller.

@param aTableName A reference to a descriptor passed by the caller. On return 
from this function it contains the name of the table. */
	{
	iView->GetTableName(aTableName);
	}

EXPORT_C TBool CCommsDbTemplateRecord::TemplateRecordExists()
/** Tests whether a template record exists in this table.

@return ETrue, if a template record exists; EFalse, otherwise */
	{
	(void) Reposition();	// it seems wrong to ignore error, but old API does not allow for handling this better
	return iRecordExists;
	}
	
EXPORT_C TInt CCommsDbTemplateRecord::InsertCopy(TUint32& aId)
/**
Creates a new record based on the contents of the template record 

@param aId On return contains the unique Id associated with this new record.
@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below. 
*/
	{
    TBool hidden = SetHiddenMask();

	TInt ret = Reposition();
	if (iRecordExists)
        {
    	ret = iView->InsertCopyRecord(aId);
        }

    ClearHiddenMask(hidden);

	return ret;
	}

TInt CCommsDbTemplateRecord::Reposition()
	{
    TBool hidden = SetHiddenMask();

	TInt ret=iView->GotoFirstRecord();
	if (ret==KErrNone)
        {
        iRecordExists=ETrue;
        }
	else if(ret != KErrNoMemory)
        {
        iRecordExists=EFalse;	
		ret = KErrNone;
        }

    ClearHiddenMask(hidden);
	return ret;
	}


void CCommsDbTemplateRecord::ClearHiddenMask(TBool aClearHiddenMask)
    {
    if (aClearHiddenMask)
        {
        iView->iDb.iImpl->iDbs->ClearAttributeMask(ECDHidden);
        }
    }

TBool CCommsDbTemplateRecord::SetHiddenMask()
    {
	CMDBSession* dbSession = iView->iDb.iImpl->iDbs;
    if (! dbSession->IsSetAttributeMask(ECDHidden))
        {
        dbSession->SetAttributeMask(ECDHidden);
        return ETrue;
        }
    else
        return EFalse;
    }

