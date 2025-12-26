#include "Node.hpp"

inline const bool Node::is(const std::initializer_list<NodeType>& types) const
{
	for (auto type : types)
		if type == _type return true;
	return false;
}

inline void Node::setRight(std::shared_ptr<Node>&& right)
{
	if (_right != nullptr)
		throw std::runtime_error("attempted to overwrite existign right node");
	_right = std::make_shared<Node>(right);
}