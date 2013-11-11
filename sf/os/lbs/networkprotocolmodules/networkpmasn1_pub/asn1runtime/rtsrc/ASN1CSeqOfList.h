// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 


/**
@file
@internalTechnology
*/

#ifndef _ASN1CSEQOFLIST_H_
#define _ASN1CSEQOFLIST_H_

#ifndef __SYMBIAN32__   /* OS: Symbian */
#include <new>
#endif

#include <stdlib.h>
#include "rtsrc/asn1CppTypes.h"
/**
 * @file rtsrc/ASN1CSeqOfList.h 
 * ASN1CSeqOfList linked list control class definition.
 */
/**
 * @addtogroup asn1ctrl 
 * @{
 */

//
//
// ASN1CSeqOfList
//
// Doubly-linked list implementation. This class provides all functionalty
// necessary for linked list. To iterate list use methods iterator, 
// iteratorFrom, iteratorFromLast. 
//
// Note: this implementation is not thread-safe.
//
// ASN1CSeqOfListIterator
//
// An iterator for lists that allows the programmer to traverse the
// list in either direction and modify the list during iteration.
//
// Author  Artem Bolgar
// version 1.09, 12 Nov, 2003
// version 1.08, 11 Feb, 2003
//
#ifndef _NO_UTILS_CLASSES

class EXTRTCLASS ASN1CSeqOfList;

//
//
// ASN1CSeqOfListIterator
//
/**
 * Linked list iterator class. The ASN1CSeqOfListIterator class is an iterator
 * for linked lists (represented by ASN1CSeqOfList) that allows the programmer
 * to traverse the list in either direction and modify the list during
 * iteration. The iterator is fail-fast. This means the list is structurally
 * modified at any time after the ASN1CSeqOfListIterator class is created, in
 * any way except through the iterator's own remove or insert methods, the
 * iterator's methods next and prev methods will return NULL. The remove, set
 * and insert methods will return the RTERR_CONCMODF error code.
 */
class EXTRTCLASS ASN1CSeqOfListIterator {
   friend class ASN1CSeqOfList;
 protected:
   ASN1CSeqOfList* pSeqList; // pointer to the list
   OSRTDListNode* nextNode; // next node
   OSRTDListNode* lastNode; // last returned node
   volatile int expectedModCount; // expect modification counter
   int stat;

   EXTRTMETHOD ASN1CSeqOfListIterator(ASN1CSeqOfList* list);
   EXTRTMETHOD ASN1CSeqOfListIterator(ASN1CSeqOfList* list, OSRTDListNode* startNode);

 public:
  /**
   * This method returns TRUE if this iterator has more elements when
   * traversing the list in the forward direction.
   *
   * In other words, the method returns TRUE if the \c next method would return
   * an element rather than returning a null value.
   *
   * @return             TRUE if next would return an element rather than
   *                       returning a null value.
   */
   inline OSBOOL hasNext() { return OSBOOL(nextNode != 0); }

  /**
   * This method returns TRUE if this iterator has more elements when
   * traversing the list in the reverse direction.
   *
   * In other words, this method will return TRUE if prev would return an
   * element rather than returning a null value.
   *
   * @return             TRUE if next would return an element rather than
   *                       returning a null value.
   */ 
   inline OSBOOL hasPrev() { return OSBOOL(nextNode != 0); }
   
  /**
   * This method returns the next element in the list.
   *
   * This method may be called repeatedly to iterate through the list or
   * intermixed with calls to prev to go back and forth.
   *
   * @return             The next element in the list. A null value will be
   *                       returned if the iteration is not successful.
   */
   EXTRTMETHOD void* next();

  /**
   * This method returns the previous element in the list.
   *
   * This method may be called repeatedly to iterate through the list or
   * intermixed with calls to next to go back and forth.
   *
   * @param             - none
   * @return             The previous element in the list. A null value will be
   *                       returned if the iteration is not successful.
   */
   EXTRTMETHOD void* prev();

  /**
   * This method removes from the list the last element that was returned by
   * the next or prev methods.
   *
   * This call can only be made once per call to the next or prev methods.
   *
   * @param             - none
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int remove();

  /**
   * This method replaces the last element returned by the next or prev methods
   * with the specified element.
   *
   * This call can be made only if neither remove nor insert methods have been
   * called after the last call to next or prev methods.
   *
   * @param data         The element that replaces the last element returned by
   *                       the next or prev methods
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int set(void* data);

  /**
   * This method inserts the specified element into the list.
   *
   * The element is inserted immediately before the next element that would be
   * returned by the next method, if any, and after the next element would be
   * returned by the prev method, if any. If the list contains no elements, the
   * new element becomes the sole element in the list. The new element is
   * inserted before the implicit cursor: a subsequent call to next would be
   * unaffected, and a subsequent call to prev would return the new element.
   *
   * @param data         The element to be inserted
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int insert(void* data);

   /* Returns the state of iterator. 0 if it is OK, RTERR_* value otherwise */
   inline int getState () { return stat; }

