// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <c32comm.h>
#include <c32root.h>
#include <es_sock.h>
#include <comms-infras/es_commsdataobject.h>
#include <connpref.h>
#include <CommDbConnPref.h>
#include <metadatabase.h>
#include <commsdattypeinfov1_1.h>
#include <comms-infras/connectionqueryset.h>
#include <comms-infras/ss_commsdataobject.h>
#include <ecom/ecom.h>

#include "queryconnsettingstiermgrimpl.h"
#include "noddyqueryconnsettingsimpl.h"

using namespace ESock;

	
void Test0cL()
	{
	// ---------------------------------------------------------------------
	// Creating the query
	// ---------------------------------------------------------------------
	XConnectionQuerySet* querySet = XConnectionQuerySet::NewL();
	CleanupDeletePushL(querySet);

	XConnectionQueryBase* query;
	query = XText8Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdNifName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdTsyName, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIpAddrFromServer, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdLastSessionClosedTimeout, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIfPromptForAuth, *querySet);

	CleanupStack::PopAndDestroy(querySet);
	}
	
	
void Test1L()
	{
	// ---------------------------------------------------------------------
	// Creating the query
	// ---------------------------------------------------------------------
	
	XConnectionQuerySet* querySet = XConnectionQuerySet::NewL();
	CleanupDeletePushL(querySet);

	XConnectionQueryBase* query;
	query = XText8Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdNifName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdTsyName, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIpAddrFromServer, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdLastSessionClosedTimeout, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIfPromptForAuth, *querySet);


	// ---------------------------------------------------------------------
	// Serialisation/Deserialisation Test
	// ---------------------------------------------------------------------
	CConnectionQuerySet* cdo = CConnectionQuerySet::NewL(querySet);
	CleanupStack::Pop(querySet);
	CleanupStack::PushL(cdo);
	
	TInt length = cdo->Length();
	HBufC8* buffer = HBufC8::NewLC(length);
	if (!buffer) 
		{
		User::Leave(KErrNoMemory);
		}


	TPtr8 storePtr(buffer->Des());
	TInt ret = cdo->Store(storePtr);
	User::LeaveIfError(ret);
	

	// Load using Load()
	TPtrC8 loadPtr = buffer->Des();
	CConnectionQuerySet* outputContainer = CConnectionQuerySet::LoadL(loadPtr);
	
	delete outputContainer;

	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PopAndDestroy(cdo);
	}



void Test3L()
	{
	// ---------------------------------------------------------------------
	// Creating the query
	// ---------------------------------------------------------------------
	
	CNoddyQueryConnSettingsImpl* impl = new (ELeave)CNoddyQueryConnSettingsImpl();
	CleanupStack::PushL(impl);
	
	XConnectionQuerySet* querySet = XConnectionQuerySet::NewL();
	CleanupDeletePushL(querySet);

	XConnectionQueryBase* query;
	query = XText8Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdNifName, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIpAddrFromServer, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdLastSessionClosedTimeout, *querySet);
	query = XIntQuery::NewL(CommsDat::KCDTIdIfPromptForAuth, *querySet);


	querySet->DispatchL(static_cast<MQueryConnSettingsApiExt*>(impl), NULL);

	
	// ---------------------------------------------------------------------
	// Testing the iterators
	// ---------------------------------------------------------------------
	TConnectionQueryIter iter1(querySet->GetQueryIter(XConnectionQueryBase::ENotAnswered));
	TInt count = 0;
	query = NULL;
	query = iter1++;
	while (query) 
		{
		count++;
		query = iter1++;;
		}
	ASSERT(count == 1);


	TConnectionQueryIter iter2(querySet->GetQueryIter(XConnectionQueryBase::EAnswered));
	count = 0;
	query = NULL;
	query = iter2++;
	while (query) 
		{
		count++;
		query = iter2++;
		}
	ASSERT(count == 4);

	
	CleanupStack::PopAndDestroy(querySet);
	CleanupStack::PopAndDestroy(impl);
	}

	
	

