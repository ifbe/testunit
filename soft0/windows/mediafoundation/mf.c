#define WINVER 0x602
#include<mfapi.h>
#include<mferror.h>
#include<mfidl.h>
#include<mfobjects.h>
#include<mftransform.h>




HRESULT CreateVideoDeviceSource()
{
    IMFMediaSource *pSource = NULL;
    IMFAttributes *pAttributes = NULL;
    IMFActivate **ppDevices = NULL;

    // Create an attribute store to specify the enumeration parameters.
    HRESULT hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr))
    {
        goto done;
    }

    // Source type: video capture devices
    hr = pAttributes->lpVtbl->SetGUID(
		pAttributes,
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, 
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
    if (FAILED(hr))
    {
        goto done;
    }

    // Enumerate devices.
    UINT32 count;
    hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
    if (FAILED(hr))
    {
        goto done;
    }

    if (count == 0)
    {
        hr = E_FAIL;
        goto done;
    }

    // Create the media source object.
    hr = ppDevices[0]->lpVtbl->ActivateObject(IID_PPV_ARGS(&pSource));
    if (FAILED(hr))
    {
        goto done;
    }

done:
    CoTaskMemFree(ppDevices);
    return hr;
}
void main()
{
	IMFMediaSource* src = 0;
	MFStartup(MF_VERSION, MFSTARTUP_LITE);

	//
	

	MFShutdown();
}