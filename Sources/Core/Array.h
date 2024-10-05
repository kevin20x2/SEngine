//
// Created by 郑文凯 on 2024/10/4.
//

#ifndef ARRAY_H
#define ARRAY_H
#include <vector>
#include <algorithm>

template <typename T>
class TArray : public std::vector <T>
{
public:
	TArray() {};

	TArray(std::initializer_list<T> il) :
		std::vector<T>(il) {}

	TArray(uint32_t Count) : std::vector<T>(Count) {}
	TArray (const TArray <T> & Rhs) : std::vector<T>(Rhs) {}
	TArray(uint32_t Count , const T & Default) : std::vector <T> (Count, Default) {}

	void Add(const T & InValue)
	{
		this->push_back(InValue);
	}

	bool Contains(const T & InValue) const
	{
		return std::find(this->begin(),this->end(),InValue) != this->end();
	}
	bool Remove(const T& InValue)
	{
		return  std::remove_if(this->begin(),this->end(),[&](T & V){
			return V == InValue;
		}) != this->end();
	}
};

#endif //ARRAY_H
