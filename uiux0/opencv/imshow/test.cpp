#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <vector>

bool compareContourArea(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
    double area1 = cv::contourArea(contour1);
    double area2 = cv::contourArea(contour2);
    return (area1 > area2);
}

bool compareByDotVecY(std::vector<float> a, std::vector<float> b) {
    return a[3] < b[3]; // 升序排序
}

cv::Mat resizeimage(cv::Mat image){
    cv::Mat resized_image;
    resize(image, resized_image, cv::Size(image.cols / 4, image.rows / 4));
    return resized_image;
}

void myshow(const char* str, cv::Mat img) {
    printf("myshow: w=%d,h=%d\n", img.cols, img.rows);
    if ((img.cols > 1920 * 4) || (img.rows > 1080*4)) {
        resize(img, img, cv::Size(img.cols / 5, img.rows / 5));
    }
    else if ((img.cols > 1920 * 3) || (img.rows > 1080 * 3)) {
        resize(img, img, cv::Size(img.cols / 4, img.rows / 4));
    }
    else if ((img.cols > 1920 * 2) || (img.rows > 1080*2)) {
        resize(img, img, cv::Size(img.cols / 3, img.rows / 3));
    }
    else if ((img.cols > 1920) || (img.rows > 1080)) {
        resize(img, img, cv::Size(img.cols / 2, img.rows / 2));
    }

    cv::imshow(str, img);
}

cv::Mat wrapimage(cv::Mat inimg, std::vector<std::vector<float>> point)
{
    int tmpw = inimg.cols / 4;
    int tmph = inimg.rows / 4;
    int iw = 500;
    int ih = 100;
    std::vector<cv::Point2f> srcPts = {
            cv::Point2f(point[0][0], point[0][1]), cv::Point2f(point[1][0], point[1][1]),
            cv::Point2f(point[3][0], point[3][1]), cv::Point2f(point[2][0], point[2][1])
    };
    std::vector<cv::Point2f> tarPts = {
            cv::Point2f(tmpw/2.0-iw/2, tmph/2-ih/2), cv::Point2f(tmpw/2.0+iw/2, tmph/2-ih/2),
            cv::Point2f(tmpw/2.0-iw/2, tmph/2+ih/2), cv::Point2f(tmpw/2.0+iw/2, tmph/2+ih/2)
    };
    cv::Mat src2dst = cv::getPerspectiveTransform(srcPts, tarPts);
    cv::Mat dst2src = cv::getPerspectiveTransform(tarPts, srcPts);

    cv::Mat outimg;
    cv::warpPerspective(inimg, outimg, src2dst, cv::Size(tmpw, tmph));
    return outimg;
}

cv::Point mat33_mul_point(cv::Mat mat, cv::Point2f in)
{
    cv::Vec3f out[4];
    out[0][0] = mat.at<double>(0, 0) * in.x + mat.at<double>(0, 1) * in.y + mat.at<double>(0, 2);
    out[0][1] = mat.at<double>(1, 0) * in.x + mat.at<double>(1, 1) * in.y + mat.at<double>(1, 2);
    out[0][2] = mat.at<double>(2, 0) * in.x + mat.at<double>(2, 1) * in.y + mat.at<double>(2, 2);
    out[0][0] /= out[0][2];
    out[0][1] /= out[0][2];

    return cv::Point2f(out[0][0], out[0][1]);
}


