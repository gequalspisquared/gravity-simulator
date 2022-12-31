#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#include "Entity.h"
#include "EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::update()
{
    for (auto e : m_toAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_toAdd.clear();

    removeDeadEntities(m_entities);

    for (auto &[tag, entityVec] : m_entityMap) {
        removeDeadEntities(entityVec);
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::make_shared<Entity>(m_totalEntities++, tag);
    m_toAdd.push_back(entity);
    return entity;
}

EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    std::remove_if(vec.begin(), vec.end(), [](Entity &e){return !e.isActive();});
}