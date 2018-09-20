#include <time.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "main.h"
#include "vmat.h"

float delta_time = 0.f;
float curr_time = 0.f;
float past_time = 0.f;

void loadMesh(std::vector<vec4> &out_vertices, std::string fileName)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Failed to open mesh: " + fileName + "; " + importer.GetErrorString());
    }

    aiNode *node = scene->mRootNode->mChildren[0];

    if (node->mNumMeshes != 1) {
        throw std::runtime_error("Only a single mesh is suported");
    }

    aiMesh *mesh = scene->mMeshes[node->mMeshes[0]];

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            vec4 v = vec4_set(mesh->mVertices[face.mIndices[j  ]].x,
                              mesh->mVertices[face.mIndices[j  ]].y,
                              mesh->mVertices[face.mIndices[j  ]].z,
                              1.f);
            out_vertices.push_back(v);
        }
    }
}

std::vector<vec4> vertices;

//-----------------------------------------------------------------------------
void MyGlDraw(void)
{
	//*************************************************************************
	// Chame aqui as funções do mygl.h
	//*************************************************************************
    
    ClearBuffer(glm::vec4(0.f));

    curr_time = (float)time(0)/10000.f;
    delta_time = curr_time - delta_time;
    past_time = curr_time;

    static float t = 0.f;
    t += delta_time*0.0000001f;

    std::vector<vec4> v = vertices;

    mat4 m;
    mat4 view;
    mat4 proj;
    mat4 ss;

    perspective(proj, 45.f, IMAGE_WIDTH/(float)IMAGE_HEIGHT, 0.1f, 100.f);
    look_at(vec4_set(3*t, 2*t, 6, 1.f),
            vec4_set(0.f, 0.f, 0.f, 1.f),
            vec4_set(0.f, 1.f, 0.f, 0.f),
            view);
    screen_space(ss, IMAGE_WIDTH, IMAGE_HEIGHT);

    mat4_identity(m);

    mat4_scale(vec4_set(t, t, t, 1.f), m);
    mat4_rotate(-t/2, t, 0, m);

    mat4_mul(view, m, m);
    mat4_mul(proj, m, m);

    for (size_t i = 0; i < v.size(); i++) {
        v[i] = vec4_mul_mat4(v[i], m);

        v[i][0] = v[i][0] / v[i][3];
        v[i][1] = v[i][1] / v[i][3];
        v[i][2] = v[i][2] / v[i][3];
        v[i][3] = v[i][3] / v[i][3];

        v[i] = vec4_mul_mat4(v[i], ss);
    }

    glm::vec4 c(1.f, 1.f, 1.f, 1.f);

    for (int i = 0; i < v.size(); i += 3) {
        DrawLineNDC(v[i  ][0], v[i  ][1], v[i+1][0], v[i+1][1], c, c);
        DrawLineNDC(v[i+1][0], v[i+1][1], v[i+2][0], v[i+2][1], c, c);
        DrawLineNDC(v[i+2][0], v[i+2][1], v[i  ][0], v[i  ][1], c, c);
    }
}


//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// Inicializações.
	InitOpenGL(&argc, argv);
	InitCallBacks();
	InitDataStructures();

	// Ajusta a função que chama as funções do mygl.h
	DrawFunc = MyGlDraw;	

        loadMesh(vertices, "test.obj");

	// Framebuffer scan loop.
	glutMainLoop();

	return 0;
}

