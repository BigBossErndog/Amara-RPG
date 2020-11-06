#pragma once 

#include "amara.h"

namespace Amara {
    class PhysicsBody: public Amara::PhysicsBase {
    public:
        int bumpDirections = 0;

        using Amara::PhysicsBase::addCollisionTarget;
        void addCollisionTarget(Amara::Entity* other) {
            addCollisionTarget(other->physics);
        }

        using Amara::PhysicsBase::collidesWith;
        bool collidesWith(Amara::Entity* other) {
            return collidesWith(other->physics);
        }

        bool hasCollided(bool pushingX, bool pushingY) {
            bool col = false;
            for (Amara::PhysicsBase* body: collisionTargets) {
                if (collidesWith(body)) {
                    if (body->isPushable) {
                        if (pushingX) body->velocityX += velocityX * body->pushFrictionX;
                        if (pushingY) body->velocityY += velocityY * body->pushFrictionY;
                    }
                    if (pushingX || pushingY) {
                        col = true;
                        continue;
                    }
                    return true;
                }
            }
            return col;
        }

        bool hasCollided() {
            return hasCollided(false, false);
        }

        void run() {
            bumpDirections = 0;
            isPushing = false;

            velocityX += accelerationX;
            velocityY += accelerationY;
            float recX = (parent) ? parent->x : x;
            float recY = (parent) ? parent->y : y;
            float targetX = recX + velocityX;
            float targetY = recY + velocityY;

            if (!hasCollided()) {
                if (parent) parent->x = targetX;
                updateProperties();
                if (hasCollided(true, false)) {
                    if (velocityX > 0) bumpDirections += Right;
                    else if (velocityX < 0) bumpDirections += Left;
                    else bumpDirections += Right + Left;

                    while (hasCollided()) {
                        float xDir = velocityX/abs(velocityX) * correctionRate;
                        if (parent) parent->x -= xDir;
                        else x -= xDir;
                        updateProperties();
                    }
                    velocityX = 0;
                }

                if (parent != nullptr) parent->y = targetY;
                updateProperties();
                if (hasCollided(false, true)) {
                    if (velocityY > 0) bumpDirections += Down;
                    else if (velocityY < 0) bumpDirections += Up;
                    else bumpDirections += Down + Up;

                    while (hasCollided()) {
                        float yDir = velocityY/abs(velocityY) * correctionRate;
                        if (parent) parent->y -= yDir;
                        else y -= yDir;
                        updateProperties();
                    }
                    velocityY = 0;
                }

                if (bumpDirections & Up) {
                    if (bumpDirections & Left) bumpDirections += UpLeft;
                    if (bumpDirections & Right) bumpDirections += UpRight;
                }
                if (bumpDirections & Down) {
                    if (bumpDirections & Left) bumpDirections += DownLeft;
                    if (bumpDirections & Right) bumpDirections += DownRight;
                }

                velocityX = velocityX * frictionX;
                velocityY = velocityY * frictionY;
            }
		}
    };

    class PhysicsRectangle: public Amara::PhysicsBody {
    public:
        float width = 0;
        float height = 0;

        PhysicsRectangle() {
            shape = PHYSICS_RECTANGLE;
            properties.rect = { 0, 0, 0, 0 };
        }
        PhysicsRectangle(float gWidth, float gHeight): PhysicsRectangle() {
            width = gWidth;
            height = gHeight;
        }
        PhysicsRectangle(float gx, float gy, float gWidth, float gHeight): PhysicsRectangle(gWidth, gHeight) {
            x = gx;
            y = gy;
        }

        void updateProperties() {
            properties.rect.x = ((parent) ? parent->x : 0) + x;
            properties.rect.y = ((parent) ? parent->y : 0) + y;
            properties.rect.width = width;
            properties.rect.height = height;
        }

