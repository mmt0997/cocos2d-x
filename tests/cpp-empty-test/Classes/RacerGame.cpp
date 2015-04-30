#include "RacerGame.h"
using namespace gameplay;
// Render queue indexes (in order of drawing).
enum RenderQueue
{
    QUEUE_OPAQUE = 0,
    QUEUE_TRANSPARENT,
    QUEUE_COUNT
};

bool __viewFrustumCulling = true;
bool __flythruCamera = false;
bool __drawDebug = false;
bool __useAccelerometer = false;

// Declare our game instance

// Input bit-flags (powers of 2)
#define ACCELERATOR (1 << 0)
#define BRAKE (1 << 1)
#define REVERSE (1 << 2)
#define UPRIGHT (1 << 3)
#define STEER_LEFT (1 << 4)
#define STEER_RIGHT (1 << 5)
#define ACCELERATOR_MOUSE (1 << 6)
#define BRAKE_MOUSE (1 << 7)

#define STEERING_RESPONSE (7.0f)

RacerGame::RacerGame()
    : _scene(NULL), _font(NULL), _keyFlags(0), _mouseFlags(0), _steering(0),
    _upsetTimer(0),
    _backgroundMusic(NULL), _engineSound(NULL), _brakingSound(NULL)
{
}

void RacerGame::initialize()
{
    setMultiTouch(true);

    _font = gameplay::Font::create("res/ui/arial.gpb");

    // Display the gameplay splash screen during loading, for at least 1 second.
    //gameplay::displayScreen(this, &RacerGame::drawSplash, NULL, 1000L);

    // Load the scene
    _scene = gameplay::Scene::load("res/common/racer.scene");

    // Set the aspect ratio for the scene's camera to match the current resolution
    _scene->getActiveCamera()->setAspectRatio(getAspectRatio());

    // Initialize scene
    _scene->visit(this, &RacerGame::initializeScene);

    // Load and initialize game script
    getScriptController()->loadScript("res/common/racer.lua");
    getScriptController()->executeFunction<void>("setScene", "<Scene>", _scene);

    // Create audio tracks
    _backgroundMusic = gameplay::AudioSource::create("res/common/background_track.ogg", true);
    if (_backgroundMusic)
    {
        _backgroundMusic->setLooped(true);
        _backgroundMusic->play();
        _backgroundMusic->setGain(0.3f);
    }

    _engineSound = gameplay::AudioSource::create("res/common/engine_loop.ogg");
    if (_engineSound)
    {
        _engineSound->setLooped(true);
        _engineSound->play();
        _engineSound->setGain(0.7f);
    }

    _brakingSound = gameplay::AudioSource::create("res/common/braking.wav", true);
    _brakingSound->setLooped(false);
    _brakingSound->setGain(0.5f);

}

bool RacerGame::initializeScene(gameplay::Node* node)
{
    static gameplay::Node* lightNode = _scene->findNode("directionalLight1");

    gameplay::Model* model = dynamic_cast<gameplay::Model*>(node->getDrawable());
    if (model)
    {
        gameplay::Material* material = model->getMaterial();

        if (material && material->getTechnique()->getPassByIndex(0)->getEffect()->getUniform("u_directionalLightDirection"))
        {
            material->getParameter("u_ambientColor")->setValue(_scene->getAmbientColor());
            material->getParameter("u_directionalLightColor[0]")->setValue(lightNode->getLight()->getColor());
            material->getParameter("u_directionalLightDirection[0]")->setValue(lightNode->getForwardVectorView());
        }
    }

    return true;
}

void RacerGame::finalize()
{
    SAFE_RELEASE(_backgroundMusic);
    SAFE_RELEASE(_engineSound);
    SAFE_RELEASE(_brakingSound);
    SAFE_RELEASE(_scene);
    SAFE_RELEASE(_font);
}

