
// main.cpp
// Build: mkdir build && cd build && cmake .. && make
// Or: g++ main.cpp -o seam_carve `pkg-config --cflags --libs opencv4` -O2

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <limits>
#include <cstring>

using namespace cv;
using namespace std; // only for cout/cerr/string

// NOTE: This implementation avoids STL containers for image/state storage.
// All large arrays use new[]/delete[].

// Compute dual-gradient energy (rows*cols sized array)
void computeEnergy(const Mat &img, float *energy) {
    int rows = img.rows, cols = img.cols;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int left = (c == 0) ? 0 : c - 1;
            int right = (c == cols - 1) ? cols - 1 : c + 1;
            int up = (r == 0) ? 0 : r - 1;
            int down = (r == rows - 1) ? rows - 1 : r + 1;

            Vec3b pc = img.at<Vec3b>(r, c);
            Vec3b pl = img.at<Vec3b>(r, left);
            Vec3b pr = img.at<Vec3b>(r, right);
            Vec3b pu = img.at<Vec3b>(up, c);
            Vec3b pd = img.at<Vec3b>(down, c);

            float dx = 0.0f, dy = 0.0f;
            for (int ch = 0; ch < 3; ++ch) {
                float xv = float(pr[ch]) - float(pl[ch]);
                float yv = float(pd[ch]) - float(pu[ch]);
                dx += xv * xv;
                dy += yv * yv;
            }
            energy[r * cols + c] = sqrtf(dx + dy);
        }
    }
}

// Find vertical seam (rows length array allocated by caller)
void findVerticalSeam(const float *energy, int rows, int cols, int *seam) {
    // cumulative energy
    float *M = new float[rows * cols];
    int *back = new int[rows * cols];

    // first row
    for (int c = 0; c < cols; ++c) {
        M[c] = energy[c];
        back[c] = -1;
    }

    for (int r = 1; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float best = numeric_limits<float>::infinity();
            int bestIdx = c;
            for (int dc = -1; dc <= 1; ++dc) {
                int pc = c + dc;
                if (pc < 0 || pc >= cols) continue;
                float val = M[(r - 1) * cols + pc];
                if (val < best) {
                    best = val;
                    bestIdx = pc;
                }
            }
            M[r * cols + c] = energy[r * cols + c] + best;
            back[r * cols + c] = bestIdx;
        }
    }

    // find min in last row
    float minv = numeric_limits<float>::infinity();
    int minc = 0;
    for (int c = 0; c < cols; ++c) {
        float v = M[(rows - 1) * cols + c];
        if (v < minv) { minv = v; minc = c; }
    }

    int cur = minc;
    for (int r = rows - 1; r >= 0; --r) {
        seam[r] = cur;
        cur = back[r * cols + cur];
        if (cur < 0 && r > 0) cur = max(0, seam[r] - 1);
    }

    delete [] M;
    delete [] back;
}

// Remove vertical seam and return new Mat
Mat removeVerticalSeam(const Mat &img, const int *seam) {
    int rows = img.rows, cols = img.cols;
    Mat out(rows, cols - 1, CV_8UC3);
    for (int r = 0; r < rows; ++r) {
        int sc = seam[r];
        // copy left
        for (int c = 0; c < sc; ++c) out.at<Vec3b>(r, c) = img.at<Vec3b>(r, c);
        // copy right shifted
        for (int c = sc + 1; c < cols; ++c) out.at<Vec3b>(r, c - 1) = img.at<Vec3b>(r, c);
    }
    return out;
}

// Transpose using OpenCV
Mat transposeImage(const Mat &img) {
    Mat t;
    transpose(img, t);
    return t;
}

// Overlay seam on image (draw red pixels on seam) into a copy and return it
Mat overlaySeam(const Mat &img, const int *seam) {
    Mat out = img.clone();
    int rows = img.rows;
    for (int r = 0; r < rows; ++r) {
        int c = seam[r];
        // draw a vertical red dot and neighbors for visibility
        for (int dc = -1; dc <= 1; ++dc) {
            int cc = c + dc;
            if (cc >= 0 && cc < img.cols) out.at<Vec3b>(r, cc) = Vec3b(0, 0, 255);
        }
    }
    return out;
}

// Carve one vertical seam with options to visualize and/or save overlay image
Mat carveOneVertical(const Mat &img, bool visualize, bool saveOverlay, const char *overlayPath) {
    int rows = img.rows, cols = img.cols;
    float *energy = new float[rows * cols];
    computeEnergy(img, energy);
    int *seam = new int[rows];
    findVerticalSeam(energy, rows, cols, seam);

    if (visualize) {
        Mat overlay = overlaySeam(img, seam);
        imshow("Seam", overlay);
        waitKey(1);
        if (saveOverlay && overlayPath) {
            imwrite(overlayPath, overlay);
        }
    }

    Mat out = removeVerticalSeam(img, seam);

    delete [] energy;
    delete [] seam;
    return out;
}

