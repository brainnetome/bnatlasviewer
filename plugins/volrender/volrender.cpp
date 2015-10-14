/*=========================================================================

  Program:   Visualization Toolkit
  Module:    FixedPointVolumeRayCastMapperCT.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/
// VTK includes
#include "vtkBoxWidget.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkColorTransferFunction.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkMetaImageReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkXMLImageDataReader.h"
#include "vtkFixedPointVolumeRayCastMapper.h"

#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkSmartPointer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"

#include "nifti1_io.h"

#define VTI_FILETYPE 1
#define MHA_FILETYPE 2
#define NIFTI_FILETYPE 3

int NIMDT2VTKDT(int datatype)
{
  int retval = 0;
  switch (datatype){
  case DT_UNSIGNED_CHAR:{retval = VTK_UNSIGNED_CHAR;break;}
  case DT_UINT16:
  case DT_INT16:        {retval = VTK_SHORT; break;}
  case DT_FLOAT:        {retval = VTK_FLOAT; break;}
  case DT_DOUBLE:       {retval = VTK_DOUBLE;break;}
  case DT_UINT32:
  case DT_INT32:        {retval = VTK_INT;break;}
  default:{fprintf(stderr,"unknown nifti image data type");break;}
  }
  return retval;
}

void PrintUsage();
vtkImageData * vtkGetImageData(nifti_image * nim,int idx);

int main(int argc, char *argv[])
{
  // Parse the parameters

  int count = 1;
  char *dirname = NULL;
  double opacityWindow = 4096;
  double opacityLevel = 2048;
  int blendType = 0;
  int clip = 0;
  double reductionFactor = 1.0;
  double frameRate = 10.0;
  char *fileName=0;
  int fileType=0;

  bool independentComponents=true;

  while ( count < argc )
  {
    if ( !strcmp( argv[count], "?" ) )
	{
      PrintUsage();
      exit(EXIT_SUCCESS);
	}
    else if ( !strcmp( argv[count], "-DICOM" ) )
	{
      dirname = new char[strlen(argv[count+1])+1];
      sprintf( dirname, "%s", argv[count+1] );
      count += 2;
	}
    else if ( !strcmp( argv[count], "-VTI" ) )
	{
      fileName = new char[strlen(argv[count+1])+1];
      fileType = VTI_FILETYPE;
      sprintf( fileName, "%s", argv[count+1] );
      count += 2;
	}
    else if ( !strcmp( argv[count], "-MHA" ) )
	{
      fileName = new char[strlen(argv[count+1])+1];
      fileType = MHA_FILETYPE;
      sprintf( fileName, "%s", argv[count+1] );
      count += 2;
	}
    else if ( !strcmp( argv[count], "-NIFTI" ) )
	{
      fileName = new char[strlen(argv[count+1])+1];
      fileType = NIFTI_FILETYPE;
      sprintf( fileName, "%s", argv[count+1] );
      count += 2;
	}
    else if ( !strcmp( argv[count], "-Clip") )
	{
      clip = 1;
      count++;
	}
    else if ( !strcmp( argv[count], "-MIP" ) )
	{
      opacityWindow = atof( argv[count+1] );
      opacityLevel  = atof( argv[count+2] );
      blendType = 0;
      count += 3;
	}
    else if ( !strcmp( argv[count], "-CompositeRamp" ) )
	{
      opacityWindow = atof( argv[count+1] );
      opacityLevel  = atof( argv[count+2] );
      blendType = 1;
      count += 3;
	}
    else if ( !strcmp( argv[count], "-CompositeShadeRamp" ) )
	{
      opacityWindow = atof( argv[count+1] );
      opacityLevel  = atof( argv[count+2] );
      blendType = 2;
      count += 3;
	}
    else if ( !strcmp( argv[count], "-CT_Skin" ) )
	{
      blendType = 3;
      count += 1;
	}
    else if ( !strcmp( argv[count], "-CT_Bone" ) )
	{
      blendType = 4;
      count += 1;
	}
    else if ( !strcmp( argv[count], "-CT_Muscle" ) )
	{
      blendType = 5;
      count += 1;
	}
    else if ( !strcmp( argv[count], "-RGB_Composite" ) )
	{
      blendType = 6;
      count += 1;
	}
    else if ( !strcmp( argv[count], "-Jet" ) )
	{
      blendType = 7;
      count += 1;
	}
    else if ( !strcmp( argv[count], "-FrameRate") )
	{
      frameRate = atof( argv[count+1] );
      if ( frameRate < 0.01 || frameRate > 60.0 )
	  {
        cout << "Invalid frame rate - use a number between 0.01 and 60.0" << endl;
        cout << "Using default frame rate of 10 frames per second." << endl;
        frameRate = 30.0;
	  }
      count += 2;
	}
    else if ( !strcmp( argv[count], "-ReductionFactor") )
	{
      reductionFactor = atof( argv[count+1] );
      if ( reductionFactor <= 0.0 || reductionFactor >= 1.0 )
	  {
        cout << "Invalid reduction factor - use a number between 0 and 1 (exclusive)" << endl;
        cout << "Using the default of no reduction." << endl;
        reductionFactor = 1.0;
	  }
      count += 2;
	}
	else if ( !strcmp( argv[count], "-DependentComponents") )
	{
      independentComponents=false;
      count += 1;
	}
    else
	{
      cout << "Unrecognized option: " << argv[count] << endl;
      cout << endl;
      PrintUsage();
      exit(EXIT_FAILURE);
	}
  }

  if ( !dirname && !fileName)
  {
    cout << "Error: you must specify a directory of DICOM data or a .vti file or a .mha!" << endl;
    cout << endl;
    PrintUsage();
    exit(EXIT_FAILURE);
  }

  // Create the renderer, render window and interactor
  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);
  renderer->SetBackground(1,1,1);

  // Connect it all. Note that funny arithematic on the
  // SetDesiredUpdateRate - the vtkRenderWindow divides it
  // allocated time across all renderers, and the renderer
  // divides it time across all props. If clip is
  // true then there are two props
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->SetDesiredUpdateRate(frameRate / (1+clip) );

  //iren->GetInteractorStyle()->SetDefaultRenderer(renderer);
  vtkInteractorStyleTrackballCamera *style = 
    vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(style);
  style->SetDefaultRenderer(renderer);

  // Read the data
  vtkAlgorithm *reader=0;
  vtkImageData *input=0;
  if(dirname)
  {
    vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(dirname);
    dicomReader->Update();
    input=dicomReader->GetOutput();
    reader=dicomReader;
  }
  else if ( fileType == VTI_FILETYPE )
  {
    vtkXMLImageDataReader *xmlReader = vtkXMLImageDataReader::New();
    xmlReader->SetFileName(fileName);
    xmlReader->Update();
    input=xmlReader->GetOutput();
    reader=xmlReader;
  }
  else if ( fileType == MHA_FILETYPE )
  {
    vtkMetaImageReader *metaReader = vtkMetaImageReader::New();
    metaReader->SetFileName(fileName);
    metaReader->Update();
    input=metaReader->GetOutput();
    reader=metaReader;
  }
  else if ( fileType == NIFTI_FILETYPE )
  {
    //vtkMetaImageReader *metaReader = vtkMetaImageReader::New();
    input = vtkGetImageData(nifti_image_read(fileName,1),0);
	reader = 0;
  }
  else
  {
    cout << "Error! Not VTI or MHA or NIFTI!" << endl;
    exit(EXIT_FAILURE);
  }

  if (reader){reader->Update();}

  // Verify that we actually have a volume
  int dim[3];
  input->GetDimensions(dim);
  if ( dim[0] < 2 ||
       dim[1] < 2 ||
       dim[2] < 2 )
  {
    cout << "Error loading data!" << endl;
    exit(EXIT_FAILURE);
  }

  vtkImageResample *resample = vtkImageResample::New();
  if ( reductionFactor < 1.0 )
  {
    // resample->SetInputConnection( reader->GetOutputPort() );
	resample->SetInputData( input );
    resample->SetAxisMagnificationFactor(0, reductionFactor);
    resample->SetAxisMagnificationFactor(1, reductionFactor);
    resample->SetAxisMagnificationFactor(2, reductionFactor);
  }

  // Create our volume and mapper
  vtkVolume *volume = vtkVolume::New();
  vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();

  if ( reductionFactor < 1.0 )
  {
    mapper->SetInputConnection( resample->GetOutputPort() );
  }
  else
  {
    // mapper->SetInputConnection( reader->GetOutputPort() );
    mapper->SetInputData( input );
  }

  // Set the sample distance on the ray to be 1/2 the average spacing
  double spacing[3];
  if ( reductionFactor < 1.0 )
  {
    resample->GetOutput()->GetSpacing(spacing);
  }
  else
  {
    input->GetSpacing(spacing);
  }

  //  mapper->SetSampleDistance( (spacing[0]+spacing[1]+spacing[2])/6.0 );
  //  mapper->SetMaximumImageSampleDistance(10.0);

  // Create our transfer function
  vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
  vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();

  // Create the property and attach the transfer functions
  vtkVolumeProperty *property = vtkVolumeProperty::New();
  property->SetIndependentComponents(independentComponents);
  property->SetColor( colorFun );
  property->SetScalarOpacity( opacityFun );
  property->SetInterpolationTypeToLinear();

  // connect up the volume to the property and the mapper
  volume->SetProperty( property );
  volume->SetMapper( mapper );

  // Depending on the blend type selected as a command line option,
  // adjust the transfer function
  switch ( blendType )
  {
    // MIP
    // Create an opacity ramp from the window and level values.
    // Color is white. Blending is MIP.
  case 0:
	colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
	opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
							opacityLevel + 0.5*opacityWindow, 1.0 );
	mapper->SetBlendModeToMaximumIntensity();
	break;

    // CompositeRamp
    // Create a ramp from the window and level values. Use compositing
    // without shading. Color is a ramp from black to white.
  case 1:
	colorFun->AddRGBSegment(opacityLevel - 0.5*opacityWindow, 0.0, 0.0, 0.0,
							opacityLevel + 0.5*opacityWindow, 1.0, 1.0, 1.0 );
	opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
							opacityLevel + 0.5*opacityWindow, 1.0 );
	mapper->SetBlendModeToComposite();
	property->ShadeOff();
	break;

    // CompositeShadeRamp
    // Create a ramp from the window and level values. Use compositing
    // with shading. Color is white.
  case 2:
	colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
	opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
							opacityLevel + 0.5*opacityWindow, 1.0 );
	mapper->SetBlendModeToComposite();
	property->ShadeOn();
	break;

    // CT_Skin
    // Use compositing and functions set to highlight skin in CT data
    // Not for use on RGB data
  case 3:
	colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
	colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
	colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-1000, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-500, 1.0, 0.33, 0.45 );
	opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

	mapper->SetBlendModeToComposite();
	property->ShadeOn();
	property->SetAmbient(0.1);
	property->SetDiffuse(0.9);
	property->SetSpecular(0.2);
	property->SetSpecularPower(10.0);
	property->SetScalarOpacityUnitDistance(0.8919);
	break;

    // CT_Bone
    // Use compositing and functions set to highlight bone in CT data
    // Not for use on RGB data
  case 4:
	colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
	colorFun->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
	colorFun->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-16, 0, .49, .61 );
	opacityFun->AddPoint(641, .72, .5, 0.0 );
	opacityFun->AddPoint(3071, .71, 0.5, 0.0);

	mapper->SetBlendModeToComposite();
	property->ShadeOn();
	property->SetAmbient(0.1);
	property->SetDiffuse(0.9);
	property->SetSpecular(0.2);
	property->SetSpecularPower(10.0);
	property->SetScalarOpacityUnitDistance(0.8919);
	break;

    // CT_Muscle
    // Use compositing and functions set to highlight muscle in CT data
    // Not for use on RGB data
  case 5:
	colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
	colorFun->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
	colorFun->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
	colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );

	opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
	opacityFun->AddPoint(-155, 0, 0.5, 0.92 );
	opacityFun->AddPoint(217, .68, 0.33, 0.45 );
	opacityFun->AddPoint(420,.83, 0.5, 0.0);
	opacityFun->AddPoint(3071, .80, 0.5, 0.0);

	mapper->SetBlendModeToComposite();
	property->ShadeOn();
	property->SetAmbient(0.1);
	property->SetDiffuse(0.9);
	property->SetSpecular(0.2);
	property->SetSpecularPower(10.0);
	property->SetScalarOpacityUnitDistance(0.8919);
	break;

    // RGB_Composite
    // Use compositing and functions set to highlight red/green/blue regions
    // in RGB data. Not for use on single component data
  case 6:
	opacityFun->AddPoint(0, 0.0);
	opacityFun->AddPoint(5.0, 0.0);
	opacityFun->AddPoint(30.0, 0.05);
	opacityFun->AddPoint(31.0, 0.0);
	opacityFun->AddPoint(90.0, 0.0);
	opacityFun->AddPoint(100.0, 0.3);
	opacityFun->AddPoint(110.0, 0.0);
	opacityFun->AddPoint(190.0, 0.0);
	opacityFun->AddPoint(200.0, 0.4);
	opacityFun->AddPoint(210.0, 0.0);
	opacityFun->AddPoint(245.0, 0.0);
	opacityFun->AddPoint(255.0, 0.5);

	mapper->SetBlendModeToComposite();
	property->ShadeOff();
	property->SetScalarOpacityUnitDistance(1.0);
	break;

	// Grayscale color map for negative
	// Jet color map for positive
  case 7:
	colorFun->AddRGBPoint( -3024, 1, 1, 1 );
	colorFun->AddRGBPoint( -1,    1, 1, 1 );
	// colorFun->AddRGBPoint( 1,    .0, .0, .5 );
	// colorFun->AddRGBPoint( 1000, .0, .5, 1 );
	// colorFun->AddRGBPoint( 2000, 1, .5,  0 );
	colorFun->AddRGBPoint( 1,    1, .5, .0 );
	colorFun->AddRGBPoint( 3071, 1,  0,  0 );

	opacityFun->AddPoint(-3024, 0.0 );
	opacityFun->AddPoint(-3000, 0.0 );
	opacityFun->AddPoint(-2999, 0.09 );
	opacityFun->AddPoint(-2044, 0.12 );
	opacityFun->AddPoint(-2000, 0.0 );
	opacityFun->AddPoint(-1,    0.0 );
	opacityFun->AddPoint(1,    0.01 );
	opacityFun->AddPoint(300,  .2 );
	opacityFun->AddPoint(2000, .2 );
	opacityFun->AddPoint(3071, .3 );

	mapper->SetBlendModeToComposite();
	property->ShadeOn();
	property->SetAmbient(0.1);
	property->SetDiffuse(0.9);
	property->SetSpecular(0.2);
	property->SetSpecularPower(10.0);
	property->SetScalarOpacityUnitDistance(0.8919);
	break;

  default:
	vtkGenericWarningMacro("Unknown blend type.");
	break;
  }

  // Set the default window size
  renWin->SetSize(600,600);
  // renWin->Render();

  // Add the volume to the scene
  renderer->AddVolume( volume );
  renderer->ResetCamera();

  // interact with data
  // renWin->Render();

  renderer->GetActiveCamera()->Pitch(90);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(90);
  renderer->ResetCamera();

  int angle;
  char foldername[1024]={0,};
  char outimgname[1024]={0,};
  int outimgslashloc=-1;
  for (int i=strlen(fileName)-1;i>1;i--){
	if (fileName[i]=='/'){outimgslashloc=i;break;}
  }
  fprintf(stderr,"%s\n",fileName);
  if (outimgslashloc>0){
	strncpy(foldername,fileName,outimgslashloc);
  }else{foldername[0]='\0';}
  for (angle=0;angle<360;angle+=10)
  {
	sprintf(outimgname,"%s/%03d.png",foldername,angle);
	fprintf(stderr,"%s\n",outimgname);
	renderer->GetActiveCamera()->Azimuth(-10);
	renderer->ResetCamera();
	renWin->Render();

#if 1
	// Screenshot
	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
	  vtkSmartPointer<vtkWindowToImageFilter>::New();
	windowToImageFilter->SetInput(renWin);
	//windowToImageFilter->SetMagnification(1); //set the resolution of the output image
	windowToImageFilter->SetViewport(.2,.20,.8,.70);
	windowToImageFilter->Update();

	vtkSmartPointer<vtkPNGWriter> writer =
	  vtkSmartPointer<vtkPNGWriter>::New();
	writer->SetFileName(outimgname);
	writer->SetInputConnection(windowToImageFilter->GetOutputPort());
	writer->Write();
#endif
  }

  // start renderer
  // iren->Start();

  opacityFun->Delete();
  colorFun->Delete();
  property->Delete();

  volume->Delete();
  mapper->Delete();
  if (reader){reader->Delete();}
  resample->Delete();
  renderer->Delete();
  style->Delete();
  renWin->Delete();
  iren->Delete();

  return 0;
}

void PrintUsage()
{
  cout << "Usage: " << endl;
  cout << endl;
  cout << "  FixedPointVolumeRayCastMapperCT <options>" << endl;
  cout << endl;
  cout << "where options may include: " << endl;
  cout << endl;
  // cout << "  -DICOM <directory>" << endl;
  // cout << "  -VTI <filename>" << endl;
  cout << "  -MHA <filename>" << endl;
  cout << "  -NIFTI <filename>" << endl;
  // cout << "  -DependentComponents" << endl;
  // cout << "  -Clip" << endl;
  // cout << "  -MIP <window> <level>" << endl;
  // cout << "  -CompositeRamp <window> <level>" << endl;
  // cout << "  -CompositeShadeRamp <window> <level>" << endl;
  // cout << "  -CT_Skin" << endl;
  // cout << "  -CT_Bone" << endl;
  // cout << "  -CT_Muscle" << endl;
  cout << "  -Jet" << endl;
  cout << "  -FrameRate <rate>" << endl;
  // cout << "  -DataReduction <factor>" << endl;
  cout << endl;
  cout << "You must use either the -DICOM option to specify the directory where" << endl;
  cout << "the data is located or the -VTI or -MHA option to specify the path of a .vti file." << endl;
  cout << endl;
  cout << "By default, the program assumes that the file has independent components," << endl;
  cout << "use -DependentComponents to specify that the file has dependent components." << endl;
  cout << endl;
  cout << "Use the -Clip option to display a cube widget for clipping the volume." << endl;
  cout << "Use the -FrameRate option with a desired frame rate (in frames per second)" << endl;
  cout << "which will control the interactive rendering rate." << endl;
  cout << "Use the -DataReduction option with a reduction factor (greater than zero and" << endl;
  cout << "less than one) to reduce the data before rendering." << endl;
  cout << "Use one of the remaining options to specify the blend function" << endl;
  cout << "and transfer functions. The -MIP option utilizes a maximum intensity" << endl;
  cout << "projection method, while the others utilize compositing. The" << endl;
  cout << "-CompositeRamp option is unshaded compositing, while the other" << endl;
  cout << "compositing options employ shading." << endl;
  cout << endl;
  cout << "Note: MIP, CompositeRamp, CompositeShadeRamp, CT_Skin, CT_Bone," << endl;
  cout << "and CT_Muscle are appropriate for DICOM data. MIP, CompositeRamp," << endl;
  cout << "and RGB_Composite are appropriate for RGB data." << endl;
  cout << endl;
  cout << "Example: FixedPointVolumeRayCastMapperCT -DICOM CTNeck -MIP 4096 1024" << endl;
  cout << endl;
}

float bn_sign(float v){return ((v>0)?1.f:((v<0)?-1.f:0));}

vtkImageData * vtkGetImageData(nifti_image * nim, int idx)
{
  if (!nim){fprintf(stderr,"invalid nifti_image input!");return 0;}
  if (idx<0 || (idx>=nim->nt && nim->nt!=0)){fprintf(stderr,"time dimension error!");return 0;}
  int tstep = nim->nbyper*nim->nx*nim->ny*nim->nz;
  vtkImageData * input = vtkImageData::New();
  input->SetDimensions(nim->nx,nim->ny,nim->nz);
  input->SetSpacing(nim->dx*bn_sign(nim->qto_xyz.m[0][0]),
					nim->dy*bn_sign(nim->qto_xyz.m[1][1]),
					nim->dz*bn_sign(nim->qto_xyz.m[2][2]));
  // input->SetOrigin(nim->qto_xyz.m[0][3], nim->qto_xyz.m[1][3], nim->qto_xyz.m[2][3]);
  input->SetOrigin(nim->qoffset_x, nim->qoffset_y, nim->qoffset_z);
  input->AllocateScalars(NIMDT2VTKDT(nim->datatype), 1);
  memcpy(input->GetScalarPointer(),((unsigned char*)nim->data)+tstep*idx,tstep);
  return input;
}
