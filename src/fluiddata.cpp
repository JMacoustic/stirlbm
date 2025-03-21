#include "fluiddata.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "json.hpp"

std::vector<Material> readCSV(const std::string& filename) {
    std::vector<Material> materials;
    std::ifstream file;
    file.open(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "error opening file: " << filename << std::endl;
    }

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Material mat;
        std::string temp;
        std::getline(ss, mat.name, ',');
        std::getline(ss, temp, ','); mat.density = std::stof(temp);
        std::getline(ss, temp, ','); mat.surface_tension = std::stof(temp);
        std::getline(ss, temp, ','); mat.dynamic_viscosity = std::stof(temp);
        std::getline(ss, temp, ','); mat.kinematic_viscosity = std::stof(temp);
    
        materials.push_back(mat);
    }

    file.close();
    return materials;
}

Material selectMaterial(const std::vector<Material>& materials, const std::string& name) {
    for (const auto& mat : materials) {
        if (mat.name == name) {
            return mat;
        }
    }
    std::cerr << "Material not found!" << std::endl;
    return { "", 0, 0, 0, 0 };
}

using json = nlohmann::json;

void exportConfig(const Material& material, const int& rpm, const std::string& option, const std::string& exportpath, const std::string& folder_str) {
    /*
    * export options: json, txt, csv
    */
    if (option == "json") {
        json config;
        config["density"] = material.density;
        config["surface_tension"] = material.surface_tension;
        config["dynamic_viscosity"] = material.dynamic_viscosity;
        config["kinematic_viscosity"] = material.kinematic_viscosity;
        config["RPM"] = rpm;

        std::ofstream configuration(exportpath + "config_"+ folder_str + ".json");
        if (configuration.is_open()) {
            configuration << config.dump(4); // Pretty-print JSON with 4-space indentation
            configuration.close();
        }
        else {
            std::cerr << "Error: Could not open config.json for writing!\n";
        }
    }
    else if (option == "txt") {
        std::ofstream configuration(exportpath + "config_" + folder_str + ".txt");
        if (configuration.is_open()) {
            configuration << "density: " << material.density << "\n";
            configuration << "surface_tension: " << material.surface_tension << "\n";
            configuration << "dynamic_viscosity: " << material.dynamic_viscosity << "\n";
            configuration << "kinematic_viscosity: " << material.kinematic_viscosity << "\n";
            configuration << "rpm: " << rpm;
            configuration.close();
        }
        else std::cerr << "oops..something went wrong while exporting configuration\n";
    }
    else if (option == "csv") {
        std::ofstream configuration(exportpath + "config_" + folder_str + ".csv");
        if (configuration.is_open()) {
            configuration << "density," << material.density << "\n";
            configuration << "surface_tension," << material.surface_tension << "\n";
            configuration << "dynamic_viscosity," << material.dynamic_viscosity << "\n";
            configuration << "kinematic_viscosity," << material.kinematic_viscosity << "\n";
            configuration << "rpm," << rpm;
            configuration.close();
        }
        else std::cerr << "oops..something went wrong while exporting configuration\n";
    }
    else std::cerr << "invalid export filetype";
}


//#ifdef _WIN32
//#define FFMPEG_PATH "ffmpeg/windows/bin/ffmpeg.exe"
//#else 
//#define FFMPEG_PATH "ffmpeg/linux/ffmpeg
//#endif
//
//void exportVideo(const std::string& exportpath) { // this function is still on develpment
//    std::string command = std::string(FFMPEG_PATH) +
//        " -framerate 10 -i \"" + exportpath + "image_%d.png\" -c:v libx264 -pix_fmt yuv420p output.mp4";
//
//    int result = std::system(command.c_str());
//
//    if (result != 0) {
//        std::cerr << "FFmpeg failed to execute!" << std::endl;
//    }
//
//    std::cout << "Video successfully created: output.mp4" << std::endl;
//}
