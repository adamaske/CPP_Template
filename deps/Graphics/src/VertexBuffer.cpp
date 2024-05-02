#include "VertexBuffer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

VertexBuffer::VertexBuffer(std::vector<Vertex> verts)
{
	//We want a buffer in that renderer
	glGenBuffers(1, &mVBO);
	//
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	//In that buffer we want an array, with the size of size and the floats of verts, which wont chagne 
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_STATIC_DRAW);

}
VertexBuffer::VertexBuffer(std::vector<float> verts) {
	//We want a buffer in that renderer
	glGenBuffers(1, &mVBO);
	//
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	//In that buffer we want an array, with the size of size and the floats of verts, which wont chagne 
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_STATIC_DRAW);
}
VertexBuffer::VertexBuffer(float* verts, size_t size) {

	//We want a buffer in that renderer
	glGenBuffers(1, &mVBO);
	//
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	//In that buffer we want an array, with the size of size and the floats of verts, which wont chagne 
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	mVerts = verts;
	mSize = size;
}

void VertexBuffer::Bind()
{
	//We want to bind the buffer we created in mRenderer
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
}

void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}