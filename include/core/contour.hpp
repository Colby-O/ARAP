#ifndef CONTOUR_HPP_
#define CONTOUR_HPP_

#include <vector>
#include <array>
#include <string>
#include <cstdint>

#include "itkImage.h"
#include "itkBinaryContourImageFilter.h"
#include "itkPointSet.h"
#include "itkImageRegionIterator.h"

#include "core/image.hpp"

namespace ARAP::Core::Contour {

using BinaryMask = ARAP::Core::Image::BinaryMask;
using PhysicalPointSetType = itk::PointSet<double, 3>;
using ImagePointSetType = itk::PointSet<unsigned int, 3>;

class Contour {
private:
	itk::SmartPointer<BinaryMask> m_mask;
	itk::SmartPointer<PhysicalPointSetType> m_points;
	itk::SmartPointer<ImagePointSetType> m_indices;
	
	void calclateContourFromImage(itk::SmartPointer<BinaryMask> mask, bool fullyConnected);
	void calculateNewPhysicalPoints();
	void calculateNewIndices();
public:
	Contour() = default;
	Contour(itk::SmartPointer<BinaryMask> mask);

	unsigned int getNumberOfPoints() const;
	void SetPhysicalPointSet(itk::SmartPointer<PhysicalPointSetType> pointSet);
	void SetImagePointSet(itk::SmartPointer<ImagePointSetType> pointSet);
	void SetMask(itk::SmartPointer<BinaryMask> mask);
	itk::SmartPointer<PhysicalPointSetType> GetPhysicalPoints() const;
	itk::SmartPointer<ImagePointSetType> GetIndices() const;
	itk::SmartPointer<BinaryMask> GetMask() const;
	PhysicalPointSetType::PointType GetPhysicalPointAt(unsigned int i) const;
	ImagePointSetType::PointType GetIndexAt(unsigned int i) const;
	PhysicalPointSetType::PointType ComputeCenterOfMass() const;
	void Print() const;
	void Write(const std::string& filename) const;
};

}

#endif
