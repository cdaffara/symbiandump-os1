// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "database.h"
#include "dbdef.h"
#include "filedump.h"

/**
makes logging accessible to all files

*/
extern FileDump* oMsg;

//
//
// PRIVATE PREFERENCE METHODS
//
//


TInt DBAccess::OpenPreferenceTable(TDesC &table)
/**
Opens the preference table

  @param table A reference to a descriptor containing the name of a table
  @return ETrue if Successful else EFalse 
  */
	{
	TInt valid = EFalse;
	
	if (table.Compare(TPtrC(CONNECTION_PREFERENCES)) == 0)
		{
		IAPpreference.iDirection = ECommDbConnectionDirectionOutgoing;
		
		oMsg->Dbg(_L("  Opening preference table for outgoing"));
		
		TRAPD(ret, _OpenPrefTableL());
		if (Evaluate(ret))
			{
			if (dbPrefView)
				{
				access |= DBPrefOut;
				valid = ETrue;
				}
			}
		}
	
	if (valid)
		if (access & DBUpdate)
			if (!Evaluate(dbPrefView->GotoFirstRecord()))
				valid = EFalse;
			
			return valid;
	}


TInt DBAccess::OpenPreferenceTableOnRank(TDesC &table)
/**
Open the preference table according to supplied rank and direction

  @param table A reference to a descriptor containing the name of a table
  @return ETrue if Successful else EFalse 
  */
	{
	TInt valid = EFalse;
	
	if (table.Compare(TPtrC(CONNECTION_PREFERENCES)) == 0)
		{
		IAPpreference.iDirection = ECommDbConnectionDirectionOutgoing;
		
		oMsg->Dbg(_L("  Opening preference table using rank [%d] direction [%d]"), IAPpreference.iRanking, IAPpreference.iDirection);
		
		TRAPD(ret, _OpenPrefTableOnRankL());
		if (Evaluate(ret))
			{
			if (dbPrefView)
				{
				access |= DBPrefOut;
				valid = ETrue;
				}
			}
		}
	
	if (valid)
		if (access & DBUpdate)
			if (!Evaluate(dbPrefView->GotoFirstRecord()))
				valid = EFalse;
			
			return valid;
	}


void DBAccess::_OpenPrefTableL()
/**
Opens specific tables.  This is done in it's own functions because
they put stuff on the stack that has to be popped off before a
TRAP can be called on it.

  */
	{
	dbPrefView = db->OpenConnectionPrefTableLC(IAPpreference.iDirection);
	
	CleanupStack::Pop(); // view
	}

void DBAccess::_OpenPrefTableOnRankL()
	{
	dbPrefView = db->OpenConnectionPrefTableViewOnRankLC(IAPpreference.iDirection, IAPpreference.iRanking);
	
	CleanupStack::Pop(); // view
	}


void DBAccess::ResetPreferenceFields()
/**
Set Preference Table structure to known values

  */
	{
	IAPpreference.iRanking = BADNUMVAL;
	IAPpreference.iDirection = ECommDbConnectionDirectionUnknown;
	IAPpreference.iDialogPref = ECommDbDialogPrefUnknown;
	IAPpreference.iBearer.iBearerSet = BADNUMVAL;
	IAPpreference.iBearer.iIapId = BADNUMVAL;
#ifdef __COMMDB_ROHID_SUPPORT__	
	isReadOnly = EFalse;
#endif
	}


TInt DBAccess::CheckPreferenceFieldsL()
/**
Makes sure we have all preference fields set

  @return ETrue if successful or EFalse
  */
	{
	oMsg->Dbg(_L("  Checking preference fields..."));
	
	if (IAPpreference.iRanking == BADNUMVAL || 
		IAPpreference.iDirection == ECommDbConnectionDirectionUnknown ||
		IAPpreference.iDialogPref == ECommDbDialogPrefUnknown ||
		IAPpreference.iBearer.iBearerSet == BADNUMVAL )
		{
		iLastError = E_NOTENOUGHDATA;
		LastErrorMessage.Set(errorCode[E_NOTENOUGHDATA]);
		return EFalse;
		}
	// now check if the optional field was supplied - if not, zero it
	if (IAPpreference.iBearer.iIapId == BADNUMVAL)
		IAPpreference.iBearer.iIapId = 0;
	
	oMsg->Msg(_L("All preference fields valid"));
	return ETrue;
	}


