#ifndef TEMPLATEGAME_H_
#define TEMPLATEGAME_H_

#include "gameplay.h"

//using namespace gameplay;

/**
 * Main game class.
 */
class RacerGame: public gameplay::Game, gameplay::Control::Listener
{
public:

    /**
     * Constructor.
     */
    RacerGame();

    /**
     * @see Game::keyEvent
     */
    void keyEvent(gameplay::Keyboard::KeyEvent evt, int key);

    /**
     * @see Game::touchEvent
     */
    void touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    /**
     * @see Game::mouseEvent
     */
    bool mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, int wheelDelta);

    /** 
     * @see Game::gamepadEvent
     */
    void gamepadEvent(gameplay::Gamepad::GamepadEvent evt, gameplay::Gamepad* gamepad);

    /**
     * @see Game::menuEvent
     */
    void menuEvent();

    /**
     * @see Control::controlEvent
     */
    void controlEvent(gameplay::Control* control, gameplay::Control::Listener::EventType evt);
    
protected:

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);

private:

    /**
     * Initializes the scene.
     */
    bool initializeScene(gameplay::Node* node);

    /**
     * Visits the scene to build render queues for a single frame.
     */
    bool buildRenderQueues(gameplay::Node* node);

    /**
     * Draws the scene.
     */
    void drawScene();

    /**
     * Draws the splash screen.
     */
    void drawSplash(void* param);

    /**
     * Reset vehicle to its initial state.
     */
    void resetToStart();

    /**
     * Upright vehicle at its current location.
     */
    void resetInPlace();

    /**
     * Generic helper function for resets.
     *
     * @param pos desired position.
     * @param rot desired rotation.
     */
    void reset(const gameplay::Vector3& pos, const gameplay::Quaternion& rot);

    /**
     * Indicates that the vehicle may be over-turned.
     */
    bool isUpset() const;

    gameplay::Scene* _scene;
    gameplay::Font* _font;
    gameplay::Form* _menu;
    gameplay::Form* _overlay;
    std::vector<gameplay::Node*> _renderQueues[2];
    unsigned int _keyFlags;
    unsigned int _mouseFlags;
    float _steering;
    gameplay::Gamepad* _gamepad;
    gameplay::Gamepad* _physicalGamepad;
    gameplay::Gamepad* _virtualGamepad;
    gameplay::AnimationClip* _virtualGamepadClip;
    gameplay::PhysicsVehicle* _carVehicle;
    float _upsetTimer;

    // Music and Sounds
    gameplay::AudioSource* _backgroundMusic;
    gameplay::AudioSource* _engineSound;
    gameplay::AudioSource* _brakingSound;
};

#endif
