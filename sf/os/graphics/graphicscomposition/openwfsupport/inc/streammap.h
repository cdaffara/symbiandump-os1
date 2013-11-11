// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// streammap.h: defines COpenWfcStreamMap
// 
//

#ifndef STREAMMAP_H
#define STREAMMAP_H

#include <e32base.h>
#include <e32std.h>			//for RFastLock
#include <e32hashtab.h>		//for RHashMap

class CSurfaceStream;
class TSurfaceId;
class MSurfaceUpdateServerProvider;
class CExtensionContainer;
class RSurfaceManager;

NONSHARABLE_CLASS(COpenWfcStreamMap): public CBase
	{
	public:
		/**
		 * Returns a reference to the singleton instance.
		 * 
		 * @return	The pointer to the singleton instance
		 */
		IMPORT_C static COpenWfcStreamMap& InstanceL();
		/**
		 * Look up a specified TSurfaceId key in the associative array and return a pointer to the
		 * corresponding to a native stream. The reference counter of the native stream is incremented by one.
		 * 
		 * @param	aSurfaceId	The TSurfaceId key to look up
		 * @return	A pointer to the corresponding native stream, if the specified TSurfacId was found
		 * 			NULL if the look up did not succeed
		 */
		CSurfaceStream* Find(const TSurfaceId& aSurfaceId);
		/**
		 * Look up a specified TSurfaceId key in the associative array and return a pointer to the
		 * corresponding to a native stream. The reference of the native stream is incremented by one.
		 * If no native stream is found a new native stream instance is created.
		 * 
		 * @param	aSurfaceId	The TSurfaceId key to look up
		 * @return	A pointer to the corresponding native stream, if the specified TSurfacId was found
		 * 			NULL if the look up did not succeed
		 */
		CSurfaceStream* AcquireL(const TSurfaceId& aSurfaceId);
		/**
		 * Query the number of elements stored in the stream hash map.
		 * 
		 * @return	The number of the key-value pairs stored
		 */
		IMPORT_C TInt Count();
		
	public:
	    
        /**
         * Register a screen number for surface update notifications.
         * 
         * @param   aScreenNum  The screen for which we enable the notification mechanism
         * @param   aPriority  The priority associated with the screen 
         * @param   aPriority  The internal version 
         * @return  KErrNone if operation is completed successfully
         */
        TInt    RegisterScreenNotifications(TInt aScreenNum, TInt aPriority,TInt aInternalVersion);
        
        /**
         * Retieves the screen updater associate with the specified screen
         * 
         * @param   aScreenNum  The screen number
         * @return  Pointer to screen updater id successful or NULL otherwise
         */
        CExtensionContainer*    RegisteredScreenNotifications(TInt aScreenNum);
        
        /**
         * Unregister the screen updater
         * 
         * @param   aScreenNum  The screen number
         * @return  KErrNone if operation is completed successfully
         */
        TInt    UnregisterScreenNotifications(TInt aScreenNum);
        
		/**
		 * Returns a reference to the surface manager.
		 * 
		 * @return	A reference to the local SurfaceManager
		 */
        IMPORT_C RSurfaceManager& SurfaceManager();
		
        /**
         * Returns a pointer to the main heap
         * 
         * @return  A pointer to the main heap
         */
		IMPORT_C RHeap* GetMainHeap();
		
        /**
         * Sets the main heap
        */
		void SetMainHeap();
		
	protected:
		friend class CSurfaceStream;
		/**
		 * Destroys the specified native stream IF the native stream reference counter is zero.
		 * The counter reference of the native stream is tested prior its destruction and
		 * if positive the destruction will not happen
		 * 
		 * @param	aStream	The stream to be destroyed
		 * @return	KErrNone if the native stream is removed
		 * 			KErrArgument if the parameter is invalid
		 * 			KErrInUse if the stream has a owner
		 * 			KErrNotFound if the TSurfaceId key was not found
		 */
		TInt LockDestroy(CSurfaceStream* aStream);
		/**
		 * Constructor
		 */
		COpenWfcStreamMap();
		/**
		 * Destructor.
		 */
		~COpenWfcStreamMap();
	private:		
		/**
		 * Auxilliary private class to insure the release of a lock
		 */
		class Guard
			{
			public:
			Guard(RFastLock& aLock);
			~Guard();
			private:
				RFastLock& iLock;
			};
	private:
		/**
		 * Symbian constructor used with two stage construction pattern
		 */
		void ConstructL();
		/**
		Forms a 32-bit hash value from a TSurfaceId.

		@param aHashKey The 64-bit key to be hashed.
		@return 32-bit hash value.
		*/
		static TUint32 HashFunction(const TSurfaceId& aHashKey);
		
		static TInt COpenWfcStreamMap::DeleteSingleton(TAny* aData);
	private:
		/**
		 * Mutex used for controlling the access to the native streams map
		 */
		RFastLock iMutex;
		/**
		 * Native stream map
		 */
		RHashMap<TSurfaceId, CSurfaceStream*> iMap;
		/**
		 * Initial HashMap size
		 */
		static const TInt iInitialSize = 0;
		/**
		 * Singleton instance placeholder
		 */
		static COpenWfcStreamMap* pInstance;
		/**
		 * Surface manager
		 */
		RSurfaceManager* iSurfaceManager;

		RHeap *iMainHeap; //< --This points to main thread's heap--
		
		RHashMap<TInt32, CExtensionContainer*>	iRegisteredUpdaters; //< Proxy objects for handing surface update notifications.
		
		MSurfaceUpdateServerProvider*   iSurfUpdateServ;    // Pointer to the surface update server
	};

#endif /* STREAMMAP_H */
