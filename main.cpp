#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const int width = 800;
const int height = 800;
const int depth = 255;

vec3 light_dir(1, 1, 1);
vec3 camera(0, 1, 3);
vec3 center(0, 0, 0);

Model* model = NULL;
Matrix ProjectionMatrix = NULL;
Matrix Viewport = NULL;
Matrix ModelMatrix = NULL;

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = depth / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}

Matrix lookAt(vec3 camera, vec3 center, vec3 up) {
    vec3 z = (camera - center).normalize();
    vec3 x = cross(up, z).normalize();
    vec3 y = cross(z, x).normalize();
    Matrix res = Matrix::identity(4);
    for (int i = 0; i < 3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int dx = (x1 - x0);
    int dy = (y1 - y0);
    int error = 0;
    int derror = std::abs(dy) * 2;
    int y = y0;
    int ystep = y1 > y0 ? 1 : -1;
    for (int x = x0; x <= x1; x++) {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        error += derror;
        if (error >= dx) {
            y += ystep;
            error -= dx * 2;
        }
    }
}

vec3 baryCentric(vec3* pts, vec3 P) {
    vec3 u = cross(
        vec3(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x), 
        vec3(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
    if (std::abs(u.z) < 1)
        return vec3(-1, 1, 1);
        return vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

vec3 m2v(Matrix m) {
    return vec3(int(m[0][0] / m[3][0]), int(m[1][0] / m[3][0]), int(m[2][0] / m[3][0]));
}

Matrix v2m(vec3 v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

struct StaticShader {
    virtual vec3 vertex(const int iface, const int nthvert) = 0;
    virtual bool fragment(const vec3 bar, TGAColor& color) = 0;
};

struct ShaderProgram : public StaticShader {
    mat<2, 3> varying_uv;
    mat<3, 3> varying_nrm;

    virtual vec3 vertex(int iface, int nthvert) {
        vec3 gl_Vertex = (model->vert(iface, nthvert)); 
        gl_Vertex = m2v(Viewport * ProjectionMatrix * ModelMatrix * v2m(gl_Vertex));     
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        //varying_nrm.set_col(nthvert, m2v((ProjectionMatrix * ModelMatrix).invert_transpose() * v2m(model->normal(iface, nthvert))));
        return gl_Vertex;
    }

    virtual bool fragment(vec3 bar, TGAColor& color) {
        vec2 uv = varying_uv * bar;   
        //vec3 bn = (varying_nrm * bar).normalize();
        //float intensity = std::max(0.0, bn * light_dir);
        color = model->diffuse(uv); //* intensity;
        return false;                             
    }
};

void triangle(vec3* pts, float* zbuffer, TGAImage& image, StaticShader& shader) {
    vec2 boxmin(width - 1, height - 1);
    vec2 boxmax(0, 0);
    for (int i = 0; i < 3; i++) {
        boxmin.x = std::max(0.0, std::min(boxmin.x, pts[i].x));
        boxmin.y = std::max(0.0, std::min(boxmin.y, pts[i].y));
        boxmax.x = std::min(width - 1.0, std::max(boxmax.x, pts[i].x));
        boxmax.y = std::min(height - 1.0, std::max(boxmax.y, pts[i].y));
    }

    vec3 P;
    TGAColor color = TGAColor(rand() % 255, rand() % 255, rand() % 255);
    for (P.x = boxmin.x; P.x <= boxmax.x; P.x++) {
        for (P.y = boxmin.y; P.y <= boxmax.y; P.y++) {
            vec3 bc_vector = baryCentric(pts, P);
            if (bc_vector.x < 0 || bc_vector.y < 0 || bc_vector.z < 0)
                continue;
            P.z = 0;
            P.z += pts[0].z * bc_vector.x;
            P.z += pts[1].z * bc_vector.y;
            P.z += pts[2].z * bc_vector.z;
            if (zbuffer[int(P.x + P.y * width)] < P.z) {
                zbuffer[int(P.x + P.y * width)] = P.z;
                bool discard = shader.fragment(bc_vector, color);
                if (!discard) {
                    image.set(P.x, P.y, color);
                }
                
            }
        }
    }
}




int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);
    model = new Model("obj/african_head/african_head.obj");
    float* zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
    
    ProjectionMatrix = Matrix::identity(4);
    Viewport = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    ModelMatrix = lookAt(camera, center, vec3(0, 1, 0));
    ProjectionMatrix[3][2] = -1.0 / camera.z;
    //ProjectionMatrix[3][2] = -1.0 / (camera - center).norm();
    ShaderProgram shader;

    for (int i = 0; i < model->nfaces(); i++) {
        vec3 screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, zbuffer, image, shader);
    }

    image.write_tga_file("output.tga");
    
    return 0;
}