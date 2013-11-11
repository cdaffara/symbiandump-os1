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

#ifndef __RIGHTSDATABASE_H__
#define __RIGHTSDATABASE_H__

#include <e32base.h>

class CSecurityDecryptBase;
class CSecurityEncryptBase;

#include <d32dbms.h>
#include <s32file.h>

const TInt KRDNoRightsCount = -1;

class CRightsDatabase : public CBase
	{
public:
	class RRightsTable 
		{
	public:
		RRightsTable(CRightsDatabase& aDatabase);		
		
		void OpenL();
		
		void AddRightsL(TUint32 aFileID, TInt32 aRightsCount);
		void AddRightsL(TUint32 aFileID, TInt32 aRightsCount, const TDesC8& aMimeType);
		void GetRightsCountL(TUint32 aFileID, TInt32& aRightsCount);
		void GetMimeTypeL(TUint32 aFileID, TDes8& aMimeType);
		void DecrementRightsCountL(TUint32 aFileID);
		TBool ExistID(TUint32 aFileID);
		void Close();
		
	private:
		CRightsDatabase& iDatabase;
		RDbView iView;
		};
		
public:		
	static CRightsDatabase* NewL(TBool aUseExisting=ETrue);
	

	
	RDbDatabase& Database();

	void GetRightsCountL(TUint32 aFileID, TInt32& aRightsCount);
	void DecrementRightsCountL(TUint32 aFileID);
	void AddRightsL(TUint32 aFileID, TInt32 aRightsCount);
	void AddRightsL(TUint32 aFileID, TInt32 aRightsCount, const TDesC8& aMimeType);
	void GetMimeTypeL(TUint32 aFileID, TDes8& aMimeType);

	TBool ExistID(TUint32 aFileID);

	
	~CRightsDatabase(); 
	
	void CreateTestDatabaseL();
	
private:
	void OpenDatabaseL(const TDesC& aFileName);
	void CreateDatabaseL(const TDesC& aFileName);
	
	void DoCreateTablesL();
	void DoCreateIndexesL();
	
	CRightsDatabase();
	
	void ConstructL(TBool aUseExisting);
	

	TUid iClientUid;
	CFileStore* iDbStore;
	RDbStoreDatabase iDatabase;
	
	TUint32 iLastID;
		
	};
	
#endif //__RIGHTSDATABASE_H__
