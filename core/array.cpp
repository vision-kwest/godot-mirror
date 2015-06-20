/*************************************************************************/
/*  array.cpp                                                            */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "array.h"
#include "vector.h"
#include "hashfuncs.h"
#include "variant.h"
#include "object.h"

struct ArrayPrivate {

	SafeRefCount refcount;
	Vector<Variant> array;
	bool shared;
};

void Array::_ref(const Array& p_from) const {

	ArrayPrivate *_fp = p_from._p;

	ERR_FAIL_COND(!_fp); // should NOT happen.

	if (_fp == _p)
		return; //wathever it is, nothing to do here move along

	bool success = _fp->refcount.ref();

	ERR_FAIL_COND(!success); //should really not happen either

	_unref();

	if (_fp->shared) {

		_p = p_from._p;

	} else {

		_p = memnew( ArrayPrivate );
		_p->shared=false;
		_p->refcount.init();
		_p->array=_fp->array;

		if (_fp->refcount.unref())
			memdelete(_fp);
	}
}

void Array::_unref() const {

	if (!_p)
		return;

	if (_p->refcount.unref()) {
		memdelete(_p);
	}
	_p=NULL;
}


Variant& Array::operator[](int p_idx) {

	return _p->array[p_idx];
}

const Variant& Array::operator[](int p_idx) const {

	return _p->array[p_idx];

}

int Array::size() const {

	return _p->array.size();
}
bool Array::empty() const {

	return _p->array.empty();
}
void Array::clear() {

	_p->array.clear();
}

bool Array::is_shared() const {

    return _p->shared;
}

bool Array::operator==(const Array& p_array) const {

	return _p==p_array._p;
}

uint32_t Array::hash() const {

	uint32_t h=hash_djb2_one_32(0);

	for (int i=0;i<_p->array.size();i++) {

		h = hash_djb2_one_32( _p->array[i].hash(), h);
	}
	return h;
}
void Array::operator=(const Array& p_array) {

	_ref(p_array);
}
void Array::push_back(const Variant& p_value) {

	_p->array.push_back(p_value);
}

Error Array::resize(int p_new_size) {

	return _p->array.resize(p_new_size);
}

void Array::insert(int p_pos, const Variant& p_value) {

	_p->array.insert(p_pos,p_value);
}

void Array::erase(const Variant& p_value) {

	_p->array.erase(p_value);
}

int Array::find(const Variant& p_value) const {

	return _p->array.find(p_value);
}

void Array::remove(int p_pos) {

	_p->array.remove(p_pos);
}


void Array::set(int p_idx,const Variant& p_value) {

	operator[](p_idx)=p_value;
}

const Variant& Array::get(int p_idx) const {

	return operator[](p_idx);
}

struct _ArrayVariantSort {

	_FORCE_INLINE_ bool operator()(const Variant& p_l, const Variant& p_r) const {
		bool valid=false;
		Variant res;
		Variant::evaluate(Variant::OP_LESS,p_l,p_r,res,valid);
		if (!valid)
			res=false;
		return res;
	}
};

void Array::sort() {

	_p->array.sort_custom<_ArrayVariantSort>();

}

struct _ArrayVariantSortCustom {

	Object *obj;
	StringName func;

	_FORCE_INLINE_ bool operator()(const Variant& p_l, const Variant& p_r) const {

		const Variant*args[2]={&p_l,&p_r};
		Variant::CallError err;
		bool res = obj->call(func,args,2,err);
		if (err.error!=Variant::CallError::CALL_OK)
			res=false;
		return res;

	}
};
void Array::sort_custom(Object *p_obj,const StringName& p_function){

	ERR_FAIL_NULL(p_obj);

	SortArray<Variant,_ArrayVariantSortCustom> avs;
	avs.compare.obj=p_obj;
	avs.compare.func=p_function;
	avs.sort(_p->array.ptr(),_p->array.size());

}

void Array::invert(){

	_p->array.invert();
}



Array::Array(const Array& p_from) {

	_p=NULL;
	_ref(p_from);

}
Array::Array(bool p_shared) {

	_p = memnew( ArrayPrivate );
	_p->refcount.init();
	_p->shared=p_shared;
}
Array::~Array() {

	_unref();
}
