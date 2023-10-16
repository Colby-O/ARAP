#include <iostream>

#include "core/contour.hpp"

using namespace ARAP::Core::Contour;

void Contour::calclateContourFromImage(itk::SmartPointer<BinaryMask> mask, bool fullyConnected = false) {
	using ContourizerType = itk::BinaryContourImageFilter<BinaryMask, BinaryMask>;
	
	auto contourizer = ContourizerType::New();
	contourizer->SetInput(mask);
	contourizer->SetFullyConnected(fullyConnected);
	contourizer->SetBackgroundValue(0);
	contourizer->SetForegroundValue(1);
	contourizer->Update();
	
	itk::SmartPointer<BinaryMask> contourImage = contourizer->GetOutput();
	
	// Extract contour points from contour image
	auto contour = PhysicalPointSetType::New();
	auto points = PhysicalPointSetType::PointsContainer::New();
	PhysicalPointSetType::PointType point;
	
	auto contourIndices = ImagePointSetType::New();
	auto indices = ImagePointSetType::PointsContainer::New();
	
	itk::ImageRegionIterator<BinaryMask> imageIterator(contourImage, contourImage->GetLargestPossibleRegion());
	
	while (!imageIterator.IsAtEnd()) {
		if (imageIterator.Get() != 0) {
			contourImage->TransformIndexToPhysicalPoint(imageIterator.GetIndex(), point);
			points->InsertElement(points->Size(), point);
			ImagePointSetType::PointType index;
			index[0] = imageIterator.GetIndex()[0];
			index[1] = imageIterator.GetIndex()[1];
			index[2] = imageIterator.GetIndex()[2];
			indices->InsertElement(indices->Size(), index);
		}
		++imageIterator;
	}
	
	contour->SetPoints(points);
	contourIndices->SetPoints(indices);
	
	m_points = contour;
	m_indices = contourIndices;
	m_mask = mask;
}

void Contour::calculateNewPhysicalPoints() {
	m_points = PhysicalPointSetType::New();
	PhysicalPointSetType::PointType point;
	ImagePointSetType::PointType indexPoint;
	BinaryMask::IndexType index;

	for (int i = 0; i < m_indices->GetNumberOfPoints(); ++i) {
		m_indices->GetPoint(i, &indexPoint);
		index[0] = indexPoint[0];
		index[1] = indexPoint[1];
		index[2] = indexPoint[2];
		m_mask->TransformIndexToPhysicalPoint(index, point);
		m_points->SetPoint(i, point);
	}
}

void Contour::calculateNewIndices() {
	m_indices = ImagePointSetType::New();
	ImagePointSetType::PointType indexPoint;
	PhysicalPointSetType::PointType point;
	BinaryMask::IndexType index;

	for (int i = 0; i < m_indices->GetNumberOfPoints(); ++i) {
		m_points->GetPoint(i, &point);
		m_mask->TransformPhysicalPointToIndex(point, index);
		indexPoint[0] = index[0];
		indexPoint[1] = index[1];
		indexPoint[2] = index[2];
		m_indices->SetPoint(i, indexPoint);
	}
}

unsigned int Contour::getNumberOfPoints() const {
	return m_points->GetNumberOfPoints();
}

void Contour::SetPhysicalPointSet(itk::SmartPointer<PhysicalPointSetType> pointSet) {
	m_points = pointSet;
	calculateNewIndices();
}

void Contour::SetImagePointSet(itk::SmartPointer<ImagePointSetType> pointSet) {
	m_indices = pointSet;
	calculateNewPhysicalPoints();
}

void Contour::SetMask(itk::SmartPointer<BinaryMask> mask) {
	m_mask = mask;
}

Contour::Contour(itk::SmartPointer<BinaryMask> mask) {
	calclateContourFromImage(mask);
}

itk::SmartPointer<PhysicalPointSetType> Contour::GetPhysicalPoints() const {
	return m_points;
}

itk::SmartPointer<ImagePointSetType> Contour::GetIndices() const {
	return m_indices;
}

itk::SmartPointer<BinaryMask> Contour::GetMask() const {
	return m_mask;
}

PhysicalPointSetType::PointType Contour::GetPhysicalPointAt(unsigned int i) const {
	PhysicalPointSetType::PointType pt;
	m_points->GetPoint(i, &pt);
	return pt;
}
	
ImagePointSetType::PointType Contour::GetIndexAt(unsigned int i) const {
	ImagePointSetType::PointType pt;
	m_indices->GetPoint(i, &pt);
	return pt;
}   

void Contour::Print() const {
	using PointType = PhysicalPointSetType::PointType;
	using IndexType = ImagePointSetType::PointType;

	for (int i = 0; i < getNumberOfPoints(); ++i) {
		auto pt = GetPhysicalPointAt(i);
		auto index = GetIndexAt(i);
		std::cout << "Point " << i << " x: " << pt[0] << " y: " << pt[1] << " z: " << pt[2];
		std::cout << " i: " << index[0] << " j: " << index[1] << " k: " << index[2] << std::endl;
	}
}