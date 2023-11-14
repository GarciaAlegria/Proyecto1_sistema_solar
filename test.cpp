#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <fstream>
#include <sstream>
#include <string>

#include "fragment.h"
#include "uniform.h"
#include "color.h"
#include "FastNoiseLite.h"
#include "shaders.h"
#include "triangle.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

std::array<std::array<float, WINDOW_WIDTH>, WINDOW_HEIGHT> zbuffer;


SDL_Renderer* renderer;

// Declare a global clearColor of type Color
Color clearColor = {0, 0, 0}; // Initially set to black

// Declare a global currentColor of type Color
Color currentColor = {255, 255, 255}; // Initially set to white

// Function to clear the framebuffer with the clearColor
void clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);


    for (auto &row : zbuffer) {
        std::fill(row.begin(), row.end(), 99999.0f);
    }
}



// Function to set a specific pixel in the framebuffer to the currentColor
void point(Fragment f) {
    if (f.z < zbuffer[f.y][f.x]) {
        SDL_SetRenderDrawColor(renderer, f.color.r, f.color.g, f.color.b, f.color.a);
        SDL_RenderDrawPoint(renderer, f.x, f.y);
        zbuffer[f.y][f.x] = f.z;
    }
}

std::vector<std::vector<Vertex>> primitiveAssembly(const std::vector<Vertex>& transformedVertices) {
    std::vector<std::vector<Vertex>> groupedVertices;

    for (int i = 0; i < transformedVertices.size(); i += 3) {
        std::vector<Vertex> vertexGroup;
        vertexGroup.push_back(transformedVertices[i]);
        vertexGroup.push_back(transformedVertices[i+1]);
        vertexGroup.push_back(transformedVertices[i+2]);
        
        groupedVertices.push_back(vertexGroup);
    }

    return groupedVertices;
}


void render(std::vector<glm::vec3> VBO, const Uniform& uniforms, const int shader) {
    // 1. Vertex Shader
    // vertex -> transformedVertices
    
    std::vector<Vertex> transformedVertices(VBO.size() / 2);

    for (int i = 0; i < VBO.size(); i+=2) {
        glm::vec3 v = VBO[i];
        glm::vec3 u = VBO[i+1];

        Vertex vertex = {v, u};
        Vertex Vertexshaded = vertexShader(vertex, uniforms);
        transformedVertices.push_back(Vertexshaded);
    }


    // 2. Primitive Assembly
    // transformedVertices -> triangles
    std::vector<std::vector<Vertex>> triangles = primitiveAssembly(transformedVertices);

    // 3. Rasterize
    // triangles -> Fragments
    std::vector<Fragment> fragments;
    for (const std::vector<Vertex>& triangleVertices : triangles) {
        std::vector<Fragment> rasterizedTriangle = triangle(
            triangleVertices[0],
            triangleVertices[1],
            triangleVertices[2],
            WINDOW_WIDTH,
            WINDOW_HEIGHT
        );
        
        fragments.insert(
            fragments.end(),
            rasterizedTriangle.begin(),
            rasterizedTriangle.end()
        );
    }

    // 4. Fragment Shader
    // Fragments -> colors

    for (Fragment fragment : fragments) {
        if (shader == 0) {
            point(fragmentShader(fragment));
        } else if (shader == 1) {
            point(fragmentShader1(fragment));
        } else if (shader == 2) {
            point(fragmentShaderjupiter(fragment));
        } else if (shader == 3) {
            point(fragmentShadertierra(fragment));
        } else if (shader == 4) {
            point(fragmentShaderextra1(fragment));
        } else if (shader == 5) {
            point(fragmentShaderextra2(fragment));
        }
    }
}


float a = 3.14f / 3.0f;
float x = 0.0f;
float y = 0.0f;
float z = -5.0f;
float xNave = 0.0f;
float yNave = -0.5f;
float zNave = 4.0f;
float cameraRotationX = 0.0f;
float cameraRotationY = 0.0f;
float cameraRotationZ = 0.0f;

glm::mat4 createModelMatrix() {
     // Calculate the camera direction based on rotation angles
    glm::vec3 cameraDirection = glm::vec3(
            glm::cos(cameraRotationY) * glm::sin(cameraRotationX),
            glm::sin(cameraRotationY),
            glm::cos(cameraRotationY) * glm::cos(cameraRotationX)
    );

    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(xNave + cameraDirection.x, yNave + cameraDirection.y, zNave + cameraDirection.z));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 0.2f));

    return translation * scale;
}

glm::mat4 createModelMatrix1() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.9f, 0.9f, 0.9f));
    a += 3;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(a), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}


glm::mat4 createModelMatrix2() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.7f, 0.7f, 0.7f));
    a += 3.0f;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(a), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}
float b = 3.14f / 3.0f;
glm::mat4 createModelMatrix3() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 0.5f));
    b += 10.0f;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}
 float c = 3.14f / 3.0f;
