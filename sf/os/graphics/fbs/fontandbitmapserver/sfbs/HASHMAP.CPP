// HASHMAP.CPP
//
// This file holds the class methods for the CHashMap
// A hash function for hash table lookup.  Assumes input data length is a multiple of 8-bits.
// 
// The original hash function was sourced from http://burtleburtle.net/bob/hash/index.html
// "By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this code any way you wish, 
// private, educational, or commercial.  It's free."
// portions Copyright (c) 1995 - 2010 Nokia Corporation and/or its subsidiary(-ies).
//

#include "SERVER.H"

CHashMap::CHashMap() :
	iTableSize(sizeof(iHashTable)/sizeof(CSharedBitmapObject*)),
	iMask(iTableSize-1)
	{
	}
	

CHashMap::~CHashMap()
	{
	for (TInt i=0; i<iTableSize; i++)
		{
		__ASSERT_DEBUG(iHashTable[i]==NULL, User::Invariant());
		}
	}


/* This function inserts a pointer to the CSharedBitmapObject into the hashmap.
The hashmap does not acquire ownership of the CSharedBitmapObject.
The key used for insertion is a pre-existing member of the CSharedBitmapObject.
The caller must ensure the key does not already exist in the hashmap prior to calling this function.

@param aBmpObj The CSharedBitmapObject to insert.
@param aHash The hash of the key of the CSharedBitmapObject.  This is provided for performance reasons explained in CFbTop::ShareBitmap.
@internalComponent
*/
void CHashMap::Insert(CSharedBitmapObject& aBmpObj, TUint aHash)
	{
	__ASSERT_DEBUG(aHash==Hash(*(aBmpObj.iKey)), User::Invariant());     // Verify the hash value
	__ASSERT_DEBUG(!Lookup(*(aBmpObj.iKey), aHash), User::Invariant());  // Duplicate keys are not allowed
	
	const TUint index = aHash & iMask;

	// Insert aBmpObj at head of list
	aBmpObj.iNext = iHashTable[index];
	iHashTable[index] = &aBmpObj;
	}


/* This function looks up a CSharedBitmapObject in the hashmap using a key.
Lookup is case sensitive.

@param aKey The key of the CSharedBitmapObject to be found.
@param aHash The hash of the key.  This is provided for performance reasons explained in CFbTop::ShareBitmap.
@return A pointer to the CSharedBitmapObject if the key is found.  NULL otherwise.
@internalComponent
*/
CSharedBitmapObject* CHashMap::Lookup(const TDesC& aKey, TUint aHash) const
	{
	const TUint index = aHash & iMask;

	for (CSharedBitmapObject* n=iHashTable[index]; n!=NULL; n=n->iNext) // n walks the links of the linked list.
		{
		if (n->iKey->Length()==aKey.Length() && !n->iKey->Compare(aKey))
			{
			return n;  // Key found
			}
		}

	return NULL;   // Key not found
	}


/* Removes a specific object from the hashmap.
The object is discovered using its key and address.
This function does not destroy the object.

@param aBmpObj The specific CSharedBitmapObject to be removed.
@return KErrNone if the object is found and removed.  An standard error otherwise.
@internalComponent
*/
TInt CHashMap::Remove(const CSharedBitmapObject& aBmpObj)
	{
	const TUint hash = Hash(*(aBmpObj.iKey));
	const TUint index = hash & iMask;

	for (CSharedBitmapObject** n=&iHashTable[index]; *n!=NULL; n=&((*n)->iNext))  // *n walks the links of the linked list.
		{
		if (&aBmpObj==*n)
			{
			*n = aBmpObj.iNext;  // Key found.  The link that was pointing to aBmpObj nows points to the successor of aBmpObj.
			return KErrNone;
			}	
		}

	return KErrNotFound;
	}


// Utility macro for the hash function
#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8);  \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12); \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5);  \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/* A hash function for hash table lookup.
Assumes input data length is a multiple of 8-bits.

The original hash function was sourced from
http://burtleburtle.net/bob/hash/index.html
"By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free."

TUint CHashMap::Hash(const TDesC8& aKey)
	{
	const TUint8* k = aKey.Ptr();
	const TUint length = aKey.Length();
	const TUint initval = 0;

	TUint len = length;
	TUint a = 0x9e3779b9;
	TUint b = 0x9e3779b9;
	TUint c = initval;

	while (len >= 12)
		{
		a += (k[0] + ((TUint)k[1]<<8) + ((TUint)k[2] <<16) + ((TUint)k[3] <<24));
		b += (k[4] + ((TUint)k[5]<<8) + ((TUint)k[6] <<16) + ((TUint)k[7] <<24));
		c += (k[8] + ((TUint)k[9]<<8) + ((TUint)k[10]<<16) + ((TUint)k[11]<<24));
		mix(a,b,c);
		k += 12; len -= 12;
		}

	c += length;
	switch(len)
		{
		case 11: c+=((TUint)k[10]<<24);
		case 10: c+=((TUint)k[9]<<16);
		case 9 : c+=((TUint)k[8]<<8);
      // the first byte of c is reserved for the length
		case 8 : b+=((TUint)k[7]<<24);
		case 7 : b+=((TUint)k[6]<<16);
		case 6 : b+=((TUint)k[5]<<8);
		case 5 : b+=k[4];
		case 4 : a+=((TUint)k[3]<<24);
		case 3 : a+=((TUint)k[2]<<16);
		case 2 : a+=((TUint)k[1]<<8);
		case 1 : a+=k[0];
     // case 0: nothing left to add
		}
	mix(a,b,c);

	return c;
	}
	
*/


/* A hash function for hash table lookup.
Assumes input data length is a multiple of 16-bits.
@param aKey The data to be hashed
@return The hash value
@internalComponent
*/
TUint CHashMap::Hash(const TDesC16& aKey)
	{
	const TUint16* k = aKey.Ptr();
	const TUint length = aKey.Length();
	const TUint initval = 0;

	TUint len = length;
	TUint a = 0x9e3779b9;
	TUint b = 0x9e3779b9;
	TUint c = initval;

	while (len >= 6)
		{
		a += (k[0] + ((TUint)k[1]<<16));
		b += (k[2] + ((TUint)k[3]<<16));
		c += (k[4] + ((TUint)k[5]<<16));
		mix(a,b,c);
		k += 6; len -= 6;
		}

	c += length;
	switch(len)
		{
		case 5 : c+=((TUint)k[4]<<16);
      // the first byte of c is reserved for the length
		case 4 : b+=((TUint)k[3]<<16);
		case 3 : b+=k[2];
		case 2 : a+=((TUint)k[1]<<16);
		case 1 : a+=k[0];
     // case 0: nothing left to add
		}
	mix(a,b,c);

	return c;
	}
