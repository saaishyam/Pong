#include <stdio.h>
#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include "raymath.h"
#include <stdlib.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))    
int main(){

    const int ScreenHeight = 720;
    const int screenWidth = 1080;
    const int player_start_height = 50;

    bool paused = false;
    int player_left_score = 0;
    int player_right_score = 0;


    Rectangle player_left_paddle = {
        .x = 0.05f * screenWidth,
        .y = 0.40f * ScreenHeight, 
        .width = 0.01f * screenWidth,
        .height = 0.2f * ScreenHeight,
    };

    Rectangle player_right_paddle = {
        .x =0.95f * screenWidth,
        .y = 0.40f * ScreenHeight,
        .width = 0.01f * screenWidth,
        .height = 0.2f * ScreenHeight,
    };

    Vector2 ball_position = {

        .x = 0.5 * screenWidth,
        .y = 0.5 * ScreenHeight,
    };

    float ball_radius = ScreenHeight * 0.02f;

    Vector2 ball_veloctity = {

        .x = -260.f,
        .y = 0.f,
    };
    
    InitWindow(screenWidth, ScreenHeight, "Pong");

    SetTargetFPS(60);

    while(!WindowShouldClose()){

        //Update
        if(IsKeyPressed(KEY_P)){
            paused = !paused;
        }

        if(!paused){
            
            //key handlers
            const float paddle_mod = 10.f;

            if(IsKeyDown(KEY_W)){
                player_left_paddle.y = max(player_start_height, player_left_paddle.y - paddle_mod); 
            }

            if(IsKeyDown(KEY_S)){
                player_left_paddle.y = min(ScreenHeight - player_left_paddle.height, player_left_paddle.y + paddle_mod);
            }

            // AI controls the right paddle
            const float ai_speed = 220.f;
            const float ai_dead_zone = 8.f;
            const float ai_target = ball_position.y - player_right_paddle.height * 0.5f;
            const float current_center = player_right_paddle.y + player_right_paddle.height * 0.5f;
            float ai_delta = ai_target - current_center;

            if(fabsf(ai_delta) < ai_dead_zone){
                ai_delta = 0.f;
            }

            if(ai_delta > ai_speed * GetFrameTime()){
                ai_delta = ai_speed * GetFrameTime();
            }
            else if(ai_delta < -ai_speed * GetFrameTime()){
                ai_delta = -ai_speed * GetFrameTime();
            }

            player_right_paddle.y += ai_delta;
            player_right_paddle.y = Clamp(player_right_paddle.y, player_start_height, ScreenHeight - player_right_paddle.height);
            
            const float dt = GetFrameTime();

            ball_position.x +=ball_veloctity.x * dt;
            ball_position.y += ball_veloctity.y * dt;

            //collisions

            if(ball_veloctity.x < 0){
                if(CheckCollisionCircleRec(ball_position,ball_radius,player_left_paddle)){

                    float hit_pos = (ball_position.y - player_left_paddle.y) / player_left_paddle.height;
                    hit_pos = Clamp(hit_pos, 0.f, 1.f);

                    const float a = (hit_pos - 0.5f) * PI/4;
                    const float speed = Vector2Length(ball_veloctity) * 1.05f;

                    ball_veloctity.x = speed * cosf(a);
                    ball_veloctity.y = speed * sinf(a);
                }
            }

            else{
                if(CheckCollisionCircleRec(ball_position,ball_radius,player_right_paddle)){

                    float hit_pos = (ball_position.y - player_right_paddle.y) / player_right_paddle.height;
                    hit_pos = Clamp(hit_pos, 0.f, 1.f);

                    const float a = PI - (hit_pos - 0.5f) * PI/4;
                    const float speed = Vector2Length(ball_veloctity) * 1.05f;

                    ball_veloctity.x = speed * cosf(a);
                    ball_veloctity.y = speed * sinf(a);
                
                }
            }

            //collision check with da wall :)

            if(ball_veloctity.y > 0){

                if(ball_position.y >= ScreenHeight - ball_radius){
                    ball_veloctity.y *= -1;
                    ball_position.y = ScreenHeight - ball_radius;
                }
            }
            else{
                if(ball_position.y <= player_start_height + ball_radius){

                    ball_veloctity.y *= -1;
                    ball_position.y = player_start_height + ball_radius;
                }
            }

            //score

            if(ball_position.x < 0){
                
                ++player_right_score;

                ball_veloctity.x =  -300.f;
                ball_position.x = 0.5f * screenWidth;
                ball_position.y = 0.5f * ScreenHeight;
            }
            else if(ball_position.x > screenWidth){

                ++player_left_score;

                ball_veloctity.x =  300.f;
                ball_position.x = 0.5f * screenWidth;
                ball_position.y = 0.5f * ScreenHeight;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        //render Game

        DrawRectangleRec(player_left_paddle,WHITE);
        DrawRectangleRec(player_right_paddle,WHITE);
        DrawCircleV(ball_position, ball_radius, WHITE);

        //render Header
        const int header_y = 17;
        const int fontsize = 30;
        const char * title = "pong";
        const int title_width = MeasureText(title, fontsize);

        DrawText("Pong",(screenWidth - title_width) / 2, header_y,fontsize, WHITE);

        char Buffer[4];

        sprintf(Buffer, "%d", player_left_score);
        DrawText(Buffer, (int)(0.1*screenWidth) , header_y, fontsize, WHITE);

        sprintf(Buffer, "%d", player_right_score);
        DrawText(Buffer, (int)(0.9*screenWidth) , header_y, fontsize, WHITE);


        DrawLine(0,player_start_height,screenWidth, player_start_height, WHITE);


        //render pause, if neccessary

        if(paused){
            const int paused_font_size = 40;
            const char* pauesd_text = "Paused";
            const int pause_width = MeasureText(pauesd_text, paused_font_size);
            const int p_x = (screenWidth - pause_width) / 2;

            const Rectangle rec = {
                .x = p_x - 10,
                .y = ScreenHeight /2 - 20,
                .width = pause_width + 20,
                .height = 40,
            };

            DrawRectangleRec(rec, WHITE);

            DrawText(pauesd_text, p_x, ScreenHeight / 2 - 20, paused_font_size, BLACK);

        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}