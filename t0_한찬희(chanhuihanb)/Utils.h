#pragma once

// 1. Define the triangle vertices
struct FVertexSimple
{
    float x, y, z;    // Position
    float r, g, b, a; // Color
};

// Structure for a 3D vector
struct FVector3
{
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
};