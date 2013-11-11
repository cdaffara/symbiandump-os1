// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__CXMLFILE_H__)
#define __CXMLFILE_H__

#include <e32base.h>
#include "dbdef.h"
#include "CXMLContentHandler.h"


class CXMLDatabase;

NONSHARABLE_CLASS(CXMLData) : public CBase 
/**
@internalComponent
*/
	{
	public:
		// Constructor, pass in name of XML file to open, relative to RAM root
		static CXMLData* NewL(RFs& aFs, const TDesC& aName, CXMLDatabase* aXmlDb, 
		                      TBool aForceXMLProcessing,
		                      TBool aAppendMode);
		virtual ~CXMLData();
		TInt GetSetting(const TInt aEntryIndex, const TDesC & aParamName, TPtrC & aParamValue);
		TInt GetStartingIndex(const TDesC& aTableName);
		TInt GetLastIndex(const TDesC& aTableName);
		TInt GetEntryNumberParameters(const TInt aEntryIndex);
		const TBuf<MAX_BUFFER_LEN>& GetOperation(const TInt aEntryIndex);
		TInt GetElementRecordID(const TInt aEntryIndex) const;
		
	protected:
		CXMLData(CXMLDatabase*, TBool, TBool);
		
	private:
		TBool ConstructL(RFs& aFs, const TDesC& aName, CXMLDatabase* aXmlDb);
		
	private:
		CXMLDatabase* xmlDb;
		TBool bForceXMLProcessing;
		TBool iAppendMode;
	};

#endif
