//
// Created by kevin on 2024/9/3.
//

#ifndef ENGINE_H
#define ENGINE_H


class SWorld;

class FEngine
{
    void Tick(float DeltaTime);
protected:
    SWorld * World;
};



#endif //ENGINE_H
