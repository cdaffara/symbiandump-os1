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
// CCommDbOverrideSettings class defintions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include "cdbover.h"
#include "OVERRIDE.H"
#include "CDBSTD.H"
#include <commsdattypeinfov1_1.h>

EXPORT_C CCommDbOverrideSettings* CCommDbOverrideSettings::NewL(TParamList aParamList)
/** Allocates and constructs an override settings object.

The construction process automatically opens the communications database and 
connects to the DBMS. The database is closed and the connection to the DBMS 
is severed when this override object is destroyed.

This function was withdrawn in 6.0, but reintroduced in 7.0.

@param aParamList Unused except	when comparing two sets.
@return A pointer to the new override settings object. */
	{
	CCommDbOverrideSettings* r=new(ELeave) CCommDbOverrideSettings(aParamList);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

EXPORT_C CCommDbOverrideSettings* CCommDbOverrideSettings::NewL(TParamList aParamList, TCommDbDatabaseType /*aDbType*/)
/** @deprecated 7.0

Allocates and constructs an override settings object.
The construction process automatically opens the communications database and connects to the DBMS. 
The database is closed and the connection to the DBMS is severed when this override object is destroyed.
Re-instated override variant of NewL function in order to maintain BC with 6.1

@param aParamList Parameter list.
@param aDbType Whether Database is IAP or ISP version. All Db's are IAP type now.
@return CCommDbOverrideSettings* the calling function takes ownership of the returned object.
*/
	{
	return CCommDbOverrideSettings::NewL(aParamList);
	}

CCommDbOverrideSettings::CCommDbOverrideSettings(TParamList aParamList)
	: iPartialFull(aParamList)
/**
Constructor 

@param aParamList An enumerator which indicates whether the override settings are partial or full. 
*/
	{}

void CCommDbOverrideSettings::ConstructL()
	{
	iDb=CCommsDatabase::NewL();
	}

EXPORT_C CCommDbOverrideSettings::~CCommDbOverrideSettings()
/** Frees all resources owned by this object, prior to its destruction. Specifically, 
it closes the communications database and severs the connection with the DBMS. */
	{
	iOverrides.ResetAndDestroy();
	iIapOverrides.Reset();

	delete iDb;
	}

EXPORT_C TInt CCommDbOverrideSettings::SetIntOverride(const TDesC& aTableName, const TDesC& aColumnName, TUint32 aValue)
/** Sets an override value for the specified unsigned integer type column in the specified table in the 
communications database.
If the column name is empty, i.e. the length of the descriptor aColumnName is zero, then the override 
value is assumed to be the overriding ID for the default record for table aTableName. This means that 
the table must be one which supports default records.

For the function to succeed:
The column must exist
The column type must be an unsigned integer
An override for this column (or the overriding default record ID) for this table must not already exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the 
communications database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table 
aTableName. If this descriptor is empty, i.e. its length is zero, then aValue is assumed to be the 
overriding ID for the default record. 
@param aValue  The override value. 

@return KErrNone on success; KErrNotSupported if the specified table does not have a default (when overriding the 
default) or the table does not support overrides; KErrNotFound if the specified table and/or column does not exist; 
KErrAlreadyExists if the specified column is already overridden. */	
	{
	TRAPD(ret,CheckL(aTableName,aColumnName,EIntValue));
	if (ret!=KErrNone)
		return ret;

	ret = FindOverride(aTableName,aColumnName,EIntValue);
	if (ret >= 0)
		{
		ret = KErrAlreadyExists;
		}
	else if (ret == KErrNotFound)
		{
		CCommDbOverride* override = new CCommDbIntOverride(aTableName,aColumnName);
		if (override != 0)
			{
			((CCommDbIntOverride*) override)->iValue = aValue;
			ret = iOverrides.Append(override);
			}
		else
			ret = KErrNoMemory;
		}

	return ret;
	}

EXPORT_C TInt CCommDbOverrideSettings::SetBoolOverride(const TDesC& aTableName, const TDesC& aColumnName, TBool aValue)
/** Sets an override value for a boolean type column in the specified table in the communications database.
For the function to succeed:
The column must exist
The column type must be boolean
An override for this column for this table must not already exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  The override value. 

@return KErrNone on success; KErrNotSupported if the specified table does not have a default (when overriding the 
default) or the table does not support overrides; KErrNotFound if the specified table and/or column does not exist; 
KErrAlreadyExists if the specified column is already overridden. */
	{
	TRAPD(ret,CheckL(aTableName,aColumnName,EBoolValue));
	if (ret!=KErrNone)
		return ret;

	ret = FindOverride(aTableName,aColumnName,EBoolValue);
	if (ret >= 0)
		{
		ret = KErrAlreadyExists;
		}
	else if (ret == KErrNotFound)
		{
		CCommDbOverride* override = new CCommDbBoolOverride(aTableName,aColumnName);
		if (override != 0)
			{
			((CCommDbBoolOverride*) override)->iValue = aValue;
			ret = iOverrides.Append(override);
			
			// if Append failed delete override
			if ( ret != KErrNone ) delete override;
				
			}
		else
			ret = KErrNoMemory;
		}
	return ret;
	}

EXPORT_C TInt CCommDbOverrideSettings::SetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, const TDesC8& aValue)
/** Sets an override value for an 8 bit descriptor text type column in the specified table in the communications 
database.

For the function to succeed:
The column must exist
The column type must be narrow (ASCII) text
An override for this column for this table must not already exist.
The table must be one which supports override settings

In addition, the maximum length of text expected by this function is the value of the constant 
KCommsDbSvrMaxFieldLength. The length of aValue supplied by the caller cannot be greater than this value.

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  The override value. 

@return KErrNone on success; KErrNotSupported if the specified table does not have a default (when overriding the 
default) or the table does not support overrides; KErrNotFound if the specified table and/or column does not exist; 
KErrAlreadyExists if the specified column is already overridden.*/
	{
	TRAPD(ret,CheckL(aTableName,aColumnName,EDes8Value));
	if (ret!=KErrNone)
		return ret;

	TInt fieldWidth(0);
	CCommsDbTableView* tableView = 0;
	TRAP(ret, tableView = iDb->OpenTableLC(aTableName); CleanupStack::Pop(tableView));
	if (ret != KErrNone)
		{
		return ret;
		}

	TRAP(ret, tableView->ReadColumnMaxLengthL(aColumnName, fieldWidth));
	if (ret != KErrNone)
		{
		delete tableView;
		return ret;
		}

	if (aValue.Length()< fieldWidth)
		{
		ret = FindOverride(aTableName,aColumnName,EDes8Value);
		if (ret >= 0)
			{
			ret = KErrAlreadyExists;
			}
		else if (ret == KErrNotFound)
			{
			CCommDbOverride* override = new CCommDbDes8Override(aTableName,aColumnName);
			if (override != 0)
				{
				((CCommDbDes8Override*) override)->iValue.Copy(aValue);
				ret = iOverrides.Append(override);

				// if Append failed delete override
				if ( ret != KErrNone ) delete override;
				}
			else
				{
				ret = KErrNoMemory;
				}
			}
		}
	else
		{
			ret = KErrOverflow;
		}

	delete tableView;
	return ret;

	}

