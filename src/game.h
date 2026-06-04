#pragma once
/**
 * Game logic and rendering implementation.
 */
#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "controller.h"
#include "board.h"
#include "candy.h"
#include <string>

struct T_SOUND;

 /**
  * Main game class: keep track of the game state it.
  * When run_graphic_game() is called, the game loop will call
  * update() and render() every frame, in that order.
  */
class Game
{
public:
    Game();

    ~Game();

    /// Run the game loop
    void run();

    /**
     * Update the game state. Called every frame when run().
     * This part is not expected to do any rendering.
     *
     * @param controller the Controller to use for input handling.
     */
    void update(const Controller& controller);

    /**
     * Draw the next frame. Called once per frame, after update().
     *
     * @param graphics the GraphicManager to use for rendering.
     */
    void render(GraphicManager& graphics);

    /**
     * Serialize and write the current game state (board and falling block) to a file.
     * @param output_path path where to save the game state.
     * @return true if the dump was successful, false otherwise.
     */
    bool dump(const std::string& output_path) const;

    /**
     * Load a serialized game state (board and falling block) from a file created with dump().
     * @param input_path path from which to load the game state.
     * @return true if the load was successful, false otherwise.
     */
    bool load(const std::string& input_path);

    /// @return true if this game is equal to the other game (same board state and falling block)
    bool operator==(const Game& other) const;

private:
    static const int BLOCK_SIZE = 3;
    static const int SPAWN_COLUMN = 5;      // 6a columna (empezando en 0)
    static const int FRAMES_PER_DROP = 60;  // baja 1 vez por segundo 60 fps
    static const int DIFFICULTY = 200;      // cuanto mas cerca de 0 mayor la difficultad

    Board* m_board;
    Candy* m_block[BLOCK_SIZE];  // 0 = arriba, 2 = abajo; nullptr si no hay
    int m_blockX;                // columna del bloque
    int m_blockY;                // fila del caramelo de ARRIBA (puede ser negativa)
    int m_frameCounter;
    bool m_gameOver;
    int m_score;
    bool hardMode;
    bool v_Pressed_After_D_Rank;

    //Sonido (rangos + OST)
    bool m_soundReady = false;
    int  m_lastRank = 0;     // 0 = sin rango, 1..7 = D,C,B,A,S,SS,SSS
    int  m_lastOst = -1;    // 0 = Devil, 1 = Silver, 2 = Bury
    T_SOUND* m_rankSounds[8] = { nullptr };  // usamos índices 1..7
    T_SOUND* m_ostDevil = nullptr;
    T_SOUND* m_ostSilver = nullptr;
    T_SOUND* m_ostBury = nullptr;
    T_SOUND* m_sndExplode = nullptr;
    T_SOUND* m_sndJackpot = nullptr;
    T_SOUND* m_sndGameOver = nullptr;

    // Helpers
    void spawnBlock();
    void freeBlock();
    bool canMoveTo(int newX, int newY) const;  
    void rotateBlock();
    void landBlock();
    void resolveExplosions();

    void initSound();
    int  rankFromScore(int score) const;
    void updateSoundForScore();
    void drawRank(GraphicManager& graphics, int x, int y) const;
    void drawEasterEgg(GraphicManager& graphics, int x, int y) const;
};
#endif
