//
// Created by kevin on 2024/11/23.
//

#ifndef MAP_H
#define MAP_H
#include <unordered_map>

template <typename Key , typename Value>
class TMap : public std::unordered_map<Key ,Value>
{
public:
    bool Contains(const Key & InKey) const
    {
        return this->find(InKey) != this->end();
    }

};

#endif //MAP_H