std::vector<cv::Point> myapprox(cv::Mat debug, std::vector<cv::Point>& contour) {

    //step0: find line
    cv::Vec4f lineParams;
    cv::fitLine(contour, lineParams, cv::DIST_L2, 0, 0.01, 0.01);

    float cx = lineParams[2];
    float cy = lineParams[3];
    float rx = lineParams[0];
    float ry = lineParams[1];
    float tx =-ry;
    float ty = rx;
    printf("cx=%f,cy=%f,rx=%f,ry=%f,tx=%f,ty=%f\n", cx, cy, rx, ry, tx, ty);

    
    //step1: find xmin and xmax
    float xmin = 10000;
    float xmax = -10000;
    std::vector<std::pair<cv::Point, cv::Point2f>> temp;
    for (auto& point : contour) {
        int x = (point.x - cx);
        int y = (point.y - cy);
        cv::Point2f pxpy = cv::Point2f(x * rx + y * ry, x * tx + y * ty);
        if (xmin > pxpy.x) xmin = pxpy.x;
        if (xmax < pxpy.x) xmax = pxpy.x;
        //cout << x0 << " " << y0 << endl;

        temp.push_back(std::make_pair(point, pxpy));
    }
    //cout << xmin << " " << xmax << endl;
    printf("xmin=%f,xmax=%f\n", xmin, xmax);


    //seprate left and right
    std::vector<cv::Point> lcontour;
    std::vector<cv::Point> rcontour;
    for (auto& tmp : temp) {
        //cout << tmp.first << " " << tmp.second << endl;
        cv::circle(debug, tmp.first, 8, cv::Scalar(150, 125, 150), 4);
        if (tmp.second.x < xmin + (xmax - xmin) * 0.2) {
            cv::circle(debug, tmp.first, 8, cv::Scalar(200, 150, 200), 8);
            lcontour.push_back(tmp.first);
        }
        if (tmp.second.x > xmax - (xmax - xmin) * 0.2) {
            cv::circle(debug, tmp.first, 8, cv::Scalar(200, 175, 200), 8);
            rcontour.push_back(tmp.first);
        }
    }

    //get left rect and right rect
    std::vector<cv::Point> lapprox;
    cv::approxPolyDP(lcontour, lapprox, 0.01 * arcLength(lcontour, true), true);
    //cout << "lapprox:" << lapprox << endl;

    std::vector<cv::Point> rapprox;
    cv::approxPolyDP(rcontour, rapprox, 0.01 * arcLength(rcontour, true), true);
    //cout << "rapprox:" << rapprox << endl;


    //left sort by rx
    temp.clear();
    for (auto& lapp : lapprox) {
        int x = (lapp.x - cx);
        int y = (lapp.y - cy);
        cv::Point2f pxpy = cv::Point2f(x * rx + y * ry, x * tx + y * ty);
        temp.push_back(std::make_pair(lapp, pxpy));
    }
    sort(temp.begin(), temp.end(), [](const std::pair<cv::Point, cv::Point2f>& a, const std::pair<cv::Point, cv::Point2f>& b) {
        return a.second.x < b.second.x;
        });
    std::vector<cv::Point> lout;
    for (auto& item : temp) {
        lout.push_back(item.first);
        cv::circle(debug, item.first, 8, cv::Scalar(255, 225, 255), 8);
    }


    //right sort by rx
    temp.clear();
    for (auto& rapp : rapprox) {
        int x = (rapp.x - cx);
        int y = (rapp.y - cy);
        cv::Point2f pxpy = cv::Point2f(x * rx + y * ry, x * tx + y * ty);
        temp.push_back(std::make_pair(rapp, pxpy));
    }
    sort(temp.begin(), temp.end(), [](const std::pair<cv::Point, cv::Point2f>& a, const std::pair<cv::Point, cv::Point2f>& b) {
        return a.second.x > b.second.x;
        });
    std::vector<cv::Point> rout;
    for (auto& item : temp) {
        rout.push_back(item.first);
        cv::circle(debug, item.first, 8, cv::Scalar(255, 255, 255), 8);
    }



    std::vector<cv::Point> out;
    out.push_back(lout[0]);
    out.push_back(lout[1]);
    out.push_back(rout[0]);
    out.push_back(rout[1]);

    //cout << "out:" << out << endl;
    return out;
}


