namespace Amara {
    class UIBox: public Amara::Actor, public Amara::MakeRect {
        public:
            SDL_Renderer* gRenderer = nullptr;
            SDL_Texture* canvas = nullptr;

            Amara::ImageTexture* texture = nullptr;
            std::string textureKey;

            SDL_Rect viewport;
            SDL_Rect srcRect;
            SDL_FRect destRect;
            SDL_FPoint origin;

            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;

            float recWidth = -1;
            float recHeight = -1;

            int minWidth = 0;
            int minHeight = 0;

            int openWidth = 0;
            int openHeight = 0;
            int recOpenWidth = -1;
            int recOpenHeight = -1;
            bool lockOpen = false;

            int openSpeedX = 0;
            int openSpeedY = 0;
            int closeSpeedX = 0;
            int closeSpeedY = 0;

            int boxTextureWidth = 0;
            int boxTextureHeight = 0;
            int imageWidth = 0;
            int imageHeight = 0;

            bool flipHorizontal = false;
            bool flipVertical = false;

            int partitionTop = 0;
            int partitionBottom = 0;
            int partitionLeft = 0;
            int partitionRight = 0;

            int frame = 0;

            bool textureLocked = true;
            bool pleaseUpdate = false;

            Amara::Alignment boxHorizontalAlignment = ALIGN_CENTER;
            Amara::Alignment boxVerticalAlignment = ALIGN_CENTER;

            Amara::StateManager* copySm = nullptr;
            Amara::StateManager mySm;

            bool keepOpen = false;

            Entity* content = nullptr;

            UIBox() {}

            UIBox(Amara::StateManager* gsm) {
                registerStateManager(gsm);
                setVisible(false);
            }

            UIBox(float gx, float gy, int gw, int gh, std::string gTextureKey): UIBox() {
                x = gx;
                y = gy;
                width = gw;
                height = gh;
                openWidth = width;
                openHeight = height;
                textureKey = gTextureKey;
            }

            UIBox(int gw, int gh, std::string gTextureKey): UIBox(0, 0, gw, gh, gTextureKey) {}

            virtual void init(Amara::GameProperties* gameProperties, Amara::Scene* givenScene, Amara::Entity* givenParent) {
                properties = gameProperties;
                load = properties->loader;
                gRenderer = properties->gRenderer;
                rectInit(this);

                if (!textureKey.empty()) {
                    setTexture(textureKey);
                }

                setSize(width, height);

                Amara::Actor::init(gameProperties, givenScene, givenParent);

                entityType = "uiBox";
            }
            
