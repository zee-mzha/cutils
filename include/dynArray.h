#ifndef CUTILS_DYNARRAY_H
#define CUTILS_DYNARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

#include "errors.h"

#define CUTILS_DEF_DYNARRAY_H(TYPE, NAME)\
	struct NAME;\
	typedef void(*NAME##RemoveFn)(TYPE*, size_t, void*);\
\
	typedef struct NAME{\
		TYPE *data;\
		size_t size;\
		size_t capacity;\
\
		int lastError;\
\
		void *usrptr;\
		NAME##RemoveFn callback;\
	} NAME;\
\
	int NAME##Init(NAME *arr, size_t size);\
	int NAME##Copy(NAME *dst, NAME *src);\
	void NAME##Move(NAME *dst, NAME *src);\
	void NAME##Swap(NAME *a, NAME *b);\
	void NAME##Deinit(NAME *arr);\
\
	NAME* NAME##New(size_t size);\
	void NAME##Free(NAME *arr);\
\
	void NAME##SetUsrptr(NAME *arr, void *data);\
	void NAME##SetFreeCallback(NAME *arr, NAME##RemoveFn callback);\
\
	int NAME##Resize(NAME *arr, size_t size);\
	int NAME##Reserve(NAME *arr, size_t capacity);\
\
	int NAME##PushBack(NAME *arr, TYPE x);\
	int NAME##PushBackPtr(NAME *arr, TYPE *x, size_t len);\
	int NAME##PushBackArr(NAME *arr, NAME *x);\
\
	int NAME##Insert(NAME *arr, TYPE x, size_t index);\
	int NAME##InsertPtr(NAME *arr, TYPE *x, size_t len, size_t index);\
	int NAME##InsertArr(NAME *arr, NAME *x, size_t index);\
\
	int NAME##Delete(NAME *arr, size_t index);\
	int NAME##DeleteRange(NAME *arr, size_t start, size_t end);

#define CUTILS_DEF_DYNARRAY_C(TYPE, NAME, DEFAULT_CALLBACK)\
	int NAME##Init(NAME *arr, size_t size){\
		arr->data = malloc(sizeof(TYPE)*size);\
		if(arr->data == NULL){\
			arr->lastError = CUTILS_NOMEM;\
			return CUTILS_NOMEM;\
		}\
\
		arr->capacity = size;\
		arr->size = 0;\
\
		arr->usrptr = NULL;\
		arr->callback = DEFAULT_CALLBACK;\
\
		arr->lastError = CUTILS_OK;\
		return CUTILS_OK;\
	}\
\
	int NAME##Copy(NAME *dst, NAME *src){\
		/*resize will handle settting the appropriate lastError so we dont touch it*/\
		int err = NAME##Resize(dst, src->size);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		memcpy(dst->data, src->data, sizeof(TYPE)*dst->size);\
\
		return CUTILS_OK;\
	}\
\
	void NAME##Move(NAME *dst, NAME *src){\
		*dst = *src;\
		memset(src, 0, sizeof(NAME));\
	}\
\
	void NAME##Swap(NAME *a, NAME *b){\
		NAME tmp = *a;\
		*a = *b;\
		*b = tmp;\
	}\
\
	void NAME##Deinit(NAME *arr){\
		if(arr->callback != NULL){\
			arr->callback(arr->data, arr->size, arr->usrptr);\
		}\
		free(arr->data);\
	}\
\
	NAME* NAME##New(size_t size){\
		NAME *ret = malloc(sizeof(NAME));\
		if(ret == NULL){\
			return NULL;\
		}\
\
		if(NAME##Init(ret, size) != CUTILS_OK){\
			free(ret);\
			return NULL;\
		}\
\
		return ret;\
	}\
\
	void NAME##Free(NAME *arr){\
		NAME##Deinit(arr);\
		free(arr);\
	}\
\
	void NAME##SetUsrptr(NAME *arr, void *data){\
		arr->usrptr = data;\
	}\
\
	void NAME##SetFreeCallback(NAME *arr, NAME##RemoveFn callback){\
		arr->callback = callback;\
	}\
\
	int NAME##Resize(NAME *arr, size_t size){\
		if(size < arr->size && arr->callback != NULL){\
			arr->callback(arr->data+size, arr->size-size, arr->usrptr);\
		}\
\
		if(size <= arr->capacity){\
			arr->size = size;\
			arr->lastError = CUTILS_OK;\
			return CUTILS_OK;\
		}\
\
		TYPE *tmp = realloc(arr->data, sizeof(TYPE)*size);\
		if(tmp == NULL){\
			arr->lastError = CUTILS_NOMEM;\
			return CUTILS_NOMEM;\
		}\
\
		arr->data = tmp;\
		arr->size = size;\
		arr->capacity = size;\
\
		arr->lastError = CUTILS_OK;\
		return CUTILS_OK;\
	}\
\
	int NAME##Reserve(NAME *arr, size_t capacity){\
		if(capacity < arr->size && arr->callback != NULL){\
			arr->callback(arr->data+capacity, arr->size-capacity, arr->usrptr);\
		}\
