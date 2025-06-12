#include "raylib.h"

// establish AnimData vals
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

// update running time and animframe
AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }      
    }
    return data;
}

int main()
{
    // window dimention
    double windowDimentions[2];
    windowDimentions[0] = 512;
    windowDimentions[1] = 380;
    InitWindow(windowDimentions[0], windowDimentions[1], "Dapper-Dasher by devIcelatte");

    // draw level background
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX{};
    float mgX{};
    float fgX{};

    // player vars
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData
    {
        {0.0, 0.0, scarfy.width/6, scarfy.width/6},
        {windowDimentions[0]/2 - scarfyData.rec.width/2, windowDimentions[1] - scarfyData.rec.height},
        0,
        1.0/12.0,
        0
    };

    // hazard vars
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
  
    const int sizeOfNebulae{7};
    AnimData nebulae[sizeOfNebulae]{};
    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowDimentions[0] + i *300;   
        nebulae[i].pos.y = windowDimentions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/16.0;
    }

    // set win point
    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

    // physics
    int velocity{0};
    int nebVel{-200}; // nebula x velocity (p/s)
    const int jumpVelocity{-600}; // (p/s)/s
    const int gravity{1'000}; // acceleration due to gravity (p/s)/s

    bool isInAir{};
    bool collision{};

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        // time since last frame
        const float dT{GetFrameTime()};
        
        BeginDrawing();
        ClearBackground(WHITE);

        // scrolling background
        bgX -= 20 * dT;
        if (bgX <= -background.width*2)
        {
            bgX = 0.0;
        }

        // scrolling midground
        mgX -= 40 * dT;
        if (mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }

        // scrolling foreground
        fgX -= 80 * dT;
        if (fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }

        // draw background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // draw midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width *2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // draw foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width *2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        // check ground
        if (isOnGround(scarfyData, windowDimentions[1]))
        {
            // on ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // in air
            velocity += gravity * dT;
            isInAir = true;
        }
        
        // jump mechanic
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVelocity;
        }

        // update each nebula position
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebVel *dT;
        }

        // update finish line
        finishLine += nebVel * dT;

        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        // update scarfy animation frame
        if (!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        // update nebulae animation frame
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{nebula.pos.x + pad, nebula.pos.y + pad, nebula.rec.width - 2 * pad, nebula.rec.height - 2 * pad};
            Rectangle scarfyRec{scarfyData.pos.x, scarfyData.pos.y, scarfyData.rec.width, scarfyData.rec.height};
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if(collision)
        {
            // lose condition reached
            DrawText("Game Over!", windowDimentions[0]/4, windowDimentions[1]/2, 40, RED);
        }
        else if(scarfyData.pos.x >= finishLine)
        {
            // win condition reached
            DrawText("You Win!", windowDimentions[0]/4, windowDimentions[1]/2, 40, RED);
        }
        else
        {
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
            
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebulas
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);    
            }
        }
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}