        bool collidesWith(Amara::PhysicsBase* body) {
            switch (body->shape) {
                case PHYSICS_RECTANGLE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.rect, &body->properties.rect);
                    break;
                case PHYSICS_CIRCLE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.rect, &body->properties.circle);
                    break;
                case PHYSICS_LINE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.rect, &body->properties.line);
                    break;
            }

            return body->collidesWith(this);
        }
    };

    class PhysicsCircle: public Amara::PhysicsBody {
    public:
        float radius = 0;

        PhysicsCircle() {
            properties.circle = { 0, 0, 0 };
            shape = PHYSICS_CIRCLE;
        }
        PhysicsCircle(float gRadius): PhysicsCircle() {
            radius = gRadius; 
        }
        PhysicsCircle(float gx, float gy, float gRadius): PhysicsCircle(gRadius)  {
            x = gx;
            y = gy;
        }

        bool collidesWith(Amara::PhysicsBase* body) {
            switch (body->shape) {
                case PHYSICS_RECTANGLE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.circle, &body->properties.rect);
                    break;
                case PHYSICS_CIRCLE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.circle, &body->properties.circle);
                    break;
                case PHYSICS_LINE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.circle, &body->properties.line);
                    break;
            }

            return body->collidesWith(this);
        }
        
        void updateProperties() {
            properties.circle.x = ((parent) ? parent->x : 0) + x;
            properties.circle.y = ((parent) ? parent->y : 0) + y;
            properties.circle.radius = radius;
        }


    };

    class PhysicsLine: public Amara::PhysicsBody {
    public:
        FloatVector2 p1 = {0, 0};
        FloatVector2 p2 = {0, 0};

        PhysicsLine() {
            properties.line = {{0, 0}, {0, 0}};
            shape = PHYSICS_LINE;
        }
        PhysicsLine(float p1x, float p1y, float p2x, float p2y): PhysicsLine() {
            float ex = ((parent) ? parent->x : 0);
            float ey = ((parent) ? parent->y : 0);
            p1 = {p1x + ex, p1y + ey};
            p2 = {p2x + ex, p2y + ey};
        }

        bool collidesWith(Amara::PhysicsBody* body) {
            switch (body->shape) {
                case PHYSICS_RECTANGLE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.line, &body->properties.rect);
                    break;
                case PHYSICS_CIRCLE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.line, &body->properties.circle);
                    break;
                case PHYSICS_LINE:
                    body->updateProperties();
                    return Amara::overlapping(&properties.line, &body->properties.line);
                    break;
            }

            return body->collidesWith(this);
        }

        void updateProperties() {
            properties.line.p1 = {p1.x + x, p1.y + y};
            properties.line.p2 = {p2.x + x, p2.y + y};
        }
    };

    class PhysicsTilemapLayer: public Amara::PhysicsBody {
    public:
        float defaultProgressRate = 0.1;
        int checkPadding = 1;

        PhysicsTilemapLayer() {
            shape = PHYSICS_TILEMAP_LAYER;
        }
        PhysicsTilemapLayer(float gx, float gy): PhysicsTilemapLayer() {
            x = gx;
            y = gy;
        }

        void create() {
            properties.tilemapLayer = (Amara::TilemapLayer*)parent;
        }

        bool lineCollision(Amara::PhysicsBase* body, float progRate) {
            Amara::TilemapLayer* tilemapLayer = properties.tilemapLayer;

            float sx, sy, tx, ty;
            float px = x;
            float py = y;
            if (tilemapLayer->tilemapEntity) {
                px += tilemapLayer->tilemapEntity->x;
                py += tilemapLayer->tilemapEntity->y;
            }

            for (float prog = 0; prog <= 1; prog += progRate) {
                sx = body->properties.line.p1.x + (body->properties.line.p2.x - body->properties.line.p1.x)*prog;
                sy = body->properties.line.p1.y + (body->properties.line.p2.y - body->properties.line.p1.y)*prog;

                Tile& tile = tilemapLayer->getTileAtXY(sx, sy);
                if (tile.id == -1) continue;
                tx = tile.x * tilemapLayer->tileWidth + px;
                ty = tile.y * tilemapLayer->tileHeight + py;

                properties.rect = { tilemapLayer->x + tx, tilemapLayer->y + ty, tilemapLayer->tileWidth, tilemapLayer->tileHeight };

                if (Amara::overlapping(&properties.rect, &body->properties.line)) {
                    return true;
                }
            }
            return false;
        }
        bool lineCollision(Amara::PhysicsBase* body) {
            return lineCollision(body, defaultProgressRate);
        }

        bool collidesWith(Amara::PhysicsBase* body) {
            Amara::TilemapLayer* tilemapLayer = properties.tilemapLayer;

            float sx, sy, ex, ey, tx, ty;
            body->updateProperties();
            switch (body->shape) {
                case PHYSICS_RECTANGLE:
                    sx = body->properties.rect.x + body->properties.rect.width/2.0;
                    sy = body->properties.rect.y + body->properties.rect.height/2.0;
                    break;
                case PHYSICS_CIRCLE:
                    sx = body->properties.circle.x;
                    sy = body->properties.circle.y;
                    break;
                case PHYSICS_LINE:
                    return lineCollision(body);
                    break;
            }

            float px = x;
            float py = y;
            if (tilemapLayer->tilemapEntity) {
                px += tilemapLayer->tilemapEntity->x;
                py += tilemapLayer->tilemapEntity->y;
            }

            Tile& centerTile = tilemapLayer->getTileAtXY(sx, sy);
            ex = centerTile.x + checkPadding;
            ey = centerTile.y + checkPadding;
            sx = centerTile.x - checkPadding;
            sy = centerTile.y - checkPadding;
            for (int i = sx; i <= ex; i++) {
                for (int j = sy; j <= ey; j++) {
                    Tile& tile = tilemapLayer->getTileAt(i, j);
                    if (tile.id == -1) continue;
                    tx = tile.x * tilemapLayer->tileWidth + px;
                    ty = tile.y * tilemapLayer->tileHeight + py;

                    properties.rect = { tilemapLayer->x + tx, tilemapLayer->y + ty, tilemapLayer->tileWidth, tilemapLayer->tileHeight };
                    switch (body->shape) {
                        case PHYSICS_RECTANGLE:
                            if (Amara::overlapping(&properties.rect, &body->properties.rect)) {
                                return true;
                            }
                            break;
                        case PHYSICS_CIRCLE:
                            if (Amara::overlapping(&properties.rect, &body->properties.circle)) {
                                return true;
                            }
                            break;
                    }
                }
            }

            return false;
        }
    };

    class PhysicsCollisionGroup: public PhysicsBody {
    public:
        std::vector<Amara::PhysicsBase*>members;

        PhysicsCollisionGroup() {
            members.clear();
        }

        Amara::PhysicsBase* add(Amara::Entity* gEntity) {
            if (gEntity->physics == nullptr) return nullptr;
            members.push_back(gEntity->physics);
            return gEntity->physics;
        }
        Amara::PhysicsBase* add(Amara::PhysicsBase* other) {
            if (other) members.push_back(other);
            return other;
        }
        Amara::PhysicsBase* remove(Amara::Entity* gEntity) {
            return remove(gEntity->physics);
        }
        Amara::PhysicsBase* remove(Amara::PhysicsBase* gBody) {
            for (int i = 0; i < members.size(); i++) {
                if (members[i] == gBody) {
                    members.erase(members.begin() + i);
                    return gBody;
                }
            }
            return nullptr;
        }
        void clear() {
            members.clear();
        }

        bool collidesWith(Amara::PhysicsBase* other) {
            for (Amara::PhysicsBase* body: members) {
                if (body == other) continue;
                if (body->collidesWith(other)) {
                    return true;
                }
            }
            return false;
        }

        void run() {}
    };
}