glm::mat4 createModelMatrix4() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.3f, 0.3f, 0.3f));
    c += 5.0f;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(c), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}
 float d = 3.14f / 3.0f;
glm::mat4 createModelMatrix5() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(0.9f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 0.2f));
    d += 1.0f;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(d), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}

glm::mat4 createViewMatrix() {

    // Calculate the camera direction based on rotation angles
    glm::vec3 cameraDirection = glm::vec3(
            glm::cos(cameraRotationY) * glm::sin(cameraRotationX),
            glm::sin(cameraRotationY),
            glm::cos(cameraRotationY) * glm::cos(cameraRotationX)
    );

    return glm::lookAt(
        // donde esta
        glm::vec3(x, y, z),
        // hacia adonde mira
        glm::vec3(x, y, 0),
        // arriba
        glm::vec3(0, -20, 0)
    );
}

glm::mat4 createProjectionMatrix() {
  float fovInDegrees = 45.0f;
  float aspectRatio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
  float nearClip = 0.1f;
  float farClip = 100.0f;

  return glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);
}

glm::mat4 createViewportMatrix() {
    // Crear la matriz de viewport
    glm::mat4 viewportMatrix = glm::mat4(1.0f);
    viewportMatrix = glm::scale(viewportMatrix, glm::vec3(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 1.0f));
    viewportMatrix = glm::translate(viewportMatrix, glm::vec3(1.0f, 1.0f, 0.0f));

    return viewportMatrix;
}

struct Face {
    std::array<int, 3> vertexIndices;
    std::array<int, 3> normalIndices;
};

bool loadOBJ(const std::string& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_textures, std::vector<glm::vec3>& out_normals, std::vector<Face>& out_faces)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "Failed to open the file: " << path << std::endl;
        return false;
    }

    std::string line;
    std::istringstream iss;
    std::string lineHeader;
    glm::vec3 vertex;
    Face face;

    while (std::getline(file, line))
    {
        iss.clear();
        iss.str(line);
        iss >> lineHeader;

        if (lineHeader == "v")
        {
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_vertices.push_back(vertex);
        }
        else if (lineHeader == "vn")
        {
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_normals.push_back(vertex);
        }
        else if (lineHeader == "vt")
        {
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_textures.push_back(vertex);
        }
        else if (lineHeader == "f")
        {
            Face face;
            for (int i = 0; i < 3; ++i)
            {
                std::string faceData;
                iss >> faceData;
                
                std::replace(faceData.begin(), faceData.end(), '/', ' ');

                std::istringstream faceDataIss(faceData);
                int temp;
                faceDataIss >> face.vertexIndices[i] >> temp >> face.normalIndices[i];

                face.vertexIndices[i]--;
                face.normalIndices[i]--;
            }

            out_faces.push_back(face);
        }
    }

    return true;
}

// Function that prints the contents of a vector of glm::vec3
auto printVec3Vector = [](const std::vector<glm::vec3>& vec) {
    for (const auto& v : vec) {
        std::cout << v << std::endl;
    }
};

void paintSpace(int ox, int oy) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            FastNoiseLite noiseGenerator;
            noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

            float scale = 1000.0f;
            float noiseValue = noiseGenerator.GetNoise((x + (ox * 100.0f)) * scale, (y + oy * 100.0f) * scale);

            // If the noise value is above a threshold, draw a star
            if (noiseValue > 0.97f) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
                SDL_RenderDrawPoint(renderer, x, y);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

}

glm::vec3 modelPositionSphere(-1.0f, 0.0f, 0.0f);  // Posición inicial de la esfera


