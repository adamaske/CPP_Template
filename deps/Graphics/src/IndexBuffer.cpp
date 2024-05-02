#include "IndexBuffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

IndexBuffer::IndexBuffer(std::vector<uint32_t> inds) : mCount(inds.size())
{
	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(uint32_t), inds.data(), GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count) : mCount(count) {
	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	mIndices = indices;
	mSize = count;
};

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
}

void IndexBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}