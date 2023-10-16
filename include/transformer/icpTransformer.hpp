#ifndef ICPTRANSFORMER_HPP_
#define ICPTRANSFORMER_HPP_

#include "itkImage.h"
#include "itkAffineTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

#include "core/contour.hpp"
#include "transformer/transformer.hpp"

namespace ARAP::Transformers {

class ICPTransformer : public Transformer {
private:
	itk::SmartPointer<itk::AffineTransform<double, 3>> m_transform;
public:
	ICPTransformer() = default;
	
	void estimateTransform(const Contour& moving, const Contour& fixed, const std::vector<PMatch>& matches) override;
	Contour applyTransform(const Contour& in) override;
	template<typename T, typename ImageType = itk::Image<T, 3>, typename ImagePointer = itk::SmartPointer<ImageType>>
	ImagePointer wrapImage(const ImagePointer img);

};

template<typename T, typename ImageType = itk::Image<T, 3>, typename ImagePointer = itk::SmartPointer<ImageType>>
ImagePointer ICPTransformer::wrapImage(const ImagePointer img) {
	using TransformType = itk::AffineTransform<double, 3>;
	using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
	using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

	itk::SmartPointer<ResampleFilterType> resampler = ResampleFilterType::New(); 
	resampler->SetInput(img);    

	resampler->SetTransform(m_transform);
	
	resampler->SetInterpolator(InterpolatorType::New());
	
	typename ImageType::RegionType region = img->GetLargestPossibleRegion();
	resampler->SetSize(region.GetSize());
	resampler->SetOutputOrigin(img->GetOrigin());
	resampler->SetOutputSpacing(img->GetOutputSpacing());

	resampler->Update();

	return resampler->GetOutput();
}

}

#endif