\
		TYPE *tmp = realloc(arr->data, sizeof(TYPE)*capacity);\
		if(tmp == NULL){\
			if(capacity < arr->size){\
				/*failed to actually free up memory, perform fake resize instead*/\
				arr->size = capacity;\
\
				arr->lastError = CUTILS_REALLOC_SHRINK;\
				return CUTILS_REALLOC_SHRINK;\
			}\
			else{\
				arr->lastError = CUTILS_NOMEM;\
				return CUTILS_NOMEM;\
			}\
		}\
\
		arr->data = tmp;\
		arr->capacity = capacity;\
		if(capacity < arr->size){\
			arr->size = capacity;\
		}\
\
		arr->lastError = CUTILS_OK;\
		return CUTILS_OK;\
	}\
\
	int NAME##PushBack(NAME *arr, TYPE x){\
		int err = NAME##Resize(arr, arr->size+1);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		arr->data[arr->size-1] = x;\
\
		return CUTILS_OK;\
	}\
\
	int NAME##PushBackPtr(NAME *arr, TYPE *x, size_t len){\
		size_t oldSize = arr->size;\
\
		int err = NAME##Resize(arr, arr->size+len);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		memcpy(arr->data+oldSize, x, len*sizeof(TYPE));\
\
		return CUTILS_OK;\
	}\
\
	int NAME##PushBackArr(NAME *arr, NAME *x){\
		size_t oldSize = arr->size;\
\
		int err = NAME##Resize(arr, arr->size+x->size);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		memcpy(arr->data+oldSize, x->data, x->size*sizeof(TYPE));\
\
		return CUTILS_OK;\
	}\
\
	/*insert functions can act the same as PushBack if index == arr->size*/\\
	int NAME##Insert(NAME *arr, TYPE x, size_t index){\
		if(index > arr->size){\
			arr->lastError = CUTILS_OUT_OF_BOUNDS;\
			return CUTILS_OUT_OF_BOUNDS;\
		}\
\
		size_t oldSize = arr->size;\
\
		int err = NAME##Resize(arr, arr->size+1);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		if(index != oldSize){\
			memmove(arr->data+index+1, arr->data+index, sizeof(TYPE)*(oldSize-index));\
		}\
		arr->data[index] = x;\
\
		return CUTILS_OK;\
	}\
	int NAME##InsertPtr(NAME *arr, TYPE *x, size_t len, size_t index){\
		if(index > arr->size){\
			arr->lastError = CUTILS_OUT_OF_BOUNDS;\
			return CUTILS_OUT_OF_BOUNDS;\
		}\
\
		size_t oldSize = arr->size;\
\
		int err = NAME##Resize(arr, arr->size+len);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		if(index != oldSize){\
			memmove(arr->data+index+len, arr->data+index, sizeof(TYPE)*(oldSize-index));\
		}\
		memcpy(arr->data+index, x, sizeof(TYPE)*len);\
\
		return CUTILS_OK;\
	}\
\
	int NAME##InsertArr(NAME *arr, NAME *x, size_t index){\
		if(index > arr->size){\
			arr->lastError = CUTILS_OUT_OF_BOUNDS;\
			return CUTILS_OUT_OF_BOUNDS;\
		}\
\
		size_t oldSize = arr->size;\
\
		int err = NAME##Resize(arr, arr->size+x->size);\
		if(err != CUTILS_OK){\
			return err;\
		}\
\
		if(index != oldSize){\
			memmove(arr->data+index+x->size, arr->data+index, sizeof(TYPE)*(oldSize-index));\
		}\
		memcpy(arr->data+index, x, sizeof(TYPE)*x->size);\
\
		return CUTILS_OK;\
	}\
\
	int NAME##Delete(NAME *arr, size_t index){\
		if(index >= arr->size){\
			arr->lastError = CUTILS_OUT_OF_BOUNDS;\
			return CUTILS_OUT_OF_BOUNDS;\
		}\
\
		if(arr->callback != NULL){\
			arr->callback(arr->data+index, 1, arr->usrptr);\
		}\
\
		if(index != arr->size-1){\
			memmove(arr->data+index, arr->data+index+1, sizeof(TYPE)*(arr->size-index-1));\
		}\
		arr->size--;\
\
		arr->lastError = CUTILS_OK;\
		return CUTILS_OK;\
	}\
\
	int NAME##DeleteRange(NAME *arr, size_t start, size_t end){\
		if(start >= arr->size || end >= arr->size){\
			arr->lastError = CUTILS_OUT_OF_BOUNDS;\
			return CUTILS_OUT_OF_BOUNDS;\
		}\
\
		if(arr->callback != NULL){\
			arr->callback(arr->data+start, end-start+1, arr->usrptr);\
		}\
\
		if(end == arr->size-1){\
			NAME##Resize(arr, end-start+1);\
			return CUTILS_OK;\
		}\
\
		memmove(arr->data+start, arr->data+end+1, sizeof(TYPE)*(arr->size-end-1));\
		NAME##Resize(arr, arr->size-(end-start)-1);\
\
		arr->lastError = CUTILS_OK;\
		return CUTILS_OK;\
	}

#endif