TInt DBAccess::SetPrefValue(TInt index)
/**
Attempts to open the preference table, or set a particular field

  @param index Index in the Table
  @return ETrue if successful or EFalse
  */
	{
	TInt valid = EFalse;
	TLex temp = TargetCols[ColCount].Value;
	TInt tempInt = 0;
	
	// once we have the ranking value, we can open the database
	if (TargetCols[ColCount].Name.Compare(TPtrC(CONNECT_PREF_RANKING)) == 0)
		{
		if (Evaluate(temp.Val(tempInt)))
			{
			TPtrC table = tableArray[index];
			
			ResetPreferenceFields();
			IAPpreference.iRanking = tempInt;
			
			if (access & DBInsert)
				{
				if (OpenPreferenceTable(table))
					valid = ETrue;
				}
			else if (access & DBUpdate)
				{
				// open record we want to edit
				if (OpenPreferenceTableOnRank(table))
					valid = ETrue;
				}
			}
		}
	else
		{
		// set IAP value
		if(Evaluate(temp.Val(tempInt)))
			{
			if (TargetCols[ColCount].Name.Compare(TPtrC(CONNECT_PREF_DIALOG_PREF)) == 0)
				{
				IAPpreference.iDialogPref = (TCommDbDialogPref)tempInt;
				oMsg->Dbg(_L("  Dialog preference set to %d"), IAPpreference.iDialogPref);
				valid = ETrue;
				}
			else if (TargetCols[ColCount].Name.Compare(TPtrC(CONNECT_PREF_IAP)) == 0)
				{
				IAPpreference.iBearer.iIapId = tempInt;
				oMsg->Dbg(_L("  Bearer ID set to %d"), IAPpreference.iBearer.iIapId);
				valid = ETrue;
				}
			else if (TargetCols[ColCount].Name.Compare(TPtrC(CONNECT_PREF_DIRECTION)) == 0)
				{
				IAPpreference.iDirection = (TCommDbConnectionDirection)tempInt;
				oMsg->Dbg(_L("  Bearer Direction set to %d"), IAPpreference.iDirection);
				valid = ETrue;
				}
			else if (TargetCols[ColCount].Name.Compare(TPtrC(CONNECT_PREF_BEARER_SET)) == 0)
				{
				IAPpreference.iBearer.iBearerSet = tempInt;
				oMsg->Dbg(_L("  Bearer Set set to %d"), IAPpreference.iBearer.iBearerSet);
				valid = ETrue;
				}
#ifdef __COMMDB_ROHID_SUPPORT__
			else if (TargetCols[ColCount].Name.Compare(TPtrC(COMMDB_READ_ONLY)) == 0)
				{
				oMsg->Dbg(_L("  ReadOnly set to %d"), tempInt);
				valid = ETrue;
				}
#endif
			else
				{
				iLastError = E_INCORRECTCOLNAME;
				LastErrorMessage.Set(errorCode[E_INCORRECTCOLNAME]);
				}
			}
		else
			{
			iLastError = E_INCORRECTTYPE;
			LastErrorMessage.Set(errorCode[E_INCORRECTTYPE]);
			}
		}
	
	return valid;
	}

TInt DBAccess::ConnectionPreferencesTable(TDesC &table)
	{
	
	if (table.Compare(TPtrC(CONNECTION_PREFERENCES)) == 0)
		{
		return ETrue;
		}
	
	return EFalse;
	}
