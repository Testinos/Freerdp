/**
 * WinPR: Windows Portable Runtime
 * Collections
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WINPR_COLLECTIONS_H
#define WINPR_COLLECTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winpr/winpr.h>
#include <winpr/wtypes.h>

#include <winpr/synch.h>

typedef void (*OBJECT_EQUALS_FN)(void* objA, void* objB);
typedef void (*OBJECT_FREE_FN)(void* obj);

struct _wObject
{
	OBJECT_EQUALS_FN fnObjectEquals;
	OBJECT_FREE_FN fnObjectFree;
};
typedef struct _wObject wObject;

/* System.Collections.Queue */

struct _wQueue
{
	int capacity;
	int growthFactor;
	BOOL synchronized;

	int head;
	int tail;
	int size;
	void** array;
	HANDLE mutex;
	HANDLE event;

	wObject object;
};
typedef struct _wQueue wQueue;

WINPR_API int Queue_Count(wQueue* queue);
WINPR_API BOOL Queue_IsSynchronized(wQueue* queue);
WINPR_API HANDLE Queue_SyncRoot(wQueue* queue);
WINPR_API HANDLE Queue_Event(wQueue* queue);

#define Queue_Object(_queue)	(&_queue->object)

WINPR_API void Queue_Clear(wQueue* queue);

WINPR_API BOOL Queue_Contains(wQueue* queue, void* obj);

WINPR_API void Queue_Enqueue(wQueue* queue, void* obj);
WINPR_API void* Queue_Dequeue(wQueue* queue);

WINPR_API void* Queue_Peek(wQueue* queue);

WINPR_API wQueue* Queue_New(BOOL synchronized, int capacity, int growthFactor);
WINPR_API void Queue_Free(wQueue* queue);

/* System.Collections.Stack */

struct _wStack
{
	BOOL synchronized;
	wObject object;
};
typedef struct _wStack wStack;

WINPR_API int Stack_Count(wStack* stack);
WINPR_API BOOL Stack_IsSynchronized(wStack* stack);

#define Stack_Object(_stack)	(&_stack->object)

WINPR_API void Stack_Clear(wStack* stack);
WINPR_API BOOL Stack_Contains(wStack* stack, void* obj);

WINPR_API void Stack_Push(wStack* stack, void* obj);
WINPR_API void* Stack_Pop(wStack* stack);

WINPR_API void* Stack_Peek(wStack* stack);

WINPR_API wStack* Stack_New(BOOL synchronized);
WINPR_API void Stack_Free(wStack* stack);

/* System.Collections.ArrayList */

struct _wArrayList
{
	int capacity;
	int growthFactor;
	BOOL synchronized;

	int size;
	void** array;
	HANDLE mutex;

	wObject object;
};
typedef struct _wArrayList wArrayList;

WINPR_API int ArrayList_Capacity(wArrayList* arrayList);
WINPR_API int ArrayList_Count(wArrayList* arrayList);
WINPR_API BOOL ArrayList_IsFixedSized(wArrayList* arrayList);
WINPR_API BOOL ArrayList_IsReadOnly(wArrayList* arrayList);
WINPR_API BOOL ArrayList_IsSynchronized(wArrayList* arrayList);

WINPR_API BOOL ArrayList_Lock(wArrayList* arrayList);
WINPR_API BOOL ArrayList_Unlock(wArrayList* arrayList);

WINPR_API void* ArrayList_GetItem(wArrayList* arrayList, int index);
WINPR_API void ArrayList_SetItem(wArrayList* arrayList, int index, void* obj);

#define ArrayList_Object(_arrayList)	(&_arrayList->object)

WINPR_API void ArrayList_Clear(wArrayList* arrayList);
WINPR_API BOOL ArrayList_Contains(wArrayList* arrayList, void* obj);

WINPR_API int ArrayList_Add(wArrayList* arrayList, void* obj);
WINPR_API void ArrayList_Insert(wArrayList* arrayList, int index, void* obj);

WINPR_API void ArrayList_Remove(wArrayList* arrayList, void* obj);
WINPR_API void ArrayList_RemoveAt(wArrayList* arrayList, int index);

WINPR_API int ArrayList_IndexOf(wArrayList* arrayList, void* obj, int startIndex, int count);
WINPR_API int ArrayList_LastIndexOf(wArrayList* arrayList, void* obj, int startIndex, int count);

WINPR_API wArrayList* ArrayList_New(BOOL synchronized);
WINPR_API void ArrayList_Free(wArrayList* arrayList);

/* System.Collections.DictionaryBase */

struct _wDictionary
{
	BOOL synchronized;
	HANDLE mutex;
};
typedef struct _wDictionary wDictionary;

/* System.Collections.Specialized.ListDictionary */

struct _wListDictionary
{
	BOOL synchronized;
	HANDLE mutex;
};
typedef struct _wListDictionary wListDictionary;

/* System.Collections.Generic.KeyValuePair<TKey,TValue> */

struct _wKeyValuePair
{
	void* key;
	void* value;
};
typedef struct _wKeyValuePair wKeyValuePair;

/* Reference Table */

struct _wReference
{
	UINT32 Count;
	void* Pointer;
};
typedef struct _wReference wReference;

typedef int (*REFERENCE_FREE)(void* context, void* ptr);

struct _wReferenceTable
{
	UINT32 size;
	HANDLE mutex;
	void* context;
	BOOL synchronized;
	wReference* array;
	REFERENCE_FREE ReferenceFree;
};
typedef struct _wReferenceTable wReferenceTable;

WINPR_API UINT32 ReferenceTable_Add(wReferenceTable* referenceTable, void* ptr);
WINPR_API UINT32 ReferenceTable_Release(wReferenceTable* referenceTable, void* ptr);

WINPR_API wReferenceTable* ReferenceTable_New(BOOL synchronized, void* context, REFERENCE_FREE ReferenceFree);
WINPR_API void ReferenceTable_Free(wReferenceTable* referenceTable);

/* Countdown Event */

struct _wCountdownEvent
{
	DWORD count;
	HANDLE mutex;
	HANDLE event;
	DWORD initialCount;
};
typedef struct _wCountdownEvent wCountdownEvent;

WINPR_API DWORD CountdownEvent_CurrentCount(wCountdownEvent* countdown);
WINPR_API DWORD CountdownEvent_InitialCount(wCountdownEvent* countdown);
WINPR_API BOOL CountdownEvent_IsSet(wCountdownEvent* countdown);
WINPR_API HANDLE CountdownEvent_WaitHandle(wCountdownEvent* countdown);

WINPR_API void CountdownEvent_AddCount(wCountdownEvent* countdown, DWORD signalCount);
WINPR_API BOOL CountdownEvent_Signal(wCountdownEvent* countdown, DWORD signalCount);
WINPR_API void CountdownEvent_Reset(wCountdownEvent* countdown, DWORD count);

WINPR_API wCountdownEvent* CountdownEvent_New(DWORD initialCount);
WINPR_API void CountdownEvent_Free(wCountdownEvent* countdown);

#endif /* WINPR_COLLECTIONS_H */