std::vector<cv::Point2f> from_red_and_green(std::vector<cv::Point>& r_arr, std::vector<cv::Point>& g_arr) {

    cv::Point2f cr = cv::Point2f((r_arr[0].x + r_arr[1].x + r_arr[2].x + r_arr[3].x) / 4, (r_arr[0].y + r_arr[1].y + r_arr[2].y + r_arr[3].y) / 4);
    cv::Point2f cg = cv::Point2f((g_arr[0].x + g_arr[1].x + g_arr[2].x + g_arr[3].x) / 4, (g_arr[0].y + g_arr[1].y + g_arr[2].y + g_arr[3].y) / 4);
    cv::Point2f gtor = cr - cg;
    //print(gtor)
    printf("gtor:%lf,%lf\n", gtor.x, gtor.y);


    //前一步已经提供按照各自x轴排序好的顶点（y未排序）
    //需要在r0,r1中取一个，r2,r3中取一个，g0g1中取一个，g2g3中取一个
    cv::Point2f rv0 = r_arr[0] - r_arr[1];
    cv::Point2f r0;
    if (rv0.x * gtor.x + rv0.y * gtor.y > 0)r0 = r_arr[0];
    else r0 = r_arr[1];

    cv::Point2f rv1 = r_arr[2] - r_arr[3];
    cv::Point2f r1;
    if (rv1.x * gtor.x + rv1.y * gtor.y > 0)r1 = r_arr[2];
    else r1 = r_arr[3];

    cv::Point2f gv0 = g_arr[0] - g_arr[1];
    cv::Point2f g0;
    if (gv0.x * gtor.x + gv0.y * gtor.y < 0)g0 = g_arr[0];
    else g0 = g_arr[1];

    cv::Point2f gv1 = g_arr[2] - g_arr[3];
    cv::Point2f g1;
    if (gv1.x * gtor.x + gv1.y * gtor.y < 0)g1 = g_arr[2];
    else g1 = g_arr[3];


    //现在只剩4个点，并且前两个是红的，后两个是绿的，确定方向
    cv::Point2f right = cv::Point(gtor.y, -gtor.x);
    cv::Point2f r0r1 = r1 - r0;
    if (right.x * r0r1.x + right.y * r0r1.y > 0) {
        r0r1 = r0;
        r0 = r1;
        r1 = r0r1;
    }
    cv::Point2f g0g1 = g1 - g0;
    if (right.x * g0g1.x + right.y * g0g1.y > 0) {
        g0g1 = g0;
        g0 = g1;
        g1 = g0g1;
    }

    std::vector<cv::Point2f> out;
    out.push_back(r0);
    out.push_back(r1);
    out.push_back(g0);
    out.push_back(g1);

    //cout << "out:" << out << endl;
    return out;
}


