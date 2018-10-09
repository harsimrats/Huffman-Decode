/* 
 * Reference Huffman coding
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/reference-huffman-coding
 * https://github.com/nayuki/Reference-Huffman-coding
 */

#include <stdexcept>
#include <utility>
#include "CodeTree.hpp"

using std::uint32_t;
using std::vector;


Node::~Node() {}


Leaf::Leaf(uint32_t sym) :
	symbol(sym) {}


InternalNode::InternalNode(std::unique_ptr<Node> &&left, std::unique_ptr<Node> &&right) :
	leftChild(std::move(left)),
	rightChild(std::move(right)) {}


CodeTree::CodeTree(iNode &&rt, uint32_t symbolLimit) :
		root(std::move(rt)) {
	if (symbolLimit < 2)
		throw std::domain_error("At least 2 symbols needed");
	if (symbolLimit > SIZE_MAX)
		throw std::length_error("Too many symbols");
	codes = vector<vector<char> >(symbolLimit, vector<char>());  // Initially all empty
	vector<char> prefix;
	buildCodeList(&root, prefix);  // Fill 'codes' with appropriate data
}


void CodeTree::buildCodeList(const iNode *node, vector<char> &prefix) {
	if (node->leftChild != NULL) {
		
		prefix.push_back(0);
		buildCodeList(node->leftChild, prefix);
		prefix.pop_back();
		
		prefix.push_back(1);
		buildCodeList(node->rightChild, prefix);
		prefix.pop_back();
		
	} else {
		if (node->symbol >= codes.size())
			throw std::invalid_argument("Symbol exceeds symbol limit");
		if (!codes.at(node->symbol).empty())
			throw std::invalid_argument("Symbol has more than one code");
		codes.at(node->symbol) = prefix;	
	}
}


const vector<char> &CodeTree::getCode(uint32_t symbol) const {
	if (codes.at(symbol).empty())
		throw std::domain_error("No code for given symbol");
	else
		return codes.at(symbol);
}
