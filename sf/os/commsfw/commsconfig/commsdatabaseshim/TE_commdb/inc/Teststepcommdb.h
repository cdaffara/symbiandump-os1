/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
// This contains CTestCaseCommDb which is the base class for all 
// the CommDb suite test cases

#if (!defined __COMMDBTESTSTEP_H__)
#define __COMMDBTESTSTEP_H__

#include "TE_commdbServer.h"

_LIT(KCommDbName, "C:\\private\\100012a5\\DBS_10004e1d_CDBV3.DAT");


class CTestSuite;
class CTestSuiteCommDb;
class CCommsDatabase;
class CCommsDbTableView;
class CCommsDbProtectTableView;
class CCommsDbProtectConnectPrefTableView;

NONSHARABLE_CLASS(CTestStepCommDb) : public CTestStep
{
public:
	CTestStepCommDb();
	~CTestStepCommDb();

	// pointer to suite which owns this test 
	CTestSuiteCommDb * iCommDbSuite;

	//Functions to set and clear the database objects that will
	//be used by test steps derived from this class.
	//This prevents the test steps from creating their own database objects
	void setRemoteDatabaseObjects( CCommsDatabase* aDb, CCommsDbTableView* aDbView );
	void clearRemoteDatabaseObjects();

	TInt executeStep( CTestStepCommDb& aTestStep );
	TInt executeStep( );
	virtual TInt executeStepL();
	TVerdict doTestStepPostambleL();
	TInt doTestStepWithHeapFailureL( CTestStepCommDb& aTestStep, TInt aReturnValue );
	TInt doTestStepWithHeapFailureDb( CTestStepCommDb& aTestStep, TInt aReturnValue );

	enum TDatabaseNumber
		{
		EDatabase_1,		//Records in ISP tables only
		EDatabase_7			//No records
		};
	TInt copyTestDatabase( TDatabaseNumber aDbNumber );
	TInt copyDatabaseToDefault( TDatabaseNumber aDbNumber );
	void copyFileL (const TDesC& anOld,const TDesC& aNew);
	void deleteFileL (const TDesC& aFileName);

	void DeleteCurrentDatabaseL();
	void CopyDatabaseL(const TDesC& aSource);


	//Pointers to the current database objects
	CCommsDatabase*		iTheDb;
	CCommsDbTableView*	iTheView;


protected:
	//Set by derived steps when local database objects have been allocated
	TBool	iLocalObjects;

	//Opens a database object and assigns it to iTheDb
	TBool openDbL();
	//Opens a database and a table view, assigns them to iTheDb and iTheView
	TBool openDbAndViewL( const TDesC16& aTable );
	//Releases any objects assigned to iTheDb and iTheView
	void popAndDestroyDatabaseObjects();

	TInt ChangeCPRankingL(CCommsDbProtectConnectPrefTableView* aTable, TUint32& Ranking);
	//Functions needed for Protection tests
	void    CheckTableIsProtectedL(CCommsDbProtectTableView* aTable);
	void    CheckTableIsNotProtectedL(CCommsDbProtectTableView* aTable);
	void    GoToPositionL(CCommsDbProtectTableView* aTable,TInt aPos);

	void    UpdateRecordL   (CCommsDbProtectTableView* aTable,TInt aResult);
	void    DeleteRecordL   (CCommsDbProtectTableView* aTable,TInt aResult);
	void    InsertRecordL   (CCommsDbProtectTableView* aTable,TInt aResult);
	void    ModifyRecordL   (CCommsDbProtectTableView* aTable);
	void    PopulateInsertedRecordL(CCommsDbProtectTableView* aTable);

	void    CheckTableIsProtectedL(CCommsDbProtectConnectPrefTableView* aTable);
	void    CheckTableIsNotProtectedL(CCommsDbProtectConnectPrefTableView* aTable);
	void    GoToPositionL(CCommsDbProtectConnectPrefTableView* aTable,TInt aPos);

	void    UpdateRecordL   (CCommsDbProtectConnectPrefTableView* aTable,TInt aResult);
	void    DeleteRecordL   (CCommsDbProtectConnectPrefTableView* aTable,TInt aResult);
	void    InsertRecordL   (CCommsDbProtectConnectPrefTableView* aTable,TInt aResult);
	void    ModifyRecordL   (CCommsDbProtectConnectPrefTableView* aTable);
	void    PopulateInsertedRecordL(CCommsDbProtectConnectPrefTableView* aTable);

	void    CheckRecordAccessL(CCommsDbProtectConnectPrefTableView* aTable, TInt aExpectedAccess);

	void    ProtectRecordL  (CCommsDbProtectTableView* aTable,TInt aResult);
	void    UnprotectRecordL(CCommsDbProtectTableView* aTable,TInt aResult);

    void    ProtectRecordL(CCommsDbProtectConnectPrefTableView* aTable, TInt aResult);
	void    UnprotectRecordL(CCommsDbProtectConnectPrefTableView* aTable,TInt aResult);


private:
	//Set when remote database object are in use
	TBool	iRemoteObjects;

};

#define CDBLEAVE( exp1, exp2 )			{	\
											TInt _r;	\
											_r = (exp1);  \
											if ( _r!=exp2 ) User::Leave(_r);\
										}

_LIT(KInvalidColumnName, "InvalidColumn");

#endif /* __COMMDBTESTSTEP_H__ */
