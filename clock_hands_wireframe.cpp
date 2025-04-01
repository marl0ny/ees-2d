#include "clock_hands_wireframe.hpp"


static std::vector<float> get_vertices_set_elements(
    IVec2 d_2d, std::vector<int> &elements
) {
    std::vector<float> vertices {};
    int vertex_count = 0;
    for (int i = 0; i < d_2d[1]; i++) {
        for (int j = 0; j < d_2d[0]; j++) {
            float u = (float(j) + 0.5F)/float(d_2d[0]);
            float v = (float(i) + 0.5F)/float(d_2d[1]);
            float r = 0.4F/float(std::min(d_2d[0], d_2d[1]));
            std::vector<int> rect_elements {};
            for (int k = 0; k < 4; k++) {
                vertices.push_back(u);
                vertices.push_back(v);
                vertices.push_back(r);
                vertices.push_back((float)k);
                rect_elements.push_back(vertex_count);
                vertex_count++;
            }
            elements.push_back(rect_elements[0]);
            elements.push_back(rect_elements[1]);
            elements.push_back(rect_elements[2]);
            elements.push_back(rect_elements[1]);
            elements.push_back(rect_elements[2]);
            elements.push_back(rect_elements[3]);
        }
    }
    return vertices;
}


WireFrame get_clock_hands_wireframe(IVec2 d_2d) {
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
    }}};
    std::vector<int> elements {};
    std::vector<float> vertices = get_vertices_set_elements(d_2d, elements);
    return WireFrame(attributes, vertices, elements, WireFrame::TRIANGLES);
}