#include <amara.h>
#include <amaraRPG.h>

using namespace Amara;
using namespace std;

class TestArea: public Area {
    public:
        Walker* gnik;

        TestArea() {
            config["map-texture"] = "tiles";
            config["map-layers"] = { "floor", "above" };
        }

        void preload() {
            load->spritesheet("teenGnik", "assets/teenGnikolas.png", 64, 64);
        }

        void create() {
            assets->addAnim("teenGnik", "downWalk", {2, 3, 2, 4}, 6, true);
            addProp(gnik = new Walker(0, 0, "teenGnik"), {
                {"originX", 0.5}, {"originY", 0.5}
            });
            gnik->play("downWalk");

            controls->addKey("up", K_UP);
            controls->addKey("down", K_DOWN);
            controls->addKey("left", K_LEFT);
            controls->addKey("right", K_RIGHT);
            controls->addKey("full", K_ESC);
        }

        void update() {
            Area::update();
            if (controls->isDown("up")) {
                gnik->walk(Up);
            }
            else if (controls->isDown("down")) {
                gnik->walk(Down);
            }
            else if (controls->isDown("left")) {
                gnik->walk(Left);
            }
            else if (controls->isDown("right")) {
                gnik->walk(Right);
            }
            if (controls->justDown("full")) {
                if (game->isFullscreen) {
                    game->startWindowedFullscreen();
                }
                else {
                    game->exitFullscreen();
                }
            }
        }
};

int main(int argc, char** args) {
    Game* game = new Game("Amara Game");
    game->init(480, 360);
    game->setWindowSize(960, 720);

    game->scenes->add("test", new TestArea());
    game->start("test");

    // game->scenes->add("enterSceneKey", new GiveSceneInstance());
    // game->start("enterStartingScene");

    return 0;
}
