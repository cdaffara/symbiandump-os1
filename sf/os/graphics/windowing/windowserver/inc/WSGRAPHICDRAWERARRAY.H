// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Utility class for indexing instances of CWsGraphicDrawer
// 
//

#ifndef __WSGRAPHICDRAWERARRAY_H__
#define __WSGRAPHICDRAWERARRAY_H__

#include <e32base.h>
#include <graphics/wsgraphicdrawer.h>

class CWsGraphicDrawerArray: public CBase
/** An index of CWsGraphicDrawer instances
A CWsGraphicDrawer can be in many such indexes, as an index does not imply ownership
@publishedPartner
@released
*/	{
public:
	//Placeholder for transacion record
	struct XRollBackBase;
	
	//transactional manipulation methods that are deprecated because they leak 
	IMPORT_C void AddLC(CWsGraphicDrawer* aDrawer);
	IMPORT_C TInt SwapLC(CWsGraphicDrawer* aDrawer);
	
	//non-transactional manipulation
	IMPORT_C TInt Remove(const TGraphicDrawerId& aId);
	IMPORT_C TInt Add(CWsGraphicDrawer* aDrawer);
	IMPORT_C TInt Swap(CWsGraphicDrawer* aDrawer);
	
	//safe transactional methods
	//The array should not be Compressed() during the transaction period to ensure that the Remove RollBack operation will always succeed.
	IMPORT_C XRollBackBase* AddTLC(CWsGraphicDrawer* aDrawer);
	IMPORT_C XRollBackBase* SwapTLC(CWsGraphicDrawer* aDrawer);
	IMPORT_C XRollBackBase* RemoveTLC(const TGraphicDrawerId& aId);
	IMPORT_C void CommitP(XRollBackBase*);

	//Query
	IMPORT_C const CWsGraphicDrawer* ResolveGraphic(const TGraphicDrawerId& aId) const;
	IMPORT_C TBool IsEmpty() const;

	//Other non-transactional methods
	IMPORT_C TInt RemoveAndDestroy(const TGraphicDrawerId& aId);
	IMPORT_C TInt RemoveAll(const MWsClient& aOwner);
	IMPORT_C TInt RemoveAndDestroyAll(const MWsClient& aOwner);
	IMPORT_C void Close();
	IMPORT_C void ResetAndDestroy();
private:
	NONSHARABLE_STRUCT(TGraphic)
		{
		TGraphicDrawerId iId;
		CWsGraphicDrawer* iDrawer;
		};
	RArray<TGraphic> iArray;
	CWsGraphicDrawer* SwapIn(CWsGraphicDrawer* aDrawer);
	TInt IndexOf(const TGraphicDrawerId& aId) const;
	static TInt GraphicDrawerCompare(const TGraphic& aFirst,const TGraphic& aSecond);
private:
	//Transactional unwinding support classes
	class XAddRollBack;
	friend class XAddRollBack;
	class XSwapRollBack;
	friend class XSwapRollBack;
	class XRemoveRollBack;
	friend class XRemoveRollBack;
	class testArrayValidator;
	friend class testArrayValidator;
	};

#endif //#ifndef __WSGRAPHICDRAWERARRAY_H__