// High-level seam carve to target width and height with interactive controls
int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <input_image> <target_width> <target_height> [--auto] [--save-steps]\n";
        cout << "Example: ./seam_carve input.jpg 400 300 --auto --save-steps\n";
        return -1;
    }

    const char *infile = argv[1];
    int targetW = atoi(argv[2]);
    int targetH = atoi(argv[3]);

    bool autoMode = false;
    bool saveSteps = false;
    for (int i = 4; i < argc; ++i) {
        if (strcmp(argv[i], "--auto") == 0) autoMode = true;
        if (strcmp(argv[i], "--save-steps") == 0) saveSteps = true;
    }

    Mat img = imread(infile, IMREAD_COLOR);
    if (img.empty()) {
        cerr << "Could not open image: " << infile << "\n";
        return -1;
    }

    if (targetW <= 0 || targetH <= 0 || targetW > img.cols || targetH > img.rows) {
        cerr << "Invalid target dimensions. This program only supports reduction.\n";
        return -1;
    }

    cout << "Original: " << img.cols << " x " << img.rows << "\n";
    cout << "Target:   " << targetW << " x " << targetH << "\n";

    bool showSeam = true;
    bool running = true;
    bool stepMode = !autoMode;
    int stepDelayMs = 15; // delay between frames in auto mode
    int saveCounter = 0;

    Mat working = img.clone();
    namedWindow("Result", WINDOW_AUTOSIZE);

    // interactive loop: remove width first, then height via transpose
    while ((working.cols > targetW) || (working.rows > targetH)) {
        // reduce width
        while (working.cols > targetW) {
            if (showSeam) {
                float *energy = new float[working.rows * working.cols];
                computeEnergy(working, energy);
                int *seam = new int[working.rows];
                findVerticalSeam(energy, working.rows, working.cols, seam);
                Mat overlay = overlaySeam(working, seam);
                imshow("Result", overlay);
                delete [] energy;
                delete [] seam;
            } else {
                imshow("Result", working);
            }

            int k = 0;
            if (stepMode) {
                cout << "Press SPACE to remove one seam, 'a' to auto-run, 's' to toggle seam overlay, 'q' to quit\n";
                k = waitKey(0);
            } else {
                k = waitKey(stepDelayMs);
            }

            if (k == 'q' || k == 27) { // ESC or q
                cout << "Quitting...\n";
                running = false; break;
            } else if (k == 'a') {
                stepMode = false; // switch to auto
            } else if (k == 's') {
                showSeam = !showSeam; // toggle overlay
            } else if (k == ' ') {
                // proceed one seam
            }

            if (!running) break;

            // carve one seam (visualize and optionally save overlay)
            char pathbuf[512];
            const char *overlayPath = nullptr;
            if (saveSteps) {
                snprintf(pathbuf, sizeof(pathbuf), "step_%04d.png", saveCounter++);
                overlayPath = pathbuf;
            }
            Mat next = carveOneVertical(working, showSeam, saveSteps, overlayPath);
            working = next;
            if (showSeam) imshow("Result", working);

            if (!stepMode && !running) break;
            if (!stepMode) {
                // allow quitting during auto
                int kk = waitKey(1);
                if (kk == 'q' || kk == 27) { running = false; break; }
            }
        }
        if (!running) break;

        // reduce height via transpose
        while (working.rows > targetH) {
            // transpose, remove column, transpose back
            Mat t = transposeImage(working);

            // show transposed seam (if overlay is on, we show seam in transposed view)
            if (showSeam) {
                float *energy = new float[t.rows * t.cols];
                computeEnergy(t, energy);
                int *seam = new int[t.rows];
                findVerticalSeam(energy, t.rows, t.cols, seam);
                Mat overlayT = overlaySeam(t, seam);
                Mat overlayBack = transposeImage(overlayT);
                imshow("Result", overlayBack);
                delete [] energy; delete [] seam;
            } else {
                imshow("Result", working);
            }

            int k = 0;
            if (stepMode) {
                cout << "(Height) Press SPACE to remove one seam, 'a' to auto-run, 's' to toggle seam overlay, 'q' to quit\n";
                k = waitKey(0);
            } else {
                k = waitKey(stepDelayMs);
            }

            if (k == 'q' || k == 27) { running = false; break; }
            else if (k == 'a') stepMode = false;
            else if (k == 's') showSeam = !showSeam;

            // carve on transposed
            char pathbuf[512];
            const char *overlayPath = nullptr;
            if (saveSteps) {
                snprintf(pathbuf, sizeof(pathbuf), "step_%04d.png", saveCounter++);
                overlayPath = pathbuf;
            }
            Mat t_after = carveOneVertical(t, showSeam, saveSteps, overlayPath);
            working = transposeImage(t_after);
            if (showSeam) imshow("Result", working);

            if (!stepMode && !running) break;
            if (!stepMode) {
                int kk = waitKey(1);
                if (kk == 'q' || kk == 27) { running = false; break; }
            }
        }
        if (!running) break;
    }

    if (!working.empty()) {
        imwrite("seam_output.png", working);
        cout << "Saved final result: seam_output.png\n";
    }

    destroyAllWindows();
    return 0;
}
