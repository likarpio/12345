#include <iostream>
#include "math.h"

#define M_PI 3.14159265


bool Math::WorldToScreen(Vector3 pos, Vector2& screen, std::array<float, 16> viewMatrix, int screenWidth, int screenHeight)
{
    float x = viewMatrix[0] * pos.x + viewMatrix[4] * pos.y + viewMatrix[8] * pos.z + viewMatrix[12];
    float y = viewMatrix[1] * pos.x + viewMatrix[5] * pos.y + viewMatrix[9] * pos.z + viewMatrix[13];
    float w = viewMatrix[3] * pos.x + viewMatrix[7] * pos.y + viewMatrix[11] * pos.z + viewMatrix[15];


    if (w < 0.01f)
        return false;

    float invW = 1.0f / w;
    screen.x = (screenWidth / 2.0f) + (x * invW * screenWidth / 2.0f);
    screen.y = (screenHeight / 2.0f) - (y * invW * screenHeight / 2.0f);

    return true;
}

float Math::DistanceTo(const Vector3& entPos, Vector3 locPlayerPos) {
    return sqrtf(
        (locPlayerPos.x - entPos.x) * (locPlayerPos.x - entPos.x) +
        (locPlayerPos.y - entPos.y) * (locPlayerPos.y - entPos.y) +
        (locPlayerPos.z - entPos.z) * (locPlayerPos.z - entPos.z));
}

bool Math::calcViewAngles(float& yaw, float& pitch, Vector3 locPlayerPos, Vector3 entPos)
{
    //calc pitch
    float delta_height = entPos.z - locPlayerPos.z;
    float distance_c = DistanceTo(entPos, locPlayerPos);
   
    pitch = asinf(delta_height / distance_c); // radians
    pitch = pitch * (180 / M_PI); // in degrees

    //calc yaw
    float delta_y = locPlayerPos.y - entPos.y;
    float delta_x = locPlayerPos.x - entPos.x;


    yaw = atan2(delta_y, delta_x); //radians
    yaw = yaw * (180 / M_PI); // in degrees (-180 to 180) cuz of atan2
    yaw -= 90; // -90 to adapt to AC (0 to 360)

    return true;
}



bool Math::getRectPos(Vector3 locPlayerPos, Vector3 entPos, Vector2 screen, Vector2& start, Vector2& end)
{
    // Distance vom Spieler zum Gegner (Kamera zum Zielpunkt)
    float distance = DistanceTo(entPos, locPlayerPos); // musst du definieren
    if (distance <= 0.0f) distance = 0.01f; // vermeidet Division durch 0

    // Größe abhängig von Entfernung
    float scale = 100.0f / distance; // passt diesen Wert an für bessere Skalierung

    float height = scale * 50.0f;    // Basisgröße mal scale
    float width = height / 2.4f;

    //screen.x,y = (0 | 0) ist links oben ecke

    start.x = screen.x - (width / 2);
    start.y = screen.y;

    end.x = start.x + width;
    end.y = screen.y + height;

    return true;
}