 protected:
   inline void* operator new(size_t, void* data) { return data; }
#if !defined(__xlC__)
   inline void operator delete(void*, void*) {}
#endif
#ifndef __BORLANDC__
   inline void operator delete(void*, size_t) {}
#endif
} ;


//
//
// ASN1CSeqOfList
//
/**
 * Doubly-linked list implementation. This class provides all functionality
 * necessary for linked list operations. It is the base class for ASN1C
 * compiler-generated ASN1C_ control classes for SEQUENCE OF and SET OF PDU
 * types.
 */
class EXTRTCLASS ASN1CSeqOfList : public ASN1CType {
   friend class ASN1CSeqOfListIterator;
 protected:
   OSRTDList* pList;           // list
   volatile int modCount;  // modification counter
   OSBOOL wasAssigned;

   // The following protected ctor could be used to perform lists' operation, 
   // which do not require the memory allocation.
   EXTRTMETHOD ASN1CSeqOfList (OSRTDList& lst);
   EXTRTMETHOD ASN1CSeqOfList (ASN1TSeqOfList& lst);
   EXTRTMETHOD ASN1CSeqOfList (ASN1TPDUSeqOfList& lst);

 public:

  /**
   * This constructor creates a linked list using the OSRTDList argument. The
   * constructor does not deep-copy the variable; it assigns a reference to it
   * to an external variable.
   *
   * The object will then directly operate on the given list variable.
   *
   * @param msgBuf           Reference to an ASN1Message buffer derived object
   *                           (for example, an ASN1BEREncodeBuffer).
   * @param lst              Reference to a linked list structure.
   * @param initBeforeUse    Set to TRUE if the passed linked list needs to be
   *                           initialized (rtxDListInit to be called).
   */
   EXTRTMETHOD ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf, OSRTDList& lst, 
      OSBOOL initBeforeUse = TRUE);

  /**
   * This constructor creates an empty linked list.
   *
   * @param msgBuf       Reference to an ASN1Message buffer derived object (for
   *                       example, an ASN1BEREncodeBuffer).
   */
   EXTRTMETHOD ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf);

  /**
   * This constructor creates an empty linked list.
   *
   * @param ccobj        Reference to a control class object (for example, 
   *                       any generated ASN1C_ class object).
   */
   EXTRTMETHOD ASN1CSeqOfList (ASN1CType& ccobj);

  /**
   * This constructor creates a linked list using the ASN1TSeqOfList (holder of
   * OSRTDList) argument.
   *
   * The construction does not deep-copy the variable, it assigns a reference
   * to it to an internal variable. The object will then directly operate on
   * the given list variable. This constructor is used with a
   * compiler-generated linked list variable.
   *
   * @param msgBuf       Reference to an ASN1Message buffer derived object (for
   *                       example, an ASN1BEREncodeBuffer).
   * @param lst          Reference to a linked list holder.
   */
   EXTRTMETHOD ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf, ASN1TSeqOfList& lst);

  /**
   * This constructor creates a linked list using the ASN1TSeqOfList (holder of
   * OSRTDList) argument.
   *
   * The construction does not deep-copy the variable, it assigns a reference
   * to it to an internal variable. The object will then directly operate on
   * the given list variable. This constructor is used with a
   * compiler-generated linked list variable.
   *
   * @param ccobj        Reference to a control class object (for example, 
   *                       any generated ASN1C_ class object).
   * @param lst          Reference to a linked list holder.
   */
   EXTRTMETHOD ASN1CSeqOfList (ASN1CType& ccobj, ASN1TSeqOfList& lst);

  /**
   * This constructor creates a linked list using the ASN1TPDUSeqOfList 
   * argument.
   *
   * The construction does not deep-copy the variable, it assigns a reference
   * to it to an internal variable. The object will then directly operate on
   * the given list variable. This constructor is used with a
   * compiler-generated linked list variable.
   *
   * @param msgBuf       Reference to an ASN1Message buffer derived object (for
   *                       example, an ASN1BEREncodeBuffer).
   * @param lst          Reference to a linked list holder.
   */
   EXTRTMETHOD ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf, ASN1TPDUSeqOfList& lst);

   EXTRTMETHOD ASN1CSeqOfList (OSRTContext& ctxt, OSRTDList& lst, 
      OSBOOL initBeforeUse = TRUE);
   EXTRTMETHOD ASN1CSeqOfList (OSRTContext& ctxt);
   EXTRTMETHOD ASN1CSeqOfList (OSRTContext& ctxt, ASN1TSeqOfList& lst);
   EXTRTMETHOD ASN1CSeqOfList (OSRTContext& ctxt, ASN1TPDUSeqOfList& lst);

   EXTRTMETHOD ~ASN1CSeqOfList();

   // Appends new list node with data
  /**
   * This method appends an item to the linked list.
   *
   * This item is represented by a void pointer that can point to an object of
   * any type. The rtxMemAlloc function is used to allocate memory for the list
   * node structure, therefore, all internal list memory will be released
   * whenever rtxMemFree is called.
   *
   * @param data         Pointer to a data item to be appended to the list.
   * @return             - none
   */
   EXTRTMETHOD void append(void* data);

   // Appends array to list data. Data won't be copied, just assigned.
  /**
   * This method appends array items' pointers to a doubly linked list.
   *
   * The rtxMemAlloc function is used to allocate memory for the list node
   * structure, therefore all internal list memory will be released whenever
   * the rtxMemFree is called. The data is not copied; it is just assigned to
   * the node.
   *
   * @param data         Pointer to source array to be appended to the list.
   * @param numElems     The number of elements in the source array.
   * @param elemSize     The size of one element in the array. Use the
   *                       <i>sizeof()</i> operator to pass this parameter.
   * @return             - none
   */
   EXTRTMETHOD void appendArray(const void* data, int numElems, int elemSize);

   // Appends array to list data. Data will be copied.
  /**
   * This method appends array items into a doubly linked list.
   *
   * The rtxMemAlloc function is used to allocate memory for the list node
   * structure; therefore all internal list memory will be released whenever
   * rtxMemFree is called. The data will be copied; the memory will be 
   * allocated using rtxMemAlloc.
   *
   * @param data         Pointer to source array to be appended to the list.
   * @param numElems     The number of elements in the source array.
   * @param elemSize     The size of one element in the array. Use the sizeof()
   *                       operator to pass this parameter.
   * @return             - none
   */
   EXTRTMETHOD void appendArrayCopy(const void* data, int numElems, int elemSize);

  /**
   * This method initializes the linked list structure.
   */
   inline void init () {
      rtxDListInit (pList);
   }

   // Inserts new list node at the specified index
  /**
   * This method inserts an item into the linked list structure.
   *
   * The item is represented by a void pointer that can point to an object of
   * any type. The rtxMemAlloc function is used to allocate memory for the list
   * node structure. All internal list memory will be released when the
   * rtxMemFree function is called.
   *
   * @param index        Index at which the specified item is to be inserted.
   * @param data         Pointer to data item to be appended to the list.
   * @return             - none
   */
   EXTRTMETHOD void insert(int index, void* data);

   // Removes list node at specified index from the list 
  /**
   * This method removed a node at the specified index from the linked list
   * structure.
   *
   * The rtxMemAlloc function was used to allocate the memory for the list node
   * structure, therefore, all internal list memory will be released whenever
   * the rtxMemFree is called.
   *
   * @param index        Index of the item to be removed.
   * @return             - none
   */
   EXTRTMETHOD void remove(int index);

   // Removes  the first occurrence of the specified element in the list.
  /**
   * This method removes the first occurrence of the node with specified data
   * from the linked list structure.
   *
   * The rtxMemAlloc function was used to allocate the memory for the list node
   * structure, therefore, all internal list memory will be released whenever
   * the rtxMemFree function is called.
   *
   * @param data         - Pointer to the data item to be appended to the list.
   */
   EXTRTMETHOD void remove(void* data);

   // Removes the first element from the list.
  /**
   * This method removes the first node (head) from the linked list structure.
   *
   * @param             - none
   * @return             - none
   */
   inline void removeFirst() {
      remove(pList->head);
   }

   // Removes the last element from the list.
  /**
   * This method removes the last node (tail) from the linked list structure.
   *
   * @param             - none
   * @return             - none
   */
   inline void removeLast() {
      remove(pList->tail);
   }

   // Returns index of the list node with specified data
  /**
   * This method returns the index in this list of the first occurrence of the
   * specified item, or -1 if the list does not contain the time.
   *
   * @param data         - Pointer to data item to searched.
   * @return             The index in this list of the first occurrence of the
   *                       specified item, or -1 if the list does not contain
   *                       the item.
   */
   EXTRTMETHOD int indexOf(void* data) const;

   // Returns 'TRUE' if this list contains the specified element.
  /**
   * This method returns TRUE if this list contains the specified pointer. Note
   * that a match is not done on the <i>contents</i> of each data item (i.e.
   * what is pointed at by the pointer), only the pointer values.
   *
   * @param data         - Pointer to data item.
   * @return             TRUE if this pointer value found in the list.
   */
   inline OSBOOL contains (void* data) const {
      return indexOf(data) != -1;
   }

  /**
   * This method returns the first item from the list or null if there are no
   * elements in the list.
   *
   * @return             The first item of the list.
   */
   EXTRTMETHOD void* getFirst();

  /**
   * This method returns the last item from the list or null if there are no
   * elements in the list.
   *
   * @return             The last item in the list.
   */
   EXTRTMETHOD void* getLast();

  /**
   * This method returns the item at the specified position in the list.
   *
   * @param index        Index of the item to be returned.
   * @return             The item at the specified index in the list.
   */
   EXTRTMETHOD void* get (int index) const;

  /**
   * This method replaces the item at the specified index in this list with the
   * specified item.
   *
   * @param index        The index of the item to be replaced.
   * @param data         The item to be stored at the specified index.
   * @return             The item previously at the specified position.
   */
   EXTRTMETHOD void* set (int index, void* data);

  /**
   * This method removes all items from the list.
   */
   EXTRTMETHOD void clear();

  /**
   * This method removes all items from the list and frees the associated
   * memory.
   */
   EXTRTMETHOD void free();

  /**
   * This method returns TRUE if the list is empty.
   *
   * @return             TRUE if this list is empty.
   */
   EXTRTMETHOD OSBOOL isEmpty() const;

  /**
   * This method returns the number of nodes in the list.
   *
   * @return             The number of items in this list.
   */
   EXTRTMETHOD int size() const;

  /**
   * This method returns an iterator over the elements in the linked list in
   * the sequence from the fist to the last.
   *
   * @return                        The iterator over this linked list.

   */
   EXTRTMETHOD ASN1CSeqOfListIterator* iterator();

   // Creates iterator from the tail of the list
  /**
   * This method creates a reverse iterator over the elements in this linked
   * list in the sequence from last to first.
   *
   * @param                        - none
   * @return                        The reverse iterator over this linked list.

   */
   EXTRTMETHOD ASN1CSeqOfListIterator* iteratorFromLast();

   // Creates iterator from the node with specified data
  /**
   * This method runs an iterator over the elements in this linked list
   * starting from the specified item in the list.
   *
   * @param data                    The item of the list to be iterated first.
   * @return                        The iterator over this linked list.

   */
   EXTRTMETHOD ASN1CSeqOfListIterator* iteratorFrom(void* data);

  /**
   * This method converts the linked list into a new array.
   *
   * The rtxMemAlloc function is used to allocate memory for an array.
   *
   * @param elemSize     The size of one element in the array. Use the
   *                       <i>sizeof()</i> operator to pass this parameter.
   * @return             The point to converted array.
   */
   EXTRTMETHOD void* toArray (int elemSize);
   
   // Converts to array
  /**
   * This method converts the linked list into an array.
   *
   * The rtxMemAlloc function is used to allocate memory for the array if the
   * capacity of the specified array is exceeded.
   *
   * @param pArray            Pointer to destination array.
   * @param elemSize          The size of one element in the array. Use the
   *                            sizeof() operator to pass this parameter.
   * @param allocatedElems    The number of elements already allocated in the
   *                            array. If this number is less than the number
   *                            of nodes in the list, then a new array is
   *                            allocated and returned. Memory is allocated
   *                            using rtxMemAlloc function.
   * @return                  The pointer to the converted array.
   */
   EXTRTMETHOD void* toArray (void* pArray, int elemSize, int allocatedElems);
   
   // Returns element at specified index
  /**
   * This method is the overloaded operator[].
   *
   * It returns the item at the specified position in the list.
   *
   * @see get           (int index)
   */
   inline void* operator[](int index) const {
      return get(index);
   }

   inline operator OSRTDList* () {
      return pList;
   }
 protected: 
   // Removes specified node from the list
   EXTRTMETHOD void remove(OSRTDListNode* node);

   // Inserts new node ('data') before another node ('node')
   EXTRTMETHOD void insertBefore(void* data, OSRTDListNode* node);

   // Inserts new node ('data') after another node ('node')
   EXTRTMETHOD void insertAfter(void* data, OSRTDListNode* node);


} ;
#else
typedef class _ASN1CSeqOfList : public ASN1CType {
 public:
   _ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf, OSRTDList& lst, 
      OSBOOL initBeforeUse = TRUE) : ASN1CType (msgBuf) {}
   _ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf, ASN1TSeqOfList& lst) :
      ASN1CType (msgBuf) {}
   _ASN1CSeqOfList (OSRTMessageBufferIF& msgBuf) : ASN1CType (msgBuf) {}
} ASN1CSeqOfList;

#endif // _NO_UTILS_CLASSES

/**
 * @}
 */

#endif // _ASN1CSEQOFLIST_H_
