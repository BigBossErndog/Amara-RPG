namespace Amara {
    class Entity;
    class Scene;
    class Broadcaster;

    typedef struct Message {
        Amara::Broadcaster* parent = nullptr;
        Amara::Scene* scene = nullptr;
        std::string key;
        nlohmann::json data;
        bool isActive = true;
        bool isNull = false;
        bool skip = false;
        bool forceRemove = false;

        bool is(std::string check) {
            if (key.compare(check) == 0) return true;
            if (json_is(data, check)) return true;
            return false;
        }
    } Message;

    class MessageQueue {
    public:
        Amara::GameProperties* properties = nullptr;
        std::list<Message> queue;

        static Message nullMessage;
        
        MessageQueue() {}
        MessageQueue(Amara::GameProperties* gProperties) {
            properties = gProperties;
        }
        
        void update() {
            for (auto it = queue.begin(); it != queue.end();) {
                Message msg = *it;
                if (msg.parent == nullptr || !msg.isActive || msg.forceRemove) {
                    if (msg.skip) msg.skip = false;
                    else {
                        it = queue.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }

        auto begin() {
            return queue.begin();
        }
        auto end() {
            return queue.end();
        }

        void clear() {
            queue.clear();
        }

        bool empty() {
            return (queue.size() == 0);
        }

        int size() {
            return queue.size();
        }

        Message& get(std::string gKey) {
            for (auto it = queue.begin(); it != queue.end(); ++it) {
                Message& msg = *it;
                if (msg.isActive && gKey.compare(msg.key) == 0) {
                    return msg;
                }
            }
            return nullMessage;
        }

        Message& broadcast(std::string key, nlohmann::json gData) {
            queue.push_back({ nullptr, properties->currentScene, key, gData });
            return queue.back();
        }

        Message& broadcast(Amara::Broadcaster* gParent, std::string key, nlohmann::json gData) {
            queue.push_back({ gParent, properties->currentScene, key, gData });
            return queue.back();
        }

        void clearMessagesOfScene(Amara::Scene* scene) {
            for (auto it = queue.begin(); it != queue.end();) {
                Message msg = *it;
                if (msg.scene == scene) {
                    it = queue.erase(it);
                    continue;
                }
                ++it;
            }
        }
    };
    Message MessageQueue::nullMessage = { nullptr, nullptr, "null", {}, false, true, true };
}