            virtual void configure(nlohmann::json config) {
                Amara::Actor::configure(config);
                rectConfigure(config);

                if (config.find("width") != config.end()) {
                    openWidth = width;
                }
                if (config.find("height") != config.end()) {
                    openHeight = height;
                }
                if (config.find("xFromRight") != config.end()) {
                    int xFromRight = config["xFromRight"];
                    x = scene->mainCamera->width - width - xFromRight;
                }
                if (config.find("yFromBottom") != config.end()) {
                    int yFromBottom = config["yFromBottom"];
                    y = scene->mainCamera->height - height - yFromBottom;
                }
				if (config.find("relativeXFromRight") != config.end()) {
					float relativeX = config["relativeXFromRight"];
					x = scene->mainCamera->width - scene->mainCamera->width*relativeX - width;
				}
				if (config.find("relativeYFromBottom") != config.end()) {
					float relativeY = config["relativeYFromBottom"];
					y = scene->mainCamera->height - scene->mainCamera->height*relativeY - height;
				}
				if (config.find("relativeXFromCenter") != config.end()) {
					float relativeX = config["relativeXFromCenter"];
					x = scene->mainCamera->centerX + scene->mainCamera->width*scene->mainCamera->zoomX*relativeX/2.0 - width/2.0;
				}
				if (config.find("relativeYFromCenter") != config.end()) {
					float relativeY = config["relativeYFromCenter"];
					y = scene->mainCamera->centerY + scene->mainCamera->height*scene->mainCamera->zoomY*relativeY/2.0 - height/2.0;
				}
                if (config.find("minWidth") != config.end()) {
                    minWidth = config["minWidth"];
                }
                if (config.find("minHeight") != config.end()) {
                    minHeight = config["minHeight"];
                }
                if (config.find("texture") != config.end()) {
                    setTexture(config["texture"]);
                }
                if (config.find("openSpeedX") != config.end()) {
                    openSpeedX = config["openSpeedX"];
                }
                if (config.find("openSpeedY") != config.end()) {
                    openSpeedY = config["openSpeedY"];
                }
                if (config.find("closeSpeedX") != config.end()) {
                    closeSpeedX = config["closeSpeedX"];
                }
                if (config.find("closeSpeedY") != config.end()) {
                    closeSpeedY = config["closeSpeedY"];
                }
                if (config.find("openCloseSpeedX") != config.end()) {
                    openSpeedX = config["openCloseSpeedX"];
                    closeSpeedX = config["openCloseSpeedX"];
                }
                if (config.find("openCloseSpeedY") != config.end()) {
                    openSpeedY = config["openCloseSpeedY"];
                    closeSpeedY = config["openCloseSpeedY"];
                }
                if (config.find("openCloseSpeed") != config.end()) {
                    setOpenCloseSpeed(config["openCloseSpeed"], config["openCloseSpeed"]);
                }
				if (config.find("fixedWithinBounds") != config.end() && config["fixedWithinBounds"]) {
					if (x < 0) x = 0;
					if (y < 0) y = 0;
					if (x + width > scene->mainCamera->width) {
						x = scene->mainCamera->width - width;
					}
					if (y + height > scene->mainCamera->height) {
						y = scene->mainCamera->height - height;
					}
				}
                if (config.find("partitionTop") != config.end()) {
                    partitionTop = config["partitionTop"];
                }
                if (config.find("partitionBottom") != config.end()) {
                    partitionBottom = config["partitionBottom"];
                }
                if (config.find("partitionLeft") != config.end()) {
                    partitionLeft = config["partitionLeft"];
                }
                if (config.find("partitionRight") != config.end()) {
                    partitionRight = config["partitionRight"];
                }
                if (config.find("boxHorizontalAlignment") != config.end()) {
                    boxHorizontalAlignment = config["boxHorizontalAlignment"];
                }
                if (config.find("boxVerticalAlignment") != config.end()) {
                    boxVerticalAlignment = config["boxVerticalAlignment"];
                }

                setOpenSpeed(openSpeedX, openSpeedY);
                setCloseSpeed(closeSpeedX, closeSpeedY);
            }

            virtual void drawBoxPart(int part) {
                bool skipDrawing = false;
                int partX = 0, partY = 0, partWidth = 0, partHeight = 0;

                float horizontalAlignmentFactor = 0.5;
                float verticalAlignmentFactor = 0.5;

                switch (boxHorizontalAlignment) {
                    case ALIGN_LEFT:
                        horizontalAlignmentFactor = 0;
                        break;
                    case ALIGN_RIGHT:
                        horizontalAlignmentFactor = 1;
                        break;
                }
                switch(boxVerticalAlignment) {
                    case ALIGN_TOP:
                        verticalAlignmentFactor = 0;
                        break;
                    case ALIGN_BOTTOM:
                        verticalAlignmentFactor = 1;
                        break;
                }

                switch (part % 3) {
                    case 0:
                        partX = 0;
                        partWidth = partitionLeft;
                        destRect.x = (width - openWidth)*horizontalAlignmentFactor;
                        destRect.w = partWidth;
                        break;
                    case 1:
                        partX = partitionLeft;
                        partWidth = boxTextureWidth - partitionLeft - partitionRight;
                        destRect.x = (width - openWidth)*horizontalAlignmentFactor + partitionLeft;
                        destRect.w = openWidth - partitionLeft - partitionRight;
                        break;
                    case 2:
                        partX = boxTextureWidth - partitionRight;
                        partWidth = partitionRight;
                        destRect.x = (width - openWidth)*horizontalAlignmentFactor + openWidth - partitionRight;
                        destRect.w = partWidth;
                        break;
                }
                switch ((int)floor(part/(float)3)) {
                    case 0:
                        partY = 0;
                        partHeight = partitionTop;
                        destRect.y = (height - openHeight)*verticalAlignmentFactor;
                        destRect.h = partHeight;
                        break;
                    case 1:
                        partY = partitionTop;
                        partHeight = boxTextureHeight - partitionTop - partitionBottom;
                        destRect.y = (height - openHeight)*verticalAlignmentFactor + partitionTop;
                        destRect.h = openHeight - partitionTop - partitionBottom;
                        break;
                    case 2:
                        partY = boxTextureHeight - partitionBottom;
                        partHeight = partitionBottom;
                        destRect.y = (height - openHeight)*verticalAlignmentFactor + openHeight - partitionBottom;
                        destRect.h = partHeight;
                        break;
                }

                if (destRect.w <= 0) skipDrawing = true;
                if (destRect.h <= 0) skipDrawing = true;

                if (!skipDrawing) {
                    if (texture != nullptr) {
                        SDL_Texture* tx = (SDL_Texture*)texture->asset;
                        switch (texture->type) {
                            case IMAGE:
                                frame = 0;
                                srcRect.x = partX;
                                srcRect.y = partY;
                                srcRect.w = partWidth;
                                srcRect.h = partHeight;
                                break;
                            case SPRITESHEET:
                                Amara::Spritesheet* spr = (Amara::Spritesheet*)texture;
                                int maxFrame = ((texture->width / spr->frameWidth) * (texture->height / spr->frameHeight));
                                frame = frame % maxFrame;

                                srcRect.x = (frame % (texture->width / spr->frameWidth)) * spr->frameWidth + partX;
                                srcRect.y = floor(frame / (texture->width / spr->frameWidth)) * spr->frameHeight + partY;
                                srcRect.w = partWidth;
                                srcRect.h = partHeight;
                                break;
                        }

                        SDL_RenderCopyF(
                            gRenderer,
                            (SDL_Texture*)(texture->asset),
                            &srcRect,
                            &destRect
                        );
                    }
                }
            }

