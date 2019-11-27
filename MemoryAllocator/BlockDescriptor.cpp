class BlockDescriptor
{
public:
	void*				m_blockAddress;		// Void pointer to point towards block of memory
	size_t				m_sizeOfBlock;		// Size of the block of memory
	BlockDescriptor*	m_nextBlock;		// Address of next block of memory
	BlockDescriptor*	m_prevBlock;		// Address of the previous block of memory

	BlockDescriptor()
	{
		this->m_blockAddress = nullptr;
		this->m_sizeOfBlock = 0;
		this->m_nextBlock = nullptr;
		this->m_prevBlock = nullptr;
	}

	BlockDescriptor(void* i_blockAddress, size_t i_sizeOfBlock, BlockDescriptor* i_nextBlock, BlockDescriptor* i_prevBlock)
	{
		this->m_blockAddress = i_blockAddress;
		this->m_sizeOfBlock = i_sizeOfBlock;
		this->m_nextBlock = i_nextBlock;
		this->m_prevBlock = i_prevBlock;
	}
};