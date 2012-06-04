#ifndef OSMPBF_COMMON_H
#define OSMPBF_COMMON_H

namespace osmpbf {
	enum NodeType { PlainNode = 0, DenseNode = 1 };
	enum PrimitiveType { NodePrimitive = 0, WayPrimitive = 1, RelationPrimitive = 2 };
}

#endif // OSMPBF_COMMON_H