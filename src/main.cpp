#include <include/raylib.h>
#include <imgui/imgui.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui_stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <random>
#include <sstream>

//Alonzo Morris, Game Engine Development, Assignment 1:

/*known bugs:
scale's default condition is at float 50.0, instead of 1.0
changing shape's velocity values to absolute maximum's and min's values causes several... physical anomolies

*/

//------------------------------------------------------------------------------------
// Define drawable, circle, and rectangle classes
//------------------------------------------------------------------------------------

//Drawable class
class Drawable {
public:
    std::string label;
    Color color;
    float scale;
    float velocityX, velocityY;
    float positionX, positionY;
    bool isActive, isVisible, displayLabel;

    Drawable(const std::string& label, Color color, float scale, float velocityX, float velocityY, float positionX, float positionY)
        : label(label), color(color), scale(scale), velocityX(velocityX), velocityY(velocityY), positionX(positionX), positionY(positionY), isActive(true), isVisible(true), displayLabel(true) {
        if (velocityX == 0 && velocityY == 0) {
            RandomizeDirection();
        }
    }
    virtual void Move(int screenWidth, int screenHeight) = 0;
    virtual void Render() const = 0;

    //using uniform dist method for randomized direction
    void RandomizeDirection() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        velocityX = dis(gen) * 5;
        velocityY = dis(gen) * 5;
    }
};

//circle class with drawable attr(s)
class CircleDrawable : public Drawable {
public:
    float radius;

    CircleDrawable(const std::string& label, Color color, float scale, float velocityX, float velocityY, float positionX, float positionY, float radius)
        : Drawable(label, color, scale, velocityX, velocityY, positionX, positionY), radius(radius) {}
    //move() method for movement implementation
    void Move(int screenWidth, int screenHeight) override {
        positionX += velocityX;
        positionY += velocityY;

        if (positionX - radius < 0 || positionX + radius > screenWidth) velocityX = -velocityX;
        if (positionY - radius < 0 || positionY + radius > screenHeight) velocityY = -velocityY;
    }
    //draw() method for displaying text over shapes
    void Render() const override {
        if (isVisible) {
            DrawCircle(static_cast<int>(positionX), static_cast<int>(positionY), radius, color);
            if (displayLabel) {
                int textWidth = MeasureText(label.c_str(), 20);
                DrawText(label.c_str(), static_cast<int>(positionX - textWidth / 2), static_cast<int>(positionY - 10), 20, WHITE);
            }
        }
    }
};

//rectangle class with drawable attr(s)
class RectangleDrawable : public Drawable {
public:
    float width, height;

    RectangleDrawable(const std::string& label, Color color, float scale, float velocityX, float velocityY, float positionX, float positionY, float width, float height)
        : Drawable(label, color, scale, velocityX, velocityY, positionX, positionY), width(width), height(height) {}
    //same implementation for rectangles instead
    void Move(int screenWidth, int screenHeight) override {
        positionX += velocityX;
        positionY += velocityY;

        if (positionX<0 || positionX + height > screenWidth) velocityX = -velocityX;
        if (positionY<0 || positionY + width > screenHeight) velocityY = -velocityY;
    }

    void Render() const override {
        if (isVisible) {
            DrawRectangle(static_cast<int>(positionX), static_cast<int>(positionY), static_cast<int>(height), static_cast<int>(width), color);
            if (displayLabel) {
                //Draws name at the center of the rectangle
                int textWidth = MeasureText(label.c_str(), 20);
                DrawText(label.c_str(), static_cast<int>(positionX + height / 2 - textWidth / 2), static_cast<int>(positionY + width / 2 - 10), 20, WHITE);
            }
        }
    }
};

