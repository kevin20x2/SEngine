//
// Created by 郑文凯 on 2024/10/4.
//

#ifndef ARRAY_H
#define ARRAY_H
#include <vector>
#include <algorithm>

template <typename T>
class TSArrayAllocator :public std::allocator<T>
{
public:
	using size_type = uint32_t;
};

template <typename T>
class TArray : public std::vector <T, TSArrayAllocator<T> >
{
public:
	using BaseVectorType = std::vector <T ,TSArrayAllocator<T> > ;
	TArray() {};

	TArray(std::initializer_list<T> il) :
		BaseVectorType(il) {}

	TArray(uint32_t Count) : BaseVectorType(Count) {}
	TArray (const TArray <T> & Rhs) : BaseVectorType(Rhs) {}
	TArray(uint32_t Count , const T & Default) : BaseVectorType(Count, Default) {}

	void Add(const T & InValue)
	{
		this->push_back(InValue);
	}

	bool Contains(const T & InValue) const
	{
		return std::find(this->begin(),this->end(),InValue) != this->end();
	}

	template <class PredictType>
	typename BaseVectorType::iterator FindByPredict(PredictType && Predict )
	{
		return std::find_if(this->begin(),this->end() , Predict );
	}

	bool Remove(const T& InValue)
	{
		return  std::remove_if(this->begin(),this->end(),[&](T & V){
			return V == InValue;
		}) != this->end();
	}
	bool AddUnique(const T & InValue)
	{
		if(Contains(InValue))
		{
			return false;
		}
		Add(InValue);
		return true;
	}
};

#endif //ARRAY_H
