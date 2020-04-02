#include <amara.h>
#include <amaraRPG.h>

using namespace Amara;
using namespace std;

class TestArea: public Area {
    public:
        Player* gnik;

        TestArea() {
            config["map-texture"] = "tiles";
            config["map-layers"] = { "floor", "above" };
        }

        void preload() {
            load->spritesheet("teenGnik", "assets/teenGnikolas.png", 64, 64);
        }

        void onCreate() {
            assets->addAnim("teenGnik", "downStand", 0);
            assets->addAnim("teenGnik", "upStand", 10);
            assets->addAnim("teenGnik", "leftStand", 20);
            assets->addAnim("teenGnik", "rightStand", 30);
            assets->addAnim("teenGnik", "downWalk", {2, 3, 2, 4}, 6, true);
            assets->addAnim("teenGnik", "upWalk", {12, 13, 12, 14}, 6, true);
            assets->addAnim("teenGnik", "leftWalk", {22, 23, 22, 24}, 6, true);
            assets->addAnim("teenGnik", "rightWalk", {32, 33, 32, 34}, 6, true);


            addProp(gnik = new Player(0, 0, "teenGnik"), {
                {"tileX", 0}, {"tileY", 0}
            });
            gnik->setOrigin(0.5, 70/80.0);
            gnik->play("downWalk");

            controls->addKey("up", KEY_UP);
            controls->addKey("down", KEY_DOWN);
            controls->addKey("left", KEY_LEFT);
            controls->addKey("right", KEY_RIGHT);
            controls->addKey("full", KEY_ESC);

            controls->addButton("up", BUTTON_DPAD_UP);
            controls->addButton("down", BUTTON_DPAD_DOWN);
            controls->addButton("left", BUTTON_DPAD_LEFT);
            controls->addButton("right", BUTTON_DPAD_RIGHT);

            controls->addButton("up", LEFTSTICK_UP);
            controls->addButton("down", LEFTSTICK_DOWN);
            controls->addButton("left", LEFTSTICK_LEFT);
            controls->addButton("right", LEFTSTICK_RIGHT);

            controls->addButton("full", BUTTON_A);
        }

        void onDuration() {
            
            if (controls->justDown("full")) {
                if (!game->isFullscreen) {
                    game->setWindowSize(game->display->width, game->display->height);
                    game->startFullscreen();
                }
                else {
                    game->setWindowSize(960, 720);
                    game->exitFullscreen();
                }
            }
        }
};

int main(int argc, char** args) {
    Game* game = new Game("Amara Game");
    game->init(480, 360);
    game->setWindowSize(960, 720);
    game->setBackgroundColor(0,0,0);

    game->scenes->add("test", new TestArea());
    game->start("test");

    // game->scenes->add("enterSceneKey", new GiveSceneInstance());
    // game->start("enterStartingScene");

    return 0;
}
