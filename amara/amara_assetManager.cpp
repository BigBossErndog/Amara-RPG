#pragma once
#ifndef AMARA_ASSETMANAGER
#define AMARA_ASSETMANAGER

#include "amara.h"

namespace Amara {
    class AssetManager {
        public:
            Amara::GameProperties* properties = nullptr;
            Amara::Loader* loader = nullptr;

            std::unordered_map<std::string, std::vector<std::string>> textureGroups;

            AssetManager(Amara::GameProperties* gameProperties) {
                properties = gameProperties;
                loader = properties->loader;
            }

            Amara::Asset* get(std::string key) {
                return loader->get(key);
            }

            nlohmann::json& getJSON(std::string key) {
                return loader->getJSON(key);
            }

            bool add(std::string key, SDL_Texture* tx, bool replace) {
                return loader->add(key, tx, replace);
            }
            bool add(std::string key, SDL_Texture* tx) {
                return add(key, tx, false);
            }

            bool add(std::string key, SDL_Texture* tx, int frwidth, int frheight, bool replace) {
                return loader->add(key, tx, frwidth, frheight, replace);
            }
            bool add(std::string key, SDL_Texture* tx, int frwidth, int frheight) {
                return add(key, tx, frwidth, frheight, true);
            }

            bool add(std::string key, Amara::Asset* newAsset, bool replace) {
                return loader->add(key, newAsset, replace);
            }
            bool add(std::string key, Amara::Asset* newAsset) {
                return loader->add(key, newAsset, false);
            }

            Amara::Animation* addAnim(std::string textureKey, std::string animKey, std::vector<int> frames, int frameRate, bool loop) {
                Amara::Asset* asset = get(textureKey);
                if (asset == nullptr) {
                    std::cout << "Couldn't find texture \"" << textureKey << "\" for animation \"" << animKey << "\"" << std::endl;
                    return nullptr;
                }
                if (asset->type == SPRITESHEET) {
                    return ((Amara::Spritesheet*)asset)->addAnim(animKey, frames, frameRate, loop);
                }
            }

            Amara::Animation* addAnim(std::string textureKey, std::string animKey, int frame) {
                return addAnim(textureKey, animKey, {frame}, 1, false);
            }

            void addAnim(std::vector<std::string> textureKeys, std::string animKey, std::vector<int> frames, int frameRate, bool loop) {
                for (std::string tKey : textureKeys) {
                    addAnim(tKey, animKey, frames, frameRate, loop);
                }
            }

            void addAnim(std::vector<std::string> textureKeys, std::string animKey, int frame) {
                addAnim(textureKeys, animKey, {frame}, 1, false);
            }

            void addAnim(std::string textureKey, std::string animKey, int startFrame, int numFrames, int frameRate, bool loop) {
                std::vector<int> frames;
                frames.clear();
                for (int i = 0; i < numFrames; i++) {
                    frames.push_back(startFrame + i);
                }
                addAnim(textureKey, animKey, frames, frameRate, loop);
            }

            void configureAnim(nlohmann::json& config) {
                std::string animKey = config["key"];
                bool loop = false;

                if (config.find("loop") != config.end()) {
                    loop = config["loop"];
                }

                if (config.find("textures") != config.end()) {
                    nlohmann::json& textures = config["textures"];
                    std::vector<std::string> textureKeys = textures;

                    if (config.find("frames") != config.end()) {
                        std::vector<int> frames = config["frames"];
                        int frameRate = config["frameRate"];
                        addAnim(textureKeys, animKey, frames, frameRate, loop);
                    }
                    else if (config.find("frame") != config.end()) {
                        int frame = config["frame"];
                        addAnim(textureKeys, animKey, frame);
                    }
                }
                if (config.find("texture") != config.end()) {
                    std::string textureKey = config["texture"];
                    if (config.find("frames") != config.end()) {
                        std::vector<int> frames = config["frames"];
                        int frameRate = config["frameRate"];
                        addAnim(textureKey, animKey, frames, frameRate, loop);
                    }
                    else if (config.find("frame") != config.end()) {
                        int frame = config["frame"];
                        addAnim(textureKey, animKey, frame);
                    }
                }
                if (config.find("group") != config.end()) {
                    std::string groupKey = config["group"];
                    if (textureGroups.find(groupKey) != textureGroups.end()) {
                        std::vector<std::string> textureKeys = textureGroups[groupKey];
                        
                        if (config.find("frames") != config.end()) {
                            std::vector<int> frames = config["frames"];
                            int frameRate = config["frameRate"];
                            addAnim(textureKeys, animKey, frames, frameRate, loop);
                        }
                        else if (config.find("frame") != config.end()) {
                            int frame = config["frame"];
                            addAnim(textureKeys, animKey, frame);
                        }
                    }
                }
                if (config.find("groups") != config.end()) {
                    std::vector<std::string> groups = config["groups"];
                    for (std::string groupKey: groups) {
                        if (textureGroups.find(groupKey) != textureGroups.end()) {
                            std::vector<std::string> textureKeys = textureGroups[groupKey];
                            
                            if (config.find("frames") != config.end()) {
                                std::vector<int> frames = config["frames"];
                                int frameRate = config["frameRate"];
                                addAnim(textureKeys, animKey, frames, frameRate, loop);
                            }
                            else if (config.find("frame") != config.end()) {
                                int frame = config["frame"];
                                addAnim(textureKeys, animKey, frame);
                            }
                        }
                    }
                }
            }

            void configureAnimations(nlohmann::json& config) {
                if (config.find("groups") != config.end()) {
                    nlohmann::json& groups = config["groups"];
                    for (nlohmann::json& group: groups) {
                        std::string key = group["key"];
                        std::vector<std::string> textures = group["textures"];
                        textureGroups[key] = textures;
                    }
                }

                if (config.find("animations") != config.end()) {
                    nlohmann::json& anims = config["animations"];
                    if (anims.is_array()) {
                        for (nlohmann::json& anim: anims) {
                            configureAnim(anim);
                        }
                    }
                }
                else {
                    configureAnim(config);
                }
            }

            void configureAnimations(std::string key) {
                Amara::JsonFile* configAsset = (Amara::JsonFile*)loader->get(key);
                if (configAsset != nullptr) {
                    configureAnimations(configAsset->getJSON());
                }
                else {
                    std::cout << "JSON animations not found: \"" << key << "\"" << std::endl;
                }
            }
    };
}

#endif