void Test4L()
	{
	// ---------------------------------------------------------------------
	// Creating the query
	// ---------------------------------------------------------------------
	
	CQueryConnSettingsTierMgrImpl* impl = CQueryConnSettingsTierMgrImpl::NewL(1);
	CleanupStack::PushL(impl);
	
	XConnectionQuerySet* querySet = XConnectionQuerySet::NewL();
	CleanupDeletePushL(querySet);

	XConnectionQueryBase* query;
	query = XUintQuery::NewL(CommsDat::KCDTIdIAPRecord | CommsDat::KCDTIdRecordTag, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdIAPRecord | CommsDat::KCDTIdRecordName, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdIAPLocation, *querySet);

	query = XText8Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdNifName, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIpAddrFromServer, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdLastSessionClosedTimeout, *querySet);
	query = XIntQuery::NewL(CommsDat::KCDTIdIfPromptForAuth, *querySet);


	querySet->DispatchL(static_cast<MQueryConnSettingsApiExt*>(impl), NULL);

	TBuf8<200> des8;
	TBuf16<200> des16;
	TBool b;
	TInt i(0x0F0F0F0F);
	TUint32 ui(0xF0F0F0F0);

	TInt count = 0;
	// Option 1 - Iterate through the query results
	TConnectionQueryIter iter (querySet->GetQueryIter(XConnectionQueryBase::EAnswered));
	const XConnectionQueryBase* qBase = iter++;
	while (qBase) 
		{
		count++;
		// Do something with query/data
		switch (qBase->Type()) 
			{
			case XConnectionQueryBase::EInt:
				{
				const XIntQuery* qInt = static_cast<const XIntQuery*>(qBase);
				i = qInt->Data();
				}
				break;

			case XConnectionQueryBase::EUint:
				{
				const XUintQuery* qUInt = static_cast<const XUintQuery*>(qBase);
				ui = qUInt->Data();
				}
				break;

			case XConnectionQueryBase::EBool:
				{
				const XBoolQuery* qBool = static_cast<const XBoolQuery*>(qBase);
				b = qBool->Data();
				}
				break;

			case XConnectionQueryBase::EText8:
				{
				const XText8Query* qText8 = static_cast<const XText8Query*>(qBase);
				des8 = qText8->Data();
				}
				break;

			case XConnectionQueryBase::EText16:
				{
				const XText16Query* qText16 = static_cast<const XText16Query*>(qBase);
				des16 = qText16->Data();
				}
				break;
			}
			
		qBase = iter++;
		}
	
	CleanupStack::PopAndDestroy(querySet);
	CleanupStack::PopAndDestroy(impl);
	}


/*
void Test4L()
	{
	// ---------------------------------------------------------------------
	// Creating the query
	// ---------------------------------------------------------------------
	
	XConnectionQuerySet* querySet = XConnectionQuerySet::NewL();
	CleanupDeletePushL(querySet);

	XConnectionQueryBase* query;
	query = XText8Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdNifName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdTsyName, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIpAddrFromServer, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdLastSessionClosedTimeout, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIfPromptForAuth, *querySet);


	// ---------------------------------------------------------------------
	// Testing Find Query/Setting Data/Answered
	// ---------------------------------------------------------------------
	XText8Query* text8Query;
	text8Query = static_cast<XText8Query*>(querySet->FindQuery(CommsDat::KCDTIdPortName));
//	text8Query->SetData(_L8("oddlength"));
//	text8Query->SetQueryAnswered();

	XText16Query* textQuery;
	textQuery = static_cast<XText16Query*>(querySet->FindQuery(CommsDat::KCDTIdNifName));
//	textQuery->SetData(_L("Testing.nif"));
//	textQuery->SetQueryAnswered();

	// ---------------------------------------------------------------------
	// Serialisation/Deserialisation Test 2 (with some populated data)
	// ---------------------------------------------------------------------
	TInt length = queryContainer->Length();
	HBufC8* buffer = HBufC8::NewLC(length);
	if (!buffer) 
		{
		User::Leave(KErrNoMemory);
		}

	TInt ret;
	TPtr8 storePtr(buffer->Des());
	ret = queryContainer->Store(storePtr);
	User::LeaveIfError(ret);
	
	CParameterSetContainer* outputContainer = CParameterSetContainer::NewL();
	TPtrC8 loadPtr(buffer->Des());
	outputContainer->Load(loadPtr);
	delete outputContainer;
	
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PopAndDestroy(queryContainer);
	}
*/	
	
