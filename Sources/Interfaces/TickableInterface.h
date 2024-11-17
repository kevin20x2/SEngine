//
// Created by kevin on 2024/11/18.
//

#ifndef TICKABLEINTERFACE_H
#define TICKABLEINTERFACE_H


class ITickable
{


public:
    virtual void Tick(float DeltaTime) {};
    virtual bool IsTickable() const
    {
        return false;
    }

};
#endif //TICKABLEINTERFACE_H
