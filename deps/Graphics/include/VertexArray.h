#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <vector>
#include <memory>

class VertexArray {

public:
	VertexArray();

	void Bind();
	void Unbind();

	void AddVertexBuffer(std::shared_ptr<VertexBuffer> buffer);
	void AddIndexBuffer(std::shared_ptr<IndexBuffer> buffer);

	std::shared_ptr<IndexBuffer> GetIndexBuffer();

private:
	uint32_t mVAO = 0;
	std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers;
	std::shared_ptr<IndexBuffer> mIndexBuffer;
};