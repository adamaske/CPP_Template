#include "vertex.h"

Vertex::Vertex()
{
}

Vertex::Vertex(glm::vec3 verts)
    :x(verts.x), y(verts.y), z(verts.z)
{
}

Vertex::Vertex(glm::vec3 verts, glm::vec2 uvs)
    : x(verts.x), y(verts.y), z(verts.z), t1(uvs.x), t2(uvs.y)
{
}

Vertex::Vertex(glm::vec3 verts, glm::vec3 norms) :
    x(verts.x), y(verts.y), z(verts.z), r(norms.x), g(norms.y), b(norms.z)
{
}

Vertex::Vertex(glm::vec3 verts, glm::vec3 norms, glm::vec2 uvs) :
    x(verts.x), y(verts.y), z(verts.z), r(norms.x), g(norms.y), b(norms.z), t1(uvs.x), t2(uvs.y)
{
}

Vertex::Vertex(glm::vec3 verts, glm::vec3 norms, std::pair<float, float> uvs)
{
    x = verts.x;
    y = verts.y;
    z = verts.z;

    r = norms.r;
    g = norms.g;
    b = norms.b;

    t1 = uvs.first;
    t2 = uvs.second;
}

Vertex::~Vertex()
{
}

std::ostream& operator<< (std::ostream& os, const Vertex& v) {
    os << std::fixed;
    os << "(" << v.x << ", " << v.y << ", " << v.z << ") ";
    os << "(" << v.r << ", " << v.g << ", " << v.b << ") ";
    os << "(" << v.t1 << ", " << v.t2 << ") ";
    return os;
}
std::istream& operator>> (std::istream& is, Vertex& v) {
    // Trenger fire temporære variabler som kun skal lese inn parenteser og komma
    char dum, dum2, dum3, dum4;
    is >> dum >> v.x >> dum2 >> v.y >> dum3 >> v.z >> dum4;
    is >> dum >> v.r >> dum2 >> v.r >> dum3 >> v.g >> dum4;
    is >> dum >> v.t1 >> dum2 >> v.t2 >> dum3;
    return is;
}