// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

#include <fstream>              // File IO
#include <iostream>             // Terminal IO
#include <sstream>              // Stringstreams
#include <thread>               // threads

// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// waiting 10 seconds vs 5 doesn't appear to help
unsigned int timeout_ms = 30000;
// base path to store images
// "/media/data"

int write_file(rs2::video_frame vf);

// helper function to wait for enter
void wait_for_enter() 
{
  std::cin.get();
}

// helper funtion to wait for choice input
void wait_for_key() 
{
  char choice;
  do {
  std::cout << "Enter 'q' to quit: ";
  std::cin  >> choice;
  std::cin.ignore(100, '\n');
  } while ( choice != 'q');
}

// Helper function for writing metadata to disk as a csv file
// void metadata_to_csv(const rs2::frame& frm, const std::string& filename);
void metadata_to_csv(const rs2::frame& frm, const std::string& filename)
{
    std::ofstream csv;

    csv.open(filename);

    //    std::cout << "Writing metadata to " << filename << endl;
    csv << "Stream," << rs2_stream_to_string(frm.get_profile().stream_type()) << "\nMetadata Attribute,Value\n";

    // Record all the available metadata attributes
    for (size_t i = 0; i < RS2_FRAME_METADATA_COUNT; i++)
    {
        if (frm.supports_frame_metadata((rs2_frame_metadata_value)i))
        {
            csv << rs2_frame_metadata_to_string((rs2_frame_metadata_value)i) << ","
                << frm.get_frame_metadata((rs2_frame_metadata_value)i) << "\n";
        }
    }

    csv.close();
}

rs2::pipeline get_pipe()
{
    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    return pipe;
}

// Start device and prime the pipeline
int warmup(rs2::pipeline pipe)
{
    // Start streaming with default recommended configuration
    pipe.start();

    // Capture 30 frames to give autoexposure, etc. a chance to settle
    for (auto i = 0; i < 30; ++i) pipe.wait_for_frames(timeout_ms);
    return 0;
}

// Capture frames to disk
int capture(rs2::pipeline pipe)
{
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // TODO add loop to continue capturing images until user input

    // Wait for the next set of frames from the camera. Now that autoexposure, etc.
    // has settled, we will write these to disk
    for (auto&& frame : pipe.wait_for_frames())
    {
        // We can only save video frames as pngs, so we skip the rest
        if (auto vf = frame.as<rs2::video_frame>())
        {
            auto stream = frame.get_profile().stream_type();
            // Use the colorizer to get an rgb image for the depth stream
            if (vf.is<rs2::depth_frame>()) vf = color_map(frame);

            // TODO consider moving writing frame to disk to a thread
            write_file(vf);
        }
    }
    return 0;
}

// Write images and meta data to disk
int write_file(rs2::video_frame vf) {
    std::stringstream png_file;
    png_file << "/media/data/rs-save-to-disk-output-" << vf.get_profile().stream_name() << ".png";
    stbi_write_png(png_file.str().c_str(), vf.get_width(), vf.get_height(),
                   vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());
    std::cout << "Saved " << png_file.str() << std::endl;

    // Record per-frame metadata for UVC streams
    std::stringstream csv_file;
    csv_file << "/media/data/rs-save-to-disk-output-" << vf.get_profile().stream_name()
             << "-metadata.csv";
    metadata_to_csv(vf, csv_file.str());
}
//end thread funtion

// main program
int main(int argc, char * argv[]) try
{
    // Positive or Negative Example? (p/n)
    // Custom Depth Stream Configs
    // Custom RGB Stream Configs

    // define a new pipe
    rs2::pipeline pipe;
    pipe = get_pipe();

    // Get the camera tape rolling
    warmup(pipe);

    // Press key to Start
    std::cout<<"Press [Enter] to start . . .";
    wait_for_enter();

    // start capturing frames
    capture(pipe);

    // make sure the pipe stops before exiting
    pipe.stop();
    std::cout << "End" << std::endl;
    return EXIT_SUCCESS;
}
catch(const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch(const std::exception & e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

