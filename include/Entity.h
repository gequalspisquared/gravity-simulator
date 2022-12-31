#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <string>

#include "Components.h"

class Entity
{
public: 
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CGravity> cGravity;

    bool isActive() const;

    const std::string &tag() const;

    const size_t id() const;

    void destroy();

private:
    friend class EntityManager;

    bool m_active     = true;
    size_t m_id       = 0;
    std::string m_tag = "Default";

    Entity(const size_t id, const std::string &tag);
};

#endif