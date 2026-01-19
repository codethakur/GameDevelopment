#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>



class Audio
{
public:
    static void Init();
    static void Shutdown();

    // Called every frame
    static void Update(bool gameStarted, bool gameOver);

    // One-shot sounds
    static void PlayEat();

private:
    static void playStartMusic();
    static void playGameOverMusic();
    static void stopAllMusic();

    static Mix_Music* startMusic;
    static Mix_Music* gameOverMusic;
    static Mix_Chunk* eatSound;

    // Internal audio state
    enum class AudioState
    {
        None,
        StartScreen,
        Playing,
        GameOver
    };

    static AudioState currentState;
};
