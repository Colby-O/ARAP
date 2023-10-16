#ifndef TRANSFORMER_HPP_
#define TRANSFORMER_HPP_

#include "itkImage.h"

#include "core/contour.hpp"

namespace ARAP::Transformers {

using PhysicalPointSetType = ARAP::Core::Contour::PhysicalPointSetType;
using Contour = ARAP::Core::Contour::Contour;

struct PMatch {
	unsigned int controlIndex;
	unsigned int targetIndex;
};

class Transformer {
public:
	virtual void estimateTransform(const Contour& moving, const Contour& fixed, const std::vector<PMatch>& matches) = 0;
	virtual Contour applyTransform(const Contour& in) = 0;
	template<typename T, typename ImageType = itk::Image<T, 3>, typename ImagePointer = itk::SmartPointer<ImageType>>
	ImagePointer wrapImage(const ImagePointer img) {};
};

}

#endif