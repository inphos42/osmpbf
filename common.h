#ifndef OSMPBF_COMMON_H
#define OSMPBF_COMMON_H

#define NULL_PRIMITIVE_ID -1
#define NULL_STRING_ID 0

namespace osmpbf {
	enum NodeType { PlainNode = 1, DenseNode = 2 };
	enum PrimitiveType { NodePrimitive = 0, WayPrimitive = 1, RelationPrimitive = 2 };
}

#endif // OSMPBF_COMMON_H