int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(".obj Renderer", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    bool running = true;
    SDL_Event event;

    std::vector<glm::vec3> verticesNave;
    std::vector<glm::vec3> texturesNave;
    std::vector<glm::vec3> normalsNave;
    std::vector<Face> facesNave;
    std::vector<glm::vec3> vertexBufferObjectNave;

    if (loadOBJ("navecita.obj", verticesNave, texturesNave, normalsNave, facesNave)) {
        // For each face
        for (const auto& face : facesNave)
        {
            for (int i = 0; i < 3; ++i)
            {
                glm::vec3 vertexPosition = verticesNave[face.vertexIndices[i]];
                glm::vec3 vertexNormal = normalsNave[face.normalIndices[i]];

                vertexBufferObjectNave.push_back(vertexPosition);
                vertexBufferObjectNave.push_back(vertexNormal);
            }
        }
    }

    std::vector<glm::vec3> verticesPlaneta;
    std::vector<glm::vec3> texturesPlaneta;
    std::vector<glm::vec3> normalsPlaneta;
    std::vector<Face> facesPlaneta;
    std::vector<glm::vec3> vertexBufferObjectPlaneta;

if (loadOBJ("sphere.obj", verticesPlaneta, texturesPlaneta, normalsPlaneta, facesPlaneta)) {
        // For each face
        for (const auto& face : facesPlaneta)
        {
            for (int i = 0; i < 3; ++i)
            {
                glm::vec3 vertexPosition = verticesPlaneta[face.vertexIndices[i]];
                glm::vec3 vertexNormal = normalsPlaneta[face.normalIndices[i]];

                vertexBufferObjectPlaneta.push_back(vertexPosition);
                vertexBufferObjectPlaneta.push_back(vertexNormal);
            }
        }
    }

    Uniform uniforms;
    Uniform uniformsPlaneta;
    Uniform uniformsjupiter;
    Uniform uniformstierra;
    Uniform uniformsextra1;
    Uniform uniformsextra2;

    Uint32 frameStart, frameTime;
    std::string title = "FPS: ";
    int speed = 10;

    while (running) {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // sdl event in which we can move the object
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        x -= 1.0f;
                        xNave -= 1.0f;
                        break;
                    case SDLK_RIGHT:
                        x += 1.0f;
                        xNave += 1.0f;
                        break;
                    case SDLK_UP:
                        y += 1.0f;
                        yNave += 1.0f;
                        break;
                    case SDLK_DOWN:
                        y -= 1.0f;
                        yNave -= 1.0f;
                        break;
                    case SDLK_w:
                        z += 1.0f;
                        zNave += 1.0f;
                        break;
                    case SDLK_s:
                        z -= 1.0f;
                        zNave -= 1.0f;
                        break;
                }
            }
        }

        clear();
        paintSpace(200, 100);

        uniforms.model = createModelMatrix();
        uniforms.view = createViewMatrix();
        uniforms.projection = createProjectionMatrix();
        uniforms.viewport = createViewportMatrix();
        // Call our render function
        render(vertexBufferObjectNave, uniforms, 0);

        uniformsPlaneta.model = createModelMatrix1();
        uniformsPlaneta.view = createViewMatrix();
        uniformsPlaneta.projection = createProjectionMatrix();
        uniformsPlaneta.viewport = createViewportMatrix();

        uniformsjupiter.model = createModelMatrix2(); // Ajusta la matriz de modelo según sea necesario
        uniformsjupiter.view = createViewMatrix();
        uniformsjupiter.projection = createProjectionMatrix();
        uniformsjupiter.viewport = createViewportMatrix();

        //uniformsMoon.model = glm::scale(uniformsMoon.model, glm::vec3(0.7f)); // Ajusta el factor de escala según sea necesario
        uniformsjupiter.model = glm::translate(uniformsjupiter.model, glm::vec3(0.0f, 0.0f, 2.0f)); // Ajusta la traslación según sea necesario
        
        //planeta tierra
        uniformstierra.model = createModelMatrix3(); // Ajusta la matriz de modelo según sea necesario
        uniformstierra.view = createViewMatrix();
        uniformstierra.projection = createProjectionMatrix();
        uniformstierra.viewport = createViewportMatrix();

        //uniformsMoon.model = glm::scale(uniformsMoon.model, glm::vec3(0.7f)); // Ajusta el factor de escala según sea necesario
        uniformstierra.model = glm::translate(uniformstierra.model, glm::vec3(0.0f, 0.0f, 4.0f)); // Ajusta la traslación según sea necesario

        //planeta extra1
        uniformsextra1.model = createModelMatrix4(); // Ajusta la matriz de modelo según sea necesario
        uniformsextra1.view = createViewMatrix();
        uniformsextra1.projection = createProjectionMatrix();
        uniformsextra1.viewport = createViewportMatrix();

        //uniformsMoon.model = glm::scale(uniformsMoon.model, glm::vec3(0.7f)); // Ajusta el factor de escala según sea necesario
        uniformsextra1.model = glm::translate(uniformsextra1.model, glm::vec3(0.0f, 0.0f, 6.0f)); // Ajusta la traslación según sea necesario

        //planeta extra2
        uniformsextra2.model = createModelMatrix5(); // Ajusta la matriz de modelo según sea necesario
        uniformsextra2.view = createViewMatrix();
        uniformsextra2.projection = createProjectionMatrix();
        uniformsextra2.viewport = createViewportMatrix();

        //uniformsMoon.model = glm::scale(uniformsMoon.model, glm::vec3(0.7f)); // Ajusta el factor de escala según sea necesario
        uniformsextra2.model = glm::translate(uniformsextra2.model, glm::vec3(0.0f, 0.0f, 15.0f)); // Ajusta la traslación según sea necesario


        render(vertexBufferObjectPlaneta, uniformsPlaneta, 1);
        render(vertexBufferObjectPlaneta, uniformsjupiter, 2);
        render(vertexBufferObjectPlaneta, uniformstierra, 3);
        render(vertexBufferObjectPlaneta, uniformsextra1, 4);
        render(vertexBufferObjectPlaneta, uniformsextra2, 5);

        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}