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
//

#include "rightsdatabase.h"

_LIT(KRightsDatabaseFileName,"c:\\system\\data\\rightsdatabase");

_LIT(KTableRights,"RightsTable");

_LIT(KAllCols, "*");
_LIT(KColFileID, "FileID");
_LIT(KColRightsCount, "RightsCount");
_LIT(KColMimeType, "MimeType");

_LIT(KSQLSelectFromTable,"Select %S From %S");
//_LIT(KSQLSelectFromTableWhereCondition,"Select %S From %S Where %S=%u");

const TInt KColNumFileID = 1;
const TInt KColNumRightsCount = 2;
const TInt KColNumMimeType = 3;

	
CRightsDatabase::RRightsTable::RRightsTable(CRightsDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	}
	
void CRightsDatabase::RRightsTable::OpenL()
	{
	TBuf<256> query;
		
	query.Format(KSQLSelectFromTable,&KAllCols,&KTableRights);
		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	}
	
void CRightsDatabase::RRightsTable::Close()
	{
	iView.Close();
	}
	
	
void CRightsDatabase::RRightsTable::AddRightsL(TUint32 aFileID, TInt32 aRightsCount)
	{
	iView.InsertL();
	iView.SetColL(KColNumFileID, aFileID);
	iView.SetColL(KColNumRightsCount, aRightsCount);
	iView.PutL();
	}

void CRightsDatabase::RRightsTable::AddRightsL(TUint32 aFileID, TInt32 aRightsCount, const TDesC8& aMimeType)
	{
	iView.InsertL();
	iView.SetColL(KColNumFileID, aFileID);
	iView.SetColL(KColNumRightsCount, aRightsCount);
	iView.SetColL(KColNumMimeType, aMimeType);
	iView.PutL();
	}
	
void CRightsDatabase::RRightsTable::GetRightsCountL(TUint32 aFileID, TInt32& rightsCount)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KColNumFileID)==aFileID)
			{
			found = true;
			rightsCount = iView.ColInt32(KColNumRightsCount);
			break;
			}
		}
	if (!found)
		User::Leave(KErrNotFound);
	}

TBool CRightsDatabase::RRightsTable::ExistID(TUint32 aFileID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KColNumFileID)==aFileID)
			{
			found = true;
			break;
			}
		}
	return found;
	}

void CRightsDatabase::RRightsTable::DecrementRightsCountL(TUint32 aFileID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KColNumFileID)==aFileID)
			{
			found = true;
			TInt32 rightsCount = iView.ColInt32(KColNumRightsCount);
			
			if (rightsCount>0)
				{
				iView.UpdateL();
				rightsCount--;
				iView.SetColL(KColNumRightsCount,rightsCount);
				iView.PutL();
				}
			break;
			}
		}
	if (!found)
		User::Leave(KErrNotFound);
	}

void CRightsDatabase::RRightsTable::GetMimeTypeL(TUint32 aFileID, TDes8& aMimeType)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KColNumFileID)==aFileID)
			{
			found = true;
			aMimeType.Copy(iView.ColDes8(KColNumMimeType));
			break;
			}
		}
	if (!found)
		User::Leave(KErrNotFound);
	}
//--------------------------------------------------------------------------------------------------


CRightsDatabase* CRightsDatabase::NewL(TBool aUseExisting)
	{
	CRightsDatabase* self = new (ELeave) CRightsDatabase;
	CleanupStack::PushL(self);
	self->ConstructL(aUseExisting);
	CleanupStack::Pop();
	return self;	
	}
	
void CRightsDatabase::ConstructL(TBool aUseExisting)
	{
 	if (aUseExisting)
		{
		TRAPD(err, OpenDatabaseL(KRightsDatabaseFileName));
		if (err!=KErrNone)
			{
			CreateDatabaseL(KRightsDatabaseFileName);
			CreateTestDatabaseL();
			}
		}
	else
		CreateDatabaseL(KRightsDatabaseFileName);
	}
	
void CRightsDatabase::OpenDatabaseL(const TDesC& aFileName)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	iDbStore = CFileStore::OpenL(fsSession,aFileName,EFileShareAny|EFileRead|EFileWrite);
	// open the database from the root stream
	iDatabase.OpenL(iDbStore,iDbStore->Root());	
	}
	
void CRightsDatabase::CreateDatabaseL(const TDesC& aFileName)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	iDbStore = CPermanentFileStore::ReplaceL(fsSession,aFileName,EFileRead|EFileWrite);
	// Complete file store creation
	iDbStore->SetTypeL(iDbStore->Layout());

				// Create a database in the store
	TStreamId id=iDatabase.CreateL(iDbStore);
	iDbStore->SetRootL(id);

				// Complete database creation by commiting the store
	iDbStore->CommitL();

	// create the CDs table
	DoCreateTablesL();

	// create an index
	DoCreateIndexesL();
	}

CRightsDatabase::CRightsDatabase()
	{
	}
	
CRightsDatabase::~CRightsDatabase()
	{
	iDatabase.Close();
	delete iDbStore;
	}
	
void CRightsDatabase::DoCreateTablesL()
	{
	CDbColSet* columns;
	
	// create Rights Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColFileID,EDbColUint32));
	columns->AddL(TDbCol(KColRightsCount,EDbColInt32));
	columns->AddL(TDbCol(KColMimeType,EDbColText8));
	User::LeaveIfError(iDatabase.CreateTable(KTableRights,*columns));	
	CleanupStack::PopAndDestroy(columns);
	}
	
void CRightsDatabase::DoCreateIndexesL()
	{
	}

void CRightsDatabase::DecrementRightsCountL(TUint32 aFileID)
	{
	RRightsTable table(*this);
	table.OpenL();
	table.DecrementRightsCountL(aFileID);
	table.Close();
	}

void CRightsDatabase::GetRightsCountL(TUint32 aFileID, TInt32& aRightsCount)
	{
	RRightsTable table(*this);
	table.OpenL();
	table.GetRightsCountL(aFileID, aRightsCount);
	table.Close();
	}

void CRightsDatabase::AddRightsL(TUint32 aFileID, TInt32 aRightsCount)
	{
	RRightsTable table(*this);
	table.OpenL();
	table.AddRightsL(aFileID, aRightsCount);
	table.Close();
	}

void CRightsDatabase::AddRightsL(TUint32 aFileID, TInt32 aRightsCount, const TDesC8& aMimeType)
	{
	RRightsTable table(*this);
	table.OpenL();
	table.AddRightsL(aFileID, aRightsCount, aMimeType);
	table.Close();
	}

void CRightsDatabase::GetMimeTypeL(TUint32 aFileID, TDes8& aMimeType)
	{
	RRightsTable table(*this);
	table.OpenL();
	table.GetMimeTypeL(aFileID, aMimeType);
	table.Close();
	}

TBool CRightsDatabase::ExistID(TUint32 aFileID) 
	{
	RRightsTable table(*this);
	table.OpenL();
	TBool found = table.ExistID(aFileID);
	table.Close();
	return found;
	}
	
RDbDatabase& CRightsDatabase::Database() 
	{
	return iDatabase;
	}
	
void CRightsDatabase::CreateTestDatabaseL()
	{
	}
