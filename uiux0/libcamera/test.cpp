//https://github.com/kbingham/simple-cam/blob/master/simple-cam.cpp
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <libcamera/libcamera.h>
using namespace libcamera;



static void requestComplete(Request *request)
{
	if (request->status() == Request::RequestCancelled)return;

	std::cout << "requestComplete" << std::endl;

	const ControlList &requestMetadata = request->metadata();
	for (const auto &ctrl : requestMetadata) {
		const ControlId *id = controls::controls.at(ctrl.first);
		const ControlValue &value = ctrl.second;

		std::cout << "\t" << id->name() << " = " << value.toString() << std::endl;
	}

	const Request::BufferMap &buffers = request->buffers();
	for (auto bufferPair : buffers) {
		// (Unused) Stream *stream = bufferPair.first;
		FrameBuffer *buffer = bufferPair.second;
		const FrameMetadata &metadata = buffer->metadata();

		/* Print some information about the buffer which has completed. */
		std::cout << "\tseq: " << metadata.sequence << std::endl;
		std::cout << "\ttimestamp: " << metadata.timestamp << std::endl;

		std::cout << "\tbytesused: ";
		unsigned int nplane = 0;
		for (const FrameMetadata::Plane &plane : metadata.planes())
		{
			std::cout << plane.bytesused;
			if (++nplane < metadata.planes().size())
				std::cout << "/";
		}
		std::cout << std::endl;
	}

	//request->reuse(Request::ReuseBuffers);
	//cam->queueRequest(request);
}



std::string cameraName(Camera *camera)
{
	const ControlList &props = camera->properties();
	std::string name;

	const auto &location = props.get(properties::Location);
	if (location) {
		switch (*location) {
		case properties::CameraLocationFront:
			name = "Internal front camera";
			break;
		case properties::CameraLocationBack:
			name = "Internal back camera";
			break;
		case properties::CameraLocationExternal:
			name = "External camera";
			const auto &model = props.get(properties::Model);
			if (model)name = *model;
			break;
		}
	}

	//name += " (" + camera->id() + ")";
	return name;
}

int main(){
	std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();

	cm->start();

	auto list = cm->cameras();
	if(list.empty()){
		std::cout << "no camera" << std::endl;
		return 0;
	}

	for (auto const &camera : list){
		std::cout << cameraName(camera.get())<< " @ " << camera->id() << std::endl;
	}

	std::string id = list[0]->id();
	auto cam = cm->get(id);
	cam->acquire();

	//std::unique_ptr<CameraConfiguration> cfglist = cam->generateConfiguration({StreamRole::Viewfinder});
	std::unique_ptr<CameraConfiguration> cfglist = cam->generateConfiguration({StreamRole::Raw});
	//std::unique_ptr<CameraConfiguration> cfglist = cam->generateConfiguration({StreamRole::VideoRecording});
	//std::unique_ptr<CameraConfiguration> cfglist = cam->generateConfiguration({StreamRole::StillCapture});
	//std::unique_ptr<CameraConfiguration> cfglist = cam->generateConfiguration({StreamRole::Raw, StreamRole::StillCapture,StreamRole::VideoRecording, StreamRole::Viewfinder});

	for(StreamConfiguration& cfg : *cfglist){
		std::cout << "config: " << cfg.toString() << std::endl;
	}

	cfglist->validate();
	cam->configure(cfglist.get());

	FrameBufferAllocator *allocator = new FrameBufferAllocator(cam);
	for (StreamConfiguration &cfg : *cfglist) {
		int ret = allocator->allocate(cfg.stream());
		if (ret < 0) {
			std::cerr << "Can't allocate buffers" << std::endl;
			return EXIT_FAILURE;
		}

		size_t allocated = allocator->buffers(cfg.stream()).size();
		std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
	}

	StreamConfiguration &streamConfig = cfglist->at(0);
	Stream *stream = streamConfig.stream();
	const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
	std::vector<std::unique_ptr<Request>> requests;
	for (unsigned int i = 0; i < buffers.size(); ++i) {
		std::unique_ptr<Request> request = cam->createRequest();
		if (!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return EXIT_FAILURE;
		}

		const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
		int ret = request->addBuffer(stream, buffer.get());
		if (ret < 0)
		{
			std::cerr << "Can't set buffer for request"
				  << std::endl;
			return EXIT_FAILURE;
		}

		/*
		 * Controls can be added to a request on a per frame basis.
		 */
		ControlList &controls = request->controls();
		controls.set(controls::Brightness, 0.5);

		requests.push_back(std::move(request));
	}
	
	cam->requestCompleted.connect(requestComplete);
	cam->start();
	for (std::unique_ptr<Request> &request : requests)
		cam->queueRequest(request.get());
	usleep(10*1000*1000);
	cam->stop();

	allocator->free(stream);
	delete allocator;

	cam->release();
	cm->stop();
	return 0;
}
