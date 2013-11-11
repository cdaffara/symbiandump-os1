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
//

#ifndef E32BTRACE_H
#define E32BTRACE_H

/**
Class for handling fast tracing.

A trace record consists of three parts: a header, header extensions,
and the trace data itself.

The header consists of four bytes containing:

-#	Size of the record in bytes. (Maximum value is KMaxBTraceRecordSize.)
-#	Flags. See enum TFlags.
-#	Category. Category value from enum BTrace::TCategory.
-#	Sub-category. The meaning of this is dependent on the value of Category.

When trace records are stored in memory they are stored word (32 bit) aligned.
Therefore the size must be rounded up to a multiple of four when calculating
the address of the next record. E.g.
@code
	TUint8* record; // pointer to trace record
	TInt size = record[BTrace::ESizeIndex];
	record += (size+3)&~3; // move record pointer on to next record.
@endcode
The NextRecord() method is provided to do this operation.

Following the header are optionally a number of 32 bit 'header extension' values.
These are present in the order shown below but only exist if the appropriate flag bit
is set in the Header.

-#	Header2.		Contains flag values from enum Flags2.
					This value is only present if the EHeader2Present flag is set.
-#	Timestamp.		A timestamp value indicating when the trace was generated.
					The format and resolution of this value are platform-dependent, but
					typically will contain the same values as would be returned by
					User::FastCounter() or NKern::FastCounter().
					This value is only present if the ETimestampPresent flag is set.
-#	Timestamp2.		Additional timestamp information. E.g. the most significant
					half of a 64bit timestamp value. Note, it is valid for a Timestamp2 value
					to be present even if the previous Timestamp is absent.
					This value is only present if the ETimestamp2Present flag is set.
-#	Context ID.		This value indicates the context in which the trace was generated.
					The meaning of the id is dependent on the contents of the two
					least significant bits:
					-	00	indicates the value is the address of the NThread object for
							the currently executing thread.
					-	01	indicates Fast Interrupt (FIQ) context.
							Other bits of the value are currently reserved for future use.
					-	10	indicates Interrupt (IRQ) context. Other bits of the value
							are currently reserved for future use.
					-	11	indicates Immediate Delayed Function Call (IDFC) context.
							Other bits of the value are currently reserved for future use.
					.
					This value is only present if the EContextIdPresent flag is set.
-#	Program Counter. This is the memory address of the instruction after the location
					the trace was output.
					This value is only present if the EPcPresent flag is set.
-#	Extra.			An extra value used for different purposes depending on the trace type.
					This value is only present if the EExtraPresent flag is set.

Following the header extensions are 0 or more bytes of trace data specified when the trace
was output.

To output a trace, the following macros can be used:
- BTrace0
- BTrace4
- BTrace8
- BTrace12
- BTraceN
- BTraceBig
- BTracePc0
- BTracePc4
- BTracePc8
- BTracePc12
- BTracePcN
- BTracePcBig
- BTraceContext0
- BTraceContext4
- BTraceContext8
- BTraceContext12
- BTraceContextN
- BTraceContextBig
- BTraceContextPc0
- BTraceContextPc4
- BTraceContextPc8
- BTraceContextPc12
- BTraceContextPcN
- BTraceContextPcBig

Whenever a trace is output, the trace handler is called with the arguments specified.
See typedef THandler and SetHandler().

Each tracing category has a filter bit, which if set to zero means that traces in that category
are discarded, see SetFilter(). This filtering is performed before the trace handler is
called. This filter may also be initialised from boot time by using the 'btrace' keyword in
an OBY file used to build a ROM image.

Traces may also be additionally sent through a second level of filtering. This examines the
first 32 bits of data in the trace and if this value isn't present in the list maintained
in the secondary filter, the trace is discarded. The contents of the secondary filter are
set using the SetFilter2 methods.

Values used for secondary filtering must be Symbian Unique Identifiers (UIDs) allocated
using the normal UID allocation process. Note, the following non-valid UID value ranges
are reserved.
- 0x00000000..0x007fffff	Reserved for platform specific use.
- 0x00800000..0x00ffffff	Reserved for use by Symbian.

To generate traces which are to be processed by the secondary filter, the following
macros can be used:

- BTraceFiltered4
- BTraceFiltered8
- BTraceFiltered12
- BTraceFilteredN
- BTraceFilteredBig
- BTraceFilteredPc4
- BTraceFilteredPc8
- BTraceFilteredPc12
- BTraceFilteredPcN
- BTraceFilteredPcBig
- BTraceFilteredContext4
- BTraceFilteredContext8
- BTraceFilteredContext12
- BTraceFilteredContextN
- BTraceFilteredContextBig
- BTraceFilteredContextPc4
- BTraceFilteredContextPc8
- BTraceFilteredContextPc12
- BTraceFilteredContextPcN
- BTraceFilteredContextPcBig

Traces generated using the above methods will be filtered twice; once using the primary
filter which checks the trace's category, and once using the secondary filter which checks
the 32 bit UID value at the start of the trace data. Therefore the trace must pass both filter
checks for it to be sent to the trace handler for output.

@publishedPartner
@released
*/
class BTrace
	{
public:
	/**
	Byte indices into the trace header for specific fields.
	*/
	enum THeaderStructure
		{
		/**
		Size of record in bytes.
		*/
		ESizeIndex = 0,

		/**
		Bitfield of flags from enum TFlags. E.g. to detect if a timestamp is present in
		the record, code like this could be used.
		@code
			TUint8* record; // pointer to trace record
			if(record[BTrace::EFlagsIndex]&BTrace::ETimestampPresent)
				TimestampPresent();
			else
				TimestampNotPresent();
		@endcode
		*/
		EFlagsIndex = 1,

		/**
		Category value from enum BTrace::TCategory.
		*/
		ECategoryIndex = 2,

		/**
		Sub-category value. The meaning of this is dependent on the Category.
		*/
		ESubCategoryIndex = 3,
		};

	/**
	Bit flags which indicate state of a trace record.
	*/
	enum TFlags
		{
		/**
		Header2 is present in the trace record.
		*/
		EHeader2Present		= 1<<0,

		/**
		A timestamp value is present in the trace record.
		*/
		ETimestampPresent	= 1<<1,

		/**
		A second timestamp value is present in the trace record.
		*/
		ETimestamp2Present	= 1<<2,

		/**
		A context ID is present in the trace record.
		*/
		EContextIdPresent	= 1<<3,

		/**
		A CPU program counter (PC) value is present in the trace record.
		*/
		EPcPresent			= 1<<4,

		/**
		An 'extra' value is present in the trace record.
		*/
		EExtraPresent		= 1<<5,

		/**
		Indicates that the data in this trace record was truncated to keep the size
		within the maximum permissible.
		*/
		ERecordTruncated	= 1<<6,

		/**
		Indicates that trace record(s) before this one are missing.
		This can happen if the trace buffer was full when a trace output was attempted.
		*/
		EMissingRecord		= 1<<7
		};

	/**
	Bit flags present in the Flags2 value of the header extension.
	*/
	enum TFlags2
		{
		/**
		Masks out the bits for the multipart trace type. (See enum TMultiPart.)
		*/
		EMultiPartFlagMask		= 3<<0,

		/**
		Masks out the bits for the CPU ID for SMP systems (zero if present on non SMP systems)
		*/
		ECpuIdMask			= 0xfff<<20,
		};

	/**
	Values for multipart trace indicator. These values are stored in Flags2 an
	are obtained by ANDing with the value EMultipartFlagMask.

	If a 'Big' trace is generated which doesn't fit into a single trace record
	then its data is split into several separate trace records; a multipart trace.

	In multipart traces the 'extra' trace value is present in the header extension.
	(EExtraPresent is set.) This extra value contains a unique trace identifier
	which is the same is all parts of the trace.

	The structure of the data part of each trace record in a multipart trace is described
	below. In this description, the following labels are used.
	-	A is the initial 4 bytes of data; the a1 argument of BTraceBig.
	-	D is the array of bytes of additional data; the aData argument of BTraceBig.
	-	N is the size of D; the aDataSize argument of BTraceBig
	-	X is the maximum number of additional bytes which will fit into a trace record. 
		This is usually KMaxBTraceDataArray but this should not be assumed, instead
		the size and other information present in each trace record should be examined.

	For the first part of a multipart trace, the data in a trace record has the following
	structure:

	-	4 bytes containing N.
	-	4 bytes containing A.
	-	X bytes containing D[0..X-1]
    
	If the parts are numbered 0 through to 'j', then a middle part of a multipart trace
	is numbered 'i' where 0<i<j. The data in these parts has the structure:

	-	4 bytes containing N.
	-	4 bytes containing X*i. I.e. the offset within D for the data in this trace record.
	-	X bytes containing D[X*i..X*i+X-1]
	
	For the last part of a multipart trace, the data has the structure:

	-	4 bytes containing N.
	-	4 bytes containing X*j. I.e. the offset within D for the data in this trace record.
	-	N modulo X bytes containing D[X*j..N-1]. I.e. the final bytes of the trace data.
	*/
	enum TMultiPart
		{
        /**
        Added value - Non symbian
        */
        ENotMultiPart = 0,

		/**
		Indicates that this trace is the first part of a multipart trace.
		*/
		EMultiPartFirst			= 1,

		/**
		Indicates that this trace is a middle part of a multipart trace.
		I.e. it is not the first or last part.
		*/
		EMultiPartMiddle		= 2,

		/**
		Indicates that this trace is the last part of a multipart trace.
		*/
		EMultiPartLast			= 3,
		};

	/**
	Enumeration of trace categories.
	*/
	enum TCategory
		{
		/**
		Trace generated by all calls to RDebug::Printf.

		The first 4 bytes of trace data contain the thread ID, RThread::Id(), for the
		thread which caused this trace to be emitted. If the trace wasn't generated in
		thread context, this id has the value KNullThreadId.
		
		Subsequent bytes of data contain the ASCII text for the formatted string
		generated by Kern::Printf.

		These traces also contain a context ID, i.e. the EContextIdPresent flag is
		set and a context ID value is present in the extended header.

		If the trace text doesn't fit completely into one trace record, then
		a multipart trace is generated. See enum TMultiPart.
		*/
		ERDebugPrintf = 0,

		/**
		Trace generated by all calls to Kern::Printf.
		Trace records in this category have the same structure as ERDebugPrintf.
		*/
		EKernPrintf = 1,

		/**
		Trace generated by platform security diagnostic messages.
		Trace records in this category have the same structure as ERDebugPrintf.
		*/
		EPlatsecPrintf = 2,

		/**
		Trace generated for the purpose of associating thread context ids with
		the textual names of threads. These traces are usually generated when a
		thread is created, renamed or destroyed.

		If #Prime is called with this category, traces will be generated for all
		threads currently extant.

		@see enum TThreadIdentification
		*/
		EThreadIdentification = 3,

		/**
		Trace generated when the CPU usage changes state, e.g. at thread context switch
		or during interrupt and DFC processing.

		The purpose of this trace category is to profile CPU usage.

		@see enum TCpuUsage
		*/
		ECpuUsage = 4,

        /**
        Category used for profiling device drivers, kernel extensions etc.
        Used by PERF_LOG macro.
        @prototype 9.3
        */
        EKernPerfLog = 5,

        /**
		@internalTechnology
        @prototype 9.3
        */
        EClientServer = 6,

        /**
		@internalTechnology
        @prototype 9.3
        */
        ERequests = 7,

        /**
		Trace generated when chunks are created and destroyed, and when memory
		is committed and decommitted to and from chunks.
		
		If #Prime is called with this category, traces will be generated for all
		chunks currently extant.

		@see TChunks
        */
        EChunks = 8,


        /**
		Trace generated when code segments are created and destroyed, mapped
		into out of processes, and when memory is committed and decommitted to
		and from them.
		
		If #Prime is called with this category, traces will be generated for all
		code segments currently extant.

		@see TCodeSegs
        */
        ECodeSegs = 9,

		/**
		Trace generated by Demand Paging.
		@prototype 9.3
		*/
		EPaging = 10,

		/**
		Trace generated when thread and process priorities are modified, whether
		directly or through priority inheritance, aging or other mechanisms used
		by the kernel.

		The purpose of this category is to enable system-wide study of thread
		priority usage.

		If #Prime is called with this category, traces will be generated for all
		threads currently extant.

		@see enum TThreadPriority
		@internalTechnology
        @prototype 9.3
		*/
		EThreadPriority = 11,

		/**
		Trace generated by processing Paging requests in the Media subsystem and Media drivers.
		@prototype 9.3
		*/
		EPagingMedia = 12,

		/**
		Trace generated by the kernel for memory regions which don't belong to any chunk.
		@see enum TKernelMemory
		@prototype 9.4
		*/
		EKernelMemory = 13,

		/**
		Trace generated by user-mode heap usage.

		Unlike other trace categories, capturing heap trace involves an additional step 
		depending on how much trace is required. To enable heap trace for a single process 
		from the moment it starts, add the following line to the .exe's project (.mmp) file:

			firstlib eexe_instrumented_heap.lib

		This overrides the build tools default implicit link (for .exe projects) against eexe.lib.

		Alternatively, to enable heap trace for all processes at once you can enable the 
		KUSERHEAPTRACE bit (#96) of the kernel trace flags. You can set this flag either at 
		ROM-building time (look for the 'kerneltrace' line generally in \epoc32\rom\<platform>\header.iby)
		or at runtime by running the following at the Eshell command prompt:

			trace 0 0 1

		Note that changing this flag at runtime only affects processes created after the flag 
		is set or unset. It will not affect running processes.
		
		@see enum THeap
		@prototype 9.4
		*/
		EHeap = 14,

		/**
		Meta trace. Trace that is only useful to programs which use or display BTrace-based data.
		@see enum TMetaTrace
		@prototype 9.4
		*/
		EMetaTrace = 15,

		/**
		Trace generated by the ram allocator to allow the physical layout of RAM
		to be tracked.
		@internalTechnology
		*/
		ERamAllocator = 16,

		/**
		Trace generated by the Fast Mutex in the Nkern.
		*/
		EFastMutex = 17,

		/**
		First category value in the range reserved for platform specific use;
		the end of this range is #EPlatformSpecificLast.
		Symbian's code will not generate any traces with categories in this range.

		It is strongly recommended that platforms reserve the first half of this range
		(128..143) for definition and use by base-port (kernel-side) code. Any general
		trace framework built on top of BTrace APIs should use the second half of the range.
		This allows fast (primary filtered only) BTrace categories to be used in device drivers
		and other base-port code, without clashing with more general trace frameworks implemented
		for application layer code. 
		*/
		EPlatformSpecificFirst = 128,

		/**
		Last category value in the range reserved for platform specific use.
		@see EPlatformSpecificFirst
		*/
		EPlatformSpecificLast = 191,

		/**
		First category value in the range reserved for Symbian tools and future trace framework
		implementations; the end of this range is #ESymbianExtentionsLast.
		*/
		ESymbianExtentionsFirst = 192,

		/**
		Last category value in the range reserved for Symbian tools and future trace framework
		implementations.
		@see ESymbianExtentionsFirst
		*/
		ESymbianExtentionsLast = 253,

		/**
		Used for testing purposes.

		This may be used for ad-hoc testing purposes, e.g. special builds of components
		with tracing enabled for diagnostic purposes.

		This category is also used by the E32 BTrace unit tests.
		@test
		*/
		ETest1 = 254,

		/**
		Used for testing purposes.

		This may be used for ad-hoc testing purposes, e.g. special builds of components
		with tracing enabled for diagnostic purposes.

		This category is also used by the E32 BTrace unit tests.
		@test
		*/
		ETest2 = 255
		};

	/**
	Enumeration of sub-category values for trace category EThreadIdentification.
	@see EThreadIdentification
	*/
	enum TThreadIdentification
		{
		/**
		A nano-kernel thread (NThread) has been created.

		Trace data format:
		- 4 bytes containing the context id (an NThread*) for this thread.
		*/
		ENanoThreadCreate,

		/**
		A nano-kernel thread (NThread) has been destroyed.

		Trace data format:
		- 4 bytes containing the context id (an NThread*) for this thread.
		*/
		ENanoThreadDestroy,

		/**
		A thread (DThread) has been created.

		Trace data format:
		- 4 bytes containing the context id (an NThread*) for this thread.
		- 4 bytes containing trace id (a DProcess*) for the process to which this thread belongs.
		- Remaining data is the ASCII name of the thread.
		*/
		EThreadCreate,

		/**
		A thread (DThread) has been destroyed.

		Trace data format:
		- 4 bytes containing the context id (an NThread*) for this thread.
		- 4 bytes containing trace id for the process to which this thread belongs.
		- 4 bytes containing thread ID, as returned by RThread::Id().
		*/
		EThreadDestroy,

		/**
		A thread (DThread) has been renamed.
		This trace may also be output by the tracing system at initialisation
		in order to identify threads already in existence.

		Trace data format:
		- 4 bytes containing the context id (an NThread*) for this thread.
		- 4 bytes containing trace id (a DProcess*) for the process to which this thread belongs.
		- Remaining data is the ASCII name of the thread.
		*/
		EThreadName,

		/**
		A process has been renamed.
		This trace may also be output together with EThreadCreate or EThreadName traces
		to help identify the name of the process to which the thread belongs.

		Trace data format:
		- 4 bytes containing zero, or if this trace is generated together with EThreadName
		  or EThreadCreate, this contains the context id (an NThread*) for the thread.
		- 4 bytes containing trace id (a DProcess*) for process.
		- Remaining data is the ASCII name of the process.
		*/
		EProcessName,

		/**
		Informational trace giving a threads ID, as returned by RThread::Id().
		Trace data format:
		- 4 bytes containing the context id (an NThread*) for this thread.
		- 4 bytes containing trace id (a DProcess*) for the process to which this thread belongs.
		- 4 bytes containing thread ID, as returned by RThread::Id().
		*/
		EThreadId,

		/**
		A process has been created.

		Trace data format:
		- 4 bytes containing trace id (a DProcess*) for the process.
		*/
		EProcessCreate,

		/**
		A process has been destroyed.

		Trace data format:
		- 4 bytes containing trace id (a DProcess*) for the process.
		*/
		EProcessDestroy

		};

	/**
	Enumeration of sub-category values for trace category ECpuUsage.
	@see ECpuUsage
	*/
	enum TCpuUsage
		{
		/**
		Trace output at start of Interrupt (IRQ) dispatch.

		On platforms which support nested interrupts, traces for these will also
		be nested.
		*/
		EIrqStart,

		/**
		Trace output at end of Interrupt (IRQ) dispatch.

		Note, this trace isn't generated if an Interrupt Service Routine queues
		a DFC or causes a thread to be scheduled. In these cases, the traces for
		these events (EIDFCStart or ENewThreadContext) should be taken to indicate
		that interrupt servicing has ended.
		*/
		EIrqEnd,

		/**
		Trace output at start of Fast Interrupt (FIQ) dispatch.

		On platforms which support nested interrupts, traces for these will also
		be nested.
		*/
		EFiqStart,

		/**
		Trace output at end of Fast Interrupt (FIQ) dispatch.

		Note, this trace isn't generated if an Interrupt Service Routine queues
		a DFC or causes a thread to be scheduled. In these cases, the traces for
		these events (EIDFCStart or ENewThreadContext) should be taken to indicate
		that interrupt servicing has ended.
		*/
		EFiqEnd,

		/**
		Trace output at start of Immediate Delayed Function Call (IDFC) processing.
		This processing also includes moving DFCs to their final queue, so the trace
		does not necessarily indicate that any IDFCs have been executed.
		*/
		EIDFCStart,

		/**
		Trace output at end of Immediate Delayed Function Call (IDFC) processing.
		*/
		EIDFCEnd,

		/**
		Trace output when a thread is scheduled to run.
		The context id (NThread*) in this trace is that of the thread being scheduled.
		*/
		ENewThreadContext
		};

    /**
	@internalTechnology
    @prototype 9.3
    */
	enum TClientServer
		{
		/**
		Trace generated whenever a server is created.

		Trace data format:
		- 4 bytes containing the server id (a DServer*).
		- 4 bytes containing undefined data
		- Remaining data is the ASCII name of the server.

		The context id (NThread*) in this trace is that of the thread creating the server.
		*/
		EServerCreate,

		/**
		Trace generated whenever a server is destroyed.

		Trace data format:
		- 4 bytes containing the server id (a DServer*).

		*/
		EServerDestroy,

		/**
		Trace generated whenever a new session is attached to a server.
		I.e. a new session has been created.

		Trace data format:
		- 4 bytes containing the session id (a DSession*).
		- 4 bytes containing the server id (a DServer*).

		The context id (NThread*) in this trace is that of the thread creating the session.
		*/
		ESessionAttach,

		/**
		Trace generated whenever a server session is detached from a server.
		I.e. a session has been closed.

		Trace data format:
		- 4 bytes containing the session id (a DSession*).
		- 4 bytes containing the reasons (error code) for the session being closed.

		*/
		ESessionDetach,

		/**
		Trace generated whenever a new message is sent to a server.
	
		Trace data format:
		- 4 bytes containing the message handle.
		- 4 bytes containing the iFunction value for the message.
		- 4 bytes containing the server id (a DServer*).

		The context id (NThread*) in this trace is that of the thread which sent the message.
		*/
		EMessageSend,

		/**
		Trace generated when a server receives a new message.

		Trace data format:
		- 4 bytes containing the message handle.
		*/
		EMessageReceive,

		/**
		Trace generated whenever a message is completed using RMessagePtr2::Complete.

		Trace data format:
		- 4 bytes containing the message handle.
		- 4 bytes containing the completion reason, or object handle, value.
		    (The object handle value is that which is delivered to the sender of the
			message, not that supplied by the server actually completing the request.)

		The context id (NThread*) in this trace is that of the thread which completed the message.
		*/
		EMessageComplete
		};


    /**
	@internalTechnology
    @prototype 9.3
    */
	enum TRequests
		{
		/**
		Trace generated whenever a request status is completed.

		Trace data format:
		- 4 bytes containing the thread id (NThread*) of the thread being signalled.
		- 4 bytes containing the address of the TRequestStatus object.
		- 4 bytes containing the completion reason.

		The context id (NThread*) in this trace is that of the thread which completed the request.
		*/
		ERequestComplete
		};


	/**
	Enumeration of sub-category values for trace category EChunks.
	@see EChunks
	*/
	enum TChunks
		{
		/**
		Trace output when a chunk is created.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the maximum size of the chunk.
		- The ASCII name of the chunk.
		*/
		EChunkCreated,

		/**
		@internalTechnology
		
		Trace output when a chunk is created containing extra chunk information.

		Note that the meaning of the data in this trace is different between
		memory models, and may change without warning.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the chunk type.
		- 4 bytes containing the chunk's attributes.
		*/
		EChunkInfo,

		/**
		Trace output when a chunk is destroyed.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*)
		*/
		EChunkDestroyed,

		/**
		Trace output when memory is allocated and committed to a chunk.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the offset into the chunk.
		- 4 bytes containing the size of the memory committed.
		*/
		EChunkMemoryAllocated,

		/**
		Trace output when memory is decommitted from a chunk and deallocated.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the offset into the chunk.
		- 4 bytes containing the size of the memory decommitted.
		*/
		EChunkMemoryDeallocated,

		/**
		Trace output when un-owned memory is committed to a chunk.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the offset into the chunk.
		- 4 bytes containing the size of the memory committed.
		*/
		EChunkMemoryAdded,

		/**
		Trace output when un-owned memory is decommitted to a chunk.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the offset into the chunk.
		- 4 bytes containing the size of the memory decommitted.
		*/
		EChunkMemoryRemoved,

		/**
		Trace to indicate the owning process of a chunk - only for local (private) chunks.
		
		Trace data format:
		- 4 bytes containing the chunk id (a DChunk*).
		- 4 bytes containing the process id of the owner (a DProcess*).
		*/
		EChunkOwner
		};

	/**
	Enumeration of sub-category values for trace category ECodeSegs.
	@see ECodeSegs
	*/
	enum TCodeSegs
		{
		/**
		Trace output when a code segment is created to associate a code segment
		id with a filename.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		- The ASCII filename.
		*/
		ECodeSegCreated,

		/**
		Trace output when a code segment is created.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		- 4 bytes containing the attributes.
		- 4 bytes containing the code base address (.text).
		- 4 bytes containing the size of the code section (.text).
		- 4 bytes containing the base address of the constant data section (.rodata).
		- 4 bytes containing the size of the constant data section (.rodata).
		- 4 bytes containing the base address of the initialised data section (.data).
		- 4 bytes containing the size of the initialised data section (.data).
		- 4 bytes containing the base address of the uninitialised data section (.bss).
		- 4 bytes containing the size of the uninitialised data section (.bss).
		*/
		ECodeSegInfo,
		
		/**
		Trace output when a code segment is destroyed.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		*/
		ECodeSegDestroyed,

		/**
		Trace output when a code segment is mapped into a process.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		- 4 bytes containing the process id (a DProcess*).
		*/
		ECodeSegMapped,

		/**
		Trace output when a code segment is unmapped from a process.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		- 4 bytes containing the process id (a DProcess*).
		*/
		ECodeSegUnmapped,
		
		/**
		Trace output when memory is allocated to a code segment.

		Under the multiple memory model, code segments for RAM-loaded user code
		own the RAM pages the code occupies.  The pages are not owned by any
		chunk, but are mapped into the code chunk of each process that uses the
		code segment.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		- 4 bytes containing the size of the memory allocated.
		*/
		ECodeSegMemoryAllocated,
		
		/**
		Trace output when memory is deallocated from a code segment.

		Under the multiple memory model, code segments for RAM-loaded user code
		own the RAM pages the code occupies.  The pages are not owned by any
		chunk, but are mapped into the code chunk of each process that uses the
		code segment.
		
		Trace data format:
		- 4 bytes containing the code segment id (a DCodeSeg*).
		- 4 bytes containing the size of the memory deallocated.
		*/
		ECodeSegMemoryDeallocated
		};

	
	/**
	Enumeration of sub-category values for trace category EPaging.
	@see EPaging
	*/
	enum TPaging
		{
		/**
		This event indicates the beginning of the 'page in' activity.
		The end of this activity is indicated by one of the following events:
		- EPagingPageInUnneeded
		- EPagingPageInROM
		- EPagingPageInCode
		
		Trace data format:
		- 4 bytes containing the virtual address which was accessed, causing this paging event.
		- 4 bytes containing the virtual address of the instruction which caused this paging event.
		  (The PC value.)

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageInBegin,

		/**
		Event which terminates the 'page in' activity when the required page was found to
		already be paged in. (See EPagingPageInBegin.)

		Trace data format:
		- 0 bytes. (No extra data.)

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageInUnneeded,

		/**
		A ROM page has been paged in.
		This event indicates the end of the 'page in' activity. (See EPagingPageInBegin.)

		Trace data format:
		- 4 bytes containing the physical address of the page 'paged in'.
		- 4 bytes containing the virtual address of the page 'paged in'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageInROM,

		/**
		A ROM page has been 'paged out'. I.e. removed from the live list to be either
		reused or returned to free pool.

		Trace data format:
		- 4 bytes containing the physical address of the page being 'paged out'.
		- 4 bytes containing the virtual address of the page being 'paged out'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageOutROM,

		/**
		A Free page has been 'paged in'. I.e. added to the live list.

		Trace data format:
		- 4 bytes containing the physical address of the page being 'paged in'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageInFree,

		/**
		A Free page has been 'paged out'. I.e. removed from the live list to be either
		reused or returned to free pool.

		Trace data format:
		- 4 bytes containing the physical address of the page being 'paged out'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageOutFree,

		/**
		A page has been made 'young' again because it was accessed.

		Trace data format:
		- 4 bytes containing the physical address of the page being rejuvenated, (made young).
		- 4 bytes containing the virtual address which was accessed, causing this paging event.
		- 4 bytes containing the virtual address of the instruction which caused this paging event.
		  (The PC value.)

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingRejuvenate,

		/**
		A page fault was found to have already been previously serviced.

		Trace data format:
		- 4 bytes containing the physical address of the page accessed.
		- 4 bytes containing the virtual address which was accessed, causing this paging event.
		- 4 bytes containing the virtual address of the instruction which caused this paging event.
		  (The PC value.)

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageNop,

		/**
		A page has been locked.

		Trace data format:
		- 4 bytes containing the physical address of the page being locked.
		- 4 bytes containing the value of the lock count after the paged was locked.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageLock,

		/**
		A page has been unlocked.

		Trace data format:
		- 4 bytes containing the physical address of the page being unlocked.
		- 4 bytes containing the value of the lock count before the paged was unlocked.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageUnlock,
		
		/**
		A page containing RAM cache has been 'paged out'. I.e. removed from the live list to be
		either reused or returned to free pool.

		Trace data format:
		- 4 bytes containing the physical address of the page being 'paged out'.
		- 4 bytes containing the virtual address of the page being 'paged out'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageOutCache,
		
		/**
		A page containing RAM-loaded code has been paged in.
		This event indicates the end of the 'page in' activity. (See EPagingPageInBegin.)

		Trace data format:
		- 4 bytes containing the physical address of the page 'paged in'.
		- 4 bytes containing the virtual address of the page 'paged in'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageInCode,

		/**
		A page containing RAM-loaded code has been 'paged out'. I.e. removed from the live list to be
		either reused or returned to free pool.

		Trace data format:
		- 4 bytes containing the physical address of the page being 'paged out'.
		- 4 bytes containing the virtual address of the page being 'paged out'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingPageOutCode,
		
		/**
		A page of RAM-loaded code was found to already be 'paged in' but not mapped in
		the faulting process.
		This event is only emitted on the multiple memory model.

		Trace data format:
		- 4 bytes containing the physical address of the page 'paged in'.
		- 4 bytes containing the virtual address of the page 'paged in'.

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingMapCode,

		/**
		A page has been made 'old' because it was the last young page to be accessed.

		This trace is only produced when the kernel is compiled with the #BTRACE_PAGING_VERBOSE
		macro is defined defined.

		Trace data format:
		- 4 bytes containing the physical address of the page being aged, (made old).

		The context id (NThread*) in this trace is that of the thread caused this paging event.
		*/
		EPagingAged,
		};

	/**
	Enumeration of sub-category values for trace category EThreadPriority.
	@see EThreadPriority
	@internalTechnology
    @prototype 9.3
	*/
	enum TThreadPriority
		{
		/**
		Trace output when a nanothread priority is changed.
		
		Trace data format:
		- 4 bytes containing the context id (an NThread*) for the thread whose priority is changing.
		- 4 bytes containing the new absolute priority.
		*/
		ENThreadPriority=0,

		/**
		Trace output when a DThread's default priority is set.

		Trace data format:
		- 4 bytes containing the context id (an NThread*) for the thread whose priority is changing.
		- 4 bytes containing the iThreadPriority member - a value from enum ::TThrdPriority.
		- 4 bytes containing the new default absolute priority.
		*/
		EDThreadPriority=1,

		/**
		Trace output when a DProcess priority is changed.
		
		Trace data format:
		- 4 bytes containing trace id (a DProcess*) for process.
		- 4 bytes containing the new process priority, a value from enum ::TProcPriority
		*/
		EProcessPriority=2
		};

	/**
	Enumeration of sub-category values for trace category EPagingMedia.
	@see EPagingMedia
	*/
	enum TPagingMedia
		{
		/**
		Event generated when a request to 'page in' data is received by the Local Media Subsystem.

		Trace data format:
		- 4 bytes containing the linear address of the buffer to where the data is to be written.
		- 4 bytes containing the offset from start of the partition to where the data to be paged in resides.
		- 4 bytes containing the number of bytes to be read off the media.
		- 4 bytes containing local drive number for the drive where the data to be paged in resides (-1 if ROM paging). 
		- 4 bytes containing the linear address in memory where this request object resides.

		The context id (NThread*) in this trace is that of the thread that took the page fault that caused this event.
		*/
		EPagingMediaLocMedPageInBegin,

		/**
		Event generated by the Local Media Subsystem when a request to 'page in' data is completed.

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing the completion code to be returned.
		- 4 bytes containing a code qualifying this request as either a ROM or Code 'page in' (see TPagingRequestId).

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaLocMedPageInPagedIn,
		
		/**
		Event generated by the Local Media Subsystem when a request to 'page in' data is deferred.

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing a code qualifying this request as either a ROM or Code 'page in' (see TPagingRequestId).

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaLocMedPageInDeferred,
		
		/**
		Event generated by the Local Media Subsystem when a request to 'page in' data that has been deferred is re-posted.

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing a code qualifying this request as either a ROM or Code 'page in' (see TPagingRequestId).

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaLocMedPageInDeferredReposted,
		
		/**
		Event generated by the Local Media Subsystem when a request to 'page in' data is re-deferred.

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing a code qualifying this request as either a ROM or Code 'page in' (see TPagingRequestId).

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaLocMedPageInReDeferred,

		/**
		Event generated by the Local Media Subsystem when a request to 'page in' data is issued when the media is not yet open.

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing the state of the media (one of TMediaState).
		- 4 bytes containing a code qualifying this request as either a ROM or Code 'page in' (see TPagingRequestId).

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaLocMedPageInQuietlyDeferred,
		
		/**
		Event generated by the Local Media Subsystem when a fragment of a Write request is created and is ready to be sent to the Media 
		Driver thread .

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing the ID of this fragment (middle or last).
		- 4 bytes containing the length of data in this request fragment.
		- 4 bytes containing the offset within the original request to the start of data in this fragment.
		- 4 bytes containing the offset from start of the partition to where the data in this fragment is to be written.
		- 4 bytes containing the address of the DThread object representing the thread that issued the original Write request.

		The context id (NThread*) in this trace is that of the File Server drive thread that issued the original Write request.
		*/
		EPagingMediaLocMedFragmentBegin,
		
		/**
		Event generated by the Local Media Subsystem when a Write fragment is completed .

		Trace data format:
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing the completion code to be returned.

		The context id (NThread*) in this trace is that of the File Server drive thread that issued the original Write request.
		*/
		EPagingMediaLocMedFragmentEnd,
		
		/**
		Event generated when the Media Driver starts processing a request to 'page in' data in its specific Request(..) function.

		Trace data format:
		- 4 bytes containing a code describing the type of the media (one of TMediaDevice).
		- 4 bytes containing the linear address in memory where this request object resides.

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaPagingMedDrvBegin,

		/**
		Event generated by the Media Driver when the data read by a 'page in' request is written to the paging buffer.

		Trace data format:
		- 4 bytes containing a code describing the type of the media (one of TMediaDevice).
		- 4 bytes containing the linear address in memory where this request object resides.
		- 4 bytes containing the linear address of the buffer to where the data is to be written.
		- 4 bytes containing the offset within the buffer to where the data will be written.
		- 4 bytes containing the length of data to be written.

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaMedDrvWriteBack,
		
		/**
		Event generated when a request to 'page in' data is put on hold because the Media Driver is performing some background
		operation (not another request) and cannot service the request.

		Trace data format:
		- 4 bytes containing a code describing the type of the media (one of TMediaDevice).
		- 4 bytes containing the linear address in memory where this request object resides.

		The context id (NThread*) in this trace is that of the media driver thread that services this 'page in' request.
		*/
		EPagingMediaMedDrvOnHold,
		};

	/**
	Enumeration of sub-category values for trace category EKernelMemory.
	@see EKernelMemory
	*/
	enum TKernelMemory 
		{
		/**
		Event recorded during startup and prime which details the initial amount of free RAM.

		Trace data format:
		- 4 bytes containing the number of bytes of RAM the system started with.
		*/
		EKernelMemoryInitialFree,

		/**
		Event recorded during prime which records the then-current amount of free RAM.

		Trace data format:
		- 4 bytes containing the number of bytes of free RAM.
		*/
		EKernelMemoryCurrentFree,

		/**
		Event recorded when a miscellaneous kernel allocation is made. These include:
		- Memory for MMU page table contents
		- Memory for MMU SPageTableInfos
		- Memory for shadow pages

		Trace data format:
		- 4 bytes containing the size, in bytes, of the allocation.
		*/
		EKernelMemoryMiscAlloc,

		/**
		Event recorded when a miscellaneous kernel allocation (see EKernelMemoryMiscAlloc
		above) is freed. 

		Trace data format:
		- 4 bytes containing the size, in bytes, of the freed allocation.
		*/
		EKernelMemoryMiscFree,

		/**
		The amount of memory reserved for the minimum demand paging cache. The *actual* DP cache
		also uses free memory, only this minimum amount is permanently reserved for that purpose.
		This event is recorded during prime and when the amount changes.

		Trace data format:
		- 4 bytes containing the minimum size, in bytes, of the demand paging cache.
		*/
		EKernelMemoryDemandPagingCache,

		/**
		Physically contiguous memory allocated by device drivers via one of:
			Epoc::AllocPhysicalRam()
			Epoc::ZoneAllocPhysicalRam()
			Epoc::ClaimPhysicalRam()
			TRamDefragRequest::ClaimRamZone()

		Trace data format:
		- 4 bytes containing the size of the allocated memory.
		- 4 bytes containing the physical base address of allocated memory.

		NB: The prime function logs a EKernelMemoryDrvPhysAlloc record where the physical 
		address is -1 and should be ignored.
		*/
		EKernelMemoryDrvPhysAlloc,

		/**
		Memory freed by device drivers via calls to all versions of 
		Epoc::FreePhysicalRam(). 

		Trace data format:
		- 4 bytes containing the size of the freed memory.
		- 4 bytes containing the physical base address of freed memory.
		*/
		EKernelMemoryDrvPhysFree,
		};

	/**
	Enumeration of sub-category values for trace category EHeap.
	@see EHeap
	*/
	enum THeap
		{
		/**
		Event recorded during process startup which logs the point of heap creation.

		Trace data format:
		- 4 bytes containing the heap ID (The RAllocator*)
		- 2 bytes containing the size of header overhead, per allocation (0xFFFF indicates a variable size)
		- 2 bytes containing the size of footer overhead, per allocation (0xFFFF indicates a variable size)
		*/
		EHeapCreate,

		/**
		Event recorded during process startup which details the chunk being used as a heap.

		Trace data format:
		- 4 bytes containing the heap ID (The RAllocator*)
		- 4 bytes containing the chunk ID (The RHandleBase* of the chunk)
		*/
		EHeapChunkCreate,
		
		/**
		Event recorded when RHeap::Alloc() is called.

		Trace data format:
		- 4 bytes containing the heap ID (The RAllocator*)
		- 4 bytes containing the address of the allocation
		- 4 bytes containing the size of the allocation
		- 4 bytes containing the requested size of allocation
		*/
		EHeapAlloc,

		/**
		Event recorded when RHeap::ReAlloc() is called.

		Trace data format:
		- 4 bytes containing the heap ID (The RAllocator*)
		- 4 bytes containing the address of the new allocation
		- 4 bytes containing the size of the allocation
		- 4 bytes containing the requested size of allocation
		- 4 bytes containing the address of the old allocation
		*/
		EHeapReAlloc,

		/**
		Event recorded when RHeap::Free() is called.

		Trace data format:
		- 4 bytes containing the heap ID (The RAllocator*)
		- 4 bytes containing the address of the free'd allocation
		*/
		EHeapFree,		
		};

	/**
	Enumeration of sub-category values for trace category EMetaTrace.
	@see EMetaTrace
	*/
	enum TMetaTrace
		{
		/**
		Information about timestamps used for tracing.

		Trace data format:
		- 4 bytes containing the period of the Timestamp value.
		- 4 bytes containing the period of the Timestamp2 value.
		- 4 bytes containing a set of bit flags with the following meaning:
			- Bit 0, if true, indicates that Timestamp and Timestamp2 are two halves
			  of a single 64bit timestamp value; Timestamp2 is the most significant part.
			- All other bits are presently undefined.

		The format of the timestamp period data is a period in seconds given using an exponent and mantissa 
		format, where the most significant 8 bits are the signed power-of-two value for the exponent, and 
		the least significant 24 bits are the integer value of the mantissa. The binary point is to the right 
		of the least significant mantissa bit, and the mantissa may not be in normalised form.

		Example code for decoding the period:
		@code
			TInt32 period; // value from trace record
			int exponent = (signed char)(period>>24);
			int mantissa = period&0xffffff;
			double periodInSeconds = (double)mantissa*pow(2,exponent);
		@endcode
		*/
		EMetaTraceTimestampsInfo,

		/**
		Trace indicating the start of a test case being measured.

		Trace data format:
		- 4 bytes containing measurement specific value.
		- 4 bytes containing a further measurement specific value.
		- Remaining data is ASCII text providing human readable information.
		*/
		EMetaTraceMeasurementStart,

		/**
		Trace indicating the end of a test case being measured.

		Trace data format:
		- 4 bytes containing measurement specific identifying value.
		- 4 bytes containing a further measurement specific identifying value.

		The values contained in this trace must be identical to those in the corresponding
		ETraceInfoMeasurementStart trace.
		*/
		EMetaTraceMeasurementEnd,

		/**
		Trace indicating a change in state of the primary filter.

		Trace data format:
		- 1 byte containing a trace category number.
		- 1 byte containing the new filter state for the category. (0=off, 1=on).
		- 2 byte padding. (Should be output as zeros.)
		*/
		EMetaTraceFilterChange
		};

	/**
	Enumeration of sub-category values for trace category ERamAllocator.
	@see BTrace::ERamAllocator
	*/
	enum TRamAllocator
		{
		/**
		The number of RAM zones.

		Trace data format:
		- 4 bytes containing the number of RAM zones.
		*/
		ERamAllocZoneCount,
		
		/**
		Information on the layout of a RAM zone.

		Trace data format:
		- 4 bytes containing the number of pages in the zone
		- 4 bytes containing the physical base address of the zone
		- 4 bytes containing the ID of the zone
		- 1 bytes containing the preference of the zone
		- 1 bytes containing the flags of the zone
		- 2 bytes reserved for future use
		*/
		ERamAllocZoneConfig,

		/**
		This trace is sent for every contiguous block of RAM that was allocated 
		during the kernel boot process.

		Trace data format:
		- 4 bytes containing the number of contiguous pages allocated for the block
		- 4 bytes containing the physical base address of the block
		*/
		ERamAllocBootAllocation,


		/** 
		This trace marks the end of the boot allocations

		Trace data format:
		- no extra bytes are sent
		*/
		ERamAllocBootAllocationEnd,

		/**
		Event generated when a RAM zone's flags have been modified
		This could occur when a RAM zone is blocked/unblocked from further 
		allocations from all/certain page types.

		Trace data format:
		- 4 bytes containing the ID of the zone
		- 4 bytes containing the flags of the zone
		*/
		ERamAllocZoneFlagsModified,

		/**
		Event generated when DRamAllocator::ClaimPhysicalRam has successfully
		claimed the specified RAM pages.

		Trace data format:
		- 4 bytes containing the number of contiguous pages
		- 4 bytes containing the base address of the pages
		*/
		ERamAllocClaimRam,

		/**
		Event generated when DRamAllocator::MarkPageAllocated has successfully
		marked the specified page as allocated.

		Trace data format:
		- 4 bytes containing the TZonePageType type of the page
		- 4 bytes containing the address of the page
		*/
		ERamAllocMarkAllocated,

		/**
		Event generated when DRamAllocator::AllocContiguousRam successfully
		allocates the requested number of pages.

		Trace data format:
		- 4 bytes containing the TZonePageType type of the pages
		- 4 bytes containing the number of contiguous pages
		- 4 bytes containing the base address of the pages
		*/
		ERamAllocContiguousRam,

		/**
		Event generated when DRamAllocator::FreePage has successfully freed 
		the specified RAM page.

		Trace data format:
		- 4 bytes containing the TZonePageType type of the page
		- 4 bytes containing the address of the page
		*/
		ERamAllocFreePage,

		/**
		Event generated when DRamAllocator::FreePhysical successfully freed 
		the specified RAM page(s).

		Trace data format:
		- 4 bytes containing the number of contiguous pages
		- 4 bytes containing the base address of the pages
		*/
		ERamAllocFreePhysical,

		/**
		Event generated for each contiguous block of pages when 
		DRamAllocator::AllocRamPages or DRamAllocator::ZoneAllocRamPages
		are attempting to fulfil a request.

		Trace data format:
		- 4 bytes containing the TZonePageType type of the pages
		- 4 bytes containing the number of contiguous pages
		- 4 bytes containing the base address of the pages
		*/
		ERamAllocRamPages,

		/**
		Event generated for contiguous block of pages when 
		DRamAllocator::FreeRamPages is invoked.

		Trace data format:
		- 4 bytes containing the TZonePageType type of the pages
		- 4 bytes containing the number of contiguous pages
		- 4 bytes containing the base address of the pages
		*/
		ERamAllocFreePages,

		/**
		Event generated when DRamAllocator::AllocRamPages has successfully 
		allocated all the requested number of RAM pages.  If DRamAllocator::AllocRamPages
		couldn't allocate all the requested pages then this event is not generated.

		Trace data format:
		- no extra bytes sent
		*/
		ERamAllocRamPagesEnd,

		/**
		Event generated when all ERamAllocFreePages events for a particular 
		call of DRamAllocator::FreeRamPages have been generated.

		Trace data format:
		- no extra bytes sent
		*/
		ERamAllocFreePagesEnd,
		
		/**
		Event generated when DRamAllocator::ChangePageType is has successfully 
		updated the type of the specified page.
	
		Trace data format:
		- 4 bytes containing the new TZonePageType type of the page
		- 4 bytes containing the physical address of the page
		*/
		ERamAllocChangePageType,

		/**
		Event generated when DRamAllocator::ZoneAllocContiguousRam has
		successfully allocated the required number of pages.

		Trace data format:
		- 4 bytes containing the TZonePageType type of the pages
		- 4 bytes containing the number of contiguous pages
		- 4 bytes containing the base address of the pages
		*/
		ERamAllocZoneContiguousRam,

		/**
		Event generated when DRamAllocator::ZoneAllocRamPages has successfully
		allocated all the requested RAM pages.  If DRamAllocator::ZoneAllocRamPages
		couldn't allocate all the requested pages then this event is not generated.

		Trace data format:
		- no extra bytes sent
		*/
		ERamAllocZoneRamPagesEnd,
		
		/**
		Event generated when Epoc::ClaimRamZone has successfully claimed 
		the requested zone.
		
		Trace data format:
		- 4 bytes containing the ID of the zone that has been claimed.
		*/
		ERamAllocClaimZone,
		};

	enum TFastMutex
		{
		/**
		Event generated when a thread acquires a fast mutex, (waits on it).

		Trace data format:
		- 4 bytes containing the fast mutex id, (an NFastMutex*).
		*/
		EFastMutexWait,
		
		/**
		Event generated when a thread releases a fast mutex, (signals it).

		Trace data format:
		- 4 bytes containing the fast mutex id, (an NFastMutex*).
		*/
		EFastMutexSignal,

		/**
		Event generated when a fast mutex is 'flashed' (signalled then immediately
		waited on again). E.g the operation performed by NKern::FlashSystem.

		Trace data format:
		- 4 bytes containing the fast mutex id, (an NFastMutex*).
		*/
		EFastMutexFlash,
		
		/**
		Trace to associate a fast mutex with a textual name.

		Trace data format:
		- 4 bytes containing the fast mutex id, (an NFastMutex*).
		- 4 bytes containing unspecified data (should be output as zero).
		- Remaining data is the ASCII name for the fast mutex.
		*/
		EFastMutexName,
		};



    
    };


/**
The maximum permissible value for aDataSize in trace outputs.
@see BTraceN BTracePcN BTraceContextN BTraceContextPcN
@publishedPartner
@released
*/
const int KMaxBTraceDataArray = 80;

/**
The maximum total number of bytes in a trace record.
@publishedPartner
@released
*/
// 4 bytes for the header, plus 6 optional 4 byte fields
// 8 bytes for something?? need to read through the stuff above again
const int KMaxBTraceRecordSize = 7*4+8+KMaxBTraceDataArray;


namespace UTrace
{

// The schema is binary, or raw data
const int KBinary = 0;


// The schema is ASCII text
const int KText = 1;

} // namespace UTrace

#endif