            virtual void draw(int vx, int vy, int vw, int vh) override {
                if (!isVisible) return;
                if (openWidth < minWidth) openWidth = minWidth;
                if (openHeight < minHeight) openHeight = minHeight;

                if (alpha < 0) {
                    alpha = 0;
                    return;
                }
                if (alpha > 1) alpha = 1;

                if (recWidth != width || recHeight != height) {
                    createTexture();
                }
				else if (properties->reloadAssets) {
					createTexture();
				}

                if (lockOpen) {
                    openWidth = width;
                    openHeight = height;
                }
                if (openWidth > width) openWidth = width;
                if (openHeight > height) openHeight = height;
                
                if (recOpenWidth != openWidth || recOpenHeight != openHeight) {
                    pleaseUpdate = true;
                    recOpenWidth = openWidth;
                    recOpenHeight = openHeight;
                }
                
                if (pleaseUpdate || !textureLocked) {
                    pleaseUpdate = false;

                    SDL_Texture* recTarget = SDL_GetRenderTarget(properties->gRenderer);
                    SDL_SetRenderTarget(properties->gRenderer, canvas);
                    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
                    SDL_SetTextureAlphaMod(canvas, 255);
                    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
                    SDL_RenderClear(gRenderer);
                    SDL_RenderSetViewport(properties->gRenderer, NULL);
                    for (int i = 0; i < 9; i++) {
                        drawBoxPart(i);
                    }
                    SDL_SetRenderTarget(properties->gRenderer, recTarget);
                }

                bool skipDrawing = false;

                viewport.x = vx;
                viewport.y = vy;
                viewport.w = vw;
                viewport.h = vh;
                SDL_RenderSetViewport(properties->gRenderer, &viewport);

                float nzoomX = 1 + (properties->zoomX-1)*zoomFactorX*properties->zoomFactorX;
                float nzoomY = 1 + (properties->zoomY-1)*zoomFactorY*properties->zoomFactorY;

                bool scaleFlipHorizontal = false;
                bool scaleFlipVertical = false;
                float recScaleX = scaleX;
                float recScaleY = scaleY;

                if (scaleX < 0) {
                    scaleFlipHorizontal = true;
                    scaleX = abs(scaleX);
                }
                if (scaleY < 0) {
                    scaleFlipVertical = true;
                    scaleY = abs(scaleY);
                }
                scaleX = scaleX * (1 + (nzoomX - 1)*(zoomScaleX - 1));
                scaleY = scaleY * (1 + (nzoomY - 1)*(zoomScaleY - 1));

                float rotatedX = (x - properties->scrollX*scrollFactorX + properties->offsetX - (originX * width * scaleX));
                float rotatedY = (y-z - properties->scrollY*scrollFactorY + properties->offsetY - (originY * height * scaleY));

                destRect.x = (rotatedX * nzoomX);
                destRect.y = (rotatedY * nzoomY);
                destRect.w = ((width * scaleX) * nzoomX);
                destRect.h = ((height * scaleY) * nzoomY);
                
                origin.x = destRect.w * originX;
                origin.y = destRect.h * originY;

                if (destRect.x + destRect.w <= 0) skipDrawing = true;
                if (destRect.y + destRect.h <= 0) skipDrawing = true;
                if (destRect.x >= vw) skipDrawing = true;
                if (destRect.y >= vh) skipDrawing = true;
                if (destRect.w <= 0) skipDrawing = true;
                if (destRect.h <= 0) skipDrawing = true;

                if (!skipDrawing) {
                    if (canvas != nullptr) {
                        SDL_SetTextureBlendMode(canvas, blendMode);
				        SDL_SetTextureAlphaMod(canvas, alpha * properties->alpha * 255);

                        SDL_RendererFlip flipVal = SDL_FLIP_NONE;
                        if (!flipHorizontal != !scaleFlipHorizontal) {
                            flipVal = (SDL_RendererFlip)(flipVal | SDL_FLIP_HORIZONTAL);
                        }
                        if (!flipVertical != !scaleFlipVertical) {
                            flipVal = (SDL_RendererFlip)(flipVal | SDL_FLIP_VERTICAL);
                        }

                        SDL_RenderCopyExF(
                            properties->gRenderer,
                            canvas,
                            NULL,
                            &destRect,
                            angle + properties->angle,
                            &origin,
                            flipVal
                        );

						checkHover(vx, vy, vw, vh, destRect.x, destRect.y, destRect.w, destRect.h);
                    }
                }

                if (openWidth == width && openHeight == height) {
                    Amara::Actor::draw(vx, vy, vw, vh);
                }
            }