void Test5L()
	{
	// ---------------------------------------------------------------------
	// Creating the query
	// ---------------------------------------------------------------------
	
	XConnectionQuerySet* querySet = XConnectionQuerySet::NewL();
	CleanupDeletePushL(querySet);

	XConnectionQueryBase* query;
	query = XText8Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdNifName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdTsyName, *querySet);
	query = XText16Query::NewL(CommsDat::KCDTIdPortName, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIpAddrFromServer, *querySet);
	query = XUintQuery::NewL(CommsDat::KCDTIdLastSessionClosedTimeout, *querySet);
	query = XBoolQuery::NewL(CommsDat::KCDTIdIfPromptForAuth, *querySet);

	// ---------------------------------------------------------------------
	// RConnection API additional method test
	// ---------------------------------------------------------------------
    TInt err;
    RSocketServ ss;
	err = ss.Connect();
	
	RConnection con;
	err = con.Open(ss);

	CConnectionQuerySet* cdo = CConnectionQuerySet::NewL(querySet);
	CleanupStack::Pop(querySet);
	CleanupStack::PushL(cdo);

	// This should fail with KErrNotReady since the connection hasn't been started
	err = con.GetParameters(*cdo);
	ASSERT(err == KErrNotReady);

	// This should succeed
	err = con.Start();
	err = con.GetParameters(*cdo);
	
	TBuf8<200> des8;
	TBuf16<200> des16;
	TBool b;
	TInt i(0x0F0F0F0F);
	TUint32 ui(0xF0F0F0F0);
	
	
	TInt count = 0;
	if (err == KErrNone)
		{
		XConnectionQuerySet& outputQuerySet = cdo->DataObject();
	
		// Option 1 - Iterate through the query results
		TConnectionQueryIter iter (outputQuerySet.GetQueryIter(XConnectionQueryBase::EAnswered));
		const XConnectionQueryBase* qBase = iter++;
		while (qBase) 
			{
			count++;
			// Do something with query/data
			switch (qBase->Type()) 
				{
				case XConnectionQueryBase::EInt:
					{
					const XIntQuery* qInt = static_cast<const XIntQuery*>(qBase);
					i = qInt->Data();
					}
					break;

				case XConnectionQueryBase::EUint:
					{
					const XUintQuery* qUInt = static_cast<const XUintQuery*>(qBase);
					ui = qUInt->Data();
					}
					break;

				case XConnectionQueryBase::EBool:
					{
					const XBoolQuery* qBool = static_cast<const XBoolQuery*>(qBase);
					b = qBool->Data();
					}
					break;

				case XConnectionQueryBase::EText8:
					{
					const XText8Query* qText8 = static_cast<const XText8Query*>(qBase);
					des8 = qText8->Data();
					}
					break;

				case XConnectionQueryBase::EText16:
					{
					const XText16Query* qText16 = static_cast<const XText16Query*>(qBase);
					des16 = qText16->Data();
					}
					break;
				}
				
			qBase = iter++;
			}
	
		// Option 2 - Get a query response directly
		const XConnectionQueryBase* directQuery;
		directQuery = outputQuerySet.FindQuery(CommsDat::KCDTIdIfNetworks); // wont be found
		directQuery = outputQuerySet.FindQuery(CommsDat::KCDTIdTsyName);
		}
	
	con.Stop();
    con.Close();
    ss.Close();

	CleanupStack::PopAndDestroy(cdo);
	querySet = NULL;
	}
	
// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
    _LIT(KLddName, "ECOMM");
    _LIT(KPddName, "ECDRV");

    // Load device drivers
    User::LoadPhysicalDevice(KPddName);
    User::LoadLogicalDevice(KLddName);
    
	// self contained query set / container tests

	TInt err = KErrNone;
//	TRAP(err, Test0cL());
	
//	TRAP(err, Test1L());
	TRAP(err, Test3L());
	TRAP(err, Test4L());
	
	// using rconnection - full end-to-end
	TRAP(err, Test5L());
	
	REComSession::FinalClose();
   	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

