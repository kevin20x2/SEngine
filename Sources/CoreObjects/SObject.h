//
// Created by kevin on 2024/8/17.
//

#ifndef SOBJECT_H
#define SOBJECT_H
#include "NanoRtti/NanoRtti.hpp"
#include "Core/BaseTypes.h"
#include <memory>

#define S_REGISTER_CLASS_COMMMON auto AsShared() \
{                                                         \
 using Type = std::remove_pointer<decltype(this)>::type;  \
 return std::static_pointer_cast<Type>(shared_from_this());\
}\


#define S_REGISTER_CLASS(...) NANO_RTTI_REGISTER_RUNTIME_CLASS(__VA_ARGS__) \
	S_REGISTER_CLASS_COMMMON


class SObject : public std::enable_shared_from_this <SObject>
{
public:
	S_REGISTER_CLASS()

	using TypeInfo = NanoRtti::TypeInfo;

	TypeInfo GetType() const
	{
		return NanoRttiTypeId();
	}

	template <class T>
	bool IsA() const
	{
		auto To =  NanoRtti::TypeId<T>;
		auto From = GetType();
		return NanoRtti::Detail::IsDynConvertible(From, To);
	}


	virtual ~SObject() {};



};



#endif //SOBJECT_H
