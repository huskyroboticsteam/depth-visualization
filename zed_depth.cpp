#include <opencv2/opencv.hpp>
#include <sl/Camera.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace sl;

cv::Mat slMat2cvMat(sl::Mat& input);


float ftToMeter(char* value) {
    return (float)strtod(value, NULL) * 0.3048; 
}

// Helper to determine how many bits and channels for ZED Mat conversion to OpenCV Mat
int getOCVtype(sl::MAT_TYPE type) {
    int cv_type = -1;
    switch (type) {
        case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
        case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
        case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
        case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
        case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
        case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
        case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
        case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
        default: break;
    }
    return cv_type;
}

// Converts ZED Mat to OpenCV Mat
cv::Mat slMat2cvMat(sl::Mat& input) {
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), getOCVtype(input.getDataType()), input.getPtr<sl::uchar1>(MEM::CPU), input.getStepBytes(sl::MEM::CPU));
}

int main(int argc, char** argv) {
    float MIN_DIST, MAX_DIST;
    if (argc == 2) {
        MIN_DIST = ftToMeter(argv[1]);
        MAX_DIST = 6.096;
    } else if (argc == 3) {
        MIN_DIST = ftToMeter(argv[1]);
        MAX_DIST = ftToMeter(argv[2]);
    } else {
        MIN_DIST = 0.1;
        MAX_DIST = 6.096;
    }

    Camera zed;

    // ZED runtime settings
    RuntimeParameters runtime_parameters;
    runtime_parameters.sensing_mode = SENSING_MODE::STANDARD;

    // Initializes ZED startup settings
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION::HD720;
    init_params.camera_fps = 30;
    init_params.depth_mode = DEPTH_MODE::ULTRA;
    init_params.coordinate_units = UNIT::METER;
    init_params.depth_minimum_distance = MIN_DIST;
    init_params.depth_maximum_distance = MAX_DIST; 

    // Checks for ZED errors when opening with params
    ERROR_CODE err = zed.open(init_params);
    if (err !=  ERROR_CODE::SUCCESS) {
        std::cout << "Error " << err << ", exit program.\n";
        return EXIT_FAILURE;
    }

    // Re-adjusting frame and window sizes
    Resolution image_size = zed.getCameraInformation().camera_resolution;
    int new_width = image_size.width / 2;
    int new_height = image_size.height / 2;

    Resolution adjusted_image_size(new_width, new_height);

    // ZED Depth Camera
    sl::Mat zed_frame(new_width, new_height, MAT_TYPE::U8_C4, MEM::CPU);
    cv::Mat cv_frame = slMat2cvMat(zed_frame);
    cv::Mat gray_scale;

    // Regular ZED Color Camera
    sl::Mat zed_normal(new_width, new_height, MAT_TYPE::U8_C4, MEM::CPU);
    cv::Mat cv_normal = slMat2cvMat(zed_normal);

    cv::Mat color_map;
    while (true) {
        if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) {
            zed.retrieveImage(zed_frame, VIEW::DEPTH, MEM::CPU, adjusted_image_size);
            zed.retrieveImage(zed_normal, VIEW::LEFT, MEM::CPU, adjusted_image_size);
    
            cvtColor(cv_frame, gray_scale, COLOR_BGR2GRAY); 
            applyColorMap(gray_scale, color_map, cv::COLORMAP_PLASMA);

            imshow("normal", cv_normal);
            imshow("depth", cv_frame);
            imshow("color_map", color_map);
            waitKey(10);
        }
    }

    zed.close();
    return EXIT_SUCCESS;
}
