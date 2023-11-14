#pragma once
#include "color.h"
#include "uniform.h"
#include "fragment.h"
#include "FastNoiseLite.h"
#include <cmath>
#include <random>

Vertex vertexShader(const Vertex& vertex, const Uniform& uniforms) {

  glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

  glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

  glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

  glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
  transformedNormal = glm::normalize(transformedNormal);

  return Vertex{
    glm::vec3(screenVertex),
    transformedNormal,
    vertex.position
  };
};

float rand(glm::vec3 co) {
    return glm::fract(sin(glm::dot(co, glm::vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

Fragment fragmentShader(Fragment& fragment) {
  fragment.color = fragment.color * fragment.intensity;

  return fragment;
};

Fragment fragmentShader1(Fragment& fragment) {
   Color color;

    // Base color of the sun
    glm::vec3 tmpColor = glm::vec3(244.0f/255.0f, 140.0f/255.0f, 6.0f/255.0f) * glm::vec3(1.0f, 0.0f, 0.0f);

    // Introduce some pseudo-random noise into the equation for a more realistic look
    float noise = rand(fragment.original);

    // Affect the color intensity with the noise
    tmpColor += glm::vec3(noise, noise, noise) * 0.3f;

    // Add a slight gradient from the center to the edge to give a sense of depth
    float distanceFromCenter = glm::length(fragment.original);
    tmpColor *= 1.0f - distanceFromCenter;

    // Add an extra layer of red
    tmpColor += glm::vec3(0.3f * noise, 0.0f, 0.0f);

    // Convert tmpColor to color
    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    // Apply glow effect
    float glow = 4.0f + 1.0f * sin(3.0f * noise);
    fragment.color = color * fragment.intensity * glow;

    return fragment;
}

Fragment fragmentShaderjupiter(Fragment& fragment) {
    Color color;

    glm::vec3 baseColor = glm::vec3(232, 174, 104);

    float stripePattern = glm::abs(glm::cos(fragment.original.y * 15.0f)) * 70.0f;
    float stripePattern2 = glm::abs(glm::sin(fragment.original.y * 25.0f)) * 70.0f;
    

    glm::vec3 tmpColor = baseColor + stripePattern + stripePattern2;

    
    float noise = rand(fragment.original);
    tmpColor += noise * 0.1f;

    glm::vec3 gray = glm::vec3(255, 255, 255);

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 900.0f;
    float oy = 10.0f;
    float zoom = 300.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    // Define el centro y el radio de la Gran Mancha Roja
    glm::vec2 redSpotCenter = (glm::vec2(-0.50f, -0.4f));
    float redSpotRadius= 0.1f;

    // Comprueba si el fragmento está dentro de la Gran Mancha Roja
    if (glm::distance(uv, redSpotCenter) < redSpotRadius){
        tmpColor = glm::vec3(231, 173, 103);  // Asigna color rojo
    }
    else if (noiseValue > 0.9f) {
        tmpColor = gray;
    }

    color = Color(static_cast<int>(tmpColor.x), static_cast<int>(tmpColor.y), static_cast<int>(tmpColor.z));

    fragment.color = color * fragment.intensity;

    return fragment;
};

Fragment fragmentShadertierra(Fragment& fragment) {
    Color color;

    glm::vec3 groundColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 brownColor = glm::vec3(0.5f, 0.25f, 0.0f); // Color café

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    glm::vec3 tmpColor = (noiseValue < 0.5f) ? oceanColor : groundColor;

    // Agrega una capa de color café en algunas partes
    float oxb = 1200.0f;
    float oyb = 3000.0f;
    float zoomb = 200.0f;

    float noiseValueB = noiseGenerator.GetNoise((uv.x + oxb) * zoomb, (uv.y + oyb) * zoomb);

    float brownFactor = 0.2f; // Controla la intensidad del color café
    if (noiseValueB > 0.6f) {
        tmpColor = glm::mix(tmpColor, tmpColor + (brownColor * brownFactor), (noiseValueB - 0.6f) * 5.0f);
    }

    float oxc = 5500.0f;
    float oyc = 6900.0f;
    float zoomc = 400.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    if (noiseValueC > 0.5f) {
        tmpColor = glm::mix(tmpColor, cloudColor, (noiseValueC - 0.5f) * 2.0f);
    }

    // Añadir efecto de atenuación atmosférica para simular la atmósfera terrestre
    float atmosphereFactor = 0.2f;
    tmpColor = glm::mix(tmpColor, glm::vec3(0.0f, 0.0f, 0.0f), fragment.intensity * atmosphereFactor);

    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color;

    return fragment;
}

Fragment fragmentShaderextra1(Fragment& fragment) {
    Color color;

    glm::vec3 baseColor = glm::vec3(0, 119, 182); // Color azul característico de Neptuno

    // Añadir patrones de rayas
    float stripePattern1 = glm::abs(glm::cos(fragment.original.y * 4.0f)) * 50.0f;

    glm::vec3 tmpColor = baseColor + glm::vec3(stripePattern1);

    // Agregar ruido para textura
    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 900.0f;
    float oy = 10.0f;
    float zoom = 300.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    // Añadir ruido para mayor realismo
    tmpColor += glm::vec3(noiseValue * 20.0f, noiseValue * 20.0f, noiseValue * 10.0f);

    // Ajustar la intensidad para obtener un aspecto más suave
    tmpColor /= 255.0f;

    color = Color(static_cast<int>(tmpColor.x * 255), static_cast<int>(tmpColor.y * 255), static_cast<int>(tmpColor.z * 255));

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment fragmentShaderextra2(Fragment& fragment) {
     Color color;

    glm::vec3 baseColor = glm::vec3(0.3f, 0.5f, 0.8f); // Color base del planeta
    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // Capa de textura rojiza para áreas elevadas
    float ox1 = 1200.0f;
    float oy1 = 3000.0f;
    float zoom1 = 200.0f;
    float noiseValue1 = noiseGenerator.GetNoise((uv.x + ox1) * zoom1, (uv.y + oy1) * zoom1);
    glm::vec3 redLayer = glm::vec3(1.0f, 0.3f, 0.1f) * noiseValue1;

    // Capa de textura amarilla para áreas con vegetación
    float ox2 = 800.0f;
    float oy2 = 500.0f;
    float zoom2 = 150.0f;
    float noiseValue2 = noiseGenerator.GetNoise((uv.x + ox2) * zoom2, (uv.y + oy2) * zoom2);
    glm::vec3 yellowLayer = glm::vec3(1.0f, 1.0f, 0.0f) * noiseValue2;

    // Capa de textura verde para áreas con agua
    float ox3 = 2000.0f;
    float oy3 = 1000.0f;
    float zoom3 = 100.0f;
    float noiseValue3 = noiseGenerator.GetNoise((uv.x + ox3) * zoom3, (uv.y + oy3) * zoom3);
    glm::vec3 greenLayer = glm::vec3(0.0f, 0.8f, 0.2f) * noiseValue3;

    // Capa de textura blanca para áreas de nieve o hielo
    float ox4 = 3000.0f;
    float oy4 = 2500.0f;
    float zoom4 = 120.0f;
    float noiseValue4 = noiseGenerator.GetNoise((uv.x + ox4) * zoom4, (uv.y + oy4) * zoom4);
    glm::vec3 whiteLayer = glm::vec3(0.5f, 0.5f, 0.5f) * noiseValue4;

    // Añade todas las capas de textura y variación de color
    glm::vec3 finalColor = baseColor + redLayer + yellowLayer + greenLayer + whiteLayer;

    // Añade un efecto de atmósfera
    float atmosphereFactor = 0.3f; // Controla la intensidad de la atmósfera
    finalColor = glm::mix(finalColor, glm::vec3(0.1f, 0.1f, 0.1f), fragment.intensity * atmosphereFactor);

    color = Color(static_cast<int>(finalColor.x * 255), static_cast<int>(finalColor.y * 255), static_cast<int>(finalColor.z * 255));

    fragment.color = color;

    return fragment;
}