namespace Amara {
    struct StateRecord {
        std::string name;
		nlohmann::json data;
        int event = 0;
        std::string jumpFlag;
    };

    class StateManager {
        public:
            Amara::GameProperties* properties = nullptr;
            std::string currentState;
            std::string lastState;

            std::vector<StateRecord> stateRecords;

            int currentEvent = 1;
            int eventLooker = 0;

            int waitCounter = 0;
            float waitTimeElapsed = 0;

            bool skipEvent = false;
            bool killLog = false;

			nlohmann::json data;

            std::string jumpFlag;

            StateManager() {
                reset();
            }

            StateManager(Amara::GameProperties* gProperties) {
                init(gProperties);
                StateManager();
            }

            void init(Amara::GameProperties* gProperties) {
                properties = gProperties;
            }

            void reset() {
                currentState.clear();
                lastState.clear();
                currentEvent = 1;
                jumpFlag.clear();
                stateRecords.clear();
            }

            bool resetEvt() {
                if (evt()) {
                    reset();
                    return true;
                }
                return false;
            }

            bool state(std::string key) {
                if (currentState.empty()) {
                    currentState = key;
                }

                if (currentState.compare(key) == 0) {
                    eventLooker = 0;
                    return true;
                }

                return false;
            }

            bool inState(std::string key) {
                if (currentState.compare(key) == 0) {
                    return true;
                }
                return false;
            }

            bool start() {
                if (currentState.empty()) {
                    eventLooker = 0;
                    return true;
                }
                return false;
            }

            void switchState(std::string key) {
                if (!currentState.empty()) {
                    lastState = currentState;
                    Amara::StateRecord record = {currentState, data, currentEvent, jumpFlag};
                    stateRecords.push_back(record);
                }

                currentState = key;
                currentEvent = 1;
				data.clear();

                if (properties && properties->testing && !killLog) {
                    SDL_Log("SWITCHSTATE: %s", key.c_str());
                }
            }

            bool switchStateEvt(std::string key) {
                if (once()) {
                    switchState(key);
                    return true;
                }
                return false;
            }

            void returnState() {
                if (stateRecords.empty()) {
                    reset();
                }
                else {
                    lastState = currentState;
                    
                    Amara::StateRecord record = stateRecords.back();
                    currentState = record.name;
                    currentEvent = record.event;
                    jumpFlag = record.jumpFlag;
					data = record.data;
                    stateRecords.pop_back();

                    if (properties && properties->testing && !killLog) {
                        SDL_Log("RETURNSTATE: %s", currentState.c_str());
                    }
                }
            }

            bool returnStateEvt() {
                if (evt()) {
                    returnState();
                    return true;
                }
                return false;
            }

            void restartState() {
                currentEvent = 1;
            }

            bool restartStateEvt() {
                if (evt()) {
                    restartState();
                    if (properties && properties->testing && !killLog) {
                        SDL_Log("RESTARTSTATE: %s", currentState.c_str());
                    }
                    return true;
                }
                return false;
            }

            bool evt() {
                eventLooker += 1;
                if (currentEvent == eventLooker) {

                    if (skipEvent) {
                        skipEvent = false;
                        return false;
                    }

                    return true;
                }
                return false;
            }

            bool once() {
                if (evt()) {
                    nextEvt();
                    return true;
                }
                return false;
            }

            void nextEvt() {
                currentEvent += 1;
                skipEvent = true;
            }

            bool nextEvtOn(bool cond) {
                if (cond) {
                    nextEvt();
                    return true;
                }
                return false;
            }

            bool wait(float time, bool skip) {
                bool ret = false;

                if (once()) {
                    waitCounter = 0;
                    waitTimeElapsed = 0;
                    ret = true;
                }

                if (evt()) {
                    float t;
                    if (properties != nullptr) {
                        t = properties->lps * time;
                    }
                    else {
                        t = 60 * time;
                    }

                    waitCounter += 1;
                    if (waitCounter >= t || skip) {
                        nextEvt();
                    }

                    ret = true;
                }

                return ret;
            }
			bool wait(float time) {
				return wait(time, false);
			}

            bool waitUntil(bool condition) {
                if (evt()) {
                    if (condition) nextEvt();
                    return true;
                }
                return false;
            }

            bool repeat(int num) {
                bool ret = false;
                for (int i = 0; i < num; i++) {
                    if (once()) {
                        ret = true;
                    }
                }
                return ret;
            }

            bool bookmark(std::string flag) {
                bool toReturn = false;

                if (once()) {}
                else {
                    if (jumpFlag.compare(flag) == 0) {
                        jumpFlag.clear();
                        currentEvent = eventLooker;
                        nextEvt();
                        toReturn = true;
                    }
                }

                return toReturn;
            }

            void jump(std::string flag) {
                jumpFlag = flag;
            }

            bool jumpEvt(std::string flag) {
                if (evt()) {
                    jump(flag);
                    return true;
                }
                return false;
            }
    };
}