EXPORT_C TInt CCommDbOverrideSettings::SetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, const TDesC16& aValue)
/** Sets an override value for a 16 bit descriptor text type column in the specified table in the communications 
database.

For the function to succeed:
The column must exist
The column type must be wide (UNICODE) text
An override for this column for this table must not already exist
The table must be one which supports override settings

In addition, the maximum length of text expected by this function is the value of the constant 
KCommsDbSvrMaxFieldLength. The length of aValue supplied by the caller cannot be greater than this value.

@param aTableName  A reference to a descriptor containing the name of a table in the communications database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  The override value. 


@return KErrNone on success; KErrNotSupported if the specified table does not have a default (when overriding the 
default) or the table does not support overrides; KErrNotFound if the specified table and/or column does not exist; 
KErrAlreadyExists if the specified column is already overridden. */
	{
	TRAPD(ret,CheckL(aTableName,aColumnName,EDes16Value));
	if (ret!=KErrNone)
		return ret;

	TInt fieldWidth(0);
	CCommsDbTableView* tableView = 0;
	TRAP(ret, tableView = iDb->OpenTableLC(aTableName); CleanupStack::Pop(tableView));
	if (ret != KErrNone)
		{
		return ret;
		}

	TRAP(ret, tableView->ReadColumnMaxLengthL(aColumnName, fieldWidth));
	if (ret != KErrNone)
		{
		delete tableView;
		return ret;
		}

	if (aValue.Length()< fieldWidth)
		{
		ret = FindOverride(aTableName,aColumnName,EDes16Value);
		if (ret >= 0)
			{
			ret = KErrAlreadyExists;
			}
		else if (ret == KErrNotFound)
			{
			CCommDbOverride* override = new CCommDbDes16Override(aTableName,aColumnName);
			if (override != 0)
				{
				((CCommDbDes16Override*) override)->iValue.Copy(aValue);
				ret = iOverrides.Append(override);

				// if Append failed delete override
				if ( ret != KErrNone ) delete override;
				}
			else
				{
				ret = KErrNoMemory;
				}
			}
		}
	else
		{
			ret = KErrOverflow;
		}

	delete tableView;
	return ret;
	}