std::vector<cv::Point2f> find_ruler_rough(cv::Mat frame) {
    cv::Mat dbgimg = frame.clone();

    int height = frame.rows;
    int width = frame.cols;
    printf("find_ruler_rough: wh=%d,%d\n", width, height);

    std::vector<cv::Mat> bgr_planes;
    split(frame, bgr_planes);
    cv::Mat b = bgr_planes[0], g = bgr_planes[1], r = bgr_planes[2];
    printf("find_ruler_rough: split rgb done\n");




    //blue
    cv::Mat blue_mask = (b - g > 38) & (b - r > 38);
    /*
        cv::Mat blue_out;
        frame.copyTo(blue_out, blue_mask);
        //cv::imshow("blue_out", resizeimage(blue_out));
        printf("find_ruler_rough: blue mask done");

        cv::Mat blue_gray;
        cvtColor(blue_out, blue_gray, cv::COLOR_BGR2GRAY);
        //cv::imshow("blue_gray", resizeimage(blue_gray));
        printf("find_ruler_rough: blue gray done");
    */
    cv::Mat blue_gray;
    b.copyTo(blue_gray, blue_mask);




    //green
    cv::Mat green_mask = (g - r > 18) & (g - b > 18);
    /*
        cv::Mat green_out;
        frame.copyTo(green_out, green_mask);
        //cv::imshow("green_out", resizeimage(green_out));
        printf("find_ruler_rough: green mask done");

        cv::Mat green_gray;
        cvtColor(green_out, green_gray, cv::COLOR_BGR2GRAY);
        //cv::imshow("green_gray", resizeimage(green_gray));
        printf("find_ruler_rough: green gray done");
    */
    cv::Mat green_gray;
    b.copyTo(green_gray, green_mask);



    //red
    cv::Mat red_mask = (r - g > 40) & (r - b > 40);
    /*
        cv::Mat red_out;
        frame.copyTo(red_out, red_mask);
        //cv::imshow("red_out", resizeimage(red_out));
        printf("find_ruler_rough: red mask done");

        cv::Mat red_gray;
        cvtColor(red_out, red_gray, cv::COLOR_BGR2GRAY);
        //cv::imshow("red_gray", resizeimage(red_gray));
        printf("find_ruler_rough: red gray done");
    */
    cv::Mat red_gray;
    b.copyTo(red_gray, red_mask);




    //green
    std::vector<std::vector<cv::Point>> green_contours;
    cv::Mat green_hierarchy;
    findContours(green_gray, green_contours, green_hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (green_contours.size() == 0) {
        printf("find_ruler_rough: green_contours.size=0\n");
        std::vector<cv::Point2f> emptyVector;
        return emptyVector;
    }
    std::sort(green_contours.begin(), green_contours.end(), compareContourArea);

    /*
    todo: find suitable green strip
    */
    int greenbest = 0;
    cv::drawContours(dbgimg, green_contours, static_cast<int>(greenbest), cv::Scalar(0, 255, 0), 1, cv::LINE_8, green_hierarchy, 0);
    std::vector<cv::Point> greenshape = myapprox(dbgimg, green_contours[0]);

    cv::circle(dbgimg, greenshape[0], 8, cv::Scalar(0, 255, 0), 4);
    cv::circle(dbgimg, greenshape[1], 8, cv::Scalar(0, 255, 0), 4);
    cv::circle(dbgimg, greenshape[2], 8, cv::Scalar(0, 255, 0), 4);
    cv::circle(dbgimg, greenshape[3], 8, cv::Scalar(0, 255, 0), 4);
    cv::putText(dbgimg, "0", cv::Point(greenshape[0].x, greenshape[0].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 0), 4);
    cv::putText(dbgimg, "1", cv::Point(greenshape[1].x, greenshape[1].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 0), 4);
    cv::putText(dbgimg, "2", cv::Point(greenshape[2].x, greenshape[2].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 0), 4);
    cv::putText(dbgimg, "3", cv::Point(greenshape[3].x, greenshape[3].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 0), 4);



    //red
    std::vector<std::vector<cv::Point>> red_contours;
    cv::Mat red_hierarchy;
    findContours(red_gray, red_contours, red_hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (red_contours.size() == 0) {
        printf("find_ruler_rough: red_contours.size=0\n");
        std::vector<cv::Point2f> emptyVector;
        return emptyVector;
    }
    std::sort(red_contours.begin(), red_contours.end(), compareContourArea);
    /*
    todo: find suitable red strip
    */
    int redbest = 0;
    cv::drawContours(dbgimg, red_contours, static_cast<int>(redbest), cv::Scalar(0, 0, 255), 1, cv::LINE_8, red_hierarchy, 0);
    std::vector<cv::Point> redshape = myapprox(dbgimg, red_contours[0]);

    cv::circle(dbgimg, redshape[0], 8, cv::Scalar(0, 0, 255), 4);
    cv::circle(dbgimg, redshape[1], 8, cv::Scalar(0, 0, 255), 4);
    cv::circle(dbgimg, redshape[2], 8, cv::Scalar(0, 0, 255), 4);
    cv::circle(dbgimg, redshape[3], 8, cv::Scalar(0, 0, 255), 4);
    cv::putText(dbgimg, "0", cv::Point(redshape[0].x, redshape[0].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 255), 4);
    cv::putText(dbgimg, "1", cv::Point(redshape[1].x, redshape[1].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 255), 4);
    cv::putText(dbgimg, "2", cv::Point(redshape[2].x, redshape[2].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 255), 4);
    cv::putText(dbgimg, "3", cv::Point(redshape[3].x, redshape[3].y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 255), 4);



    //ruler
    std::vector<cv::Point2f> rulerpoint = from_red_and_green(redshape, greenshape);



    //debug
    //cv::imwrite("/sdcard/DCIM/debug00_findrulerrough.jpg", dbgimg);
    myshow("/sdcard/DCIM/debug00_findrulerrough.jpg", dbgimg);



    return rulerpoint;
}

std::pair<cv::Point, cv::Point> find_closest_points(const std::vector<cv::Point>& contours1, const std::vector<cv::Point>& contours2) {
    double min_dist = std::numeric_limits<double>::infinity();
    std::pair<cv::Point, cv::Point> closest_points;

    for (size_t i = 0; i < contours1.size(); ++i) {
        for (size_t j = 0; j < contours2.size(); ++j) {
            // Take the first point of each contour
            cv::Point point1 = contours1[i];
            cv::Point point2 = contours2[j];

            // Calculate Euclidean distance
            double dist = cv::norm(point1 - point2);

            // Update minimum distance and corresponding points
            if (dist < min_dist) {
                min_dist = dist;
                closest_points = std::make_pair(point1, point2);
            }
        }
    }

    return closest_points;
}
std::pair<cv::Mat, cv::Point> bettercorner(cv::Mat img, cv::Point2f xy, const char* color) {
    int srcw = 40;
    int srch = 40;
    int dstw = 100;
    int dsth = 100;

    cv::Rect roiRect(round(xy.x) - srcw/2, round(xy.y) - srch/2, srcw, srch);
    cv::Mat roi = img(roiRect).clone();
    //std::cout << roi.size() << std::endl;
    cv::resize(roi, roi, cv::Size(dstw, dsth));
    // cv::imshow(str + ".roi", roi);

    std::vector<cv::Mat> channels;
    cv::split(roi, channels);
    cv::Mat b = channels[0];
    cv::Mat g = channels[1];
    cv::Mat r = channels[2];
    // cv::imshow(str + ".b", b);
    // cv::imshow(str + ".g", g);
    // cv::imshow(str + ".r", r);

    std::vector<std::vector<cv::Point>> thecontour;

    if (color[0] == 'b' || color[1] == 'b') {
        cv::Mat mask = (b - g > 38) & (b - r > 38);
        cv::Mat blue_only = cv::Mat::zeros(b.size(), CV_8UC1);
        b.copyTo(blue_only, mask);
        // cv::imshow(str + ".blue_only", blue_only);

        std::vector<std::vector<cv::Point>> bcontours;
        cv::findContours(blue_only, bcontours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::sort(bcontours.begin(), bcontours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
            return cv::contourArea(c1) > cv::contourArea(c2);
            });
        cv::drawContours(roi, bcontours, -1, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);

        if (!bcontours.empty()) {
            //std::cout << bcontours[0] << std::endl;
            printf("find b\n");
            thecontour.push_back(bcontours[0]);
        }
    }

    if (color[0] == 'g' || color[1] == 'g') {
        cv::Mat mask = (g - b > 15) & (g - r > 15);
        cv::Mat green_only = cv::Mat::zeros(g.size(), CV_8UC1);
        g.copyTo(green_only, mask);
        // cv::imshow(str + ".green_only", green_only);

        std::vector<std::vector<cv::Point>> gcontours;
        cv::findContours(green_only, gcontours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::sort(gcontours.begin(), gcontours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
            return cv::contourArea(c1) > cv::contourArea(c2);
            });
        cv::drawContours(roi, gcontours, -1, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);

        if (!gcontours.empty()) {
            //std::cout << gcontours[0] << std::endl;
            printf("find g\n");
            thecontour.push_back(gcontours[0]);
        }
    }

    if (color[0] == 'r' || color[1] == 'r') {
        cv::Mat mask = (r - b > 25) & (r - g > 25);
        cv::Mat red_only = cv::Mat::zeros(r.size(), CV_8UC1);
        r.copyTo(red_only, mask);
        // cv::imshow(str + ".red_only", red_only);

        std::vector<std::vector<cv::Point>> rcontours;
        cv::findContours(red_only, rcontours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::sort(rcontours.begin(), rcontours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
            return cv::contourArea(c1) > cv::contourArea(c2);
            });
        cv::drawContours(roi, rcontours, -1, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

        if (!rcontours.empty()) {
            //std::cout << rcontours[0] << std::endl;
            printf("find r\n");
            thecontour.push_back(rcontours[0]);
        }
    }

    // Assuming 'w' stands for white
    if (color[0] == 'w' || color[1] == 'w') {
        cv::Mat mask = (r > 140) & (g > 140) & (b > 130);
        cv::Mat white_only = cv::Mat::zeros(r.size(), CV_8UC1);
        r.copyTo(white_only, mask);
        // cv::imshow(str + ".white_only", white_only);

        std::vector<std::vector<cv::Point>> wcontours;
        cv::findContours(white_only, wcontours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::sort(wcontours.begin(), wcontours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
            return cv::contourArea(c1) > cv::contourArea(c2);
            });
        cv::drawContours(roi, wcontours, -1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

        if (!wcontours.empty()) {
            //std::cout << wcontours[0] << std::endl;
            printf("find w\n");
            thecontour.push_back(wcontours[0]);
        }
    }

    if (thecontour.size() < 2) {
        printf("thecontour.size()<2\n");
        return std::make_pair(roi, xy);
    }

    std::pair<cv::Point, cv::Point> closest = find_closest_points(thecontour[0], thecontour[1]);
    printf("first=%d,%d\n", closest.first.x, closest.first.y);
    printf("second=%d,%d\n", closest.second.x, closest.second.y);
    float partx = (closest.first.x + closest.second.x) / 2;
    float party = (closest.first.y + closest.second.y) / 2;
    printf("closest=%f,%f\n", partx, party);

    cv::circle(roi, cv::Point(closest.first.x, closest.first.y), 3, cv::Scalar(255, 100, 255), 2);
    cv::circle(roi, cv::Point(closest.second.x, closest.second.y), 4, cv::Scalar(255, 100, 255), 2);
    cv::circle(roi, cv::Point(partx, party), 3, cv::Scalar(255, 255, 255), 2);
    // cv::imshow(str + ".final", roi);

    float fixx = xy.x + (partx*1.0/dstw - 0.5) * srcw;
    float fixy = xy.y + (party*1.0/dsth - 0.5) * srch;
    cv::Point2f fixed = cv::Point2f(fixx, fixy);
    printf("inxy=%f,%f\n", xy.x, xy.y);
    printf("fixed=%f,%f\n", fixx, fixy);
    return std::make_pair(roi, fixed);
}
std::vector<cv::Point2f> bettercorner_all(cv::Mat img, cv::Point2f tltrblbr[4]) {
    cv::Mat dbgimg = cv::Mat::zeros(512, 512, CV_8UC3);
    std::pair<cv::Mat, cv::Point> result;

    cv::Mat imgtl;
    cv::Point2f fixtl;
    result = bettercorner(img, tltrblbr[0], "rb");
    imgtl = result.first;
    fixtl = result.second;

    cv::Mat imgtr;
    cv::Point2f fixtr;
    result = bettercorner(img, tltrblbr[1], "rg");
    imgtr = result.first;
    fixtr = result.second;

    cv::Mat imgbl;
    cv::Point2f fixbl;
    result = bettercorner(img, tltrblbr[2], "gr");
    imgbl = result.first;
    fixbl = result.second;

    cv::Mat imgbr;
    cv::Point2f fixbr;
    result = bettercorner(img, tltrblbr[3], "gw");
    imgbr = result.first;
    fixbr = result.second;


    imgtl.copyTo(dbgimg(cv::Rect(128 - imgtl.cols / 2, 128 - imgtl.rows / 2, imgtl.cols, imgtl.rows)));
    imgtr.copyTo(dbgimg(cv::Rect(384 - imgtl.cols / 2, 128 - imgtl.rows / 2, imgtl.cols, imgtl.rows)));
    imgbl.copyTo(dbgimg(cv::Rect(128 - imgtl.cols / 2, 384 - imgtl.rows / 2, imgtl.cols, imgtl.rows)));
    imgbr.copyTo(dbgimg(cv::Rect(384 - imgtl.cols / 2, 384 - imgtl.rows / 2, imgtl.cols, imgtl.rows)));
    //cv::imwrite("/sdcard/DCIM/debug010_corner.jpg", dbgimg);
    myshow("/sdcard/DCIM/debug010_corner.jpg", dbgimg);

    std::vector<cv::Point2f> out;
    out.push_back(fixtl);
    out.push_back(fixtr);
    out.push_back(fixbl);
    out.push_back(fixbr);
    return out;
}

std::vector<cv::Point2f> find_ruler_exact(cv::Mat origin, std::vector<cv::Point2f> ruler_rough) {
    int wrap1_sx = 1880;
    int wrap1_sy = 300;
    int wrap1_sw = wrap1_sx + 60;
    int wrap1_sh = wrap1_sy + 60;

    // 定义原始和目标四个顶点的坐标
    cv::Point2f wrap1_psrc[4] = {
        cv::Point2f(ruler_rough[0].x, ruler_rough[0].y),
        cv::Point2f(ruler_rough[1].x, ruler_rough[1].y),
        cv::Point2f(ruler_rough[2].x, ruler_rough[2].y),
        cv::Point2f(ruler_rough[3].x, ruler_rough[3].y)
    };
    cv::Point2f wrap1_pdst[4] = {
        cv::Point2f(wrap1_sw / 2 - wrap1_sx / 2, wrap1_sh / 2 - wrap1_sy / 2),
        cv::Point2f(wrap1_sw / 2 + wrap1_sx / 2, wrap1_sh / 2 - wrap1_sy / 2),
        cv::Point2f(wrap1_sw / 2 - wrap1_sx / 2, wrap1_sh / 2 + wrap1_sy / 2),
        cv::Point2f(wrap1_sw / 2 + wrap1_sx / 2, wrap1_sh / 2 + wrap1_sy / 2)
    };

    // 计算透视变换矩阵
    cv::Mat wrap1_M = cv::getPerspectiveTransform(wrap1_psrc, wrap1_pdst);
    cv::Mat wrap1_invM = cv::getPerspectiveTransform(wrap1_pdst, wrap1_psrc);

    // 进行透视变换
    cv::Mat wrap1_img;
    cv::warpPerspective(origin, wrap1_img, wrap1_M, cv::Size(wrap1_sw, wrap1_sh));
    myshow("wrap1_img", wrap1_img);

    // 修正4个顶点
    std::vector<cv::Point2f> corner_better_in_wrapimg = bettercorner_all(wrap1_img, wrap1_pdst);






    std::vector<cv::Point2f> corner_better_in_origin;
    corner_better_in_origin.push_back(mat33_mul_point(wrap1_invM, corner_better_in_wrapimg[0]));
    corner_better_in_origin.push_back(mat33_mul_point(wrap1_invM, corner_better_in_wrapimg[1]));
    corner_better_in_origin.push_back(mat33_mul_point(wrap1_invM, corner_better_in_wrapimg[2]));
    corner_better_in_origin.push_back(mat33_mul_point(wrap1_invM, corner_better_in_wrapimg[3]));
    return corner_better_in_origin;
}

cv::Mat goodimage(cv::Mat origin, std::vector<cv::Point2f> corner){
    int wrap2_sx = 1880;
    int wrap2_sy = 300;
    int wrap2_sw = wrap2_sx + 20;
    int wrap2_sh = wrap2_sy + 20;

    // 定义原始和目标四个顶点的坐标
    cv::Point2f wrap2_psrc[4] = {
        cv::Point2f(corner[0].x, corner[0].y),
        cv::Point2f(corner[1].x, corner[1].y),
        cv::Point2f(corner[2].x, corner[2].y),
        cv::Point2f(corner[3].x, corner[3].y)
    };
    cv::Point2f wrap2_pdst[4] = {
        cv::Point2f(wrap2_sw / 2 - wrap2_sx / 2, wrap2_sh / 2 - wrap2_sy / 2),
        cv::Point2f(wrap2_sw / 2 + wrap2_sx / 2, wrap2_sh / 2 - wrap2_sy / 2),
        cv::Point2f(wrap2_sw / 2 - wrap2_sx / 2, wrap2_sh / 2 + wrap2_sy / 2),
        cv::Point2f(wrap2_sw / 2 + wrap2_sx / 2, wrap2_sh / 2 + wrap2_sy / 2)
    };

    // 计算透视变换矩阵
    cv::Mat wrap2_M = cv::getPerspectiveTransform(wrap2_psrc, wrap2_pdst);
    cv::Mat wrap2_invM = cv::getPerspectiveTransform(wrap2_pdst, wrap2_psrc);

    // 进行透视变换
    cv::Mat dbgimg;
    cv::warpPerspective(origin, dbgimg, wrap2_M, cv::Size(wrap2_sw, wrap2_sh));

    // 显示结果图像（假设myshow是一个展示图像的自定义函数）
    //cv::imwrite("/sdcard/DCIM/debug011_findrulerexact.jpg", dbgimg);
    myshow("/sdcard/DCIM/debug011_findrulerexact.jpg", dbgimg);
    return dbgimg;
}


std::pair<std::vector<cv::Point2f>, std::vector<int>> find_text_rough(cv::Mat rectify)
{
    int wrap1_sw = 2048;
    int wrap1_sh = 512;
    int wrap1_sx = rectify.cols;
    int wrap1_sy = rectify.rows;

    // 定义原始和目标四个顶点的坐标
    cv::Point2f wrap1_psrc[4] = {
        cv::Point2f(       0,        0),
        cv::Point2f(wrap1_sx,        0),
        cv::Point2f(       0, wrap1_sy),
        cv::Point2f(wrap1_sx, wrap1_sy)
    };
    cv::Point2f wrap1_pdst[4] = {
        cv::Point2f(wrap1_sw / 2 - wrap1_sx / 2, wrap1_sh / 2 - wrap1_sy / 2),
        cv::Point2f(wrap1_sw / 2 + wrap1_sx / 2, wrap1_sh / 2 - wrap1_sy / 2),
        cv::Point2f(wrap1_sw / 2 - wrap1_sx / 2, wrap1_sh / 2 + wrap1_sy / 2),
        cv::Point2f(wrap1_sw / 2 + wrap1_sx / 2, wrap1_sh / 2 + wrap1_sy / 2)
    };

    // 计算透视变换矩阵
    cv::Mat wrap1_M = cv::getPerspectiveTransform(wrap1_psrc, wrap1_pdst);
    cv::Mat wrap1_invM = cv::getPerspectiveTransform(wrap1_pdst, wrap1_psrc);

    // 进行透视变换
    cv::Mat newimg;
    cv::warpPerspective(rectify, newimg, wrap1_M, cv::Size(wrap1_sw, wrap1_sh));

    int newimgw = newimg.cols;
    int newimgh = newimg.rows;
    printf("find_text_rough: newimg w=%d,h=%d\n", newimgw, newimgh);

    myshow("newimg", newimg);

    std::vector<cv::Point2f> out1;
    std::vector<int> out2;
    return std::make_pair(out1, out2);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "usage: .\a.exe xxx.jpg";
        return 0;
    }

    std::cout << argv[1];
    cv::Mat origin = cv::imread(argv[1], cv::IMREAD_COLOR);
    
    if (origin.empty()) {
        std::cout << "Could not open or find the image!\n";
        return -1;
    }
    std::cout << std::endl;

    myshow("origin", origin);

    //step00: find ruler rough
    std::vector<cv::Point2f> ruler_rough = find_ruler_rough(origin);
    printf("ruler_rough: %lf,%lf,%lf,%lf,%lf,%lf,%lf\n", ruler_rough[0].x, ruler_rough[0].y, ruler_rough[1].x, ruler_rough[1].y, ruler_rough[2].x, ruler_rough[2].y, ruler_rough[3].x, ruler_rough[3].y);


    //step01: find ruler exact
    std::vector<cv::Point2f> ruler_exact = find_ruler_exact(origin, ruler_rough);
    printf("ruler_exact: %lf,%lf,%lf,%lf,%lf,%lf,%lf\n", ruler_exact[0].x, ruler_exact[0].y, ruler_exact[1].x, ruler_exact[1].y, ruler_exact[2].x, ruler_exact[2].y, ruler_exact[3].x, ruler_exact[3].y);



    //step1: rectify
    //step1: rectify
    int wrap2_sx = 1880;
    int wrap2_sy = 300;
    int wrap2_sw = wrap2_sx + 20;
    int wrap2_sh = wrap2_sy + 20;

    // 定义原始和目标四个顶点的坐标
    cv::Point2f wrap2_psrc[4] = {
        cv::Point2f(ruler_exact[0].x, ruler_exact[0].y),
        cv::Point2f(ruler_exact[1].x, ruler_exact[1].y),
        cv::Point2f(ruler_exact[2].x, ruler_exact[2].y),
        cv::Point2f(ruler_exact[3].x, ruler_exact[3].y)
    };
    cv::Point2f wrap2_pdst[4] = {
        cv::Point2f(wrap2_sw / 2 - wrap2_sx / 2, wrap2_sh / 2 - wrap2_sy / 2),
        cv::Point2f(wrap2_sw / 2 + wrap2_sx / 2, wrap2_sh / 2 - wrap2_sy / 2),
        cv::Point2f(wrap2_sw / 2 - wrap2_sx / 2, wrap2_sh / 2 + wrap2_sy / 2),
        cv::Point2f(wrap2_sw / 2 + wrap2_sx / 2, wrap2_sh / 2 + wrap2_sy / 2)
    };

    // 计算透视变换矩阵
    cv::Mat wrap2_M = cv::getPerspectiveTransform(wrap2_psrc, wrap2_pdst);
    cv::Mat wrap2_invM = cv::getPerspectiveTransform(wrap2_pdst, wrap2_psrc);

    // 进行透视变换
    cv::Mat rectify;
    cv::warpPerspective(origin, rectify, wrap2_M, cv::Size(wrap2_sw, wrap2_sh));

    // 显示结果图像（假设myshow是一个展示图像的自定义函数）
    //cv::imwrite("/sdcard/DCIM/debug011_findrulerexact.jpg", rectify);
    myshow("/sdcard/DCIM/debug011_findrulerexact.jpg", rectify);



    //step20: find text rough
    std::pair<std::vector<cv::Point2f>, std::vector<int>> text_rough_result = find_text_rough(rectify);

    //step21: find text exact




    std::vector<cv::Point2f> point_in_origin;
    point_in_origin.push_back(mat33_mul_point(wrap2_invM, wrap2_pdst[0]));
    point_in_origin.push_back(mat33_mul_point(wrap2_invM, wrap2_pdst[1]));
    point_in_origin.push_back(mat33_mul_point(wrap2_invM, wrap2_pdst[2]));
    point_in_origin.push_back(mat33_mul_point(wrap2_invM, wrap2_pdst[3]));
    printf("point_in_origin: %lf,%lf,%lf,%lf,%lf,%lf,%lf\n", point_in_origin[0].x, point_in_origin[0].y, point_in_origin[1].x, point_in_origin[1].y, point_in_origin[2].x, point_in_origin[2].y, point_in_origin[3].x, point_in_origin[3].y);



    cv::waitKey(0);
    return 0;
}