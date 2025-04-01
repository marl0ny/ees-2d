#include "levels_wireframe.hpp"

static std::vector<float> get_vertices_set_elements(
    int number_of_levels,
    std::vector<int> &elements
) {
    std::vector<float> vertices {};
    int vertex_count = 0;
    for (int i = 0; i < number_of_levels; i++) {
        float v = ((float)(i) + 0.5)/number_of_levels;
        if (i == 0) {
            vertices.push_back(0.0);
            vertices.push_back(v);
            vertices.push_back(0);
            vertices.push_back(1);
            elements.push_back(vertex_count);
            vertex_count++;
            vertices.push_back(1.0);
            vertices.push_back(v);
            vertices.push_back(0);
            vertices.push_back(1);
            elements.push_back(vertex_count);
            vertex_count++;
        }
        vertices.push_back(0.0);
        vertices.push_back(v);
        vertices.push_back(0);
        vertices.push_back(0);
        elements.push_back(vertex_count);
        vertex_count++;
        vertices.push_back(1.0);
        vertices.push_back(v);
        vertices.push_back(0);
        vertices.push_back(0);
        elements.push_back(vertex_count);
        vertex_count++;
    }
    return vertices;
}

WireFrame get_levels_wireframe(int number_of_levels) {
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
    }}};
    std::vector<int> elements {};
    std::vector<float> vertices 
        = get_vertices_set_elements(number_of_levels, elements);
    return WireFrame(attributes, vertices, elements, WireFrame::LINES);

}