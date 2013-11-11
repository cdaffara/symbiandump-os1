// bsymtree.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef FSHELL_BSYMTREE_H
#define FSHELL_BSYMTREE_H

#include <e32std.h>
class CDesC16Array;

namespace LtkUtils
	{
	class RLtkBuf16;

	class RNode
		{
	public:
		static RNode* NewL();
		void InsertStringL(const TUint16* aString, TInt aValue=0);
		~RNode();
		void CompleteL(TDes& aPrefix, CDesC16Array& aResults);
		TInt ValueForStringL(const TDesC& aString) const;

	private:
		RNode* ChildForLetter(char aChild) const;
		RNode* AddChildL(char aChild);
		RNode(char aLetter);
		RNode* Sprog() const;
		RArray<RNode*>& Sprogs() const;
		RNode* WalkToEndOfString(TUint16*& aString);
		RNode* TabFill(TUint16*& aString, const TUint16* aEnd);
		void DoCompleteOptionsL(CDesC16Array& aResults, RLtkBuf16& aCurrent);
		//void Dump() const; // Debug

	private:
		char iLetter;
		char iHasChildArray;
		short iPad;
		TAny* iPtr; // Either an RNode* or an RArray<RNode*>*. Or a TInt for the value if (iLetter == 0 && !iHasChildArray)
		};

	}

#endif
