#include <iostream>

#include "itkPointSet.h"
#include "itkPointSetToImageFilter.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSetToPointSetRegistrationMethod.h"

#include "transformer/icpTransformer.hpp"

using namespace ARAP::Transformers;

void ICPTransformer::estimateTransform(const Contour& moving, const Contour& fixed, const std::vector<PMatch>& matches) {
	using MetricType = itk::EuclideanDistancePointMetric<PhysicalPointSetType, PhysicalPointSetType>;
	using TransformType = itk::AffineTransform<double, 3>;
	using OptimizerType = itk::LevenbergMarquardtOptimizer;
	using RegistrationType = itk::PointSetToPointSetRegistrationMethod<PhysicalPointSetType, PhysicalPointSetType>;
	
	//auto correspondences = findCorrespondences(moving, fixed);

	itk::SmartPointer<TransformType> transform = TransformType::New();
	transform->SetIdentity();

	itk::SmartPointer<MetricType> metric = MetricType::New();
	metric->SetMovingPointSet(moving.GetPhysicalPoints());
	metric->SetFixedPointSet(fixed.GetPhysicalPoints());
	//metric->SetCorrespondences(correspondences);

	itk::SmartPointer<OptimizerType> optimizer = OptimizerType::New();
	optimizer->SetUseCostFunctionGradient(false);

	itk::SmartPointer<RegistrationType> registeration = RegistrationType::New();
	registeration->SetMetric(metric);
	registeration->SetOptimizer(optimizer);
	registeration->SetTransform(transform);
	registeration->SetMovingPointSet(moving.GetPhysicalPoints());
	registeration->SetFixedPointSet(fixed.GetPhysicalPoints());

	try {
		registeration->Update();
		transform->SetParameters(transform->GetParameters());
		m_transform = transform;    
	} catch (itk::ExceptionObject& exp) {
		exp.Print(std::cerr);
		m_transform = nullptr;
	}
}

Contour ICPTransformer::applyTransform(const Contour& in) {
	using TransformType = itk::AffineTransform<double, 3>;

	itk::SmartPointer<PhysicalPointSetType> inputPointSet = in.GetPhysicalPoints();
	itk::SmartPointer<PhysicalPointSetType> transformedPointSet = PhysicalPointSetType::New();

	PhysicalPointSetType::PointsContainer::Iterator it = inputPointSet->GetPoints()->Begin();

	while (it != inputPointSet->GetPoints()->End()) {
		PhysicalPointSetType::PointType origPt = it.Value();
		PhysicalPointSetType::PointType newPt = m_transform->TransformPoint(origPt);
		transformedPointSet->GetPoints()->InsertElement(it->Index(), newPt);
		++it;
	}

	Contour transformedContour;
	transformedContour.SetMask(in.GetMask());
	transformedContour.SetPhysicalPointSet(transformedPointSet);

	return transformedContour;

}
