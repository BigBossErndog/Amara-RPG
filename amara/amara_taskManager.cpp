#pragma once
#ifndef AMARA_TASKMANAGER
#define AMARA_TASKMANAGER

#include "amara.h"

namespace Amara {
    class Entity;
    class SceneTransitionBase;
    
    class TaskManager {
        public:
            Amara::GameProperties* properties = nullptr;
            std::vector<Amara::Entity*> entityDeletionQueue;
            std::vector<Amara::SceneTransitionBase*> transitionDeletionQueue;

            TaskManager(Amara::GameProperties* gameProperties) {
                properties = gameProperties;
            }

            void queueDeletion(Amara::Entity* obj) {
                entityDeletionQueue.push_back(obj);
            }

            void queueDeletion(Amara::SceneTransitionBase* transition) {
                transitionDeletionQueue.push_back(transition);
            } 

            std::vector<Amara::Entity*>& getEntityQueue() {
                return entityDeletionQueue;
            }
            std::vector<Amara::SceneTransitionBase*>& getTransitionQueue() {
                return transitionDeletionQueue;
            }

            void run() {}
    };
}

#endif