void RacerGame::update(float elapsedTime)
{
    gameplay::Node* cameraNode;
    if (_scene->getActiveCamera() && (cameraNode = _scene->getActiveCamera()->getNode()))
    {
        float dt = elapsedTime / 1000.0f;
        gameplay::Vector3 commandedPosition(-263.990082f, 5.000000f, 286.007416f);
        cameraNode->translateSmooth(commandedPosition, dt, 0.2f);
        gameplay::Quaternion q(-0.177254f, -0.310793f, -0.059113f, 0.931931f);
        cameraNode->setRotation(q);
    }
}

bool RacerGame::isUpset() const
{
    return true;
}

void RacerGame::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, gameplay::Vector4::zero(), 1.0f, 0);
    

    // Visit all the nodes in the scene to build our render queues
    for (unsigned int i = 0; i < QUEUE_COUNT; ++i)
        _renderQueues[i].clear();
    _scene->visit(this, &RacerGame::buildRenderQueues);

    // Draw the scene from our render queues
    drawScene();

//    if (__drawDebug)
//    {
//        Game::getInstance()->getPhysicsController()->drawDebug(_scene->getActiveCamera()->getViewProjectionMatrix());
//    }
    
    clear(Game::CLEAR_DEPTH, Vector4::zero(), 1, 0);
        
    // Draw FPS and speed
    int carSpeed = 0;//_carVehicle ? (int)_carVehicle->getSpeedKph() : 0;
    _font->start();
    char fps[32];
    sprintf(fps, "%d", getFrameRate());
    _font->drawText(fps, 5, 5, gameplay::Vector4(0,0.5f,1,1), 20);
    char kph[32];
    sprintf(kph, "%d [km/h]", carSpeed);
    _font->drawText(kph, getWidth() / 2 - 50, getHeight() - 60, gameplay::Vector4(1,1,1,1), 40);
    _font->finish();
}

bool RacerGame::buildRenderQueues(gameplay::Node* node)
{
    gameplay::Model* model = dynamic_cast<gameplay::Model*>(node->getDrawable());
    if (model)
    {
        // Perform view-frustum culling for this node
        if (__viewFrustumCulling && !node->getBoundingSphere().intersects(_scene->getActiveCamera()->getFrustum()))
            return true;

        // Determine which render queue to insert the node into
        std::vector<gameplay::Node*>* queue;
        if (node->hasTag("transparent"))
            queue = &_renderQueues[QUEUE_TRANSPARENT];
        else
            queue = &_renderQueues[QUEUE_OPAQUE];

        queue->push_back(node);
    }
    return true;
}

void RacerGame::drawScene()
{
    // Iterate through each render queue and draw the nodes in them
    for (unsigned int i = 0; i < QUEUE_COUNT; ++i)
    {
        std::vector<gameplay::Node*>& queue = _renderQueues[i];

        for (size_t j = 0, ncount = queue.size(); j < ncount; ++j)
        {
            queue[j]->getDrawable()->draw();
        }
    }
}

void RacerGame::drawSplash(void* param)
{
    clear(CLEAR_COLOR_DEPTH, gameplay::Vector4(0, 0, 0, 1), 1.0f, 0);

    gameplay::SpriteBatch* batch = gameplay::SpriteBatch::create("res/logo_powered_white.png");
    batch->start();
    batch->draw(this->getWidth() * 0.5f, this->getHeight() * 0.5f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f, 1.0f, 0.0f, gameplay::Vector4::one(), true);
    batch->finish();
    SAFE_DELETE(batch);
}

