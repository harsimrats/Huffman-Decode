/* 
 * Reference Huffman coding
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/reference-huffman-coding
 * https://github.com/nayuki/Reference-Huffman-coding
 */

#include <stdexcept>
#include "HuffmanCoder.hpp"


HuffmanDecoder::HuffmanDecoder(BitInputStream &in) :
	input(in) {}


int HuffmanDecoder::read() {
	if (codeTree == nullptr)
		throw std::logic_error("Code tree is null");
	
	const iNode *currentNode = &codeTree->root;
	while (true) {
		int temp = input.readNoEof();
		const iNode *nextNode;
		if      (temp == 0) nextNode = currentNode->leftChild;
		else if (temp == 1) nextNode = currentNode->rightChild;
		else throw std::logic_error("Assertion error: Invalid value from readNoEof()");
		
		if (nextNode->leftChild == NULL)
			return (nextNode->symbol);
		else
			currentNode = nextNode;
	}
}


HuffmanEncoder::HuffmanEncoder(BitOutputStream &out) :
	output(out) {}


void HuffmanEncoder::write(std::uint32_t symbol) {
	if (codeTree == nullptr)
		throw std::logic_error("Code tree is null");
	for (char b : codeTree->getCode(symbol))
		output.write(b);
}
