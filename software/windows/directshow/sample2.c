int main()
{
// Playback
IGraphBuilder *pGraphBuilder = NULL;
ICaptureGraphBuilder2 *pCaptureGraphBuilder2 = NULL;
IMediaControl *pMediaControl = NULL;
IBaseFilter *pDeviceFilter = NULL;
IAMStreamConfig *pStreamConfig = NULL;
BYTE *videoCaps = NULL;
AM_MEDIA_TYPE **mediaTypeArray = NULL;

// Device selection
ICreateDevEnum *pCreateDevEnum = NULL;
IEnumMoniker *pEnumMoniker = NULL;
IMoniker *pMoniker = NULL;
ULONG nFetched = 0;

HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

// Create CreateDevEnum to list device
hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (PVOID *)&pCreateDevEnum);
if (FAILED(hr)) goto ReleaseDataAndFail;

// Create EnumMoniker to list devices 
hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
if (FAILED(hr)) goto ReleaseDataAndFail;

pEnumMoniker->Reset();

// Find desired device
while (pEnumMoniker->Next(1, &pMoniker, &nFetched) == S_OK) 
{
  IPropertyBag *pPropertyBag;
  TCHAR devname[256];

  // bind to IPropertyBag
  hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropertyBag);

  if (FAILED(hr))
  {
    pMoniker->Release();
    continue;
  }

  VARIANT varName;
  VariantInit(&varName);
  HRESULT hr = pPropertyBag->Read(L"DevicePath", &varName, 0);

  if (FAILED(hr))
  {
    pMoniker->Release();
    pPropertyBag->Release();
    continue;
  }

  char devicePath[DeviceInfo::STRING_LENGTH_MAX] = "";

  wcstombs(devicePath, varName.bstrVal, DeviceInfo::STRING_LENGTH_MAX);

  if (strcmp(devicePath, deviceId) == 0)
  {
    // Bind Moniker to Filter
    pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pDeviceFilter);

    break;
  }

  pMoniker->Release();
  pPropertyBag->Release();
}

if (pDeviceFilter == NULL) goto ReleaseDataAndFail;

// Create sample grabber
IBaseFilter *pGrabberF = NULL;
hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberF);
if (FAILED(hr)) goto ReleaseDataAndFail;

hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
if (FAILED(hr)) goto ReleaseDataAndFail;

// Create FilterGraph
hr = CoCreateInstance(CLSID_FilterGraph,
NULL,
CLSCTX_INPROC,
IID_IGraphBuilder,
(LPVOID *)&pGraphBuilder);
if (FAILED(hr)) goto ReleaseDataAndFail;

// create CaptureGraphBuilder2
hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (LPVOID *)&pCaptureGraphBuilder2);
if (FAILED(hr)) goto ReleaseDataAndFail;

// set FilterGraph
hr = pCaptureGraphBuilder2->SetFiltergraph(pGraphBuilder);
if (FAILED(hr)) goto ReleaseDataAndFail;

// get MediaControl interface
hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&pMediaControl);
if (FAILED(hr)) goto ReleaseDataAndFail;

// Add filters
hr = pGraphBuilder->AddFilter(pDeviceFilter, L"Device Filter");
if (FAILED(hr)) goto ReleaseDataAndFail;

hr = pGraphBuilder->AddFilter(pGrabberF, L"Sample Grabber");
if (FAILED(hr)) goto ReleaseDataAndFail;

// Set sampe grabber options
AM_MEDIA_TYPE mt;
ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
mt.majortype = MEDIATYPE_Video;
mt.subtype = MEDIASUBTYPE_RGB32;
hr = pGrabber->SetMediaType(&mt);
if (FAILED(hr)) goto ReleaseDataAndFail;

hr = pGrabber->SetOneShot(FALSE);
if (FAILED(hr)) goto ReleaseDataAndFail;

hr = pGrabber->SetBufferSamples(TRUE);
if (FAILED(hr)) goto ReleaseDataAndFail;