            void createTexture() {
                recWidth = width;
                recHeight = height;
                if (canvas != nullptr) {
                    tasks->queueTexture(canvas);
                }
                canvas = SDL_CreateTexture(
                    properties->gRenderer,
                    SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_TARGET,
                    floor(width),
                    floor(height)
                );
                
                pleaseUpdate = true;
            }

            bool setTexture(std::string gTextureKey) {
                if (texture) removeTexture();
                if (load == nullptr || properties == nullptr) {
                    textureKey = gTextureKey;
                    return true;
                }
                texture = (Amara::ImageTexture*)(load->get(gTextureKey));
                if (texture != nullptr) {
                   textureKey = texture->key;

                    if (texture->type == SPRITESHEET) {
                        boxTextureWidth = ((Amara::Spritesheet*)texture)->frameWidth;
                        boxTextureHeight = ((Amara::Spritesheet*)texture)->frameHeight;
                    }
                    else {
                        boxTextureWidth = texture->width;
                        boxTextureHeight = texture->height;
                    }

                    imageWidth = boxTextureWidth;
                    imageHeight = boxTextureHeight;

                    partitionLeft = imageWidth/3.0;
                    partitionRight = imageWidth/3.0;
                    partitionTop = imageHeight/3.0;
                    partitionBottom = imageHeight/3.0;

                    pleaseUpdate = true;

                    return true;
                }
                else {
                    SDL_Log("Texture with key: \"%s\" was not found.", gTextureKey.c_str());
                }
                return false;
            }

            UIBox* setPartitions(int top, int bottom, int left, int right) {
                partitionTop = top;
                partitionBottom = bottom;
                partitionLeft = left;
                partitionRight = right;
                return this;
            }

            void reloadAssets() {
                setTexture(textureKey);
            }

            void removeTexture() {
                textureKey.clear();
                if (texture && texture->temp) {
                    properties->taskManager->queueAsset(texture);
                }
                texture = nullptr;
            }

            void setSize(int nw, int nh) {
                width = nw;
                height = nh;
            }

            void setOpenSize(int nw, int nh) {
                openWidth = nw;
                openHeight = nh;
                if (openWidth > width) openWidth = width;
                if (openHeight > height) openHeight = height;
                if (openWidth < minWidth) openWidth = minWidth;
                if (openHeight < minHeight) openHeight = minHeight;
            }
            void changeOpenSize(int gx, int gy) {
                setOpenSize(openWidth + gx, openHeight + gy);
            }

            void forceOpenSize(int nw, int nh) {
                openWidth = nw;
                openHeight = nh;
                if (width < openWidth) width = openWidth;
                if (height < openHeight) height = openHeight;
                if (openWidth < minWidth) minWidth = (openWidth < 0) ? 0 : openWidth;
                if (openHeight < minHeight) minHeight = (openHeight < 0) ? 0 : openHeight;
            }

			void snapClosed(bool hor, bool ver) {
				if (hor) openWidth = minWidth;
				if (ver) openHeight = minHeight;
                if (content) content->setVisible(false);
			}
			void snapOpen(bool hor, bool ver) {
				if (hor) openWidth = width;
				if (ver) openHeight = height;
                if (content) content->setVisible(true);
			} 

