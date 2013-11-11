// bsymtree.cpp
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
#include "bsymtree.h"
#include <badesca.h>
#include <fshell/descriptorutils.h>
//#include <e32debug.h>

using namespace LtkUtils;

RNode* RNode::NewL()
	{
	RNode* top = new(ELeave) RNode(0);
	CleanupDeletePushL(top);
	top->iPtr = new(ELeave) RArray<RNode*>(8);
	top->iHasChildArray = 1;
	CleanupStack::Pop(top);
	return top;
	}

RNode::RNode(char aLetter)
	: iLetter(aLetter), iHasChildArray(0), iPad(0), iPtr(NULL)
	{
	}

RNode* RNode::Sprog() const
	{
	ASSERT(!iHasChildArray);
	ASSERT(iLetter != 0);
	return reinterpret_cast<RNode*>(iPtr);
	}

RArray<RNode*>& RNode::Sprogs() const
	{
	ASSERT(iHasChildArray);
	return *reinterpret_cast<RArray<RNode*>*>(iPtr);
	}

RNode* RNode::ChildForLetter(char aChild) const
	{
	if (iHasChildArray)
		{
		const TInt count = Sprogs().Count();
		for (TInt i = 0; i < count; i++)
			{
			RNode* node = Sprogs()[i];
			if (node->iLetter == aChild) return node;
			}
		}
	else if (iLetter != 0) // null terminators use next for the value
		{
		RNode* child = Sprog();
		if (child && child->iLetter == aChild) return child;
		}
	return NULL;
	}

RNode* RNode::AddChildL(char aChild)
	{
	RNode* node = new(ELeave) RNode(aChild);
	CleanupStack::PushL(node);
	if (!iHasChildArray)
		{
		if (iPtr == NULL)
			{
			// Easy
			CleanupStack::Pop(node);
			iPtr = node;
			return node;
			}
		else
			{
			// Need to upgrade to a childArray
			RArray<RNode*>* children = new(ELeave) RArray<RNode*>(8);
			CleanupStack::PushL(children);
			children->AppendL(reinterpret_cast<RNode*>(iPtr));
			iPtr = children;
			CleanupStack::Pop(children);
			iHasChildArray = 1;
			}
		}
	Sprogs().AppendL(node);
	CleanupStack::Pop(node);
	return node;
	}

void RNode::InsertStringL(const TUint16* aString, TInt aValue)
	{
	char letter = (char)*aString;
	RNode* child = ChildForLetter(letter);
	if (!child)
		{
		child = AddChildL(letter);
		}
		
	if (letter == 0)
		{
		// If the letter was the null termination at the end, we're finished and just need to set the value
		//ASSERT(child->iPtr == NULL); // There should never be a child of a null terminator (unless the same string has been inserted twice...)
		// ^ Annoyingly there can be duplicate symbols that are only distinguished by information we've actually thrown away in the BSYM.
		//   We will ignore such duplicate symbols. If this is a problem, someone feel free to fix it.
		if (!child->iPtr)
			{
			child->iPtr = (TAny*)aValue;
			}
		}
	else
		{
		aString++;
		child->InsertStringL(aString, aValue);
		}
	}

RNode* RNode::WalkToEndOfString(TUint16*& aString)
	{
	RNode* currentNode = this;
	while (*aString)
		{
		RNode* child = currentNode->ChildForLetter((char)*aString);
		if (!child) return currentNode;
		currentNode = child;
		aString++;
		}
	return currentNode;
	}
	
RNode* RNode::TabFill(TUint16*& aString, const TUint16* aEnd)
	{
	// This function completes as far as the next choice point
	// aString is a pointer into a buffer with plenty of space in it
		
	//Dump(); // DEBUG
	if (!iHasChildArray && iLetter && iPtr && aString+1 < aEnd)
		{
		*aString = Sprog()->iLetter;
		aString++;
		*aString = 0;
		return Sprog()->TabFill(aString, aEnd);
		}
	else
		{
		return this;
		}
	}

void RNode::CompleteL(TDes& aPrefix, CDesC16Array& aResults)
	{
	TUint16* ptr = (TUint16*)aPrefix.PtrZ();
	TUint16* end = ptr + aPrefix.MaxLength();
	RNode* endNode = WalkToEndOfString(ptr);
	if (ptr != aPrefix.Ptr() + aPrefix.Length()) return; // We failed to walk the whole of the string so we have nothing to suggest (the string is longer than anything in the tree)

	endNode = endNode->TabFill(ptr, end);
	TInt newLen = ptr - aPrefix.Ptr();
	if (endNode->iLetter == 0 && newLen > 0) newLen--; // If ptr got as far as the null termination, we need to make our length one shorter
	//RDebug::Printf("Setting aPrefix len to %d, len=%d maxlen=%d", newLen, aPrefix.Length(), aPrefix.MaxLength());
	aPrefix.SetLength(newLen);

	// Now that aPrefix has been completed with as much as didn't have any choices, fill aResults with the choices
	RLtkBuf current;
	current.CreateLC(256);
	current.AppendL(aPrefix);
	if (current.Length()) current.SetLength(current.Length()-1); // Remove endNode's char as DoCompleteOptionsL expects to do that bit
	endNode->DoCompleteOptionsL(aResults, current);
	// Make sure last string is included
	aResults.AppendL(current);
	CleanupStack::PopAndDestroy(&current);
	aResults.Sort();
	}
	
void RNode::DoCompleteOptionsL(CDesC16Array& aResults, RLtkBuf& aCurrent)
	{
	if (iLetter)
		{
		aCurrent.ReserveExtraL(1);
		aCurrent.Append(TChar(iLetter));
		}
	TInt currentLen = aCurrent.Length();

	if (iHasChildArray)
		{
		for (int i = 0; i < Sprogs().Count(); i++)
			{
			RNode* child = Sprogs()[i];
			if (i > 0)
				{
				// A new prefix is in order
				aResults.AppendL(aCurrent);
				aCurrent.SetLength(currentLen);
				}
			child->DoCompleteOptionsL(aResults, aCurrent);
			}
		}
	else if (iLetter != 0)
		{
		if (Sprog()) Sprog()->DoCompleteOptionsL(aResults, aCurrent);
		}
	}

RNode::~RNode()
	{
	if (iHasChildArray)
		{
		RArray<RNode*>& children = Sprogs();
		const TInt count = children.Count();
		for (TInt i = 0; i < count; i++)
			{
			RNode* node = children[i];
			delete node;
			}
		children.Close();
		delete &children;
		}
	else if (iLetter != 0)
		{
		delete Sprog();
		}
	}

/*
#include <e32debug.h>
void RNode::Dump() const
	{
	TInt sprogCount = Sprogs().Count();
	for (int i = 0; i < sprogCount; i++)
		{
		RNode* child = Sprogs()[i];
		RDebug::Printf("Sprog %i is letter %c", i, child->iLetter);
		}
	}
*/

TInt RNode::ValueForStringL(const TDesC& aString) const
	{
	const RNode* node = this;
	for (TInt i = 0; i < aString.Length(); i++)
		{
		node = node->ChildForLetter(aString[i]);
		if (!node) User::Leave(KErrNotFound);
		}
	node = node->ChildForLetter(0); // The null terminator node is the one with the value in
	if (!node) User::Leave(KErrNotFound);
	ASSERT(!node->iHasChildArray);
	return reinterpret_cast<TInt>(node->iPtr);
	}
