// ltkutils.inl
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
namespace LtkUtils
	{
	template <class T>
	class CleanupDestroy
		{
	public:
		inline static void PushL(T& aRef) { CleanupStack::PushL(TCleanupItem(&Destroy, &aRef)); }
	private:
		static void Destroy(TAny *aPtr) { static_cast<T*>(aPtr)->ResetAndDestroy(); }
		};
		
	template <class T>
	class CleanupReset
		{
	public:
		inline static void PushL(T& aRef) { CleanupStack::PushL(TCleanupItem(&Reset, &aRef)); }
	private:
		static void Reset(TAny *aPtr) { static_cast<T*>(aPtr)->Reset(); }
		};
		
	template <class T>
	inline void CleanupResetAndDestroyPushL(T& aRef) { CleanupDestroy<T>::PushL(aRef); }

	template <class T>
	inline void CleanupResetPushL(T& aRef) {CleanupReset<T>::PushL(aRef);}

	}