// Get stream config interface
hr = pCaptureGraphBuilder2->FindInterface(NULL, &MEDIATYPE_Video, pDeviceFilter, IID_IAMStreamConfig, (void **)&pStreamConfig);
if (FAILED(hr)) goto ReleaseDataAndFail;

int streamCapsCount = 0, capsSize, bestFit = -1, bestFitPixelDiff = 1000000000, desiredPixelCount = _width * _height,
bestFitWidth = 0, bestFitHeight = 0;

float desiredAspectRatio = (float)_width / (float)_height;

hr = pStreamConfig->GetNumberOfCapabilities(&streamCapsCount, &capsSize);
if (FAILED(hr)) goto ReleaseDataAndFail;

videoCaps = (BYTE *)malloc(capsSize * streamCapsCount);
mediaTypeArray = (AM_MEDIA_TYPE **)malloc(sizeof(AM_MEDIA_TYPE *) * streamCapsCount);

for (int i = 0; i < streamCapsCount; i++)
{
  hr = pStreamConfig->GetStreamCaps(i, &mediaTypeArray[i], videoCaps + capsSize * i);
  if (FAILED(hr)) continue;

  VIDEO_STREAM_CONFIG_CAPS *currentVideoCaps = (VIDEO_STREAM_CONFIG_CAPS *)(videoCaps + capsSize * i);

  int closestWidth = MAX(currentVideoCaps->MinOutputSize.cx, MIN(currentVideoCaps->MaxOutputSize.cx, width));
  int closestHeight = MAX(currentVideoCaps->MinOutputSize.cy, MIN(currentVideoCaps->MaxOutputSize.cy, height));

  int pixelDiff = ABS(desiredPixelCount - closestWidth * closestHeight);

  if (pixelDiff < bestFitPixelDiff && ABS(desiredAspectRatio - (float)closestWidth / (float)closestHeight) < 0.1f)
  {
    bestFit = i;
    bestFitPixelDiff = pixelDiff;
    bestFitWidth = closestWidth;
    bestFitHeight = closestHeight;
  }
}

if (bestFit == -1) goto ReleaseDataAndFail;

AM_MEDIA_TYPE *mediaType;
hr = pStreamConfig->GetFormat(&mediaType);
if (FAILED(hr)) goto ReleaseDataAndFail;

VIDEOINFOHEADER *videoInfoHeader = (VIDEOINFOHEADER *)mediaType->pbFormat;
videoInfoHeader->bmiHeader.biWidth = bestFitWidth;
videoInfoHeader->bmiHeader.biHeight = bestFitHeight;
//mediaType->subtype = MEDIASUBTYPE_RGB32;
hr = pStreamConfig->SetFormat(mediaType);
if (FAILED(hr)) goto ReleaseDataAndFail;

pStreamConfig->Release();
pStreamConfig = NULL;

free(videoCaps);
videoCaps = NULL;
free(mediaTypeArray);
mediaTypeArray = NULL;

// Connect pins
IPin *pDeviceOut = NULL, *pGrabberIn = NULL;

if (FindPin(pDeviceFilter, PINDIR_OUTPUT, 0, &pDeviceOut) && FindPin(pGrabberF, PINDIR_INPUT, 0, &pGrabberIn))
{
  hr = pGraphBuilder->Connect(pDeviceOut, pGrabberIn);
  if (FAILED(hr)) goto ReleaseDataAndFail;
}
else
{
  goto ReleaseDataAndFail;
}

// start playing
hr = pMediaControl->Run();
if (FAILED(hr)) goto ReleaseDataAndFail;

hr = pGrabber->GetConnectedMediaType(&mt);

// Set dimensions
width = bestFitWidth;
height = bestFitHeight;
_width = bestFitWidth;
_height = bestFitHeight;

// Allocate pixel buffer
pPixelBuffer = (unsigned *)malloc(width * height * 4);

// Release objects
pGraphBuilder->Release();
pGraphBuilder = NULL;
pEnumMoniker->Release();
pEnumMoniker = NULL;
pCreateDevEnum->Release();
pCreateDevEnum = NULL;

return true;
}
