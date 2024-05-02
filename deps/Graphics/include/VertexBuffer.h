#pragma once

#include <vector>
#include "Vertex.h"
//Class for all vertex buffers
class VertexBuffer {
public:
	VertexBuffer(std::vector<Vertex> verts);
	VertexBuffer(std::vector<float> verts);
	VertexBuffer(float* verts, size_t size);
	virtual ~VertexBuffer() { };

	virtual void Bind();
	virtual void Unbind();

	float* mVerts;
	size_t mSize;

private:
	uint32_t mVBO = 0;
};