EXPORT_C TInt CCommDbOverrideSettings::SetLongDesOverride(const TDesC& aTableName, const TDesC& aColumnName, const TDesC& aValue)
/** Sets an override value for a long text type column in a specified table in the communications database.
 
For the function to succeed:
The column must exist
The column type is assumed to be arbitrarily long text (within the limits defined by descriptors)
An override for this column for this table must not already exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  The override value. 

@return KErrNone on success; KErrNotSupported if the specified table does not have a default (when overriding the 
default) or the table does not support overrides; KErrNotFound if the specified table and/or column does not exist; 
KErrAlreadyExists if the specified column is already overridden. */
	{
	TRAPD(ret,CheckL(aTableName,aColumnName,ELongDesValue));
	if (ret!=KErrNone)
		return ret;

	ret = FindOverride(aTableName,aColumnName,ELongDesValue);
	if (ret >= 0)
		{
		ret = KErrAlreadyExists;
		}
	else if (ret == KErrNotFound)
		{
		CCommDbOverride* override = 0;
		TRAP(ret,(override=CCommDbLongDesOverride::NewL(aTableName,aColumnName,aValue)));
		if (ret==KErrNone)
			{
			ret = iOverrides.Append(override);

			// if Append failed delete override
			if ( ret != KErrNone ) delete override;
			}
		}
	return ret;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetIntOverride(const TDesC& aTableName, const TDesC& aColumnName, TUint32& aValue)
/** Gets an override value for an unsigned integer type column in the specified table in the communications 
database. 

If the column name is empty, i.e. the length of the descriptor aColumnName is zero, then the override 
value is assumed to be the overriding Id for the default record for table aTableName.

For this function to succeed:
An override for this column (or the the overriding default record Id) for this table must exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  An unsigned integer type passed by the caller. On successful return from this function, it 
contains the override value. 

@return KErrNone on success; KErrNotFound if the specified table and/or column has not been overridden or does not 
exist */
	{
	TInt index=FindOverride(aTableName,aColumnName,EIntValue);
	if (index < 0)
		return KErrNotFound;

	aValue=((CCommDbIntOverride*)(iOverrides[index]))->iValue;
	return KErrNone;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetBoolOverride(const TDesC& aTableName, const TDesC& aColumnName, TBool& aValue)
/** Gets an override value for a boolean type column in the specified table in the communications database.

For this function to succeed:
An override for this column for this table must exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  A boolean type passed by the caller. On successful return from this function, it contains 
the override value. 

@return KErrNone on success; KErrNotFound if the specified table and/or column has not been overridden or does not 
exist. */
	{
	TInt index=FindOverride(aTableName,aColumnName,EBoolValue);
	if (index < 0)
		return KErrNotFound;

	aValue=((CCommDbBoolOverride*)(iOverrides[index]))->iValue;
	return KErrNone;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, TDes8& aValue)
/** Gets an override value for an 8 bit descriptor text type column in the specified table in the communications
 database.

For this function to succeed:
An override for this column for this table must exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  An 8 bit type descriptor passed by the caller. On successful return from this function, it 
contains the override value. This parameter must be as long as KCommsDbSvrMaxFieldLength or the function will fail.

@return KErrNone on success; KErrNotFound if the specified table and/or column has not been overridden or does not 
exist; KErrOverflow if the aValue parameter was too small.*/
	{
	TInt fieldWidth(0);
	TInt returnValue = KErrNone;
	CCommsDbTableView* tableView = 0;
	TRAP(returnValue, tableView = iDb->OpenTableLC(aTableName); CleanupStack::Pop(tableView));
	if (returnValue != KErrNone)
		{
		return returnValue;
		}

	TRAP(returnValue, tableView->ReadColumnMaxLengthL(aColumnName, fieldWidth));
	if (returnValue != KErrNone)
		{
		delete tableView;
		return returnValue;
		}

	//TG-hack to restrict OutgoingGPRS/APN to 252 chars to avoid overflow
	TInt maxLength = aValue.MaxLength();
	if((aTableName.CompareF(TPtrC(KCDTypeNameOutgoingWCDMA)) == 0 || aTableName.CompareF(TPtrC(KCDTypeNameIncomingWCDMA)) == 0 ) 
		&& aColumnName.CompareF(TPtrC(KCDTypeNameGPRSAPN)) == 0)
		{
		fieldWidth = 252;	// KGSNNameLength from etelpckt.h
		}

	if (aValue.MaxLength() >= fieldWidth)
		{
		TInt index=FindOverride(aTableName,aColumnName,EDes8Value);
		if (index >= 0)
			{
			aValue.Copy(((CCommDbDes8Override*)(iOverrides[index]))->iValue);
			}
		else
			{
			returnValue = KErrNotFound;
			}
		}
	else
		{
		returnValue = KErrOverflow;
		}

	delete tableView;
	return returnValue ;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, TDes16& aValue)
/**Gets an override value for a 16 bit descriptor text type column in the specified table in the communications 
database.

For this function to succeed:
An override for this column for this table must exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  A 16 bit type descriptor passed by the caller. On successful return from this function, it 
contains the override value. This parameter must be as long as KCommsDbSvrMaxFieldLength or the function will fail.

@return KErrNone on success; KErrNotFound if the specified table and/or column has not been overridden or does not 
exist; KErrOverflow if the aValue parameter was too small.*/
	{
	TInt fieldWidth(0);
	TInt returnValue = KErrNone;
	CCommsDbTableView* tableView = 0;
	TRAP(returnValue, tableView = iDb->OpenTableLC(aTableName); CleanupStack::Pop(tableView));
	if (returnValue != KErrNone)
		{
		return returnValue;
		}

	TRAP(returnValue, tableView->ReadColumnMaxLengthL(aColumnName, fieldWidth));
	if (returnValue != KErrNone)
		{
		delete tableView;
		return returnValue;
		}

	if (aValue.MaxLength() >= fieldWidth)
		{
		TInt index=FindOverride(aTableName,aColumnName,EDes16Value);
		if (index >= 0)
			{
			aValue.Copy(((CCommDbDes16Override*)(iOverrides[index]))->iValue);
			}
		else
			{
			returnValue = KErrNotFound;
			}
		}
	else
		{
		returnValue = KErrOverflow;
		}

	delete tableView;
	return returnValue ;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetLongDesOverride(const TDesC& aTableName, const TDesC& aColumnName, TDes& aValue)
/**Gets the override value for a column in the specified table in the communications database.

For this function to succeed:
An override for this column for this table must exist
The table must be one which supports override settings
Notes:
The maximum length of the descriptor aValue must be large enough to contain the override text. 
Use the GetLongDesOverrideLength() function to find the length of this text.

@param aTableName  A reference to a descriptor containing the name of a table in the communications 
database. 
@param aColumnName  A reference to a descriptor containing the name of a column in table aTableName. 
@param aValue  A descriptor passed by the caller. On successful return from this function, it contains the 
override value. 

@return KErrNone on success; KErrNotFound if the specified table and/or column has not been overridden or does not 
exist. */
	{
	TInt index=FindOverride(aTableName,aColumnName,ELongDesValue);
	if (index < 0)
		return KErrNotFound;

	TPtr ptr(((CCommDbLongDesOverride*)(iOverrides[index]))->iValue->Des());

	if (aValue.MaxLength() < ptr.Length())
		{
		return KErrOverflow;
		}

	aValue.Copy(ptr);
	return KErrNone;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetLongDesOverrideLength(const TDesC& aTableName, const TDesC& aColumnName, TInt& aLength)
/**Gets the length of the long text override value for a column in the specified table in the communications database.

For this function to succeed:
An override for this column for this table must exist
The table must be one which supports override settings

@param aTableName  A reference to a descriptor containing the name of a table in 
the communications database. 
@param aColumnName  A reference to a descriptor containing the name of a column in 
table aTableName. 
@param aLength  A signed integer type passed by the caller. On successful return from 
this function, it contains the override value. 

@return KErrNone on success; KErrNotFound if the specified table and/or column has not been overridden or does not exist; 
KErrOverflow if the aValue parameter was too small.*/
	{
	TInt index=FindOverride(aTableName,aColumnName,ELongDesValue);
	if (index < 0)
		return KErrNotFound;

	aLength=((CCommDbLongDesOverride*)(iOverrides[index]))->iValue->Des().Length();
	return KErrNone;
	}

EXPORT_C TBool CCommDbOverrideSettings::Compare(CCommDbOverrideSettings* aOverrides) const
/** Compares aOverrides with this class.

@param aOverrides Overrides.
@return ETrue if aOverrides has the same as this object; EFalse if not. */
	{
	//Compare *this override values with the passed override values and
	//return true if all fields match else return false

	//compare override count
	TInt overrideCount = iOverrides.Count();
	if(overrideCount != aOverrides->iOverrides.Count())
		return EFalse;

	//If everything else checks out then compare override types and values
	TInt i;
	for(i = 0; i < overrideCount; ++i)
		{
		const CCommDbOverride* myOverride = iOverrides[i];

		TInt found = aOverrides->FindOverride(myOverride->iTableName,
								myOverride->iColumnName, myOverride->iType);
		if (found < 0)
			return EFalse;

		// have found one now check its value
		const CCommDbOverride* argOverride = aOverrides->iOverrides[found];

		switch (myOverride->iType)
			{
			case EIntValue:
				if (((CCommDbIntOverride*) myOverride)->iValue !=
					((CCommDbIntOverride*) argOverride)->iValue)
					return EFalse;
				break;
			case EBoolValue:
				if (((CCommDbBoolOverride*) myOverride)->iValue !=
					((CCommDbBoolOverride*) argOverride)->iValue)
					return EFalse;
				break;
			case EDes8Value:
				if (((CCommDbDes8Override*) myOverride)->iValue !=
					((CCommDbDes8Override*) argOverride)->iValue)
					return EFalse;
				break;
			case EDes16Value:
				if (((CCommDbDes16Override*) myOverride)->iValue !=
					((CCommDbDes16Override*) argOverride)->iValue)
					return EFalse;
				break;
			case ELongDesValue:
				if (*((CCommDbLongDesOverride*) myOverride)->iValue !=
					*((CCommDbLongDesOverride*) argOverride)->iValue)
					return EFalse;
				break;
			default:
				return EFalse;
			}
		}

	// compare connection preference override count
	overrideCount = iIapOverrides.Count();
	if(overrideCount != aOverrides->iIapOverrides.Count())
		return EFalse;

	// compare connection preference override setting
	for(i = 0; i < overrideCount; ++i)
		{
		// Compare value of override parameter with that which was passed in
		if((aOverrides->iIapOverrides.Find(iIapOverrides[i], MatchIapOverride)) < 0)
			{
			return EFalse;
			}
		}

	return ETrue;
	}

TBool CCommDbOverrideSettings::MatchIapOverride(const TCommDbIapConnectionPrefOverride& aFirst,
												const TCommDbIapConnectionPrefOverride& aSecond)
/**
Checks if the dialog preference fields in the passed objects of TCommDbIapConnectionPrefOverride are equal.

@param aFirst A reference to TCommDbIapConnectionPrefOverride class
@param aSecond A reference to TCommDbIapConnectionPrefOverride class
@return ETrue if the dialog preferences are equal else EFalse
*/
	{
	return aFirst.iPref == aSecond.iPref;
	}

EXPORT_C TBool CCommDbOverrideSettings::IsOverridden(const TDesC& aTableName, const TDesC& aColumnName, TValueType aType)
/** Tests whether an override value of a specified type exists in the communications 
database. 

@param aTableName A reference to a descriptor containing the name of a table 
in the communications database. 
@param aColumnName A reference to a descriptor containing the name of a column 
in table aTableName which is to be overridden. 
@param aType An enumeration which indicates the type of data in column aType 
which is to be overridden. 
@return ETrue if an override has been set for `aColumnName`; EFalse otherwise. */
	{
	// If Find returns an index then it `IsOverridden`
	return FindOverride(aTableName,aColumnName,aType) >= 0;
	}

EXPORT_C TInt CCommDbOverrideSettings::SetConnectionPreferenceOverride(const CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref& aPref)

/** Sets overrides for a record in the Connection preferences table.

The rank (CONNECT_PREF_RANKING and the direction (CONNECTION_PREF_DIRECTION 
part of the aPref parameter must be filled in to indicate which connection 
preference is to be overridden. It is therefore not possible to override the 
rank or direction of a connection preference.

The other fields should be filled in one of the following ways to override 
the other settings:

	Only dialog preference (CONNECT_PREF_DIALOG_PREF) is filled in to override 
	this field (all others should have zero value).

	Only bearer set and IAP ID (CONNECT_PREF_BEARER_SET) are filled in to override 
	these fields (all other fields should have zero value).

	The bearer set, IAP ID and the dialog preference are filled in to override 
	these fields.

CommDb treats zero values as fields that the client does not wish to override.


A note on overriding connection preferences. This is clearly a desirable thing 
to be able to do, so that the application may choose its own particular first 
and second preference for a connection. However, at the moment it is only 
possible to override one row of settings per table, i.e. it would only be 
possible to override the first ranked connection preferences or the second 
ranked connection preferences, but not both. To solve this problem, two API 
functions are added to the CCommDbOverrideSettings class specifically for 
overriding connection preferences.

@param aPref Connection preference settings by which to override.
@return An error code from an override setting function, e.g. SetIntOverride(), 
as documented above. */
	{
	if (aPref.iRanking == 0 || aPref.iDirection == ECommDbConnectionDirectionUnknown)
		return KErrNotSupported;

	if (aPref.iBearer.iIapId == 0 &&
		aPref.iDialogPref == ECommDbDialogPrefUnknown &&
		aPref.iBearer.iBearerSet == 0)
		return KErrArgument;

	for (TInt count = iIapOverrides.Count() - 1; count >= 0; count--)
		{
		CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref& currentPref = iIapOverrides[count].iPref;

		if (aPref.iRanking == currentPref.iRanking && aPref.iDirection == currentPref.iDirection)
			return KErrAlreadyExists;
		}

	TCommDbIapConnectionPrefOverride override;
	override.iPref = aPref;
	return iIapOverrides.Append(override);
	}

EXPORT_C TInt CCommDbOverrideSettings::SetConnectionPreferenceOverride(const CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref& /*aPref*/)
/**
@deprecated 7.0

Framework left in place for 6.1 compatibility purposes.
@param aPref Not supported.
@return KErrNotSupported*/
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetConnectionPreferenceOverride(CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref& aPref)
/** Gets the overrides for a record in the Connection preferences table
filtered by the contents of `aPref`. Any field in `aPref` may be ignored in the filter by
initialising with the following values:
aPref.iRanking == 0,
aPref.iDirection == ECommDbConnectionDirectionUnknown,
aPref.iDialogPref == ECommDbDialogPrefUnknown,
aPref.iBearer.iBearerSet == 0,
aPref.iBearer.iIapId == 0
The client should set the ranking and the direction of the aPref parameter to the values 
to be matched when getting the override, e.g. set the iRanking member to 1 
and iDirection to ECommDbConnectionDirectionOutgoing to retrieve the overrides 
for the outgoing connection preference with ranking 1.

@param aPref Initially specifies the record for which to get the overrides 
(the record is matched against the settings in aPref). On return, the override 
settings for that record.
@return KErrArgument if all the members of aPref are zero; KErrNotFound if 
no matching override is found. */
	{
	if (aPref.iRanking == 0 &&
		aPref.iDirection == ECommDbConnectionDirectionUnknown &&
		aPref.iDialogPref == ECommDbDialogPrefUnknown &&
		aPref.iBearer.iBearerSet == 0)
		return KErrArgument;

	for (TInt count = iIapOverrides.Count() - 1; count >= 0; count--)
		{
		CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref = iIapOverrides[count].iPref;

		if ((aPref.iRanking == 0 || aPref.iRanking == pref.iRanking) &&
			(aPref.iDirection == ECommDbConnectionDirectionUnknown || aPref.iDirection == pref.iDirection) &&
			(aPref.iDialogPref == ECommDbDialogPrefUnknown || aPref.iDialogPref == pref.iDialogPref) &&
			(aPref.iBearer.iBearerSet == 0 || aPref.iBearer.iBearerSet == pref.iBearer.iBearerSet) &&
			(aPref.iBearer.iIapId == 0 || aPref.iBearer.iIapId == pref.iBearer.iIapId))
			{
			aPref = pref;
			return KErrNone;
			}
		}

	return KErrNotFound;
	}

EXPORT_C TInt CCommDbOverrideSettings::GetConnectionPreferenceOverride(CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref& /*aPref*/)
/** @deprecated v7.0S

Left in place for 6.1 BC purposes.
	
@param aPref Connection preference settings.
@return KErrNotSupported. */
	{
	return KErrNotSupported;
	}

TInt CCommDbOverrideSettings::FindOverride(const TDesC& aTableName, const TDesC& aColumnName, TValueType aType) const
/**
Checks whether the given table, column are overridden for the given type.

@param aTableName A reference to a descriptor containing the name of a table in the communications database. 
@param aColumnName A reference to a descriptor containing the name of a column in table aTableName. 
@param aType The override type.
@return The index in the table if the values are matched else KErrNotFound
*/
	{
	if (IllegalOverride(aTableName,aColumnName))
		return KErrNotSupported;

	TInt index;
	CCommDbOverride toMatch(aType);
	toMatch.iColumnName.Copy(aColumnName);
	toMatch.iTableName.Copy(aTableName);
	index = iOverrides.Find(&toMatch, toMatch.MatchOverrides);

	if (index < 0)
		index = KErrNotFound;

	return index;
	}

TBool CCommDbOverrideSettings::IllegalOverride(const TDesC& aTableName, const TDesC& aColumnName) const
/**
Checks whether the overrides are legal or not.

@param aTableName A reference to a descriptor containing the name of a table in the communications database. 
@param aColumnName A reference to a descriptor containing the name of a column in table aTableName. 
@return ETrue if the override is legal else EFalse
*/
	{
	if (aTableName.CompareF(TPtrC(MODEM_BEARER)) == 0 && aColumnName.CompareF(TPtrC(MODEM_TSY_NAME)) != 0)
		return ETrue;

	// required for BC purposes - check for the old name for the ModemBearer table
	if (aTableName.CompareF(TPtrC(OLD_MODEM_TABLE)) == 0 && aColumnName.CompareF(TPtrC(MODEM_TSY_NAME)) != 0)
		return ETrue;

	if (aTableName.CompareF(TPtrC(PROXIES)) == 0)
		return ETrue;

	if (aTableName.CompareF(TPtrC(WAP_ACCESS_POINT)) == 0)
		return ETrue;

	if (aTableName.CompareF(TPtrC(WAP_IP_BEARER)) == 0)
		return ETrue;

	if (aTableName.CompareF(TPtrC(WAP_SMS_BEARER)) == 0)
		return ETrue;

	if (aTableName.CompareF(TPtrC(DEFAULT_GPRS)) == 0)
		return ETrue;

	// Globals that may not be overriden
	if (aColumnName.Length() == 0)
		{
		return (aTableName.CompareF(TPtrC(WAP_ACCESS_POINT)) == 0 ||
				aTableName.CompareF(TPtrC(CLIENT_TIMEOUT)) == 0 ||
				aTableName.CompareF(TPtrC(ROUTE_TIMEOUT)) == 0 ||
				aTableName.CompareF(TPtrC(REDIAL_ATTEMPTS)) == 0 ||
				aTableName.CompareF(TPtrC(SMS_BEARER)) == 0 ||
				aTableName.CompareF(TPtrC(SMS_RECEIVE_MODE)) == 0 ||
				aTableName.CompareF(TPtrC(GPRS_ATTACH_MODE)) == 0 ||
				aTableName.CompareF(TPtrC(ACCEPT_INCOMING_GPRS)) == 0 ||
				aTableName.CompareF(TPtrC(GPRS_CLASS_C_BEARER)) == 0 ||
				aTableName.CompareF(TPtrC(MODEM_DATA_FAX)) == 0 ||
				aTableName.CompareF(TPtrC(MODEM_PHONE_SERVICES_SMS)) == 0 ||
				aTableName.CompareF(TPtrC(LOCATION_DATA_FAX)) == 0 ||
				aTableName.CompareF(TPtrC(LOCATION_PHONE_SERVICES_SMS)) == 0
				);
		}

	return EFalse;
	}

void CCommDbOverrideSettings::CheckL(const TDesC& aTableName, const TDesC& aColumnName, TValueType aType) const
/**
Checks whether the column aColumnName of type aType in the table aTableName exists.
Panic if the column doesn't exist in the table, otherwise leave with KErrNotFound.

@param aTableName A reference to a descriptor containing the name of a table in the communications database. 
@param aType The override type.
*/
	{
	if (aColumnName.Length()!=0)
		{
		CCommsDbTableView* table=iDb->OpenTableLC(aTableName);
		TDbColType colType;
		TUint32 attrib;
		table->ReadTypeAttribL(aColumnName,colType,attrib);			// checks column exists (cannot leave)
		CleanupStack::PopAndDestroy(table);

// check the correct type
		switch (aType)
			{
		case EIntValue:
			if ((colType!=EDbColUint32) && (colType!=EDbColUint8))
				User::Leave(KErrNotFound);
			break;
		case EBoolValue:
			if (colType!=EDbColBit)
				User::Leave(KErrNotFound);
			break;
		case EDes8Value:
			if (colType!=EDbColText8)
				User::Leave(KErrNotFound);
			break;
		case EDes16Value:
			if (colType!=EDbColText16)
				User::Leave(KErrNotFound);
			break;
		case ELongDesValue:
			if (colType!=EDbColLongText)
				User::Leave(KErrNotFound);
			break;
		default:
			User::Leave(KErrNotFound);
			break;
			}
		}
	else		// global setting
		{
		if (aType==EIntValue)
			{
			TUint32 val=0;		// ignore this
			TRAPD(ret,iDb->GetGlobalSettingL(aTableName,val));
			if (ret==KErrNotSupported)
				User::Leave(KErrNotFound);
			}
		else if (aType==EDes16Value)
			{
			HBufC16* bufHeap = HBufC::NewLC(KCommsDbSvrRealMaxFieldLength) ;
			TPtr buf = bufHeap->Des() ;
			TRAPD(ret,iDb->GetGlobalSettingL(aTableName,buf));
			CleanupStack::PopAndDestroy(bufHeap) ; // bufHeap
			if (ret==KErrNotSupported)
				User::Leave(KErrNotFound);
			}
		else	// all global settings are one of these
			User::Leave(KErrNotFound);
		}
	}
