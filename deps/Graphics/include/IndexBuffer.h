#pragma once
#include<vector>

class IndexBuffer {
public:
	IndexBuffer(std::vector<uint32_t> inds);
	IndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~IndexBuffer() {};

	virtual void Bind();
	virtual void Unbind();

	uint32_t* mIndices;
	size_t mSize;

	uint32_t GetCount() { return mCount; };
private:
	uint32_t mIBO = 0;
	uint32_t mCount;


};