namespace Amara {
    class Keyboard;
    class Mouse;
    class GamepadManager;
    class TouchManager;

    enum InputMode {
        InputMode_None = 0,
        InputMode_Mouse = 1,
        InputMode_Keyboard = 2,
        InputMode_Gamepad = 4,
        InputMode_Touch = 8
    };
    
    class InputManager {
        public:
            Amara::GameProperties* properties = nullptr;
            Amara::Keyboard keyboard;
            Amara::Mouse mouse;
            Amara::GamepadManager gamepads;
            Amara::TouchManager touches;

            int mode = InputMode_None;
            InputMode lastMode = InputMode_None;

            std::string inputText;
            bool textInputOn = false;

            InputManager() {}
            InputManager(Amara::GameProperties* gProperties): Amara::InputManager() {
                properties = gProperties;
                keyboard = Amara::Keyboard(properties);
				mouse = Amara::Mouse(properties);
				gamepads = Amara::GamepadManager(properties);
				touches = Amara::TouchManager(properties);
            }

            void startTextInput() {
                inputText.clear();
                textInputOn = true;
                SDL_StartTextInput();
            }

            std::string stopTextInput() {
                textInputOn = false;
                SDL_StopTextInput();
                return inputText;
            }

            std::string getClipboardText() {
                inputText = SDL_GetClipboardText();
            }

            void setClipboardText(std::string txt) {
                SDL_SetClipboardText(txt.c_str());
            }
    };
} 