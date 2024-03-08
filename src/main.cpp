#include <include/raylib.h>
#include <imgui/imgui.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui_stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Assignment 1 Starter Code");
    
    //initialize the raylib ImGui backend
    rlImGuiSetup(true);
    //increase ImGui item size to 2x
    ImGui::GetStyle().ScaleAllSizes(2);

    // Set raylib to target 60 frames-per-second (this does not mean it will actually RUN at 60 fps)
    SetTargetFPS(60);               

    // General variables
    //--------------------------------------------------------------------------------------

    //shape properties to draw on the screen (circle for this example)
    //units of size and speed are in pixels
    float circRadius=50;
    float circSpeedX=1.0f;
    float circSpeedY=0.5f;
    bool drawCirc=true;
    float circX=50.0f;
    float circY=50.0f;
    float color[3] = {0.0f,0.0,1.0f}; //color is from 0-1

    //Let's draw some text to the screen too
    bool drawText=true;
    std::string strText= "Some Text";
    std::string newText= strText;

    //load a font (Raylib gives warning if font can't be found, then uses default as fallback)
    Font font = LoadFont("assets/Orbitron.ttf");

    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        //move circle
        circX+=circSpeedX;
        circY+=circSpeedY;

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            //********** Raylib Drawing Content **********

            //draw the cricle (center x, center y, radius, color(r,g,b,a))
            if(drawCirc){
                DrawCircle((int)circX, (int)circY, circRadius, ColorFromNormalized({ color[0],color[1],color[2],1.0f }));
            }
            
            //draw the text
            if(drawText){
                //get the size (x and y) of the text object
                //(font,c string, font size, font spaceing)
                Vector2 textSize= MeasureTextEx(font, strText.c_str(), 18, 1);

                //draw the text (using the text size to help draw it in the corner
                //(font,c string, vector2, font size, font spaceing, color)
                DrawTextEx(font, strText.c_str(), { 0.0f, screenHeight - textSize.y }, 18, 1, WHITE);
            }

            //********** ImGUI Content *********
            
            //Draw imgui stuff last so it is over the top of everything else
            rlImGuiBegin();

                //sets the next window to be at this position
                //also uses the imgui.ini that gets created at first run
                ImGui::SetNextWindowSize(ImVec2(350, 250));
                //creates a new window
                ImGui::Begin("My Window",NULL,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
                    ImGui::Text("The Window Text!");
                    //checkboxes, they directly modify the value (which is why we send a reference)
                    ImGui::Checkbox("Draw Cricle",&drawCirc);
                    ImGui::SameLine();
                    ImGui::Checkbox("Draw Text",&drawText);

                    //slider, again directly modifies the value and limites between 0 and 300 for this example
                    ImGui::SliderFloat("Radius",&circRadius,0.0f,300.0f);
                    
                    //color picker button, directly modifies the color (3 element float array)
                    ImGui::ColorEdit3("Circle Color",color);
                    
                    //text input field, directly modifies the string
                    ImGui::InputText("Text",&newText);
                    
                    //buttons, returns true if clicked on this frame
                    if(ImGui::Button("Set Text")){
                        strText=newText;
                    }

                    //The next item will be on the same line as the previous one
                    ImGui::SameLine();

                    //Another button
                    if(ImGui::Button("Reset Circle")){
                        circX=50.0;
                        circY=50.0;
                        circRadius=50;
                    }
                //ends this window
                ImGui::End();

                //show ImGui Demo Content if you want to see it
                //bool open = true;
                //ImGui::ShowDemoWindow(&open);

            // end ImGui Content
            rlImGuiEnd();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // Clean Up
    //--------------------------------------------------------------------------------------
    rlImGuiShutdown();    // Shuts down the raylib ImGui backend
    UnloadFont(font);     // Remove font from memory
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}