            Amara::UIBox* setOrigin(float gx, float gy) {
                originX = gx;
                originY = gy;
                return this;
            }
            Amara::UIBox* setOrigin(float g) {
                return setOrigin(g, g);
            }
            Amara::UIBox* setOriginPosition(float gx, float gy) {
                originX = gx/width;
                originY = gy/height;
                return this;
            }
            Amara::UIBox* setOriginPosition(float g) {
                return setOriginPosition(g, g);
            }

            void registerStateManager(Amara::StateManager* gsm) {
                copySm = gsm;
            }
            void registerStateManager(Amara::StateManager& gsm) {
                copySm = &gsm;
            }

            Amara::StateManager& checkSm() {
                if (copySm != nullptr) {
                    return *copySm;
                }
                return mySm;
            }

            virtual bool show() {
                Amara::StateManager& sm = checkSm();
                if (sm.once()) {
                    setVisible(true);
                    if (content) content->setVisible(true);
                    return true;
                }
                return false;
            }

            virtual bool hide() {
                Amara::StateManager& sm = checkSm();
                if (sm.once()) {
                    setVisible(false);
                    if (content) content->setVisible(false);
                    return true;
                }
                return false;
            }

            virtual void onOpen() {}
            virtual void onClose() {}

            virtual bool open() {
                Amara::StateManager& sm = checkSm();
                bool toReturn = false;

                if (sm.once()) {
                    if (!keepOpen) {
                        resetOpenSize();
                    }
                }

                if (show()) {
                    toReturn = true;
                }

                if (sm.evt()) {
                    bool complete = true;

                    if (openSpeedX > 0) {
                        openWidth += openSpeedX;
                        if (openWidth >= width) {
                            openWidth = width;
                        }
                        else {
                            complete = false;
                        }
                    }

                    if (openSpeedY > 0) {
                        openHeight += openSpeedY;
                        if (openHeight >= height) {
                            openHeight = height;
                        }
                        else {
                            complete = false;
                        }
                    }

                    if (complete) {
                        keepOpen = true;
                        sm.nextEvt();
                    }

                    toReturn = true;
                }

                if (sm.once()) {
                    if (content) content->setVisible(true);
                    onOpen();
                    toReturn = true;
                }

                return toReturn;
            }

            virtual bool close() {
                Amara::StateManager& sm = checkSm();
                bool toReturn = false;

                if (sm.once()) {
                    if (content) content->setVisible(false);
                    onClose();
                    toReturn = true;
                }

                if (sm.evt()) {
                    bool complete = true;
                    if (closeSpeedX > 0) {
                        openWidth -= closeSpeedX;
                        if (openWidth <= minWidth) {
                            openWidth = minWidth;
                        }
                        else {
                            complete = false;
                        }
                    }

                    if (closeSpeedY > 0) {
                        openHeight -= closeSpeedY;
                        if (openHeight <= minHeight) {
                            openHeight = minHeight;
                        }
                        else {
                            complete = false;
                        }
                    }

                    if (complete) {
                        keepOpen = false;
                        sm.nextEvt();
                    }

                    toReturn = true;
                }

                if (hide()) {
                    toReturn = true;
                }

                return toReturn;
            }

            void setOpenSpeed(int gx, int gy) {
                openSpeedX = gx;
                openSpeedY = gy;

                if (openSpeedX < 0) openSpeedX = 0;
                if (openSpeedY < 0) openSpeedY = 0;

                resetOpenSize();

                lockOpen = false;
            }
            void setOpenSpeed(int gy) {
                setOpenSpeed(0, gy);
            }
            void setOpenSpeed() {
                setOpenSpeed(0);
            }

            void setCloseSpeed(int gx, int gy) {
                closeSpeedX = gx;
                closeSpeedY = gy;

                if (closeSpeedX < 0) closeSpeedX = 0;
                if (closeSpeedY < 0) closeSpeedY = 0;

                if (openWidth < minWidth) openWidth = minWidth;
                if (openHeight < minHeight) openHeight = minHeight;

                lockOpen = false;
            }
            void setCloseSpeed(int gy) {
                setCloseSpeed(0, gy);
            }
            void setCloseSpeed() {
                setCloseSpeed(0);
            }

            void setOpenCloseSpeed(int gx, int gy) {
                setOpenSpeed(gx, gy);
                setCloseSpeed(gx, gy);
            }
            void setOpenCloseSpeed(int gy) {
                setOpenCloseSpeed(0, gy);
            }
            void setOpenCloseSpeed() {
                setOpenCloseSpeed(0);
            }

