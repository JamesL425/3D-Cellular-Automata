#include <iostream>
#include <array>
#include "math.h"
#include <algorithm>
#include "raylib.h"
#include <random>

// ruleset total neighbours 9 + 8 + 9 = 26
static const int rows = 100;

static const std::array<bool, 27> alive = {
    false,
    false,
    false,
    false,
    false,
    true,
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

static const std::array<bool, 27> dead = {
    false,
    false,
    false,
    false,
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

int count_neighbours(std::array<std::array<std::array<bool, rows>, rows>, rows>& copy_grid, int row,
    int col, int lay) {
        int total = 0;

        for (int row_offset = -1; row_offset <= 1; ++row_offset) {
            for (int col_offset = -1; col_offset <=1; ++col_offset) {
                for (int lay_offset = -1; lay_offset <= 1; ++ lay_offset) {
                    if (row_offset == 0 && col_offset == 0 && lay_offset == 0) {
                        continue;
                    }

                    // total += (copy_grid[(row + row_offset) % rows]
                    //                     [(col + col_offset) % rows]
                    //                     [(lay + lay_offset) % rows]) ? 1 : 0;

                    if (row + row_offset < 0 || row + row_offset >= rows ||
                    col + col_offset < 0 || col + col_offset >= rows ||
                    lay + lay_offset < 0 || lay + lay_offset >= rows) {
                        continue;
                    }

                    total += (copy_grid[(row + row_offset)]
                                        [(col + col_offset)]
                                        [(lay + lay_offset)]) ? 1 : 0;
                }
            }
        }

        return total;
    }

bool apply_alive(std::array<std::array<std::array<bool, rows>, rows>, rows>& copy_grid, int row,
    int col, int lay) {
        return alive[count_neighbours(copy_grid, row, col, lay)];
    }

bool apply_dead(std::array<std::array<std::array<bool, rows>, rows>, rows>& copy_grid, int row,
    int col, int lay) {
        return dead[count_neighbours(copy_grid, row, col, lay)];
    }

void apply_ruleset(std::array<std::array<std::array<bool, rows>, rows>, rows>& grid) {
    std::array<std::array<std::array<bool, rows>, rows>, rows> copy_grid;
    std::copy(grid.begin(), grid.end(), copy_grid.begin());

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < rows; ++col) {
            for (int lay = 0; lay < rows; ++lay) {
                if (copy_grid[row][col][lay]) {
                    grid[row][col][lay] = apply_alive(copy_grid, row, col, lay);
                }
                else {
                    grid[row][col][lay] = apply_dead(copy_grid, row, col, lay);
                }
            }
        }
    }
}

int main() {
    std::array<std::array<std::array<bool, rows>, rows>, rows> grid{};

    // for (int i = 4; i < 7; ++i) {
    //     grid[4][4][i] = 1;
    //     grid[4][6][i] = 1;
    //     grid[6][4][i] = 1;
    //     grid[6][6][i] = 1;
    // }

    // for (int i = 4; i < 7; ++i) {
    //     for (int j = 4; j < 7; ++j) {
    //         for (int k = 4; k < 7; ++k) {
    //             grid[i][j][k] = 1;
    //         }
    //     }
    // }

    auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < rows; ++col) {
            for (int lay = 0; lay < rows; ++lay) {
                grid[row][col][lay] = gen();
            }
        }
    }


    const int width = 1000;
    const int height = 1000;
    const int block_size = 1;

    InitWindow(width, height, "3d-automata");

    float pos = (float) rows * (float) block_size;

    Camera3D camera = { 0 };
    camera.position = (Vector3){ pos * 1.5f, pos * 1.5f, pos * 1.5f };
    camera.target = (Vector3){ pos / 2, pos / 2, pos / 2 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        BeginMode3D(camera);
        ClearBackground(RAYWHITE);

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < rows; ++col) {
                for (int lay = 0; lay < rows; ++lay) {
                    if (!grid[row][col][lay]) {
                        continue;
                    }

                    Vector3 cube_pos = {
                        (float) row * block_size,
                        (float) col * block_size,
                        (float) lay * block_size
                    };

                    Color cube_colour = ColorFromHSV((float)(((
                        row + col + lay) * 18) % 360), 0.75f, 0.9f);
                    
                    DrawCube(cube_pos, block_size, block_size, block_size, cube_colour);
                }
            }
        }

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
        apply_ruleset(grid);
    }

    CloseWindow();
    return 0;
}
