#pragma once
#include <iostream>
#include <fstream>
#include <istream>
#include "glm/glm.hpp"

class Vertex {
	//! Overloaded ostream operator which writes all vertex data on an open textfile stream
	friend std::ostream& operator<< (std::ostream&, const Vertex&);

	//! Overloaded ostream operator which reads all vertex data from an open textfile stream
	friend std::istream& operator>> (std::istream&, Vertex&);

public:
	float x{ 0.f }, y{ 0.f }, z{ 0.f };
	float r{ 1.f }, g{ 1.f }, b{ 1.f };
	float t1{ 0.f }, t2{ 0.f };
public:
	Vertex();
	Vertex(glm::vec3 verts);
	Vertex(glm::vec3 verts, glm::vec2 uvs);
	Vertex(glm::vec3 verts, glm::vec3 norms);
	Vertex(glm::vec3 verts, glm::vec3 norms, glm::vec2 uvs);
	Vertex(glm::vec3 verts, glm::vec3 norms, std::pair<float, float> uvs);
	~Vertex();
};