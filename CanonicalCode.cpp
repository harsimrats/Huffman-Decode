/* 
 * Reference Huffman coding
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/reference-huffman-coding
 * https://github.com/nayuki/Reference-Huffman-coding
 */

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <iostream>
#include "CanonicalCode.hpp"

using std::uint32_t;
using std::vector;


CanonicalCode::CanonicalCode(const vector<uint32_t> &codeLens) {
	// Check basic validity
	if (codeLens.size() < 2)
		throw std::invalid_argument("At least 2 symbols needed");
	if (codeLens.size() > UINT32_MAX)
		throw std::length_error("Too many symbols");
	
	// Copy once and check for tree validity
	codeLengths = codeLens;
	std::sort(codeLengths.begin(), codeLengths.end(), std::greater<uint32_t>());
	uint32_t currentLevel = codeLengths.front();
	uint32_t numNodesAtLevel = 0;
	for (uint32_t cl : codeLengths) {
		if (cl == 0)
			break;
		while (cl < currentLevel) {
			if (numNodesAtLevel % 2 != 0)
				throw std::invalid_argument("Under-full Huffman code tree");
			numNodesAtLevel /= 2;
			currentLevel--;
		}
		numNodesAtLevel++;
	}
	while (currentLevel > 0) {
		if (numNodesAtLevel % 2 != 0)
			throw std::invalid_argument("Under-full Huffman code tree");
		numNodesAtLevel /= 2;
		currentLevel--;
	}
	if (numNodesAtLevel < 1)
		throw std::invalid_argument("Under-full Huffman code tree");
	if (numNodesAtLevel > 1)
		throw std::invalid_argument("Over-full Huffman code tree");
	
	// Copy again
	codeLengths = codeLens;
}


CanonicalCode::CanonicalCode(const CodeTree &tree, uint32_t symbolLimit) {
	if (symbolLimit < 2)
		throw std::invalid_argument("At least 2 symbols needed");
	codeLengths = vector<uint32_t>(symbolLimit, 0);
	buildCodeLengths(&tree.root, 0);
}


void CanonicalCode::buildCodeLengths(const iNode *node, uint32_t depth) {
	if (node->leftChild != NULL) {
		buildCodeLengths(node->leftChild, depth + 1);
		buildCodeLengths(node->rightChild, depth + 1);
	} else {
		uint32_t symbol = node->symbol;
		if (symbol >= codeLengths.size())
			throw std::invalid_argument("Symbol exceeds symbol limit");
		// Note: CodeTree already has a checked constraint that disallows a symbol in multiple leaves
		if (codeLengths.at(symbol) != 0)
			throw std::logic_error("Assertion error: Symbol has more than one code");
		codeLengths.at(symbol) = depth;
	}
}


uint32_t CanonicalCode::getSymbolLimit() const {
	return static_cast<uint32_t>(codeLengths.size());
}


uint32_t CanonicalCode::getCodeLength(uint32_t symbol) const {
	if (symbol >= codeLengths.size())
		throw std::domain_error("Symbol out of range");
	return codeLengths.at(symbol);
}

CodeTree CanonicalCode::toCodeTree() const {
	vector<iNode*> nodes;
	for (uint32_t i = *std::max_element(codeLengths.cbegin(), codeLengths.cend()); ; i--) {  // Descend through code lengths
		if (nodes.size() % 2 != 0)
			throw std::logic_error("Assertion error: Violation of canonical code invariants");
		vector<iNode*> newNodes;
		
		// Add leaves for symbols with positive code length i
		if (i > 0) {
			uint32_t j = 0;
			for (uint32_t cl : codeLengths) {
				if (cl == i)
				{
					iNode* temp = (iNode*) calloc(1, sizeof(iNode));
					temp->symbol = j;
					newNodes.push_back(temp);
				}
				j++;
			}
		}
		
		// Merge pairs of nodes from the previous deeper layer
		for (std::size_t j = 0; j < nodes.size(); j += 2) {
			iNode* temp = (iNode*) calloc(1, sizeof(iNode));
			temp->leftChild = nodes.at(j);
			temp->rightChild = nodes.at(j+1);
			newNodes.push_back(temp);
		}
		nodes = std::move(newNodes);
		
		if (i == 0)
			break;
	}
	
	if (nodes.size() != 1)
		throw std::logic_error("Assertion error: Violation of canonical code invariants");
	
	iNode *root = nodes.front();
	CodeTree result(std::move(*root), static_cast<uint32_t>(codeLengths.size()));
	delete root;
	return result;
}