//reads config and loads shape
std::vector<Drawable*> LoadDrawablesFromConfig(const std::string& filename) {
    std::vector<Drawable*> drawables;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) { //while loop with nested if else statements to determine attr(s) shape type, dimensions, vel, etc
        std::istringstream iss(line);
        std::string type, label;
        float positionX, positionY, velocityX, velocityY, colorR, colorG, colorB, dim1, dim2;
        iss >> type >> label >> positionX >> positionY >> velocityX >> velocityY >> colorR >> colorG >> colorB >> dim1;
        Color color = { static_cast<unsigned char>(colorR * 255), static_cast<unsigned char>(colorG * 255), static_cast<unsigned char>(colorB * 255), 255 };

        if (type == "Circle") {
            drawables.push_back(new CircleDrawable(label, color, 1.0f, velocityX, velocityY, positionX, positionY, dim1));
        }
        else if (type == "Rectangle") {
            iss >> dim2;
            drawables.push_back(new RectangleDrawable(label, color, 1.0f, velocityX, velocityY, positionX, positionY, dim1, dim2));
        }
    }

    return drawables;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Assignment 1");

    // Initialize the raylib ImGui backend
    rlImGuiSetup(true);
    // Increase ImGui item size to 2x
    ImGui::GetStyle().ScaleAllSizes(2);

    // Set raylib to target 60 frames-per-second (this does not mean it will actually RUN at 60 fps)
    SetTargetFPS(60);

    //General variables
    //--------------------------------------------------------------------------------------
    bool simulate = true;
    bool renderDrawables = true;
    bool renderText = true;

    //config and vector implementation
    std::vector<Drawable*> drawables = LoadDrawablesFromConfig("assets/input.txt");
    std::map<std::string, Drawable*> drawableMap;
    for (auto drawable : drawables) {
        drawableMap[drawable->label] = drawable;
    }

    std::vector<std::string> labels;
    for (const auto& drawable : drawables) {
        labels.push_back(drawable->label);
    }

    //Variable for the selected shape
    std::string selectedDrawableLabel = labels[0];
    Drawable* selectedDrawable = drawableMap[selectedDrawableLabel];

    //Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (simulate) {
            for (auto drawable : drawables) {
                if (!drawable->isActive) continue;
                drawable->Move(screenWidth, screenHeight);
            }
        }

        //Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        for (auto drawable : drawables) {
            if (drawable->isActive) drawable->Render();
        }

        //Start ImGui
        rlImGuiBegin();

        //Create ImGui window
        ImGui::Begin("Assignment 1 Controls");

        //Toggle for draw shapes
        if (ImGui::Checkbox("Draw Shapes##renderDrawables", &renderDrawables)) {
            for (auto& drawable : drawables) {
                drawable->isVisible = renderDrawables;
            }
        }
        //Toggle for draw text
        if (ImGui::Checkbox("Draw Text##renderText", &renderText)) {
            for (auto& drawable : drawables) {
                drawable->displayLabel = renderText;
            }
        }
        //Toggle for simulate
        ImGui::Checkbox("Simulate##simulate", &simulate);
        //shape select dropdown functionality
        if (ImGui::BeginCombo("Shape##shapeCombo", selectedDrawableLabel.c_str())) {
            for (auto& label : labels) {
                bool isSelected = (selectedDrawableLabel == label);
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    selectedDrawableLabel = label;
                    selectedDrawable = drawableMap[label];
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        //visibility checkbox
        ImGui::Checkbox("Active##active", &selectedDrawable->isVisible);

        //radius/dimension sliders
        if (dynamic_cast<CircleDrawable*>(selectedDrawable)) {
            ImGui::SliderFloat("Scale##radius", &dynamic_cast<CircleDrawable*>(selectedDrawable)->radius, 0.0f, 300.0f);
        }
        else if (dynamic_cast<RectangleDrawable*>(selectedDrawable)) {
            ImGui::SliderFloat("Width##width", &dynamic_cast<RectangleDrawable*>(selectedDrawable)->width, 0.0f, 300.0f);
            ImGui::SliderFloat("Height##height", &dynamic_cast<RectangleDrawable*>(selectedDrawable)->height, 0.0f, 300.0f);
        }

        //velocity sliders
        float itemWidth = ImGui::GetContentRegionAvail().x * 0.25f;
        ImGui::PushItemWidth(itemWidth);
        ImGui::SliderFloat("##velocityX", &selectedDrawable->velocityX, -30.0f, 30.0f);
        ImGui::SameLine();
        ImGui::SliderFloat("Velocity##velocity", &selectedDrawable->velocityY, -30.0f, 30.0f);
        ImGui::PopItemWidth();

        //3 option color editor
        float normalizedColor[3] = { selectedDrawable->color.r / 255.0f, selectedDrawable->color.g / 255.0f, selectedDrawable->color.b / 255.0f };
        ImGui::ColorEdit3("Color##drawableColor", normalizedColor);

        //normalized color updater
        selectedDrawable->color = Color{
            static_cast<unsigned char>(normalizedColor[0] * 255),
            static_cast<unsigned char>(normalizedColor[1] * 255),
            static_cast<unsigned char>(normalizedColor[2] * 255),
            255
        };

        //modifies text string
        ImGui::InputText("Name##textInput", &selectedDrawable->label);
        //new item will be on the previous line
        ImGui::SameLine();
        //closes window
        ImGui::End();
        //End ImGui Content
        rlImGuiEnd();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    //Cleans Up
    //--------------------------------------------------------------------------------------
    rlImGuiShutdown();
    CloseWindow();

    for (auto drawable : drawables) {
        delete drawable;
    }
    return 0;
}
