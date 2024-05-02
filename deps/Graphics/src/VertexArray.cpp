#include "VertexArray.h"

VertexArray::VertexArray() {
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
};

void VertexArray::Bind() {
	glBindVertexArray(mVAO);
};
void VertexArray::Unbind() {
	glBindVertexArray(0);
};


void VertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> buffer) {
	mVertexBuffers.push_back(buffer);
	//For this vertex buffer, we want to specify the layot
	//So we bind it
	buffer->Bind();
	//Verts

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//uvs

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Unbinds ? 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
};
void VertexArray::AddIndexBuffer(std::shared_ptr<IndexBuffer> buffer) {
	//Use this vertex array
	glBindVertexArray(mVAO);

	//So we bind it
	buffer->Bind();

	mIndexBuffer = buffer;
};

std::shared_ptr<IndexBuffer> VertexArray::GetIndexBuffer() {
	return mIndexBuffer;
}
