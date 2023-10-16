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

	m_transform = TransformType::New();
	m_transform->SetIdentity();

	itk::SmartPointer<MetricType> metric = MetricType::New();
	metric->SetMovingPointSet(moving.GetPhysicalPoints());
	metric->SetFixedPointSet(fixed.GetPhysicalPoints());
	//metric->SetCorrespondences(correspondences);

	OptimizerType::ScalesType scales(m_transform->GetNumberOfParameters());
	scales.Fill(0.01);
	itk::SmartPointer<OptimizerType> optimizer = OptimizerType::New();
	optimizer->SetScales(scales);
	optimizer->SetNumberOfIterations(1000);
	optimizer->SetValueTolerance(1e-5);
	optimizer->SetGradientTolerance(1e-5);
	optimizer->SetEpsilonFunction(1e-6);
	optimizer->SetUseCostFunctionGradient(true);

	itk::SmartPointer<RegistrationType> registeration = RegistrationType::New();
	registeration->SetInitialTransformParameters(m_transform->GetParameters());
	registeration->SetMetric(metric);
	registeration->SetOptimizer(optimizer);
	registeration->SetTransform(m_transform);
	registeration->SetMovingPointSet(moving.GetPhysicalPoints());
	registeration->SetFixedPointSet(fixed.GetPhysicalPoints());

	try {
		registeration->Update();
		m_transform->SetParameters(m_transform->GetParameters());
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
		std::cout << "Old: " << origPt[0] << " New: " << newPt[0] << std::endl;
		transformedPointSet->GetPoints()->InsertElement(it->Index(), newPt);
		++it;
	}

	Contour transformedContour;
	transformedContour.SetMask(in.GetMask());
	transformedContour.SetPhysicalPointSet(transformedPointSet);

	return transformedContour;

}
