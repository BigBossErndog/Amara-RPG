#include <amara.h>
#include <amaraRPG.h>

using namespace Amara;
using namespace std;

class TestArea: public RPGScene {
    public:
        Player* gnik;

        TestArea() {
            
        }

        void preload() {
            load->spritesheet("teenGnik", "assets/teenGnikolas.png", 64, 64);
            load->json("reeds_home", "assets/reeds_home.json");
            load->image("tiles", "assets/tiles.png");
        }

        void onPrepare() {
            config["map_texture"] = "tiles";
            config["map_wallLayers"] = { "walls" };
            config["map_aboveLayers"] = { "above" };
            config["map_hiddenLayers"] = { "walls" };
            config["map_json"] = "reeds_home";
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

            addProp(gnik = new Player());
            gnik->configure({
                {"texture", "teenGnik"},
                {"tileX", 2},
                {"tileY", 10},
                {"walkSpeed", 2}
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

            controls->addButton("walk", BUTTON_A);
            controls->addKey("walk", KEY_SPACE);

            controls->addKey("full", KEY_ESC);
            controls->addButton("full", BUTTON_RIGHTSHOULDER);

            mainCamera->startFollow(gnik);
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

            if (controls->isDown("walk")) {
                gnik->walkTo(4, 4);
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