            void resetOpenSize() {
                if (openSpeedX > 0) setOpenSize(0, openHeight);
                if (openSpeedY > 0) setOpenSize(openWidth, 0);
            }

            void drawOnce() {
                textureLocked = true;
                pleaseUpdate = true;
            }

            using Amara::Actor::destroy;
            virtual void destroy(bool recursive) {
                if (canvas) {
                    tasks->queueTexture(canvas);
                    canvas = nullptr;
                }
                Amara::Actor::destroy(recursive);
            }
    };

	class UIBox_Open: public Script {
	public:
		UIBox* box = nullptr;

		UIBox_Open() {}
		UIBox_Open(UIBox* gBox) { box = gBox; }

		void prepare() {
			if (box == nullptr) box = (UIBox*)parent;
			box->registerStateManager(this);
		}

		void script() {
			if (box->isDestroyed) {
				finish();
				return;
			}
			box->registerStateManager(this);
			start();
			box->open();
			finishEvt();
		}
	};

	class UIBox_Close: public Script {
	public:
		UIBox* box = nullptr;

		UIBox_Close() {}
		UIBox_Close(UIBox* gBox) { box = gBox; }

		void prepare() {
			if (box == nullptr) box = (UIBox*)parent;
		}

		void script() {
			if (box->isDestroyed) {
				finish();
				return;
			}
			box->registerStateManager(this);
			start();
			box->close();
			finishEvt();
		}
	};

	class UIBox_Timed: public Amara::Tween {
	public:
		UIBox* box = nullptr;
		
		int targetWidth = -1;
		int targetHeight = -1;
		int startWidth = 0;
		int startHeight = 0;
        
		UIBox_Timed(float tw, float th, float tt, Easing gEasing) {
			targetWidth = tw;
			targetHeight = th;
			time = tt;
			easing = gEasing;
		}
		UIBox_Timed(float tw, float th, float tt): UIBox_Timed(tw, th, tt, EASE_LINEAR) {}
		UIBox_Timed(float tt, Easing gEasing) {
            targetWidth = -1;
            targetHeight = -1;
            time = tt;
            easing = gEasing;
        }
        UIBox_Timed(float tt): UIBox_Timed(tt, EASE_LINEAR) {}
        
        UIBox_Timed(UIBox* gBox, float tw, float th, float tt, Easing gEasing): UIBox_Timed(tw, th, tt, gEasing) {
			box = gBox;
		}
		UIBox_Timed(UIBox* gBox, float tw, float th, float tt): UIBox_Timed(gBox, tw, th, tt, EASE_LINEAR) {}

		void prepare() {
			if (box == nullptr) box = (UIBox*)parent;

			startWidth = box->openWidth;
			startHeight = box->openHeight;

			if (targetWidth == -1) targetWidth = (box->openWidth > box->minWidth) ? box->minWidth : box->width;
			if (targetHeight == -1) targetHeight = (box->openHeight > box->minHeight) ? box->minHeight : box->height;

            if (targetWidth < box->minWidth) targetWidth = box->minWidth;
            if (targetHeight < box->minHeight) targetHeight = box->minHeight;
		}

		void script() {
			progressFurther();
			switch (easing) {
				case EASE_LINEAR:
					box->openWidth = linearEase(startWidth, targetWidth, progress);
					box->openHeight = linearEase(startHeight, targetHeight, progress);
					break;
				case EASE_SINE_INOUT:
					box->openWidth = sineInOutEase(startWidth, targetWidth, progress);
					box->openHeight = sineInOutEase(startHeight, targetHeight, progress);
					break;
				case EASE_SINE_IN:
					box->openWidth = sineInEase(startWidth, targetWidth, progress);
					box->openHeight = sineInEase(startHeight, targetHeight, progress);
					break;
				case EASE_SINE_OUT:
					box->openWidth = sineOutEase(startWidth, targetWidth, progress);
					box->openHeight = sineOutEase(startHeight, targetHeight, progress);
					break;
			}
		}

		void finish() {
			Amara::Tween::finish();
			box->openWidth = targetWidth;
			box->openHeight = targetHeight;
            if (box->content) {
                if (box->openWidth <= box->minWidth || box->openHeight <= box->minHeight) {
                    box->content->setVisible(false);
                }
                else {
                    box->content->setVisible(true);
                }
            }
		}
	};
}