void RacerGame::keyEvent(gameplay::Keyboard::KeyEvent evt, int key)
{
    if (evt == gameplay::Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case gameplay::Keyboard::KEY_ESCAPE:
            menuEvent();
            break;
        case gameplay::Keyboard::KEY_A:
        case gameplay::Keyboard::KEY_CAPITAL_A:
        case gameplay::Keyboard::KEY_LEFT_ARROW:
            _keyFlags |= STEER_LEFT;
            break;
        case gameplay::Keyboard::KEY_D:
        case gameplay::Keyboard::KEY_CAPITAL_D:
        case gameplay::Keyboard::KEY_RIGHT_ARROW:
            _keyFlags |= STEER_RIGHT;
            break;
        case Keyboard::KEY_W:
        case Keyboard::KEY_CAPITAL_W:
        case Keyboard::KEY_UP_ARROW:
            _keyFlags |= ACCELERATOR;
            break;
        case Keyboard::KEY_S:
        case Keyboard::KEY_CAPITAL_S:
        case Keyboard::KEY_DOWN_ARROW:
            _keyFlags |= REVERSE;
            break;
        case Keyboard::KEY_SPACE:
            _keyFlags |= BRAKE;
            break;
        case Keyboard::KEY_Y:
        case Keyboard::KEY_CAPITAL_Y:
            _keyFlags |= UPRIGHT;
            break;
        case Keyboard::KEY_V:
            __viewFrustumCulling = !__viewFrustumCulling;
            break;
        case Keyboard::KEY_F:
            __flythruCamera = !__flythruCamera;
            getScriptController()->executeFunction<void>("toggleCamera");
            break;
        case Keyboard::KEY_B:
            __drawDebug = !__drawDebug;
            break;
        case Keyboard::KEY_J:
            __useAccelerometer = !__useAccelerometer;
            break;
        }
    }
    else if (evt == Keyboard::KEY_RELEASE)
    {
        switch (key)
        {
        case Keyboard::KEY_A:
        case Keyboard::KEY_CAPITAL_A:
        case Keyboard::KEY_LEFT_ARROW:
            _keyFlags &= ~STEER_LEFT;
            break;
        case Keyboard::KEY_D:
        case Keyboard::KEY_CAPITAL_D:
        case Keyboard::KEY_RIGHT_ARROW:
            _keyFlags &= ~STEER_RIGHT;
            break;
        case Keyboard::KEY_W:
        case Keyboard::KEY_CAPITAL_W:
        case Keyboard::KEY_UP_ARROW:
            _keyFlags &= ~ACCELERATOR;
            break;
        case Keyboard::KEY_S:
        case Keyboard::KEY_CAPITAL_S:
        case Keyboard::KEY_DOWN_ARROW:
            _keyFlags &= ~REVERSE;
            break;
        case Keyboard::KEY_SPACE:
            _keyFlags &= ~BRAKE;
            break;
        case Keyboard::KEY_Y:
        case Keyboard::KEY_CAPITAL_Y:
            _keyFlags &= ~UPRIGHT;
            break;
        }
    }
}

void RacerGame::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

bool RacerGame::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    bool consumed = false;
#if 0
    switch (evt)
    {
    case Mouse::MOUSE_PRESS_LEFT_BUTTON:
        _keyFlags |= ACCELERATOR_MOUSE;
        break;
    case Mouse::MOUSE_PRESS_RIGHT_BUTTON:
        _keyFlags |= BRAKE_MOUSE;
        break;
    case Mouse::MOUSE_RELEASE_LEFT_BUTTON:
        _keyFlags &= ~ACCELERATOR_MOUSE;
        break;
    case Mouse::MOUSE_RELEASE_RIGHT_BUTTON:
        _keyFlags &= ~BRAKE_MOUSE;
        break;
    }
#endif
    return consumed;
}

void RacerGame::gamepadEvent(Gamepad::GamepadEvent evt, Gamepad* gamepad)
{
    // Prioritise physical gamepads over the virtual one.
    switch(evt)
    {
        default:
            break;
    }
}

void RacerGame::menuEvent()
{

}

void RacerGame::resetToStart()
{
    Vector3 pos(-258, 1, 278);
    Quaternion rot(Vector3::unitY(), MATH_DEG_TO_RAD(143.201f));

    reset(pos, rot);
}

void RacerGame::resetInPlace()
{
}

void RacerGame::reset(const Vector3& pos, const Quaternion& rot)
{
}
