namespace Amara {
    struct LoadTask {
        Amara::AssetType type;
        bool replace;

        std::string key;
        std::string path;

        int frameWidth;
        int frameHeight;

        int size;
        int style;
        Amara::Color color;

		int tries = 0;
    };

    class LoadManager: public Loader {
        public:
            Amara::Loader* load = nullptr;

            std::deque<Amara::LoadTask> tasks;

            LoadManager(Amara::GameProperties* gameProperties): Amara::Loader(gameProperties) {
                load = properties->loader;
                reset();
            }

            void reset() {
                tasks.clear();
                load->clearBasePath();
            }

            Amara::Asset* get(std::string key) {
                return load->get(key);
            }

			bool has(std::string key) {
				return load->has(key);
			}

            bool remove(std::string key) {
                return load->remove(key);
            }

            bool add(std::string key, SDL_Texture* tx, bool replace) {
                return load->add(key, tx, replace);
            }
            bool add(std::string key, SDL_Texture* tx) {
                return load->add(key, tx, false);
            }
            bool add(std::string key, SDL_Texture* tx, int frwidth, int frheight, bool replace) {
                return load->add(key, tx, frwidth, frheight, replace);
            }
            bool add(std::string key, Amara::Asset* newAsset, bool replace) {
                return load->add(key, newAsset, replace);
            }
            bool add(std::string key, Amara::Asset* newAsset) {
                return load->add(key, newAsset, false);
            }

            virtual void clearAssets() {
                load->clearAssets();
            }

            void regenerateAssets() {
                load->regenerateAssets();
            }

            virtual void clearPseudonyms() {
                load->clearPseudonyms();
            }

            virtual void clearPseudonyms(nlohmann::json config) {
                load->clearPseudonyms(config);
            }

            virtual void setPseudonyms(nlohmann::json config) {
                load->setPseudonyms(config);
            }
            
            virtual int numberOfTasksLeft() {
                return tasks.size();
            }

            void run() {
                stillLoading = false;
                Amara::LoadTask task;
                int count = 0;
                bool success = false;
                while (tasks.size() > 0 && count < load->loadSpeed) {
                    task = tasks.front();
                    success = false;
                    stillLoading  = true;

                    switch (task.type) {
                        case ASSETPATH:
                            load->setBasePath(task.path);
                            success = true;
                            break;
						case SURFACE:
							success = load->surface(task.key, task.path, task.replace);
							break;
                        case IMAGE:
                            success = load->image(task.key, task.path, task.replace);
                            break;
                        case SPRITESHEET:
                            success = load->spritesheet(task.key, task.path, task.frameWidth, task.frameHeight, task.replace);
                            break;
                        case STRINGFILE:
                            success = load->string(task.key, task.path, task.replace);
                            break;
                        case JSONFILE:
                            success = load->json(task.key, task.path, task.replace);
                            break;
                        case TTF:
                            success = load->ttf(task.key, task.path, task.size, task.color, task.style, task.replace);
                            break;
                        case SOUND:
                            success = load->sound(task.key, task.path, task.replace);
                            break;
                        case MUSIC:
                            success = load->music(task.key, task.path, task.replace);
                            break;
                        case LINEBYLINE:
                            success = load->lineByLine(task.key, task.path, task.replace);
                            break;
                        case CSVFILE:
                            success = load->csv(task.key, task.path, task.replace);
                            break;
                    }

                    count += 1;
                    progress = 1 - (numberOfTasksLeft()/(float)numberOfTasks);

                    if (success) {
                        tasks.pop_front();
                    }
					else {
						if (task.tries < numberOfTries) {
							task.tries += 1;
						}
						else {
                            SDL_Log("Gave up on load task: %s", task.key.c_str());
                            tasks.pop_front();
                            failedTasks += 1;
						}
					}
                }
            }

            void setLoadSpeed(int speed) {
                load->setLoadSpeed(speed);
            }

            void pushTask(std::string key, Amara::LoadTask asset) {
                asset.key = key;
                tasks.push_back(asset);
                numberOfTasks += 1;
            }

            void setBasePath(std::string path) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = ASSETPATH;
                t.path = path;
                pushTask("setPath", t);
            }

			bool surface(std::string key, std::string path, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = SURFACE;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }

            bool image(std::string key, std::string path, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = IMAGE;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }

            bool spritesheet(std::string key, std::string path, int frameWidth, int frameHeight, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = SPRITESHEET;
                t.path = path;
                t.replace = replace;
                t.frameWidth = frameWidth;
                t.frameHeight = frameHeight;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }

            bool sound(std::string key, std::string path, bool replace) {
				Amara::LoadTask t = Amara::LoadTask();
                t.type = SOUND;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
			}

            bool music(std::string key, std::string path, bool replace) {
				Amara::LoadTask t = Amara::LoadTask();
                t.type = MUSIC;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
			}

            bool string(std::string key, std::string path, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = STRINGFILE;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }

            bool json(std::string key, std::string path, bool replace) {
                Amara::LoadTask t  = Amara::LoadTask();
                t.type = JSONFILE;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }

            bool ttf(std::string key, std::string path, int size, Amara::Color color, int style, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = TTF;
                t.path = path;
                t.replace = replace;
                t.size = size;
                t.color = color;
                t.style = style;
                pushTask(key, t);
                return true;
            }

            bool lineByLine(std::string key, std::string path, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = LINEBYLINE;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }

            bool csv(std::string key, std::string path, bool replace) {
                Amara::LoadTask t = Amara::LoadTask();
                t.type = CSVFILE;
                t.path = path;
                t.replace = replace;
                pushTask(key, t);
                return true;
            }
    };
}