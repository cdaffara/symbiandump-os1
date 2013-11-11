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

#ifndef __TESTAGENTDATA_H__
#define __TESTAGENTDATA_H__

#include <caf/caf.h>
#include <caf/agentinterface.h>
#include <caf/bitset.h>
#include "rightsdatabase.h"

using namespace ContentAccess;
	class CTestAgentData : public CAgentData
		{
	public:
		static CTestAgentData* NewL(const TDesC& aURI, TContentShareMode aMode);
		static CTestAgentData* NewLC(const TDesC& aURI, TContentShareMode aMode);
		~CTestAgentData();

		// From CAgentData
		virtual void GetAttributeL(const CBitset &aQuerySet,
								   CBitset &aResponseSet);
		virtual void DataSizeL(TInt &aSize);
		virtual TInt EvaluateIntent(TIntent aIntent);
		virtual TInt ExecuteIntent(TIntent aIntent);
		virtual TInt Read(TDes8& aDes);
		virtual TInt Read(TDes8& aDes, TInt aLength);
		virtual void Read(TDes8& aDes, TRequestStatus& aStatus);
		virtual void Read(TDes8& aDes,
						  TInt aLength,
						  TRequestStatus& aStatus);
		virtual TInt Seek(TSeek aMode, TInt& aPos);
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue);
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet);						
	private:
		CTestAgentData();
		void ConstructL(const TDesC& aURI, TContentShareMode aMode);
		
	private:
		/* Handle to the filesystem */
		RFs iFs;
		
		/* RFile object */
		RFile iFile;

		/* Test Scheme specific */
		TInt iDataSize;
		TInt iDataOffset;
		TUint32 iDatabaseID;
		CRightsDatabase* iRightsDatabase;
		};

#endif // __TESTAGENTDATA_H__
