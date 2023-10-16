#include <iostream>
#include <vector>
#include <array>
#include <cstdint>

#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkPointSet.h"

#include "core/image.hpp"
#include "core/contour.hpp"
#include "transformer/icpTransformer.hpp"

int main(void) {
	
	using ImageType = itk::Image<double, 3>;    
	using ImagePointer = itk::SmartPointer<ImageType>;

	ImagePointer imgSrc = ARAP::Core::Image::load<double, 3>("../test_1.nii.gz");
	ImagePointer imgRef = ARAP::Core::Image::load<double, 3>("../test_2.nii.gz");
	
	ImagePointer resampled = ARAP::Core::Image::resample<double, 3>(imgSrc, imgRef);
	
	using WriterType = itk::ImageFileWriter<ImageType>;
	itk::SmartPointer<WriterType> writer = WriterType::New();

	writer->SetFileName("out.nii.gz");
	writer->SetInput(resampled);

	try
	{
		writer->Update();
	}
	catch (const itk::ExceptionObject& exp)
	{
		exp.Print(std::cerr);
		return EXIT_FAILURE;  
	}


	using BinaryMask = itk::Image<std::uint8_t, 3>;
	
	auto maskSrc = ARAP::Core::Image::load<std::uint8_t, 3>("../test_mask_1.nrrd");
	auto maskRef = ARAP::Core::Image::load<std::uint8_t, 3>("../test_mask_2.nrrd");

	auto maskResampled = ARAP::Core::Image::resample<uint8_t, 3>(maskSrc, maskRef);

	using MaskWriterType = itk::ImageFileWriter<BinaryMask>;
	itk::SmartPointer<MaskWriterType> maskWriter = MaskWriterType::New();

	maskWriter->SetFileName("mask_resmapled.nii.gz");
	maskWriter->SetInput(maskResampled);

	try
	{
		maskWriter->Update();
	}
	catch (const itk::ExceptionObject& exp)
	{
		exp.Print(std::cerr);
		return EXIT_FAILURE;  
	}

	auto cSrc = ARAP::Core::Contour::Contour(maskResampled);
	auto cRef = ARAP::Core::Contour::Contour(maskRef);

	ARAP::Transformers::ICPTransformer transformer;

	transformer.estimateTransform(cSrc, cRef, std::vector<ARAP::Transformers::PMatch>());
	auto cTrans = transformer.applyTransform(cSrc);

	cSrc.Write("../source.txt");
	cRef.Write("../reference.txt");
	cTrans.Write("../transformed.txt");
	
	return EXIT_SUCCESS;
}
