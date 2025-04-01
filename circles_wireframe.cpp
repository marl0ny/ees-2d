#include "circles_wireframe.hpp"
#include "gl_wrappers.hpp"


const float PI = 3.141592653589793;


static std::vector<float> get_vertices_set_elements(
    IVec2 d_2d, 
    std::vector<int> &elements
) {
    std::vector<float> vertices {};
    int vertex_count = 0;
    for (int i = 0; i < d_2d[1]; i++) {
        for (int j = 0; j < d_2d[0]; j++) {
            float u = (float(j) + 0.5F)/float(d_2d[0]);
            float v = (float(i) + 0.5F)/float(d_2d[1]);
            float r = 0.4F/float(std::min(d_2d[0], d_2d[1]));
            int max_k = 50;
            std::vector<int> circle_elements1 {};
            std::vector<int> circle_elements2 {};
            for (int k = 0; k < max_k; k++) {
                float angle = 2.0*k*PI/max_k;
                float c = cos(angle), s = sin(angle);
                vertices.push_back(u + r*c);
                vertices.push_back(v + r*s);
                circle_elements1.push_back(vertex_count);
                vertex_count++;
            }
            for (int k = 1; k < circle_elements1.size(); k++)
                circle_elements2.push_back(circle_elements1[k]);
            circle_elements2.push_back(circle_elements1[0]);
            for (int k = 0; k < circle_elements1.size(); k++) {
                elements.push_back(circle_elements1[k]);
                elements.push_back(circle_elements2[k]);
            }
        }
    }
    return vertices;
}

WireFrame get_circles_wireframe(IVec2 d_2d) {
    Attributes attributes = {
        {"position", {
            .size=2, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
    }}};
    std::vector<int> elements {};
    std::vector<float> vertices = get_vertices_set_elements(d_2d, elements);
    return WireFrame(attributes, vertices, elements, WireFrame::LINES);
}