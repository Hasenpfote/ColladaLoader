#pragma once

namespace collada{

class Geometry;
typedef std::vector<Geometry*> GeometryPtrArray;

struct tagTransformationElement;
typedef std::vector<tagTransformationElement*> TransformationPtrArray;

class Input;
typedef std::vector<Input*> InputPtrArray;

typedef std::vector<float> FloatArray;
typedef std::vector<unsigned int> UintArray;

class Triangles;
typedef std::vector<Triangles*> TrianglesPtrArray;

class VertexInput;
typedef std::vector<VertexInput*> VertexInputPtrArray;

} // namespace collada