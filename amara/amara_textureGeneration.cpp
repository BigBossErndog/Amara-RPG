#pragma once
#ifndef AMARA_TEXTUREGENERATION
#define AMARA_TEXTUREGENERATION

#include "amara.h"

namespace Amara {
    void drawRadialGradient(SDL_Renderer* gRenderer, int gx, int gy, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart, SDL_BlendMode blendMode) {
        SDL_SetRenderDrawBlendMode(gRenderer, blendMode);

        SDL_Color drawColor;
        int smallSide = (width < height) ? width : height;
        int smallSideWhich = (width < height) ? 0 : 1;
        
        float s, xDist, yDist, radius, progress;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                switch (smallSideWhich) {
                    case 0:
                        s = (y/(float)height) * width;
                        xDist = abs((width/2.0) - x);
                        yDist = abs((height/2.0) - s);
                        break;
                    case 1:
                        s = (x/(float)width) * height;
                        xDist = abs((width/2.0) - s);
                        yDist = abs((height/2.0) - y);
                        break;
                }
                radius = sqrt(xDist*xDist + yDist*yDist);
                progress = radius/(smallSide/2.0);
                progress = (progress-fadeStart)/(1-fadeStart);
                if (progress > 0) {
                    if (progress > 1) progress = 1;
                }
                else {
                    progress = 0;
                }
                drawColor.r = innerColor.r + (progress * (outerColor.r - innerColor.r));
                drawColor.g = innerColor.g + (progress * (outerColor.g - innerColor.g));
                drawColor.b = innerColor.b + (progress * (outerColor.b - innerColor.b));
                drawColor.a = innerColor.a + (progress * (outerColor.a - innerColor.a));

                SDL_SetRenderDrawColor(gRenderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
                SDL_RenderDrawPoint(gRenderer, gx + x, gy + y);
            }
        }
    }
    void drawRadialGradient(SDL_Renderer* gRenderer, int gx, int gy, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        drawRadialGradient(gRenderer, gx, gy, width, height, innerColor, outerColor, fadeStart, SDL_BLENDMODE_BLEND);
    }
    void drawRadialGradient(Amara::Entity* entity, int gx, int gy, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        drawRadialGradient(entity->properties->gRenderer, gx, gy, width, height, innerColor, outerColor, fadeStart);
    }
    void drawRadialGradient(Amara::Loader* loader, int gx, int gy, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        drawRadialGradient(loader->properties->gRenderer, gx, gy, width, height, innerColor, outerColor, fadeStart);
    }

    SDL_Texture* createRadialGradientTexture(SDL_Renderer* gRenderer, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        SDL_Texture* texture = SDL_CreateTexture(
                                    gRenderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    width,
                                    height
                                );

        SDL_SetRenderTarget(gRenderer, texture);
        drawRadialGradient(gRenderer, 0, 0, width, height, innerColor, outerColor, fadeStart, SDL_BLENDMODE_NONE);

        return texture;
    }

    SDL_Texture* createRadialGradientTexture(Amara::Entity* entity, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        return createRadialGradientTexture(entity->properties->gRenderer, width, height, innerColor, outerColor, fadeStart);
    }

    Amara::RadialGradientTexture* createRadialGradient(SDL_Renderer* gRenderer, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        SDL_Texture* tx = createRadialGradientTexture(gRenderer, width, height, innerColor, outerColor, fadeStart);
        Amara::RadialGradientTexture* radial = new RadialGradientTexture("", IMAGE, tx);
        radial->configure(innerColor, outerColor, fadeStart);
        return radial;
    }
    Amara::RadialGradientTexture* createRadialGradient(Amara::Entity* entity, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        return createRadialGradient(entity->properties->gRenderer, width, height, innerColor, outerColor, fadeStart);
    }
    Amara::RadialGradientTexture* createRadialGradient(Amara::Loader* loader, int width, int height, SDL_Color innerColor, SDL_Color outerColor, float fadeStart) {
        return createRadialGradient(loader->properties->gRenderer, width, height, innerColor, outerColor, fadeStart);
    }


	void drawCircle(SDL_Renderer* gRenderer, int gx, int gy, float radius, SDL_Color color, SDL_BlendMode blendMode) {
        SDL_SetRenderDrawBlendMode(gRenderer, blendMode);

        int width = ceil(radius*2);
		int height = ceil(radius*2);
        
		float dist;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                dist = distanceBetween(x, y, (width/2 - 0.5), (height/2 - 0.5));
				if (dist <= radius) {
					SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
                	SDL_RenderDrawPoint(gRenderer, gx + x, gy + y);
				}
            }
        }
    }
	void drawCircle(SDL_Renderer* gRenderer, int gx, int gy, float radius, SDL_Color color) {
		drawCircle(gRenderer, gx, gy, radius, color, SDL_BLENDMODE_BLEND);
	}
	void drawCircle(Amara::Entity* entity, int gx, int gy, float radius, SDL_Color color) {
		drawCircle(entity->properties->gRenderer, gx, gy, radius, color);
	}
	void drawCircle(Amara::Loader* loader, int gx, int gy, float radius, SDL_Color color) {
		drawCircle(loader->properties->gRenderer, gx, gy, radius, color);
	}

	SDL_Texture* createCircleTexture(SDL_Renderer* gRenderer, float radius, SDL_Color color) {
		int width = ceil(radius*2);
		int height = ceil(radius*2);

		SDL_Texture* texture = SDL_CreateTexture(
                                    gRenderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    width,
                                    height
                                );

		SDL_SetRenderTarget(gRenderer, texture);
		SDL_RenderClear(gRenderer);
        drawCircle(gRenderer, 0, 0, radius, color, SDL_BLENDMODE_NONE);
	}
	SDL_Texture* createCircleTexture(Amara::Entity* entity, float radius, SDL_Color color) {
		return createCircleTexture(entity->properties->gRenderer, radius, color);
	}

	Amara::CircleTexture* createCircle(SDL_Renderer* gRenderer, float radius, SDL_Color color) {
		SDL_Texture* tx = createCircleTexture(gRenderer, radius, color);
        Amara::CircleTexture* circle = new CircleTexture("", IMAGE, tx);
        circle->configure(radius, color);
        return circle;
	}
}

#endif