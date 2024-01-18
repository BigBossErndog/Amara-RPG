namespace Amara {
    class Tween_XY: public Tween {
        public:
            float startX = 0;
            float startY = 0;
            float targetX = 0;
            float targetY = 0;

            Tween_XY(float tx, float ty, double tt, Amara::Easing gEasing) {
                targetX = tx;
                targetY = ty;
                time = tt;
                easing = gEasing;
            }
            Tween_XY(float tx, float ty, double tt): Tween_XY(tx, ty, tt, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                startX = actor->x;
                startY = actor->y;
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();
                switch (easing) {
                    case LINEAR:
                        actor->x = linearEase(startX, targetX, progress);
                        actor->y = linearEase(startY, targetY, progress);
                        break;
                    case SINE_INOUT:
                        actor->x = sineInOutEase(startX, targetX, progress);
                        actor->y = sineInOutEase(startY, targetY, progress);
                        break;
                    case SINE_IN:
                        actor->x = sineInEase(startX, targetX, progress);
                        actor->y = sineInEase(startY, targetY, progress);
                        break;
                    case SINE_OUT:
                        actor->x = sineOutEase(startX, targetX, progress);
                        actor->y = sineOutEase(startY, targetY, progress);
                        break;
                }
            }
    };

    class Tween_X: public Tween {
    public:
        float startX = 0;
        float targetX = 0;

        Tween_X(float tx, double tt, Amara::Easing gEasing) {
            targetX = tx;
            time = tt;
            easing = gEasing;
        }
        Tween_X(float tx, double tt): Tween_X(tx, tt, LINEAR) {}

        void prepare(Amara::Actor* actor) {
            startX = actor->x;
        }

        void script(Amara::Actor* actor) {
            Amara::Tween::progressFurther();
            actor->x = ease(startX, targetX, progress, easing);
        }
    };

    class Tween_Y: public Tween {
    public:
        float startY = 0;
        float targetY = 0;

        Tween_Y(float ty, double tt, Amara::Easing gEasing) {
            targetY = ty;
            time = tt;
            easing = gEasing;
        }
        Tween_Y(float ty, double tt): Tween_Y(ty, tt, LINEAR) {}

        void prepare(Amara::Actor* actor) {
            startY = actor->y;
        }

        void script(Amara::Actor* actor) {
            Amara::Tween::progressFurther();
            actor->y = ease(startY, targetY, progress, easing);
        }
    };

    class Tween_RelativeXY: public Tween {
        public:
            float startX = 0;
            float startY = 0;
            float targetX = 0;
            float targetY = 0;

            Tween_RelativeXY(float tx, float ty, double tt, Amara::Easing gEasing) {
                targetX = tx;
                targetY = ty;
                time = tt;
                easing = gEasing;
            }
            Tween_RelativeXY(float tx, float ty, double tt): Tween_RelativeXY(tx, ty, tt, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                startX = actor->x;
                startY = actor->y;
                targetX += actor->x;
                targetY += actor->y;
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();
                actor->x = ease(startX, targetX, progress, easing);
                actor->y = ease(startY, targetY, progress, easing);
            }
    };

	class Tween_XYZ: public Tween {
        public:
            float startX = 0;
            float startY = 0;
			float startZ = 0;
            float targetX = 0;
            float targetY = 0;
			float targetZ = 0;

            Tween_XYZ(float tx, float ty, float tz, double tt, Amara::Easing gEasing) {
                targetX = tx;
                targetY = ty;
				targetZ = tz;
                time = tt;
                easing = gEasing;
            }
            Tween_XYZ(float tx, float ty, float tz, double tt): Tween_XYZ(tx, ty, tz, tt, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                startX = actor->x;
                startY = actor->y;
				startZ = actor->z;
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();
                actor->x = ease(startX, targetX, progress, easing);
                actor->y = ease(startY, targetY, progress, easing);
                actor->z = ease(startZ, targetZ, progress, easing);
            }
    };

    class Tween_ScaleXY: public Tween {
        public:
            float startScaleX;
            float startScaleY;
            float targetScaleX;
			float targetScaleY;

            Tween_ScaleXY(float tx, float ty, double tt, Amara::Easing gEasing) {
                targetScaleX = tx;
				targetScaleY = ty;
                time = tt;
                easing = gEasing;
            }
			Tween_ScaleXY(float ts, double tt, Amara::Easing gEasing): Tween_ScaleXY(ts, ts, tt, gEasing) {}
            Tween_ScaleXY(float ts, double tt): Tween_ScaleXY(ts, tt, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                startScaleX = actor->scaleX;
                startScaleY = actor->scaleY;
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();
                actor->scaleX = ease(startScaleX, targetScaleX, progress, easing);
                actor->scaleY = ease(startScaleY, targetScaleY, progress, easing);
            }
    };
	
	class Tween_SnapXY: public Script {
	public:
		float snapX;
		float snapY;

		Tween_SnapXY(float gx, float gy) {
			snapX = gx;
			snapY = gy;
		}

		void finish() {
			parent->x = snapX;
			parent->y = snapY;
			Script::finish();
		}

		void script() {
			finish();
		}
	};

    class Tween_Rotate: public Tween {
    public:
        double rotateAmount = 0;
        double targetAngle = 0;
        double startAngle = 0;

        Tween_Rotate(double amount, double tt, Amara::Easing gEasing) {
            rotateAmount = amount;
            time = tt;
            easing = gEasing;
        }
        Tween_Rotate(double amount, double tt): Tween_Rotate(amount, tt, LINEAR) {}

        void prepare() {
            startAngle = parent->angle;
            targetAngle = parent->angle + rotateAmount;
        }

        void script() {
            Amara::Tween::progressFurther();
            parent->angle = ease(startAngle, targetAngle, progress, easing);
        }

        void finish() {
            parent->angle = fmod(targetAngle, 360);
            while (parent->angle < 0) parent->angle += 360;
            Tween::finish();
        }
    };

    class Tween_ShakeXY: public Tween {
        public:
            float startX;
            float startY;
            float maxShakeX;
            float maxShakeY;
			float targetX;
			float targetY;

            Actor* ent;

			Tween_ShakeXY(float gMaxShakeX, float gMaxShakeY, float gTargetX, float gTargetY, float tt, Amara::Easing gEasing) {
				maxShakeX = gMaxShakeX;
                maxShakeY = gMaxShakeY;
				targetX = gTargetX;
				targetY = gTargetY;
                time = tt;
                easing = gEasing;
			}
            Tween_ShakeXY(float gMaxShakeX, float gMaxShakeY, float tt, Amara::Easing gEasing): Tween_ShakeXY(gMaxShakeX, gMaxShakeY, 0, 0, tt, gEasing)  {}
            Tween_ShakeXY(float gMaxShake, float tt, Amara::Easing gEasing): Tween_ShakeXY(gMaxShake, gMaxShake, tt, gEasing) {}
            Tween_ShakeXY(float gMaxShake, float tt): Tween_ShakeXY(gMaxShake, tt, LINEAR) {}
			Tween_ShakeXY(float gMaxShakeX, float gMaxShakeY, float gTargetX, float gTargetY, float tt): Tween_ShakeXY(gMaxShakeX, gMaxShakeY, gTargetX, gTargetY, tt, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                startX = actor->x;
                startY = actor->y;
                ent = actor;

				if (targetX == -1) targetX = maxShakeX;
				if (targetY == -1) targetY = maxShakeY;
            }

            void finish() {
                Tween::finish();
                ent->x = startX;
                ent->y = startY;
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();

                float shakeAmountX;
                float shakeAmountY;

                shakeAmountX = ease(maxShakeX, targetX, progress, easing);
                shakeAmountY = ease(maxShakeY, targetY, progress, easing);
                
                actor->x = startX + properties->rng->random()*shakeAmountX - shakeAmountX/2.0;
                actor->y = startY + properties->rng->random()*shakeAmountY - shakeAmountY/2.0;
            }

			void cancel(Amara::Actor* actor) {
				actor->x = startX;
				actor->y = startY;
			}
    };

	class Tween_ReverseShakeXY: public Tween {
        public:
            float startX;
            float startY;
            float maxShakeX;
            float maxShakeY;

            Actor* ent;

            Tween_ReverseShakeXY(float gMaxShakeX, float gMaxShakeY, float tt, Amara::Easing gEasing) {
                maxShakeX = gMaxShakeX;
                maxShakeY = gMaxShakeY;
                time = tt;
                easing = gEasing;
            }
            Tween_ReverseShakeXY(float gMaxShake, float tt, Amara::Easing gEasing): Tween_ReverseShakeXY(gMaxShake, gMaxShake, tt, gEasing) {}
            Tween_ReverseShakeXY(float gMaxShake, float tt): Tween_ReverseShakeXY(gMaxShake, tt, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                startX = actor->x;
                startY = actor->y;
                ent = actor;
            }

            void finish() {
                Tween::finish();
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();

                float shakeAmountX;
                float shakeAmountY;

                shakeAmountX = ease(maxShakeX, 0, (1-progress), easing);
                shakeAmountY = ease(maxShakeY, 0, (1-progress), easing);

                actor->x = startX + properties->rng->random()*shakeAmountX - shakeAmountX/2.0;
                actor->y = startY + properties->rng->random()*shakeAmountY - shakeAmountY/2.0;
            }

			void cancel(Amara::Actor* actor) {
				actor->x = startX;
				actor->y = startY;
			}
    };

    class Tween_Alpha: public Tween {
        public:
            float startAlpha = -1;
            float targetAlpha = 0;

            Tween_Alpha(float gTarget, float gTime, Amara::Easing gEasing) {
                targetAlpha = gTarget;
                time = gTime;
                easing = gEasing;
            }
            Tween_Alpha(float gTarget, float gTime): Tween_Alpha(gTarget, gTime, LINEAR) {}
            Tween_Alpha(float gTime): Tween_Alpha(0, gTime) {}


            Tween_Alpha(float gStart, float gTarget, float gTime, Amara::Easing gEasing): Tween_Alpha(gTarget, gTime, gEasing) {
                startAlpha = gStart;
            }
            Tween_Alpha(float gStart, float gTarget, float gTime): Tween_Alpha(gStart, gTarget, gTime, LINEAR) {}

            void prepare(Amara::Actor* actor) {
                if (startAlpha == -1) startAlpha = actor->alpha;
            }

            void script(Amara::Actor* actor) {
                Amara::Tween::progressFurther();
                actor->alpha = ease(startAlpha, targetAlpha, progress, easing);
            }

            void finish() {
                Tween::finish();
                parent->alpha = targetAlpha;
            }
    };

    class Tween_Jump: public Tween {
    public:
        float targetX;
        float targetY;
        float maxZ;

        float startX;
        float startY;
        float startZ;
        
        Tween_Jump(float gx, float gy, float gz, float gt) {
            targetX = gx;
            targetY = gy;
            maxZ = gz;
            time = gt;
        }

        void prepare(Actor* actor) {
            startX = actor->x;
            startY = actor->y;
            startZ = actor->z;
        }

        void script(Actor* actor) {
            Tween::progressFurther();
            actor->x = linearEase(startX, targetX, progress);
            actor->y = linearEase(startY, targetY, progress);
            actor->z = circularEase(startZ, maxZ, progress);
        }
    };

    class Script_SetVisible: public Script {
    public:
        bool toSet = true;
        bool justToggle = false;

        Script_SetVisible() {
            justToggle = true;
        }
        Script_SetVisible(bool v) {
            toSet = v;
        }

        void prepare() {
            if (justToggle) toSet = !parent->isVisible;
        }

        void script() {
            finish();
        }

        void finish() {
            parent->setVisible(toSet);
            Script::finish();
        }
    };

    class Tween_Depth: public Tween {
    public:
        float startDepth = 0;
        float targetDepth = 0;

        Tween_Depth(float gTarget, float gTime, Amara::Easing gEasing) {
            targetDepth = gTarget;
            time = gTime;
            easing = gEasing;
        }
        Tween_Depth(float gTarget, float gTime): Tween_Depth(gTarget, gTime, LINEAR) {}
        Tween_Depth(float gTime): Tween_Depth(-1, gTime) {}
        Tween_Depth(): Tween_Depth(0) {}

        void prepare(Amara::Actor* actor) {
            startDepth = actor->depth;
            if (targetDepth = -1) {
                Amara::Entity* entity;
                for (auto it = actor->children.begin(); it != actor->children.end();) {
                    entity = *it;
                    if (entity->depth > targetDepth) {
                        targetDepth = entity->depth + 0.1;
                    }
                }
            }
        }

        void script(Amara::Actor* actor) {
            Amara::Tween::progressFurther();
            actor->depth = ease(startDepth, targetDepth, progress, easing);
        }

        void finish() {
            Tween::finish();
            parent->depth = targetDepth;
        }
    };

    class Tween_FillRectWH: public Tween {
    public:
        FillRect* rect = nullptr;

        float targetW = 0;
        float targetH = 0;
        float startW = 0;
        float startH = 0;

        Tween_FillRectWH(float tw, float th, float tt, Amara::Easing gEasing) {
            targetW = tw;
            targetH = th;
            time = tt;
            easing = gEasing;
        }
        Tween_FillRectWH(float tw, float th, float tt): Tween_FillRectWH(tw, th, tt, LINEAR) {}

        void prepare() {
            if (rect == nullptr) rect = (FillRect*)parent;
            startW = rect->width;
            startH = rect->height;
        }

        void script(Amara::Actor* actor) {
            Amara::Tween::progressFurther();
            rect->width = ease(startW, targetW, progress, easing);
            rect->height = ease(startH, targetH, progress, easing);
        }

        void finish() {
            Tween::finish();
            rect->width = targetW;
            rect->height = targetH;
        }
    };

    class Tween_Color: public Tween {
    public:
        SDL_Color startColor;
        SDL_Color endColor;

        SDL_Color* affectColor;

        Tween_Color(SDL_Color* gAffect, SDL_Color gEnd, float tt, Amara::Easing gEasing) {
            startColor = *gAffect;
            affectColor = gAffect;
            endColor = gEnd;
            time = tt;
            easing = gEasing;
        }
        Tween_Color(SDL_Color& gAffect, SDL_Color gEnd, float tt, Amara::Easing gEasing): Tween_Color(&gAffect, gEnd, tt, gEasing) {}
        Tween_Color(SDL_Color& gAffect, SDL_Color gEnd, float tt): Tween_Color(gAffect, gEnd, tt, LINEAR) {}
        Tween_Color(FillRect* rect, SDL_Color gEnd, float tt, Amara::Easing gEasing): Tween_Color(rect->color,gEnd, tt, gEasing) {}
        Tween_Color(FillRect* rect, SDL_Color gEnd, float tt): Tween_Color(rect, gEnd, tt, LINEAR) {}

        void script(Amara::Actor* actor) {
            Amara::Tween::progressFurther();
            affectColor->r = ease(startColor.r, endColor.r, progress, easing);
            affectColor->g = ease(startColor.g, endColor.g, progress, easing);
            affectColor->b = ease(startColor.b, endColor.b, progress, easing);
            affectColor->a = ease(startColor.a, endColor.a, progress, easing);
        }

        void finish() {
            Tween::finish();
            affectColor->r = endColor.r;
            affectColor->g = endColor.g;
            affectColor->b = endColor.b;
            affectColor->a = endColor.a;
        }
    };

    class Tween_Wait: public Tween {
    public:
        Tween_Wait(double gt) {
            time = gt;
        }
        
        void script() {
            progressFurther();
        }
    };

    class Script_TimedBroadcast: public Amara::Script {
    public:
        std::string messageKey;
        nlohmann::json messageData = nullptr;

        float time = 0;

        Script_TimedBroadcast(float gTime, std::string gKey) {
            time = gTime;
            messageKey = gKey;
        }
        Script_TimedBroadcast(float gTime, std::string gKey, nlohmann::json gData): Script_TimedBroadcast(gTime, gKey) {
            messageData = gData;
        }

        void script() {
            start();

            wait(time);

            if (evt()) {
                parent->broadcastMessage(messageKey, messageData);
                finish();
            }
        }
    };

    class Script_Broadcast: public Amara::Script {
    public:
        std::string messageKey;
        nlohmann::json messageData = nullptr;

        Script_Broadcast(std::string gKey) {
            messageKey = gKey;
        }
        Script_Broadcast(std::string gKey, nlohmann::json gData): Script_Broadcast(gKey) {
            messageData = gData;
        }

        void script() {
            start();
            if (once()) broadcastMessage(messageKey, messageData);
            finishEvt();
        }
    };

    class Script_Destroy: public Script {
    public:
        Entity* toDestroy = nullptr;

        Script_Destroy() {}
        Script_Destroy(Entity* gEntity) {
            toDestroy = gEntity;
        }

        void prepare() {
            if (!toDestroy) toDestroy = parent;
        }

        void script() {
            if (toDestroy) toDestroy->destroy();
        }
    };

    class Script_Configure: public Script {
    public:
        Amara::Entity* target = nullptr;
        nlohmann::json config = nullptr;

        Script_Configure() {}
        Script_Configure(nlohmann::json gConfig) {
            config = gConfig;
        }
        Script_Configure(Entity* gTarget, nlohmann::json gConfig): Script_Configure(gConfig) {
            target = gTarget;
        }

        void prepare() {
            if (!target) target = parent;
        }

        void script() {
            target->configure(config);
            finish();
        }
    };
}
