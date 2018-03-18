#include <opencv2\opencv.hpp>
#include <opencv2\tracking.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\ocl.hpp>


using namespace cv;
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()


int main() {
	
	//List of tracker types in OpenCV 3.4
	//NOTE: GOTURN implementation is bugged and don't work
	string trackerTypes[6] = { "BOOSTING","MIL","KCF","TLD","MEDIALFLOW","GOTURN" };
	//vector <string> trackerTypes(type,std::end(type));

	//create tracker
	string trackerType = trackerTypes[2]; //KCF

	Ptr<Tracker> tracker;

#if (CV_MINOR_VERSION <3)
	{
		tracker = Tracker::create(trackerType);
	}
#else
	{
	if (trackerType == "BOOSTING") tracker = TrackerBoosting::create();
	if (trackerType == "MIL") tracker = TrackerMIL::create();
	if (trackerType == "KCF") tracker = TrackerKCF::create();
	if (trackerType == "TLD") tracker = TrackerTLD::create();
	if (trackerType == "MEDIAFLOW") tracker = TrackerMedianFlow::create();
	if (trackerType == "GOTURN") tracker = TrackerGOTURN::create();
	}
#endif

	//Read video
	VideoCapture video("data/film1gray.mp4");

	if (!video.isOpened()) {
		cout << "ERROR: File not opened!"<<endl;
		int key= waitKey(0);
		return 1;
	}
	else {
		Mat frame;

		bool ok = video.read(frame);
		
		//Define initial bounding box
		Rect2d bbox(287, 23, 86, 320);

		bbox = selectROI(frame, false);

		//Display bounding box.
		rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
		imshow("Tracking", frame);

		tracker->init(frame, bbox);

		while (video.read(frame)) {

			//Start timer
			double timer = (double)getTickCount();

			//Update the tracking result
			bool ok = tracker->update(frame, bbox);

			//calculate Frames per second (FPS)
			float fps = getTickFrequency() / ((double)getTickCount() - timer);

			if (ok) {
				//Tracking success :Dram the tracked object
				rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
			}
			else {
				//Tracking failure detecked
				putText(frame, "Tracking failure", Point(250, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
			}

			//Displat tracker type on frame
			putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

			//Diplay FPS on frame
			putText(frame, "FPS : " + SSTR(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

			//Displat frame
			imshow("Tracking", frame);

			int k = waitKey(1);
			if (k == 27) {
				break;
			}


		}
		
		return 0;
	}


	
}