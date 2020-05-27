#include <amara.h>
#include <amaraRPG.h>

using namespace Amara;
using namespace std;

class WalkTo: public Script {
    public:
        Amara::Player* gnik;
        int x = 0, y = 0;

        WalkTo(int tx, int ty) {
            x = tx;
            y = ty;
        }

        void prepare(Amara::Actor* actor) {
            gnik = (Amara::Player*)actor;
            // gnik->disableControls();
        }

        void script() {
            if (gnik->walkTo(x, y)) {
                finish();
            }
        }
};

class CameraStuff: public Cutscene {
    public:
        Scene* scene;
        Player* gnik;
        TextBox* tb;

        void prepare(Actor* actor) {
            scene = (Scene*)actor;
            gnik = (Player*)scene->get("Gnikolas");
            gnik->controlsEnabled = false;
            tb = (TextBox*)scene->get("textBox");
            tb->copyStateManager(this);
            tb->setOpenCloseSpeed(10);
        }
        void script() {
            start();
            if (once()) {
                scene->mainCamera->scrollTo(15*TILE_WIDTH+TILE_WIDTH/2, 10*TILE_HEIGHT+TILE_WIDTH/2, 4, SINE_INOUT);
            }
            walk(gnik, 15, 10);
            if (once()) {
                gnik->play("downStand");
                scene->mainCamera->zoomTo(2, 1, SINE_INOUT);
            }
            wait(1);
            if (once()) {
                scene->mainCamera->zoomTo(1, 1, SINE_INOUT);
            }
            wait(1);
            tb->say("Hello mom, this textbox works now.");
            tb->say("I am very happy with this. It's awesome!");
            tb->close();
            if (once()) {
                scene->mainCamera->startFollow(gnik);
                gnik->enableControls();
            }
        }
};

class TestArea: public RPGScene {
    public:
        TextBox* box;
        Player* gnik;

        TestArea() {
            
        }

        void preload() {
            load->spritesheet("teenGnik", "assets/teenGnikolas.png", 64, 64);
            load->json("reeds_home", "assets/reeds_home.json");
            load->image("tiles", "assets/tiles.png");

            load->image("box", "assets/orangeTextbox.png");
            load->ttf("pressStart", "assets/press-start.regular.ttf", 8);
        }

        void prepare() {
            mapData["texture"] = "tiles";
            mapData["wallLayers"] = { "walls" };
            mapData["aboveLayers"] = { "above" };
            mapData["hiddenLayers"] = { "walls" };
            mapData["json"] = "reeds_home";
        }

        void create() {
            assets->addAnim("teenGnik", "downStand", 0);
            assets->addAnim("teenGnik", "upStand", 10);
            assets->addAnim("teenGnik", "leftStand", 20);
            assets->addAnim("teenGnik", "rightStand", 30);
            assets->addAnim("teenGnik", "downWalk", {3, 2, 4, 2}, 6, true);
            assets->addAnim("teenGnik", "upWalk", {13, 12, 14, 12}, 6, true);
            assets->addAnim("teenGnik", "leftWalk", {23, 22, 24, 22}, 6, true);
            assets->addAnim("teenGnik", "rightWalk", {33, 32, 34, 32}, 6, true);

            add(gnik = new Player());
            gnik->configure({
                {"id","Gnikolas"},
                {"texture", "teenGnik"},
                {"tileX", 2},
                {"tileY", 10},
                {"walkSpeed", 1}
            });
            gnik->setOrigin(0.5, 70/80.0);
            // gnik->recite(new WalkTo(15, 10));

            add(box = new TextBox(20, 20, 200, 80, "box", "pressStart"));
            box->setText("Somebody once told me the world revolved around me.");
            box->setTextColor(255, 255, 255);
            box->setProgressive();
            box->id = "textBox";
            box->setScrollFactor(0);
            box->setZoomFactor(0);
            box->bringToFront();
            box->setVisible(false);

            mainCamera->centerOn(gnik);

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

            controls->addKey("zoomIn", KEY_W);
            controls->addKey("zoomOut", KEY_S);

            controls->addButton("walk", BUTTON_A);
            controls->addKey("walk", KEY_SPACE);

            controls->addKey("full", KEY_ESC);
            controls->addButton("full", BUTTON_RIGHTSHOULDER);

            controls->addKey("confirm", KEY_Z);
            box->setProgressControl("confirm");

            // mainCamera->startFollow(gnik);
            mainCamera->offsetY = -TILE_HEIGHT/2;
            startCutscene(new CameraStuff());
        }

        void update() {
            if (controls->justDown("full")) {
                if (!game->isFullscreen) {
                    int w = 480;
                    int h = w * (game->display->height/(float)game->display->width);
                    game->setResolution(w, h);
                    game->setWindowSize(game->display->width, game->display->height);
                    game->startWindowedFullscreen();
                }
                else {
                    game->setResolution(480, 360);
                    game->setWindowSize(960, 720);
                    game->exitFullscreen();
                }
            }
            if (controls->isDown("zoomIn")) {
                mainCamera->changeZoom(0.1);
            }
            if (controls->isDown("zoomOut")) {
                mainCamera->changeZoom(-0.1);
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
