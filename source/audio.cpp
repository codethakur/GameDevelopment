#include "../sounds/audio.h"
#include <iostream>

#include "audio.h"
#include <iostream>

Mix_Music* Audio::startMusic = nullptr;
Mix_Music* Audio::gameOverMusic = nullptr;
Mix_Chunk* Audio::eatSound = nullptr;

Audio::AudioState Audio::currentState = Audio::AudioState::None;

void Audio::Init()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer init failed\n";
        return;
    }

    startMusic    = Mix_LoadMUS("sounds/gameStart.mp3");
    gameOverMusic = Mix_LoadMUS("sounds/gameOver.mp3");
    eatSound      = Mix_LoadWAV("sounds/fruit.wav");
}

void Audio::Shutdown()
{
    stopAllMusic();

    Mix_FreeMusic(startMusic);
    Mix_FreeMusic(gameOverMusic);
    Mix_FreeChunk(eatSound);

    Mix_CloseAudio();
}

void Audio::Update(bool gameStarted, bool gameOver)
{
    AudioState nextState;

    if (!gameStarted && !gameOver)
        nextState = AudioState::StartScreen;
    else if (gameOver)
        nextState = AudioState::GameOver;
    else
        nextState = AudioState::Playing;

    if (nextState == currentState)
        return; // no transition → no audio change

    // Handle transitions
    stopAllMusic();

    switch (nextState)
    {
        case AudioState::StartScreen:
            playStartMusic();
            break;

        case AudioState::GameOver:
            playGameOverMusic();
            break;

        case AudioState::Playing:
            // No background music during gameplay
            break;

        default:
            break;
    }

    currentState = nextState;
}

void Audio::PlayEat()
{
    if (eatSound)
        Mix_PlayChannel(-1, eatSound, 0);
}

void Audio::playStartMusic()
{
    if (startMusic)
        Mix_PlayMusic(startMusic, -1); // loop until game starts
}

void Audio::playGameOverMusic()
{
    if (gameOverMusic)
        Mix_PlayMusic(gameOverMusic, -1); // loop until restart
}

void Audio::stopAllMusic()
{
    Mix_HaltMusic();
}
